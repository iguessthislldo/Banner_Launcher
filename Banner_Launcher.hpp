#ifndef BANNER_LAUNCHER_HPP
#define BANNER_LAUNCHER_HPP

#include <vector>

#include <QMainWindow>
#include <QScrollArea>
#include <QGridLayout>

#include "Menu_Item.hpp"

class Banner_Launcher : public QMainWindow
{
    Q_OBJECT

public:
    Banner_Launcher(QWidget *parent = 0);
    ~Banner_Launcher();
    void keyPressEvent(QKeyEvent * event);

public slots:
    void start();

private:
    QWidget *gui;
    QGridLayout *layout;
    QScrollArea *scroll_gui;
    std::vector<Menu_Item *> items;
};

#endif // BANNER_LAUNCHER_HPP
