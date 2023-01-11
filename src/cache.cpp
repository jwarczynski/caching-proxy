#include "cache.hpp"
#include<map>
#include<iostream>

string extractEtag(string requestBody);

struct CacheEntryKey {
    string url;
};

// Compare the keys, for example lexicographically
bool operator<(const CacheEntryKey& l, const CacheEntryKey& r) {
    return (l.url < r.url);
}

// Value = response body
map<CacheEntryKey, CacheEntry> cache;

CacheEntry retrieveFromCache(Request *request){
    CacheEntry response = {
        .status = CacheEntry::NOT_FOUND,
        .etag = "",
        .responseBody = ""
    };

    string url = buildUrl(request);
    CacheEntryKey key = {
        .url = url
    };
    if(cache.find(key) != cache.end()){
        response = cache.at(key);
    }else{
        cout << "Cache miss for: " << url << endl;
    }

    if(response.status == CacheEntry::READY){
        cout << "Cache hit for: " << url << endl;
        // string requestEtag = "";
        // if(request->headers->find("if-none-match") != request->headers->end()){
        //     requestEtag = request->headers->at("if-none-match");
        // }
        // if(requestEtag == response.etag && requestEtag != ""){
        //     response.responseBody = "HTTP/1.1 304 Not Modified\r\nETag: " + response.etag + "\r\n\r\n";
        // }
    }

    return response;
}

void saveToCache(Request *request, string responseBody){
    string url = buildUrl(request);
    CacheEntryKey key = {
        .url = url
    };
    CacheEntry entry = {
        .status = CacheEntry::READY,
        .etag = extractEtag(responseBody),
        .responseBody = responseBody
    };
    cache[key] = entry;

    cout << "Saved to cache: " << url << endl;
}

void markAsWaiting(Request *request){
    string url = buildUrl(request);
    CacheEntryKey key = {
        .url = url
    };
    CacheEntry entry = {
        .status = CacheEntry::WAITING,
        .etag = "",
        .responseBody = ""
    };
    cache[key] = entry;

    cout << "Marked as waiting: " << url << endl;
}

string extractEtag(string requestBody){
    map<string, string> *headers = parseHttpHeaders(requestBody, 0);
    if(headers->find("etag") == headers->end()) return "";

    string etag = headers->at("etag");
    delete headers;
    return etag;
}
