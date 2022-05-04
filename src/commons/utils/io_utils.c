#include "io_utils.h"
#include <stdlib.h>
#include <string.h>
#include "array_utils.h"
#include "str_utils.h"

char *stream_read(FILE *f) {
    fseek(f, 0, SEEK_END);
    long filesize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *content = malloc((filesize + 1) * sizeof(char));
    fread(content, sizeof(char), filesize, f);
    fclose(f);

    content[filesize] = '\0';
    return content; // must be freed outside
}

int stream_write(FILE *f, const char *data) {
    if (!data)
        return 0;

    int ret = fputs(data, f);
    fclose(f);

    return ret;
}


char *file_read(const char *filename) {
    FILE *f = fopen(filename, "rb");
    return f ? stream_read(f) : NULL;
}

static int file_write_or_append(const char *filename, const char *data, bool append) {
    FILE *f = append ? fopen(filename, "ab") : fopen(filename, "wb");
    return f ? stream_write(f, data) : 0;
}

int file_write(const char *filename, const char *data) {
    return file_write_or_append(filename, data, false /* append */);
}

int file_append(const char *filename, const char *data) {
    return file_write_or_append(filename, data, true /* append */);
}

int file_clear(const char *filename) {
    return file_write_or_append(filename, "", false /* append */);
}

char * stream_parse(FILE *file,
                    parse_options options,
                    parse_callback callback,
                    void *callback_arg) {
    static const int MAX_ERROR_LENGTH = 256;
    char error_reason[MAX_ERROR_LENGTH];
    error_reason[0] = '\0';

    static const int MAX_LINE_LENGTH = 256;
    char raw_line[MAX_LINE_LENGTH];
    char *line;
    int line_num = 0;

    bool continue_parsing = true;

    while (continue_parsing && fgets(raw_line, ARRAY_LENGTH(raw_line), file)) {
        line_num++;
        line = strtrim(raw_line);

        if (options.skip_empty_lines && strempty(line))
            continue;

        if (strstarts(line, options.comment_prefix))
            continue;

        char *err = callback(line, callback_arg);
        if (err) {
            continue_parsing = false;
            snprintf(error_reason, MAX_ERROR_LENGTH, "%s", err);
            free(err);
        }
    }

    if (!strempty(error_reason))
        return strmake("parse error at line %d (%s)", line_num, error_reason);

    return NULL; // success
}

char * file_parse(const char *filename,
                 parse_options options,
                 parse_callback callback,
                 void *callback_arg) {
    FILE *file = fopen(filename, "r");
    if (!file)
         return strmake("failed to open '%s'", filename);

    return stream_parse(file, options, callback, callback_arg);
}


/*
 * Returns NULL on success, or a malloc-ed error string on failure.
 * `callback` is responsible for the parsing of the lines.
 */
char * string_parse(const char *string,
                 parse_options options,
                 parse_callback callback,
                 void *callback_arg) {
    static const int MAX_ERROR_LENGTH = 256;
    char error_reason[MAX_ERROR_LENGTH];
    error_reason[0] = '\0';

    char *line;
    int line_num = 0;

    bool continue_parsing = true;

    char *string_copy = strdup(string);
    char *strtok_saveptr;
    char *raw_line = strtok_r(string_copy, "\n", &strtok_saveptr);

    while(continue_parsing && raw_line) {
        line_num++;
        line = strtrim(raw_line);

        if (options.skip_empty_lines && strempty(line))
            continue;

        if (strstarts(line, options.comment_prefix))
            continue;


        char *err = callback(line, callback_arg);
        if (err) {
            continue_parsing = false;
            snprintf(error_reason, MAX_ERROR_LENGTH, "%s", err);
            free(err);
        }

        raw_line = strtok_r(NULL, "\n", &strtok_saveptr);
    }

    free(string_copy);

    if (!strempty(error_reason))
        return strmake("parse error at line %d (%s)", line_num, error_reason);

    return NULL; // success
}

parse_options parse_options_default() {
    parse_options opts;
    opts.comment_prefix = '\0';
    opts.skip_empty_lines = true;
    return opts;
}
