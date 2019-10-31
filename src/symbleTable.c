#include "symbleTable.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

void addToTable(char *name, enum Kind kind, enum Type type, int level, int size, ...) {
    struct Table *new = (struct Table *) malloc(sizeof(struct Table));
    strcpy(new->name, name);
    new->kind = kind;
    new->type = type;
    new->level = level;
    new->next = NULL;

    va_list vl;
    va_start(vl, size);
    if (kind == VAR && type == ARRAY && size == 3) {
        //如果新增的是一个数组
        struct ArrayTable *newAT = (struct ArrayTable *) malloc(sizeof(struct ArrayTable));
        newAT->type = va_arg(vl, int);
        newAT->size = str2int(va_arg(vl, char*));
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
    return !isRetFunc(func);
}

int str2int(char *addr) {
    char *str = addr;
    int res = 0;
    while (*str) {
        res = (*str - '0') + 10 * res;
        str++;
    }
    return res;
}