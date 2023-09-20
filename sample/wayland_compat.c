#include <wayland-util.h>
#include <wayland-client.h>

#include <memory.h>

uint32_t wl_proxy_get_version(struct wl_proxy *proxy)
{
    (void)proxy;
    return 0;
}

static int parse_msg_signature(const char *signature, int *new_id_index)
{
    int count = 0;
    for (; *signature; ++signature) {
        switch (*signature) {
            case 'n':
                *new_id_index = count;
                // Intentional fallthrough
            case 'i':
            case 'u':
            case 'f':
            case 's':
            case 'o':
            case 'a':
            case 'h':
                ++count;
        }
    }
    return count;
}

struct wl_proxy *wl_proxy_marshal_constructor(struct wl_proxy *proxy, uint32_t opcode,
                                                       const struct wl_interface *interface, ...)
{
    va_list ap;
    void *varargs[20 /*WL_CLOSURE_MAX_ARGS*/];
    int num_args;
    int new_id_index = -1;
    struct wl_interface *proxy_interface;
    struct wl_proxy *id = wl_proxy_create(proxy, interface);
    if (!id) {
        return NULL;
    }

    proxy_interface = (*(struct wl_interface **)proxy);
    if (opcode > proxy_interface->method_count) {
        return NULL;
    }
    num_args = parse_msg_signature(proxy_interface->methods[opcode].signature, &new_id_index);
    if (new_id_index < 0) {
        return NULL;
    }
    memset(varargs, 0, sizeof(varargs));
    va_start(ap, interface);
    for (int i = 0; i < num_args; i++) {
        varargs[i] = va_arg(ap, void *);
    }
    va_end(ap);
    varargs[new_id_index] = id;
    wl_proxy_marshal(proxy, opcode, varargs[0], varargs[1], varargs[2], varargs[3], varargs[4],
                     varargs[5], varargs[6], varargs[7], varargs[8], varargs[9],
                     varargs[10], varargs[11], varargs[12], varargs[13], varargs[14],
                     varargs[15], varargs[16], varargs[17], varargs[18], varargs[19]);
    return id;
}

struct wl_proxy *wl_proxy_marshal_constructor_versioned(struct wl_proxy *proxy, uint32_t opcode,
                                                                 const struct wl_interface *interface,
                                                                 uint32_t version, ...)
{
    va_list ap;
    void *varargs[20 /*WL_CLOSURE_MAX_ARGS*/];
    int num_args;
    int new_id_index = -1;
    struct wl_interface *proxy_interface;
    struct wl_proxy *id = wl_proxy_create(proxy, interface);
    if (!id) {
        return NULL;
    }

    proxy_interface = (*(struct wl_interface **)proxy);
    if (opcode > proxy_interface->method_count) {
        return NULL;
    }
    num_args = parse_msg_signature(proxy_interface->methods[opcode].signature, &new_id_index);
    if (new_id_index < 0) {
        return NULL;
    }
    memset(varargs, 0, sizeof(varargs));
    va_start(ap, version);
    for (int i = 0; i < num_args; i++) {
        varargs[i] = va_arg(ap, void *);
    }
    va_end(ap);
    varargs[new_id_index] = id;
    wl_proxy_marshal(proxy, opcode, varargs[0], varargs[1], varargs[2], varargs[3], varargs[4],
                     varargs[5], varargs[6], varargs[7], varargs[8], varargs[9],
                     varargs[10], varargs[11], varargs[12], varargs[13], varargs[14],
                     varargs[15], varargs[16], varargs[17], varargs[18], varargs[19]);
    return id;
}
