#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "launcher.h"
#include "Entry.h"
#include "util.h"

Entry * Entry_new() {
    return calloc(sizeof(Entry), 1);
}

void Entry_delete(Entry * entry) {
    if (entry->name) g_free(entry->name);
    if (entry->uc_name) g_free(entry->uc_name);
    if (entry->image_path) g_free(entry->image_path);
    if (entry->image) g_object_unref(entry->image);
    if (entry->event_box) g_object_unref(entry->event_box);
    if (entry->exec) g_free(entry->exec);
    if (entry->cd) g_free(entry->cd);
    free(entry);
}

void Entry_set_name(Entry * entry, const char * name) {
    if (entry->name) g_free(entry->name);
    if (entry->uc_name) g_free(entry->uc_name);
    entry->name = g_strdup(name);
    entry->uc_name = g_utf8_strup(name, -1);
}

void Entry_set_image(Entry * entry, const char * path) {
    if (entry->image_path) g_free(entry->image_path);
    entry->image_path = g_strdup(path);
    // TODO Set error icon if image is invalid
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
        gtk_container_remove(container, node->entry->event_box);
    }
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

bool Entry_compare(Entry * a, Entry * b) {
    // a is true, b is false
    if (a->count > b->count) {
        return true;
    } else if (a->count < b->count) {
        return false;
    }
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
