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
    // Request body is not important for us.
};

Request *parseRequestBody(string &requestBody);
void freeRequest(Request *request);
string buildUrl(Request *request);

#endif