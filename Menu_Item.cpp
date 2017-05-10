#include "Menu_Item.hpp"

Menu_Item::Menu_Item(const QFileInfo& dir, QWidget* parent) : QLabel(parent) {
    QString path = dir.absoluteFilePath();
    setText(path);

    QFileInfo image_path(path + "/image");
    if (image_path.exists()) {
        image = new QImage(image_path.absoluteFilePath());
        setPixmap(QPixmap::fromImage(*image));
    }

    setFixedWidth(460);
    setFixedHeight(215);
}

void Menu_Item::mousePressEvent(QMouseEvent* event) {
    emit clicked();
}
