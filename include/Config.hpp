/*
 * Class passed to the Main Application Class and between the classes.
 */
#ifndef CONFIG_HEADER
#define CONFIG_HEADER

#include <list>
#include <map>
#include <string>

#include <QCoreApplication>
#include <QString>

#include "Path.hpp"
#include "Entry.hpp"

class Config {
public:
    Config(const QCoreApplication & application);

    void load();
    void save();

    const Path & config_directory() { return _config_directory; }
    const Path & header_directory() { return _header_directory; }
    const Path & steam_directory() { return _steam_directory; }
    const Path & steam_executable() { return _steam_executable; }

    QString css();
    std::string steam_header_url();

    std::map<QString, Entry> entries;
    std::list<QString> steam_ids;

private:
    const char * CONFIG_FILE_NAME = "config.json";
    const char * HEADER_DIRECTORY_NAME = "headers";

    // Default Values
    const char * DEFAULT_BG_COLOR = "#383838";
    const char * DEFAULT_FG_COLOR = "#00ff00";
    const char * DEFAULT_STEAM_PATH = "~/.local/share/Steam";
    const char * DEFAULT_STEAM_EXECUTABLE = "/usr/bin/steam";
    const char * DEFAULT_STEAM_HEADER_URL_HEAD = "http://cdn.akamai.steamstatic.com/steam/apps/";
    const char * DEFAULT_STEAM_HEADER_URL_TAIL = "/header.jpg";

    Path _config_directory,
         _header_directory,
         _steam_directory,
         _steam_executable;

    unsigned next_id = 0;
};

#endif
