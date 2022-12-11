#ifndef CACHE_HPP
#define CACHE_HPP

#include<string>
#include<map>
#include "http.hpp"

using namespace std;

struct CacheEntry {
    enum { READY, WAITING, NOT_FOUND } status;
    string responseBody;
};

CacheEntry retrieveFromCache(Request *request);
void saveToCache(Request *request, string responseBody);

#endif