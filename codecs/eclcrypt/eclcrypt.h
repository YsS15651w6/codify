#ifndef ECLCRYPT_H
#define ECLCRYPT_H

#ifdef __cplusplus
extern "C" {
#endif

char *eclcrypt_encode(const char *input);
char *eclcrypt_decode(const char *input);

#ifdef __cplusplus
}
#endif

#endif // ECLCRYPT_H