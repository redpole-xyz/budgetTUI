#include <stdio.h>
#include "defs.h"

void print_entry(Entry *entry) {
    printf("%04d-%02d-%02d\n", entry->date.tm_year + 1900, entry->date.tm_mon+1, entry->date.tm_mday);
    switch (entry->category) {
        case 1:
            printf("Housing");
            break;
        case 2:
            printf("Groceries");
            break;
        case 3:
            printf("Transit");
            break;
        case 4:
            printf("Recurring");
            break;
        case 5:
            printf("Shopping");
            break;
        case 6:
            printf("Social");
            break;
        case 7:
            printf("Projects");
            break;
    }
    printf("\n");
    printf("$%.2f\n", entry->amount);
    if (entry->desc != NULL) {
        printf("%s\n", entry->desc);
    } else {
        printf("No description\n");
    }
}

void print_entry_line(Entry *entry, int line_number) {
    printf("| %03d | ", line_number);
    printf("%04d-%02d-%02d | ", entry->date.tm_year + 1900, entry->date.tm_mon+1, entry->date.tm_mday);
    switch (entry->category) {
        case 1:
            printf("Housing   | ");
            break;
        case 2:
            printf("Groceries | ");
            break;
        case 3:
            printf("Transit   | ");
            break;
        case 4:
            printf("Recurring | ");
            break;
        case 5:
            printf("Shopping  | ");
            break;
        case 6:
            printf("Social    | ");
            break;
        case 7:
            printf("Projects  | ");
            break;
    }
    printf("$%-7.2f | ", entry->amount);
    if (entry->desc != NULL) {
        printf("%s\n", entry->desc);
    } else {
        printf("none\n");
    }
}
