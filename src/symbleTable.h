#ifndef WORDANALYZER_C_SYMBLETABLE_H
#define WORDANALYZER_C_SYMBLETABLE_H

#include "word.h"
#include "syntax.h"

int level;

/**
 * 标识符Kind和Type属性
 */
enum Kind {
    CONST,//常量
    VAR,//变量
    FUNC,//函数
    PARA//参数
};
enum Type {
    CHAR,//char
    INT,//int
    ARRAY,//数组
    VOID//空
};
/**
 * array和func的额外信息
 */
struct ArrayTable {
    enum Type type;//数组元素类型
    int size;//数组大小
};
struct FuncTable {
    int paraSize;//参数个数
    enum Type ret;//返回值类型
};
struct ConstTable {
    int value;//常量的值
};
/**
 * 符号表的通用表项
 */
struct Table {
    char name[TOKENLENGTH];//名
    enum Kind kind;
    enum Type type;
    int level;//层级
    int reg;//已分配寄存器
    void *info;//指向额外信息的指针
    struct Table *next;
};

void addToTable(char *name, enum Kind kind, enum Type type, int leve, int size, ...);

int isRetFunc(char *func);

int isUnRetFunc(char *func);

int checkRedef(char *name, int leve);

int checkExist(char *name, int leve);

int checkParaType(char *fname, int index, enum Type type);

int checkParaSize(char *fname, int paraIndex);

enum Type getType(char *name, int leve);

int isConst(char *name, int leve);

int getConstValue(char *name, int leve);

void printTable();

int __str2int(char *str);

int getCommonReg(char *func1, char *func2);

struct Table *table;

#endif //WORDANALYZER_C_SYMBLETABLE_H
