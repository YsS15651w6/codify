#include "backend.h"

// codecs
#include "../codecs/pigl/pigl.h"
#include "../codecs/ubdb/ubdb.h"
#include "../codecs/eclcrypt/eclcrypt.h"
#include "../codecs/eclcrypt_pwd/eclcrypt_pwd.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *version = "1.2.0";

FormatEntry format_map[] = {
    {"pigl", "PIGL Encoding", pigl_encode_wrap, pigl_decode_wrap, false},
    {"ubdb", "UBDB Encoding", ubdb_encode_wrap, ubdb_decode_wrap, false},
    {"eclcrypt", "ECL Crypt Encoding", eclcrypt_encode_wrap, eclcrypt_decode_wrap, false},
    {"eclcrypt_pwd", "ECL Crypt with Password Encoding", eclcrypt_pwd_encode_wrap, eclcrypt_pwd_decode_wrap, true}
};

const size_t format_map_count = sizeof(format_map) / sizeof(format_map[0]);

const char *lookup(const char *key, FormatEntry *map, size_t map_len) {
    for (size_t i = 0; i < map_len; i++)
        if (strcmp(key, map[i].key) == 0)
            return map[i].value;
    return NULL;
}

int get_index(const char *key, FormatEntry *map, size_t map_len) {
    for (size_t i = 0; i < map_len; i++)
        if (strcmp(key, map[i].key) == 0)
            return (int)i;
    return -1;
}

const char *encode(const char *format, const char *input, const char *password) {
    for (size_t i = 0; i < format_map_count; i++) {
        if (strcmp(format, format_map[i].key) == 0) {
            if (format_map[i].requires_password && !password)
                return NULL; // fail if password required but missing
            return format_map[i].func_encode(input, password);
        }
    }
    return NULL; // not found
}

const char *decode(const char *format, const char *input, const char *password) {
    for (size_t i = 0; i < format_map_count; i++) {
        if (strcmp(format, format_map[i].key) == 0) {
            if (format_map[i].requires_password && !password)
                return NULL; // fail if password required but missing
            return format_map[i].func_decode(input, password);
        }
    }
    return NULL; // not found
}

void handle_args(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s encode|decode <format> [text] [-i file] [-o file]\n",
                argv[0]);
        fprintf(stderr, "For help, run with -h flag.\n");
        return;
    }

    int do_encode = 0;
    if (argc < 2 || (argc >= 2 && (
            strcmp(argv[1], "help") == 0 ||
            strcmp(argv[1], "--help") == 0 ||
            strcmp(argv[1], "-h") == 0))) {
        fprintf(stdout, "Usage: %s <command> [args...]\n", argv[0]);
        fprintf(stdout, "Available formats (NAME,ENC/DEC):\n");
        for (size_t i = 0; i < sizeof(format_map)/sizeof(format_map[0]); i++) 
            fprintf(stdout, " %s: %s\n", format_map[i].value, format_map[i].key);
        fprintf(stdout, "Commands:\n");
        fprintf(stdout, " encode <format> [text] [password i.a]      Encode text (or stdin if no text)\n");
        fprintf(stdout, " decode <format> [text] [password i.a]      Decode text (or stdin if no text)\n");
        fprintf(stdout, " help                                       Show this help message\n");
        fprintf(stdout, "Flags:\n");
        fprintf(stdout, " --help, -h                                 Show this help message\n");
        fprintf(stdout, " --version, -v                              Show version information\n");
        fprintf(stdout, " --input <file/path>, -i <file/path>        Specify input file (default: stdin)\n");
        fprintf(stdout, " --output <file/path>, -o <file/path>       Specify output file (default: stdout)\n");
        return;
    }

    if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
        fprintf(stdout, "Codify version v%s\n", version);
        return;
    }

    if (strcmp(argv[1], "encode") == 0)      do_encode = 1;
    else if (strcmp(argv[1], "decode") == 0) do_encode = 0;
    else {
        fprintf(stderr, "Unknown command: %s\n", argv[1]);
        return;
    }

    if (argc < 3) {
        fprintf(stderr, "Missing format argument.\n");
        return;
    }

    const char *format_key = argv[2];
    const char *format_name =
        lookup(format_key, format_map, sizeof(format_map)/sizeof(format_map[0]));
    if (!format_name) {
        fprintf(stderr, "Unknown format: %s\n", format_key);
        return;
    }

    const char *input_path  = NULL;
    const char *output_path = NULL;
    const char *input_text = NULL;
    const char *password = NULL;

    // Parse arguments - look for text argument and flags
    int i = 3;
    if (i < argc && argv[i][0] != '-') {
        // Next argument is text input
        input_text = argv[i];
        i++;
    }
    
    if (i < argc && argv[i][0] != '-' && format_map[get_index(format_key, format_map, format_map_count)].requires_password) {
        password = argv[i];
        i++;
    }

    // Parse remaining flags
    for (; i < argc; i++) {
        if ((strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--input") == 0) &&
            i + 1 < argc) {
            input_path = argv[++i];
            input_text = NULL; // Clear text input if file specified
        }
        else if ((strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) &&
                 i + 1 < argc) {
            output_path = argv[++i];
        }
        else {
            fprintf(stderr, "Unknown argument: %s\n", argv[i]);
            return;
        }
    }

    char *buf = NULL;
    long len = 0;

    if (input_text) {
        // Use text from command line argument
        len = strlen(input_text);
        buf = malloc(len + 1);
        strcpy(buf, input_text);
    }
    else if (input_path) {
        // Read from specified file
        FILE *in = fopen(input_path, "rb");
        if (!in) {
            perror("input");
            return;
        }
        fseek(in, 0, SEEK_END);
        len = ftell(in);
        fseek(in, 0, SEEK_SET);
        buf = malloc((size_t)len + 1);
        fread(buf, 1, (size_t)len, in);
        buf[len] = '\0';
        fclose(in);
    }
    else {
        // Read from stdin as fallback
        size_t capacity = 1024;
        buf = malloc(capacity);
        size_t total_read = 0;
        size_t bytes_read;
        
        while ((bytes_read = fread(buf + total_read, 1, capacity - total_read, stdin)) > 0) {
            total_read += bytes_read;
            if (total_read >= capacity - 1) {
                capacity *= 2;
                buf = realloc(buf, capacity);
            }
        }
        len = (long)total_read;
        buf[len] = '\0';
    }

    const char *result = do_encode ?
        encode(format_key, buf, password) : decode(format_key, buf, password);

    free(buf);
    if (!result) {
        fprintf(stderr, "Processing failed.\n");
        return;
    }

    // Open output (stdout if no -o flag specified)
    FILE *out = output_path ? fopen(output_path, "wb") : stdout;
    if (!out) {
        perror("output");
        return;
    }

    fputs(result, out);
    if (out != stdout) fclose(out);
}