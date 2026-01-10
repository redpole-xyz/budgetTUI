#define _XOPEN_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "defs.h"
#include "log_err.h"
#include "print_dat.h"
#include "ledger.h"

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

void new_entry(Ledger *ledger);

bool menu(Ledger *ledger) {
    printf("\nMenu:\n");
    printf("create new entry (n/new)\n");
    printf("exit program     (q/quit)\n");
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
            new_entry(ledger);
            return true;
            break;
        default:
            printf("Unrecognized command\n");
            return true;
    }
}


void new_entry(Ledger *ledger) {
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
        printf("Category menu\n"); 
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
    while (1) {
        printf("Press ENTER for today's date\n");
        printf("Format: YY-MM-DD (dashes optional)\n");
        if (fgets(str, 256, stdin) == NULL) {
            log_err("bad input");
        }
        time_t t = time(NULL);
        entry->date = *localtime(&t);
        if (str == NULL) {
            break;
        }
        if (strptime(str, "%d", &entry->date) != NULL) {
            break;
        }
        if (strptime(str, "%Y-%m-%d", &entry->date) != NULL) {
            break;
        } 
        if (strptime(str, "%Y%m%d", &entry->date) != NULL) {
            break;
        } 
        if (str[0] == 'q') {
            printf("discarding entry\n");
            return;
        }
        printf("failed to parse string\n");
    }
    // Get desc
    printf("Description: (enter to skip)\n");
    if (fgets(str, 256, stdin) == NULL) {
        log_err("bad input");
    }
    entry->desc = str;
    LedgerNode *node = ledger_add(ledger, *entry);
    print_entry(node->entry);
    free(str);
    free(entry);
}
