#ifndef _LEXER_H_
#define _LEXER_H_

#include "def.h"

enum {
#define xx(a,b,c,d,e,f,g) a=b,
#define yy(a,b,c,d,e,f,g)
#include "tok.h"
	LAST
};

struct lookup_table {
    int value;
    char string[20];
};

int lexer(scc_state *s1);

#endif