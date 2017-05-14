/* Filesystem Abstraction Class inspired by python's pathlib and built around
 * Unix file API's.
 */

// TODO: Build in more error detection and handling
// TODO: SymLink detection and resolution
// TODO: Relative and Absolute methods
// TODO: get_parent method
// TODO: Get File extension(s)
// TODO: Get filename without extension(s)
// TODO: Change Extension(s)
// TODO: Permissions
// TODO: Support Tilde and Shell variable expansion
// TODO: Move/Copy/Remove/Touch

#ifndef PATH_HEADER
#define PATH_HEADER

#include <string>
#include <list>

class Path {
public:
    Path();
    Path(const std::string & path);
    Path(const Path & other);
    ~Path();

    enum Path_Type {
        ANY,
        FILE,
        DIRECTORY
    };

    void path(const std::string & path);
    std::string path() const;
    const char * c_str() const;

    bool exists() const;
    bool is_file() const;
    bool is_directory() const;
    std::string get_name() const;

    bool ensure_directory();

    std::list<Path> contents(Path_Type path_type) const;
    std::list<Path> subdirectories() const;

    static Path home();
    static Path xdg_config_home();

    Path operator/(const Path & other) const;
    Path operator/(const std::string & other) const;

private:
    std::string _path;

    bool has_tailing_slash() const;
    static Path append(const Path & a, const std::string & b);
};

#endif // PATH_HPP
