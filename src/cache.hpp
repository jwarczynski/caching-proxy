#ifndef CACHE_HPP
#define CACHE_HPP

#include<string>
#include<map>

using namespace std;

struct CacheEntry {
    enum { READY, WAITING, NOT_FOUND } status;
    string responseBody;
};

CacheEntry retrieveFromCache(string requestBody);
void saveToCache(string requestBody, string responseBody);

#endif