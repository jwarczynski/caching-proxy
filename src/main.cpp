#include "server.hpp"
#include "cache.hpp"
#include "client.hpp"

string processRequest(string requestBody);

int main(){
    startServer(processRequest);

    return 0;
}

string processRequest(string requestBody) {
    string cachedResponse = retrieveFromCache(requestBody);
    if(cachedResponse != "") {
        return cachedResponse;
    }

    string remoteResponse = makeRequest(requestBody);
    saveToCache(requestBody, remoteResponse);
    return remoteResponse;
}