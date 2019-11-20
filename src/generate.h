#ifndef WORDANALYZER_C_GENERATE_H
#define WORDANALYZER_C_GENERATE_H

#include "mid.h"

int paraCount;//参数计数器
char strLabel[LABELSIZE][LABELLENGTH];//string的label，用于开辟空间
char strList[LABELSIZE][TOKENLENGTH];//string的值，用于开辟空间
int strCount;//string的计数器
char arrLabList[LABELSIZE][LABELLENGTH];//array的label，用于开辟空间
int arrSizeList[LABELSIZE];//array的大小，用于开辟空间
int hasEntry;//遇到Func即置1

void genMips();

void genFunc(struct Func *p);

void genPush(struct Push *p);

void genCall(struct Call *p);

void genPara(struct Para *p);

void genReadRet(struct ReadRet *p);

void genRet(struct Ret *p);

void genVar(struct Var *p);

void genConst(struct Const *p);

void genTuple(struct Tuple *p);

void genAssig(struct Assig *p);

void genGoto(struct Goto *p);

void genBra(struct Bra *p);

void genLab(struct Label *p);

void genArrL(struct ArrL *p);

void genArrS(struct ArrS *p);

void genRead(struct Read *p);

void genWrite(struct Write *p);

void printMips(const char *msg, ...);

void end();

void initData();

char *getStrLab(char *str);

#endif //WORDANALYZER_C_GENERATE_H
