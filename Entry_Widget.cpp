#include "Entry_Widget.hpp"
#include "Entry.hpp"

#include <QMouseEvent>
#include <QDebug>
#include <QString>
#include <QPixmap>
#include <QMessageBox>

Entry_Widget::Entry_Widget(Entry * entry, QWidget *parent) : QLabel(parent) {
    this->entry = entry;
    setText(entry->get_name().c_str());
    setPixmap(QPixmap::fromImage(entry->get_image()));
    setFixedWidth(banner_width);
    setFixedHeight(banner_height);
}

Entry_Widget::~Entry_Widget() { }

void Entry_Widget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        run();
    } else if (event->button() == Qt::RightButton) {
        qDebug() << "Right Button Pressed on \"" << this->entry->get_name().c_str() << '\"';
    }
}

void Entry_Widget::run() {
    QMessageBox::critical(
        this, "Couldn't run program",
        QString(entry->run().c_str())
    );
}
