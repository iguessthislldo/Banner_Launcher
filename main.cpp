#include <QApplication>

#include "Banner_Launcher.hpp"
#include "Path.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Banner_Launcher w;
    w.show();

    return a.exec();
}
