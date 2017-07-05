/*
 * Main Class
 */
#ifndef BANNER_LAUNCHER_HPP
#define BANNER_LAUNCHER_HPP

#include <list>

#include <QMainWindow>
#include <QLabel>
#include <QScrollArea>
#include <QFont>
#include <QString>

#include "Entry.hpp"
#include "Entry_Widget.hpp"
#include "Steam_Dialog.hpp"
#include "Path.hpp"

class Banner_Launcher : public QMainWindow
{
    Q_OBJECT

public:
    Banner_Launcher(float window_height_in_rows, unsigned no_columns, QWidget *parent = 0);
    ~Banner_Launcher();

    /*
     * Load and save from config.json
     */
    void load();
    void save();

    void keyPressEvent(QKeyEvent * event);
    void set_displayed_entries(const std::list<Entry *> & entries, bool is_filtered);
    void update_filter();

    Path get_application_directory();
    Path get_header_directory();

public slots:
    void start();
    void ShowContextMenu(const QPoint &pos);
    void show_steam_dialog();

private:
    QWidget *gui;
    QScrollArea *scroll_gui;
    QLabel * filter_label, * no_entires_label;

    QFont font;

    std::list<Entry_Widget *> widgets;

    Path application_directory,
         header_directory,
         steam_directory,
         config_file;

    unsigned no_columns, next_id;
    QString filter, bg_color, fg_color;
    std::list<Entry *> all_entries;
    std::list<Entry *> displayed_entries;
};

extern Banner_Launcher * application;

#endif // BANNER_LAUNCHER_HPP
