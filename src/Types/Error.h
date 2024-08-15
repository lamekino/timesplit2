#pragma once

enum ErrorLevel {
    LEVEL_SUCCESS,
    LEVEL_SHOW_HELP,

    LEVEL_FAILED,
    LEVEL_NO_MEM,

    ERROR_LEVEL_COUNT
};

union Error {
    enum ErrorLevel level;
    char *description;
};

#define ERROR_MAX_LEN 256

#define error_level(lvl) ((union Error) { .level = lvl })

#define IS_OK(e) ((e).level < LEVEL_FAILED)
#define IS_ERROR(e) ((e).level >= LEVEL_FAILED)
#define IS_ERROR_LEVEL(e, id) ((e).level == (id))

union Error
error_msg(const char *fmt, ...);

int
error_report(const union Error *);
