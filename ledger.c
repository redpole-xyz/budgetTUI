#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "defs.h"
#include "log_err.h"

// doubly linked list
// ALL data for entry is actually within the struct once added to ledger
// new entry can have pointers to desc / receipt wherever 
// desc & receipt are formatted & realloced in ledger_add

void ledger_open(const char * path, Ledger * ledger) {
    return;
}

void ledger_close(const char * path, const char * swap_path, Ledger * ledger) {
    FILE *fptr = fopen(swap_path, "wb");
    if (fptr == NULL) {
        log_err("unable to open file for write.");
    }

    LedgerNode *node = ledger->head;
    int i = 0;
    while (node != NULL) {
        fwrite(node->entry, node->entry->size, 1, fptr); 
        node = node->next;
        ++i;
    }
    fclose(fptr);
    if (i != ledger->node_count) {
        log_err("ledger size mismatch (in ledger_close)");
    }
    if (rename(swap_path, path)) {
        log_err("unable to rename swapfile");
    }
}

// Make sure all data within entry is consecutive to ease saving
// Sort by date (newest@head oldest@tail)
LedgerNode * ledger_add(Ledger *ledger, Entry entry) {
    size_t desc_size = strlen(entry.desc) + 1;
    size_t receipt_size = strlen(entry.receipt) + 1;
    int str_size = (int)(desc_size + receipt_size);
    Entry * ret_entry = (Entry*)calloc(1, (sizeof(Entry)) + str_size); 
    *ret_entry = entry;
    ret_entry->size = str_size + sizeof(Entry);
    // Copy desc into ret_entry
    int i = 0;
    while (entry.desc != NULL) {
        if (i > str_size) {
            log_err("Allocation mismatch while copying entry.desc to ret_entry->desc");
        }
        if (entry.desc[i] == '\0') {
            ret_entry->strings[i] = '\0';
            ++i;
            break;
        }
        ret_entry->strings[i] = entry.desc[i];
        ++i;
    } 
    // Copy receipt into ret_entry
    int j = 0;
    while (entry.receipt != NULL) {
        if (i > str_size) {
            log_err("Allocation mismatch while copying entry.receipt to ret_entry->receipt");
        }
        if (entry.receipt[j] == '\0') {
            ret_entry->strings[i] = '\0';
            break;
        }
        ret_entry->strings[i] = entry.receipt[j];
        ++i;
        ++j;
    }

    ret_entry->desc = ret_entry->strings;
    ret_entry->receipt = ret_entry->strings + desc_size;

    // entry is alloc'd time to create ledger node
    LedgerNode *ledger_node = (LedgerNode *)calloc(1, sizeof(LedgerNode));
    ledger_node->entry = ret_entry;
    LedgerNode *comp_node = ledger->head;

    if (comp_node == NULL) { // if head is null
        ledger->head = ledger_node;
        ledger->tail = ledger_node;
        ledger->curr = ledger_node;
    }

    time_t new_time = mktime(&entry.date);
    time_t comp_time = mktime(&(comp_node->entry->date));
    while (comp_node != NULL) {
        if (comp_node->next == NULL) {
            comp_node->next = ledger_node;
            break;
        } 
        if (new_time > comp_time) {
            ledger_node->next = comp_node;
            ledger_node->prev = comp_node->prev;
            comp_node->prev = ledger_node;
        }
        comp_node = comp_node->next;
        comp_time = mktime(&(comp_node->entry->date));
    }
    ledger->node_count += 1;
    if (ledger->node_count < 0) {
        log_err("node_count less than 0");
    }
    return ledger_node;
}

void ledger_del(Ledger *ledger, LedgerNode *node) {
    free(node->entry);
    ledger->node_count -= 1;
    if (ledger->node_count < 0) {
        log_err("node_count less than 0");
    }
    if (ledger->head == ledger->tail) {
        if (ledger->head != node || ledger->tail != node) {
            log_err("node not in or out of sync of list");
        }
        ledger->head = NULL;
        ledger->tail = NULL;
        free(node);
        return;
    }
    if (ledger->head == node) {
        ledger->head = node->next;
        node->next->prev = NULL;
        free(node);
        return;
    }
    if (ledger->tail == node) {
        ledger->tail = node->prev;
        node->prev->next = NULL;
        free(node);
        return;
    }
    node->next->prev = node->prev;
    node->prev->next = node->next;
    free(node);
    return;
}

