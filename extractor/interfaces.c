#include "interfaces.h"

#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <stdlib.h>

wl_interface_entry *wl_interface_entries_load(FILE *symbols_list, const char *lib_path) {
    // Read file with format: 00012b4c D input_method_context_interface
    // Skip the first column, use the second for external flag, and the third for the name
    char *line = NULL;
    size_t len;
    ssize_t nread;
    wl_interface_entry *list = NULL;
    wl_interface_entry *prev = NULL;
    while ((nread = getline(&line, &len, symbols_list)) != -1) {
        size_t str_len = nread;
        if (line[str_len - 1] == '\n') {
            str_len--;
        }
        const char *suffix = "_interface";
        if (strncmp(line + str_len - 10, suffix, 10) != 0) {
            continue;
        }
        wl_interface_entry *cur = calloc(1, sizeof(wl_interface_entry));
        cur->name = strndup(line, str_len);
        cur->interface = dlsym(RTLD_DEFAULT, cur->name);
        Dl_info dl_info;
        dladdr(cur->interface, &dl_info);
        cur->defined = strcmp(dl_info.dli_fname, lib_path) == 0;
        if (prev != NULL) {
            prev->next = cur;
        }
        if (list == NULL) {
            list = cur;
        }
        prev = cur;
    }
    free(line);

    return list;
}

void wl_interface_entries_free(wl_interface_entry *entries) {
    wl_interface_entry *cur = entries;
    while (cur != NULL) {
        wl_interface_entry *next = cur->next;
        free(cur->name);
        free(cur);
        cur = next;
    }
}
