#ifndef ENTRY_HEADER
#define ENTRY_HEADER

#include <gtk/gtk.h> // GtkWidget

/* ===========================================================================
 * Represents a Launchable Entry
 *
 * Entires must have an id, name, image_path, and either a exec or a steam_id.
 * cd is optional and is ignored if it is a steam entry.
 */
typedef struct Entry_struct Entry;
struct Entry_struct {
    // Common
    unsigned id;
    char * name;
    char * image_path;

    // Run through exec
    char * exec;
    char * cd;

    // Run through "steam steam://run/<appid>"
    char * steam_id;

    // GUI Elements
    GtkWidget * image;
    GtkWidget * event_box;
};

/*
 * Create a new, unfilled Entry on the clib heap
 */
Entry * Entry_new();

/*
 * Valididate Entry values and can be run "afaik"
 */
bool Entry_is_valid(Entry * entry);

/* ===========================================================================
 * Double Linked List of Entries
 */
typedef struct Entries_struct Entries;
struct Entries_struct {
    unsigned size;
    struct Node_struct * head;
    struct Node_struct * tail;
};

typedef struct Node_struct Node;
struct Node_struct {
    struct Entry_struct * entry;
    struct Node_struct * next;
    struct Node_struct * prev;
};

/*
 * Create emtpy Entry List
 */
Entries * Entries_new();

/*
 * Insert Entry into Entry List
 *
 * Automatically manages nodes
 */
void Entries_insert(Entries * entries, Entry * entry);

/*
 * Delete all the nodes and the Entry List, but NOT the Entries
 */
void Entries_delete(Entries * entries);

/*
 * Delete all the nodes and the Entry List, AND the Entries
 */
void Entries_delete_all(Entries * entries);

/*
 * Return new entry list with entries that start with *filter* ignoring case.
 */
Entries * Entries_filter(Entries * entries, const char * filter);

/*
 * Iterate through Entires and attempt to remove them from the Gtk Container
 */
Entries * Entries_clear_container(GtkContainer * container, Entries * entries);

#endif
