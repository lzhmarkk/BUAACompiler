#ifndef WORDANALYZER_C_SYNTAX_H
#define WORDANALYZER_C_SYNTAX_H

#include "word.h"
#include <stdio.h>
#include "const.h"

enum SYMBLE symbleList[WORDSIZE];
char tokenList[TOKENLENGTH][WORDSIZE];
int wp;

void programDef();

void charDef();

void stringDef();

void constExpln();

void constDef();

void intDef();

void unsgIntDef();

void idenDef();

void explnheadDef();

void varyExplnDef();

void varyDef();

void retFuncDef();

void unRetFuncDef();

void mutiSentDef();

void paraDef();

void mainDef();

void expressDef();

void itemDef();

void factorDef();

void sentDef();

void assignSentDef();

void conditSentDef();

void conditDef();

void loopDef();

void stepDef();

void retFuncCallDef();

void unRetFuncCallDef();

void assignParaDef();

void sentsDef();

void readSentDef();

void writeSentDef();

void retDef();

int error();

void assert(enum SYMBLE a, enum SYMBLE b);

void asserts(enum SYMBLE a, enum SYMBLE b1, enum SYMBLE b2);

int isRetFunc(char *func);

int isUnRetFunc(char *func);

#endif //WORDANALYZER_C_SYNTAX_Hgi