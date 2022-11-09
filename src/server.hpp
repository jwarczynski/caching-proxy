#ifndef SERVER_HPP
#define SERVER_HPP

#include<string>

using namespace std;

typedef string (*requestHandler)(string requestBody);

void startServer(requestHandler handleRequest);

#endif