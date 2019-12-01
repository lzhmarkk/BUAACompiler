#ifndef WORDANALYZER_C_MID_H
#define WORDANALYZER_C_MID_H

#include "symbleTable.h"

int Register;//空出t0和t1作为中间结果寄存器
int maxRegister;
int saveReg;
struct Code *code;

int loopCount;
char labelBase[LABELSIZE][LABELLENGTH];//存储中间代码中所有Label


enum Op {
    PlusOp,
    MinuOp,
    MultOp,
    DiviOp
};
enum CodeType {
    Func,//函数头
    Push,//参数入栈(传入)
    Call,//调用函数
    Para,//参数出栈(读取)
    Ret,//返回
    ReadRet,//读取函数返回值
    Var,//变量
    Const,//常量
    Tuple,//四元式
    Assig,//赋值
    Goto,//无条件跳转
    Bra,//有条件跳转
    Label,//标签
    ArrL,//读数组
    ArrS,//写数组
    Read,//读变量
    Write,//写字符串或变量
    SavEnv,//保存环境
    RevEnv//回复环境
};
enum BranchType {
    BEQ,
    BNE,
    BGE,
    BGT,
    BLE,
    BLT
};
enum WriteType {
    STR_ONLY,
    REG_ONLY,
    VALUE_ONLY,
    STR_REG,
    STR_VALUE
};
struct Func {
    char *name;//函数名
    int paraSize;//参数个数
};
struct Push {
    int value;//值
    enum factorKind kind;//值的类型(int,char,reg)
};
struct Call {
    struct Func *func;
};
struct Para {
    int reg;//对应寄存器
};
struct ReadRet {
    int reg;//对应寄存器
};
struct Ret {
    int value;//值
    enum factorKind kind;//值的类型(int,char,reg)
};
struct Var {
    //如果是数组,name为标签,reg=-1,size为数组大小
    enum Type type;
    char *name;//名
    int reg;//对应寄存器
    int size;//数组非0,非数组为0
};
struct Const {
    enum Type type;
    char *name;//名
    int reg;//对应寄存器//todo 常量不用寄存器
    int value;//值
};
struct Tuple {
    enum Op op;//操作符
    int valueA;
    enum factorKind factorKindA;//操作数A的类型(int,char,reg)
    int valueB;
    enum factorKind factorKindB;//操作数B的类型(int,char,reg)
    int regC;//返回的一定是寄存器//todo 若A!=reg,B!=reg，直接计算
};
struct Assig {
    int fromValue;//源的值
    enum factorKind fromKind;//源的类型(int,char,reg)
    int to;//目的寄存器
};
struct Goto {
    struct Label *label;
};
struct Bra {
    int reg;//判断值的寄存器
    enum BranchType type;
    struct Label *label;
};
struct Label {
    char *name;
};
struct ArrL {
    char *label;//数组标志
    int offsetValue;//偏移值
    enum factorKind offKind;//偏移值的类型(int,char,reg)
    int to;//目的寄存器
};
struct ArrS {
    char *label;//数组标志
    int offsetValue;//偏移值
    enum factorKind offKind;//偏移值的类型(int,char,reg)
    int fromValue;//源的值
    enum factorKind fromKind;//源的类型(int,char,reg)
};
struct Read {
    int reg;//被读入的寄存器
    enum Type type;//读入的类型(int,char)
};
struct Write {
    enum WriteType writeType;
    char *string;
    int value;//值
    enum Type type;//值的类型(int,char)
};
struct SavEnv {
    int isRecursion;//是否为递归前的环境保存;如果是,则要保存局部数组
};
struct RevEnv {
    int isRecursion;//是否为递归后的环境恢复
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