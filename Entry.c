#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "launcher.h"
#include "main_window.h"
#include "Entry.h"
#include "util.h"
#include "steam.h"

Entry * Entry_new() {
    return calloc(sizeof(Entry), 1);
}

void Entry_delete(Entry * entry) {
    if (entry) {
        if (debug) printf("Deleting entry %s: %s\n", entry->id, entry->name);

        if (entry->id) g_free(entry->id);
        if (entry->name) g_free(entry->name);
        if (entry->uc_name) g_free(entry->uc_name);
        if (entry->image) g_free(entry->image);

        if (entry->exec) g_free(entry->exec);
        if (entry->cd) g_free(entry->cd);
        if (entry->last_ran) free(entry->last_ran);

        if (entry->fixed_widget) g_object_unref(entry->fixed_widget);
        if (entry->image_widget) g_object_unref(entry->image_widget);
        if (entry->event_box) g_object_unref(entry->event_box);

        free(entry);
    }
}

void Entry_set_name(Entry * entry, const char * name) {
    if (entry->name) g_free(entry->name);
    if (entry->uc_name) g_free(entry->uc_name);
    entry->name = g_strdup(name);
    entry->uc_name = g_utf8_strup(name, -1);
}

bool Entry_is_valid(Entry * entry) {
    if (!entry->id) {
        return false;
    }
    if (entry->name && entry->uc_name) {
        if (!(entry->name[0] && entry->uc_name[0])) {
            return false;
        }
    } else {
        return false;
    }
    // TODO finish validation
    return true;
}

void Entry_run(Entry * entry) {
    char * exec = NULL;

    if (entry->exec) {
        if (entry->cd) {
            chdir(entry->cd);
        }
        exec = malloc(6 + strlen(entry->exec));
        sprintf(exec, "exec %s", entry->exec);
    } else if (entry->steam_id) {
        exec = malloc(28 + strlen(steam_path) + strlen(entry->steam_id));
        sprintf(exec, "exec %s/steam.sh steam://run/%s", steam_path, entry->steam_id);
    }

    if (exec) {
        if (debug) {
            printf("Would run \"%s\": %s\n", entry->name, exec);
        } else {
            // Update Entry
            entry->count++;
            if (entry->last_ran) g_free(entry->last_ran);
            entry->last_ran = get_time_string();
            Entries_save(entries_file);

            // execl wants a file to run AND what to set as argv[0]
            execl("/bin/sh", "/bin/sh", "-c", exec, NULL);
            // If we get here, execl had a problem running sh...
            fprintf(stderr, "Could not run \"%s\": %s\n", entry->name, exec);
        }
    } else {
        fprintf(stderr,
            "\"%s\" (%s) does not have a command"
            " or steam appid, can not run.\n",
            entry->name,
            entry->id
        );
    }

    free(exec);
}

Entries * Entries_new() {
    return calloc(sizeof(Entries), 1);
}

void Entries_append(Entries * entries, Entry * entry) {
    Node * node = malloc(sizeof(Node));
    node->entry = entry;
    node->next = NULL;
    if (entries->head) {
        entries->tail->next = node;
    } else {
        entries->head = node;
    }
    entries->tail = node;
    entries->size++;
}

void Entries_remove(Entries * entries, Entry * entry) {
    Node * prev = NULL;
    for (Node * node = entries->head; node; node = node->next) {
        if (node->entry == entry) {
            if (prev) prev->next = node->next;
            if (node == entries->head) entries->head = node->next;
            if (node == entries->tail) entries->tail= prev;
            entries->size--;
            free(node);
            return;
        }
        prev = node;
    }
}

void Entries_delete(Entries * entries) {
    Node * next = entries->head;
    for (Node * node = next; node; node = next) {
        next = node->next;
        free(node);
    }
}

void Entries_delete_all(Entries * entries) {
    Node * next = entries->head;
    for (Node * node = next; node; node = next) {
        Entry_delete(node->entry);
        next = node->next;
        free(node);
    }
}

Entries * Entries_clear_container(GtkContainer * container, Entries * entries) {
    for (Node * node = entries->head; node; node = node->next) {
        gtk_container_remove(container, node->entry->fixed_widget);
    }
}

bool Entries_load(Entries * entries, const gchar * path) {
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

            // id
            entry->id = g_strdup(groups[i]);

            // name
            Entry_set_name(entry,
                g_key_file_get_string(ini, groups[i], "name", NULL)
            );

            // count
            entry->count = g_key_file_get_integer(ini, groups[i], "count", NULL);

            // image
            entry->image = g_key_file_get_string(ini, groups[i], "image", NULL);

            // last_ran
            if (g_key_file_has_key(ini, groups[i], "last_ran", NULL)) {
                entry->last_ran = g_key_file_get_string(ini, groups[i], "last_ran", NULL);
            }

            // Run Information
            if (g_key_file_has_key(ini, groups[i], "exec", NULL)) {
                entry->exec = g_key_file_get_string(ini, groups[i], "exec", NULL);
            }
            if (g_key_file_has_key(ini, groups[i], "cd", NULL)) {
                entry->cd = g_key_file_get_string(ini, groups[i], "cd", NULL);
            }
            if (g_key_file_has_key(ini, groups[i], "steam_id", NULL)) {
                entry->steam_id = g_key_file_get_string(
                    ini, groups[i], "steam_id", NULL
                );
                entry->downloaded_image = g_key_file_get_string(
                    ini, groups[i], "downloaded_image", NULL
                );
                if (!entry->downloaded_image) download_images_count++;
            }

            if (debug) {
                printf("  %s: \"%s\"\n",
                    groups[i], entry->name
                );
            }
            Entries_append(entries, entry);
        }
    } else {
        error = true;
    }
    g_key_file_free(ini);
    return error;
}

Entries * Entries_filter(Entries * entries, const char * filter) {
    char * uc_filter = g_utf8_strup(filter, -1);
    Entries * filtered = Entries_new();
    for (Node * node = entries->head; node; node = node->next) {
        Entry * e = node->entry;
        if (starts_with(e->uc_name, uc_filter)) {
            if (debug) printf("  %s\n", e->name);
            Entries_append(filtered, e);
        }
    }
    return filtered;
}

/*
 * Return true if "a" goes first, else returns false.
 */
bool Entry_compare(Entry * a, Entry * b) {
    bool more_ran = a->count > b->count;
    bool less_ran = a->count < b->count;
    switch (sort_by) {
        case LAST_RAN:
            if (a->last_ran && !b->last_ran)
                return true;
            else if (!a->last_ran && b->last_ran)
                return false;
            else if (a->last_ran && b->last_ran)
                return compare_time_strings(a->last_ran, b->last_ran);
            break;
        case MOST_RAN:
            if (more_ran)
                return true;
            else if (less_ran)
                return false;
            break;
        case LEAST_RAN:
            if (more_ran)
                return false;
            else if (less_ran)
                return true;
            break;
    };
    return g_utf8_collate(a->uc_name, b->uc_name) < 0;
}

Node * _merge(Node * a, Node * b, Node ** _tail) {
    if (!(a && b)) return a ? a : b;
    Node * head = NULL;
    Node * tail = NULL;
    while (a && b) {
        if (Entry_compare(a->entry, b->entry)) {
            if (head) {
                tail->next = a;
            } else  {
                head = a;
            }
            tail = a;
            a = a->next;
        } else {
            if (head) {
                tail->next = b;
            } else  {
                head = b;
            }
            tail = b;
            b = b->next;
        }
    }
    tail->next = a ? a : b;
    *_tail = tail;
    return head;
}

void Entries_sort(Entries * entries) {
    if (entries->size == 0 || entries->size == 1) return;

    size_t rsize = entries->size + entries->size % 2;
    Node ** roster = malloc(sizeof(Node*) * rsize);
    if (entries->size % 2) {
        roster[entries->size] = NULL;
    }
    Node ** ri = roster;
    Node * next;
    for (Node * i = entries->head; i; i = next) {
        *ri = i;
        ri++;
        next = i->next;
        i->next = NULL;
    }

    size_t subsize = 1;
    while (subsize <= rsize) {
        subsize *= 2;
        for (size_t i = 0; i+(subsize/2) < rsize; i += subsize) {
            roster[i] = _merge(roster[i], roster[i+(subsize/2)], &entries->tail);
        }
    }

    entries->head = roster[0];

    free(roster);
}

void Entries_insert_steam() {
    if (debug) printf("Checking Previous Steam Entries:\n");
    for (Node * n = all_entries->head; n; n = n->next) {
        if (n->entry->steam_id) {
            bool found = false;
            for (Node * sn = steam_entries->head; sn; sn = sn->next) {
                if (!strcmp(n->entry->steam_id, sn->entry->steam_id)) {
                    found = true;
                    break;
                }
            }
            n->entry->disabled = !found;
        }
    }
    if (debug) printf("Including New Steam Entries:\n");
    for (Node * snode = steam_entries->head; snode; snode = snode->next) {
        bool found = false;
        for (Node * n = all_entries->head; n; n = n->next) {
            if (n->entry->steam_id) {
                if (!strcmp(n->entry->steam_id, snode->entry->steam_id)) {
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            Entry * entry = snode->entry;
            if (debug) printf("  %s: %s\n", entry->steam_id, entry->name);
            Entries_append(all_entries, entry);
            entries_changed = true;
            download_images_count++;

            // id
            char id[64];
            unsigned id_len = sprintf(&id[0], "%u", next_id++);
            entry->id = g_strdup(&id[0]);

            // image
            char * image = malloc(id_len + 5);
            sprintf(image, "%s.jpg", entry->id);
            entry->image = g_strdup(image);
            free(image);

            // "Remove" from steam_entries
            snode->entry = NULL;
        }
    }
}

bool Entries_save(const char * path) {
    bool had_error = false;
    GKeyFile * ini = g_key_file_new();

    g_key_file_set_integer(ini, "meta", "next_id", next_id);

    for (Node * node = all_entries->head; node; node = node->next) {
        Entry * e = node->entry;
        g_key_file_set_string(ini, e->id, "name", e->name);
        g_key_file_set_string(ini, e->id, "image", e->image);
        g_key_file_set_integer(ini, e->id, "count", e->count);
        g_key_file_set_boolean(ini, e->id, "favorite", e->favorite);
        if (e->last_ran)
            g_key_file_set_string(ini, e->id, "last_ran", e->last_ran);
        if (e->exec)
            g_key_file_set_string(ini, e->id, "exec", e->exec);
        if (e->cd)
            g_key_file_set_string(ini, e->id, "cd", e->cd);
        if (e->steam_id) {
            g_key_file_set_string(ini, e->id, "steam_id", e->steam_id);
            g_key_file_set_boolean(ini, e->id,
                "downloaded_image", e->downloaded_image
            );
        }
    }

    // Save entries to file
    GError * error = NULL;
    if (!g_key_file_save_to_file(ini, path, &error)) {
        had_error = true;
        g_error("Could not save entries to %s: \"%s\"\n",
            path, error->message
        );
        g_error_free(error);
    }
    g_key_file_free(ini);
    return had_error;
}
