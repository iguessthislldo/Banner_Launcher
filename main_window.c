#include "main_window.h"

void entry_click(GtkWidget * widget, GdkEvent * event, gpointer data) {
    Entry_run((Entry *) data);
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

void init_entries_gui(Entries * entries) {
    for (Node * node = entries->head; node; node = node->next) {
        Entry * entry = node->entry;
        gchar * image_file = g_build_filename(
            banners_dir,
            entry->image_path,
        NULL);
        entry->image = gtk_image_new_from_file(image_file);
        g_object_ref(entry->image);
        g_free(image_file);
    }
}

void init_main_window(GtkApplication * app, gpointer user_data) {
    init_entries_gui(all_entries);
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

int update_bar(
    void * data,
    double dltotal, double dlnow,
    double ultotal, double ulnow
) {
    if (data) {
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(data), dlnow / dltotal);
    }
    return 0;
}

