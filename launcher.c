#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <glib.h>
#include <gtk/gtk.h>

#include "launcher.h"
#include "steam.h"

GtkWidget * window;
GtkWidget * layout;
GtkWidget * filter;
GtkWidget * scroll;
GtkWidget * grid;

bool load_config(const gchar * path) {
    bool error = false;
    if (debug) printf("Loading config from %s\n", path);
    GKeyFile * ini = g_key_file_new();
    if (g_key_file_load_from_file(
        ini, path, G_KEY_FILE_NONE, NULL
    )) {
        gsize num_groups;
        gchar ** groups = g_key_file_get_groups(ini, &num_groups);
        if (num_groups == 1 && !strcmp(groups[0], "config")) {
            printf("  steam_path: %s\n",
                g_key_file_get_string(ini, "config", "steam_path", NULL)
            );
        } else {
            fprintf(stderr,
                "config.ini file must only have one group called \"config\"\n"
            );
            error = true;
        }
    } else {
        error = true;
    }
    g_key_file_free(ini);
    return error;
}

bool load_entries(Entries * entries, const gchar * path) {
    bool error = false;
    if (debug) printf("Loading Entries from %s\n", path);
    GKeyFile * ini = g_key_file_new();
    if (g_key_file_load_from_file(
        ini, path, G_KEY_FILE_NONE, NULL
    )) {
        gsize num_groups;
        gchar ** groups = g_key_file_get_groups(ini, &num_groups);
        if (!num_groups || strcmp("meta", groups[0])) {
            error = true;
        }
        next_id = g_key_file_get_integer(ini, groups[0], "next_id", NULL);
        for (gsize i = 1; i < num_groups; i++) {
            Entry * entry = Entry_new();
            entry->name = g_key_file_get_string(ini, groups[i], "name", NULL);
            gchar * image_file = g_build_filename(
                banners_dir,
                g_key_file_get_string(ini, groups[i], "image", NULL),
            NULL);
            entry->image = gtk_image_new_from_file(image_file);
            g_object_ref(entry->image);
            if (debug) {
                printf("  Entry: %s\n    \"%s\"\n",
                    groups[i], entry->name
                );
            }
            g_free(image_file);
            Entries_insert(entries, entry);
        }
    } else {
        error = true;
    }
    g_key_file_free(ini);
    return error;
}

void entry_click(GtkWidget * widget, GdkEvent * event, gpointer data) {
    if (debug) printf("Run: %s\n", ((Entry *) data)->name);
}

void add_entries_to_grid(Entries * entries) {
    int row = 0;
    int col = 0;
    int cols = GRID_WIDTH;
    for (Node * n = entries->head; n; n = n->next) {
        Entry * e = n->entry;
        gtk_grid_attach(GTK_GRID(grid), e->event_box, col, row, 1, 1);
        if (++col >= cols) {
            row++;
            col = 0;
        }
    }
}

void filter_changed(GtkEntryBuffer * b) {
    const char * filter = gtk_entry_buffer_get_text(b);
    if (debug) printf("Filter: %s\n", filter);
    Entries_clear_container(GTK_CONTAINER(grid), visable_entries);
    if (visable_entries != all_entries) {
        Entries_delete(visable_entries);
    }
    if (filter[0]) {
        visable_entries = Entries_filter(all_entries, filter);
    } else {
        if (debug) printf("  <RESET>\n");
        visable_entries = all_entries;
    }
    add_entries_to_grid(visable_entries);
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
    config_file = g_build_filename(
        config_dir,
        "config.ini",
    NULL);
    all_entries = Entries_new();
    visable_entries = NULL;
    load_entries(all_entries, entries_file);
    load_config(config_file);
    steam_entries = Entries_new();
    load_steam_entries(steam_path, steam_entries);

    window = gtk_application_window_new(app);
    gtk_window_set_resizable(GTK_WINDOW(window), false);
    gtk_window_set_title(GTK_WINDOW(window), APP_NAME);
    gtk_window_set_default_size(GTK_WINDOW(window), BANNER_WIDTH * GRID_WIDTH, BANNER_HIGHT * 4);
    layout = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), layout);

    GtkEntryBuffer * filter_buffer = gtk_entry_buffer_new("", -1);
    filter = gtk_entry_new_with_buffer(filter_buffer);
    g_signal_connect(
        G_OBJECT(filter_buffer),
        "inserted-text",
        G_CALLBACK(filter_changed),
        NULL
    );
    g_signal_connect(
        G_OBJECT(filter_buffer),
        "deleted-text",
        G_CALLBACK(filter_changed),
        NULL
    );
    gtk_container_add(GTK_CONTAINER(layout), filter);

    scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), BANNER_HIGHT * 4);
    gtk_container_add(GTK_CONTAINER(layout), scroll);
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(scroll), grid);

    for (Node * n = all_entries->head; n; n = n->next) {
        Entry * e = n->entry;
        GtkWidget * event_box = gtk_event_box_new();
        e->event_box = event_box;
        g_object_ref(event_box);
        gtk_widget_set_events(event_box, GDK_BUTTON_RELEASE_MASK);
        g_signal_connect(
            G_OBJECT(event_box),
            "button_release_event",
            G_CALLBACK(entry_click),
            (gpointer) e
        );
        gtk_container_add(GTK_CONTAINER(event_box), e->image);
    }

    visable_entries = all_entries;
    add_entries_to_grid(visable_entries);

    gtk_widget_show_all(window);
}

int main(int argc, char * argv[]) {
    debug = true;
    visable_entries = NULL;

    GtkApplication * app;
    int status;

    app = gtk_application_new("us.hornsey.launcher", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    if (visable_entries != all_entries) Entries_delete(visable_entries);
    Entries_delete_all(all_entries);
    Entries_delete_all(steam_entries);
    g_free(config_dir);
    g_free(config_file);
    g_free(entries_file);
    g_free(banners_dir);

    return status;
}
