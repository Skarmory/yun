#include "list.h"

#include <stdlib.h>
#include <stdio.h>

static inline void _add_node(List* list, ListNode* node)
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

static inline void _remove_node(List* list, ListNode* node)
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

List* list_new(void)
{
    List* list = (List*)malloc(sizeof(List));

    list_init(list);

    return list;
}

void list_free(List* list)
{
    list_uninit(list);
    free(list);
}

void list_init(List* list)
{
    list->count = 0;
    list->head = NULL;
    list->tail = NULL;
}

void list_uninit(List* list)
{
    ListNode *node, *n;
    list_for_each_safe(list, node, n)
    {
        free(node);
    }
}

void list_add(List* list, void* data)
{
    ListNode* node = malloc(sizeof(ListNode));
    node->data = data;
    node->next = NULL;
    node->prev = NULL;

    _add_node(list, node);
}

void list_rm(List* list, ListNode* node)
{
    if(!node)
    {
        return;
    }

    _remove_node(list, node);

    free(node);
}

ListNode* list_find(List* list, void* data)
{
    ListNode* node;
    list_for_each(list, node)
    {
        if(node->data == data) return node;
    }

    return NULL;
}

void list_splice_node(List* list_from, List* list_to, ListNode* node)
{
    if(!node)
    {
        return;
    }

    _remove_node(list_from, node);

    _add_node(list_to, node);
}

void* list_pop_head(List* list)
{
    void* data = list->head->data;

    list_rm(list, list->head);

    return data;
}

void* list_peek_head(const List* list)
{
    return list->head->data;
}

bool list_empty(const List* list)
{
    return list->count == 0;
}
