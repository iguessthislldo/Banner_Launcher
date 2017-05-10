#ifndef ITEM_HPP
#define ITEM_HPP

#include <QDebug>
#include <QString>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QFileInfo>

class Item {
public:
    Item(const QFileInfo & directory);
    ~Item();
    QString get_path() {return path;}
    QLabel * get_gui() {return gui;}

private:
    QFileInfo dir;
    QString path;
    QImage * image;
    QLabel * gui;
    bool has_image = false;
};

#endif // ITEM_HPP
