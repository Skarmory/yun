#include "list.h"

#include <stdlib.h>
#include <stdio.h>

static inline void _add_node(struct List* list, struct ListNode* node)
{
    if(list->tail)
    {
        node->prev = list->tail;
        list->tail->next = node;
    }
    else
    {
        list->head = node;
    }

    ++list->count;
    list->tail = node;
}

static inline void _remove_node(struct List* list, struct ListNode* node)
{
    if(node->prev)
    {
        node->prev->next = node->next;
    }
    else
    {
        list->head = node->next;
    }

    if(node->next)
    {
        node->next->prev = node->prev;
    }
    else
    {
        list->tail = node->prev;
    }

    node->prev = NULL;
    node->next = NULL;
    --list->count;
}

struct List* list_new(void)
{
    struct List* list = (struct List*)malloc(sizeof(struct List));

    list_init(list);

    return list;
}

void list_free(struct List* list)
{
    list_uninit(list);
    free(list);
}

void list_init(struct List* list)
{
    list->count = 0;
    list->head = NULL;
    list->tail = NULL;
}

void list_uninit(struct List* list)
{
    struct ListNode *node, *n;
    list_for_each_safe(list, node, n)
    {
        free(node);
    }

    list->count = 0;
    list->head = NULL;
    list->tail = NULL;
}

void list_add(struct List* list, void* data)
{
    struct ListNode* node = malloc(sizeof(struct ListNode));
    node->data = data;
    node->next = NULL;
    node->prev = NULL;

    _add_node(list, node);
}

void list_rm(struct List* list, struct ListNode* node)
{
    if(!node)
    {
        return;
    }

    _remove_node(list, node);

    free(node);
}

struct ListNode* list_find(struct List* list, void* data)
{
    struct ListNode* node;
    list_for_each(list, node)
    {
        if(node->data == data) return node;
    }

    return NULL;
}

void list_splice_node(struct List* list_from, struct List* list_to, struct ListNode* node)
{
    if(!node)
    {
        return;
    }

    _remove_node(list_from, node);

    _add_node(list_to, node);
}

void* list_pop_head(struct List* list)
{
    void* data = list->head->data;

    list_rm(list, list->head);

    return data;
}

void* list_peek_head(const struct List* list)
{
    return list->head->data;
}

void* list_peek_tail(const struct List* list)
{
    return list->tail->data;
}

bool list_empty(const struct List* list)
{
    return list->count == 0;
}
