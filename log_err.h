#pragma once
#include <stdlib.h>
#include <stdio.h>

static inline void log_err(const char *msg) {
    fprintf(stderr, "[!] Error: %s\n", msg);
    exit(EXIT_FAILURE);
}
