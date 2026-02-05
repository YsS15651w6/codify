#include "eclcrypt_pwd.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// copy of eclcrypt but with password func

static const char* map[256] = {
    ['A'] = "⅃",   
    ['B'] = "⊔",
    ['C'] = "L",   
    ['D'] = "]",      
    ['E'] = "□",    
    ['F'] = "[",     
    ['G'] = "⅂",   
    ['H'] = "⊓",  
    ['I'] = "⎾", 
    ['J'] = "·⅃",
    ['K'] = "·⊔",
    ['L'] = "·",
    ['M'] = "·]",
    ['N'] = "·□·",
    ['O'] = "·[",
    ['P'] = "·⅂",
    ['Q'] = "·⊓·",
    ['R'] = "·⎾",
    ['S'] = "V", 
    ['T'] = "<",
    ['U'] = ">",
    ['V'] = "Ʌ",
    ['W'] = "·V·",
    ['X'] = "<·",
    ['Y'] = "·>",
    ['Z'] = "·Ʌ·",
    ['0'] = ")",
    ['1'] = "!",
    ['2'] = "@",
    ['3'] = "#",
    ['4'] = "$",
    ['5'] = "%", // learned my lesson
    ['6'] = "^",
    ['7'] = "&",
    ['8'] = "*",
    ['9'] = "(",
    [')'] = "0",
    ['!'] = "1",
    ['@'] = "2",
    ['#'] = "3",
    ['$'] = "4",
    ['%'] = "5",
    ['^'] = "6",
    ['&'] = "7",
    ['*'] = "8",
    ['('] = "9"     
}; // base map... add U2060 joiner delimiter & move with seed from pwd

const char *DELIMITER = "\xe2\x81\xa0"; // said thing

typedef struct TrieNode {
    unsigned char letter; // original letter
    struct TrieNode *children[256];
    int is_leaf;
} TrieNode;

static TrieNode* create_node() {
    TrieNode* node = calloc(1, sizeof(TrieNode));
    return node;
}

static void trie_insert(TrieNode *root, const char *encoded, unsigned char letter) { 
    TrieNode *node = root;
    for (size_t i = 0; encoded[i]; i++) {
        unsigned char c = (unsigned char)encoded[i];
        if (!node->children[c]) node->children[c] = create_node();
        node = node->children[c];
    }
    node->is_leaf = 1;
    node->letter = letter;
}

static void trie_free(TrieNode *node) {
    if (!node) return;
    for (int i = 0; i < 256; i++) trie_free(node->children[i]);
    free(node); 
}

// Decode a single "word" using the trie
static void decode_word_trie(TrieNode *root, const char *w, char *out) {
    size_t out_idx = 0;
    const char *p = w;

    while (*p) {
        TrieNode *node = root;
        const char *match_start = p;
        TrieNode *last_match = NULL;
        const char *last_match_end = NULL;

        while (*p && node->children[(unsigned char)*p]) {
            node = node->children[(unsigned char)*p];
            p++;
            if (node->is_leaf) {
                last_match = node;
                last_match_end = p;
            }
        }

        if (last_match) {
            out[out_idx++] = last_match->letter;
            p = last_match_end;
        } else {
            out[out_idx++] = *match_start;
            p = match_start + 1;
        }
    }

    out[out_idx] = '\0';
}

static int get_seed_from_password(const char *password) {
    int seed = 0;
    for (size_t i = 0; password[i]; i++) {
        seed += (unsigned char)password[i]; // more weird shit
    }
    return seed/strlen(password);
}

void shuffle_map(const char **shuffled_map, int seed) {
    // copy base map first
    for (int i = 0; i < 256; i++)
        shuffled_map[i] = map[i];

    srand(seed);

    // God knows what this does... not me. Have a word with Him if you don't like it.
    for (int i = 255; i > 0; i--) {
        int j = rand() % (i + 1);
        const char *tmp = shuffled_map[i];
        shuffled_map[i] = shuffled_map[j];
        shuffled_map[j] = tmp;
    }
}


// publicly faced shit
char *eclcrypt_pwd_encode(const char *input, const char *password) {
    if (!input) return NULL;
    size_t len = strlen(input);
    int seed = get_seed_from_password(password);
    const char *shuffled_map[256];
    shuffle_map(shuffled_map, seed);
    char *result = malloc(len * 8 + 1); // make room for delimiter per letter
    if (!result) return NULL;
    result[0] = '\0';

    const char *p = input;
    char word[256], tmp[256];
    while (*p) {
        // skip leading whitespace
        size_t wslen = strspn(p, " \t\n\r");
        p += wslen;
        if (*p == '\0') break;

        // get next word
        size_t wlen = strcspn(p, " \t\n\r");
        if (wlen >= sizeof(word)) wlen = sizeof(word) - 1;
        strncpy(word, p, wlen);
        word[wlen] = '\0';

        // encode each letter
        tmp[0] = '\0';
        for (size_t i = 0; word[i]; i++) {
            unsigned char c = (unsigned char)word[i];
            if (c >= 'a' && c <= 'z') c -= 32; // lowercase → uppercase
            const char *replacement = shuffled_map[c];
            if (replacement) {
                strcat(tmp, replacement);
                strcat(tmp, DELIMITER); // invisible delimiter between letters
            } else {
                size_t len_tmp = strlen(tmp);
                tmp[len_tmp] = word[i];
                tmp[len_tmp+1] = '\0';
                strcat(tmp, DELIMITER); // delimiter even for unknown chars
            }
        }

        strcat(result, tmp);
        strcat(result, " "); // keep word separation
        p += wlen;
    }

    // trim trailing space
    size_t res_len = strlen(result);
    if (res_len > 0 && result[res_len-1] == ' ')
        result[res_len-1] = '\0';

    return result;
}

char *eclcrypt_pwd_decode(const char *input, const char *password) {
    if (!input) return NULL;
    size_t len = strlen(input);
    char *result = malloc(len + 1);
    int seed = get_seed_from_password(password);
    const char *shuffled_map[256];
    shuffle_map(shuffled_map, seed);
    if (!result) return NULL;
    result[0] = '\0';

    // Build the trie
    TrieNode *root = create_node();
    for (int i = 0; i < 256; i++) {
        if (shuffled_map[i]) trie_insert(root, shuffled_map[i], (unsigned char)i);
    }

    const char *p = input;
    char tmp[256];
    size_t delim_len = strlen(DELIMITER);

    while (*p) {
        // skip spaces between words
        if (*p == ' ') {
            strcat(result, " ");
            p++;
            continue;
        }

        // grab next letter token
        const char *next = strstr(p, DELIMITER);
        if (!next) break; // end of string
        size_t tok_len = next - p;
        if (tok_len >= sizeof(tmp)) tok_len = sizeof(tmp)-1;
        strncpy(tmp, p, tok_len);
        tmp[tok_len] = '\0';

        // decode single letter using trie
        char decoded[2] = {0};
        decode_word_trie(root, tmp, decoded);
        strcat(result, decoded);

        p = next + delim_len;
    }

    trie_free(root);
    return result;
}

char *eclcrypt_pwd_encode_wrap(const char *input, const char *password) {
    return eclcrypt_pwd_encode(input, password);
}
char *eclcrypt_pwd_decode_wrap(const char *input, const char *password) {
    return eclcrypt_pwd_decode(input, password);
}