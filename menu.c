#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "log_err.h"
#include "new_entry"

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
            new_entry();
            return true;
            break;
        default:
            printf("Unrecognized command\n");
            return true;
    }
}


new_entry() {
    char * str = (char *)calloc(256, sizeof(char));
    Entry *entry = (Entry *)calloc(1, sizeof(Entry));
    printf("\nNew Entry\n");
    // Get amount
    while (1) {
        printf("AMOUNT: $");
        if (fgets(str, 256, stdin) == NULL) {
            log_err("bad input");
        }
        entry->amount = strtod(str, NULL);
        if (entry->amount >= 0.01 && entry->amount < 100000000000) {
            break;
        }
    }
    // Get category
    while (1) {
        printf("Catagory menu\n"); 
        printf("0: Housing\n");
        printf("1: Groceries\n");
        printf("2: Transit\n");
        printf("3: Subscriptions\n");
        printf("4: Shopping\n");
        printf("5: Social\n");
        printf("6: Projects\n");
        printf("Select a category: \n");
        scanf("%d", &entry->category);
        getchar();
        if (entry->category >= 0 && entry->category <= 6) {
            break;
        } else {
            printf("Invalid entry\n");
        }
    }
    // Get date
    char * date_str;
    while (success != true) {
        printf("Press ENTER for today's date\n");
        printf("Format: YY-MM-DD (dashes optional)\n");
        if (fgets(str, 256, stdin) == NULL) {
            log_err("bad input");
        }
        time_t t = time(NULL);
        entry->date = *localtime(&t);
        if (date_str == NULL) {
            break;
        }
        if (strptime(date_str, "%d", &entry->date) != NULL) {
            break;
        }
        if (strptime(date_str, "%Y-%m-%d", &entry->date) != NULL) {
            break;
        } 
        if (strptime(date_str, "%Y%m%d", &entry->date) != NULL) {
            break;
        } 
        if (date_str[0] == 'q') {
            printf("discarding entry\n");
            return NULL;
        }
        printf("failed to parse string\n");
    }
    // Get desc
    printf("Description: (enter to skip)\n");
    if (fgets(str, 256, stdin) == NULL) {
        log_err("bad input");
    }
    entry->desc = str;
    ledger_add(entry);
    free(str);
}
