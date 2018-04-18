#include <string.h>

#include "launcher.h"
#include "main_window.h"
#include "edit_window.h"

void show_edit_window(GtkWidget * window, Entry * entry, bool edit) {
    GtkWidget * modal = gtk_dialog_new_with_buttons(
        edit ? "Edit Game" : "Add Game",
        GTK_WINDOW(window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "Save", GTK_RESPONSE_OK,
        "Cancel", GTK_RESPONSE_CANCEL,
        NULL
    );

    GtkWidget * content = gtk_dialog_get_content_area(GTK_DIALOG(modal));

    GtkWidget * name_widget = gtk_entry_new();
    gtk_container_add(GTK_CONTAINER(content), name_widget);

    if (edit) {
        gtk_entry_set_text(GTK_ENTRY(name_widget), entry->name);
    }

    // Show Dialog
    gtk_widget_show_all(modal);
    GtkResponseType r = gtk_dialog_run(GTK_DIALOG(modal));

    // Handle Results
    if (r == GTK_RESPONSE_OK) {
        Entry_set_name(entry, gtk_entry_get_text(GTK_ENTRY(name_widget)));
        gtk_label_set_text(GTK_LABEL(entry->name_label), entry->name);
        entries_changed = true;
        update_visable_entries();
    }
    gtk_widget_destroy(modal);
}
