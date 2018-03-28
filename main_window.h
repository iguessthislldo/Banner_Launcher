#ifndef MAIN_WINDOW_HEADER
#define MAIN_WINDOW_HEADER

#include <glib.h>
#include <gtk/gtk.h>

#include "launcher.h"

GtkWidget * window;
GtkWidget * layout;
GtkWidget * filter;
GtkWidget * scroll;
GtkWidget * grid;
GtkWidget * menu;

void init_main_window(GtkApplication * app, gpointer user_data);
int update_bar(
    void * data,
    double dltotal, double dlnow,
    double ultotal, double ulnow
);

#endif
