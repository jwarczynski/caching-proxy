#include "server.hpp"
#include "cache.hpp"
#include "client.hpp"

string processRequest(string requestBody);

int main(){
    startServer(processRequest);

    return 0;
}

string processRequest(string requestBody) {
    CacheEntry cachedResponse = retrieveFromCache(requestBody);
    if(cachedResponse.status != CacheEntry::NOT_FOUND) {
        // TODO: If waiting?
        return cachedResponse.responseBody;
    }

    string remoteResponse = makeRequest(requestBody);
    saveToCache(requestBody, remoteResponse);
    return remoteResponse;
}