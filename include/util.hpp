#ifndef UTIL_HEADER
#define UTIL_HEADER

#include <string>

/*
 * Download file over HTTP from url to destination, returns 0 if successful else 1.
 * Uses libcurl.
 */
int download(const char * url, const char * destination);

/*
 * Returns true if string begins with prefix
 */
bool starts_with(const std::string & string, const std::string & prefix);

#endif // UTIL_HEADER
