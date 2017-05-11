#ifndef BANNER_LAUNCHER_HPP
#define BANNER_LAUNCHER_HPP

#include <vector>

#include <QMainWindow>
#include <QDebug>
#include <QDir>
#include <QString>

#include <QScrollArea>
#include <QVBoxLayout>

#include "Menu_Item.hpp"
#include "Path.hpp"

class Banner_Launcher : public QMainWindow
{
    Q_OBJECT

public:
    Banner_Launcher(QWidget *parent = 0);
    ~Banner_Launcher();

public slots:
    void start();

private:
    QWidget *gui;
    QVBoxLayout *layout;
    QScrollArea *scroll_gui;
    std::vector<Menu_Item *> items;
};

#endif // BANNER_LAUNCHER_HPP
