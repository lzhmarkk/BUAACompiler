#ifndef WORDANALYZER_C_WORD_H
#define WORDANALYZER_C_WORD_H

#include "const.h"

char buf[MAXLENGTH];
char token[TOKENLENGTH];
int i;

enum SYMBLE {
    UNDEFINED,
    IDENFR,
    INTCON,
    CHARCON,
    STRCON,
    CONSTTK,
    INTTK,
    CHARTK,
    VOIDTK,
    MAINTK,
    IFTK,
    ELSETK,
    DOTK,
    WHILETK,
    FORTK,
    SCANFTK,
    PRINTFTK,
    RETURNTK,
    PLUS,
    MINU,
    MULT,
    DIV,
    LSS,
    LEQ,
    GRE,
    GEQ,
    EQL,
    NEQ,
    ASSIGN,
    SEMICN,
    COMMA,
    LPARENT,
    RPARENT,
    LBRACK,
    RBRACK,
    LBRACE,
    RBRACE
} symble;

int isSpace(char ch);

int isDoubQuo(char ch);

int isSiglQuo(char ch);

int isLetter(char ch);

int isDigit(char ch);

int isComma(char ch);

int isSemi(char ch);

int isEqu(char ch);

int isPlus(char ch);

int isMinus(char ch);

int isDivi(char ch);

int isMult(char ch);

int isLpar(char ch);

int isRpar(char ch);

int isLbrack(char ch);

int isRbrack(char ch);

int isLbrace(char ch);

int isRbrace(char ch);

int isLss(char ch);

int isGre(char ch);

int isExcla(char ch);

int isReserved(char *ch);

void clearToken();

int getSymble(char *str);

void catToken(char c);

char *getReserved(int s);

#endif