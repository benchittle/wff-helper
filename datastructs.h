#ifndef DATASTRUCTS_H_
#define DATASTRUCTS_H_

#include <stdlib.h>
#include <stdbool.h>

typedef enum {
    WTT_NONE,
    WTT_LPAREN,
    WTT_RPAREN,
    WTT_PROPOSITION,
    WTT_OPERATOR
} WffTokenType;


typedef enum {
    WO_NOT,
    WO_AND,
    WO_OR,
    WO_COND,
    WO_BICOND
} WffOperator;

typedef enum {
    WPTNT_NONTERMINAL,
    WPTNT_TERMINAL,
    WPTNT_SEARCHVAR
} WffParseTreeNodeType;

typedef struct Wff Wff;
typedef struct WffToken WffToken;
typedef struct WffParseTree WffParseTree;
typedef struct WffParseTreeNode WffParseTreeNode;
typedef struct WffTree WffTree;
typedef struct WffTreeNode WffTreeNode;
typedef struct WffMatch WffMatch;

typedef struct WffList WffList;
typedef struct WffTokenList WffTokenList;
typedef struct WffMatchList WffMatchList;
typedef struct WffParseTreeNodeList WffParseTreeNodeList;

struct Wff {
    const char* string;
    WffParseTree* parse_tree;
    WffToken* token_array;
    size_t token_count;
    WffTree* wff_tree;
};

struct WffMatch {
    WffParseTreeNode* wff_node;
    WffParseTreeNode* pattern_var_node;
};

// TODO: Generic list data structure

WffMatch* wff_match_create(WffParseTreeNode* wff_node, WffParseTreeNode* pattern_var_node);

WffList* wff_list_create();
void wff_list_destroy(WffList* list);
void wff_list_append(WffList* list, Wff* wff);
void wff_list_reset_current(WffList* list);
Wff* wff_list_next(WffList* list);
size_t wff_list_length(WffList* list);


WffTokenList* wff_token_list_create();
void wff_token_list_destroy(WffTokenList* list);
void wff_token_list_append(WffTokenList* list, WffToken* wff);
void wff_token_list_reset_current(WffTokenList* list);
WffToken* wff_token_list_next(WffTokenList* list);
size_t wff_token_list_length(WffTokenList* list);


WffMatchList* wff_match_list_create();
void wff_match_list_destroy(WffMatchList* list);
void wff_match_list_append(WffMatchList* list, WffMatch* match);
void wff_match_list_reset_current(WffMatchList* list);
WffMatch* wff_match_list_next(WffMatchList* list);
size_t wff_match_list_length(WffMatchList* list);
void wff_match_list_merge(WffMatchList* list1, WffMatchList* list2);


WffParseTreeNodeList* wff_parse_tree_node_list_create();
void wff_parse_tree_node_list_destroy(WffParseTreeNodeList* list);
void wff_parse_tree_node_list_append(WffParseTreeNodeList* list, WffParseTreeNode* parse_node);
void wff_parse_tree_node_list_reset_current(WffParseTreeNodeList* list);
WffParseTreeNode* wff_parse_tree_node_list_next(WffParseTreeNodeList* list);
size_t wff_parse_tree_node_list_length(WffParseTreeNodeList* list);

#endif