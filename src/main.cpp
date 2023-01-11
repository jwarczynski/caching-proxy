#include "server.hpp"
#include "cache.hpp"
#include "client.hpp"
#include "http.hpp"
#include<thread>

string processRequest(string requestBody);

int main(){
    startServer(processRequest);

    return 0;
}

string processRequest(string requestBody) {
    Request *request = parseRequestBody(requestBody);

    CacheEntry cachedResponse = retrieveFromCache(request);
    if(cachedResponse.status != CacheEntry::NOT_FOUND) {
        while(cachedResponse.status == CacheEntry::WAITING){
            this_thread::sleep_for(chrono::milliseconds(100));
            cachedResponse = retrieveFromCache(request);
        }

        freeRequest(request);
        return cachedResponse.responseBody;
    }

    markAsWaiting(request);
    string remoteResponse = makeRequest(requestBody);
    saveToCache(request, remoteResponse);

    freeRequest(request);
    return remoteResponse;
}