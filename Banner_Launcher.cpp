#include "Banner_Launcher.hpp"

Banner_Launcher::Banner_Launcher(QWidget *parent)
    : QMainWindow(parent)
{
    gui = new QWidget(this);
    layout = new QVBoxLayout(gui);
    gui->setLayout(layout);
    scroll_gui = new QScrollArea;
    scroll_gui->setBackgroundRole(QPalette::Dark);
    scroll_gui->setWidgetResizable(true);
    scroll_gui->setWidget(gui);

    char * xdg_home = getenv("XDG_CONFIG_HOME");
    bool free_xdg_home = false;
    if (xdg_home == NULL) { // Else guessing that it's at $HOME/.config
        char * home = getenv("HOME");
        int len = strlen(home);
        const char * config_str = "/.config";
        xdg_home = (char *) malloc(sizeof(char) * (len + strlen(config_str))); // No free
        strcpy(xdg_home, home);
        strcpy(xdg_home + len, config_str);
        free_xdg_home = true;
    }

    QDir path(QString(xdg_home) + "/BannerLauncher");
    if (free_xdg_home) free(xdg_home);
    qDebug() << path.absolutePath();
    Menu_Item * item;
    for (auto & dir : path.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        qDebug() << "    " << dir.absoluteFilePath();
        item = new Menu_Item(dir);
        layout->addWidget(item);
        items.push_back(item);
    }

    setCentralWidget(scroll_gui);
}

Banner_Launcher::~Banner_Launcher()
{
    delete gui;
    delete layout;
    delete scroll_gui;
    for (Menu_Item * i : items)
        delete i;
}
