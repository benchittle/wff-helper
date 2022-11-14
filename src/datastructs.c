#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "datastructs.h"

typedef struct WffListNode WffListNode;
typedef struct WffTokenListNode WffTokenListNode;
typedef struct WffMatchListNode WffMatchListNode;
typedef struct WffParseTreeNodeListNode WffParseTreeNodeListNode;

// TODO: HIDE THESE (ABSTRACTION)?

struct WffToken {
    WffTokenType type;
    union {
        WffTokenVariable* variable;
        WffOperator operator;
    };
};

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
        WffToken token;
    };
};

struct WffMatch {
    WffParseTreeNode* wff_node;
    WffParseTreeNode* subwff_root;
    WffParseTreeNode* pattern_var_node;
};


const char* const STR_NOT = "~";
const char* const STR_AND = "^";
const char* const STR_OR = "v";
const char* const STR_COND = "=>";
const char* const STR_BICOND = "<=>";
const char* const STR_LPAREN = "(";
const char* const STR_RPAREN = ")";


void _wff_parse_tree_set_searchvars(WffParseTreeNode* node);

void test() {
    char* wff_string = "((p v (q ^ r)) <=> ((p v q) ^ (p v r)))"; //"(((p v q) ^ (p v ~q)) => p)";
    char* search = "(a <=> b)";
    char* replace = "(b <=> a)";
    Wff* wff = wff_create(wff_string);

    /*
    printf("%s\n", wff->string);
    for (int j = 0; j < wff->token_count; j++) {
        printf("\t\tToken %d: ", j);
        wff_token_print(&(wff->token_array[j]));
        printf("\n");
    }
    */
    printf("PARSE TREE:\n");
    wff_parse_tree_print(wff->parse_tree);
    //printf("wff_compare: %d\n", wff_parse_tree_subtree_equals(wff->parse_tree->root, wff->parse_tree->root));

    
    WffList* subwffs = wff_subwffs(wff);
    wff_list_print_unique(subwffs);
    

    printf("\n\nWFF TREE:\n");
    wff_tree_print(wff->wff_tree);

    WffMatchList* result = wff_match(wff, search);
    printf("Searching in wff '%s' for pattern '%s': %ld\n", wff->string, search, wff_match_list_length(result));

    printf("\nFOUND:\n");
    wff_match_list_reset_current(result);
    WffMatch* match = wff_match_list_next(result);
    int i = 0;
    while (match != NULL) {
        printf("%s: %s\n", wff_parse_tree_node_print(match->pattern_var_node), wff_parse_tree_node_print(match->wff_node));
        match = wff_match_list_next(result);
        i++;
        if (i == 1) {
            printf("\n");
            i = 0;
        }
    }
    
    printf("BEFORE: %s\n", wff_parse_tree_node_print(wff->parse_tree->root));
    wff_substitute(wff, search, replace, 0);
    printf("AFTER: %s\n", wff_parse_tree_node_print(wff->parse_tree->root));
}

/* === Wff === */

char* _wff_subwffs(WffList* list, WffParseTreeNode* node);
void _wff_find_vars(WffParseTreeNode* node, WffParseTreeNodeList* list);
bool _wff_match(WffParseTreeNode* wff_parse_node, WffParseTreeNode* pattern_parse_node, WffMatchList* list);
void _wff_match_traversal(WffParseTreeNode* wff_parse_node_root, WffParseTree* pattern_tree, WffMatchList* list);


Wff* wff_create(char* wff_string) {
    Wff* wff = malloc(sizeof(Wff));
    wff->string = wff_string;

    WffTokenList* token_list = wff_tokenize(wff->string);
    size_t var_count = 0;
    wff_token_list_reset_current(token_list);
    for (WffToken* token = wff_token_list_next(token_list); token != NULL; token = wff_token_list_next(token_list)) {
        if (token->type == WTT_PROPOSITION) {
            var_count++;
        }
    }
    wff->var_count = var_count;
    //wff->token_array = token_array;
    //wff->token_count = token_count;

    wff->parse_tree = wff_parse_tree_create(token_list);
    if (wff->parse_tree == NULL) {
        printf("ERROR: Invalid wff '%s'\n", wff->string);
        exit(1);
    }

    wff->wff_tree = wff_tree_create(wff->parse_tree);

    return wff;
}

// TODO
void wff_destroy(Wff* wff) {
    return;
}

WffTokenList* wff_tokenize(char* wff_string) {
    WffTokenList* list = wff_token_list_create(); 

    for (char* c = wff_string; *c != '\0'; c++) {
        WffToken token = {.type = WTT_NONE};
        switch (*c) {
            case ' ': break;
            case '~':
                token.type = WTT_OPERATOR;
                token.operator = WO_NOT;
                break;
            case 'v': 
                token.type = WTT_OPERATOR;
                token.operator = WO_OR;
                break;
            case '^':
                token.type = WTT_OPERATOR;
                token.operator = WO_AND;
                break;
            case '=':
                token.type = WTT_OPERATOR;
                token.operator = WO_COND;
                c += 1;
                break;
            case '<':
                token.type = WTT_OPERATOR;
                token.operator = WO_BICOND;
                c += 2;
                break;

            case '(':
                token.type = WTT_LPAREN;
                break;
            case ')':
                token.type = WTT_RPAREN;
                break;

            default: {
                if (('a' <= *c && *c <= 'z') || ('A' <= *c && *c <= 'Z')) {
                    token.type = WTT_PROPOSITION;
                    char* variable_string = calloc(2, sizeof(char));
                    variable_string[0] = *c;
                    token.variable = wff_token_variable_create(variable_string);
                } else {
                    printf("ERROR: Unexpected token: %c\n", *c);
                    exit(1);   
                }
            }
        } // switch
        
        // If a token was created, append it to the list.
        if (token.type != WTT_NONE) {
            WffToken* new_token = malloc(sizeof(WffToken));
            memcpy(new_token, &token, sizeof(WffToken));
            wff_token_list_append(list, new_token);
        }
    }
    return list;
}

WffList* wff_subwffs(Wff* wff) {
    WffList* wff_list = wff_list_create(); 
    _wff_subwffs(wff_list, wff->parse_tree->root);
    
    return wff_list;
}

char* _wff_subwffs(WffList* list, WffParseTreeNode* node) {
    char* str;
    if (node->type == WPTNT_TERMINAL) {
        str = wff_token_get_string(&(node->token));
    } else {
        str = malloc(1);
        *str = '\0';
        size_t str_size = 1;
        for (int i = 0; i < node->child_count; i++) {
            char* substr = _wff_subwffs(list, node->children[i]);
            str_size += strlen(substr);
            str = realloc(str, str_size);
            strcat(str, substr);
        }
        wff_list_append(list, wff_create(str));
    }
    return str;
}

WffParseTreeNodeList* wff_find_vars(Wff* wff) {
    WffParseTreeNodeList* variable_nodes = wff_parse_tree_node_list_create();
    _wff_find_vars(wff->parse_tree->root, variable_nodes);
    return variable_nodes;
}

void _wff_find_vars(WffParseTreeNode* node, WffParseTreeNodeList* list) {
    for (int i = 0; i < node->child_count; i++) {
        WffParseTreeNode* child = node->children[i];
        if (child->type == WPTNT_TERMINAL) {
            if (child->token.type == WTT_PROPOSITION) {
                wff_parse_tree_node_list_append(list, child);
            }
        } else {
            _wff_find_vars(child, list);
        }
    }
}

WffMatchList* wff_match(Wff* wff, char* wff_pattern_string) {
    Wff* pattern = wff_create(wff_pattern_string);
    _wff_parse_tree_set_searchvars(pattern->parse_tree->root);

    WffMatchList* token_matches = wff_match_list_create();
    _wff_match_traversal(wff->parse_tree->root, pattern->parse_tree, token_matches);
    return token_matches;
}

void _wff_match_traversal(WffParseTreeNode* wff_parse_node_root, WffParseTree* pattern_tree, WffMatchList* list) {
    if (wff_parse_node_root->type == WPTNT_NONTERMINAL) {
        WffMatchList* temp_list = wff_match_list_create();
        bool result = _wff_match(wff_parse_node_root, pattern_tree->root, temp_list);
        if (result) {
            wff_match_list_reset_current(temp_list);
            wff_match_list_next(temp_list)->subwff_root = wff_parse_node_root;
            wff_match_list_merge(list, temp_list);
        }
        //wff_match_list_append(list, wff_parse_node_root);
        for (int i = 0; i < wff_parse_node_root->child_count; i++) {
            _wff_match_traversal(wff_parse_node_root->children[i], pattern_tree, list);
        }
    }
}

bool _wff_match(WffParseTreeNode* wff_parse_node, WffParseTreeNode* pattern_parse_node, WffMatchList* list) {
    if (wff_parse_node->type == WPTNT_TERMINAL && pattern_parse_node->type == WPTNT_TERMINAL ) {
        // Modified token equality check: whenever we see a proposition in the
        // pattern, we'll match it against any valid wff or subwff (including
        // other, different propositions).
        WffToken wff_token = wff_parse_node->token;
        WffToken pattern_token = pattern_parse_node->token;
        if (wff_token.type != pattern_token.type) {
            return false;
        }
        switch (wff_token.type) {
            case WTT_LPAREN:
            case WTT_RPAREN:
                return true;
                break;
            case WTT_OPERATOR:
                return wff_token.operator == pattern_token.operator;
                break;
            default:
                printf("ERROR: Unhandled token type\n");
                exit(1);
        }

    } else if (wff_parse_node->type == WPTNT_TERMINAL && pattern_parse_node->type == WPTNT_NONTERMINAL) {
        //wff_parse_tree_node_list_append(list, wff_parse_node);
        return pattern_parse_node->child_count == 0;

    } else if (wff_parse_node->type == WPTNT_NONTERMINAL) {
        if (pattern_parse_node->type == WPTNT_TERMINAL) {
            return false;
        } else if (pattern_parse_node->type == WPTNT_NONTERMINAL) {
            if (wff_parse_node->child_count == pattern_parse_node->child_count) {
                bool isEqual = true;
                for (int i = 0; i < wff_parse_node->child_count; i++) {
                    isEqual = isEqual && _wff_match(wff_parse_node->children[i], pattern_parse_node->children[i], list);
                }
                return isEqual;
            } else {
                return false;
            }
        } else if (pattern_parse_node->type == WPTNT_SEARCHVAR) {
            // Implement behaviour for same variable appearing in search string 
            // more than once.
            wff_match_list_reset_current(list);
            WffMatch* previous_match = wff_match_list_next(list);
            while (previous_match != NULL) {
                if (wff_token_variable_equals(previous_match->pattern_var_node->token.variable, pattern_parse_node->token.variable)) {
                    if (!wff_parse_tree_subtree_equals(previous_match->wff_node, wff_parse_node)) {
                        return false;
                    }
                }
                previous_match = wff_match_list_next(list);
            }
            wff_match_list_append(list, wff_match_create(wff_parse_node, pattern_parse_node));
            return true;

            /*bool isEqual = true;
            for (int i = 0; i < wff_parse_node->child_count; i++) {
                isEqual = isEqual && _wff_match(wff_parse_node->children[i], pattern_parse_node, list);
            }
            return isEqual;*/
        }
    }
    printf("ERROR: Unhandled case\n");
    abort();
}

bool wff_substitute(Wff* wff, char* search, char* replace, size_t index) {
    WffMatchList* candidates = wff_match(wff, search);
    if (wff_match_list_length(candidates) == 0) {
        return false;
    }
    Wff* search_wff = wff_create(search);
    WffMatch* chosen_matches[search_wff->var_count];
    WffMatch* match = wff_match_list_get(candidates, search_wff->var_count * index, true);
    for (size_t i = 0; i < search_wff->var_count; i++) {
        chosen_matches[i] = match;
        match = wff_match_list_next(candidates);
    }

    Wff* replace_wff = wff_create(replace);

    //printf("Mid: %s\n", wff_parse_tree_node_print(wff->parse_tree->root));
    
    // Replace the terminals in the replace expression with the subwffs found in
    // the original expression.
    WffParseTreeNodeList* variable_nodes = wff_find_vars(replace_wff);
    wff_parse_tree_node_list_reset_current(variable_nodes);
    WffParseTreeNode* var_node = wff_parse_tree_node_list_next(variable_nodes);
    while (var_node != NULL) {
        for (size_t i = 0; i < search_wff->var_count; i++) {
            if (wff_token_variable_equals(var_node->token.variable, chosen_matches[i]->pattern_var_node->token.variable)) {
                var_node->child_count = chosen_matches[i]->wff_node->child_count;
                for (int j = 0; j < chosen_matches[i]->wff_node->child_count; j++) {
                    var_node->children[j] = chosen_matches[i]->wff_node->children[j];
                }
                var_node->type = WPTNT_NONTERMINAL;
                break;
            }
        }
        var_node = wff_parse_tree_node_list_next(variable_nodes);
    }

    // Replace the desired section in the wff with the replace expression nodes.
    WffParseTreeNode* parent = chosen_matches[0]->subwff_root; 
    parent->child_count = replace_wff->parse_tree->root->child_count;
    for (int i = 0; i < replace_wff->parse_tree->root->child_count; i++) {
        parent->children[i] = replace_wff->parse_tree->root->children[i];
    }
    //make_nonterminal(replace_wff->parse_tree->root);

    return true;
}


/* === WffToken === */

bool wff_token_equal(WffToken token1, WffToken token2) {
    if (token1.type != token2.type) {
        return false;
    }
    switch (token1.type) {
        case WTT_LPAREN:
        case WTT_RPAREN:
        case WTT_NONE:
            return true;
            break;
        case WTT_OPERATOR:
            return token1.operator == token2.operator;
            break;
        case WTT_PROPOSITION:
            return wff_token_variable_equals(token1.variable, token2.variable);
            break;
        default:
            printf("ERROR: Unhandled token type\n");
            exit(1);
    }
}

const char* wff_token_get_string(WffToken* token) {
    switch (token->type) {
        case WTT_OPERATOR:
            switch (token->operator) {
                case WO_NOT: 
                    return STR_NOT;
                case WO_AND: 
                    return STR_AND;
                case WO_OR:
                    return STR_OR;
                case WO_COND:
                    return STR_COND;
                case WO_BICOND:
                    return STR_BICOND;
            }
        case WTT_PROPOSITION:
            return wff_token_variable_get_string(token->variable);
        case WTT_LPAREN:
            return STR_LPAREN;
        case WTT_RPAREN:
            return STR_RPAREN;
        default:
            printf("ERROR: Unhandled case\n");
            abort();
    }
}


/* === WffVariable === */

struct WffTokenVariable {
    const char* string;
};

WffTokenVariable* wff_token_variable_create(const char* variable_string) {
    WffTokenVariable* variable = malloc(sizeof(WffTokenVariable));
    variable->string = variable_string;
    return variable;
}

void wff_token_variable_destroy(WffTokenVariable* variable) {
    free(variable->string);
    free(variable);
}

bool wff_token_variable_equals(WffTokenVariable* variable1, WffTokenVariable* variable2) {
    return strcmp(variable1->string, variable2->string) == 0;
}

const char* wff_token_variable_get_string(WffTokenVariable* variable) {
    return variable->string;
}


/* === WffParseTree === */

bool _wff_parse(WffTokenList* token_list, WffParseTreeNode* node);
void _wff_parse_tree_print(WffParseTreeNode* node, int level);


WffParseTree* wff_parse_tree_create(WffTokenList* token_list) {
    WffParseTreeNode* root = malloc(sizeof(WffParseTreeNode));
    wff_token_list_reset_current(token_list);

    bool valid = _wff_parse(token_list, root);
    // Ensure that the tokens parsed were valid and ALL tokens were parsed.
    if (valid && wff_token_list_next(token_list) == NULL) {
        WffParseTree* tree = malloc(sizeof(WffParseTree));
        tree->root = root;
        return tree;
    } else {
        // FREE NODES
        return NULL;
    }
}

bool _wff_parse(WffTokenList* token_list, WffParseTreeNode* node) {
    //int savedIndex = *index; 
    WffToken* next = wff_token_list_next(token_list);
    if (next->type == WTT_PROPOSITION) {
        node->type = WPTNT_NONTERMINAL;
        node->child_count = 1;
        WffParseTreeNode* newNode = malloc(sizeof(WffParseTreeNode));
        newNode->type = WPTNT_TERMINAL;
        newNode->token = *next;
        node->children[0] = newNode;
        return true;
    } else if (next->type == WTT_OPERATOR && next->operator == WO_NOT) {
        node->type = WPTNT_NONTERMINAL;
        node->child_count = 2;
        // First child
        WffParseTreeNode* newNode = malloc(sizeof(WffParseTreeNode));
        newNode->type = WPTNT_TERMINAL;
        newNode->token = *next;
        node->children[0] = newNode;
        // Second child
        newNode = malloc(sizeof(WffParseTreeNode));
        _wff_parse(token_list, newNode);
        node->children[1] = newNode;
        return true;
    } else if (next->type == WTT_LPAREN) {
        node->type = WPTNT_NONTERMINAL;
        node->child_count = 5;
        // First child
        WffParseTreeNode* newNode = malloc(sizeof(WffParseTreeNode));
        newNode->type = WPTNT_TERMINAL;
        newNode->token = *next;
        node->children[0] = newNode;
        // Second child
        newNode = malloc(sizeof(WffParseTreeNode));
        _wff_parse(token_list, newNode);
        node->children[1] = newNode;
        // Third child
        next = wff_token_list_next(token_list);
        if (next->type != WTT_OPERATOR || (next->type == WTT_OPERATOR && next->operator != WO_AND && next->operator != WO_OR && next->operator != WO_COND && next->operator != WO_BICOND)) {
            return false;
        }
        newNode = malloc(sizeof(WffParseTreeNode));
        newNode->type = WPTNT_TERMINAL;
        newNode->token = *next;
        node->children[2] = newNode;
        // Fourth child
        newNode = malloc(sizeof(WffParseTreeNode));
        _wff_parse(token_list, newNode);
        node->children[3] = newNode;
        // Fifth child
        next = wff_token_list_next(token_list);
        if (next->type != WTT_RPAREN) {
            return false;
        }
        newNode = malloc(sizeof(WffParseTreeNode));
        newNode->type = WPTNT_TERMINAL;
        newNode->token = *next;
        node->children[4] = newNode;
        return true;
    }
    return false;
}

void wff_parse_tree_print(WffParseTree* tree) {
    _wff_parse_tree_print(tree->root, 0);
}

void _wff_parse_tree_print(WffParseTreeNode* node, int level) {
    if (node->type == WPTNT_TERMINAL) {
        for (int i = 0; i < level; i++) {
            printf("    ");
        }
        printf("%s\n", wff_token_get_string(&(node->token)));
    } else {
        for (int i = 0; i < node->child_count / 2; i++) {
            _wff_parse_tree_print(node->children[i], level + 1);
        }
        for (int i = 0; i < level; i++) {
            printf("    ");
        }
        printf("wff\n");
        for (int i = node->child_count / 2; i < node->child_count; i++) {
            _wff_parse_tree_print(node->children[i], level + 1);
        }
    }
}

char* wff_parse_tree_node_print(WffParseTreeNode* node) {
    if (node->type == WPTNT_TERMINAL || node->type == WPTNT_SEARCHVAR) {
        return wff_token_get_string(&(node->token));
    } else if (node->type == WPTNT_NONTERMINAL) {
        char* str = malloc(1);
        str[0] = '\0';
        int str_size = 1;
        for (int i = 0; i < node->child_count; i++) {
            char* substr = wff_parse_tree_node_print(node->children[i]);
            str_size += strlen(substr);
            str = realloc(str, str_size);
            strcat(str, substr);
        }
        return str;
    }

    printf("ERROR: Unhandled case\n");
    abort();
}

bool wff_parse_tree_subtree_equals(WffParseTreeNode* node1, WffParseTreeNode* node2) {
    if (node1->type != node2->type) {
        return false;
    }
    if (node1->type == WPTNT_TERMINAL) {
        return wff_token_equal(node1->token, node2->token);
    } else if (node1->type == WPTNT_NONTERMINAL) {
        if (node1->child_count == node2->child_count) {
            bool isEqual = true;
            for (int i = 0; i < node1->child_count; i++) {
                isEqual = isEqual && wff_parse_tree_subtree_equals(node1->children[i], node2->children[i]);
            }
            return isEqual;
        } else {
            return false;
        }
    }
    printf("ERROR: Unhandled case\n");
    abort();
}

void _wff_parse_tree_set_searchvars(WffParseTreeNode* node) {
    for (int i = 0; i < node->child_count; i++) {
        WffParseTreeNode* child = node->children[i];
        if (child->type == WPTNT_TERMINAL) {
            if (child->token.type == WTT_PROPOSITION) {
                node->type = WPTNT_SEARCHVAR;
                node->token = child->token;
                free(child);
                return;
            }
        } else {
            _wff_parse_tree_set_searchvars(child);
        }
    }
}


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
void _wff_tree_print(WffTreeNode* wff_node, int level);


WffTree* wff_tree_create(WffParseTree* parse_tree) {
    WffTreeNode* root = malloc(sizeof(WffTreeNode));
    root->wff_string = malloc(sizeof(char));
    root->wff_string[0] = '\0';
    root->subwffs_count = 0;
    _wff_tree_create(parse_tree->root, root);

    WffTree* wff_tree = malloc(sizeof(WffTree));
    wff_tree->root = root->subwffs[0];
    free(root->wff_string);
    free(root);
    return wff_tree;
}

void _wff_tree_create(WffParseTreeNode* parse_node, WffTreeNode* wff_node) {
    // If terminal, copy the symbol into wff_node's string (essentially, each
    // recursive call builds the string for the previous call and passes it up).
    if (parse_node->type == WPTNT_TERMINAL) {
        const char* token_string = wff_token_get_string(&(parse_node->token));
        wff_node->wff_string = realloc(wff_node->wff_string, (strlen(wff_node->wff_string) + strlen(token_string) + 1) * sizeof(char));
        strcat(wff_node->wff_string, token_string);
        return; 
    } else {
        WffTreeNode* subwff_node = malloc(sizeof(WffTreeNode));
        subwff_node->wff_string = malloc(sizeof(char));
        subwff_node->wff_string[0] = '\0';
        subwff_node->subwffs_count = 0;

        wff_node->subwffs[wff_node->subwffs_count] = subwff_node;
        wff_node->subwffs_count++;

        for (int i = 0; i < parse_node->child_count; i++) {
            _wff_tree_create(parse_node->children[i], subwff_node);
        }

        wff_node->wff_string = realloc(wff_node->wff_string, (strlen(wff_node->wff_string) + strlen(subwff_node->wff_string) + 1) * sizeof(char));
        strcat(wff_node->wff_string, subwff_node->wff_string);
    }
}

void wff_tree_print(WffTree* wff_tree) {
    _wff_tree_print(wff_tree->root, 0);
}

void _wff_tree_print(WffTreeNode* wff_node, int level) { 
    for (int i = 0; i < wff_node->subwffs_count / 2; i++) {
        _wff_tree_print(wff_node->subwffs[i], level + 1);
    }
    for (int i = 0; i < level; i++) {
        printf("\t\t");
    }
    printf("%s\n", wff_node->wff_string);
    for (int i = wff_node->subwffs_count / 2; i < wff_node->subwffs_count; i++) {
        _wff_tree_print(wff_node->subwffs[i], level + 1);
    }
}


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
    return list;
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

// Use set / hash set
void wff_list_print_unique(WffList* subwffs_list) {
    char* done[subwffs_list->length];
    size_t done_count = 0;
    for (WffListNode* node = subwffs_list->start; node != NULL; node = node->next) {
        bool isUniqe = true;
        for (int i = 0; i < done_count; i++) {
            if (strcmp(node->wff->string, done[i]) == 0) {
                isUniqe = false;
                break;
            }
        }
        if (isUniqe) {
            printf("%s\n", node->wff->string);
            done[done_count] = node->wff->string;
            done_count++;
        }
    }
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
    return list;
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
    return list;
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

WffMatch* wff_match_list_get(WffMatchList* list, size_t index, bool set_current) {
    if (index >= list->length) {
        return NULL;
    } else if (index + 1 == list->length) {
        if (set_current) {
            list->current = list->end->next;
        }
        return list->end->match;
    }

    WffMatchListNode* node = list->start; 
    for (size_t i = 0; i < index; i++) {
        node = node->next;
    }
    if (set_current) {
        list->current = node->next;
    }
    return node->match;
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
    return list;
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