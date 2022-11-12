#include "stdlib.h"

#include "datastructs.h"

typedef struct WffListNode WffListNode;
typedef struct WffTokenListNode WffTokenListNode;
typedef struct WffMatchListNode WffMatchListNode;
typedef struct WffParseTreeNodeListNode WffParseTreeNodeListNode;

/* === WffMatch === */

WffMatch* wff_match_create(WffParseTreeNode* wff_node, WffParseTreeNode* pattern_var_node) {
    WffMatch* match = malloc(sizeof(WffMatch));
    match->wff_node = wff_node;
    match->pattern_var_node = pattern_var_node;
    return match;
}


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

void wff_token_list_append(WffTokenList* list, WffToken* wff_token) {
    WffTokenListNode* node = malloc(sizeof(WffTokenListNode));
    node->wff_token = wff_token;
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


/* === WffMatchList === */

struct WffMatchList {
    WffMatchListNode* start;
    WffMatchListNode* end;
    WffMatchListNode* current;
    size_t length;
};

struct WffMatchListNode {
    WffMatch* match;
    struct WffMatchListNode* next;
};


WffMatchList* wff_match_list_create() {
    WffMatchList* list = malloc(sizeof(WffMatchList));
    list->start = NULL;
    list->end = NULL;
    list->current = NULL;
    list->length = 0;
}

void wff_match_list_destroy(WffMatchList* list) {
    WffMatchListNode* node = list->start;
    while (node != NULL) {
        WffMatchListNode* next = node->next;
        free(node);
        node = next;
    }
    free(list);
}

void wff_match_list_append(WffMatchList* list, WffMatch* match) {
    WffMatchListNode* node = malloc(sizeof(WffMatchListNode));
    node->match = match;
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

void wff_match_list_reset_current(WffMatchList* list) {
    list->current = list->start;
}

WffMatch* wff_match_list_next(WffMatchList* list) {
    if (list->current == NULL) {
        return NULL;
    }
    WffMatchListNode* current = list->current;
    list->current = list->current->next;
    return current->match;
}

size_t wff_match_list_length(WffMatchList* list) {
    return list->length;
}

void wff_match_list_merge(WffMatchList* list1, WffMatchList* list2) {
    if (list1->length == 0) {
        list1->start = list2->start;
        list1->end = list2->end;
        list1->length = list2->length;
    } else if (list2->length != 0) {
        list1->end->next = list2->start;
        list1->end = list2->end;
        list1->length += list2->length;
    }
    free(list2);
}


/* === WffParseTreeNodeList === */

struct WffParseTreeNodeList {
    WffParseTreeNodeListNode* start;
    WffParseTreeNodeListNode* end;
    WffParseTreeNodeListNode* current;
    size_t length;
};

struct WffParseTreeNodeListNode {
    WffParseTreeNode* parse_node;
    struct WffParseTreeNodeListNode* next;
};


WffParseTreeNodeList* wff_parse_tree_node_list_create() {
    WffParseTreeNodeList* list = malloc(sizeof(WffParseTreeNodeList));
    list->start = NULL;
    list->end = NULL;
    list->current = NULL;
    list->length = 0;
}

void wff_parse_tree_node_list_destroy(WffParseTreeNodeList* list) {
    WffParseTreeNodeListNode* list_node = list->start;
    while (list_node != NULL) {
        WffParseTreeNodeListNode* next = list_node->next;
        free(list_node);
        list_node = next;
    }
    free(list);
}

void wff_parse_tree_node_list_append(WffParseTreeNodeList* list, WffParseTreeNode* parse_node) {
    WffParseTreeNodeListNode* list_node = malloc(sizeof(WffParseTreeNodeListNode));
    list_node->parse_node = parse_node;
    list_node->next = NULL;

    if (list->length == 0) {
        list->start = list_node;
        list->length = 1;
    } else {
        list->end->next = list_node;
        list->length++;
    }
    list->end = list_node;
}

void wff_parse_tree_node_list_reset_current(WffParseTreeNodeList* list) {
    list->current = list->start;
}

WffParseTreeNode* wff_parse_tree_node_list_next(WffParseTreeNodeList* list) {
    if (list->current == NULL) {
        return NULL;
    }
    WffParseTreeNodeListNode* current = list->current;
    list->current = list->current->next;
    return current->parse_node;
}

size_t wff_parse_tree_node_list_length(WffParseTreeNodeList* list) {
    return list->length;
}