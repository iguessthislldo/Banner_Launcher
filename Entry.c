#include <stdlib.h>
#include <stdio.h>

#include "Entry.h"

Entry * Entry_new() {
    Entry * entry = malloc(sizeof(Entry));
    entry->id = 0;
    entry->name = "<NULL Entry>";
    entry->image_path = NULL; 
    entry->exec = NULL; 
    entry->cd = NULL;
    entry->image = NULL;
    entry->steam_id = 0;
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
        if (entry->exec) g_free(entry->exec);
        if (entry->cd) g_free(entry->cd);
        free(entry);

        next = node->next;
        free(node);
    }
}

