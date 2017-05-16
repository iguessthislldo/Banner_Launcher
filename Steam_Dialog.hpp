#ifndef STEAM_DIALOG_HEADER
#define STEAM_DIALOG_HEADER

#include <string>
#include <list>

#include <QDialog>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QTableWidgetItem>

#include "Path.hpp"

class Steam_Dialog : public QDialog {
    Q_OBJECT

public:
    Steam_Dialog(QWidget *parent = 0);
    ~Steam_Dialog();

public slots:
    void accept();

private:
    const char * STEAMAPPS_PATH = ".local/share/Steam/steamapps";
    const char * APPMANIFEST = "appmanifest";
    const char * APPID_STRING = "appid";
    const char * NAME_STRING = "name";
    const char * STEAM_HEADER_URL_HEAD = "http://cdn.akamai.steamstatic.com/steam/apps/";
    const char * STEAM_HEADER_URL_TAIL = "/header.jpg";

    QVBoxLayout * layout;
    QTableWidget * table_widget;
    std::list<QTableWidgetItem *> table_items;

    void add_game(const std::string & name, const std::string & id);
    void get_installed_steam_apps();
    int get_steam_banner(const std::string & id, const Path & destination);

    void disable_row(unsigned row);
    void enable_row(unsigned row);
};


#endif // STEAM_DIALOG_HPP
