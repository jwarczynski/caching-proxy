#include "cache.hpp"
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

CacheEntry retrieveFromCache(Request *request){
    CacheEntry responseBody = {
        .status = CacheEntry::NOT_FOUND,
        .responseBody = ""
    };

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

    return responseBody;
}

void saveToCache(Request *request, string responseBody){
    string url = buildUrl(request);
    CacheEntryKey key = {
        .url = url
    };
    CacheEntry entry = {
        .status = CacheEntry::READY,
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
        .responseBody = ""
    };
    cache[key] = entry;

    cout << "Marked as waiting: " << url << endl;
}
