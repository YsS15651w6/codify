#ifndef ECLCRYPT_PWD_H
#define ECLCRYPT_PWD_H

#ifdef __cplusplus
extern "C" {
#endif

char *eclcrypt_pwd_encode(const char *input, const char *password);
char *eclcrypt_pwd_decode(const char *input, const char *password);
char *eclcrypt_pwd_encode_wrap(const char *input, const char *password);
char *eclcrypt_pwd_decode_wrap(const char *input, const char *password); // wrappers... not needed but goes with the flow
#ifdef __cplusplus
}
#endif

#endif // ECLCRYPT_PWD_H