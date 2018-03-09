#include "util.h"

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

