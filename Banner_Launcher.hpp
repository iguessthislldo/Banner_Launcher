#ifndef BANNER_LAUNCHER_HPP
#define BANNER_LAUNCHER_HPP

#include <vector>

#include <QMainWindow>
#include <QDebug>
#include <QDir>
#include <QString>

#include <QScrollArea>
#include <QVBoxLayout>

#include "Item.hpp"

class Banner_Launcher : public QMainWindow
{
    Q_OBJECT

public:
    Banner_Launcher(QWidget *parent = 0);
    ~Banner_Launcher();

private:
    QWidget *gui;
    QVBoxLayout *layout;
    QScrollArea *scroll_gui;
    std::vector<Item *> items;
};

#endif // BANNER_LAUNCHER_HPP
