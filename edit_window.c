#include <string.h>

#include "launcher.h"
#include "main_window.h"
#include "edit_window.h"
#include "util.h"

struct image_params {
    Entry * entry;
    GtkImage * preview;
    char * filepath;
};

void image_changed(GtkFileChooserButton * fc, struct image_params * ip) {
    ip->filepath = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fc));
    if (debug) printf("Image Path: %s\n", ip->filepath);
    gtk_image_set_from_file(ip->preview, ip->filepath);
}

void show_edit_window(GtkWidget * window, Entry * entry) {
    GtkWidget * modal = gtk_dialog_new_with_buttons(
        entry ? "Edit Game" : "Add Game",
        GTK_WINDOW(window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "Save", GTK_RESPONSE_OK,
        "Cancel", GTK_RESPONSE_CANCEL,
        NULL
    );

    GtkWidget * content = gtk_dialog_get_content_area(GTK_DIALOG(modal));

    // Contents --------------------------------------------------------------

    // Name
    GtkWidget * name_widget = gtk_entry_new();
    gtk_container_add(GTK_CONTAINER(content), name_widget);
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
        gtk_container_add(GTK_CONTAINER(content), preview);
    }

    // Chooose Image
    GtkWidget * image_button = gtk_file_chooser_button_new(
        "Choose Image", GTK_FILE_CHOOSER_ACTION_OPEN);
    gtk_container_add(GTK_CONTAINER(content), image_button);
    struct image_params ip;
    ip.preview = GTK_IMAGE(preview);
    ip.entry = entry;
    ip.filepath = 0;
    g_signal_connect(
        G_OBJECT(image_button), "file-set",
        G_CALLBACK(image_changed), &ip
    );

    // Entry Type
    GtkWidget
        * radio_box,
        * steam_radio,
        * xdg_radio,
        * command_radio
    ;
    radio_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
    gtk_box_set_homogeneous(GTK_BOX(radio_box), true);

    steam_radio = gtk_radio_button_new(NULL);
    gtk_container_add(GTK_CONTAINER(steam_radio), gtk_label_new("Steam Game"));

    xdg_radio = gtk_radio_button_new_with_label_from_widget(
        GTK_RADIO_BUTTON(steam_radio), "Choose Installed Non-Steam Game");

    command_radio = gtk_radio_button_new_with_label_from_widget(
        GTK_RADIO_BUTTON(steam_radio), "Custom Command");

    gtk_box_pack_start(GTK_BOX(radio_box), steam_radio, true, true, 3);
    gtk_box_pack_start(GTK_BOX(radio_box), xdg_radio, true, true, 3);
    gtk_box_pack_start(GTK_BOX(radio_box), command_radio, true, true, 3);
    gtk_container_add(GTK_CONTAINER(content), radio_box);

    // -----------------------------------------------------------------------

    // Show Dialog
    gtk_widget_show_all(modal);
    GtkResponseType r = gtk_dialog_run(GTK_DIALOG(modal));

    // Handle Results
    if (r == GTK_RESPONSE_OK && entry) {
        Entry_set_name(entry, gtk_entry_get_text(GTK_ENTRY(name_widget)));
        entries_changed = true;
        if (ip.filepath) { // TODO: Handle Errors Here
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
            char * ext_str = file_ext(ip.filepath, &ext_len);
            unsigned id_len = strlen(entry->id);
            char * dest_filename = g_malloc(id_len + ext_len);
            memcpy(dest_filename, entry->id, id_len);
            memcpy(dest_filename + id_len, ext_str, ext_len);
            dest_filename[id_len + ext_len] = '\0';

            // Set Entry Image filename
            g_free(entry->image);
            entry->image = dest_filename;

            // Copy New File
            GFile * src_file = g_file_new_for_path(ip.filepath);
            char * dest_file_str = g_build_filename(
                banners_dir,
                dest_filename,
            NULL);
            GFile * dest_file = g_file_new_for_path(dest_file_str);
            if (debug) printf("cp %s %s\n", ip.filepath, dest_file_str);
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
    gtk_widget_destroy(modal);
}
