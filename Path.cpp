#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include "Path.hpp"

Path::Path() { }

Path::Path(const std::string & path) {
    this->path(path);
}

Path::Path(const Path & other) {
    this->path(other.path());
}

Path::~Path() { }

void Path::path(const std::string & path) {
    this->_path = path;
}

std::string Path::path() const {
    return _path;
}

const char * Path::c_str() const {
    return _path.c_str();
}

bool Path::exists() const {
    struct stat s;
    int rv = stat(c_str(), &s);
    return !rv;
}

bool Path::is_file() const {
    struct stat s;
    int rv = stat(c_str(), &s);
    if (rv == -1) return false;
    return (s.st_mode & S_IFMT) == S_IFREG;
}

bool Path::is_directory() const {
    struct stat s;
    int rv = stat(c_str(), &s);
    if (rv == -1) return false;
    return (s.st_mode & S_IFMT) == S_IFDIR;
}

bool Path::is_parent_or_current() const {
    return _path == "." || _path == "..";
}

bool Path::ensure_directory() {
    return !mkdir(c_str(), 0700);
}

std::list<Path> Path::subdirectories() const {
    std::list<Path> result;

    DIR * directory;
    if ((directory = opendir(c_str())) != NULL) {
        bool parent = true;
        bool current = true;
        for (
            struct dirent * i = readdir(directory);
            i != NULL; i = readdir(directory)
        ) {
            if (parent) {
                if(!strcmp(i->d_name, "..")) {
                    parent = false;
                    continue;
                }
            }
            if (current) {
                if(!strcmp(i->d_name, ".")) {
                    current = false;
                    continue;
                }
            }

            Path child = *this / i->d_name;
            struct stat s;
            if (stat(child.c_str(), &s) == -1) {
                continue;
            }
            if ((s.st_mode & S_IFMT ) == S_IFDIR) {
                result.push_back(child);
            } else {
                continue;
            }
        }
    }

    return result;
}

Path Path::home() {
    return Path(getenv("HOME"));
}

Path Path::xdg_config_home() {
    char * xdg_config_home = getenv("XDG_CONFIG_HOME");

    if (xdg_config_home == NULL) { // Else guessing that it's at $HOME/.config
        return home() / ".config";
    }

    return Path(xdg_config_home);
}

Path Path::operator/(const Path & other) const {
    return append(*this, other.path());
}

Path Path::operator/(const std::string & other) const {
    return append(*this, other);
}

bool Path::has_tailing_slash() const {
    if (_path.size()) {
        return _path.back() == '/';
    }
    return false;
}

Path Path::append(const Path & a, const std::string & b) {
    if (a.has_tailing_slash()) {
        return Path(a.path() + b);
    } else {
        return Path(a.path() + "/" + b);
    }
}
