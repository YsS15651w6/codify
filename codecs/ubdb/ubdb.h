#ifndef UBDB_H
#define UBDB_H

#ifdef __cplusplus
extern "C" {
#endif

char *ubdb_encode(const char *input);
char *ubdb_decode(const char *input);
char *ubdb_encode_wrap(const char *input, const char *password);
char *ubdb_decode_wrap(const char *input, const char *password);
#ifdef __cplusplus
}
#endif

#endif // UBDB_H