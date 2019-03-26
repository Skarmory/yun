#include "list.h"

#include <stdlib.h>

List* list_new(void)
{
    List* list = (List*)malloc(sizeof(List));

    list_init(list);

    return list;
}

void list_free(List* list)
{
    ListNode *node, *n;
    list_for_each_safe(list, node, n)
    {
        free(node);
    }
    free(list);
}

void list_init(List* list)
{
    list->count = 0;
    list->head = NULL;
    list->tail = NULL;
}

void list_add(List* list, void* data)
{
    ListNode* node = (ListNode*)malloc(sizeof(ListNode));
    node->data = data;
    node->next = NULL;
    node->prev = NULL;

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

void list_rm(List* list, ListNode* node)
{
    if(!node) return;

    if(node->prev)
        node->prev->next = node->next;
    else
        list->head = node->next;

    if(node->next)
        node->next->prev = node->prev;
    else
        list->tail = node->prev;

    --list->count;
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
