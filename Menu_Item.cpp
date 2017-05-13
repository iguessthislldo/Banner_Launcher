#include <errno.h>
#include <error.h>
#include <stdio.h>

#include <unistd.h> // execvp

#include <QDebug>
#include <QString>
#include <QPixmap>
#include <QMessageBox>

#include "Menu_Item.hpp"

/*
 * Names of files in each entry's directory
 */
static const char * IMAGE_FILENAME = "image";
static const char * EXECUTABLE_FILENAME = "execute";
static const char * COUNT_FILENAME = "count";

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

    Path count_path = path / COUNT_FILENAME;
    if (count_path.exists()) {
        FILE * file = fopen(count_path.c_str(), "r");
        fscanf(file, "%u", &count);
        fclose(file);
    }

    name = path.get_name();

    setFixedWidth(banner_width);
    setFixedHeight(banner_height);
}

void Menu_Item::set_name(const std::string & name) {
    this->name = name;
}

std::string Menu_Item::get_name() {
    return this->name;
}

void Menu_Item::mousePressEvent(QMouseEvent* event) {
    run();
}

bool Menu_Item::is_valid() {
    return executable.is_file();
}

unsigned Menu_Item::get_count() {
    return count;
}

void Menu_Item::run() {
    Path count_path = path / COUNT_FILENAME;
    FILE * file = fopen(count_path.c_str(), "w");
    fprintf(file, "%u\n", count+1);
    fclose(file);

    const char * exe_path = executable.c_str();
    qDebug() << execl((const char *) exe_path, (const char *) exe_path, (char *) NULL);
    QMessageBox::critical(
        this, "Couldn't run program",
        QString("Couldn't run program:\n    \"") + exe_path + "\"\n    " + strerror(errno)
    );
}

bool Menu_Item::compare(Menu_Item * a, Menu_Item * b) {
    if (a->get_count() < b->get_count())
        return false;

    if (a->get_count() > b->get_count())
        return true;

    std::string a_lower = a->get_name();
    for (size_t i = 0; i < a_lower.size(); i++)
        a_lower[i] = std::tolower(a_lower[i]);

    std::string b_lower = b->get_name();
    for (size_t i = 0; i < b_lower.size(); i++)
        b_lower[i] = std::tolower(b_lower[i]);

    return a_lower < b_lower;
}
