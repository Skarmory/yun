#ifndef YUN_LIST_H
#define YUN_LIST_H

#include <stdbool.h>

struct ListNode
{
    void* data;
    struct ListNode* next;
    struct ListNode* prev;
};

struct List
{
    int              count;
    struct ListNode* head;
    struct ListNode* tail;
};

struct List* list_new(void);
void list_free(struct List* list);
void list_init(struct List* list);
void list_uninit(struct List* list);
void list_add(struct List* list, void* data);
void list_rm(struct List* list, struct ListNode* node);
struct ListNode* list_find(struct List* list, void* data);
void list_splice_node(struct List* list_from, struct List* list_to, struct ListNode* node);
void* list_pop_head(struct List* list);
void* list_peek_head(const struct List* list);
void* list_peek_tail(const struct List* list);
bool list_empty(const struct List* list);

#define list_for_each(list, it)\
    for(it = (list)->head; it != NULL; it = it->next)

#define list_for_each_safe(list, it, n)\
    for(it = (list)->head, n = (it ? it->next : NULL); it != NULL; it = n, n = (it ? it->next : NULL))

#endif
