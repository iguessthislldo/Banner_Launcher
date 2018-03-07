#include <QApplication>

#include "Banner_Launcher.hpp"
#include "Config.hpp"
#include "Path.hpp"

int main(int argc, char *argv[]) {
    // Application
    QApplication a(argc, argv);
    a.setApplicationName("Banner_Launcher");
    a.setApplicationDisplayName("Banner Launcher");
    a.setApplicationVersion("0.2");

    // Initialize Main Class and pass configuration
    Config config(a);
    Banner_Launcher w(config);
    w.show();

    // Run
    return a.exec();
}
