#ifndef WORDANALYZER_C_GENERATE_H
#define WORDANALYZER_C_GENERATE_H

#include "mid.h"

/**
 * 函数调用栈示意图
 * 父函数：在第一个push之前(有参数)或者call之前(无参数)
 * 父函数：保存自己用到的所有局部变量（含参数,普通变量和数组变量)以及$ra
 * 父函数：push所有的实参
 *  ________ _______ _______
 * |  t6    |       |       |
 * |  t7    |       | 父函数 |
 * |  t8    |       |保存环境|
 * |  t9    |       |       |
 * |__t10___|_______|_______|
 * |  ra    |       |  $ra  |
 * |________|_______|_______|
 * |  var1  |       | 父函数 |
 * |  var2  |       |  PUSH |
 * |  var3  |       | 参数表 |
 * |__var4__|_______|_______|
 * 父函数：随后call函数
 * 子函数：第一步是从后往前读取参数到形参中,然后继续执行
 * 子函数：jr $ra
 * 父函数：从后往前依次恢复$ra和其他局部变量,然后继续执行
 */

int paraCount;//参数计数器
char strLabel[LABELSIZE][LABELLENGTH];//string的label，用于开辟空间
char strList[LABELSIZE][TOKENLENGTH];//string的值，用于开辟空间
int strCount;//string的计数器
char arrLabList[LABELSIZE][LABELLENGTH];//array的label，用于开辟空间
int arrSizeList[LABELSIZE];//array的大小，用于开辟空间
int hasEntry;//遇到Func即置1
int startPush;
struct Code *temP;

void genMips();

void genFunc(struct Func *p);

void genPush(struct Push *p);

void genCall(struct Call *p);

void genPara(struct Para *p, int index);

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

void saveEnv();

void revertEnv();

void __inSp();

void __outSp(char *msg);

#endif //WORDANALYZER_C_GENERATE_H
