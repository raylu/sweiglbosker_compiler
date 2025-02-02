#ifndef _PARSER_H
#define _PARSER_H

#include "lexer.h"

typedef enum {
	RULE_TERMINAL,
	RULE_E,
	RULE_E2,  
	RULE_E3
} Rule; 

static const char *RULENAME[] = {
	[RULE_TERMINAL] = "TERMINAL",
	[RULE_E] = "E",
	[RULE_E2] = "E2",
	[RULE_E3] = "E3",
};

// Parse Tree
typedef struct ParseTree {
	Rule r;	/* grammar used to contruct tree */ 

	union {
		Token token;
	//	int n;	/* number of child trees. (X) */
	//	struct T *X[];
		struct {
			struct ParseTree *neighbor; /* one of the few optimizations I will make. children/siblings are already ordered so we can use linked list */ 
			struct ParseTree *child;
		};
	};
} ParseTree;


// production = table[nonterminal][terminal]

ParseTree* NewTree(Rule r);
ParseTree* ParseE(TokenList **);
ParseTree* ParseE2(TokenList **);
ParseTree* ParseE3(TokenList **);
ParseTree* ParseTerminal(TokenList **);

#endif /* PARSER_H */ 
