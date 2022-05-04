#ifndef IO_UTILS_H
#define IO_UTILS_H

#include <stdbool.h>
#include <stdio.h>

char *stream_read(FILE *file);
int stream_write(FILE *file, const char *data);

char *file_read(const char *filename);
int file_write(const char *filename, const char *data);
int file_append(const char *filename, const char *data);
int file_clear(const char *filename);

/* Callback passed to `fileparse`.
 * Should return NULL on success, or a malloc-ed string on error. */
typedef char * (*parse_callback)(
        const char *    /* in: line */,
        void *          /* in: arg */);

typedef struct parse_options {
    char comment_prefix;
    bool skip_empty_lines;
} parse_options;

parse_options parse_options_default();

/*
 * Returns NULL on success, or a malloc-ed error string on failure.
 * `callback` is responsible for the parsing of the lines.
 */
char * file_parse(const char *filename, parse_options options,
                  parse_callback callback, void *callback_arg);


/*
 * Returns NULL on success, or a malloc-ed error string on failure.
 * `callback` is responsible for the parsing of the lines.
 */
char * string_parse(const char *string, parse_options options,
                    parse_callback callback, void *callback_arg);


#endif // IO_UTILS_H