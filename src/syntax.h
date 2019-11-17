#ifndef WORDANALYZER_C_SYNTAX_H
#define WORDANALYZER_C_SYNTAX_H

#include "word.h"
#include <stdio.h>

enum SYMBLE symbleList[WORDSIZE];
int lines[WORDSIZE];
char tokenList[TOKENLENGTH][WORDSIZE];
int wp;
int retArr[2];

void programDef();

int charDef();

void stringDef();

void constExpln();

void constDef();

int intDef();

int unsgIntDef();

int idenDef();

void explnheadDef();

void varyExpln();

void varyDef();

void retFuncDef();

void unRetFuncDef();

void mutiSentDef();

void paraDef();

void mainDef();

int *expressDef();

int *itemDef();

int *factorDef();

void sentDef();

void assignSentDef();

void conditSentDef();

void conditDef();

void loopDef();

int stepDef();

int retFuncCallDef();

void unRetFuncCallDef();

void assignParaDef(char *fname);

void sentsDef();

void readSentDef();

void writeSentDef();

void retDef();

int panic(char *msg);

void assert(enum SYMBLE a, enum SYMBLE b);

void asserts(enum SYMBLE a, enum SYMBLE b1, enum SYMBLE b2);

void printSyntax(char *msg);

void nextLine(int p);

#endif //WORDANALYZER_C_SYNTAX_Hgi