/*
 * Main Class
 */
#ifndef BANNER_LAUNCHER_HPP
#define BANNER_LAUNCHER_HPP

#include <list>
#include <string>

#include <QMainWindow>
#include <QLabel>
#include <QScrollArea>
#include <QFont>

#include "Entry.hpp"
#include "Entry_Widget.hpp"
#include "Steam_Dialog.hpp"

class Banner_Launcher : public QMainWindow
{
    Q_OBJECT

public:
    Banner_Launcher(float window_height_in_rows, unsigned no_columns, QWidget *parent = 0);
    ~Banner_Launcher();

    void keyPressEvent(QKeyEvent * event);
    void set_displayed_entries(const std::list<Entry *> & entries);
    void update_filter();

    static Path get_application_directory();

public slots:
    void start();
    void ShowContextMenu(const QPoint &pos);
    void show_steam_dialog();

private:
    QWidget *gui;
    QScrollArea *scroll_gui;
    QLabel *search_text;

    QFont font;

    std::list<Entry_Widget *> widgets;

    static Path application_directory;

    unsigned no_columns;
    std::string filter;
    std::list<Entry *> all_entries;
    std::list<Entry *> displayed_entries;
};

#endif // BANNER_LAUNCHER_HPP
