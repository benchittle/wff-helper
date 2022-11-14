#ifndef LOGIC_H_
#define LOGIC_H_

#include <stdlib.h>
#include <stdbool.h>


typedef struct Wff Wff;
typedef struct WffMatch WffMatch;

typedef struct WffToken WffToken;
typedef struct WffTokenVariable WffTokenVariable;

typedef struct WffParseTree WffParseTree;
typedef struct WffTree WffTree;

typedef struct WffList WffList;
typedef struct WffTokenList WffTokenList;
typedef struct WffMatchList WffMatchList;


struct Wff {
    const char* string;
    size_t var_count;
    WffParseTree* parse_tree;
    WffTree* wff_tree;
};


// TODO: Generic list data structure
void test();

Wff* wff_create(const char* wff_string);
void wff_destroy(Wff* wff);
WffTokenList* wff_tokenize(const char* wff_string);
WffList* wff_subwffs(Wff* wff);
WffMatchList* wff_match(Wff* wff, const char* wff_pattern_string);
bool wff_substitute(Wff* wff, const char* search, const char* replace, size_t index);

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

WffTree* wff_tree_create(WffParseTree* parse_tree);
void wff_tree_destroy(WffTree* tree);
void wff_tree_print(WffTree* wff_tree);

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

#endif