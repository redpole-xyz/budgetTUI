#pragma once
#include <time.h>

#define DATA_PATH "~/.local/share/budgetTUI/"
#define LEDGER_FILE "ledger.dat"
#define SWAP_FILE "ledger.swap"
#define RECEIPT_FOLDER "receipts"

typedef struct {
    int account;
    int category;
    double amount;
    struct tm date;
    char * desc;
    char * receipt;
    size_t size;
    char strings[];
} Entry;

typedef struct LedgerNode {
    Entry *entry;
    struct LedgerNode *next;
    struct LedgerNode *prev;
} LedgerNode;

typedef struct {
    int node_count;
    LedgerNode *head;
    LedgerNode *tail;
    LedgerNode *curr;
} Ledger;

