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
    Banner_Launcher(Config * config, QWidget *parent = 0);
    ~Banner_Launcher();

    void keyPressEvent(QKeyEvent * event);
    void set_displayed_entries(const std::list<Entry *> & entries, bool is_filtered);
    void update_filter();

    void update_entries();

public slots:
    void start();
    void ShowContextMenu(const QPoint &pos);
    void show_steam_dialog();

private:

    QFont font;

    QWidget *gui;
    QScrollArea *scroll_gui;
    QLabel * filter_label, * no_entires_label;

    std::map<Entry_Widget *> widgets;

    unsigned no_columns;
    QString filter;
    std::list<Entry *> displayed_entries;
};

extern Banner_Launcher * application;

#endif // BANNER_LAUNCHER_HPP
