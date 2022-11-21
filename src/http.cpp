#include "http.hpp"

Request *parseRequestBody(string &requestBody){
    Request *request = new Request();

    // Extract the URL, method and version
    size_t methodEnd = requestBody.find(' ');
    request->method = requestBody.substr(0, methodEnd);
    
    size_t urlEnd = requestBody.find(' ', methodEnd + 1);
    request->resourcePath = requestBody.substr(methodEnd + 1, urlEnd - methodEnd - 1);
    
    size_t versionEnd = requestBody.find("\r\n", urlEnd + 1);
    request->version = requestBody.substr(urlEnd + 1, versionEnd - urlEnd - 1);


    // Parse headers
    map<string, string> *headers = new map<string, string>();
    ssize_t lineBeginning = versionEnd + 2;
    ssize_t lineEnd = requestBody.find("\r\n", lineBeginning + 1);
    do {
        string line = requestBody.substr(lineBeginning, lineEnd - lineBeginning);
        size_t colon = line.find(':');
        string key = line.substr(0, colon);
        string value = line.substr(colon + 1);
        headers->insert(pair<string, string>(key, value));
        lineBeginning = lineEnd + 2;
        lineEnd = requestBody.find("\r\n", lineBeginning + 1);
    } while (lineEnd - lineBeginning > 0 && lineEnd != string::npos);

    request->headers = headers;

    // Body can be extracted here, but we don't need it.

    return request;
}

void freeRequest(Request *request){
    delete request->headers;
    delete request;
}

// TODO: Determine if the target host is passed as a header or as a part of the path
string buildUrl(Request *request){
    string url = request->resourcePath;
    if(request->headers->find("Host") != request->headers->end()){
        url = request->headers->at("Host") + url;
    }
    return url;
}