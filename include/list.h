#ifndef LIST_ENTRY_H
#define LIST_ENTRY_H

#define list_elem(ptr, type, list_name)\
    (type*)((char*)(ptr) - (unsigned long)(&((type*)0)->list_name))

#define list_prev(ptr, type, list_name)\
    ptr->list_name.prev ? (type*)((char*)(ptr->list_name.prev) - (unsigned long)(&((type*)0)->list_name)) : NULL

#define list_next(ptr, type, list)\
    ptr->list_name.next ? (type*)((char*)(ptr->list_name.next) - (unsigned long)(&((type*)0)->list_name)) : NULL

struct ListEntry
{
    struct ListEntry* next;
    struct ListEntry* prev;
};

#endif
