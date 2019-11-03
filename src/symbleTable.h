#ifndef WORDANALYZER_C_SYMBLETABLE_H
#define WORDANALYZER_C_SYMBLETABLE_H

#include "word.h"
#include "const.h"
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
    enum Type type;
    int size;
    int addr;
};
struct FuncTable {
    int paraSize;
    enum Type ret;
};
/**
 * 符号表的通用表项
 */
struct Table {
    char name[TOKENLENGTH];
    enum Kind kind;
    enum Type type;
    int level;
    void *info;//指向额外信息的指针
    struct Table *next;
};

void addToTable(char *name, enum Kind kind, enum Type type, int leve, int size, ...);

int isRetFunc(char *func);

int isUnRetFunc(char *func);

int checkRedef(char *name, int leve);

int checkExist(char *name, int leve);

int checkPara1(char *fname, int index, enum Type type);

int checkPara2(char *fname, int paraIndex);

enum Type getType(char *name, int leve);

int isConst(char *name,int leve);

int __str2int(char *str);

struct Table *table;

#endif //WORDANALYZER_C_SYMBLETABLE_H
