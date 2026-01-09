#pragma once
#include <time.h>

#define DATA_PATH "~/.local/share/budgetTUI/"
#define LEDGER_FILE "ledger.dat"
#define RECEIPT_FOLDER "receipts"

typedef struct {
    int account;
    int catagory;
    double amount;
    struct tm date;
    char * desc;
    char * receipt;
    char strings[];
} Entry;

typedef struct LedgerNode {
    Entry *entry;
    struct LedgerNode *next;
    struct LedgerNode *prev;
} LedgerNode;

typedef struct {
    LedgerNode *head;
    LedgerNode *tail;
    LedgerNode *curr;
} Ledger;

