#include "main.h"
#include "def.h"

char *prog;
char token[8];
char ch;
int syn,p,m=0,n,row,sum=0;
char *rwtab[6]={"begin","if","then","while","do","end"};
static char tok_two_chars[] = "<=\236>=\235!=\225&&\240||\241++\244--\242==\224<<\1>>\2+=\253-=\255*=\252/=\257=\245&=\246^=\336|=\374->\313..\250##\266";


void next_tok()
{
    /* 共分为三大块，分别是标示符、数字、符号，对应下面的 if   else if  和 else */ 
    for(n=0;n<8;n++) token[n]=NULL;
    ch=prog[p++];
    while(ch==' ')
    {
        ch=prog[p];
        p++;
    }
    switch (ch) {
    case 'a': case 'b': case 'c': case 'd':
    case 'e': case 'f': case 'g': case 'h':
    case 'i': case 'j': case 'k': case 'l':
    case 'm': case 'n': case 'o': case 'p':
    case 'q': case 'r': case 's': case 't':
    case 'u': case 'v': case 'w': case 'x':
    case 'y': case 'z': 
    case 'A': case 'B': case 'C': case 'D':
    case 'E': case 'F': case 'G': case 'H':
    case 'I': case 'J': case 'K': case 'L':
    case 'M': case 'N': case 'O': case 'P':
    case 'Q': case 'R': case 'S': case 'T':
    case 'U': case 'V': case 'W': case 'X':
    case 'Y': case 'Z':
    case '_':
        //可能是标示符或者变量名 
        m=0;
        while((ch>='0'&&ch<='9')||(ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z')||(ch=='_'))
        {
            token[m++]=ch;
            ch=prog[p++];
        }
        token[m++]='\0';
        p--;
        syn=10;
        //将识别出来的字符和已定义的标示符作比较
        for(n=0;n<6;n++){  
            if(!strcmp(token,rwtab[n]))
            {
                syn=n+1;
                break;
            }
        }
        break;
    case '0': case '1': case '2': case '3':
    case '4': case '5': case '6': case '7':
    case '8': case '9':
        {
            sum=0;
            while((ch>='0'&&ch<='9')) {
                sum=sum*10+ch-'0';
                ch=prog[p++];
            }
        }
        p--;
        syn=11;
        if(sum>32767)
            syn=-1;
        case'<':m=0;token[m++]=ch;
        ch=prog[p++];
        if(ch=='>')
        {
            syn=21;
            token[m++]=ch;
        }
        else if(ch=='=')
        {
            syn=22;
            token[m++]=ch;
        }
        else
        {
            syn=23;
            p--;
        }
        break;
    case'>':m=0;token[m++]=ch;
        ch=prog[p++];
        if(ch=='=')
        {
            syn=24;
            token[m++]=ch;
        }
        else
        {
            syn=20;
            p--;
        }
        break;
    case':':m=0;token[m++]=ch;
        ch=prog[p++];
        if(ch=='=')
        {
            syn=18;
            token[m++]=ch;
        }
        else
        {
            syn=17;
            p--;
        }
        break;
    case'*':syn=13;token[0]=ch;break;
    case'/':syn=14;token[0]=ch;break;
    case'+':syn=15;token[0]=ch;break;
    case'-':syn=16;token[0]=ch;break;
    case'=':syn=25;token[0]=ch;break;
    case';':syn=26;token[0]=ch;break;
    case'(':syn=27;token[0]=ch;break;
    case')':syn=28;token[0]=ch;break;
    case'#':syn=0;token[0]=ch;break;
    case'\n':syn=-2;break;
    default: syn=-1;break;
    }
}



int lexer(scc_state *s1)
{
    p=0;
    prog=s1->src_file_buf;
    do
    {
        next_tok();
        switch(syn)
        {
        case 11: printf("(%d, %d)\n", syn, sum); break;
        case -1: printf("error\n"); break;
        case -2: row=row++;break;
        default: printf("(%d, %s)\n", syn, token); break;
        }
    }
    while (syn!=0);
}