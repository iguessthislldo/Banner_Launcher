#include "Entry_Widget.hpp"
#include "Entry.hpp"

#include <QMouseEvent>
#include <QDebug>
#include <QString>
#include <QPixmap>
#include <QMessageBox>

#include <QPainter>
#include <QColor>

Entry_Widget::Entry_Widget(Entry * entry, QWidget *parent) : QLabel(parent) {
    this->entry = entry;
    setText(entry->get_name().c_str());
    if (entry->get_image() != NULL)
        setPixmap(QPixmap::fromImage(entry->get_image_ref()));
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

void Entry_Widget::draw_frame() {
    if (entry->get_image() != NULL) {
        QColor c = Qt::green;
        QImage img(entry->get_image_ref());
        QPainter p(&img);
        p.setPen(QPen(c));
        p.setBrush(c);
        p.drawRect(0, 0, banner_width, 2);
        p.drawRect(0, banner_height - 2, banner_width, 2);
        p.drawRect(0, 0, 2, banner_width);
        p.drawRect(banner_width - 2, 0, 2, banner_width);
        setPixmap(QPixmap::fromImage(img));
    }
}

void Entry_Widget::draw_wo_frame() {
    if (entry->get_image() != NULL) {
        setPixmap(QPixmap::fromImage(entry->get_image_ref()));
    }
}
