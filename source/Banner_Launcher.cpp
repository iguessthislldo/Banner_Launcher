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

Path Banner_Launcher::application_directory = Path();

Banner_Launcher::Banner_Launcher(float window_height_in_rows, unsigned no_columns, QWidget *parent) : QMainWindow(parent) {
    QTimer::singleShot(0, this, SIGNAL(start()));

    this->no_columns = no_columns;

    gui = new QWidget(this);
    scroll_gui = new QScrollArea;
    scroll_gui->setStyleSheet("background-color: #383838;");
    //scroll_gui->setWidgetResizable(true);
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

    Path xdg_config_home = Path::xdg_config_home();
    qDebug() << xdg_config_home.path().c_str();
    if (!xdg_config_home.exists()) {
        QMessageBox::critical(
            this, "No XDG_CONFIG_HOME",
            "XDG_CONFIG_HOME (usually ~/.config) does not exist."
        );
        this->close();
    }

    application_directory = xdg_config_home / APPLICATION_DIRECTORY_NAME;
    qDebug() << application_directory.path().c_str();
    if (!application_directory.exists()) {
        qDebug() << "    Doesn't Exist, creating...";
        application_directory.make_directory();
    }

    font.setPointSize(16);
    search_text = new QLabel(this);
    search_text->move(10, 10);
    search_text->setText("");
    search_text->setMaximumWidth(0);
    search_text->setFont(font);
    search_text->setStyleSheet("QLabel {"
        "background-color: #383838;"
        "color: #00ff00;"
    "}");
    search_text->hide();
}

Banner_Launcher::~Banner_Launcher() { }

void Banner_Launcher::start() {
    // Get Menu Items
    Entry * entry;
    for (auto & directory : application_directory.subdirectories()) {
        entry = new Entry(directory);
        if (entry->is_valid()) {
            all_entries.push_back(entry);
            widgets.push_back(entry->get_widget(gui));
        } else {
            delete entry;
        }
    }

    set_displayed_entries(all_entries);
}

Path Banner_Launcher::get_application_directory() {
    return application_directory;
}

void Banner_Launcher::keyPressEvent(QKeyEvent * event) {
    auto key = event->key();
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
                search_text->hide();
                search_text->setText("");
                set_displayed_entries(all_entries);
            } else {
                search_text->setText(QString::fromStdString(filter));
                int size = search_text->fontMetrics().width(QString::fromStdString(filter));
                search_text->setMaximumWidth(size);
                search_text->setMinimumWidth(size);
                update_filter();
            }
        }
    // Add character to filter / search
    } else if ((key >= Qt::Key_A && key <= Qt::Key_Z) || (key >= Qt::Key_0 && key <= Qt::Key_9) || key == Qt::Key_Space) {
            char c = (char) event->key();
            filter.push_back(c);
            search_text->setMinimumWidth(search_text->fontMetrics().width(QString::fromStdString(filter)));
            search_text->show();
            search_text->setText(QString::fromStdString(filter));
            std::cerr << filter << std::endl;
            std::cerr.flush();
            update_filter();
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

    if (displayed_entries.size())
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

    gui->resize(
        Entry_Widget::banner_width * no_columns,
        Entry_Widget::banner_height * (
            displayed_entries.size() / no_columns + (displayed_entries.size() % no_columns ? 1 : 0)
        )
    );
}

void Banner_Launcher::ShowContextMenu(const QPoint &pos) {
    QMenu contextMenu("Context menu", this);

    QAction add_game("Add Game...", this);
    contextMenu.addAction(&add_game);

    QAction add_steam_games("Add Steam Games...", this);
    connect(&add_steam_games, SIGNAL(triggered()), this, SLOT(show_steam_dialog()));
    contextMenu.addAction(&add_steam_games);

    contextMenu.exec(mapToGlobal(pos));
}

void Banner_Launcher::show_steam_dialog() {
    Steam_Dialog * steam_dialog = new Steam_Dialog(this);
    if (steam_dialog->exec()) {
        for (auto i : children())
            i->deleteLater();
        start();
    }
}
