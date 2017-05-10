#ifndef MENU_ITEM_HPP
#define MENU_ITEM_HPP

#include <unistd.h> // execvp

#include <QDebug>
#include <QString>
#include <QFileInfo>

#include <QLabel>
#include <QPixmap>
#include <QImage>

class Menu_Item : public QLabel
{
    Q_OBJECT

public:
    explicit Menu_Item(const QFileInfo& dir, QWidget* parent=0);
   ~Menu_Item() {
       if (image != NULL) delete image;
   }

signals:
   void clicked();

protected:
   void mousePressEvent(QMouseEvent* event);

private:
    QFileInfo dir;
    QImage * image = NULL;

};

#endif // MENU_ITEM_HPP
