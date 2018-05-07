# Banner Launcher

Banner Launcher is a (WIP) games launcher written with GTK+3 which uses Steam
banner/header images. They are 460x215 sized images which are used in the
grid view of the Steam Client. Banner Launcher can be used to launch any
application but can automatically use Steam games.

## Old QT5 Demo
![Example of Usage](example.gif)

## Todo List

### Todo

- New Application Name
- Application Icon
- New Example/Demo GIF
- Add/Edit Entry Window
    - XDG Application
    - Steam
    - Custom Command
    - Add New Entry
- Settings Window
    - Steam Path (Default: Blank)
    - If to add steam game automatically (Default: Yes)
    - Remove Missing Steam Games Automatically?
    - Default Sort (Default: Last Run)
    - Favorites Trumps in All Sorts (Default: No)
    - Colors?
    - Window Decoration?
- Favorites
- Ignore Steam Ids?
- Entry Selection
- Info Box
    - Display Error or Warning Icon
    - Favorite Icon
    - Icon for Steam
- Input Works on main window as I desire:
  - Typing always edits the filter box and filters games
  - Arrow keys go from filter/search box down into Entries
- First Run Window:
  - Where is steam (if not found)?
  - Do you want the app to add new steam games automatically?
- Entries Error Widgets should be 460x215

### Done

- Add/Edit Entry Window
    - Change Name
    - Change Image
- Download Progress Menu
- "Sort By" Menu with "Last Ran", "Most Ran", "Least Ran", "Added",
    and "Alphabetically"
- Context Menus for Entries
- Config file
- Sort games by count then lex
- Launch Game Functionality
    - cd
    - Steam launch by id
- Update Steam games every run
- Input Works on main window as I desire:
    - Enter Runs Game
    - Scrolling scrolls the entries
    - Escape Closes Launcher
