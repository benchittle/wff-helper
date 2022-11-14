#ifndef LOGIC_INTERNAL_H_
#define LOGIC_INTERNAL_H_

#include <stdlib.h>
#include <stdbool.h>

#include "logic.h"

typedef struct WffParseTreeNode WffParseTreeNode;
typedef struct WffTreeNode WffTreeNode;

typedef struct WffListNode WffListNode;

typedef struct WffTokenListNode WffTokenListNode;

typedef struct WffParseTreeNodeList WffParseTreeNodeList;
typedef struct WffParseTreeNodeListNode WffParseTreeNodeListNode;

typedef struct WffMatchListNode WffMatchListNode;

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


/* === Wff === */
WffParseTreeNodeList* wff_find_vars(Wff* wff);
const char* _wff_subwffs(WffList* list, WffParseTreeNode* node);
void _wff_find_vars(WffParseTreeNode* node, WffParseTreeNodeList* list);
bool _wff_match(WffParseTreeNode* wff_parse_node, WffParseTreeNode* pattern_parse_node, WffMatchList* list);
void _wff_match_traversal(WffParseTreeNode* wff_parse_node_root, WffParseTree* pattern_tree, WffMatchList* list);


/* === WffToken === */
struct WffToken {
    WffTokenType type;
    union {
        WffTokenVariable* variable;
        WffOperator operator;
    };
};


/* === WffTokenVariable === */
struct WffTokenVariable {
    const char* string;
};


/* === WffParseTree === */
struct WffParseTree {
    WffParseTreeNode* root;
};

struct WffParseTreeNode {
    WffParseTreeNodeType type;
    union {
        struct {
            int child_count;
            WffParseTreeNode* children[5];
        };
        WffToken* token;
    };
};

const char* wff_parse_tree_get_subwff_string(WffParseTreeNode* node);
bool wff_parse_tree_subtree_equals(WffParseTreeNode* node1, WffParseTreeNode* node2);
void _wff_parse_tree_destroy(WffParseTreeNode* node);
bool _wff_parse(WffTokenList* token_list, WffParseTreeNode* node);
void _wff_parse_tree_print(WffParseTreeNode* node, int level);
void _wff_parse_tree_set_searchvars(WffParseTreeNode* node);


/* === WffMatch === */
struct WffMatch {
    WffParseTreeNode* wff_node;
    WffParseTreeNode* subwff_root;
    WffParseTreeNode* pattern_var_node;
};

WffMatch* wff_match_create(WffParseTreeNode* wff_node, WffParseTreeNode* pattern_var_node);
// NOTE: Does not free members, just the 'match' struct
void wff_match_destroy(WffMatch* match);


/* === WffTree === */
struct WffTree {
    WffTreeNode* root;
};

struct WffTreeNode {
    char* wff_string;
    int subwffs_count;
    struct WffTreeNode* subwffs[3];
};

void _wff_tree_create(WffParseTreeNode* parse_node, WffTreeNode* wff_node);
void _wff_tree_destroy(WffTreeNode* node);
void _wff_tree_print(WffTreeNode* wff_node, int level);


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


/* === WffParseTreeNodeList ===*/
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

WffParseTreeNodeList* wff_parse_tree_node_list_create();
void wff_parse_tree_node_list_destroy(WffParseTreeNodeList* list);
void wff_parse_tree_node_list_append(WffParseTreeNodeList* list, WffParseTreeNode* parse_node);
void wff_parse_tree_node_list_reset_current(WffParseTreeNodeList* list);
WffParseTreeNode* wff_parse_tree_node_list_next(WffParseTreeNodeList* list);
size_t wff_parse_tree_node_list_length(WffParseTreeNodeList* list);

#endif