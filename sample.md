# Examples

## Example 1: Happy Path

Start of proof
```
PROVE (p v q) ^ (p v ~q) => p
(Direct Proof)
1. (p v q) ^ (p v ~q)              (hypothesis)
2. [show preview of result based on what user has typed]





> E14 1
```

User types valid action 
```
PROVE (p v q) ^ (p v ~q) => p
(Direct Proof)
1. (p v q) ^ (p v ~q)              (hypothesis)
2. p v (q ^ ~q)                    (E14, 1)
3. 





> E1 2
```

User types valid action 
```
PROVE (p v q) ^ (p v ~q) => p
(Direct Proof)
1. (p v q) ^ (p v ~q)              (hypothesis)
2. p v (q ^ ~q)                    (E14, 1)
3. p v F                           (E1, 2)
4.




> E6 3
```

User types valid action
```
PROVE (p v q) ^ (p v ~q) => p
(Direct Proof)
1. (p v q) ^ (p v ~q)              (hypothesis)
2. p v (q ^ ~q)                    (E14, 1)
3. p v F                           (E1, 2)
4. p                               (E6, 3)




PROOF DONE
```

## Example 2: Ambiguous Commands
Start of proof
```
PROVE (p v q) ^ (p v ~q) => p
(Direct Proof)
1. (q v p) ^ (p v ~q)              (hypothesis)
2. [show preview of result based on what user has typed]





> E10 1
```

There is ambiguity with the previous action: which pair of symbols should we apply E10 to? We should allow the user to scroll through possible outcomes, or type in the desired wff manually. Typing will be complicated: we will have to check if the user's wff is one of the valid outcomes. If not, we must tell them. Autocomplete would be neat with this, but not necessary. Typing could also be optional at first. Assume the user scrolls to their desired usage here:
```
PROVE (p v q) ^ (p v ~q) => p
(Direct Proof)
1. (q v p) ^ (p v ~q)                (hypothesis)
2. (p v q) ^ (p v ~q)                    (E10, 1)





AMBIGUOUS COMMAND: Scroll using arrow keys, press enter to select
> E10 1
```

Result of scroll to right:
(the change should be highlighted)
```
PROVE (p v q) ^ (p v ~q) => p
(Direct Proof)
1. (q v p) ^ (p v ~q)                (hypothesis)
2. (q v p) ^ (~q v p)                    (E10, 1)





AMBIGUOUS COMMAND: Scroll using arrow keys, press enter to select
> E10 1
```

Another scroll to the right should loop back to the first option:
(the change should be highlighted)
```
PROVE (p v q) ^ (p v ~q) => p
(Direct Proof)
1. (q v p) ^ (p v ~q)                (hypothesis)
2. (p v q) ^ (p v ~q)                    (E10, 1)





AMBIGUOUS COMMAND: Scroll using arrow keys, press enter to select
> E10 1
```

User presses enter
```
PROVE (p v q) ^ (p v ~q) => p
(Direct Proof)
1. (q v p) ^ (p v ~q)                (hypothesis)
2. (p v q) ^ (p v ~q)                    (E10, 1)
3. 





>
```

# Requirements

## Parsing
We need to be able to tokenize and parse wffs

For example given the wff `(p v q) ^ (p v ~q) => p`, we need to convert it into a series of tokens:
```
tokenize_wff(wff)

OUTPUT
{LPAREN, proposition(p), operator(v), proposition(q), RPAREN, operator(^), LPAREN, proposition(p), operator(v), operator(~), proposition(q), RPAREN, operator(=>), proposition(p)}
```

For example given the wff `(p v q) ^ (p v ~q) => p`, we need a way to split this into all the possible sub-wffs:
```
sub_wffs(wff)

OUTPUT:
{"p", "q", "p v q", "~q", "p v ~q", "(p v q) ^ (p v ~q)", "(p v q) ^ (p v ~q) => p"}
```

Grammar
```
proposition ::= [a-zA-Z]

wff ::= T 
      | F
      | proposition
      | ~wff 
      | (wff ^ wff) 
      | (wff v wff) 
      | (wff => wff) 
      | (wff <=> wff)

====
proposition ::= [a-zA-Z]

bioperator ::= ^ | v | => | <=>

unarywff ::= ~wff

binarywff ::= (wff bioperator wff)

wff ::= proposition
      | unarywff
      | binarywff
```

## Applying Equivalences
Given a wff and an equivalence, we need to determine all possible resulting wffs

For example, given the wff `(p v q) ^ (p v ~q)` if we want to apply E14:
```
equivalent_wffs(wff, equivalence)

OUTPUT:
{"p v (q ^ ~q)"}
```
Alternatively, given the wff `(p v q) ^ (p v ~q)`, suppose we want to apply E10:
```
equivalent_wffs(wff, equivalence)

OUTPUT:
{"(q v p) ^ (p v ~q)", "(p v q) ^ (~q v p)"}
```


binary unary


old code
```
bool _wff(token_t* const tokenArray, const int length, int* index) {
    int savedIndex = *index; 
    token_t* next = next_token(tokenArray, length, index);
    if (next->type == PROPOSITION) {
        print_token(next);
        return true;
    } 
    *index = savedIndex;
    next = next_token(tokenArray, length, index);
    if (next->type == OPERATOR && next->operator == NOT && _wff(tokenArray, length, index)) {
        print_token(next);
        return true;
    } 
    *index = savedIndex;
    next = next_token(tokenArray, length, index);
    if (next->type == LPAREN) {
        print_token(next);
        if (_wff(tokenArray, length, index)) {
            savedIndex = *index;
            next = next_token(tokenArray, length, index);
            if (next->type == OPERATOR && (next->operator == AND || next->operator == OR || next->operator == COND || next->operator == BICOND)) {
                print_token(next);
                if (_wff(tokenArray, length, index)) {
                    next = next_token(tokenArray, length, index);
                    if (next->type == RPAREN) {
                        print_token(next);
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
```