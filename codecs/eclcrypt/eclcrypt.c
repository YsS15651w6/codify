#include "eclcrypt.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// bigass private shit... more like diarrhea

// bigass mapping table
// probably not the most efficient way to store this but whatever
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
    ['5'] = "%%", // c likes to think my percentages are formatters... tell it to stfu with TWO percent signs
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
}; // base map... add U2060 joiner delimiter

const char *DELIM = "\xe2\x81\xa0"; // said delimiter

// trie shit... FML
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
    // when i wrote this, only god and i understood what it does
    // now, only god knows
    // totally not a trie, just a linked list of nodes for each possible starting character, but it works so who tf cares
    // amt_hrs_wasted_on_this: 2
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
    for (int i = 0; i < 256; i++) trie_free(node->children[i]); // insert hide the pain harold meme here
    free(node); // frees it. you're welcome. (i hate myself)
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

// publicly faced shit
char *eclcrypt_encode(const char *input) {
    if (!input) return NULL;
    size_t len = strlen(input);
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
            const char *replacement = map[c];
            if (replacement) {
                strcat(tmp, replacement);
                strcat(tmp, DELIM); // invisible delimiter between letters
            } else {
                size_t len_tmp = strlen(tmp);
                tmp[len_tmp] = word[i];
                tmp[len_tmp+1] = '\0';
                strcat(tmp, DELIM); // delimiter even for unknown chars
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

char *eclcrypt_decode(const char *input) {
    if (!input) return NULL;
    size_t len = strlen(input);
    char *result = malloc(len + 1);
    if (!result) return NULL;
    result[0] = '\0';

    // Build the trie
    TrieNode *root = create_node();
    for (int i = 0; i < 256; i++) {
        if (map[i]) trie_insert(root, map[i], (unsigned char)i);
    }

    const char *p = input;
    char tmp[256];
    size_t delim_len = strlen(DELIM);

    while (*p) {
        // skip spaces between words
        if (*p == ' ') {
            strcat(result, " ");
            p++;
            continue;
        }

        // grab next letter token
        const char *next = strstr(p, DELIM);
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
