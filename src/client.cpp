#include "client.hpp"

#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdexcept>


#define HOST_EXPR "Host"

sockaddr_in* getServerAddressByHost(string domain);
sockaddr_in *getServerAddressByHostAndPort(string host, string port);

string getHostNameFromRequest(string requestBody);

string prepareRemoteRequestBody(string requestBody, string hostName, string resourcePath);
int prepareRemoteRequest(string *requestBody, sockaddr_in **serverAddr);

bool isPortIncluded(string host);
void seperateIpAndPort(string *hostName, string *port);
bool isResourcePathIncluded(string hostName);
void separateHostAndResourcePath(string *hostName, string *resourcePath);

string getErrorPage(string errorMessage){

        return 
        "HTTP/1.1 500 Internal Server Error\r\n\r\n"
        "<html>\r\n"
        "<body>\r\n"
        "<h1>"+ errorMessage + "</h1>\r\n"
        "</body>\r\n"
        "</html>\r\n";
}

sockaddr_in* getServerAddressByHost(string host){
    return getServerAddressByHostAndPort(host, "http");
}

sockaddr_in* getServerAddressByHostAndPort(string host, string port){
    addrinfo hints {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    addrinfo *resolved;
    int res = getaddrinfo(host.c_str(), port.c_str(), &hints, &resolved);
    if(res || !resolved){
        throw runtime_error("can not resolved host");
    }

    freeaddrinfo(resolved);
    return (sockaddr_in *)resolved->ai_addr;
}

string handleInternalError(int socketDescriptor, string errorMessage){
    close(socketDescriptor);
    return getErrorPage(errorMessage);
}

string makeRequest(string requestBody) {
    
    sockaddr_in *target;
    if(prepareRemoteRequest(&requestBody, &target) == -1){
        return getErrorPage(requestBody);
    }

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    int res = connect(clientSocket, (sockaddr*)target, sizeof(*target));
    if(res == -1){
        return handleInternalError(clientSocket, "annot connect with remote server");
    }

    ssize_t bytesWritten = write(clientSocket, requestBody.c_str(), requestBody.length());
    if(bytesWritten != (long int) requestBody.length()){
        handleInternalError(clientSocket, "wrote uncomplete request to remote server");
    }

    char rcvBuf[INT16_MAX];
    ssize_t bytesRead = read(clientSocket, rcvBuf, INT16_MAX);
    if(bytesRead < 0){
        return handleInternalError(clientSocket, "error during reading http response");
    }
    else if(bytesRead == 0){
        return handleInternalError(clientSocket, "received nothing from remote server");
    }

    close(clientSocket);
    return string(rcvBuf, bytesRead);
}

string getHostFromResourcePath(int resourcePathStartPos, string requestBody){
    string hostName = requestBody.substr(resourcePathStartPos, requestBody.find(" ", resourcePathStartPos) - resourcePathStartPos);
    return hostName;
}

string getHostFromResourcePathGetRequest(string requestBody){
    return getHostFromResourcePath(5, requestBody);
}

string getHostFromResourcePathPostRequest(string requestBody){
    return getHostFromResourcePath(6, requestBody);
}

bool isGetRequest(string requestBody){
    return ((int)requestBody.find("GET") == 0) ? true : false;
}

int prepareRemoteRequest(string *requestBody, sockaddr_in **serverAddr){
    string hostName = getHostNameFromRequest(*requestBody);
    string port("http");
    string resourcePath("/");

    if(isPortIncluded(hostName)){
        seperateIpAndPort(&hostName, &port);

         if((!hostName.compare("localhost") || !hostName.compare("127.0.0.1")) && !port.compare("8080")){
            port = "http";

            if(isGetRequest(*requestBody))
                hostName = getHostFromResourcePathGetRequest(*requestBody);
            else
                hostName = getHostFromResourcePathPostRequest(*requestBody);

            if(isResourcePathIncluded(hostName)){
                separateHostAndResourcePath(&hostName, &resourcePath);
            }
        }
    }
    try
    {
      *serverAddr = getServerAddressByHostAndPort(hostName, port);
    } catch(const std::runtime_error& e)
    {
        *requestBody = e.what();
        return -1;
    }

    *requestBody = prepareRemoteRequestBody(*requestBody, hostName, resourcePath);

    return 0;
}

void separateHostAndResourcePath(string *hostName, string *resourcePath){
    size_t resourcePathStartPos = hostName->find("/");
    *resourcePath = hostName->substr(resourcePathStartPos, hostName->length() - resourcePathStartPos + 1);
    *hostName = hostName->substr(0, resourcePathStartPos);
}

bool isResourcePathIncluded(string hostName){
    if((int)hostName.find("/") != -1)
        return true;
    return false;
}

void seperateIpAndPort(string *hostName, string *port){
    size_t colonPos = hostName->find(":");
    *port = hostName->substr(colonPos + 1, hostName->length() - colonPos);
    *hostName = hostName->substr(0, colonPos);
}

bool isPortIncluded(string host){
    if ((int) host.find(":") != -1)
        return true;
    return false;
}

string getHostNameFromRequest(string requestBody){

    size_t hostStartPos = requestBody.find(HOST_EXPR) + 6;
    string hostName = requestBody.substr(hostStartPos, requestBody.find("\r\n", hostStartPos) - hostStartPos);

    return hostName;
}

string prepareRemoteRequestBody(string requestBody, string hostName, string resourcePath){
    int resourcePathStartPos = requestBody.find(" ") + 1;
    int resourcePathEndPos = requestBody.find(" ", resourcePathStartPos);
    int resourcePathLen = resourcePathEndPos - resourcePathStartPos;
    requestBody.replace(resourcePathStartPos, resourcePathLen, resourcePath);

    int hostStartPos = requestBody.find("Host: ") + 6;
    int hostEndPos = requestBody.find("\r\n", hostStartPos);
    int hostLen = hostEndPos - hostStartPos;
    requestBody.replace(hostStartPos, hostLen, hostName);

    return requestBody;
}
