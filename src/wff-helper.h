#ifndef WFF_HELPER_H_
#define WFF_HELPER_H_

#include <stdbool.h>
#include <stdlib.h>

#include "logic.h"




WffMatchList* wff_match(Wff* wff, char* wff_pattern_string);
bool wff_substitute(Wff* wff, char* search, char* replace, size_t index);


#endif