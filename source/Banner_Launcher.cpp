#include <iostream>
#include <string>

#include <QDebug>
#include <QMessageBox>
#include <QTimer>
#include <QSpacerItem>
#include <QKeyEvent>
#include <QMenu>
#include <QAction>
#include <QJsonDocument>

#include "Banner_Launcher.hpp"
#include "Path.hpp"
#include "Entry_Widget.hpp"

#include "Steam_Dialog.hpp"

const char * APPLICATION_DIRECTORY_NAME = "BannerLauncher";
const char * CONFIG_FILE_NAME = "config.json";
const char * HEADER_DIRECTORY_NAME = "headers";
const char * DEFAULT_BG_COLOR = "#383838";
const char * DEFAULT_FG_COLOR = "#00ff00";
const char * DEFAULT_STEAM_PATH = "~/.local/share/Steam";

Banner_Launcher * application = NULL;

Banner_Launcher::Banner_Launcher(float window_height_in_rows, unsigned no_columns, QWidget *parent) : QMainWindow(parent) {
    // Populate Entry widgets which requires Qt to be fully working
    QTimer::singleShot(0, this, SIGNAL(start()));

    // Set application global variable
    application = this;

    // Entry GUI
    gui = new QWidget(this);
    scroll_gui = new QScrollArea;
    scroll_gui->setWidget(gui);
    scroll_gui->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll_gui->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setCentralWidget(scroll_gui);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(
        this, SIGNAL(customContextMenuRequested(const QPoint &)),
        this, SLOT(ShowContextMenu(const QPoint &))
    );

    // Data
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
    bool new_config = !application_directory.exists();
    if (new_config) {
        qDebug() << "    Doesn't Exist, creating...";
        application_directory.make_directory();
    }

    header_directory = application_directory / HEADER_DIRECTORY_NAME;

    config_file = application_directory / CONFIG_FILE_NAME;
    if (new_config) {
        bg_color = DEFAULT_BG_COLOR;
        fg_color = DEFAULT_FG_COLOR;
        steam_directory = Path(DEFAULT_STEAM_PATH, true);
        save();
    } else {
        load();
    }

    // Continue GUI
    setStyleSheet(QString("* {"
        "background-color: ") + bg_color + ";"
        "color: " + fg_color + ";"
    "}");

    this->no_columns = no_columns;
    setFixedSize(
        Entry_Widget::banner_width * no_columns,
        Entry_Widget::banner_height * window_height_in_rows
    );

    // Labels

    font.setPointSize(16);

    // "No Entries" Label
    no_entires_label = new QLabel(this);
    no_entires_label->setFont(font);
    //no_entires_label->hide();
    QString no_entries_string = "No entries found, maybe add some?";
    no_entires_label->setText(no_entries_string);
    int no_entries_width = no_entires_label->fontMetrics().width(no_entries_string);
    QPoint point = this->rect().center();
    point.setX(point.x() - no_entries_width/2);
    no_entires_label->setMinimumWidth(no_entries_width);
    no_entires_label->move(point);

    // Filter/Search Label
    filter_label = new QLabel(this);
    filter_label->move(this->rect().center());
    filter_label->setText("");
    filter_label->setMaximumWidth(0);
    filter_label->setFont(font);
    filter_label->hide();
}

Banner_Launcher::~Banner_Launcher() { }

void Banner_Launcher::start() {
    set_displayed_entries(all_entries, false);
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
            filter.chop(1);
            if (size == 1) {
                filter_label->hide();
                filter_label->setText("");
                set_displayed_entries(all_entries, true);
            } else {
                filter_label->setText(filter);
                int size = filter_label->fontMetrics().width(filter);
                filter_label->setMaximumWidth(size);
                filter_label->setMinimumWidth(size);
                update_filter();
            }
        }
    // Add character to filter / search
    } else if ((key >= Qt::Key_A && key <= Qt::Key_Z) || (key >= Qt::Key_0 && key <= Qt::Key_9) || key == Qt::Key_Space) {
        if (all_entries.size()) {
            char c = (char) event->key();
            filter.push_back(c);
            filter_label->setMinimumWidth(filter_label->fontMetrics().width(filter));
            filter_label->show();
            filter_label->setText(filter);
            update_filter();
        }
    }
}

void Banner_Launcher::update_filter() {
    std::list<Entry *> new_entries;
    for (auto i : all_entries) {
        if (!i->filter_name().compare(filter)) {
            new_entries.push_front(i);
        }
    }
    set_displayed_entries(new_entries, true);
}

void Banner_Launcher::set_displayed_entries(const std::list<Entry *> & entries, bool is_filtered) {
    displayed_entries = entries;
    displayed_entries.sort(Entry::compare);

    for (auto entry : all_entries) {
        entry->get_widget()->draw_wo_frame();
        entry->get_widget()->hide();
    }

    if (entries.size()) {
        no_entires_label->hide();
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
    } else if (!is_filtered) {
        no_entires_label->show();
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

void Banner_Launcher::load() {
    qDebug() << "LOAD:" << config_file.c_str();
    // Read File
    QFile f(config_file.c_str());
    f.open(QIODevice::ReadOnly);
    QByteArray a = f.readAll();
    f.close();

    // Default Values
    QJsonObject entries;

    // Parse first level JSON
    QJsonDocument d = QJsonDocument::fromJson(a);
    if (d.isNull()) {
        qDebug() << "Invalid Config";
        return;
    }
    QJsonObject o = d.object();
    qDebug() << o;
    for (const auto & i : o.keys()) {
        if (i == "bg_color") {
            bg_color = o[i].toString();
        } else if (i == "fg_color") {
            fg_color = o[i].toString();
        } else if (i == "steam_path") {
            steam_directory = Path(o[i].toString().toStdString());
        } else if (i == "next_id"){
            next_id = o[i].toInt();
        } else if (i == "entries"){
            entries = o[i].toObject();
        }
    }

    Entry * entry;
    QJsonObject entry_json;
    for (const auto & entry_id : entries.keys()) {
        entry_json = entries.take(entry_id).toObject();
        entry = new Entry(entry_id, entry_json);
        if (entry->is_valid()) {
            all_entries.push_back(entry);
            widgets.push_back(entry->get_widget(gui));
        } else {
            delete entry;
        }
    }
}

void Banner_Launcher::save() {

}

Path Banner_Launcher::get_application_directory() {
    return application_directory;
}

Path Banner_Launcher::get_header_directory() {
    return header_directory;
}
