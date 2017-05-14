/*
 * Main Class
 */
#ifndef BANNER_LAUNCHER_HPP
#define BANNER_LAUNCHER_HPP

#include <list>
#include <string>

#include <QMainWindow>
#include <QScrollArea>
#include <QGridLayout>
#include <QSpacerItem>

#include "Entry.hpp"
#include "Entry_Widget.hpp"

class Banner_Launcher : public QMainWindow
{
    Q_OBJECT

public:
    Banner_Launcher(float window_height_in_rows, unsigned no_columns, QWidget *parent = 0);
    ~Banner_Launcher();

    void keyPressEvent(QKeyEvent * event);
    void set_displayed_entries(const std::list<Entry *> & entries);
    void update_filter();

public slots:
    void start();

private:
    QWidget *gui;
    QGridLayout *layout = NULL;
    QScrollArea *scroll_gui;

    std::list<Entry_Widget *> widgets;
    std::list<QSpacerItem *> spacers;

    unsigned no_columns;
    std::string filter;
    std::list<Entry *> all_entries;
    std::list<Entry *> displayed_entries;
};

#endif // BANNER_LAUNCHER_HPP
