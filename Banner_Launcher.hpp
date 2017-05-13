/*
 * Main Class
 */
#ifndef BANNER_LAUNCHER_HPP
#define BANNER_LAUNCHER_HPP

#include <list>

#include <QMainWindow>
#include <QScrollArea>
#include <QGridLayout>

#include "Menu_Item.hpp"

class Banner_Launcher : public QMainWindow
{
    Q_OBJECT

public:
    Banner_Launcher(float window_height_in_rows, unsigned no_columns, QWidget *parent = 0);
    ~Banner_Launcher();

    void keyPressEvent(QKeyEvent * event);
    void set_displayed_items(const std::list<Menu_Item *> & items);

public slots:
    void start();

private:
    QWidget *gui;
    QGridLayout *layout;
    QScrollArea *scroll_gui;

    unsigned no_columns;
    std::list<Menu_Item *> all_items;
    std::list<Menu_Item *> displayed_items;
};

#endif // BANNER_LAUNCHER_HPP
