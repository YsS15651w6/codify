#ifndef UBDB_H
#define UBDB_H

#ifdef __cplusplus
extern "C" {
#endif

char *ubdb_encode(const char *input);
char *ubdb_decode(const char *input);

#ifdef __cplusplus
}
#endif

#endif // UBDB_H