#ifndef ENTRY_HEADER
#define ENTRY_HEADER

#include <gtk/gtk.h>

/*
 * Represents a Launchable Entry
 */
typedef struct Entry_struct Entry;
struct Entry_struct {
    size_t id;
    gchar * name;
    gchar * image_path;
    gchar * exec;
    gchar * cd;
    size_t steam_id;
    GtkWidget * image;
};

Entry * Entry_new();

typedef struct Entries_struct Entries;
struct Entries_struct {
    size_t size;
    struct Node_struct * head;
    struct Node_struct * tail;
};

typedef struct Node_struct Node;
struct Node_struct {
    struct Entry_struct * entry;
    struct Node_struct * next;
    struct Node_struct * prev;
};

Entries * Entries_new();
void Entries_insert(Entries * entries, Entry * entry);
void Entries_delete(Entries * entries);
void Entries_delete_all(Entries * entries);

#endif
