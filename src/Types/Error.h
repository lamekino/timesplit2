#pragma once

enum ErrorLevel {
    ERROR_LEVEL_NULL,

    LEVEL_SUCCESS,

    /* TODO: xmacro of failure values */
    LEVEL_FAILED,
    LEVEL_SHOW_HELP,
    LEVEL_NO_MEM,

    ERROR_LEVEL_COUNT
};

union Error {
    enum ErrorLevel level;
    char *description;
};

#define ERROR_MAX_LEN 256

#define error_level(lvl) ((union Error) { .level = lvl })

#define IS_OK(e) (0 < (e).level && (e).level < LEVEL_FAILED)
#define IS_ERROR(e) ((e).level == ERROR_LEVEL_NULL || (e).level >= LEVEL_FAILED)
#define IS_ERROR_LEVEL(e, id) ((e).level == (id))

union Error
error_msg(const char *fmt, ...);

int
error_report(const char *progname, const union Error *err);
