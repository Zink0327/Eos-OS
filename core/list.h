#ifndef GENERIC_LIST_H
#define GENERIC_LIST_H

#include "global.h"

#define DECLARE_LIST(TYPE) \
typedef struct List_##TYPE { \
    TYPE data; \
    struct List_##TYPE *prev; \
    struct List_##TYPE *next; \
} List_##TYPE; \
\
void list_init_##TYPE(List_##TYPE *list); \
void list_append_##TYPE(List_##TYPE *entry, List_##TYPE *new_node); \
void list_insert_to_head_##TYPE(List_##TYPE *entry, List_##TYPE *new_node); \
void list_del_##TYPE(List_##TYPE *entry); \
int is_list_empty_##TYPE(List_##TYPE *entry); \
List_##TYPE *list_prev_##TYPE(List_##TYPE *entry); \
List_##TYPE *list_next_##TYPE(List_##TYPE *entry); \
uint64_t list_length_##TYPE(List_##TYPE *entry);

#define IMPLEMENT_LIST(TYPE) \
void list_init_##TYPE(List_##TYPE *list) { \
    if (list != NULL) { \
        list->prev = list; \
        list->next = list; \
    } \
} \
\
void list_append_##TYPE(List_##TYPE *entry, List_##TYPE *new_node) { \
    if (entry != NULL && new_node != NULL) { \
        new_node->next = entry->next; \
        new_node->prev = entry; \
        new_node->next->prev = new_node; \
        entry->next = new_node; \
    } \
} \
\
void list_insert_to_head_##TYPE(List_##TYPE *entry, List_##TYPE *new_node) { \
    if (entry != NULL && new_node != NULL) { \
        new_node->next = entry; \
        new_node->prev = entry->prev; \
        entry->prev->next = new_node; \
        entry->prev = new_node; \
    } \
} \
\
void list_del_##TYPE(List_##TYPE *entry) { \
    if (entry != NULL && entry->next != entry) { \
        entry->next->prev = entry->prev; \
        entry->prev->next = entry->next; \
        entry->next = NULL; \
        entry->prev = NULL; \
    } \
} \
\
int is_list_empty_##TYPE(List_##TYPE *entry) { \
    return (entry == NULL) || (entry == entry->next && entry == entry->prev); \
} \
\
List_##TYPE *list_prev_##TYPE(List_##TYPE *entry) { \
    return (entry == NULL) ? NULL : entry->prev; \
} \
\
List_##TYPE *list_next_##TYPE(List_##TYPE *entry) { \
    return (entry == NULL) ? NULL : entry->next; \
} \
\
uint64_t list_length_##TYPE(List_##TYPE *entry) { \
    if (entry == NULL) return 0; \
    uint64_t count = 0; \
    List_##TYPE *current = entry->next; \
    while (current != entry) { \
        count++; \
        current = current->next; \
    } \
    return count; \
}

#endif
