#ifndef PIGL_H
#define PIGL_H

#ifdef __cplusplus
extern "C" {
#endif

// Encode/Decode an entire string to/from Pig Latin.
// Caller must free() the returned string.
char *pigl_encode(const char *input);
char *pigl_decode(const char *input);

#ifdef __cplusplus
}
#endif

#endif // PIGL_H
