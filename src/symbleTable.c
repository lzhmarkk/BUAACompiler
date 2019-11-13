#include "symbleTable.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "error.h"

void addToTable(char *name, enum Kind kind, enum Type type, int leve, int size, ...) {
    struct Table *new = (struct Table *) malloc(sizeof(struct Table));
    strcpy(new->name, name);
    new->kind = kind;
    new->type = type;
    new->level = leve;
    new->next = NULL;

    va_list vl;
    va_start(vl, size);
    if (kind == VAR && type == ARRAY && size == 3) {
        //如果新增的是一个数组
        struct ArrayTable *newAT = (struct ArrayTable *) malloc(sizeof(struct ArrayTable));
        newAT->type = va_arg(vl, int);
        newAT->size = __str2int(va_arg(vl, char*));
        newAT->addr = va_arg(vl, int);
        new->info = newAT;
    } else if (kind == FUNC && type == VOID && size == 2) {
        //如果是一个函数
        struct FuncTable *newFT = (struct FuncTable *) malloc(sizeof(struct FuncTable));
        newFT->paraSize = va_arg(vl, int);
        newFT->ret = va_arg(vl, int);
        new->info = newFT;
    } else {
        new->info = NULL;
    }

    //将new添加到table表中
    if (table == NULL) {
        table = new;
    } else {
        struct Table *p = table;
        while (p->next != NULL) {
            p = p->next;
        }
        p->next = new;
    }
}

int isRetFunc(char *func) {
    struct Table *p;
    for (p = table; p != NULL; p = p->next) {
        if (p->kind == FUNC && !strcmp(p->name, func)) {
            return ((struct FuncTable *) p->info)->ret != VOID;
        }
    }
    return 0;
}

int isUnRetFunc(char *func) {
    struct Table *p;
    for (p = table; p != NULL; p = p->next) {
        if (p->kind == FUNC && !strcmp(p->name, func)) {
            return ((struct FuncTable *) p->info)->ret == VOID;
        }
    }
    return 0;
}

int __str2int(char *addr) {
    char *str = addr;
    int res = 0;
    while (*str) {
        res = (*str - '0') + 10 * res;
        str++;
    }
    return res;
}

int checkRedef(char *name, int leve) {
    struct Table *p;
    //当前层重复定义
    for (p = table; p != NULL; p = p->next) {
        if (p->level == leve && !strcmp(p->name, name)) {
            return REDEFINED_LOC;
        }
    }
    return SUCCESS;
}

int checkExist(char *name, int leve) {
    struct Table *p;
    //在当前层查找
    for (p = table; p != NULL; p = p->next) {
        if (p->level == leve && !strcmp(p->name, name)) {
            return SUCCESS;
        }
    }
    //在全局查找
    for (p = table; p != NULL; p = p->next) {
        if (p->level == 0 && !strcmp(p->name, name)) {
            return SUCCESS;
        }
    }
    return NOTDEFINED;
}

/**
 * 检测参数类型匹配问题
 */
int checkParaType(char *fname, int index, enum Type type) {
    struct Table *p;
    for (p = table; p != NULL; p = p->next) {
        if (p->kind == FUNC && p->type == VOID && !strcmp(p->name, fname)) {
            //找到该函数p
            while (index--) {
                p = p->next;
            }
            //找到该参数p
            if (p->kind == PARA && p->type != type) {
                return PARATYPE_MISMATCH;
            } /*else if (p->kind != PARA) {
                //实参多于形参
                return PARASIZE_MISMATCH;
            }*/
            return SUCCESS;
        }
    }
    //找不到函数
    return NOTDEFINED;
}

/**
 * 检测参数数量类型问题
 */
int checkParaSize(char *fname, int paraIndex) {
    struct Table *f, *p;
    for (f = table; f != NULL; f = f->next) {
        if (f->kind == FUNC && f->type == VOID && !strcmp(f->name, fname)) {
            //找到该函数f
            int paraCount = 0;
            //遍历参数p
            for (p = f->next; p != NULL && p->kind == PARA; p = p->next) {
                paraCount++;
            }
            return paraCount == paraIndex ? SUCCESS : PARASIZE_MISMATCH;
        }
    }
    //找不到函数
    return NOTDEFINED;
}

/**
 * 取得类型
 */
enum Type getType(char *name, int leve) {
    struct Table *p;
    for (p = table; p != NULL; p = p->next) {
        if (!strcmp(p->name, name) && p->level == leve) {
            if (p->kind == FUNC && p->type == VOID) {
                return ((struct FuncTable *) p->info)->ret;
            } else if (p->kind == VAR && p->type == ARRAY) {
                return ((struct ArrayTable *) p->info)->type;
            } else {
                return p->type;
            }
        }
    }
    //局部找不到，去全局找
    for (p = table; p != NULL; p = p->next) {
        if (!strcmp(p->name, name) && p->level == 0) {
            if (p->kind == FUNC && p->type == VOID) {
                return ((struct FuncTable *) p->info)->ret;
            } else if (p->kind == VAR && p->type == ARRAY) {
                return ((struct ArrayTable *) p->info)->type;
            } else {
                return p->type;
            }
        }
    }
    return VOID;
}

/**
 * 查找是否是常量
 */
int isConst(char *name, int leve) {
    struct Table *p;
    for (p = table; p != NULL; p = p->next) {
        if (!strcmp(p->name, name) && p->level == leve) {
            return p->kind == CONST;
        }
    }
    for (p = table; p != NULL; p = p->next) {
        if (!strcmp(p->name, name) && p->level == 0) {
            return p->kind == CONST;
        }
    }
    return 0;
}