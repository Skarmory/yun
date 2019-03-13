#ifndef LIST_ENTRY_H
#define LIST_ENTRY_H

#include <stddef.h>
#include <stdbool.h>

struct ListEntry
{
    struct ListEntry* next;
    struct ListEntry* prev;
};

#define container_of(ptr, type, member) (type*)((char*)(ptr) - (unsigned long)(&((type*)0)->member))

#define list_elem(ptr, type, list_name) container_of(ptr, type, list_name)

#define list_prev(ptr, type, list_name) (ptr->list_name.prev ? container_of((ptr)->list_name.prev, type, list_name) : NULL)

#define list_next(ptr, type, list_name) (ptr->list_name.next ? container_of((ptr)->list_name.next, type, list_name) : NULL)

static inline void list_init(struct ListEntry* list)
{
    list->prev = list->next = NULL;
}

static inline void list_add(struct ListEntry* add, struct ListEntry* head)
{
    add->next = add->prev = NULL;
    if(head)
    {
        add->next = head;
        head->prev = add;
    }
    else
    {
        head = add;
    }
}

static inline void list_rm(struct ListEntry* entry)
{
    if(entry->prev) entry->prev->next = entry->next;
    if(entry->next) entry->next->prev = entry->prev;
    entry->prev = entry->next = NULL;
}

static inline bool list_is_head(struct ListEntry* entry)
{
    return entry->prev == NULL;
}

#endif
