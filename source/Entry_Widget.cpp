#include "Entry_Widget.hpp"
#include "Entry.hpp"

#include <QMouseEvent>
#include <QDebug>
#include <QString>
#include <QMenu>
#include <QAction>
#include <QPixmap>
#include <QMessageBox>

#include <QPainter>
#include <QColor>

Entry_Widget::Entry_Widget(Entry * entry, QWidget *parent) : QLabel(parent) {
    this->entry = entry;
    setText(entry->name());
    if (entry->image_ptr() != NULL)
        setPixmap(QPixmap::fromImage(entry->image_ref()));
    setFixedWidth(banner_width);
    setFixedHeight(banner_height);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(
        this, SIGNAL(customContextMenuRequested(const QPoint &)),
        this, SLOT(ShowContextMenu(const QPoint &))
    );
}

Entry_Widget::~Entry_Widget() { }

void Entry_Widget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        run();
    } else if (event->button() == Qt::RightButton) {
        qDebug() << "Right Button Pressed on \"" << this->entry->name() << '\"';
    }
}

void Entry_Widget::run() {
    QMessageBox::critical(
        this, "Couldn't run program",
        entry->run()
    );
}

void Entry_Widget::draw_frame() {
    if (entry->image_ptr() != NULL) {
        QColor c = Qt::green;
        QImage img(entry->image_ref());
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
    if (entry->image_ptr() != NULL) {
        setPixmap(QPixmap::fromImage(entry->image_ref()));
    }
}

void Entry_Widget::ShowContextMenu(const QPoint &pos) {
    QMenu contextMenu(tr("Context menu"), this);

    QAction add_game("Add Game...", this);
    add_game.setEnabled(false);
    contextMenu.addAction(&add_game);

    contextMenu.exec(mapToGlobal(pos));
}
