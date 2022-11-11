#include "stdlib.h"

#include "datastructs.h"
#include "parsing.h"

typedef struct WffListNode WffListNode;
typedef struct WffTokenListNode WffTokenListNode;

/* === WffList === */

struct WffList {
    WffListNode* start;
    WffListNode* end;
    WffListNode* current;
    size_t length;
};

struct WffListNode {
    Wff* wff;
    struct WffListNode* next;
};


WffList* wff_list_create() {
    WffList* list = malloc(sizeof(WffList));
    list->start = NULL;
    list->end = NULL;
    list->current = NULL;
    list->length = 0;
}

void wff_list_destroy(WffList* list) {
    WffListNode* node = list->start;
    while (node != NULL) {
        WffListNode* next = node->next;
        free(node);
        node = next;
    }
    free(list);
}

void wff_list_append(WffList* list, Wff* wff) {
    WffListNode* node = malloc(sizeof(WffListNode));
    node->wff = wff;
    node->next = NULL;

    if (list->length == 0) {
        list->start = node;
        list->length = 1;
    } else {
        list->end->next = node;
        list->length++;
    }
    list->end = node;
}

void wff_list_reset_current(WffList* list) {
    list->current = list->start;
}

Wff* wff_list_next(WffList* list) {
    if (list->current == NULL) {
        return NULL;
    }
    WffListNode* current = list->current;
    list->current = list->current->next;
    return current->wff;
}

size_t wff_list_length(WffList* list) {
    return list->length;
}



/* === WffTokenList === */

struct WffTokenList {
    WffTokenListNode* start;
    WffTokenListNode* end;
    WffTokenListNode* current;
    size_t length;
};

struct WffTokenListNode {
    WffToken* wff_token;
    struct WffTokenListNode* next;
};


WffTokenList* wff_token_list_create() {
    WffTokenList* list = malloc(sizeof(WffTokenList));
    list->start = NULL;
    list->end = NULL;
    list->current = NULL;
    list->length = 0;
}

void wff_token_list_destroy(WffTokenList* list) {
    WffTokenListNode* node = list->start;
    while (node != NULL) {
        WffTokenListNode* next = node->next;
        free(node);
        node = next;
    }
    free(list);
}

void wff_token_list_append(WffTokenList* list, WffToken* wff) {
    WffTokenListNode* node = malloc(sizeof(WffTokenListNode));
    node->wff_token = wff;
    node->next = NULL;

    if (list->length == 0) {
        list->start = node;
        list->length = 1;
    } else {
        list->end->next = node;
        list->length++;
    }
    list->end = node;
}

void wff_token_list_reset_current(WffTokenList* list) {
    list->current = list->start;
}

WffToken* wff_token_list_next(WffTokenList* list) {
    if (list->current == NULL) {
        return NULL;
    }
    WffTokenListNode* current = list->current;
    list->current = list->current->next;
    return current->wff_token;
}

size_t wff_token_list_length(WffTokenList* list) {
    return list->length;
}