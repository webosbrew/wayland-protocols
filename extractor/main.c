#include <stdio.h>
#include <wayland-util.h>
#include <dlfcn.h>
#include <link.h>

#include "interfaces.h"
#include "output.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <library> <protocol-name>\n", argv[0]);
        return 1;
    }
    if (dlopen("libwayland-client.so.0", RTLD_GLOBAL | RTLD_NOW) == NULL) {
        fprintf(stderr, "Failed to load libwayland-client.so.0: %s\n", dlerror());
        return 1;
    }
    void *lib_handle;
    if ((lib_handle = dlopen(argv[1], RTLD_GLOBAL | RTLD_NOW)) != NULL) {
        fprintf(stderr, "Loaded %s\n", argv[1]);
    } else {
        fprintf(stderr, "Failed to load %s: %s\n", argv[1], dlerror());
        return 1;
    }
    struct link_map *link_map = NULL;
    if (dlinfo(lib_handle, RTLD_DI_LINKMAP, &link_map) != 0) {
        fprintf(stderr, "Failed to get link map for %s\n", argv[1]);
        return 1;
    }
    char strings_cmd[1024];
    snprintf(strings_cmd, sizeof(strings_cmd), "strings -n 11 %s", link_map->l_name);
    FILE *f = popen(strings_cmd, "r");
    if (!f) {
        fprintf(stderr, "Failed to run %s\n", strings_cmd);
        return 1;
    }
    wl_interface_entry *entries = wl_interface_entries_load(f, link_map->l_name);
    pclose(f);
    write_output(entries, argv[2], stdout);
    wl_interface_entries_free(entries);
    dlclose(lib_handle);
    return 0;
}