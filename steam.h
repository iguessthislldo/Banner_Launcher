#ifndef STEAM_HEADER
#define STEAM_HEADER

#include "Entry.h"

#define STEAM_HEADER_URL_HEAD "http://cdn.akamai.steamstatic.com/steam/apps/"
#define STEAM_HEADER_URL_TAIL "/header.jpg"

char * steam_header_url_head;
unsigned steam_header_url_head_len;
char * steam_header_url_tail;
unsigned steam_header_url_tail_len;

unsigned download_images_count;

void load_steam_entries();

#endif
