#ifndef ENTRY_HEADER
#define ENTRY_HEADER

#include <stdbool.h>
#include <stdint.h>

#include <gtk/gtk.h>

/* ===========================================================================
 * Represents a Launchable Entry
 */

typedef enum Entry_Type_enum Entry_Type;
enum Entry_Type_enum {
    ENTRY_TYPE_INVALID = 0,
    ENTRY_TYPE_SHELL,
    ENTRY_TYPE_STEAM,
    ENTRY_TYPE_XDG,

    ENTRY_TYPE_COUNT
};

const char const * entry_type_names[ENTRY_TYPE_COUNT];

// Specific Info for Entires Run through Shell
typedef struct Entry_Type_Info_Shell_struct Entry_Type_Info_Shell;
struct Entry_Type_Info_Shell_struct {
    char * exec;
    char * cd;
};

// Specific Info for Steam Entires
// Run through "steam steam://run/<appid>"
typedef struct Entry_Type_Info_Steam_struct Entry_Type_Info_Steam;
struct Entry_Type_Info_Steam_struct {
    char * steam_id;
    bool downloaded_image;
};

// Specific Info for Entries
typedef union Entry_Type_Info_union Entry_Type_Info;
union Entry_Type_Info_union {
    Entry_Type_Info_Shell shell;
    Entry_Type_Info_Steam steam;
};

typedef struct Entry_struct Entry;
struct Entry_struct {
    /*
     * Save/Load
     */

    // Common
    char * id;
    char * name;
    char * image;

    // Sort
    unsigned count; // Number of times this Entry has been run
    bool favorite;
    char * last_ran; // As YYYYMMDDhhmmss

    // How it is ran
    Entry_Type type;
    Entry_Type_Info info;

    /*
     * Runtime
     */
    char * uc_name; // Generated from name
    bool disabled;

    // GUI Elements
    GtkWidget * fixed_widget;
    GtkWidget * info_box;
    GtkWidget * image_widget;
    GtkWidget * event_box;
    GtkWidget * name_label;
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
 * Find and Remove entry for the Entries list
 */
void Entries_remove(Entries * entries, Entry * entry);

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
