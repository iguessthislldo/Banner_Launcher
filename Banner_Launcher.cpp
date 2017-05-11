#include <QDebug>
#include <QMessageBox>
#include <QTimer>
#include <QSpacerItem>
#include <QKeyEvent>

#include "Banner_Launcher.hpp"
#include "Path.hpp"

const char * APPLICATION_DIRECTORY_NAME = "BannerLauncher";
const unsigned ROW_SIZE = 4;
const unsigned COL_SIZE = 3;

Banner_Launcher::Banner_Launcher(QWidget *parent) : QMainWindow(parent) {
    QTimer::singleShot(0, this, SIGNAL(start()));

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
        Menu_Item::banner_width * COL_SIZE,
        Menu_Item::banner_height * ROW_SIZE
    );

    setCentralWidget(scroll_gui);
}

Banner_Launcher::~Banner_Launcher() {
    delete gui;
    delete layout;
    delete scroll_gui;
    for (Menu_Item * i : items)
        delete i;
}

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

    // Add Menu Items
    Menu_Item * item;
    unsigned row = 0;
    unsigned col = 0;
    for (auto & item_directory : application_directory.subdirectories()) {
        item = new Menu_Item(item_directory);
        if (item->is_valid()) {
            layout->addWidget(item, row, col, 1, 1, Qt::AlignTop);
            items.push_back(item);
            col++;
            if (col == COL_SIZE) {
                col = 0;
                row++;
            }
        item = new Menu_Item(item_directory);
        } else {
            delete item;
        }
    }

    // Try to correct Grid layout
    int insert = ROW_SIZE - row;
    qDebug() << "INSERT: " << insert;
    qDebug() << "col: " << col;
    if (col) insert--;
    for (int i = 0; i < insert; i++) {
        QSpacerItem * spacer = new QSpacerItem(Menu_Item::banner_width, Menu_Item::banner_height);
        layout->addItem(spacer, row + i + 1, col);
    }
}

void Banner_Launcher::keyPressEvent(QKeyEvent * event) {
    if (event->key() == Qt::Key_Escape) {
        this->close();
    }
}
