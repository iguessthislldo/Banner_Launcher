#include <errno.h>
#include <error.h>
#include <stdio.h>

#include <unistd.h> // execvp

#include <QDebug>

#include "Banner_Launcher.hpp"
#include "Entry.hpp"

Entry::Entry(unsigned id) {
    _id = QString::number(id);
}

Entry::Entry(const QString & id, const QJsonObject & json) {
    _id = id;
    for (const auto & key : json.keys()) {
        if (key == "name") {
            qDebug() << json[key].toString();
            name(json[key].toString());
        } else if (key == "steam_id") {
            _steam_id = json[key].toString();
        } else if (key == "image") {
            _image_path = json[key].toString();
            load_image();
        } else if (key == "execute") {
            _execute = json[key].toString();
        } else if (key == "count") {
            _count = json[key].toVariant().toUInt();
        } else if (key == "cd") {
            _cd = json[key].toString();
        }
    }
}

Entry::~Entry() {
    if (_image != NULL) delete _image;
}

QString Entry::id() {
    return _id;
}

QString Entry::name() {
    return _name;
}

void Entry::name(const QString & value) {
    _name = value;
    _filter_name.clear();
    for (char c : value.toStdString()) {
        if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == ' ') {
            _filter_name.push_back(c);
        } else if (c >= 'a' && c <= 'z') {
            _filter_name.push_back((c - 'a') + 'A');
        }
    }
}

QString Entry::filter_name() {
    return _filter_name;
}

QString Entry::steam_id() {
    return _steam_id;
}

void Entry::steam_id(const QString & value) {
    _steam_id = value;
}

unsigned Entry::count() {
    return _count;
}

QString Entry::image_path() {
    return _image_path;
}

void Entry::image_path(const QString & value) {
    if (_image != NULL) {
        delete _image;
        _image = NULL;
    }
    _image_path = value;
}

void Entry::load_image() {
    if (_image_path.size() && _image == NULL) {
        Path img_path = application->get_header_directory() / _image_path.toStdString();
        qDebug() << "Loading Image:" << img_path.c_str();
        if (img_path.exists()) {
            _image = new QImage(img_path.c_str());
            // Rescale image becuase it might not be 460x215
            *_image = _image->scaled(Entry_Widget::banner_width, Entry_Widget::banner_height);
        } else {
            qDebug() << "    Image doesn't exist";
        }
    }
}

QImage * Entry::image_ptr() {
    return _image;
}

QImage & Entry::image_ref() {
    return *_image;
}

bool Entry::is_valid() {
    if (!_filter_name.size()) {
        qDebug() << "Invalid Name";
        return false;
    }
    if (_image == NULL) {
        qDebug() << "Invalid Image";
        return false;
    }
    if (!(_steam_id.size() || _execute.size())) {
        qDebug() << "Must have steam_id or command";
        return false;
    }
    return true;
}

bool Entry::compare(Entry * a, Entry * b) {
    if (a->count() < b->count())
        return false;

    if (a->count() > b->count())
        return true;

    std::string a_lower = a->name().toStdString();
    for (size_t i = 0; i < a_lower.size(); i++)
        a_lower[i] = std::tolower(a_lower[i]);

    std::string b_lower = b->name().toStdString();
    for (size_t i = 0; i < b_lower.size(); i++)
        b_lower[i] = std::tolower(b_lower[i]);

    return a_lower < b_lower;
}

QString Entry::run() {
    /*
    _count++;
    application->save();

    if (_steam_id.size()) { // Run Steam App

    } else { // Run Executable

    }
    const char * exe_path = executable.c_str();
    execl((const char *) exe_path, (const char *) exe_path, (char *) NULL);
    _count--;
    application->save();
    return QString("Couldn't run program:\n    \"") + exe_path + "\"\n    " + strerror(errno);
    */
    return "?";
}

void Entry::set_parent(QWidget *parent) {
    if (widget == NULL) {
        this->widget = new Entry_Widget(this, parent);
    }
}

Entry_Widget * Entry::get_widget() {
    return this->widget;
}

QJsonObject Entry::toJSON() {
    QJsonObject o;
    o["name"] = _name;
    o["count"] = QString::number(_count);
    o["image"] = _image_path;
    if (_steam_id.size()) {
        o["steam_id"] = _steam_id;
    } else {
        o["execute"] = _execute;
        if (_cd.size())
            o["cd"] = _cd;
    }
    return o;
}
