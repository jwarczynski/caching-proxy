#ifndef HTTP_HPP
#define HTTP_HPP

#include<string>
#include<map>

using namespace std;

struct Request {
    string resourcePath;
    string method;
    string version;
    map<string, string> *headers;
    string rawRequest;
};

struct Url {
    string protocol;
    string host;
    string port;
    string path;
};

Request *parseRequestBody(string &requestBody);
map<string, string>* parseHttpHeaders(string requestBody, int startPos);
void freeRequest(Request *request);
string buildUrl(Request *request);
Url parseUrl(string url);

#endif