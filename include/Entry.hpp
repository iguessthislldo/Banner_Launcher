/*
 * Represents an Executable Entry. The actual GUI widget is the Entry_Widget.
 */
#ifndef ENTRY_HPP
#define ENTRY_HPP

#include <QString>
#include <QImage>
#include <QJsonObject>

#include "Entry_Widget.hpp"

class Entry {
public:
    Entry(unsigned id);
    Entry(const QString & id, const QJsonObject & json);
    ~Entry();

    /*
     * Internal ID used to differ entries.
     */
    QString id();

    /*
     * Name is the displayed name.
     */
    QString name();
    void name(const QString & value);

    /*
     * Filter Name is the simplified version of the name. All uppercase,
     * digits, and spaces, striped of anything else.
     */
    QString filter_name();

    /*
     * steam id is the entries steam app id. It is blank if the app doesn't
     * have one. If not blank it is used to start the entry through steam.
     */
    QString steam_id();
    void steam_id(const QString & value);

    /*
     * Number of times this entry has been run.
     */
    unsigned count();

    /*
     * Image to display.
     */
    QString image_path();
    void image_path(const QString & value);
    void load_image();
    QImage * image_ptr();
    QImage & image_ref();

    void set_parent(QWidget *parent);
    Entry_Widget * get_widget();

    /*
     * Run the Entry. If successful, the program will be replaced. Otherwise
     * return the error message to display which will have the reason for
     * failure.
     */
    QString run();

    /*
     * Returns true if the Entry has a valid image, name, filter_name, and
     * executable.
     */
    bool is_valid();

    /*
     * Return QJsonObject of this Entry
     */
    QJsonObject toJSON();

    /*
     * Compare first by the count, then using built in string compare on the
     * entry name.
     */
    static bool compare(Entry * a, Entry * b);


private:
    QString _id, _name, _filter_name, _steam_id, _execute, _cd, _image_path;
    unsigned _count = 0;
    QImage * _image = NULL;
    Entry_Widget * widget = NULL;
};

#endif // ENTRY_HPP
