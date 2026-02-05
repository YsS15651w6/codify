#include "pigl.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int is_vowel(char c) {
    c = (char)tolower((unsigned char)c);
    return c=='a'||c=='e'||c=='i'||c=='o'||c=='u';
}

static void encode_word(const char *w, char *out) {
    if (!isalpha((unsigned char)w[0])) {      // skip non-alphabetic tokens
        strcpy(out, w);
        return;
    }
    if (is_vowel(w[0]))
        sprintf(out, "%sway", w);
    else
        sprintf(out, "%s%cay", w+1, w[0]);
}

static void decode_word(const char *w, char *out) {
    size_t len = strlen(w);
    if (len > 3 && strcmp(w + len - 3, "way") == 0) {
        strncpy(out, w, len - 3);
        out[len - 3] = '\0';
    } else if (len > 2 && strcmp(w + len - 2, "ay") == 0) {
        char first = w[len - 3];
        out[0] = first;
        strncpy(out + 1, w, len - 3);
        out[len - 2] = '\0';
    } else {
        strcpy(out, w);
    }
}

char *pigl_encode(const char *input) {
    if (!input) return NULL;
    size_t len = strlen(input);
    char *result = (char *)malloc(len * 4 + 1);    // generous buffer
    if (!result) return NULL;
    result[0] = '\0';

    const char *p = input;
    char word[256], tmp[256];
    while (*p) {
        size_t wslen = strspn(p, " \t\n\r");
        strncat(result, p, wslen);          // preserve whitespace
        p += wslen;

        if (*p == '\0') break;

        size_t wlen = strcspn(p, " \t\n\r");
        if (wlen >= sizeof(word)) wlen = sizeof(word)-1;
        strncpy(word, p, wlen);
        word[wlen] = '\0';

        encode_word(word, tmp);
        strcat(result, tmp);

        p += wlen;
    }
    return result;
}

char *pigl_decode(const char *input) {
    if (!input) return NULL;
    size_t len = strlen(input);
    char *result = (char *)malloc(len + 1);
    if (!result) return NULL;
    result[0] = '\0';

    const char *p = input;
    char word[256], tmp[256];
    while (*p) {
        size_t wslen = strspn(p, " \t\n\r");
        strncat(result, p, wslen);
        p += wslen;

        if (*p == '\0') break;

        size_t wlen = strcspn(p, " \t\n\r");
        if (wlen >= sizeof(word)) wlen = sizeof(word)-1;
        strncpy(word, p, wlen);
        word[wlen] = '\0';

        decode_word(word, tmp);
        strcat(result, tmp);

        p += wlen;
    }
    return result;
}

char *pigl_encode_wrap(const char *input, const char *password) {
    (void)password;
    return pigl_encode(input);
}

char *pigl_decode_wrap(const char *input, const char *password) {
    (void)password;
    return pigl_decode(input);
}
