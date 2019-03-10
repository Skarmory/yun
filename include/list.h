#ifndef LIST_ENTRY_H
#define LIST_ENTRY_H

#define list_init(list)\
    do\
    {\
        (list).next = NULL;\
        (list).prev = NULL;\
    }\
    while(0)

#define list_elem(ptr, type, list_name)\
    (type*)((char*)(ptr) - (unsigned long)(&((type*)0)->list_name))

#define list_prev(ptr, type, list_name)\
    (ptr->list_name.prev ? (type*)((char*)(ptr->list_name.prev) - (unsigned long)(&((type*)0)->list_name)) : NULL)

#define list_next(ptr, type, list_name)\
    (ptr->list_name.next ? (type*)((char*)(ptr->list_name.next) - (unsigned long)(&((type*)0)->list_name)) : NULL)

#define list_add(to_add, list_head, list_name)\
    do\
    {\
        (to_add)->list_name.next = (to_add)->list_name.prev = NULL;\
        if(list_head)\
        {\
            (to_add)->list_name.next = &((list_head)->list_name);\
            (list_head)->list_name.prev = &((to_add)->list_name);\
        }\
        else\
        {\
            (list_head) = (to_add);\
        }\
    }\
    while(0)

#define list_rm(ptr, list_name)\
    do\
    {\
        if((ptr)->list_name.prev)\
            (ptr)->list_name.prev->next = (ptr)->list_name.next;\
        if((ptr)->list_name.next)\
            (ptr)->list_name.next->prev = (ptr)->list_name.prev;\
        (ptr)->list_name.next = NULL;\
        (ptr)->list_name.prev = NULL;\
    }\
    while(0)

struct ListEntry
{
    struct ListEntry* next;
    struct ListEntry* prev;
};

#endif
