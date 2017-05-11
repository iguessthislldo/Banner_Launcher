#include <errno.h>
#include <error.h>
#include <string>

#include <QMessageBox>

#include "Menu_Item.hpp"

static const char * IMAGE_FILENAME = "image";
static const char * EXECUTABLE_FILENAME = "execute";

Menu_Item::Menu_Item(const Path & path, QWidget* parent) : QLabel(parent) {
    qDebug() << "    " << path.c_str();
    this->path = path;
    this->executable = path / EXECUTABLE_FILENAME;
    setText(path.c_str());
    Path image_path = path / IMAGE_FILENAME;
    if (image_path.exists()) {
        image = new QImage(image_path.c_str());
        setPixmap(QPixmap::fromImage(*image));
    } else {
        qDebug() << "    No Image";
    }

    setFixedWidth(banner_width);
    setFixedHeight(banner_height);
}

void Menu_Item::mousePressEvent(QMouseEvent* event) {
    //emit clicked();
    run();
}

bool Menu_Item::is_valid() {
    return executable.is_file();
}

void Menu_Item::run() {
    const char * exe_path = executable.c_str();
    qDebug() << execl((const char *) exe_path, (const char *) exe_path, (char *) NULL);
    QMessageBox::critical(
        this, "Couldn't run program",
        QString("Couldn't run program:\n    \"") + exe_path + "\"\n    " + strerror(errno)
    );
}
