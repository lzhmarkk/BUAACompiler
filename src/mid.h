#ifndef WORDANALYZER_C_MID_H
#define WORDANALYZER_C_MID_H

#include "symbleTable.h"

int Register;
int saveReg;
struct Code *code;

int loopCount;
char labelBase[LABELSIZE][LABELLENGTH];//存储中间代码中所有Label

struct Code *branchP;

enum Op {
    PlusOp,
    MinuOp,
    MultOp,
    DiviOp
};
enum CodeType {
    Func,
    Push,
    Call,
    Para,
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
struct Func {
    char *name;
    int paraSize;
};
struct Push {
    int reg;
};
struct Call {
    struct Func *func;
};
struct Para {
    int reg;
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
    int reg;
    int size;
};
struct Const {
    enum Type type;
    char *name;
    int reg;
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
    int reg;
    int offset;
    int to;
};
struct ArrS {
    char *name;
    int reg;
    int offset;
    int from;
};
struct Read {
    int reg;
    enum Type type;
};
struct Write {
    char *string;//如果为NULL，说明只有寄存器
    int reg;//如果为-1,说明只有字符串
    enum Type type;
};

struct Code {
    enum CodeType type;
    void *info;
    struct Code *prev;
    struct Code *next;
};

struct Code *emit(enum CodeType t, int size, ...);

int newRegister();

void saveRegister();

void revertRegister();

int getReg(char *name, int leve);

char *genLabel();

struct Code *getLabel(char *name);

enum BranchType reverse(enum BranchType type);

void printCode();

#endif //WORDANALYZER_C_MID_H