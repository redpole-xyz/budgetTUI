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
    CMD_UNKNOWN,
    CMD_LIST_ALL
} CommandParse;

CommandParse parse_command(char *cmd) {
    if (strcmp(cmd, "quit\n") == 0) return CMD_QUIT;
    if (strcmp(cmd, "q\n") == 0) return CMD_QUIT;
    if (strcmp(cmd, "new\n") == 0) return CMD_NEW_ENTRY;
    if (strcmp(cmd, "n\n") == 0) return CMD_NEW_ENTRY;
    if (strcmp(cmd, "list\n") == 0) return CMD_LIST_ALL;
    if (strcmp(cmd, "ls\n") == 0) return CMD_LIST_ALL;
    return CMD_UNKNOWN;
}

void new_entry(Ledger *ledger);
void list_entries(Ledger *ledger);
void delete_entry(Ledger *ledger);

bool menu(Ledger *ledger) {
    printf("\033[H\033[J");
    printf("Menu:\n");
    printf("create new entry (n/new)\n");
    printf("print entries    (ls/list)\n");
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
        case CMD_LIST_ALL:
            list_entries(ledger);
            return true;
            break;
        default:
            printf("Unrecognized command\n");
            return true;
    }
}

void list_entries(Ledger *ledger) {
#define LIST_SIZE 50
    int i = 0;
    char ans[256];
    LedgerNode *node_ptr_list[LIST_SIZE] = {0};
    while(i >= 0) {
        printf("\033[H\033[J");
        printf("| ### | Date       | Category  | Amount   | Description\n");
        printf("|-----|------------|-----------|----------|-------------\n");
        if (ledger->head == NULL) {
            printf("\nPress enter to continue: ");
            i = -1;
            getchar();
            break;
        }
        ledger->curr = ledger->head;
        int sel;
        // TODO 1000% error with how list size is handled
        while(1) {
            print_entry_line(ledger->curr->entry, i);
            if (i >= LIST_SIZE) log_err("node_ptr_list i out of range"); 
            node_ptr_list[i] = ledger->curr;
            ledger->curr = ledger->curr->next;
            i++;
            if (ledger->curr == NULL) {
                printf("\nPress ENTER to exit\n");
                printf("\nEnter d ${num} to delete entry\n");
                i = -1;
                if(fgets(ans, 256, stdin) == NULL) {
                    log_err("Parse error in list_entries");
                }
                if (ans[0] == 'd' && ans[1] == ' ') {
                    sel = atoi(ans + 2);
                    ledger_del(ledger, node_ptr_list[sel]);
                } else {
                    break;
                }
            }
            if (i > LIST_SIZE) { // TODO TEST THIS
                printf("\nPress ENTER to exit\n");
                printf("\nEnter d to delete entry\n");
                printf("\n Previous(p) Next(n)\n");
                if(fgets(ans, 256, stdin) == NULL) {
                    log_err("Parse error in list_entries");
                }
                if (ans[0] == 'd' && ans[1] == ' ') {
                    sel = atoi(ans + 2);
                    ledger_del(ledger, node_ptr_list[sel]);
                    break;
                }
                if (ans[0] == 'n') {
                    i = 0;
                    break;
                } else if (ans[0] == 'p') {
                    for (i = LIST_SIZE; i > 0; --i) { // This might be messed up
                        ledger->curr = ledger->curr->prev;
                        if (ledger->curr == NULL) {
                            log_err("Out of bounds while reversing list");
                        }
                    }
                    break;
                } else {
                    i = -1;
                    break;
                }
            }
        } 
    }
}

void new_entry(Ledger *ledger) {
    char * str = (char *)calloc(256, sizeof(char));
    Entry entry = {0};
    printf("\033[H\033[J");
    printf("New Entry\n");
    // Get amount
    while (1) {
        printf("AMOUNT: $");
        if (fgets(str, 256, stdin) == NULL) {
            log_err("bad input");
        }
        entry.amount = strtod(str, NULL);
        if (entry.amount >= 0.01 && entry.amount < 100000000000) {
            break;
        }
    }
    // Get category
    while (1) {
        printf("Category menu\n"); 
        printf("1: Housing\n");
        printf("2: Groceries\n");
        printf("3: Transit\n");
        printf("4: Recurring\n");
        printf("5: Shopping\n");
        printf("6: Social\n");
        printf("7: Projects\n");
        printf("Select a category: \n");
        if (fgets(str, 256, stdin) == NULL) {
            log_err("Bad input (Catalog selection)");
        } else {
            entry.category = atoi(str);
        }
        if (entry.category >= 1 && entry.category <= 7) {
            break;
        } else {
            printf("\033[H\033[J");
            printf("Invalid entry\n\n");
        }
    }
    // Get date
    while (1) {
        printf("Press ENTER for today's date\n");
        printf("Format: YYYY-MM-DD (dashes optional)\n");
        if (fgets(str, 256, stdin) == NULL) {
            log_err("bad input");
        }
        time_t t = time(NULL);
        entry.date = *localtime(&t);
        if (str[0] == '\n') {
            break;
        }
        if (strlen(str) <= 3 && strptime(str, "%d", &entry.date) != NULL) {
            break;
        }
        if (strptime(str, "%Y-%m-%d", &entry.date) != NULL) {
            break;
        } 
        if (strptime(str, "%Y%m%d", &entry.date) != NULL) {
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
    entry.desc = str;
    printf("\033[H\033[J");
    printf("Confirm Entry?\n\n");
    print_entry(&entry);
    printf("Enter q to discard: ");
    char buf[256];
    if (fgets(buf, 256, stdin) == NULL) {
        log_err("bad input");
    }
    if (buf[0] != 'q') {
        ledger_add(ledger, entry);
    } else {
        printf("discarded entry!\n");
        printf("Press enter to continue");
        if (fgets(buf, 256, stdin) == NULL) {
            log_err("bad input");
        }
    }
    free(str);
}

