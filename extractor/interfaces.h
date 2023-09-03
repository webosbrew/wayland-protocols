#pragma once

#include <stdbool.h>
#include <stdio.h>

typedef struct wl_interface_entry {
    char *name;
    bool defined;
    const struct wl_interface *interface;
    struct wl_interface_entry *next;
} wl_interface_entry;

wl_interface_entry *wl_interface_entries_load(FILE *symbols_list, const char *lib_path);

void wl_interface_entries_free(wl_interface_entry *entries);
