#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include "parsing.h"
#include "datastructs.h"

/* 
 * A parsing token. If it is of type WTT_PROPOSITION, the 'variable' member will
 * store the letter denoting the propositional variable. Otherwise, if it is of 
 * type WTT_OPERATOR, the 'operator' member will indicate which operator using the 
 * above enum.
 */
struct WffToken {
    WffTokenType type;
    union {
        char variable;
        WffOperator operator;
    };
};



/*
 * A parse tree node. If it is a terminal node, we use the 'token' member to
 * store the token for the terminal symbol. Otherwise, we use the 'child_count'
 * and 'children' members to store pointers to this node's children nodes 
 * (there can be up to 5, depending on the wff). 
 */
typedef struct WffParseTreeNode {
    bool isTerminal;
    union {
        struct {
            int child_count;
            struct WffParseTreeNode* children[5];
        };
        WffToken token;
    };
} WffParseTreeNode;

struct WffParseTree {
    WffParseTreeNode* root;
};

struct WffTreeNode {
    char* wff_string;
    uint8_t subwffs_count;
    struct WffTreeNode* subwffs[3];
};

struct WffTree {
    WffTreeNode* root;
};



size_t wff_tokenize(char* wff_string, WffToken** token_array_ptr);
bool wff_token_equal(WffToken token1, WffToken token2);
void wff_token_print(WffToken* token);

bool wff_validate(const WffToken* token_array, const size_t token_count);
bool _wff_validate(WffToken* const tokenArray, const int length, int* index);
WffToken* _wff_next_token(WffToken* const tokenArray, const int length, int* index);

WffParseTree* wff_parse(WffToken* const tokenArray, int length);
bool _wff_parse(WffToken* const tokenArray, const int length, int* index, WffParseTreeNode* node);
void wff_parsetree_print(WffParseTree* tree);
void _wff_parsetree_print(WffParseTreeNode* node, int level);

char* token_get_string(WffToken* token);
char* _wff_subwffs(WffParseTreeNode* node, char* subwffs[], size_t* index);

WffTree* wff_tree_create(WffParseTree* parse_tree);
void _wff_tree_create(WffParseTreeNode* parse_node, WffTreeNode* wff_node);
void wff_tree_print(WffTree* wff_tree);
void _wff_tree_print(WffTreeNode* wff_node, int level);

bool _wff_match(WffParseTreeNode* wff_parse_node, WffParseTreeNode* pattern_parse_node, WffList* list);

// ========

Wff* wff_create(char* wff_string) {
    Wff* wff = malloc(sizeof(Wff));
    wff->string = wff_string;
    wff->parse_tree = NULL;

    WffToken* token_array = NULL;
    size_t token_count = wff_tokenize(wff->string, &token_array);

    wff->token_array = token_array;
    wff->token_count = token_count;

    wff->parse_tree = wff_parse(token_array, token_count);
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

/* wff_tokenize
 * wff -- Wff to tokenize
 * token_array_ptr -- Address of the caller's pointer to store an array. The 
 *      caller's pointer will be realloc'd and overwritten.
 * Returns the number of tokens in the array starting from *token_array_ptr.
 */
size_t wff_tokenize(char* wff_string, WffToken** token_array_ptr) {
    // Get the pointer to the array and reallocate it with a default size of 32
    // tokens.
    WffToken* token_array = *token_array_ptr;
    size_t array_length = 32;
    token_array = realloc(token_array, array_length * sizeof(*token_array));
    size_t token_count = 0;

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

            default:
                if (('a' <= *c && *c <= 'z') || ('A' <= *c && *c <= 'Z')) {
                    token.type = WTT_PROPOSITION;
                    token.variable = *c;
                } else {
                    printf("ERROR: Unexpected token: %c\n", *c);
                    return -1;
                }
        } // switch
        
        // If a token was created, append it to the array.
        if (token.type != WTT_NONE) {
            token_array[token_count] = token;
            token_count++;
            // Resize the array if full.
            if (token_count == array_length) {
                array_length *= 2;
                token_array = realloc(token_array, array_length * sizeof(*token_array));
            }
        }
    }
    // Resize the array to the exact number of tokens.
    if (token_count != array_length) {
        token_array = realloc(token_array, token_count * sizeof(*token_array));
    }
    // Update the caller's pointer to the potentially new address of the array.
    *token_array_ptr = token_array;
    return token_count;
}

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
            return token1.variable == token2.variable;
            break;
        default:
            printf("ERROR: Unhandled token type\n");
            exit(1);
    }
}

void wff_token_print(WffToken* token) {
    if (token->type == WTT_OPERATOR) {
        switch (token->operator) {
            case WO_NOT: 
                printf("~"); 
                break;
            case WO_AND: 
                printf("^"); 
                break;
            case WO_OR:
                printf("v");
                break;
            case WO_COND:
                printf("=>");
                break;
            case WO_BICOND:
                printf("<=>");
                break;
        }
    } else if (token->type == WTT_PROPOSITION) {
        printf("%c", token->variable);
    } else if (token->type == WTT_LPAREN) {
        printf("(");
    } else {
        printf(")");
    }
}


bool wff_validate(const WffToken* token_array, const size_t token_count) {
    int index = 0;
    bool result = _wff_validate(token_array, token_count, &index);
    if (index < token_count) {
        return false;
    } else {
        return result;
    }
}

bool _wff_validate(WffToken* const tokenArray, const int length, int* index) {
    int savedIndex = *index; 
    WffToken* next;
    if (_wff_next_token(tokenArray, length, index)->type == WTT_PROPOSITION) {
        return true;
    } 
    *index = savedIndex;
    next = _wff_next_token(tokenArray, length, index);
    if (next->type == WTT_OPERATOR && next->operator == WO_NOT && _wff_validate(tokenArray, length, index)) {
        return true;
    } 
    *index = savedIndex;
    if (_wff_next_token(tokenArray, length, index)->type == WTT_LPAREN && _wff_validate(tokenArray, length, index)) {
        savedIndex = *index;
        next = _wff_next_token(tokenArray, length, index);
        if (next->type == WTT_OPERATOR
                && (next->operator == WO_AND || next->operator == WO_OR || next->operator == WO_COND || next->operator == WO_BICOND)
                && _wff_validate(tokenArray, length, index)
                && _wff_next_token(tokenArray, length, index)->type == WTT_RPAREN) {
            return true;
        }
    }
    return false;
}

WffToken* _wff_next_token(WffToken* const tokenArray, const int length, int* index) {
    if (*index <= length) {
        *index += 1;
        return tokenArray + *index - 1;
    } else {
        return NULL;
    }
}


WffParseTree* wff_parse(WffToken* const token_array, int length) {
    WffParseTreeNode* root = malloc(sizeof(WffParseTreeNode));
    int index = 0;

    bool valid = _wff_parse(token_array, length, &index, root);
    if (valid && index >= length) {
        WffParseTree* tree = malloc(sizeof(WffParseTree));
        tree->root = root;
        return tree;
    } else {
        // FREE NODES
        return NULL;
    }
}

bool _wff_parse(WffToken* const tokenArray, const int length, int* index, WffParseTreeNode* node) {
    //int savedIndex = *index; 
    WffToken* next = _wff_next_token(tokenArray, length, index);
    if (next->type == WTT_PROPOSITION) {
        node->isTerminal = false;
        node->child_count = 1;
        WffParseTreeNode* newNode = malloc(sizeof(WffParseTreeNode));
        newNode->isTerminal = true;
        newNode->token = *next;
        node->children[0] = newNode;
        return true;
    } else if (next->type == WTT_OPERATOR && next->operator == WO_NOT) {
        node->isTerminal = false;
        node->child_count = 2;
        // First child
        WffParseTreeNode* newNode = malloc(sizeof(WffParseTreeNode));
        newNode->isTerminal = true;
        newNode->token = *next;
        node->children[0] = newNode;
        // Second child
        newNode = malloc(sizeof(WffParseTreeNode));
        _wff_parse(tokenArray, length, index, newNode);
        node->children[1] = newNode;
        return true;
    } else if (next->type == WTT_LPAREN) {
        node->isTerminal = false;
        node->child_count = 5;
        // First child
        WffParseTreeNode* newNode = malloc(sizeof(WffParseTreeNode));
        newNode->isTerminal = true;
        newNode->token = *next;
        node->children[0] = newNode;
        // Second child
        newNode = malloc(sizeof(WffParseTreeNode));
        _wff_parse(tokenArray, length, index, newNode);
        node->children[1] = newNode;
        // Third child
        next = _wff_next_token(tokenArray, length, index);
        if (next->type != WTT_OPERATOR || (next->type == WTT_OPERATOR && next->operator != WO_AND && next->operator != WO_OR && next->operator != WO_COND && next->operator != WO_BICOND)) {
            return false;
        }
        newNode = malloc(sizeof(WffParseTreeNode));
        newNode->isTerminal = true;
        newNode->token = *next;
        node->children[2] = newNode;
        // Fourth child
        newNode = malloc(sizeof(WffParseTreeNode));
        _wff_parse(tokenArray, length, index, newNode);
        node->children[3] = newNode;
        // Fifth child
        next = _wff_next_token(tokenArray, length, index);
        if (next->type != WTT_RPAREN) {
            return false;
        }
        newNode = malloc(sizeof(WffParseTreeNode));
        newNode->isTerminal = true;
        newNode->token = *next;
        node->children[4] = newNode;
        return true;
    }
    return false;
}

void wff_parsetree_print(WffParseTree* tree) {
    _wff_parsetree_print(tree->root, 0);
}

void _wff_parsetree_print(WffParseTreeNode* node, int level) {
    if (node->isTerminal) {
        for (int i = 0; i < level; i++) {
            printf("    ");
        }
        wff_token_print(&(node->token));
        printf("\n");
    } else {
        for (int i = 0; i < node->child_count / 2; i++) {
            _wff_parsetree_print(node->children[i], level + 1);
        }
        for (int i = 0; i < level; i++) {
            printf("    ");
        }
        printf("wff\n");
        for (int i = node->child_count / 2; i < node->child_count; i++) {
            _wff_parsetree_print(node->children[i], level + 1);
        }
    }
}


char* token_get_string(WffToken* token) {
    char* str;
    if (token->type == WTT_OPERATOR) {
        switch (token->operator) {
            case WO_NOT: 
                str = calloc(2, sizeof(char));
                str[0] = '~';
                break;
            case WO_AND: 
                str = calloc(2, sizeof(char));
                str[0] = '^';
                break;
            case WO_OR:
                str = calloc(2, sizeof(char));
                str[0] = 'v';
                break;
            case WO_COND:
                str = calloc(3, sizeof(char));
                str[0] = '=';
                str[1] = '>';
                break;
            case WO_BICOND:
                str = calloc(4, sizeof(char));
                str[0] = '<';
                str[1] = '=';
                str[2] = '>';
                break;
        }
    } else if (token->type == WTT_PROPOSITION) {
        str = calloc(2, sizeof(char));
        str[0] = token->variable;
    } else if (token->type == WTT_LPAREN) {
        str = calloc(2, sizeof(char));
        str[0] = '(';
    } else {
        str = calloc(2, sizeof(char));
        str[0] = ')';
    }
    return str;
}

//UNFINISHED: subwff_strs is limited length
size_t wff_subwffs(Wff* wff, Wff*** subwffs_ptr) {
    size_t index = 0;
    Wff** subwffs = *subwffs_ptr;
    char* subwff_strs[100];
    _wff_subwffs(wff->parse_tree->root, subwff_strs, &index);
    
    subwffs = realloc(subwffs, index * sizeof(Wff*));
    for (int i = 0; i < index; i++) {
        subwffs[i] = wff_create(subwff_strs[i]);
    }
    *subwffs_ptr = subwffs;
    return index;
}

char* _wff_subwffs(WffParseTreeNode* node, char* subwffs[], size_t* index) {
    char* str;
    if (node->isTerminal) {
        str = token_get_string(&(node->token));
        return str;
    } else {
        str = malloc(1);
        int str_size = 1;
        for (int i = 0; i < node->child_count; i++) {
            char* substr = _wff_subwffs(node->children[i], subwffs, index);
            str_size += strlen(substr);
            str = realloc(str, str_size);
            strcat(str, substr);
        }
        subwffs[*index] = str;
        *index += 1;
        return str;
    }
}

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
    if (parse_node->isTerminal) {
        char* token_string = token_get_string(&(parse_node->token));
        wff_node->wff_string = realloc(wff_node->wff_string, (strlen(wff_node->wff_string) + strlen(token_string) + 1) * sizeof(char));
        strcat(wff_node->wff_string, token_string);
        free(token_string);
        return; 
    } else {
        WffTreeNode* subwff_node = malloc(sizeof(WffTreeNode));
        subwff_node->wff_string = malloc(sizeof(char));
        subwff_node->wff_string[0] = '\0';
        subwff_node->subwffs_count = 0;

        wff_node->subwffs[wff_node->subwffs_count] = subwff_node;
        wff_node->subwffs_count++;

        for (uint8_t i = 0; i < parse_node->child_count; i++) {
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

void print_unique(Wff** subwffs, size_t length) {
    char* done[length];
    int done_count = 0;
    for (int i = 0; i < length; i++) {
        bool isUniqe = true;
        for (int j = 0; j < done_count; j++) {
            if (strcmp(subwffs[i]->string, done[j]) == 0) {
                isUniqe = false;
                break;
            }
        }
        if (isUniqe) {
            printf("%d: '%s'\n", i, subwffs[i]->string);
            done[done_count] = subwffs[i]->string;
            done_count++;
        }
    }
}


// TODO: We need to identify a list of all matches for a given equivalence
// - need a linked list?
// - what is an equivalence doing ?= swapping out one subwff (subtree) for an
//   equivalent one
//      - part 1: identify one or more subwffs (subtrees) matching our rule
//      - part 2: make the substition in one subwff
// Think about making Wff a tree itself 

WffParseTreeNode* _wff_e12(WffParseTreeNode* node) {
    if (node->isTerminal) {
        return NULL;
    }
    if (node->child_count == 5) {
        WffParseTreeNode* child = node->children[2];
        if (child->isTerminal && child->token.type == WTT_OPERATOR && child->token.operator == WO_AND) {
            return node;
        }
    }
}

Wff* wff_e12(Wff* wff) {
    
}

// TEMP: Maybe have this function be done by parse tree creation using flag?
void make_nonterminal(WffParseTreeNode* node) {
    for (int i = 0; i < node->child_count; i++) {
        WffParseTreeNode* child = node->children[i];
        if (child->isTerminal) {
            if (child->token.type == WTT_PROPOSITION) {
                free(child);
                node->child_count = 0;
                return;
            }
        } else {
            make_nonterminal(child);
        }
    }
}

bool wff_match(Wff* wff, char* wff_pattern_string) {
    Wff* pattern = wff_create(wff_pattern_string);
    make_nonterminal(pattern->parse_tree->root);

    WffTokenList* token_matches = NULL; //wff_token_list_create();
    return _wff_match(wff->parse_tree->root, pattern->parse_tree->root, token_matches);
}

bool _wff_match(WffParseTreeNode* wff_parse_node, WffParseTreeNode* pattern_parse_node, WffList* list) {
    if (pattern_parse_node->isTerminal) {
        if (wff_parse_node->isTerminal) {
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
        } else {
            return false;
        }
    } else {
        if (wff_parse_node->isTerminal) {
            return pattern_parse_node->child_count == 0;
        } else {
            if (pattern_parse_node->child_count > 0) {
                if (wff_parse_node->child_count == pattern_parse_node->child_count) {
                    bool isEqual = true;
                    for (int i = 0; i < wff_parse_node->child_count; i++) {
                        isEqual = isEqual && _wff_match(wff_parse_node->children[i], pattern_parse_node->children[i], list);
                    }
                    return isEqual;
                } else {
                    return false;
                }
            } else {
                bool isEqual = true;
                for (int i = 0; i < wff_parse_node->child_count; i++) {
                    isEqual = isEqual && _wff_match(wff_parse_node->children[i], pattern_parse_node, list);
                }
                return isEqual;
            }
        }
    }
}


/*
void test_wffs() {
    char* wffs[] = {"~", "v", "^", "=>", "<=>",
                    "p", "q", "pq", "qp",
                    "~p", "(p v q)", "(p ^ q)", "(p => q)", "(p <=> q)",
                    "~(p v q)", "(~p v ~q)", "~(p => ~q)", "~~(~~p ^ ~~q)",
                    "(((p v q) ^ (p v ~q)) => p)", "(p v q) ^ (p v ~q) => p",
                    "(p v q v r)", "(p v q ^ r)"};
    bool results[] = {false, false, false , false, false,
                      true, true, false, false,
                      true, true, true, true, true,
                      true, true, true, true,
                      true, false,
                      false, false
                      };
    printf("Testing wffs...\n");
    int passed = 0;
    int failed = 0;
    for (int i = 0; i < sizeof(wffs) / sizeof(*wffs); i++) {
        char* wff = wffs[i];
        printf("wff %d: '%s'\n", i, wff);

        WffToken tokenArray[100];
        int count = tokenize_wff(wff, tokenArray);
        printf("\t%d tokens\n", count);

        for (int j = 0; j < count; j++) {
            printf("\t\tToken %d: ", j);
            print_token(tokenArray + j);
            printf("\n");
        }

        bool isValid = validate_wff(tokenArray, count);
        if (isValid == results[i]) {
            printf("\tTest passed: result=%d\n", isValid);
            passed++;
        } else {
            printf("\tTest failed: result=%d, should be %d\n", isValid, results[i]);
            failed++;
        }
    }
    printf("%d CASES PASSED\n%d CASES FAILED\n", passed, failed);
}
*/

int main(void) {
    char* wff_string = "(((p v q) v (r v t)) <=> ((p v q) v (r v t)))"; //"(((p v q) ^ (p v ~q)) => p)";
    Wff* wff = wff_create(wff_string);

    printf("%s\n", wff->string);
    for (int j = 0; j < wff->token_count; j++) {
        printf("\t\tToken %d: ", j);
        wff_token_print(&(wff->token_array[j]));
        printf("\n");
    }
    printf("PARSE TREE:\n");
    wff_parsetree_print(wff->parse_tree);

    /*
    Wff** subwffs = NULL;
    size_t subwff_count = wff_subwffs(wff, &subwffs);
    print_unique(subwffs, subwff_count);
    */

    printf("\n\nWFF TREE:\n");
    wff_tree_print(wff->wff_tree);

    char* search = "(((p v q) v (r v t)) => ((p v q) v (r v t)))";
    bool result = wff_match(wff, search);
    printf("Searching in wff '%s' for pattern '%s': %d\n", wff->string, search, result);





    
    /*char* subwffs[100];
    WffToken* tokenArray[100];
    int token_count = tokenize_wff(wff, tokenArray);
    WffParseTree tree = parse_wff(tokenArray, token_count);
    int num_subwffs = sub_wffs(tree, subwffs);
    printf("%d subwffs:\n", num_subwffs);
    print_unique(subwffs, num_subwffs); 
    */
    
    /*for (int i = 0; i < num_subwffs; i++) {
        printf("%d: '%s'\n", i, subwffs[i]);
    }*/
    //print_parsetree(tree);

    printf("done\n");
}







