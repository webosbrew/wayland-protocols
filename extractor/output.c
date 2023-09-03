#include "output.h"

#include <stdlib.h>
#include <wayland-util.h>
#include <string.h>

static void write_message(const char *type, const struct wl_message *message, FILE *f);

static void write_args(const struct wl_message *message, FILE *f);

static void write_arg(char type, bool nullable, int index, const struct wl_interface **types, FILE *f);

void write_output(wl_interface_entry *interfaces, const char *protocol_name, FILE *f) {
    fprintf(f, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(f, "<protocol name=\"%s\">\n", protocol_name);
    for (wl_interface_entry *cur = interfaces; cur != NULL; cur = cur->next) {
        if (!cur->defined) {
            continue;
        }
        fprintf(f, "  <interface name=\"%s\" version=\"%d\">\n", cur->interface->name, cur->interface->version);
        for (int i = 0; i < cur->interface->method_count; i++) {
            write_message("request", &cur->interface->methods[i], f);
        }
        for (int i = 0; i < cur->interface->event_count; i++) {
            write_message("event", &cur->interface->events[i], f);
        }
        fprintf(f, "  </interface>\n");
    }
    fprintf(f, "</protocol>\n");
}

static void write_message(const char *type, const struct wl_message *message, FILE *f) {
    long since_version = strtol(message->signature, NULL, 10);
    const char *extras = "";
    if (strcmp(message->name, "destroy") == 0) {
        extras = " type=\"destructor\"";
    }
    if (since_version > 1) {
        fprintf(f, "    <%s name=\"%s\"%s since=\"%ld\">\n", type, message->name, extras, since_version);
    } else {
        fprintf(f, "    <%s name=\"%s\"%s>\n", type, message->name, extras);
    }
    write_args(message, f);
    fprintf(f, "    </%s>\n", type);
}

static void write_args(const struct wl_message *message, FILE *f) {
    const char *sig_start = message->signature;
    char *version_rem = NULL;
    strtol(message->signature, &version_rem, 10);
    if (version_rem != NULL) {
        sig_start = version_rem;
    }
    bool nullable = false;
    int arg_index = 0;
    for (; *sig_start; ++sig_start) {
        char type = *sig_start;
        switch (type) {
            case 'i':
            case 'u':
            case 'f':
            case 's':
            case 'o':
            case 'n':
            case 'a':
            case 'h':
                write_arg(type, nullable, arg_index++, message->types, f);
                nullable = false;
                break;
            case '?':
                nullable = true;
            default:
                continue;
        }
    }
}

static void write_arg(char type, bool nullable, int index, const struct wl_interface **types, FILE *f) {
    const char *type_name = NULL;
    switch (type) {
        case 'i':
            type_name = "int";
            break;
        case 'u':
            type_name = "uint";
            break;
        case 'f':
            type_name = "fixed";
            break;
        case 's':
            type_name = "string";
            break;
        case 'o':
            type_name = "object";
            break;
        case 'n':
            type_name = "new_id";
            break;
        case 'a':
            type_name = "array";
            break;
        case 'h':
            type_name = "fd";
            break;
        default:
            return;
    }
    fprintf(f, "      <arg name=\"p%d\" type=\"%s\"", index, type_name);
    const struct wl_interface *interface = types[index];
    if (interface != NULL) {
        fprintf(f, " interface=\"%s\"", interface->name);
    }
    if (nullable) {
        fprintf(f, " allow-null=\"true\"");
    }
    fprintf(f, "/>\n");
}