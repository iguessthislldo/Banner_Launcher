#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <glib.h>

#include "util.h"
#include "steam.h"
#include "launcher.h"

unsigned download_images_count = 0;

void load_steam_entries() {

    // Resolve steam directory
    if (!(steam_path && steam_path[0])) {
        if (steam_path) g_free(steam_path);
        steam_path = g_build_filename(
            g_get_home_dir(),
            ".steam",
            NULL
        );
    }

    // Strings to Find
    GString * name_key = g_string_new("name");
    GString * appid_key = g_string_new("appid");

    // Get steamapps
    if (debug) printf("Loading Steam Games from %s:\n", steam_path);
    gchar * steamapps_str = g_build_filename(
        steam_path,
        "steam",
        "steamapps",
        NULL
    );
    GDir * steamapps = g_dir_open(steamapps_str, 0, NULL);

    // Go through every entry in the steamapps directory
    gchar * dir_entry;
    while ((dir_entry = (gchar *) g_dir_read_name(steamapps))) {
        char * full_path = g_build_filename(
            steamapps_str,
            dir_entry,
            NULL
        );
        if (!starts_with(dir_entry, "appmanifest_")) continue;

        /*
         * Scans the vdf appmanifest file for quoted strings.
         * When a quoted string matched the app name and id keys,
         * skip to the next quotation mark and copy the chars into the
         * corresponding string. When both strings are captured, exit the loop.
         */
        FILE * file = fopen(full_path, "r");
        if (file == NULL) {
            fprintf(stderr,
                "Couldn't open app manifest %s: %s\n",
                dir_entry,
                strerror(errno)
            );
            continue;
        }

        enum State_Enum {
            IGNORE, // Skip to next "
            WORD, // Add character to word string if not "
            IGNORE_TO_NAME, // Skip to next "
            IGNORE_TO_ID, // Skip to next "
            NAME, // Add character to name string if not "
            ID // add character to id string if not "
        };
        enum State_Enum state = IGNORE;

        char c;
        GString * word = g_string_new("");
        GString * id = g_string_new("");
        GString * name = g_string_new("");
        bool got_name = false;
        bool got_id = false;
        bool loop = true;

        while (loop && (c = fgetc(file)) != EOF) {
            switch (state) {
            case IGNORE:
                if (c == '"')
                    state = WORD;
                continue;
            case WORD:
                if (c == '"') {
                    if (!got_name && g_string_equal(word, name_key)) {
                        state = IGNORE_TO_NAME;
                    } else if (!got_id && g_string_equal(word, appid_key)) {
                        state = IGNORE_TO_ID;
                    } else {
                        state = IGNORE;
                    }
                    g_string_erase(word, 0, -1);
                } else g_string_append_c(word, c);
                continue;
            case IGNORE_TO_ID:
                if (c == '"')
                    state = ID;
                continue;
            case ID:
                if (c == '"') {
                    state = IGNORE;
                    got_id = true;
                    if (got_name)
                        loop = false;
                } else g_string_append_c(id, c);
                continue;
            case IGNORE_TO_NAME:
                if (c == '"')
                    state = NAME;
                continue;
            case NAME:
                if (c == '"') {
                    state = IGNORE;
                    got_name = true;
                    if (got_id)
                        loop = false;
                } else g_string_append_c(name, c);
                continue;
            }
        }

        // Create Entry
        if (debug) printf("  %s: %s\n", id->str, name->str);
        Entry * entry = Entry_new();
        Entry_set_name(entry, name->str);
        entry->steam_id = g_strdup(id->str);
        Entries_append(steam_entries, entry);

        // Clean up on Entry
        g_string_free(id, TRUE);
        g_string_free(name, TRUE);
        g_string_free(word, TRUE);
        g_free(full_path);
        fclose(file);
    }

    // Clean up
    g_string_free(name_key, TRUE);
    g_string_free(appid_key, TRUE);
    g_dir_close(steamapps);
    g_free(steamapps_str);
}

