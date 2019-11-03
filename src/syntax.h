#ifndef WORDANALYZER_C_SYNTAX_H
#define WORDANALYZER_C_SYNTAX_H

#include "word.h"
#include <stdio.h>
#include "const.h"

enum SYMBLE symbleList[WORDSIZE];
int lines[WORDSIZE];
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

void varyExpln();

void varyDef();

void retFuncDef();

void unRetFuncDef();

void mutiSentDef();

void paraDef();

void mainDef();

enum Type expressDef();

enum Type itemDef();

enum Type factorDef();

void sentDef();

void assignSentDef();

void conditSentDef();

void conditDef();

void loopDef();

void stepDef();

void retFuncCallDef();

void unRetFuncCallDef();

void assignParaDef(char *fname);

void sentsDef();

void readSentDef();

void writeSentDef();

void retDef();

int panic(char *msg);

void assert(enum SYMBLE a, enum SYMBLE b);

void asserts(enum SYMBLE a, enum SYMBLE b1, enum SYMBLE b2);

void printWord();

void printSyntax(char *msg);

void nextLine(int p);

#endif //WORDANALYZER_C_SYNTAX_Hgi