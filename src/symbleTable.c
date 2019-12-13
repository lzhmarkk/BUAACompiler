#include "symbleTable.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "error.h"
#include "reg.h"

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
        //数组分配寄存器，用于存头指针
        if (leve == 0) {
            new->reg = alloRegGlo();
        } else {
            new->reg = alloReg();
        }
        new->info = newAT;
    } else if (kind == FUNC && type == VOID && size == 2) {
        //如果是一个函数
        struct FuncTable *newFT = (struct FuncTable *) malloc(sizeof(struct FuncTable));
        newFT->paraSize = va_arg(vl, int);
        newFT->ret = va_arg(vl, int);
        new->reg = 0;//函数不分配寄存器
        new->info = newFT;
    } else if (kind == CONST && size == 1) {
        //如果是一个常量
        struct ConstTable *newCT = (struct ConstTable *) malloc(sizeof(struct ConstTable));
        newCT->value = va_arg(vl, int);
        new->reg = 0;//常量采用传播，不使用寄存器
        new->info = newCT;
    } else {
        if (leve == 0) {
            //说明是全局变量
            new->reg = alloRegGlo();
        } else {
            //局部变量
            new->reg = alloReg();
        }
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


int getConstValue(char *name, int leve) {
    struct Table *p;
    for (p = table; p != NULL; p = p->next) {
        if (!strcmp(p->name, name) && p->level == leve) {
            if (p->kind == CONST) {
                return ((struct ConstTable *) p->info)->value;
            } else panic("should be a const");
        }
    }
    for (p = table; p != NULL; p = p->next) {
        if (!strcmp(p->name, name) && p->level == 0) {
            if (p->kind == CONST) {
                return ((struct ConstTable *) p->info)->value;
            } else panic("should be a const");
        }
    }
    return 0;
}

void printTable() {
    printf("      Name  Kind  Type  Lev  Reg\n");
    struct Table *p;
    for (p = table; p != NULL; p = p->next) {
        char *kind = NULL, *type = NULL;
        switch (p->kind) {
            case CONST:
                kind = "CONST";
                break;
            case VAR:
                kind = "VAR";
                break;
            case FUNC:
                kind = "FUNC";
                break;
            case PARA:
                kind = "PARA";
                break;
        }
        switch (p->type) {
            case CHAR:
                type = "CHAR";
                break;
            case INT:
                type = "INT";
                break;
            case ARRAY:
                type = "ARRAY";
                break;
            case VOID:
                type = "VOID";
                break;
        }
        if (p->kind == FUNC && p->type == VOID) {
            enum Type ret = ((struct FuncTable *) p->info)->ret;
            printf("%10s %5s %5s   %2d      --return(%s)\n", p->name, kind, type, p->level,
                   (ret == INT ? "INT" : ret == CHAR ? "CHAR" : "VOID"));
        } else if (p->kind == VAR && p->type == ARRAY) {
            struct ArrayTable *t = (struct ArrayTable *) p->info;
            if (p->reg >= GLO) {
                printf("%10s %5s %5s   %2d  $t%d(Glo) --(%s)\n", p->name, kind, type, p->level, p->reg,
                       t->type == INT ? "INT" : "CHAR");
            } else {
                printf("%10s %5s %5s   %2d  $t%d --(%s)\n", p->name, kind, type, p->level, p->reg,
                       t->type == INT ? "INT" : "CHAR");
            }
        } else if (p->kind == CONST) {
            printf("%10s %5s %5s   %2d      --%d\n", p->name, kind, type, p->level,
                   ((struct ConstTable *) p->info)->value);
        } else {
            if (p->reg < 0) {
                printf("%10s %5s %5s   %2d  $t%d\n", p->name, kind, type, p->level, p->reg);
            } else if (p->reg >= GLO) {
                printf("%10s %5s %5s   %2d  $t%d(Glo)\n", p->name, kind, type, p->level, p->reg);
            } else {
                printf("%10s %5s %5s   %2d  $t%d\n", p->name, kind, type, p->level, p->reg);
            }
        }
    }
}

int getCommonReg(char *func1, char *func2) {
    int func1Size = 0, func2Size = 0;
    struct Table *p;
    for (p = table; p != NULL; p = p->next) {
        if (p->kind == FUNC && p->type == VOID && !strcmp(func1, p->name)) {
            //找到函数1
            p = p->next;
            while (p != NULL && p->kind != FUNC) {
                if (p->kind == PARA || p->kind == VAR) {
                    func1Size++;
                }
                p = p->next;
            }
            break;
        }
    }
    for (p = table; p != NULL; p = p->next) {
        if (p->kind == FUNC && p->type == VOID && !strcmp(func2, p->name)) {
            //找到函数2
            p = p->next;
            while (p != NULL && p->kind != FUNC) {
                if (p->kind == PARA || p->kind == VAR) {
                    func2Size++;
                }
                p = p->next;
            }
            break;
        }
    }
    return func1Size < func2Size ? func1Size : func2Size;
}