# Banner Launcher
![Example of Usage](example.gif)

Banner Launcher is an launcher designed for launching games displayed with a
Steam banner/header image. They are 460x215 sized images which are used in the
grid view of the Steam Client. Banner Launcher can be used to launch any
application.

Banner Launcher is written using Qt5 with QT Creator.

## Usage
The application works as is but is still a little rough around the edges.
The ability to add installed steam games is working but the menus used to
access this functionality is a work in progress.

Once the applications have games to use, the applications are displayed by
usage and alphabetically. The highlighted entry will be ran if enter is
pressed. Typing will filter the entries. This is ASCII only at the momment.

To mannually add games, a directory must be made in .config/Banner\_Launcher
named for the game with the contents being a executable file called execute
and a image called image. The image can be JPEG, PNG, or, [any other format
supported by Qt5](http://doc.qt.io/qt-5/qtimageformats-index.html). It
should be 460x215 but it will be scaled to 460x215 regardless.

## Major Todos
- Finish Menus
- Ability to add non-steam games
- Restructure XDG config directory
- Make Filtering/Searching more proper.
- Custom Colors, size, other options
- Command Line Arguments
