#include "server.hpp"

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int createSocket();
void bindSocket(int socket, short port);
void listenOnSocket(int socket, int backlog);


void startServer(requestHandler handleRequest){
    int serverSocket = createSocket();
    bindSocket(serverSocket, 8080);
    listenOnSocket(serverSocket, 1);

    while(true){
        struct sockaddr_in clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
        if(clientSocket == -1){
            perror("Failed to accept connection");
            exit(1);
        }

        char buffer[1024];
        int bytesRead = read(clientSocket, buffer, 1024);
        if(bytesRead == -1){
            perror("Failed to read from socket");
            exit(1);
        }

        try {
            string requestBody(buffer, bytesRead);
            string responseBody = handleRequest(requestBody);

            write(clientSocket, responseBody.c_str(), responseBody.length());
        }catch(exception &e){
            // TODO: Error page?
        }
        close(clientSocket);
    }
}

int createSocket(){
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocket == -1){
        perror("Failed to create socket");
        exit(1);
    }

    int optval = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    return serverSocket;
}

void bindSocket(int serverSocket, short port){
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if(bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1){
        perror("Failed to bind socket");
        exit(1);
    }
}

void listenOnSocket(int socket, int backlog){
    if(listen(socket, backlog) == -1){
        perror("Failed to listen on socket");
        exit(1);
    }
}