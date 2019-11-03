#include <string.h>

#include "launcher.h"
#include "main_window.h"
#include "edit_window.h"
#include "util.h"

typedef struct {
    Entry * entry;

    GtkImage * preview;
    char * filepath;

    GtkWidget * modal;
    GtkWidget * content;
    GtkWidget * steam_options;
    GtkWidget * xdg_options;
    GtkWidget * shell_options;

    GtkWidget * steam_radio;
    GtkWidget * xdg_radio;
    GtkWidget * shell_radio;
} Edit_Window;

void image_changed(GtkFileChooserButton * fc, Edit_Window * ip) {
    ip->filepath = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fc));
    if (debug) printf("Image Path: %s\n", ip->filepath);
    gtk_image_set_from_file(ip->preview, ip->filepath);
}

void show_options(Edit_Window * w, Entry_Type type) {
    if (type == ENTRY_TYPE_STEAM) {
        gtk_widget_show_all(w->steam_options);
    } else {
        gtk_widget_hide(w->steam_options);
    }
    if (type == ENTRY_TYPE_XDG) {
        gtk_widget_show_all(w->xdg_options);
    } else {
        gtk_widget_hide(w->xdg_options);
    }
    if (type == ENTRY_TYPE_SHELL) {
        gtk_widget_show_all(w->shell_options);
    } else {
        gtk_widget_hide(w->shell_options);
    }
};

Entry_Type get_selected_type(Edit_Window * w) {
    Entry_Type selected_type = ENTRY_TYPE_INVALID;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w->steam_radio))) {
        selected_type = ENTRY_TYPE_STEAM;
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w->xdg_radio))) {
        selected_type = ENTRY_TYPE_XDG;
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w->shell_radio))) {
        selected_type = ENTRY_TYPE_SHELL;
    }
    return selected_type;
}

void selected_type_changed(GtkRadioButton * b, Edit_Window * w) {
    printf("selected_type_changed\n");
    show_options(w, get_selected_type(w));
};

void show_edit_window(GtkWidget * window, Entry * entry) {
    Edit_Window w;
    w.modal = gtk_dialog_new_with_buttons(
        entry ? "Edit Game" : "Add Game",
        GTK_WINDOW(window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "Save", GTK_RESPONSE_OK,
        "Cancel", GTK_RESPONSE_CANCEL,
        NULL
    );

    // Contents --------------------------------------------------------------

    w.content = gtk_dialog_get_content_area(GTK_DIALOG(w.modal));

    // Name
    GtkWidget * name_widget = gtk_entry_new();
    gtk_container_add(GTK_CONTAINER(w.content), name_widget);
    if (entry) {
        gtk_entry_set_text(GTK_ENTRY(name_widget), entry->name);
    }

    // Preview
    GtkWidget * preview = NULL;
    if (entry && entry->image_widget) {
        preview = gtk_image_new_from_pixbuf(
            gtk_image_get_pixbuf(GTK_IMAGE(entry->image_widget))
        );
    }
    if (preview) {
        gtk_container_add(GTK_CONTAINER(w.content), preview);
    }

    // Chooose Image
    GtkWidget * image_button = gtk_file_chooser_button_new(
        "Choose Image", GTK_FILE_CHOOSER_ACTION_OPEN);
    gtk_container_add(GTK_CONTAINER(w.content), image_button);
    w.preview = GTK_IMAGE(preview);
    w.entry = entry;
    w.filepath = NULL;
    g_signal_connect(
        G_OBJECT(image_button), "file-set",
        G_CALLBACK(image_changed), &w
    );

    // Entry Type
    GtkWidget * radio_box;

    radio_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
    gtk_box_set_homogeneous(GTK_BOX(radio_box), true);

    w.steam_radio = gtk_radio_button_new(NULL);
    gtk_container_add(GTK_CONTAINER(w.steam_radio), gtk_label_new("Steam Game"));

    w.xdg_radio = gtk_radio_button_new_with_label_from_widget(
        GTK_RADIO_BUTTON(w.steam_radio), "Choose Installed Non-Steam Game");

    w.shell_radio = gtk_radio_button_new_with_label_from_widget(
        GTK_RADIO_BUTTON(w.steam_radio), "Custom Command");

    gtk_box_pack_start(GTK_BOX(radio_box), w.steam_radio, true, true, 3);
    gtk_box_pack_start(GTK_BOX(radio_box), w.xdg_radio, true, true, 3);
    gtk_box_pack_start(GTK_BOX(radio_box), w.shell_radio, true, true, 3);
    gtk_container_add(GTK_CONTAINER(w.content), radio_box);

    g_signal_connect(
        G_OBJECT(image_button), "file-set",
        G_CALLBACK(image_changed), &w
    );

    // Show all widgets up to this point
    gtk_widget_show_all(w.modal);

    // -----------------------------------------------------------------------
    // Steam
    // -----------------------------------------------------------------------

    w.steam_options = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);

    // Steam Id
    GtkWidget * steam_id_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
    gtk_box_pack_start(GTK_BOX(steam_id_box), gtk_label_new("Steam AppID:"), true, true, 3);
    GtkWidget * steam_id_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(steam_id_box), steam_id_entry, true, true, 3);
    gtk_container_add(GTK_CONTAINER(w.steam_options), steam_id_box);

    gtk_container_add(GTK_CONTAINER(w.content), w.steam_options);

    // -----------------------------------------------------------------------
    // XDG
    // -----------------------------------------------------------------------

    w.xdg_options = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);

    gtk_container_add(GTK_CONTAINER(w.content), w.xdg_options);

    // -----------------------------------------------------------------------
    // Shell
    // -----------------------------------------------------------------------

    w.shell_options = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);

    // Command
    GtkWidget * command_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
    gtk_box_pack_start(GTK_BOX(command_box), gtk_label_new("Command:"), true, true, 3);
    GtkWidget * command_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(command_box), command_entry, true, true, 3);
    gtk_container_add(GTK_CONTAINER(w.shell_options), command_box);

    gtk_container_add(GTK_CONTAINER(w.content), w.shell_options);

    // -----------------------------------------------------------------------
    // Populate
    // -----------------------------------------------------------------------

    switch (entry->type) {
    case ENTRY_TYPE_STEAM:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w.steam_radio), true);
        gtk_entry_set_text(GTK_ENTRY(steam_id_entry), entry->info.steam.steam_id);
        break;
    case ENTRY_TYPE_XDG:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w.xdg_radio), true);
        break;
    case ENTRY_TYPE_SHELL:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w.shell_radio), true);
        gtk_entry_set_text(GTK_ENTRY(command_entry), entry->info.shell.exec);
        break;
    };
    show_options(&w, entry->type);

    g_signal_connect(
        G_OBJECT(w.steam_radio), "toggled",
        G_CALLBACK(selected_type_changed), &w);

    // -----------------------------------------------------------------------

    // Show Dialog
    GtkResponseType r = gtk_dialog_run(GTK_DIALOG(w.modal));

    // Handle Results
    if (r == GTK_RESPONSE_OK && entry) {
        Entry_set_name(entry, gtk_entry_get_text(GTK_ENTRY(name_widget)));
        entries_changed = true;
        if (w.filepath) { // TODO: Handle Errors Here
            // Delete Old File
            char * old_file = g_build_filename(
                banners_dir,
                entry->image,
            NULL);
            if (debug) printf("rm %s\n", old_file);
            unlink(old_file);
            g_free(old_file);

            // Create New Image Filename String
            unsigned ext_len;
            char * ext_str = file_ext(w.filepath, &ext_len);
            unsigned id_len = strlen(entry->id);
            char * dest_filename = g_malloc(id_len + ext_len);
            memcpy(dest_filename, entry->id, id_len);
            memcpy(dest_filename + id_len, ext_str, ext_len);
            dest_filename[id_len + ext_len] = '\0';

            // Set Entry Image filename
            g_free(entry->image);
            entry->image = dest_filename;

            // Copy New File
            GFile * src_file = g_file_new_for_path(w.filepath);
            char * dest_file_str = g_build_filename(
                banners_dir,
                dest_filename,
            NULL);
            GFile * dest_file = g_file_new_for_path(dest_file_str);
            if (debug) printf("cp %s %s\n", w.filepath, dest_file_str);
            g_free(dest_file_str);
            g_file_copy(src_file, dest_file,
                G_FILE_COPY_OVERWRITE,
                NULL, NULL, NULL, NULL
            );
            g_object_unref(src_file);
            g_object_unref(dest_file);

            // Replace Image in main window
            gtk_image_set_from_pixbuf(
                GTK_IMAGE(entry->image_widget),
                gtk_image_get_pixbuf(GTK_IMAGE(preview))
            );

        }
        update_visable_entries();
    }
    gtk_widget_destroy(w.modal);
}
