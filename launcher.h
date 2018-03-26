#ifndef LAUNCHER_HEADER
#define LAUNCHER_HEADER

#include <stdbool.h>

#include "Entry.h"

#define APP_NAME "Banner Launcher"
#define BANNER_WIDTH 460
#define BANNER_HIGHT 215
#define GRID_WIDTH 3

bool debug;
bool dev_mode;

Entries * all_entries;
Entries * visable_entries;

char * steam_path;
Entries * steam_entries;
bool include_steam_entries;

char * config_dir;
char * entries_file;
char * config_file;
char * banners_dir;

unsigned next_id;

#endif
