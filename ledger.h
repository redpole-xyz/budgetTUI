#pragma once
#include "defs.h"

// reads ledger.dat file from path
// ledger should be calloc'd in main
int ledger_open(char * path, Ledger * ledger);
// saves ledger from memory into ledger.dat
// iterates thru ledger_del to free everything
int ledger_close(char * path, Ledger * ledger);
// allocs new ledger item with entry
// updates head, tail, next, prev pointers as needed 
// returns pointer to new entry item
// returns NULL on failure
LedgerNode * ledger_add(Ledger *ledger, Entry entry);
// frees entry and ledger node
// updates head, tail, next, prev pointers as needed 
int ledger_del(Ledger *ledger, LedgerNode *node);
