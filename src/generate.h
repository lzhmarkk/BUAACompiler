#ifndef WORDANALYZER_C_GENERATE_H
#define WORDANALYZER_C_GENERATE_H

#include "mid.h"

/**
 * 运行栈示意图
 * 1. $fp <- $sp，\Delta <- 0
 * 2. 分析Para和Var，\Delta+1；遇到数组，额外存到arrList里
 * 3. 对arrList，*reg <- $sp-4*\Delta，\Delta+size
 * 3. $sp -= 4*\Delta，运行栈申请完毕
 * 注：在jr $ra前，需要做 $sp <- $fp
 *
 * 调用栈示意图
 * 父函数：在函数调用的参数之前保存环境：已经Push的参数，本身的参数、普通变量和数组指针 以及$fp,$ra
 * 父函数：Push不够的实参
 * f(1,2,3,4,5,6)的栈
 *  ________ _______ _______ <-fp   ___
 * |  var1  |       |       |        ^
 * |  var2  |       |局部变量|        |
 * |__var3__|_______|_______|        | \Delta
 * |  [0]   |       |局部数组|        |
 * |__[1]___|_______|_______|       _v_
 * |  $a0:? |       |       |
 * |  $a3:? |       | 父函数 |
 * |  $t0   |       |保存环境|
 * |  $t1   |       |       |
 * |__$t2___|_______|_______|
 * |  $fp   |       |  $fp  |
 * |__$ra___|_______|__$ra__|
 * |   5    |       |       |
 * |   6    |       |  参数  |//1,2,3,4在a寄存器里
 * |________|_______|_______|<-sp
 * 父函数：随后call函数
 * 子函数：第一步是从后往前读取参数到形参中,然后继续执行
 * 子函数：申请运行栈，执行，然后jr $ra
 * 父函数：从后往前依次恢复$ra和其他局部变量,然后继续执行
 */

char strLabel[LABELSIZE][LABELLENGTH];//string的label，用于开辟空间
char strList[LABELSIZE][TOKENLENGTH];//string的值，用于开辟空间
int strCount;//string的计数器
int hasEntry;//遇到Func即置1

struct {
    int size;//大小
    int reg;//头指针的寄存器位置
} arrList[ARRSIZE];//一个函数内，所有要开空间的数组的数据
int arrListSize;

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

void allocateSpace(struct AlloSpa *as);

void printMips(const char *msg, ...);

void end();

void initData();

char *getStrLab(char *str);

void saveEnv(int isRecursion, int regListSize);

void revertEnv(int isRecursion, int regListSize);

#endif //WORDANALYZER_C_GENERATE_H
