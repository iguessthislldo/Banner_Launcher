#ifndef MENU_ITEM_HPP
#define MENU_ITEM_HPP

#include <string>

#include <QLabel>
#include <QImage>

#include "Path.hpp"

class Menu_Item : public QLabel
{
    Q_OBJECT

public:
    explicit Menu_Item(const Path & path, QWidget* parent=0);
    ~Menu_Item() {
       if (image != NULL) delete image;
    }

    void set_name(const std::string & name);
    std::string get_name();
    unsigned get_count();
    bool is_valid();

    /*
     * Steam banner size
     * TODO: make an option
     */
    const static unsigned banner_width = 460;
    const static unsigned banner_height = 215;

    /*
     * Compare first by the count, then using built in string compare on the
     * entry name.
     */
    static bool compare(Menu_Item * a, Menu_Item * b);

signals:
    void clicked();

public slots:
    void run();

protected:
    void mousePressEvent(QMouseEvent* event);

private:
    Path path, executable;
    std::string name;
    QImage * image = NULL;
    unsigned count = 0; // Run count
};

#endif // MENU_ITEM_HPP
