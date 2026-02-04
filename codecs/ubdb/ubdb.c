#include "ubdb.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int is_vowel(char c) {
    c = (char)tolower((unsigned char)c);
    return c=='a'||c=='e'||c=='i'||c=='o'||c=='u';
}

static void encode_word(const char *w, char *out) {
    size_t j = 0;
    for (size_t i = 0; w[i]; i++) {
        if (is_vowel(w[i])) {
            out[j++] = 'u';
            out[j++] = 'b';
        }
        out[j++] = w[i];
    }
    out[j] = '\0';
}


static void decode_word(const char *w, char *out) {
    size_t i=0, j=0;
    while (w[i]) {
        if (w[i]=='u' && w[i+1]=='b' && is_vowel(w[i+2])) {
            i += 2;
        }
        out[j++] = w[i++];
    }
    out[j] = '\0';
}


char *ubdb_encode(const char *input) {
    if (!input) return NULL;
    size_t len = strlen(input);
    char *result = (char *)malloc(len * 4 + 1);
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

        encode_word(word, tmp);
        strcat(result, tmp);

        p += wlen;
    }

    return result;
}

char *ubdb_decode(const char *input) {
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