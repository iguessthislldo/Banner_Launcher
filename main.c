#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <glib.h>
#include <gtk/gtk.h>

#include "Entry.h"

#define NAME
#define BANNER_WIDTH 460
#define BANNER_HIGHT 215
#define GRID_WIDTH 3

Entries * all_entries;

gchar * config_dir;
gchar * entries_file;
gchar * banners_dir;

bool load_entries(Entries * entries, const gchar * path) {
    GKeyFile * ini = g_key_file_new();
    if (g_key_file_load_from_file(
        ini, path, G_KEY_FILE_NONE, NULL
    )) {
        gsize num_groups;
        gchar ** groups = g_key_file_get_groups(ini, &num_groups);
        for (gsize i = 0; i < num_groups; i++) {
            Entry * entry = Entry_new();
            entry->name = g_key_file_get_value(ini, groups[i], "name", NULL);
            gchar * image_file = g_build_filename(
                banners_dir,
                g_key_file_get_value(ini, groups[i], "image", NULL),
            NULL);
            entry->image = gtk_image_new_from_file(image_file);
            g_free(image_file);
            Entries_insert(entries, entry);
        }
        return false;
    } else {
        return true;
    }
}

void entry_click(GtkWidget * widget, GdkEvent * event, gpointer data) {
    printf("Run: %s\n", ((Entry *) data)->name);
}

static void activate(GtkApplication * app, gpointer user_data) {
    config_dir = g_build_filename(
        g_get_user_config_dir(),
        "banner_launcher",
    NULL);
    entries_file = g_build_filename(
        config_dir,
        "entries.ini",
    NULL);
    banners_dir = g_build_filename(
        config_dir,
        "banners",
    NULL);
    all_entries = Entries_new();
    load_entries(all_entries, entries_file);

    GtkWidget * window;
    GtkWidget * layout;
    GtkWidget * search;
    GtkWidget * scroll;
    GtkWidget * grid;

    window = gtk_application_window_new(app);
    gtk_window_set_resizable(GTK_WINDOW(window), false);
    gtk_window_set_title(GTK_WINDOW(window), "Window");
    gtk_window_set_default_size(GTK_WINDOW(window), BANNER_WIDTH * GRID_WIDTH, BANNER_HIGHT * 4);
    layout = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), layout);

    search = gtk_entry_new();
    gtk_container_add(GTK_CONTAINER(layout), search);

    scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), BANNER_HIGHT * 4);
    gtk_container_add(GTK_CONTAINER(layout), scroll);
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(scroll), grid);

    int row = 0;
    int col = 0;
    int cols = GRID_WIDTH;
    for (Node * n = all_entries->head; n; n = n->next) {
        Entry * e = n->entry;
        GtkWidget * event_box = gtk_event_box_new();
        gtk_widget_set_events(event_box, GDK_BUTTON_RELEASE_MASK);
        g_signal_connect(
            G_OBJECT(event_box),
            "button_release_event",
            G_CALLBACK(entry_click),
            (gpointer) e
        );
        gtk_container_add(GTK_CONTAINER(event_box), e->image);
        gtk_grid_attach(GTK_GRID(grid), event_box, col, row, 1, 1);
        if (++col >= cols) {
            row++;
            col = 0;
        }
    }

    gtk_widget_show_all(window);
}

int main(int argc, char * argv[]) {
    GtkApplication * app;
    int status;

    app = gtk_application_new("us.hornsey.launcher", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    Entries_delete_all(all_entries);
    g_free(config_dir);
    g_free(entries_file);
    g_free(banners_dir);

    return status;
}
