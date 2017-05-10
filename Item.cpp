#include "Item.hpp"

Item::Item(const QFileInfo & directory) {
    this->path = directory.absoluteFilePath();

    gui = new QLabel("TEST");

    QFileInfo image_path(path + "/image");
    if (image_path.exists()) {
        qDebug() << "        " << image_path.absoluteFilePath();
        image = new QImage(image_path.absoluteFilePath());
        gui->setPixmap(QPixmap::fromImage(*image));
        has_image = true;
    }

    gui->setFixedWidth(460);
    gui->setFixedHeight(215);
}

Item::~Item() {
    delete gui;
    if (has_image) delete image;
}
