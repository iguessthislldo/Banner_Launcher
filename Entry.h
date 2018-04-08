#ifndef ENTRY_HEADER
#define ENTRY_HEADER

#include <stdbool.h>
#include <stdint.h>

#include <gtk/gtk.h>

/* ===========================================================================
 * Represents a Launchable Entry
 *
 * Entires must have an id, name, image_path, and either a exec or a steam_id.
 * cd is optional and is ignored if it is a steam entry.
 */
typedef struct Entry_struct Entry;
struct Entry_struct {
    // Common
    char * id;
    char * name;
    char * uc_name;
    char * image_path;

    // Sort
    unsigned count; // Number of times this Entry has been run
    bool favorite;
    char * last_ran; // As YYYYMMDDhhmmss

    // Run through exec
    char * exec;
    char * cd;

    // Run through "steam steam://run/<appid>"
    char * steam_id;
    bool downloaded_image;

    // GUI Elements
    GtkWidget * image;
    GtkWidget * event_box;
};

/*
 * Create a new, unfilled Entry on the clib heap
 */
Entry * Entry_new();

/*
 * Delete an Entry struct and it's resources
 */
void Entry_delete(Entry * entry);

/*
 * Set the name and the uppercase name as copies at the same time.
 */
void Entry_set_name(Entry * entry, const char * name);

/*
 * Valididate Entry values and can be run "afaik"
 */
bool Entry_is_valid(Entry * entry);

/*
 * Run an entry, should not return
 */
void Entry_run(Entry * entry);

/* ===========================================================================
 * Linked List of Entries
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
};

/*
 * Create emtpy Entry List
 */
Entries * Entries_new();

/*
 * Append Entry onto Entry List
 */
void Entries_append(Entries * entries, Entry * entry);

/*
 * Delete all the nodes and the Entry List, but NOT the Entries
 */
void Entries_delete(Entries * entries);

/*
 * Delete all the nodes and the Entry List, AND the Entries
 */
void Entries_delete_all(Entries * entries);

/*
 * Load Entries from ini file
 */
bool Entries_load(Entries * entries, const gchar * path);

/*
 * Return new entry list with entries that start with *filter* ignoring case.
 */
Entries * Entries_filter(Entries * entries, const char * filter);

/*
 * Iterate through Entires and attempt to remove them from the Gtk Container
 */
Entries * Entries_clear_container(GtkContainer * container, Entries * entries);

/*
 * Sort Entries in place
 */
void Entries_sort(Entries * entries);

/*
 * Include Steam Entries
 */
void Entries_insert_steam();

/*
 * Save Entries to "path"
 */
bool Entries_save(const char * path);

#endif
