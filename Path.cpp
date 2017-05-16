#include <stdlib.h>
#include <string.h>

#include <regex>
#include <sstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>

#include "Path.hpp"

/*
 * Path_Exception
 */

Path_Exception::Path_Exception(const std::string &message) {
    this->message = message;
}

const char * Path_Exception::what() const throw() {
    return message.c_str();
}

/*
 * Path
 */

Path::Path() { }

Path::Path(const std::string & path, bool expand_tilde) {
    unsigned path_length = path.size();
    if (expand_tilde && path_length && path.front() == '~') {
        uid_t uid = geteuid();
        struct passwd *pw = getpwuid(uid);
        if (pw == NULL) {
            std::stringstream ss;
            ss << "Couldn't get current user: \"" << strerror(errno) << '\"';
            throw Path_Exception(ss.str());
        }
        this->path(pw->pw_dir);
    } else {
        this->path(path);
    }
}

Path::Path(const Path & other) {
    this->path(other.path());
}

Path::~Path() { }

void Path::path(const std::string & path) { this->_path = path; }
std::string Path::path() const { return _path; }
const char * Path::c_str() const { return _path.c_str(); }

Path Path::cwd() {
    char *cwd;
    char buff[PATH_MAX + 1];
    if ((cwd = getcwd(buff, PATH_MAX + 1)) == NULL) {
        std::stringstream ss;
        ss << "Couldn't get current working dictionay: \"" << strerror(errno) << '\"';
        throw Path_Exception(ss.str());
    }
    return Path(cwd);
}

bool Path::exists() const { return access(c_str(), F_OK) == 0; }
bool Path::is_readable() const { return access(c_str(), R_OK) == 0; }
bool Path::is_writable() const { return access(c_str(), W_OK) == 0; }
bool Path::is_executable() const { return access(c_str(), X_OK) == 0; }

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

void Path::make_directory(mode_t mode) {
    if (mkdir(c_str(), mode)) {
        std::stringstream ss;
        ss << "Couldn't create dictionay at\"" << _path
           << "\": \""
           << strerror(errno) << '\"';
        throw Path_Exception(ss.str());
    }
}

std::string Path::get_name() const {
    std::regex re("(.*\\/)?([^/]+)\\/?$");
    std::smatch match;

    std::regex_match(_path, match, re);
    if (match.size() == 3)
        return match[2];

    throw Path_Exception("Couldn't get name.");
}

std::list<Path> Path::contents(Path_Type path_type) const {
    std::list<Path> result;

    DIR * directory;
    if ((directory = opendir(c_str())) != NULL) {
        bool parent = true;
        bool current = true;
        for (
            struct dirent * i = readdir(directory);
            i != NULL; i = readdir(directory)
        ) {
            if (parent && !strcmp(i->d_name, "..")) {
                parent = false;
                continue;
            }
            if (current && !strcmp(i->d_name, ".")) {
                current = false;
                continue;
            }

            Path child = *this / i->d_name;
            struct stat s;
            if (stat(child.c_str(), &s) == -1) {
                continue;
            }

            if (
                path_type == ANY ||
                (path_type == FILE && ((s.st_mode & S_IFMT ) == S_IFREG)) ||
                (path_type == DIRECTORY && ((s.st_mode & S_IFMT ) == S_IFDIR))
            ) {
                result.push_back(child);
            }
        }
        closedir(directory);
    } else {
        std::stringstream ss;
        ss << "Couldn't open \"" << _path
           << "\"as dictionary to list contents: \""
           << strerror(errno) << '\"';
        throw Path_Exception(ss.str());
    }

    return result;
}

std::list<Path> Path::subdirectories() const {
    return contents(DIRECTORY);
}

Path Path::home() {
    char * home = getenv("HOME");
    if (home == NULL)
        throw Path_Exception("Couldn't get $HOME envriomental variable");
    return Path(home);
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
