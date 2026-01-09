#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "log_err.h"

typedef enum {
    CMD_QUIT,
    CMD_NEW_ENTRY,
    CMD_DATAVIEW,
    CMD_UNKNOWN
} CommandParse;

CommandParse parse_command(char *cmd) {
    if (strcmp(cmd, "quit\n") == 0) return CMD_QUIT;
    if (strcmp(cmd, "q\n") == 0) return CMD_QUIT;
    if (strcmp(cmd, "new\n") == 0) return CMD_NEW_ENTRY;
    if (strcmp(cmd, "n\n") == 0) return CMD_NEW_ENTRY;
    return CMD_UNKNOWN;
}

bool menu() {
    char str[256];
    if(fgets(str, 256, stdin) == NULL) {
        log_err("failed to get menu input");
    }
    switch (parse_command(str)) {
        case CMD_QUIT:
            printf("Exiting program\n");
            return false;
            break;
        case CMD_NEW_ENTRY: 
            return true;
            break;
        default:
            printf("Unrecognized command\n");
            return true;
    }
}
