#ifndef WORDANALYZER_C_SYNTAX_H
#define WORDANALYZER_C_SYNTAX_H

#include "word.h"
#include <stdio.h>

enum SYMBLE symbleList[WORDSIZE];
int lines[WORDSIZE];
char tokenList[TOKENLENGTH][WORDSIZE];
int wp;
//retArr[0]表示值的类型为int/char；retArr[1]表示返回值；retArr[2]表示返回值是一个寄存器标号，整形或字符
int retArr[3];

enum factorKind {
    facInt,//表示返回值为一个整数
    facChar,//表示返回值为一个字符
    facReg//表示返回值为一个寄存器
};

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