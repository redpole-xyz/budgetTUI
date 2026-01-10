#include <stdio.h>
#include "defs.h"

void print_entry(Entry *entry) {

    printf("%04d-%02d-%02d\n", entry->date.tm_year + 1900, entry->date.tm_mon+1, entry->date.tm_mday);
    switch (entry->category) {
        case 0:
            printf("Housing");
            break;
        case 1:
            printf("Groceries");
            break;
        case 2:
            printf("Transit");
            break;
        case 3:
            printf("Subscriptions");
            break;
        case 4:
            printf("Shopping");
            break;
        case 5:
            printf("Social");
            break;
        case 6:
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

