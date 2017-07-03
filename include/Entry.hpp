#ifndef ENTRY_HPP
#define ENTRY_HPP

#include <string>

#include <QImage>

#include "Path.hpp"
#include "Entry_Widget.hpp"

class Entry {
public:
    Entry(const Path & path);
    ~Entry();

    std::string steam_id();
    void steam_id(const std::string & value);
    std::string get_name();
    const Path & get_path();
    std::string get_filter_name();
    QImage * get_image();
    QImage & get_image_ref();
    unsigned get_count();
    bool is_valid();
    Entry_Widget * get_widget(QWidget *parent = 0);

    std::string run();

    /*
     * Compare first by the count, then using built in string compare on the
     * entry name.
     */
    static bool compare(Entry * a, Entry * b);

private:
    Entry_Widget * widget = NULL;
    Path path, executable;
    std::string name, filter_name, _steam_id;
    QImage * image = NULL;
    unsigned count = 0; // Run count
};

#endif // ENTRY_HPP
