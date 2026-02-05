#ifndef ECLCRYPT_H
#define ECLCRYPT_H

#ifdef __cplusplus
extern "C" {
#endif

char *eclcrypt_encode(const char *input);
char *eclcrypt_decode(const char *input);
char *eclcrypt_encode_wrap(const char *input, const char *password);
char *eclcrypt_decode_wrap(const char *input, const char *password);
#ifdef __cplusplus
}
#endif

#endif // ECLCRYPT_H