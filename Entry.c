#include <stdlib.h>
#include <stdio.h>

#include "launcher.h"
#include "Entry.h"
#include "util.h"

Entry * Entry_new() {
    Entry * entry = malloc(sizeof(Entry));
    entry->id = 0;
    entry->name = "<NULL Entry>";
    entry->image_path = NULL; 
    entry->exec = NULL; 
    entry->cd = NULL;
    entry->steam_id = 0;
    entry->image = NULL;
    entry->event_box = NULL;
    return entry;
}

Entries * Entries_new() {
    Entries * entries = malloc(sizeof(Entries));
    entries->size = 0;
    entries->head = NULL;
    entries->tail = NULL;
}

void Entries_insert(Entries * entries, Entry * entry) {
    Node * node = malloc(sizeof(Node));
    node->entry = entry;
    node->next = NULL;
    if (entries->head) {
        entries->tail->next = node;
        node->prev = entries->tail;
    } else {
        entries->head = node;
        node->prev = NULL;
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
        Entry * entry = node->entry;
        g_free(entry->name);
        g_free(entry->image_path);
        if (entry->image) g_object_unref(entry->image);
        if (entry->event_box) g_object_unref(entry->event_box);
        if (entry->exec) g_free(entry->exec);
        if (entry->cd) g_free(entry->cd);
        free(entry);

        next = node->next;
        free(node);
    }
}

Entries * Entries_clear_container(GtkContainer * container, Entries * entries) {
    Node * node = entries->head;
    for (Node * next = node; next; node = next) {
        gtk_container_remove(container, node->entry->event_box);
        next = node->next;
    }
}

Entries * Entries_filter(Entries * entries, const char * filter) {
    char * uc_filter = g_utf8_strup(filter, -1);
    Entries * filtered = Entries_new();
    Node * node = entries->head;
    for (Node * next = node; next; node = next) {
        Entry * e = node->entry;
        char * uc_entry_name = g_utf8_strup(e->name, -1);
        if (starts_with(uc_entry_name, uc_filter)) {
            if (debug) printf("  %s\n", e->name);
            Entries_insert(filtered, e);
        }
        g_free(uc_entry_name);
        next = node->next;
    }
    return filtered;
}

