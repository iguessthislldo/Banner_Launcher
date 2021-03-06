#ifndef UTIL_HEADER
#define UTIL_HEADER

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/*
 * Try to download a file from "url" to "destination" with "data"
 * being passed to "callback".
 *
 * Returns true if there was an error
 */
bool download(void * data,
    int (*callback)(void*, double, double, double, double),
    const char * url, const char * destination
);

/*
 * Returns true if "string" starts with or is the same as "prefix".
 */
bool starts_with(const char * string, const char * prefix);

/*
 * Return the file extension of the filename in the string, including
 * the last period, in regex:
 *     ^.*(\..*)$
 * If there are no periods in the filename, it returns a pointer to
 * the terminating null character.
 */
char * file_ext(const char * path, unsigned * len);

/* ======================================================================
 * Time Functions
 *
 * Time is stored as a string, in a format similar to ISO 8601:
 * YYYYMMDDhhmmss
 * Ex: July 20, 1969 at 20:18:00 is "19690720201800"
 */

/*
 * Return the current time in YYYYMMDDhhmmss format on the glib heap.
 */
char * get_time_string();

/*
 * Compare 2 time strings, returns true if "a" is more recent than "b"
 */
//bool compare_time_strings(const char * a, const char * b)
#define compare_time_strings(a, b) (strcmp((a), (b)) > 0)

#endif
