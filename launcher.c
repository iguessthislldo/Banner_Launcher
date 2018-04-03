#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <glib.h>
#include <gtk/gtk.h>

#include "launcher.h"
#include "steam.h"
#include "util.h"
#include "main_window.h"

static GOptionEntry options[] = {
    {
        "config", 0, 0, G_OPTION_ARG_FILENAME, &config_dir,
        "DIR is the directory where config and image files are kept,"
            "default is $XDG_CONFIG_HOME/banner_launcher",
        "DIR"
    },
    {
        "debug", 0, 0, G_OPTION_ARG_NONE, &debug,
        "Enable Debug Messages",
        NULL
    },
    {
        "dev", 'd', 0, G_OPTION_ARG_NONE, &dev_mode,
        "Development mode, equivlant to \" --debug --config debug_config\"",
        NULL
    },
    { NULL }
};

bool save_config(const char * path) {
    bool had_error = false;
    GKeyFile * ini = g_key_file_new();

    g_key_file_set_string(ini, "config", "steam_path",
        steam_path ? steam_path : ""
    );

    g_key_file_set_boolean(ini, "config", "include_steam_entries",
        include_steam_entries
    );

    // Save config to file
    GError * error = NULL;
    if (!g_key_file_save_to_file(ini, path, &error)) {
        had_error = true;
        g_error("Could not save config to %s: \"%s\"\n",
            path, error->message
        );
        g_error_free(error);
    }
    g_key_file_free(ini);
    return had_error;
}

bool load_config(const char * path) {
    bool error = false;
    if (debug) printf("Loading config from %s\n", path);
    GKeyFile * ini = g_key_file_new();
    if (g_key_file_load_from_file(
        ini, path, G_KEY_FILE_NONE, NULL
    )) {
        gsize num_groups;
        gchar ** groups = g_key_file_get_groups(ini, &num_groups);
        if (num_groups == 1 && !strcmp(groups[0], "config")) {
            steam_path = g_key_file_get_string(
                ini, "config", "steam_path", NULL
            );
            include_steam_entries = g_key_file_get_boolean(
                ini, "config", "include_steam_entries", NULL
            );
            if (debug) {
                printf("  steam_path: %s\n", steam_path);
                printf("  include_steam_entries: %d\n", include_steam_entries);
            }
        } else {
            fprintf(stderr,
                "The config.ini file must only have one group: [config]\n"
            );
            error = true;
        }
    } else {
        error = true;
    }
    g_key_file_free(ini);
    return error;
}

void init_data() {
    // Paths
    if (!config_dir) {
        config_dir = g_build_filename(
            g_get_user_config_dir(),
            "banner_launcher",
        NULL);
    }
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

    // Check the paths, creating defaults if not found
    if (!g_file_test(config_dir, G_FILE_TEST_IS_DIR)) {
        if (g_mkdir_with_parents(config_dir, 0744)) {
            fprintf(stderr,
                "Could not create new config directory: \"%s\"\n",
                strerror(errno)
            );
            exit(1);
        }
    }
    if (!g_file_test(entries_file, G_FILE_TEST_IS_REGULAR)) {
        GError * error = NULL;
        const char * default_file = "[meta]\nnext_id=0\n";
        if (!g_file_set_contents(entries_file, default_file, -1, &error)) {
            g_error(
                "Could not create new entries file: %s\n", error->message
            );
            g_error_free(error);
        }
    }
    if (!g_file_test(config_file, G_FILE_TEST_IS_REGULAR)) {
        save_config(config_file);
    }
    if (!g_file_test(banners_dir, G_FILE_TEST_IS_DIR)) {
        if (g_mkdir_with_parents(banners_dir, 0744)) {
            g_error(
                "Could not create new banners directory %s: \"%s\"\n",
                banners_dir,
                strerror(errno)
            );
        }
    }

    // Load files
    all_entries = Entries_new();
    visable_entries = NULL;
    Entries_load(all_entries, entries_file);
    Entries_sort(all_entries);
    load_config(config_file);
    steam_entries = Entries_new();
    load_steam_entries();

    if (include_steam_entries) {
        Entries_insert_steam();
    }
}

int main(int argc, char * argv[]) {
    debug = false;
    dev_mode = false;
    config_dir = NULL;
    visable_entries = NULL;
    steam_path = NULL;
    include_steam_entries = false;
    entries_changed = false;
    sort_by = LAST_RAN;

    steam_header_url_head = STEAM_HEADER_URL_HEAD;
    steam_header_url_tail = STEAM_HEADER_URL_TAIL;
    steam_header_url_head_len = strlen(STEAM_HEADER_URL_HEAD);
    steam_header_url_tail_len = strlen(STEAM_HEADER_URL_TAIL);

    GError * error = NULL;
    GOptionContext * option_context = g_option_context_new(
        "Launch applications using Steam 460x215 images"
    );
    g_option_context_add_main_entries(option_context, options, NULL);
    g_option_context_add_group(option_context, gtk_get_option_group(TRUE));
    if (!g_option_context_parse(option_context, &argc, &argv, &error)) {
        g_print("Option parsing failed: %s\n", error->message);
        exit(1);
    }

    if (dev_mode) {
        printf("Development Debug Mode\n");
        config_dir = "debug_config";
        debug = true;
    }

    init_data();

    GtkApplication * app;
    int status;

    app = gtk_application_new("us.hornsey.launcher", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(init_main_window), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    if (visable_entries != all_entries) Entries_delete(visable_entries);
    Entries_delete_all(all_entries);
    Entries_delete_all(steam_entries);
    if (!dev_mode) g_free(config_dir);
    g_free(config_file);
    g_free(entries_file);
    g_free(banners_dir);

    return status;
}
