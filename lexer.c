#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "reader.h"

Lexer *NewLexer(Reader *r) {
	Lexer *l = calloc(1, sizeof(Lexer));
	l->r = r;
	l->state = STATE_A;
	l->pos.line=1;
	l->pos.col=1;
	
	return l;
}

Token lexer_advance(Lexer *l) {
	Token t = {0};
	char buf[12] = {0};
	int token_index = 0;

	l->state = STATE_A;

	while (true) {
		int c = reader_peek(l->r);

		enum Q input = tolang(c);
		
		enum lexer_state nextstate = delta[l->state][input];

		switch (nextstate) {
		case (STATE_B):
		case (STATE_NUM):
			if (token_index > 11) {
				printf("error: integer overflow (%d, %d)\n", l->pos.line, l->pos.col);
			} else {
				buf[token_index++] = c;
			}
			break;
		case (STATE_OP):
			t.val.op = (l->state == STATE_B) ?\
					buf[0] : c;
		default:
			break;
		}	

		//	printf("got input %d, so we transition from state %d state %d\n", input, l->state, nextstate);

		if (nextstate == STATE_PHI) {
			if (ISFINALSTATE(l->state))
				break;
			else {
				if (t.val.err) {
					printf("invalid token at (%d, %d). final lexer state = %d\n", l->pos.line, l->pos.col, l->state);
					t.kind = TOKEN_ERR;
					t.val.err = 1;
					break;
				}
			}
		} 
		c = reader_next(l->r);
		if (input == INPUT_NEWLINE) {
			l->pos.col = 1;
			l->pos.line++;
		}
		l->pos.i++;
		l->pos.col++;
		l->state = nextstate;
	}

	switch (l->state) {
		case (STATE_LPAREN):
			t.kind = TOKEN_LPAREN; 
			break;
		case (STATE_RPAREN):
			t.kind = TOKEN_RPAREN; 
			break;
		case (STATE_NUM):
			t.kind = TOKEN_NUMBER;
			t.val.number = strtol(buf, NULL, 0);
			break;
		case (STATE_EOF):
			t.kind = TOKEN_EOF;
			break;
		case (STATE_OP):
			switch (t.val.op) {
			case '+':
				t.kind = TOKEN_PLUS;
				break;
			case '-':
				t.kind = TOKEN_MINUS;
				break;
			case '/':
				t.kind = TOKEN_SLASH;
				break;
			case '*':
				t.kind = TOKEN_ASTERIX;
				break;
			case '%':
				t.kind = TOKEN_PERCENT;
				break;
			}
			break;
		default:
			printf("idk what happend... final state: %d\n", l->state);
			break;
	}

	return t;
}

TokenList *Scan(Lexer *l) {
	TokenList *list = calloc(1, sizeof(TokenList));
	TokenList *tail = list;
	while (true) {
		tail->token = lexer_advance(l);

		if (tail->token.kind == TOKEN_EOF || tail->token.kind == TOKEN_ERR) 
				break;

		tail->next = calloc(1, sizeof(TokenList));
		tail = tail->next;
	}

	return list;
}

void PrintToken(Token *t) {
	if (!t || t->kind > TOKEN_EOF)
		return;
	
	printf("kind: %s\n", TOKEN_NAME[t->kind]);
	if (t->kind == TOKEN_NUMBER)
		printf("val: %li\n", t->val.number);	
	
	return;
}
