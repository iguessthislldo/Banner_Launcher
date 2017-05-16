#include <stdio.h>
#include <string.h> // strerror
#include <errno.h>

#include <curl/curl.h>

#include "util.hpp"

int download(const char * url, const char * destination) {
    CURL *curl;
    if ((curl = curl_easy_init()) == NULL) {
        fprintf(stderr, "Couldn't init curl");
        return 1;
    }

    FILE *file;
    if ((file = fopen(destination, "wb")) == NULL) {
        fprintf(
            stderr,
            "Couldn't open file \"%s\" to write: \"%s\"\n",
            destination,
            strerror(errno)
        );
        return 1;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

    CURLcode result = curl_easy_perform(curl);
    if (result != CURLE_OK) {
        fprintf(
            stderr,
            "Couldn't download file from \"%s\" to \"%s\": \"%s\" (%d)\n",
            url,
            destination,
            curl_easy_strerror(result),
            result
        );
        return 1;
    }

    curl_easy_cleanup(curl);
    fclose(file);
    return 0;
}

bool starts_with(const std::string & string, const std::string & prefix) {
    unsigned string_size = string.size();
    unsigned prefix_size = prefix.size();
    if (string_size < prefix_size)
        return false;

    for (unsigned i = 0; i < prefix_size; i++) {
        if (string[i] != prefix[i])
            return false;
    }
    return true;
}
