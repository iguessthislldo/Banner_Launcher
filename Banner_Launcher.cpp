#include <string>
#include <algorithm>

#include <QDebug>
#include <QMessageBox>
#include <QTimer>
#include <QSpacerItem>
#include <QKeyEvent>

#include "Banner_Launcher.hpp"
#include "Path.hpp"

const char * APPLICATION_DIRECTORY_NAME = "BannerLauncher";

Banner_Launcher::Banner_Launcher(float window_height_in_rows, unsigned no_columns, QWidget *parent) : QMainWindow(parent) {
    QTimer::singleShot(0, this, SIGNAL(start()));

    this->no_columns = no_columns;

    gui = new QWidget(this);
    layout = new QGridLayout(gui);
    layout->setSpacing(0);
    layout->setMargin(0);
    gui->setLayout(layout);
    scroll_gui = new QScrollArea;
    scroll_gui->setStyleSheet("background-color: #383838;");
    scroll_gui->setWidgetResizable(true);
    scroll_gui->setWidget(gui);
    scroll_gui->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll_gui->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setFixedSize(
        Menu_Item::banner_width * no_columns,
        Menu_Item::banner_height * window_height_in_rows
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
    Menu_Item * item;
    for (auto & item_directory : application_directory.subdirectories()) {
        item = new Menu_Item(item_directory);
        if (item->is_valid()) {
            all_items.push_back(item);
        } else {
            delete item;
        }
    }

    all_items.sort(Menu_Item::compare);

    set_displayed_items(all_items);
}

void Banner_Launcher::keyPressEvent(QKeyEvent * event) {
    if (event->key() == Qt::Key_Escape) {
        this->close();
    } else if (event->key() == Qt::Key_F) {
        displayed_items = all_items;
        displayed_items.reverse();
        set_displayed_items(displayed_items);
    } else {
        set_displayed_items(all_items);
    }
}

void Banner_Launcher::set_displayed_items(const std::list<Menu_Item *> & items) {
    // Removed all the items in layout
    QLayoutItem * layout_item;
    while((layout_item = layout->takeAt(0)))
        delete layout_item;

    // Add items to layout
    unsigned row = 0;
    unsigned col = 0;
    for (auto item : items) {
        layout->addWidget(item, row, col, 1, 1, Qt::AlignTop);
        col++;
        if (col == no_columns) {
            col = 0;
            row++;
        }
    }

    // Try to correct Grid layout
    int insert = 4 - row;
    if (col) insert--;
    for (int i = 0; i < insert; i++) {
        QSpacerItem * spacer = new QSpacerItem(Menu_Item::banner_width, Menu_Item::banner_height);
        layout->addItem(spacer, row + i + 1, col);
    }
}
