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
typedef struct WffMatch WffMatch;

typedef struct WffToken WffToken;
typedef struct WffTokenVariable WffTokenVariable;

typedef struct WffParseTree WffParseTree;
typedef struct WffTree WffTree;

typedef struct WffList WffList;
typedef struct WffTokenList WffTokenList;
typedef struct WffMatchList WffMatchList;
typedef struct WffParseTreeNodeList WffParseTreeNodeList;

// HIDE THESE? (ABSTRACTION)
typedef struct WffToken WffToken;
typedef struct WffTreeNode WffTreeNode;
typedef struct WffParseTreeNode WffParseTreeNode;


struct Wff {
    const char* string;
    size_t var_count;
    WffParseTree* parse_tree;
    //WffToken* token_array;
    //size_t token_count;
    WffTree* wff_tree;
};


// TODO: Generic list data structure
void test();

Wff* wff_create(char* wff_string);
void wff_destroy(Wff* wff);
WffTokenList* wff_tokenize(char* wff_string);
WffList* wff_subwffs(Wff* wff);
WffParseTreeNodeList* wff_find_vars(Wff* wff);
WffMatchList* wff_match(Wff* wff, char* wff_pattern_string);
bool wff_substitute(Wff* wff, char* search, char* replace, size_t index);

void wff_token_destroy(WffToken* token);
WffToken* wff_token_copy(WffToken* token);
bool wff_token_equal(WffToken* token1, WffToken* token2);
const char* const wff_token_get_string(WffToken* token);

WffTokenVariable* wff_token_variable_create(const char* variable_string);
void wff_token_variable_destroy(WffTokenVariable* variable);
WffTokenVariable* wff_token_variable_copy(WffTokenVariable* variable);
bool wff_token_variable_equals(WffTokenVariable* variable1, WffTokenVariable* variable2);
const char* wff_token_variable_get_string(WffTokenVariable* variable);

WffParseTree* wff_parse_tree_create(WffTokenList* token_list);
void wff_parse_tree_destroy(WffParseTree* tree);
void wff_parse_tree_print(WffParseTree* tree);
char* wff_parse_tree_node_print(WffParseTreeNode* node);
bool wff_parse_tree_subtree_equals(WffParseTreeNode* node1, WffParseTreeNode* node2);

WffTree* wff_tree_create(WffParseTree* parse_tree);
void wff_tree_destroy(WffTree* tree);
void wff_tree_print(WffTree* wff_tree);

WffMatch* wff_match_create(WffParseTreeNode* wff_node, WffParseTreeNode* pattern_var_node);
// NOTE: Does not free members, just the 'match' struct
void wff_match_destroy(WffMatch* match);

WffList* wff_list_create();
void wff_list_destroy(WffList* list);
void wff_list_append(WffList* list, Wff* wff);
void wff_list_reset_current(WffList* list);
Wff* wff_list_next(WffList* list);
size_t wff_list_length(WffList* list);
void wff_list_print_unique(WffList* subwffs_list);


WffTokenList* wff_token_list_create();
void wff_token_list_destroy(WffTokenList* list);
void wff_token_list_append(WffTokenList* list, WffToken* wff);
void wff_token_list_reset_current(WffTokenList* list);
WffToken* wff_token_list_next(WffTokenList* list);
size_t wff_token_list_length(WffTokenList* list);


WffMatchList* wff_match_list_create();
void wff_match_list_destroy(WffMatchList* list);
void wff_match_list_append(WffMatchList* list, WffMatch* match);
WffMatch* wff_match_list_get(WffMatchList* list, size_t index, bool set_current);
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