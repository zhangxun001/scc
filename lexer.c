#include "main.h"
#include "lexer.h"
#include "def.h"

enum { BLANK=01,  NEWLINE=02, LETTER=04,
       DIGIT=010, HEX=020,    OTHER=040 };

static unsigned char map[256] = { 
                   /* 000 nul */	0,
				   /* 001 soh */	0,
				   /* 002 stx */	0,
				   /* 003 etx */	0,
				   /* 004 eot */	0,
				   /* 005 enq */	0,
				   /* 006 ack */	0,
				   /* 007 bel */	0,
				   /* 010 bs  */	0,
				   /* 011 ht  */	BLANK,
				   /* 012 nl  */	NEWLINE,
				   /* 013 vt  */	BLANK,
				   /* 014 ff  */	BLANK,
				   /* 015 cr  */	0,
				   /* 016 so  */	0,
				   /* 017 si  */	0,
				   /* 020 dle */	0,
				   /* 021 dc1 */	0,
				   /* 022 dc2 */	0,
				   /* 023 dc3 */	0,
				   /* 024 dc4 */	0,
				   /* 025 nak */	0,
				   /* 026 syn */	0,
				   /* 027 etb */	0,
				   /* 030 can */	0,
				   /* 031 em  */	0,
				   /* 032 sub */	0,
				   /* 033 esc */	0,
				   /* 034 fs  */	0,
				   /* 035 gs  */	0,
				   /* 036 rs  */	0,
				   /* 037 us  */	0,
				   /* 040 sp  */	BLANK,
				   /* 041 !   */	OTHER,
				   /* 042 "   */	OTHER,
				   /* 043 #   */	OTHER,
				   /* 044 $   */	0,
				   /* 045 %   */	OTHER,
				   /* 046 &   */	OTHER,
				   /* 047 '   */	OTHER,
				   /* 050 (   */	OTHER,
				   /* 051 )   */	OTHER,
				   /* 052 *   */	OTHER,
				   /* 053 +   */	OTHER,
				   /* 054 ,   */	OTHER,
				   /* 055 -   */	OTHER,
				   /* 056 .   */	OTHER,
				   /* 057 /   */	OTHER,
				   /* 060 0   */	DIGIT,
				   /* 061 1   */	DIGIT,
				   /* 062 2   */	DIGIT,
				   /* 063 3   */	DIGIT,
				   /* 064 4   */	DIGIT,
				   /* 065 5   */	DIGIT,
				   /* 066 6   */	DIGIT,
				   /* 067 7   */	DIGIT,
				   /* 070 8   */	DIGIT,
				   /* 071 9   */	DIGIT,
				   /* 072 :   */	OTHER,
				   /* 073 ;   */	OTHER,
				   /* 074 <   */	OTHER,
				   /* 075 =   */	OTHER,
				   /* 076 >   */	OTHER,
				   /* 077 ?   */	OTHER,
				   /* 100 @   */	0,
				   /* 101 A   */	LETTER|HEX,
				   /* 102 B   */	LETTER|HEX,
				   /* 103 C   */	LETTER|HEX,
				   /* 104 D   */	LETTER|HEX,
				   /* 105 E   */	LETTER|HEX,
				   /* 106 F   */	LETTER|HEX,
				   /* 107 G   */	LETTER,
				   /* 110 H   */	LETTER,
				   /* 111 I   */	LETTER,
				   /* 112 J   */	LETTER,
				   /* 113 K   */	LETTER,
				   /* 114 L   */	LETTER,
				   /* 115 M   */	LETTER,
				   /* 116 N   */	LETTER,
				   /* 117 O   */	LETTER,
				   /* 120 P   */	LETTER,
				   /* 121 Q   */	LETTER,
				   /* 122 R   */	LETTER,
				   /* 123 S   */	LETTER,
				   /* 124 T   */	LETTER,
				   /* 125 U   */	LETTER,
				   /* 126 V   */	LETTER,
				   /* 127 W   */	LETTER,
				   /* 130 X   */	LETTER,
				   /* 131 Y   */	LETTER,
				   /* 132 Z   */	LETTER,
				   /* 133 [   */	OTHER,
				   /* 134 \   */	OTHER,
				   /* 135 ]   */	OTHER,
				   /* 136 ^   */	OTHER,
				   /* 137 _   */	LETTER,
				   /* 140 `   */	0,
				   /* 141 a   */	LETTER|HEX,
				   /* 142 b   */	LETTER|HEX,
				   /* 143 c   */	LETTER|HEX,
				   /* 144 d   */	LETTER|HEX,
				   /* 145 e   */	LETTER|HEX,
				   /* 146 f   */	LETTER|HEX,
				   /* 147 g   */	LETTER,
				   /* 150 h   */	LETTER,
				   /* 151 i   */	LETTER,
				   /* 152 j   */	LETTER,
				   /* 153 k   */	LETTER,
				   /* 154 l   */	LETTER,
				   /* 155 m   */	LETTER,
				   /* 156 n   */	LETTER,
				   /* 157 o   */	LETTER,
				   /* 160 p   */	LETTER,
				   /* 161 q   */	LETTER,
				   /* 162 r   */	LETTER,
				   /* 163 s   */	LETTER,
				   /* 164 t   */	LETTER,
				   /* 165 u   */	LETTER,
				   /* 166 v   */	LETTER,
				   /* 167 w   */	LETTER,
				   /* 170 x   */	LETTER,
				   /* 171 y   */	LETTER,
				   /* 172 z   */	LETTER,
				   /* 173 {   */	OTHER,
				   /* 174 |   */	OTHER,
				   /* 175 }   */	OTHER,
				   /* 176 ~   */	OTHER, };

struct lookup_table lt[] = {
#define xx(a,b,c,d,e,f,g) {.value = b, .string = g},
#define yy(a,b,c,d,e,f,g) {.value = b, .string = g},
#include "tok.h"
#undef xx
#undef yy
};

unsigned char *cp;
char *token;		/* current token */
unsigned long num;

char buf[1000] = {0};
char *p_buf = buf;
char *stringn(const char* token, size_t len)
{
    char *tok = p_buf;
    for (int i = 0; i < len; i++) {
        *(p_buf + i) = token[i];
    }
    p_buf += len;
    *p_buf = '\0';
    p_buf++;
    return tok;
}

int gettok(void)
{
	for (;;) {
		register unsigned char *rcp = cp;
		while (map[*rcp]&BLANK)
			rcp++;
		cp = rcp + 1;
        char ch = *rcp++;
		switch (ch) {
        case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9': {
			unsigned long n = 0;
            char* lcp = rcp - 1;
			if (*lcp == '0' && (*rcp == 'x' || *rcp == 'X')) {
				int d, overflow = 0;
				while (*++rcp) {
					if (map[*rcp]&DIGIT)
						d = *rcp - '0';
					else if (*rcp >= 'a' && *rcp <= 'f')
						d = *rcp - 'a' + 10;
					else if (*rcp >= 'A' && *rcp <= 'F')
						d = *rcp - 'A' + 10;
					else
						break;
					if (n&~(~0UL >> 4))
						overflow = 1;
					else
						n = (n<<4) + d;
				}
				if ((char *)rcp - lcp <= 2)
					error("invalid hexadecimal constant `%S'\n", lcp, (char *)rcp-lcp);
				cp = rcp;
				num = n;
			} else if (*lcp == '0') {
				int err = 0, overflow = 0;
				for ( ; map[*rcp]&DIGIT; rcp++) {
					if (*rcp == '8' || *rcp == '9')
						err = 1;
					if (n&~(~0UL >> 3))
						overflow = 1;
					else
						n = (n<<3) + (*rcp - '0');
				}
				if (*rcp == '.' || *rcp == 'e' || *rcp == 'E') {
					cp = rcp;
                    /* TODO: float */
					return FCON;
				}
				cp = rcp;
                num = n;
				if (err)
					error("invalid octal constant `%S'\n", lcp, (char*)cp-lcp);
			} else {
				int overflow = 0;
				for (n = *lcp - '0'; map[*rcp]&DIGIT; ) {
					int d = *rcp++ - '0';
                    #define ULONG_MAX (~0UL)
					if (n > (ULONG_MAX - d)/10)
						overflow = 1;
					else
						n = 10*n + d;
				}
				if (*rcp == '.' || *rcp == 'e' || *rcp == 'E') {
					cp = rcp;
					return FCON;
				}
				cp = rcp;
				num = n;
			}
			return ICON;
		}
        case 'i':
        if (rcp[0] == 'f'
        && !(map[rcp[1]]&(DIGIT|LETTER))) {
            cp = rcp + 1;
            return IF;
        }
        if (rcp[0] == 'n'
        &&  rcp[1] == 't'
        && !(map[rcp[2]]&(DIGIT|LETTER))) {
            cp = rcp + 2;
            return INT;
        }
        goto id;
        case 'h': case 'j': case 'k': case 'm': case 'n': case 'o':
		case 'p': case 'q': case 'x': case 'y': case 'z':
		case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
		case 'G': case 'H': case 'I': case 'J': case 'K':
		case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
		case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
		case 'Y': case 'Z':
		id:
			token = (char *)rcp - 1;
			while (map[*rcp]&(DIGIT|LETTER))
				rcp++;
			token = stringn(token, (char *)rcp - token);
			cp = rcp;
			return ID;
		case 'a':
			if (rcp[0] == 'u'
			&&  rcp[1] == 't'
			&&  rcp[2] == 'o'
			&& !(map[rcp[3]]&(DIGIT|LETTER))) {
				cp = rcp + 3;
				return AUTO;
			}
			goto id;
		case 'b':
			if (rcp[0] == 'r'
			&&  rcp[1] == 'e'
			&&  rcp[2] == 'a'
			&&  rcp[3] == 'k'
			&& !(map[rcp[4]]&(DIGIT|LETTER))) {
				cp = rcp + 4;
				return BREAK;
			}
			goto id;
		case 'c':
			if (rcp[0] == 'a'
			&&  rcp[1] == 's'
			&&  rcp[2] == 'e'
			&& !(map[rcp[3]]&(DIGIT|LETTER))) {
				cp = rcp + 3;
				return CASE;
			}
			if (rcp[0] == 'h'
			&&  rcp[1] == 'a'
			&&  rcp[2] == 'r'
			&& !(map[rcp[3]]&(DIGIT|LETTER))) {
				cp = rcp + 3;
				return CHAR;
			}
			if (rcp[0] == 'o'
			&&  rcp[1] == 'n'
			&&  rcp[2] == 's'
			&&  rcp[3] == 't'
			&& !(map[rcp[4]]&(DIGIT|LETTER))) {
				cp = rcp + 4;
				return CONST;
			}
			if (rcp[0] == 'o'
			&&  rcp[1] == 'n'
			&&  rcp[2] == 't'
			&&  rcp[3] == 'i'
			&&  rcp[4] == 'n'
			&&  rcp[5] == 'u'
			&&  rcp[6] == 'e'
			&& !(map[rcp[7]]&(DIGIT|LETTER))) {
				cp = rcp + 7;
				return CONTINUE;
			}
			goto id;
		case 'd':
			if (rcp[0] == 'e'
			&&  rcp[1] == 'f'
			&&  rcp[2] == 'a'
			&&  rcp[3] == 'u'
			&&  rcp[4] == 'l'
			&&  rcp[5] == 't'
			&& !(map[rcp[6]]&(DIGIT|LETTER))) {
				cp = rcp + 6;
				return DEFAULT;
			}
			if (rcp[0] == 'o'
			&&  rcp[1] == 'u'
			&&  rcp[2] == 'b'
			&&  rcp[3] == 'l'
			&&  rcp[4] == 'e'
			&& !(map[rcp[5]]&(DIGIT|LETTER))) {
				cp = rcp + 5;
				return DOUBLE;
			}
			if (rcp[0] == 'o'
			&& !(map[rcp[1]]&(DIGIT|LETTER))) {
				cp = rcp + 1;
				return DO;
			}
			goto id;
		case 'e':
			if (rcp[0] == 'l'
			&&  rcp[1] == 's'
			&&  rcp[2] == 'e'
			&& !(map[rcp[3]]&(DIGIT|LETTER))) {
				cp = rcp + 3;
				return ELSE;
			}
			if (rcp[0] == 'n'
			&&  rcp[1] == 'u'
			&&  rcp[2] == 'm'
			&& !(map[rcp[3]]&(DIGIT|LETTER))) {
				cp = rcp + 3;
				return ENUM;
			}
			if (rcp[0] == 'x'
			&&  rcp[1] == 't'
			&&  rcp[2] == 'e'
			&&  rcp[3] == 'r'
			&&  rcp[4] == 'n'
			&& !(map[rcp[5]]&(DIGIT|LETTER))) {
				cp = rcp + 5;
				return EXTERN;
			}
			goto id;
		case 'f':
			if (rcp[0] == 'l'
			&&  rcp[1] == 'o'
			&&  rcp[2] == 'a'
			&&  rcp[3] == 't'
			&& !(map[rcp[4]]&(DIGIT|LETTER))) {
				cp = rcp + 4;
				return FLOAT;
			}
			if (rcp[0] == 'o'
			&&  rcp[1] == 'r'
			&& !(map[rcp[2]]&(DIGIT|LETTER))) {
				cp = rcp + 2;
				return FOR;
			}
			goto id;
		case 'g':
			if (rcp[0] == 'o'
			&&  rcp[1] == 't'
			&&  rcp[2] == 'o'
			&& !(map[rcp[3]]&(DIGIT|LETTER))) {
				cp = rcp + 3;
				return GOTO;
			}
			goto id;
		case 'l':
			if (rcp[0] == 'o'
			&&  rcp[1] == 'n'
			&&  rcp[2] == 'g'
			&& !(map[rcp[3]]&(DIGIT|LETTER))) {
				cp = rcp + 3;
				return LONG;
			}
			goto id;
		case 'r':
			if (rcp[0] == 'e'
			&&  rcp[1] == 'g'
			&&  rcp[2] == 'i'
			&&  rcp[3] == 's'
			&&  rcp[4] == 't'
			&&  rcp[5] == 'e'
			&&  rcp[6] == 'r'
			&& !(map[rcp[7]]&(DIGIT|LETTER))) {
				cp = rcp + 7;
				return REGISTER;
			}
			if (rcp[0] == 'e'
			&&  rcp[1] == 't'
			&&  rcp[2] == 'u'
			&&  rcp[3] == 'r'
			&&  rcp[4] == 'n'
			&& !(map[rcp[5]]&(DIGIT|LETTER))) {
				cp = rcp + 5;
				return RETURN;
			}
			goto id;
		case 's':
			if (rcp[0] == 'h'
			&&  rcp[1] == 'o'
			&&  rcp[2] == 'r'
			&&  rcp[3] == 't'
			&& !(map[rcp[4]]&(DIGIT|LETTER))) {
				cp = rcp + 4;
				return SHORT;
			}
			if (rcp[0] == 'i'
			&&  rcp[1] == 'g'
			&&  rcp[2] == 'n'
			&&  rcp[3] == 'e'
			&&  rcp[4] == 'd'
			&& !(map[rcp[5]]&(DIGIT|LETTER))) {
				cp = rcp + 5;
				return SIGNED;
			}
			if (rcp[0] == 'i'
			&&  rcp[1] == 'z'
			&&  rcp[2] == 'e'
			&&  rcp[3] == 'o'
			&&  rcp[4] == 'f'
			&& !(map[rcp[5]]&(DIGIT|LETTER))) {
				cp = rcp + 5;
				return SIZEOF;
			}
			if (rcp[0] == 't'
			&&  rcp[1] == 'a'
			&&  rcp[2] == 't'
			&&  rcp[3] == 'i'
			&&  rcp[4] == 'c'
			&& !(map[rcp[5]]&(DIGIT|LETTER))) {
				cp = rcp + 5;
				return STATIC;
			}
			if (rcp[0] == 't'
			&&  rcp[1] == 'r'
			&&  rcp[2] == 'u'
			&&  rcp[3] == 'c'
			&&  rcp[4] == 't'
			&& !(map[rcp[5]]&(DIGIT|LETTER))) {
				cp = rcp + 5;
				return STRUCT;
			}
			if (rcp[0] == 'w'
			&&  rcp[1] == 'i'
			&&  rcp[2] == 't'
			&&  rcp[3] == 'c'
			&&  rcp[4] == 'h'
			&& !(map[rcp[5]]&(DIGIT|LETTER))) {
				cp = rcp + 5;
				return SWITCH;
			}
			goto id;
		case 't':
			if (rcp[0] == 'y'
			&&  rcp[1] == 'p'
			&&  rcp[2] == 'e'
			&&  rcp[3] == 'd'
			&&  rcp[4] == 'e'
			&&  rcp[5] == 'f'
			&& !(map[rcp[6]]&(DIGIT|LETTER))) {
				cp = rcp + 6;
				return TYPEDEF;
			}
			goto id;
		case 'u':
			if (rcp[0] == 'n'
			&&  rcp[1] == 'i'
			&&  rcp[2] == 'o'
			&&  rcp[3] == 'n'
			&& !(map[rcp[4]]&(DIGIT|LETTER))) {
				cp = rcp + 4;
				return UNION;
			}
			if (rcp[0] == 'n'
			&&  rcp[1] == 's'
			&&  rcp[2] == 'i'
			&&  rcp[3] == 'g'
			&&  rcp[4] == 'n'
			&&  rcp[5] == 'e'
			&&  rcp[6] == 'd'
			&& !(map[rcp[7]]&(DIGIT|LETTER))) {
				cp = rcp + 7;
				return UNSIGNED;
			}
			goto id;
		case 'v':
			if (rcp[0] == 'o'
			&&  rcp[1] == 'i'
			&&  rcp[2] == 'd'
			&& !(map[rcp[3]]&(DIGIT|LETTER))) {
				cp = rcp + 3;
				return VOID;
			}
			if (rcp[0] == 'o'
			&&  rcp[1] == 'l'
			&&  rcp[2] == 'a'
			&&  rcp[3] == 't'
			&&  rcp[4] == 'i'
			&&  rcp[5] == 'l'
			&&  rcp[6] == 'e'
			&& !(map[rcp[7]]&(DIGIT|LETTER))) {
				cp = rcp + 7;
				return VOLATILE;
			}
			goto id;
		case 'w':
			if (rcp[0] == 'h'
			&&  rcp[1] == 'i'
			&&  rcp[2] == 'l'
			&&  rcp[3] == 'e'
			&& !(map[rcp[4]]&(DIGIT|LETTER))) {
				cp = rcp + 4;
				return WHILE;
			}
			goto id;
		case '_':
			if (rcp[0] == '_'
			&&  rcp[1] == 't'
			&&  rcp[2] == 'y'
			&&  rcp[3] == 'p'
			&&  rcp[4] == 'e'
			&&  rcp[5] == 'c'
			&&  rcp[6] == 'o'
			&&  rcp[7] == 'd'
			&&  rcp[8] == 'e'
			&& !(map[rcp[9]]&(DIGIT|LETTER))) {
				cp = rcp + 9;
				return TYPECODE;
			}
			if (rcp[0] == '_'
			&&  rcp[1] == 'f'
			&&  rcp[2] == 'i'
			&&  rcp[3] == 'r'
			&&  rcp[4] == 's'
			&&  rcp[5] == 't'
			&&  rcp[6] == 'a'
			&&  rcp[7] == 'r'
			&&  rcp[8] == 'g'
			&& !(map[rcp[9]]&(DIGIT|LETTER))) {
				cp = rcp + 9;
				return FIRSTARG;
			}
			goto id;
        case 'L':
			if (*rcp == '\'') {
                /* Not known */
				return ICON;
			} else if (*rcp == '"') {
                /* Not known */
				return SCON;
			} else goto id;
        case '.':
			if (rcp[0] == '.' && rcp[1] == '.') {
				cp += 2;
				return ELLIPSIS;
			}
			if ((map[*rcp]&DIGIT) == 0)
				return '.';
			cp = rcp - 1;
			token = (char *)cp;
			/* TODO: float */
			return FCON;
        case '<':
			if (*rcp == '=') return cp++, LEQ;
			if (*rcp == '<') return cp++, LSHIFT;
			return '<';
		case '>':
			if (*rcp == '=') return cp++, GEQ;
			if (*rcp == '>') return cp++, RSHIFT;
			return '>';
		case '-':
			if (*rcp == '>') return cp++, DEREF;
			if (*rcp == '-') return cp++, DECR;
			return '-';
		case '=': return *rcp == '=' ? cp++, EQL    : '=';
		case '!': return *rcp == '=' ? cp++, NEQ    : '!';
		case '|': return *rcp == '|' ? cp++, OROR   : '|';
		case '&': return *rcp == '&' ? cp++, ANDAND : '&';
		case '+': return *rcp == '+' ? cp++, INCR   : '+';
		case ';': case ',': case ':':
		case '*': case '~': case '%': case '^': case '?':
		case '[': case ']': case '{': case '}': case '(': case ')': 
			return rcp[-1];
        case '\n': case '\v': case '\r': case '\f':
			// if (cp == limit) {
			// 	tsym = NULL;
			// 	return EOI;
			// }
			continue;
        case '/': if (*rcp == '*') {
			  	int c = 0;
			  	for (rcp++; *rcp != '/' || c != '*'; )
			  		if (map[*rcp]&NEWLINE) {
			  			cp = rcp + 1;
			  			for (char* line = (char *)cp; *cp==' ' || *cp=='\t'; cp++);
			  			rcp = cp;
			  		} else
			  			c = *rcp++;
			  	rcp++;
			  	cp = rcp;
			  	continue;
			  }
			  return '/';
		default:
            if ((map[cp[-1]]&BLANK) == 0)
                dev_err("%c %d, unrecognized!", ch, ch);  
        }
	}
}

char *get_tokname(int tok)
{
    for (size_t i = 0; i < sizeof(lt) / sizeof(lt[0]); i++)
    {
        if(tok == lt[i].value)
            return lt[i].string;
    }
    return NULL;
}

int lexer(scc_state *s1)
{
    cp=s1->src_file_buf;
    for (size_t i = 0; i < 20; i++)
    {
        int tok = gettok();
        char *tok_name = get_tokname(tok);
        if (tok == ID) {
            dev_dbg("gettok: ID, string: %s", token);
        }
        else if (tok == ICON) {
            dev_dbg("gettok: ICON, num: %lu", num);
        }
        else {
            dev_dbg("gettok: %d, token: %s", tok, tok_name);
        }
    }
    
}