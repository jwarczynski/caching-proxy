#ifndef CACHE_HPP
#define CACHE_HPP

#include<string>
#include<map>

using namespace std;

string retrieveFromCache(string requestBody);
void saveToCache(string requestBody, string responseBody);

#endif