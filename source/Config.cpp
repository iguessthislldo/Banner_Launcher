#include <QStringList>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include "Config.hpp"

Config::Config() {
    // Command Line Options
    QCommandLineParser parser;
    parser.setApplicationDescription(
        "Game launcher for Steam and non-steam games."
    );
    parser.addHelpOption();
    parser.addVersionOption();

    // Set Config Directory Option
    QCommandLineOption config_option(
        QStringList() << "c" << "config",
        "Set Configuration Directory"
        "CONFIG_DIRECTORY"
    );
    parser.addOption(config_option);

    // Steam Options
    QCommandLineOption steam_root_option(
        "steam-root",
        "Set Steam Root Directory",
        "STEAM_ROOT_DIRECTORY"
    );
    parser.addOption(steam_root_option);
    QCommandLineOption steam_exe_option(
        "steam-exe",
        "Set Steam Executable",
        "STEAM_EXE_FILE"
    );
    parser.addOption(steam_exe_option);

    // Color Options
    QCommandLineOption fg_color_option(
        "foreground",
        "Set Foreground Color as a HTML Hex Color",
        "COLOR"
    );
    parser.addOption(fg_color_option);
    QCommandLineOption bg_color_option(
        "background",
        "Set Background Color",
        "COLOR"
    );
    parser.addOption(bg_color_option);

    // Parse Arguments
    parser.process(a);
}
