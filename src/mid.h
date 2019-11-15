//
// Created by lzhmark on 2019/11/14.
//

#ifndef WORDANALYZER_C_MID_H
#define WORDANALYZER_C_MID_H

#include "symbleTable.h"

int Register;
struct Code *code;

int loopCount;
char labelBase[100][9];

struct Code *labelP1;
struct Code *labelP2;
struct Code *branchP;
struct Code *ifP;

enum Op {
    PlusOp,
    MinuOp,
    MultOp,
    DiviOp
};
enum CodeType {
    Push,
    Call,
    Ret,
    ReadRet,
    Var,
    Const,
    Tuple,
    Eql,
    Assig,
    Goto,
    Bra,
    Label,
    ArrL,
    ArrS,
    Read,
    Write
};
enum BranchType {
    BEQ,
    BNE,
    BGE,
    BGT,
    BLE,
    BLT
};
struct Push {
    int reg;
};
struct Call {
    struct Label *label;
};
struct ReadRet {
    int reg;
};
struct Ret {
    int reg;
};
struct Var {
    enum Type type;
    char *name;
    int size;
};
struct Const {
    enum Type type;
    char *name;
    int value;
};
struct Tuple {
    enum Op op;
    int regA;
    int regB;
    int regC;
};
struct Assig {
    int from;
    int to;
};
struct Eql {
    int value;
    int to;
};
struct Goto {
    struct Label *label;
};
struct Bra {
    int reg;
    enum BranchType type;
    struct Label *label;
};
struct Label {
    char *name;
};
struct ArrL {
    char *name;
    int offset;
    int to;
};
struct ArrS {
    char *name;
    int offset;
    int from;
};
struct Read {
    int reg;
};
struct Write {
    int isReg;
    char *string;
    int reg;
};

struct Code {
    enum CodeType type;
    void *info;
    struct Code *prev;
    struct Code *next;
};

struct Code *emit(enum CodeType t, int size, ...);

int newRegister();

int getReg(char *name, int leve);

char *genLabel();

struct Code *getLabel(char *name);

enum BranchType reverse(enum BranchType type);

void printCode();

#endif //WORDANALYZER_C_MID_H