#ifndef NAXX_LIST_ENTRY_H
#define NAXX_LIST_ENTRY_H

#include "log.h"

#include <stddef.h>
#include <stdbool.h>

struct ListEntry
{
    struct ListEntry* next;
    struct ListEntry* prev;
};

struct List
{
    int count;
    struct ListEntry* head;
    struct ListEntry* tail;
};

#define container_of(ptr, type, member) (type*)((char*)(ptr) - (unsigned long)(&((type*)0)->member))

#define list_elem(ptr, type, list_name) container_of((ptr), type, list_name)

#define list_prev(ptr, type, list_name) ((ptr)->list_name.prev ? container_of((ptr)->list_name.prev, type, list_name) : NULL)

#define list_next(ptr, type, list_name) ((ptr)->list_name.next ? container_of((ptr)->list_name.next, type, list_name) : NULL)

#define list_head(list_ptr, entry_type, list_name) ((list_ptr)->head ? list_elem((list_ptr)->head, entry_type, list_name) : NULL)

static inline void list_debug_ptr(struct List* list)
{
    int counter = 0;
    struct ListEntry* entry = list->head;
    while(entry)
    {
        log_format_msg(DEBUG, "%d: %p", counter++, entry);
        entry = entry->next;
    }
}

static inline void list_init(struct List* list)
{
    list->count = 0;
    list->head = list->tail = NULL;
}

static inline void list_entry_init(struct ListEntry* entry)
{
    entry->next = entry->prev = NULL;
}

static inline void list_add(struct ListEntry* add, struct List* list)
{
    add->next = add->prev = NULL;
    if(list->head)
    {
        add->next = list->head;
        list->head->prev = add;
    }
    else
    {
        list->tail = add;
    }

    ++list->count;
    list->head = add;
}

// Remove list entry from list with weak head/tail checking (be careful)
static inline void list_rm(struct ListEntry* entry, struct List* list)
{
    if(entry->prev)
        entry->prev->next = entry->next;
    else
        list->head = entry->next;

    if(entry->next)
        entry->next->prev = entry->prev;
    else
        list->tail = entry->prev;

    --list->count;
    entry->prev = entry->next = NULL;
}

static inline bool list_is_head(struct ListEntry* entry, struct List* list)
{
    return entry == list->head;
}

#endif
