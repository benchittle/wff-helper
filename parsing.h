#ifndef PARSING_H_
#define PARSING_H_

#include <stdbool.h>
#include <stdlib.h>

#include "datastructs.h"


Wff* wff_create(char* wff_string);
void wff_destroy(Wff* wff);
size_t wff_subwffs(Wff* wff, Wff*** subwffs);
WffMatchList* wff_match(Wff* wff, char* wff_pattern_string);


/* 
 * Given a string denoting a wff, identify all wff tokens in the string and 
 * store them in order (left to right) in 'tokenArray'.
 * Returns the number of tokens found.
 */
int tokenize_wff(const char* wff, WffToken* tokenArray);

#endif