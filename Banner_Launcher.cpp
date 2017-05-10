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


    QDir path("/home/fred/.config/BannerLauncher");
    qDebug() << path.absolutePath();
    Item * item;
    for (auto & dir : path.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        qDebug() << "    " << dir.absoluteFilePath();
        item = new Item(dir);
        layout->addWidget(item->get_gui());
        items.push_back(item);
    }

    setCentralWidget(scroll_gui);
}

Banner_Launcher::~Banner_Launcher()
{
    delete gui;
    delete layout;
    delete scroll_gui;
    for (Item * i : items)
        delete i;
}
