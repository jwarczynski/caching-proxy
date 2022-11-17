#include "server.hpp"
#include "cache.hpp"
#include "client.hpp"

string processRequest(string requestBody);

int main(){
    startServer(processRequest);

    return 0;
}

string processRequest(string requestBody) {

    string remoteResponse = makeRequest(requestBody);
    return remoteResponse;


    string cachedResponse = retrieveFromCache(requestBody);
    if(cachedResponse != "") {
        return cachedResponse;
    }

    saveToCache(requestBody, remoteResponse);
    return remoteResponse;
}