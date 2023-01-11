#include "client.hpp"
#include "http.hpp"

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdexcept>
#include <thread>
#include <iostream>


sockaddr_in* getServerAddressByHost(string domain);
sockaddr_in *getServerAddressByHostAndPort(string host, string port);
void removeHeader(string *requestBody, string header);

string prepareRemoteRequestBody(string requestBody, string hostName, string resourcePath);
int prepareRemoteRequest(string *requestBody, sockaddr_in **serverAddr);

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
    int i = 0;
    ssize_t bytesRead = 0;
    ssize_t currRead;
    do{
        currRead = recv(clientSocket, rcvBuf + bytesRead, INT16_MAX - bytesRead, i ? MSG_DONTWAIT : 0);
        if(currRead > 0)
            bytesRead += currRead;

        if(currRead < 0 && errno != EWOULDBLOCK){
            return handleInternalError(clientSocket, "error during reading http response");
        }
        this_thread::sleep_for(chrono::milliseconds(50));
        i++;
    } while(currRead > 0);

    if(bytesRead == 0){
        return handleInternalError(clientSocket, "received nothing from remote server");
    }

    close(clientSocket);
    return string(rcvBuf, bytesRead);
}

int prepareRemoteRequest(string *requestBody, sockaddr_in **serverAddr){
    Request* request = parseRequestBody(*requestBody);
    string urlString = buildUrl(request);
    Url url = parseUrl(urlString);

    freeRequest(request);

    try{
        *serverAddr = getServerAddressByHostAndPort(url.host, url.port);
    }catch(const std::runtime_error& e){
        *requestBody = e.what();
        return -1;
    }

    *requestBody = prepareRemoteRequestBody(*requestBody, url.host, url.path);
    return 0;
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

    removeHeader(&requestBody, "If-None-Match:");
    removeHeader(&requestBody, "If-Modified-Since:");

    return requestBody;
}

void removeHeader(string *requestBody, string header){
    size_t etagStartPos = requestBody->find(header);
    if(etagStartPos != string::npos){
        cout << "Removing " << header << endl;
        size_t etagEndPos = requestBody->find("\r\n", etagStartPos) + 2;
        size_t etagLen = etagEndPos - etagStartPos;
        requestBody->erase(etagStartPos, etagLen);
    }
}
