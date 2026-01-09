#pragma once
#include <time.h>

#define DATA_PATH "~/.local/share/budgetTUI/"
#define LEDGER_FILE "ledger.dat"
#define RECIEPT_FOLDER "reciepts"

typedef struct {
    int account;
    int catagory;
    double amount;
    struct tm date;
    char * desc;
    char * reciept;
} Entry;

typedef struct {
    Entry *entry;
    struct LedgerNode *next;
    struct LedgerNode *prev;
} LedgerNode;

typedef struct {
    LedgerNode *head;
    LedgerNode *tail;
    LedgerNode *curr;
} Ledger;

