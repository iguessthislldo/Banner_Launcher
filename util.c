#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>

#include <curl/curl.h>
#include <glib.h>

#include "util.h"

bool download(
    void * data,
    int (*callback)(void*, double, double, double, double),
    const char * url, const char * destination
) {
    CURL * curl;
    if ((curl = curl_easy_init()) == NULL) {
        fprintf(stderr, "Couldn't init curl\n");
        return true;
    }

    FILE * file;
    if ((file = fopen(destination, "wb")) == NULL) {
        fprintf(
            stderr,
            "Couldn't open file \"%s\" to write: \"%s\"\n",
            destination,
            strerror(errno)
        );
        return true;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
    if (callback) {
        curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, data);
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, callback);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    }

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
        return true;
    }

    curl_easy_cleanup(curl);
    fclose(file);
    return false;
}

bool starts_with(const char * string, const char * prefix) {
    size_t i = 0;
    char string_char, prefix_char;
    while (true) {
        string_char = string[i];
        prefix_char = prefix[i];
        if (!prefix_char) {
            return true;
        }
        if (string_char !=  prefix_char) {
            return false;
        }
        i++;
    }
}

#define TIME_STRING_LEN 15
// 4 + 2+2+2+2+2+  1 = 15
// YYYYMMDDhhmmss /0
char * get_time_string() {
    char * string = g_malloc(TIME_STRING_LEN);
    time_t epoch = time(NULL);
    struct tm utc = *gmtime(&epoch);
    strftime(string, TIME_STRING_LEN, "%Y%m%d%H%M%S", &utc);
    return string;
}

