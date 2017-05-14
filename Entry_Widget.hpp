#ifndef ENTRY_WIDGET_HPP
#define ENTRY_WIDGET_HPP

#include <string>

#include <QLabel>
#include <QDebug>

#include "Path.hpp"

class Entry;

class Entry_Widget : public QLabel
{
    Q_OBJECT

public:
    explicit Entry_Widget(Entry *entry, QWidget *parent = 0);
    ~Entry_Widget();

    /*
     * Steam banner size
     * TODO: make an option
     */
    const static unsigned banner_width = 460;
    const static unsigned banner_height = 215;

signals:
    void clicked();

public slots:
    void run();

protected:
    void mousePressEvent(QMouseEvent* event);

private:
    Entry * entry;
};

#endif // ENTRY_WIDGET_HPP
