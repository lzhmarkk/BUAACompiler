#ifndef WORDANALYZER_C_REG_H
#define WORDANALYZER_C_REG_H

//给中间变量s寄存器个数
#define MIDREG 5
#define GLO 1000


#define $zero 0
#define $at 1
#define $v0 2
#define $v1 3
#define $a0 4
#define $a1 5
#define $a2 6
#define $a3 7
#define $t0 8
#define $t1 9
#define $t2 10
#define $t3 11
#define $t4 12
#define $t5 13
#define $t6 14
#define $t7 15
#define $s0 16
#define $s1 17
#define $s2 18
#define $s3 19
#define $s4 20
#define $s5 21
#define $s6 22
#define $s7 23
#define $t8 24
#define $t9 25

/**
 * 寄存器使用指南
 * $t0-$t9($8-$25)
 * $8 - $8+MIDREG 中间变量寄存器 {0,1,2,3,4}
 * $8+MIDREG+1 - $25 局部变量寄存器 {5,6,...,16,17}
 * $-1 - ... 额外局部变量寄存器，存在内存Reg区域中，前加-以示内存 {-1,-2,-3,-4...}
 * $1+GLO - ... 全局变量寄存器，存在内存Glo区域中，前加OFF以示全局 {1001,1002,1003...}
 */

int reg;
int regMem;
int regMemMax;

int gloRegMem;
int gloRegMemMax;

int regTmpBusy[MIDREG];

int alloReg();

void clearReg();

int alloRegGlo();

int isRegTmp(int r);

int isRegGlo(int r);

int isRegMem(int r);

int alloRegTmp();

void clearRegTmp(int r);

#endif //WORDANALYZER_C_REG_H
