#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "defs.h"
#include "log_err.h"

// doubly linked list
// ALL data for entry is actually within the struct once added to ledger
// new entry can have pointers to desc / receipt wherever 
// desc & receipt are formatted & realloced in ledger_add
// Sort by date (newest@head oldest@tail)
LedgerNode * ledger_add(Ledger *ledger, Entry entry) {
    size_t desc_size = 0;
    size_t receipt_size = 0;
    if (entry.desc != NULL) {
        desc_size = strlen(entry.desc) + 1;
    }
    if (entry.receipt != NULL) {
        receipt_size = strlen(entry.receipt) + 1;
    }
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
        if (entry.desc[i] == '\0' || entry.desc[i] == '\n') {
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
        if (entry.receipt[j] == '\0' || '\n') {
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
    time_t new_time = 0;
    time_t comp_time = 0;
    if (comp_node == NULL) { // if head is null
        ledger->head = ledger_node;
        ledger->tail = ledger_node;
        ledger->curr = ledger_node;
        comp_node = NULL;
    } else {
        new_time = mktime(&entry.date);
        comp_time = mktime(&(comp_node->entry->date));
    }
    int past_head = 0;
    while (comp_node != NULL) {
        if (new_time >= comp_time) {
            if (past_head == 0) {
                ledger->head = ledger_node;
            } else {
                ledger_node->prev = comp_node->prev;
                comp_node->prev->next = ledger_node;
            }
            ledger_node->next = comp_node;
            comp_node->prev = ledger_node;
            break;
        }
        if (comp_node->next == NULL) {
            comp_node->next = ledger_node;
            ledger_node->prev = comp_node;
            ledger->tail = ledger_node;
            break;
        }
        past_head = 1;
        comp_node = comp_node->next;
        comp_time = mktime(&(comp_node->entry->date));
    }
    ledger->node_count += 1;
    if (ledger->node_count < 0) {
        log_err("node_count less than 0");
    }
    if (ledger->head->prev != NULL) {
        log_err("head prev not null (ledger.c)");
    }
    if (ledger->tail->next != NULL) {
        log_err("tail next not null (ledger.c)");
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

void ledger_open(const char * path, Ledger * ledger) {
    FILE *fptr = fopen(path, "rb");
    if (fptr == NULL) {
        log_err("unable to open file for read.");
    }

    int bytes_read = -1;
    Entry entry_buf = {0};
    char * desc = (char *)malloc(sizeof(char)*1024);
    int desc_size;
    char * receipt = (char *)malloc(sizeof(char)*1024);
    int receipt_size;
    while(bytes_read != 0) {
        bytes_read = fread(&entry_buf, sizeof(Entry), 1, fptr);
        if (bytes_read != sizeof(Entry) || 0) {
            log_err("unexpected number of bytes reading entry");
        } else if (bytes_read == 0) {
            break;
        }
        desc_size = entry_buf.receipt - entry_buf.desc;
        bytes_read = fread(desc, sizeof(char), desc_size, fptr);
        if (bytes_read != desc_size || 0) {
            log_err("unexpected number of bytes reading entry.desc");
        }
        receipt_size = entry_buf.size - desc_size - sizeof(Entry);
        bytes_read = fread(receipt, sizeof(char), receipt_size, fptr);
        if (bytes_read != receipt_size || 0) {
            log_err("unexpected number of bytes reading receipt.desc");
        }
        ledger_add(ledger, entry_buf);
    }
    free(desc);
    free(receipt);
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
        ledger_del(ledger, node);
        node = ledger->head;
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


