#include "server.hpp"
#include "cache.hpp"
#include "client.hpp"

string processRequest(string requestBody);

int main(){
    startServer(processRequest);

    return 0;
}

string processRequest(string requestBody) {
    // Print "Hello, world!" example page
    return "HTTP/1.1 200 OK\r\n"
        "Content-Length: 58\r\n"
        "Content-Type: text/html\r\n"
        "Connection: Closed\r\n"
        "\r\n"
        "<html>\r\n"
        "<body>\r\n"
        "<h1>Hello, World!</h1>\r\n"
        "</body>\r\n"
        "</html>\r\n";

    string cachedResponse = retrieveFromCache(requestBody);
    if(cachedResponse != "") {
        return cachedResponse;
    }

    string remoteResponse = makeRequest("http://example.com", requestBody);
    saveToCache(requestBody, remoteResponse);
    return remoteResponse;
}