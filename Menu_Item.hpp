#ifndef MENU_ITEM_HPP
#define MENU_ITEM_HPP

#include <unistd.h> // execvp

#include <QDebug>
#include <QString>
#include <QFileInfo>

#include <QLabel>
#include <QPixmap>
#include <QImage>

#include "Path.hpp"

class Menu_Item : public QLabel
{
    Q_OBJECT

public:
    explicit Menu_Item(const Path & path, QWidget* parent=0);
    ~Menu_Item() {
       if (image != NULL) delete image;
    }

    bool is_valid();

    const static unsigned banner_width = 460;
    const static unsigned banner_height = 215;

signals:
    void clicked();

public slots:
    void run();

protected:
    void mousePressEvent(QMouseEvent* event);

private:
    Path path, executable;
    QImage * image = NULL;

};

#endif // MENU_ITEM_HPP
