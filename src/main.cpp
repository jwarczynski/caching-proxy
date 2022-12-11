#include "server.hpp"
#include "cache.hpp"
#include "client.hpp"
#include "http.hpp"

string processRequest(string requestBody);

int main(){
    startServer(processRequest);

    return 0;
}

string processRequest(string requestBody) {
    Request *request = parseRequestBody(requestBody);

    CacheEntry cachedResponse = retrieveFromCache(request);
    if(cachedResponse.status != CacheEntry::NOT_FOUND) {
        // TODO: If waiting?
        return cachedResponse.responseBody;
    }

    string remoteResponse = makeRequest(requestBody);
    saveToCache(request, remoteResponse);

    freeRequest(request);
    return remoteResponse;
}