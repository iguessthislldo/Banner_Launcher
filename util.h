#include <stdbool.h>
#include <stdlib.h>

bool download(void * data,
    int (*callback)(void*, double, double, double, double),
    const char * url, const char * destination
);
bool starts_with(const char * string, const char * prefix);
