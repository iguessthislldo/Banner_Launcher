/* Filesystem Abstraction Class inspired by python's pathlib and built around
 * Unix file API's.
 */

// TODO: Build in more error detection and handling
// TODO: SymLink detection and resolution
// TODO: Relative and Absolute methods
// TODO: get_parent method
// TODO: Get/change File extension(s)
// TODO: Get/Change filename without extension(s)
// TODO: Permissions
// TODO: Support Tilde and Shell variable expansion
// TODO: Move/Copy/Remove/Touch

#ifndef PATH_HEADER
#define PATH_HEADER

#include <string>
#include <list>
#include <exception>

#include <sys/stat.h>

class Path_Exception : public std::exception {
public:
    Path_Exception(const std::string & message);
    virtual const char * what() const throw();
private:
    std::string message;
};

class Path {
public:
    enum Path_Type {
        ANY,
        FILE = S_IFSOCK,
        DIRECTORY = S_IFDIR,
        SYMBOLIC_LINK = S_IFLNK,
        SOCKET = S_IFSOCK,
        FIFO = S_IFIFO,
        BLOCK_DEVICE = S_IFBLK ,
        CHARACTER_DEVICE = S_IFCHR
    };

    // Blank Path
    Path();

    /*
     * When expand_tilde is true, a tilde at the begining of the path argument
     * is replaced with the current users home directory.
     */
    Path(const std::string & path, bool expand_tilde = false);

    Path(const Path & other);
    ~Path();

    /*
     * Get current working directory
     */
    Path cwd();

    // Path functions
    void path(const std::string & path);
    std::string path() const;
    const char * c_str() const;

    // Permission Functions
    bool exists() const;
    bool is_readable() const;
    bool is_writable() const;
    bool is_executable() const;

    // Path Resolution
    bool is_absolute() const;
    bool is_relative() const;
    Path resolve() const;

    // Type Functions
    Path_Type get_type() const;
    bool is_file() const;
    bool is_directory() const;
    bool is_symbolic_link() const;
    bool is_socket() const;
    bool is_fifo() const;
    bool is_block_device() const;
    bool is_character_device() const;

    // Filename Functions
    std::string get_name() const;

    // File Manipulation Functions
    void touch() const;
    void unlink() const;

    // Directory Functions
    void make_directory(mode_t mode = 0744);
    void remove_directory() const;
    void rmfr() const; // Recursive Delete
    std::list<Path> contents(Path_Type path_type) const;
    std::list<Path> subdirectories() const;

    // Static Common path functions
    static Path root();
    static Path home();
    static Path xdg_config_home();

    // Misc
    static Path from_var_else(const std::string & variable, const std::string & otherwise);
    static Path from_var_else(const std::string & variable, const Path & otherwise);

    // Append
    Path operator/(const Path & other) const;
    Path operator/(const std::string & other) const;

private:
    std::string _path;
    std::list<std::string> path_list;

    bool has_tailing_slash() const;
    static Path append(const Path & a, const std::string & b);
};

#endif // PATH_HEADER
