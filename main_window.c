#include "main_window.h"

void entry_click(GtkWidget * widget, GdkEventButton * event, gpointer data) {
    Entry * entry = (Entry *) data;
    if (debug) printf(
        "Mouse button %d on Entry #%s\n", event->button, entry->id
    );
    switch (event->button) {
    case 1:
        Entry_run(entry);
        break;
    case 3:
        gtk_menu_popup_at_pointer(GTK_MENU(menu), NULL);
        break;
    default:
        break;
    }
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

        GtkWidget * event_box = gtk_event_box_new();
        entry->event_box = event_box;
        g_object_ref(event_box);
        gtk_widget_set_events(event_box, GDK_BUTTON_RELEASE_MASK);
        g_signal_connect(
            G_OBJECT(event_box),
            "button_release_event",
            G_CALLBACK(entry_click),
            (gpointer) entry
        );
        gtk_container_add(GTK_CONTAINER(event_box), entry->image);
    }
}

void quit() {
    gtk_widget_destroy(GTK_WIDGET(window));
}

static bool esc_close(GtkWindow * widget, GdkEventKey *event, gpointer data) {
    if (event->keyval == GDK_KEY_Escape) {
        quit();
        return true;
    }
    return false;
}

void init_menu() {
    menu = gtk_menu_new();

    unsigned a = 0;
    unsigned b = 1;

    GtkWidget * edit_item = gtk_menu_item_new_with_label("Edit Game");
    gtk_menu_attach(GTK_MENU(menu), edit_item, 0, 1, a++, b++);

    GtkWidget * remove_item = gtk_menu_item_new_with_label("Remove Game");
    gtk_menu_attach(GTK_MENU(menu), remove_item, 0, 1, a++, b++);

    GtkWidget * add_item = gtk_menu_item_new_with_label("Add Game(s)");
    gtk_menu_attach(GTK_MENU(menu), add_item, 0, 1, a++, b++);

    GtkWidget * settings_item = gtk_menu_item_new_with_label("Settings");
    gtk_menu_attach(GTK_MENU(menu), settings_item, 0, 1, a++, b++);

    GtkWidget * quit_item = gtk_menu_item_new_with_label("Quit");
    gtk_menu_attach(GTK_MENU(menu), quit_item, 0, 1, a++, b++);
    g_signal_connect_swapped(G_OBJECT(quit_item), "activate", G_CALLBACK(quit), NULL);

    gtk_menu_attach_to_widget(GTK_MENU(menu), window, NULL);
    gtk_widget_show_all(menu);
}

void init_main_window(GtkApplication * app, gpointer user_data) {
    // Window
    window = gtk_application_window_new(app);
    gtk_window_set_resizable(GTK_WINDOW(window), false);
    gtk_window_set_title(GTK_WINDOW(window), APP_NAME);
    gtk_window_set_default_size(GTK_WINDOW(window),
        BANNER_WIDTH * GRID_WIDTH, BANNER_HIGHT * 4
    );
    // Close when Escape is pressed
    g_signal_connect(window, "key_press_event", G_CALLBACK(esc_close), NULL);
    layout = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), layout);

    // Entry Context Menu
    init_menu();

    // Filter Textbox
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

    // Scrolling widget for Entries
    scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), BANNER_HIGHT * 4);
    gtk_container_add(GTK_CONTAINER(layout), scroll);
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(scroll), grid);

    // Init Entry Elements and add them
    init_entries_gui(all_entries);
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

