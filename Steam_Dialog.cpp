#include <stdio.h>

#include <QStringList>
#include <QDebug>
#include <QTableWidgetSelectionRange>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>

#include "Path.hpp"
#include "Steam_Dialog.hpp"
#include "util.hpp"
#include "Banner_Launcher.hpp"

Steam_Dialog::Steam_Dialog(QWidget *parent) : QDialog(parent) {
    setMinimumWidth(420);
    setMinimumHeight(620);
    setSizeGripEnabled(true);

    layout = new QVBoxLayout(this);
    setLayout(layout);

    layout->addWidget(new QLabel("Select installed Steam games to add", this));

    table_widget = new QTableWidget(this);
    table_widget->setRowCount(0);
    QStringList table_labels;
    table_labels << "Add?" << "AppID" << "Name";
    table_widget->setColumnCount(table_labels.size());
    table_widget->setColumnWidth(0, 35);
    table_widget->setColumnWidth(1, 75);
    table_widget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    table_widget->verticalHeader()->hide();
    table_widget->setHorizontalHeaderLabels(table_labels);
    table_widget->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_widget->setSelectionMode(QAbstractItemView::NoSelection);
    layout->addWidget(table_widget);

    QPushButton * ok = new QPushButton("OK", this);
    connect(ok, SIGNAL(clicked(bool)), this, SLOT(accept()));
    layout->addWidget(ok);


    get_installed_steam_apps();
}

Steam_Dialog::~Steam_Dialog() {
    for (QTableWidgetItem * i : table_items)
        delete i;
}

void Steam_Dialog::add_game(const std::string &name, const std::string &id) {
    unsigned rows = table_widget->rowCount();
    table_widget->insertRow(rows);

    QTableWidgetItem * check_item = new QTableWidgetItem();
    check_item->setCheckState(Qt::Checked);
    check_item->setFlags(check_item->flags() & ~Qt::ItemIsEditable);
    table_widget->setItem(rows, 0, check_item);
    table_items.push_back(check_item);

    QTableWidgetItem * id_item = new QTableWidgetItem(id.c_str());
    id_item->setFlags(check_item->flags() & ~Qt::ItemIsEditable);
    table_widget->setItem(rows, 1, id_item);
    table_items.push_back(id_item);

    QTableWidgetItem * name_item = new QTableWidgetItem(name.c_str());
    table_widget->setItem(rows, 2, name_item);
    table_items.push_back(name_item);
}

void Steam_Dialog::get_installed_steam_apps() {
    Path steam_apps = Path::home(); // TODO: Double free if these two lines are brought together,
    steam_apps = steam_apps / STEAMAPPS_PATH; // at least for me at time of writing...
    std::string filename;
    for (auto & i : steam_apps.contents(Path::FILE)) {
        filename = i.get_name();
        if(starts_with(filename, APPMANIFEST)) { // Then Parse the app manifest file
            /*
             * Scans the file for quoted strings.
             * When a quoted string matched the app name and id keys,
             * skip to the next quotation mark and copy the chars into the
             * corresponding string. When both strings are captured, exit the loop.
             */
            FILE * file = fopen(i.c_str(), "r");
            if (file == NULL) {
                fprintf(stderr, "Couldn't open app manifest %s\n", i.c_str());
                continue;
            }

            enum State_Enum {
                IGNORE, // Skip to next "
                WORD, // Add character to word string if not "
                IGNORE_TO_NAME, // Skip to next "
                IGNORE_TO_ID, // Skip to next "
                NAME, // Add character to name string if not "
                ID // add character to id string if not "
            };
            State_Enum state = IGNORE;

            char c;
            std::string word, id, name;
            bool got_name = false;
            bool got_id = false;
            bool loop = true;

            while (loop && (c = fgetc(file)) != EOF) {
                switch (state) {
                case IGNORE:
                    if (c == '"')
                        state = WORD;
                    continue;
                case WORD:
                    if (c == '"') {
                        if (!got_name && word == NAME_STRING) {
                            state = IGNORE_TO_NAME;
                        } else if (!got_id && word == APPID_STRING) {
                            state = IGNORE_TO_ID;
                        } else {
                            state = IGNORE;
                        }
                        word = "";
                    } else word.push_back(c);
                    continue;
                case IGNORE_TO_ID:
                    if (c == '"')
                        state = ID;
                    continue;
                case ID:
                    if (c == '"') {
                        state = IGNORE;
                        got_id = true;
                        if (got_name)
                            loop = false;
                    } else id.push_back(c);
                    continue;
                case IGNORE_TO_NAME:
                    if (c == '"')
                        state = NAME;
                    continue;
                case NAME:
                    if (c == '"') {
                        state = IGNORE;
                        got_name = true;
                        if (got_id)
                            loop = false;
                    } else name.push_back(c);
                    continue;
                }
            }
            qDebug() << id.c_str() << ":" << name.c_str();

            fclose(file);

            if (id.size() && name.size())
                add_game(name, id);
        }
    }
}

int Steam_Dialog::get_steam_banner(const std::string & id, const Path & destination) {
    std::string url = std::string(STEAM_HEADER_URL_HEAD) + id + STEAM_HEADER_URL_TAIL;
    return download(url.c_str(), destination.c_str());
}

void Steam_Dialog::disable_row(unsigned row) {
    QTableWidgetItem * check_item = table_widget->item(row, 0);
    check_item->setFlags(check_item->flags() & ~Qt::ItemIsEnabled);

    QTableWidgetItem * id_item = table_widget->item(row, 1);
    id_item->setFlags(id_item->flags() & ~Qt::ItemIsEnabled);

    QTableWidgetItem * name_item = table_widget->item(row, 2);
    name_item->setFlags(name_item->flags() & ~Qt::ItemIsEnabled);
}

void Steam_Dialog::enable_row(unsigned row) {
    QTableWidgetItem * check_item = table_widget->item(row, 0);
    check_item->setFlags(check_item->flags() & Qt::ItemIsEnabled);

    QTableWidgetItem * id_item = table_widget->item(row, 1);
    id_item->setFlags(id_item->flags() & Qt::ItemIsEnabled);

    QTableWidgetItem * name_item = table_widget->item(row, 2);
    name_item->setFlags(name_item->flags() & Qt::ItemIsEnabled);
}

void Steam_Dialog::accept() {
    for (int row = 0; row < table_widget->rowCount(); row++) {
        QTableWidgetItem * check_item = table_widget->item(row, 0);
        if (check_item->checkState() == Qt::Checked) {
            QTableWidgetItem * name_item = table_widget->item(row, 2);
            Path path = Banner_Launcher::get_application_directory() / name_item->text().toStdString();
            path.make_directory();

            QTableWidgetItem * id_item = table_widget->item(row, 1);
            Path execute = path / "execute";
            FILE * file = fopen(execute.c_str(), "w");
            fprintf(file, "#!/bin/bash\nexec /bin/sh -c '\"$HOME/.local/share/Steam/steam.sh\" \"steam://run/%s\"\'\n", id_item->text().toStdString().c_str());
            fclose(file);
            execute.set_executable();
        }
    }
    close();
}
