#include <errno.h>
#include <error.h>
#include <stdio.h>

#include <unistd.h> // execvp

#include <QDebug>

#include "Entry.hpp"

/*
 * Names of files in each entry's directory
 */
static const char * IMAGE_FILENAME = "image";
static const char * EXECUTABLE_FILENAME = "execute";
static const char * COUNT_FILENAME = "count";

Entry::Entry(const Path & path)
{
    qDebug() << "    " << path.c_str();

    this->path = path;
    this->executable = path / EXECUTABLE_FILENAME;

    // Image
    Path image_path = path / IMAGE_FILENAME;
    if (image_path.exists()) {
        image = new QImage(image_path.c_str());
    } else {
        qDebug() << "    No Image";
    }

    // Count
    Path count_path = path / COUNT_FILENAME;
    if (count_path.exists()) {
        FILE * file = fopen(count_path.c_str(), "r");
        fscanf(file, "%u", &count);
        fclose(file);
    }
    qDebug() << "    Count:" << count;

    // Names
    name = path.get_name();
    for (char c : name) {
        if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == ' ') {
            filter_name.push_back(c);
        } else if (c >= 'a' && c <= 'z') {
            filter_name.push_back((c - 'a') + 'A');
        }
    }
    qDebug() << "    Directory Name: \"" << path.get_name().c_str() << '\"';
    qDebug() << "    Filter Name: \"" << filter_name.c_str() << '\"';

}

Entry::~Entry() {
    if (image != NULL) delete image;
}

std::string Entry::get_name() {
    return this->name;
}

bool Entry::is_valid() {
    return executable.is_file();
}

unsigned Entry::get_count() {
    return count;
}

std::string Entry::get_filter_name() {
    return filter_name;
}

QImage * Entry::get_image() {
    return image;
}

QImage & Entry::get_image_ref() {
    return *image;
}

bool Entry::compare(Entry * a, Entry * b) {
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

std::string Entry::run() {
    Path count_path = path / COUNT_FILENAME;
    FILE * file = fopen(count_path.c_str(), "w");
    fprintf(file, "%u\n", count + 1);
    fclose(file);
    const char * exe_path = executable.c_str();
    execl((const char *) exe_path, (const char *) exe_path, (char *) NULL);
    return std::string("Couldn't run program:\n    \"") + exe_path + "\"\n    " + strerror(errno);
}

Entry_Widget * Entry::get_widget(QWidget *parent) {
    if (widget == NULL) {
        this->widget = new Entry_Widget(this, parent);
    }
    return this->widget;
}
