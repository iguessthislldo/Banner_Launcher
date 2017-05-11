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

    void path(const std::string & path);
    std::string path() const;
    const char * c_str() const;

    bool exists() const;
    bool is_file() const;
    bool is_directory() const;
    bool is_parent_or_current() const;

    bool ensure_directory();

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
