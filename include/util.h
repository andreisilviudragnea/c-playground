#ifndef PROJECT_UTIL_H
#define PROJECT_UTIL_H

#include <stdlib.h>

#define unused __attribute__((unused))

#define eprinterrno(format, ...) fprintf(stderr, \
    "%s:%d: Error on " format ": %s\n", __FILE__, \
    __LINE__, ##__VA_ARGS__, strerror(errno))

#define DIE(condition, format, ...) \
    do { \
        if (condition) { \
            eprinterrno(format, ##__VA_ARGS__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)

#endif //PROJECT_UTIL_H
