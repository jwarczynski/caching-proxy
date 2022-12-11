#include "cache.hpp"
#include "http.hpp"
#include<map>
#include<iostream>

struct CacheEntryKey {
    string url;
};

// Compare the keys, for example lexicographically
bool operator<(const CacheEntryKey& l, const CacheEntryKey& r) {
    return (l.url < r.url);
}

// Value = response body
map<CacheEntryKey, CacheEntry> cache;

CacheEntry retrieveFromCache(string requestBody){
    CacheEntry responseBody = {
        .status = CacheEntry::NOT_FOUND,
        .responseBody = ""
    };
    Request *request = parseRequestBody(requestBody);

    string url = buildUrl(request);
    CacheEntryKey key = {
        .url = url
    };
    if(cache.find(key) != cache.end()){
        responseBody = cache.at(key);
        cout << "Cache hit for: " << url << endl;
    }else{
        cout << "Cache miss for: " << url << endl;
    }

    freeRequest(request);
    return responseBody;
}

void saveToCache(string requestBody, string responseBody){
    Request *request = parseRequestBody(requestBody);

    string url = buildUrl(request);
    CacheEntryKey key = {
        .url = url
    };
    CacheEntry entry = {
        .status = CacheEntry::READY,
        .responseBody = responseBody
    };
    cache.insert(pair<CacheEntryKey, CacheEntry>(key, entry));

    cout << "Saved to cache: " << url << endl;

    freeRequest(request);
}