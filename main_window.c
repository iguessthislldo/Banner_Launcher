#include <stdlib.h>
#include <string.h>

#include "steam.h"
#include "util.h"
#include "main_window.h"

GtkWidget * window;
GtkWidget * layout;
GtkWidget * filter;
GtkWidget * scroll;
GtkWidget * grid;
GtkWidget * menu;

char * filter_string;

void entry_click(GtkWidget * widget, GdkEventButton * event, gpointer data) {
    Entry * entry = (Entry *) data;
    if (debug) printf(
        "Mouse button %d on Entry #%s\n", event->button, entry->id
    );
    switch (event->button) {
    case 1: // Left Mouse Button
        Entry_run(entry);
        break;
    case 3: // Right Mouse Button
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

void update_visable_entries() {
    Entries_clear_container(GTK_CONTAINER(grid), visable_entries);
    if (visable_entries != all_entries) {
        Entries_delete(visable_entries);
    }
    if (filter_string && filter_string[0]) {
        visable_entries = Entries_filter(all_entries, filter_string);
    } else {
        visable_entries = all_entries;
    }
    add_entries_to_grid(visable_entries);
}

void filter_changed(GtkEntryBuffer * b) {
    if (filter_string) free(filter_string);
    filter_string = strdup(gtk_entry_buffer_get_text(b));
    if (debug) printf("Filter: %s\n", filter_string);
    update_visable_entries();
}

void init_entries_gui(Entries * entries) {
    for (Node * node = entries->head; node; node = node->next) {
        Entry * entry = node->entry;

        // Event Box
        GtkWidget * event_box = gtk_event_box_new();
        entry->event_box = event_box;
        g_object_ref(event_box);
        gtk_widget_set_size_request(event_box, BANNER_WIDTH, BANNER_HIGHT);
        gtk_widget_set_events(event_box, GDK_BUTTON_RELEASE_MASK);
        g_signal_connect(
            G_OBJECT(event_box),
            "button_release_event",
            G_CALLBACK(entry_click),
            (gpointer) entry
        );

        // Image
        gchar * full_image_path = g_build_filename(
            banners_dir,
            entry->image_path,
        NULL);
        GError * error = NULL;
        GdkPixbuf * image = gdk_pixbuf_new_from_file_at_scale(
            full_image_path, BANNER_WIDTH, BANNER_HIGHT,
            false, // Do not perserve aspect ratio
            &error
        );
        g_free(full_image_path);
        if (image) { // Create Normal GUI
            entry->image = gtk_image_new_from_pixbuf(image);
            g_object_unref(image); // Image is copied and no longer needed
            g_object_ref(entry->image);
            gtk_container_add(GTK_CONTAINER(event_box), entry->image);

        } else { // Create Error Entry GUI
            char * error_message = g_strdup_printf(
                "Could load image %s: \"%s\"\n",
                full_image_path,
                error->message
            );
            g_error_free(error);

            if (debug) fprintf(stderr, error_message);

            GtkWidget * error_message_box = gtk_box_new(
                GTK_ORIENTATION_VERTICAL, 0);
            gtk_container_add(GTK_CONTAINER(error_message_box),
                gtk_label_new(entry->name)
            );
            gtk_container_add(GTK_CONTAINER(error_message_box),
                gtk_image_new_from_icon_name(
                    "image-missing", GTK_ICON_SIZE_DIALOG
                )
            );
            gtk_container_add(GTK_CONTAINER(error_message_box),
                gtk_label_new(error_message)
            );
            gtk_container_add(GTK_CONTAINER(event_box), error_message_box);
            g_free(error_message);
        }

    }
}


bool download_cancelled;
unsigned finished_count;

gpointer download_thread(gpointer data) {
    GtkWidget * dl_modal = ((GtkWidget**)data)[0];
    GtkWidget * dl_bar = ((GtkWidget**)data)[1];

    finished_count = 0;

    for (Node * node = all_entries->head; node; node = node->next) {
        Entry * entry = node->entry;
        if (entry->steam_id && !entry->downloaded_image) {
            gtk_progress_bar_set_text(GTK_PROGRESS_BAR(dl_bar), entry->name);
            char * path = g_build_filename(
                banners_dir,
                entry->image_path,
            NULL);

            // Build URL
            unsigned steam_id_len = strlen(entry->steam_id);
            char * url = malloc(
                steam_header_url_head_len +
                steam_header_url_tail_len +
                steam_id_len + 1
            );
            sprintf(url, "%s%s%s",
                steam_header_url_head,
                entry->steam_id,
                steam_header_url_tail
            );

            // Download
            if (debug) printf("  Download %s\n    to %s\n", url, path);
            download(NULL, NULL, url, path);
            gtk_progress_bar_set_fraction(
                GTK_PROGRESS_BAR(dl_bar),
                ((double) finished_count) / download_images_count
            );
            finished_count++;

            // Cleanup
            g_free(path);
            free(url);
        }
        //gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(dl_bar), );

        if (download_cancelled) {
            break;
        }
    }
    gtk_widget_destroy(dl_modal);
    return NULL;
}

void download_modal() {
    download_cancelled = false;

    // Create Download Modal Dialog
    const char * dl_msg = "Downloading Images for Steam Games...";
    GtkWidget * dl_modal = gtk_dialog_new_with_buttons(
        dl_msg,
        GTK_WINDOW(window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "Cancel", GTK_RESPONSE_CANCEL,
        NULL
    );

    // Add Contents to Dialog
    GtkWidget * content = gtk_dialog_get_content_area(GTK_DIALOG(dl_modal));
    gtk_container_add(GTK_CONTAINER(content), gtk_label_new(dl_msg));
    GtkWidget * dl_bar = gtk_progress_bar_new();
    gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(dl_bar), true);
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(dl_bar), 0.0);
    gtk_container_add(GTK_CONTAINER(content), dl_bar);

    // Run Download thread
    GtkWidget * thread_data[2];
    thread_data[0] = dl_modal;
    thread_data[1] = dl_bar;
    GThread * dl_thread = g_thread_new(
        "download thread", download_thread, &thread_data
    );

    // Show Dialog
    gtk_widget_show_all(dl_modal);
    GtkResponseType r = gtk_dialog_run(GTK_DIALOG(dl_modal));

    // Handle Results
    if (r == GTK_RESPONSE_NONE) {
        if (debug) printf("  Downloads Finished\n");
    } else {
        // Download was cancelled either by "Cancel" button or the dialog
        // was closed
        if (debug) printf("  Downloads Cancelled\n");
        download_cancelled = true;
    }

    // Wait for thread to finish (Should already be done or almost done.)
    g_thread_join(dl_thread);
}

void quit() {
    gtk_widget_destroy(GTK_WIDGET(window));
}

static bool kb_shortcuts(GtkWindow * widget, GdkEventKey *event, gpointer data) {
    switch (event->keyval) {

    // Close when Escape is pressed
    case GDK_KEY_Escape:
        quit();
        return true;

    // When Enter is pressed, run the first rntry in visable_entries
    case GDK_KEY_Return:
        if (visable_entries->size)
            Entry_run(visable_entries->head->entry);
        return true;

    }
    return false;
}

void set_sort_by(void * value) {
    sort_by = (Sort_By) value;
    Entries_sort(all_entries);
    update_visable_entries();
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

    // Sort Entries By
    GtkWidget * sort_by_item = gtk_menu_item_new_with_label("Sort By");
    GtkWidget * sort_by_menu = gtk_menu_new();
    char * names[3] = {"Last Ran", "Most Ran", "Least Ran"};
    GSList * group = NULL;
    for (unsigned i = 0; i < 3; i++) {
        GtkWidget * item = gtk_radio_menu_item_new_with_label(group, names[i]);
        if (!i) gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), true);
        group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(item));
        gtk_menu_attach(GTK_MENU(sort_by_menu), item, 0, 1, i, i+1);
        g_signal_connect_swapped(
            G_OBJECT(item), "activate",
            G_CALLBACK(set_sort_by), (void *) (long unsigned) i
        );
    }
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(sort_by_item), sort_by_menu);
    gtk_menu_attach(GTK_MENU(menu), sort_by_item, 0, 1, a++, b++);

    GtkWidget * settings_item = gtk_menu_item_new_with_label("Settings");
    gtk_menu_attach(GTK_MENU(menu), settings_item, 0, 1, a++, b++);

    GtkWidget * quit_item = gtk_menu_item_new_with_label("Quit");
    gtk_menu_attach(GTK_MENU(menu), quit_item, 0, 1, a++, b++);
    g_signal_connect_swapped(G_OBJECT(quit_item), "activate", G_CALLBACK(quit), NULL);

    gtk_menu_attach_to_widget(GTK_MENU(menu), window, NULL);
    gtk_widget_show_all(menu);
}

void init_main_window(GtkApplication * app, gpointer user_data) {
    filter = NULL;

    // Window
    window = gtk_application_window_new(app);
    gtk_window_set_resizable(GTK_WINDOW(window), false);
    gtk_window_set_title(GTK_WINDOW(window), APP_NAME);
    gtk_window_set_default_size(GTK_WINDOW(window),
        BANNER_WIDTH * GRID_WIDTH, BANNER_HIGHT * 4
    );

    // Handle Keyboard Shortcuts
    g_signal_connect(window, "key_press_event", G_CALLBACK(kb_shortcuts), NULL);

    // Vertical Layout
    layout = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), layout);

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
    gtk_scrolled_window_set_min_content_height(
        GTK_SCROLLED_WINDOW(scroll), BANNER_HIGHT * 4);
    gtk_container_add(GTK_CONTAINER(layout), scroll);

    // Show Window without Entries
    gtk_widget_show_all(window);

    // Download Missing Steam Images
    if (debug) printf("download_image_count: %u\n", download_images_count);
    if (download_images_count)  {
        download_modal();
    }

    // Init Entry Elements and add them
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(scroll), grid);
    init_entries_gui(all_entries);
    visable_entries = all_entries;
    update_visable_entries();

    // Entry Context Menu
    init_menu();

    // Show Window with Entries
    gtk_widget_show_all(grid);
}

