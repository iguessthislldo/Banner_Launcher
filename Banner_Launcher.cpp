#include <QApplication>
#include <QMessageBox>
#include <QTimer>

#include "Banner_Launcher.hpp"
#include "Path.hpp"

const char * APPLICATION_DIRECTORY_NAME = "BannerLauncher";

Banner_Launcher::Banner_Launcher(QWidget *parent) : QMainWindow(parent) {
    QTimer::singleShot(0, this, SIGNAL(start()));

    gui = new QWidget(this);
    layout = new QVBoxLayout(gui);
    gui->setLayout(layout);
    scroll_gui = new QScrollArea;
    scroll_gui->setBackgroundRole(QPalette::Dark);
    scroll_gui->setWidgetResizable(true);
    scroll_gui->setWidget(gui);

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
    for (auto & item_directory : application_directory.subdirectories()) {
        item = new Menu_Item(item_directory);
        if (item->is_valid()) {
            layout->addWidget(item);
            items.push_back(item);
        } else {
            delete item;
        }
    }
}
