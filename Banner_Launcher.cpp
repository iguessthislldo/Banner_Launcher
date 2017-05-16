#include <iostream>
#include <string>

#include <QDebug>
#include <QMessageBox>
#include <QTimer>
#include <QSpacerItem>
#include <QKeyEvent>
#include <QMenu>
#include <QAction>

#include "Banner_Launcher.hpp"
#include "Path.hpp"
#include "Entry_Widget.hpp"

#include "Steam_Dialog.hpp"

const char * APPLICATION_DIRECTORY_NAME = "BannerLauncher";

Banner_Launcher::Banner_Launcher(float window_height_in_rows, unsigned no_columns, QWidget *parent) : QMainWindow(parent) {
    QTimer::singleShot(0, this, SIGNAL(start()));

    this->no_columns = no_columns;

    gui = new QWidget(this);
    scroll_gui = new QScrollArea;
    scroll_gui->setStyleSheet("background-color: #383838;");
    scroll_gui->setWidgetResizable(true);
    scroll_gui->setWidget(gui);
    scroll_gui->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll_gui->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setFixedSize(
        Entry_Widget::banner_width * no_columns,
        Entry_Widget::banner_height * window_height_in_rows
    );

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(
        this, SIGNAL(customContextMenuRequested(const QPoint &)),
        this, SLOT(ShowContextMenu(const QPoint &))
    );

    setCentralWidget(scroll_gui);
}

Banner_Launcher::~Banner_Launcher() { }

void Banner_Launcher::start() {
    Path xdg_config_home = Path::xdg_config_home();
    qDebug() << xdg_config_home.path().c_str();
    if (!xdg_config_home.exists()) {
        QMessageBox::critical(
            this, "No XDG_CONFIG_HOME",
            "XDG_CONFIG_HOME (usually ~/.config) does not exist."
        );
        this->close();
    }

    Path application_directory = xdg_config_home / APPLICATION_DIRECTORY_NAME;
    qDebug() << application_directory.path().c_str();
    if (!application_directory.exists()) {
        qDebug() << "    Doesn't Exist, creating...";
        application_directory.ensure_directory();
    }

    // Get Menu Items
    Entry * entry;
    for (auto & directory : application_directory.subdirectories()) {
        entry = new Entry(directory);
        if (entry->is_valid()) {
            all_entries.push_back(entry);
            widgets.push_back(entry->get_widget(this));
        } else {
            delete entry;
        }
    }

    set_displayed_entries(all_entries);
}

void Banner_Launcher::keyPressEvent(QKeyEvent * event) {
    if (event->key() == Qt::Key_Escape) { // Close
        this->close();
    } else if (event->key() == Qt::Key_Return) { // Enter Key
        if (displayed_entries.size())
            displayed_entries.front()->run();
    } else if (event->key() == Qt::Key_Backspace) {
        unsigned size = filter.size();
        if (size) {
            filter.pop_back();
            if (size == 1) {
                set_displayed_entries(all_entries);
            } else {
                update_filter();
            }
        }
    } else { // Add character to filter / search
        char c = (char) event->key();
        if (c >= 'a' && c <= 'z')
            c = (c - 'a') + 'A';
        if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == ' ') {
            filter.push_back(c);
            std::cerr << filter << std::endl;
            std::cerr.flush();
            update_filter();
        }
    }
}

void Banner_Launcher::update_filter() {
    std::list<Entry *> new_entries;
    for (auto i : all_entries) {
        if (!i->get_filter_name().compare(0, filter.size(), filter)) {
            new_entries.push_front(i);
        }
    }
    set_displayed_entries(new_entries);
}

void Banner_Launcher::set_displayed_entries(const std::list<Entry *> & entries) {
    displayed_entries = entries;
    displayed_entries.sort(Entry::compare);

    for (auto entry : all_entries) {
        entry->get_widget()->draw_wo_frame();
        entry->get_widget()->hide();
    }

    for (auto entry : entries) {
        entry->get_widget()->show();
    }

    if(displayed_entries.size())
        displayed_entries.front()->get_widget()->draw_frame();

    // Add items to layout
    unsigned row = 0;
    unsigned col = 0;
    for (auto entry : displayed_entries) {
        entry->get_widget()->move(Entry_Widget::banner_width * col, Entry_Widget::banner_height * row);
        col++;
        if (col == no_columns) {
            col = 0;
            row++;
        }
    }
}

void Banner_Launcher::ShowContextMenu(const QPoint &pos) {
    QMenu contextMenu(tr("Context menu"), this);

    QAction action1("Add Steam Games...", this);
    connect(&action1, SIGNAL(triggered()), this, SLOT(show_steam_dialog()));
    contextMenu.addAction(&action1);

    contextMenu.exec(mapToGlobal(pos));
}

void Banner_Launcher::show_steam_dialog() {
    Steam_Dialog * steam_dialog = new Steam_Dialog(this);
    steam_dialog->exec();
}
