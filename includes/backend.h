#ifndef BACKEND_H
#define BACKEND_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char *key;
    const char *value;
    char *(*func_encode)(const char *input, const char *password);
    char *(*func_decode)(const char *input, const char *password);
    bool requires_password;
} FormatEntry; // king of all structs


extern const char *version;
extern FormatEntry format_map[];
extern const size_t format_map_count;

const char *lookup(const char *key, FormatEntry *map, size_t map_len);
int  get_index(const char *key, FormatEntry *map, size_t map_len);
const char *encode(const char *format, const char *input, const char *password);
const char *decode(const char *format, const char *input, const char *password);
void handle_args(int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif // BACKEND_H
