#include "http.hpp"
#include<bits/stdc++.h>

Request *parseRequestBody(string &requestBody){
    Request *request = new Request();

    // Extract the URL, method and version
    size_t methodEnd = requestBody.find(' ');
    request->method = requestBody.substr(0, methodEnd);
    
    size_t urlEnd = requestBody.find(' ', methodEnd + 1);
    request->resourcePath = requestBody.substr(methodEnd + 1, urlEnd - methodEnd - 1);
    
    size_t versionEnd = requestBody.find("\r\n", urlEnd + 1);
    request->version = requestBody.substr(urlEnd + 1, versionEnd - urlEnd - 1);

    request->headers = parseHttpHeaders(requestBody, versionEnd + 2);

    request->rawRequest = requestBody;

    return request;
}

map<string, string>* parseHttpHeaders(string requestBody, int startPos){
    map<string, string> *headers = new map<string, string>();
    size_t lineBeginning = startPos;
    size_t lineEnd = requestBody.find("\r\n", lineBeginning + 1);
    do {
        string line = requestBody.substr(lineBeginning, lineEnd - lineBeginning);
        size_t colon = line.find(':');
        if(colon != string::npos){
            size_t valueStart = line.find_first_not_of(' ', colon + 1);
            string key = line.substr(0, colon);
            transform(key.begin(), key.end(), key.begin(), ::tolower);
            string value = line.substr(valueStart);
            headers->insert(pair<string, string>(key, value));
        }
        lineBeginning = lineEnd + 2;
        lineEnd = requestBody.find("\r\n", lineBeginning + 1);
    } while (lineEnd - lineBeginning > 0 && lineEnd != string::npos);
    return headers;
}

void freeRequest(Request *request){
    delete request->headers;
    delete request;
}

string buildUrl(Request *request){
    string url = request->resourcePath;

    size_t httpPos = url.find("http://");
    if(httpPos == 0) return url.substr(7);
    if(httpPos == 1 && url[0] == '/') return url.substr(8);

    size_t httpsPos = url.find("https://");
    if(httpsPos == 0) return url.substr(8);
    if(httpsPos == 1 && url[0] == '/') return url.substr(9);

    if(request->headers->find("host") != request->headers->end()){
        url = request->headers->at("host") + url;
    }
    return url;
}

Url parseUrl(string url){
    Url parsedUrl = {
        .protocol = "http",
        .host = "",
        .port = "80",
        .path = "/"
    };

    size_t protocolEnd = url.find("://");
    if(protocolEnd != string::npos){
        parsedUrl.protocol = url.substr(0, protocolEnd);
        url = url.substr(protocolEnd + 3);
    }

    size_t pathStart = url.find('/');
    if(pathStart != string::npos){
        parsedUrl.path = url.substr(pathStart);
        url = url.substr(0, pathStart);
    }

    size_t portStart = url.find(':');
    if(portStart != string::npos){
        parsedUrl.port = stoi(url.substr(portStart + 1));
        url = url.substr(0, portStart);
    }

    parsedUrl.host = url;

    return parsedUrl;
}