#include "mid.h"
#include "string.h"
#include "stdarg.h"
#include "stdlib.h"
#include "reg.h"

struct Code *emit(enum CodeType t, int size, ...) {
    struct Code *new = (struct Code *) malloc(sizeof(struct Code));
    new->type = t;
    new->prev = NULL;
    new->next = NULL;

    va_list vl;
    va_start(vl, size);
    if (t == Func && size == 1) {
        struct Func *p = (struct Func *) malloc(sizeof(struct Func));
        p->name = va_arg(vl, char*);
        p->paraSize = 0;
        new->info = p;
    } else if (t == Push && size == 2) {
        struct Push *p = (struct Push *) malloc(sizeof(struct Push));
        p->value = va_arg(vl, int);
        p->kind = va_arg(vl, enum factorKind);
        new->info = p;
    } else if (t == Call && size == 1) {
        struct Call *p = (struct Call *) malloc(sizeof(struct Call));
        p->func = va_arg(vl, struct Func*);
        new->info = p;
    } else if (t == Para && size == 1) {
        struct Para *p = (struct Para *) malloc(sizeof(struct Para));
        p->reg = va_arg(vl, int);
        new->info = p;
    } else if (t == ReadRet && size == 1) {
        struct ReadRet *p = (struct ReadRet *) malloc(sizeof(struct ReadRet));
        p->reg = va_arg(vl, int);
        new->info = p;
    } else if (t == Ret && size == 2) {
        struct Ret *p = (struct Ret *) malloc(sizeof(struct Ret));
        p->value = va_arg(vl, int);
        p->kind = va_arg(vl, enum factorKind);
        new->info = p;
    } else if (t == Var && size == 4) {
        struct Var *p = (struct Var *) malloc(sizeof(struct Var));
        p->type = va_arg(vl, enum Type);
        p->name = va_arg(vl, char*);
        p->reg = va_arg(vl, int);
        p->size = va_arg(vl, int);
        new->info = p;
    } else if (t == Const && size == 4) {
        struct Const *p = (struct Const *) malloc(sizeof(struct Const));
        p->type = va_arg(vl, enum Type);
        p->name = va_arg(vl, char*);
        p->reg = va_arg(vl, int);
        p->value = va_arg(vl, int);
        new->info = p;
    } else if (t == Tuple && size == 6) {
        struct Tuple *p = (struct Tuple *) malloc(sizeof(struct Tuple));
        p->op = va_arg(vl, enum Op);
        p->valueA = va_arg(vl, int);
        p->factorKindA = va_arg(vl, enum factorKind);
        p->valueB = va_arg(vl, int);
        p->factorKindB = va_arg(vl, enum factorKind);
        p->regC = va_arg(vl, int);
        new->info = p;
    } else if (t == Assig && size == 3) {
        struct Assig *p = (struct Assig *) malloc(sizeof(struct Assig));
        p->fromValue = va_arg(vl, int);
        p->fromKind = va_arg(vl, enum factorKind);
        p->to = va_arg(vl, int);
        new->info = p;
    } else if (t == Goto && size == 1) {
        struct Goto *p = (struct Goto *) malloc(sizeof(struct Goto));
        p->label = va_arg(vl, struct Label*);
        new->info = p;
    } else if (t == Bra && size == 3) {
        struct Bra *p = (struct Bra *) malloc(sizeof(struct Bra));
        p->reg = va_arg(vl, int);
        p->type = va_arg(vl, enum BranchType);
        p->label = va_arg(vl, struct Label*);
        new->info = p;
    } else if (t == Label && size == 1) {
        struct Label *p = (struct Label *) malloc(sizeof(struct Label));
        p->name = va_arg(vl, char*);
        new->info = p;
    } else if (t == ArrL && size == 4) {
        struct ArrL *p = (struct ArrL *) malloc(sizeof(struct ArrL));
        p->label = va_arg(vl, char*);
        p->offsetValue = va_arg(vl, int);
        p->offKind = va_arg(vl, enum factorKind);
        p->to = va_arg(vl, int);
        new->info = p;
    } else if (t == ArrS && size == 5) {
        struct ArrS *p = (struct ArrS *) malloc(sizeof(struct ArrS));
        p->label = va_arg(vl, char*);
        p->offsetValue = va_arg(vl, int);
        p->offKind = va_arg(vl, enum factorKind);
        p->fromValue = va_arg(vl, int);
        p->fromKind = va_arg(vl, enum factorKind);
        new->info = p;
    } else if (t == Read && size == 2) {
        struct Read *p = (struct Read *) malloc(sizeof(struct Read));
        p->reg = va_arg(vl, int);
        p->type = va_arg(vl, enum Type);
        new->info = p;
    } else if (t == Write && size == 4) {
        struct Write *p = (struct Write *) malloc(sizeof(struct Write));
        p->writeType = va_arg(vl, enum WriteType);
        p->string = va_arg(vl, char*);
        p->value = va_arg(vl, int);
        p->type = va_arg(vl, enum Type);
        new->info = p;
    } else if (t == SavEnv && size == 2) {
        struct SavEnv *s = (struct SavEnv *) malloc(sizeof(struct SavEnv));
        char *curFunc = va_arg(vl, char*);
        char *name = va_arg(vl, char*);
        s->isRecursion = curFunc ? !strcmp(name, curFunc) : 0;
        s->regListSize = getCommonReg(curFunc, name);
        new->info = s;
    } else if (t == RevEnv && size == 2) {
        struct RevEnv *r = (struct RevEnv *) malloc(sizeof(struct RevEnv));
        char *curFunc = va_arg(vl, char*);
        char *name = va_arg(vl, char*);
        r->isRecursion = curFunc ? !strcmp(name, curFunc) : 0;
        r->regListSize = getCommonReg(curFunc, name);
        new->info = r;
    }

    if (code == NULL) {
        code = new;
    } else {
        struct Code *p = code;
        while (p->next != NULL)
            p = p->next;
        p->next = new;
        new->prev = p;
    }
    return new;
}

/**
 * 查找符号表，找到变量分配的虚拟寄存器
 */
int getReg(char *name, int leve) {
    struct Table *p;
    for (p = table; p != NULL; p = p->next) {
        if (!strcmp(name, p->name) && p->level == leve) {
            return p->reg;
        }
    }

    for (p = table; p != NULL; p = p->next) {
        if (!strcmp(name, p->name) && p->level == 0) {
            return p->reg;
        }
    }
    return -1;
}

/**
 * 获取函数的入口
 */
struct Code *getLabel(char *name) {
    struct Code *p;
    for (p = code; p != NULL; p = p->next) {
        if (p->type == Func) {
            struct Func *l = p->info;
            if (!strcmp(l->name, name)) {
                return p;
            }
        }
    }
    return NULL;
}

char *genLabel() {
    labelBase[loopCount][0] = 'L';
    labelBase[loopCount][1] = 'a';
    labelBase[loopCount][2] = 'b';
    labelBase[loopCount][3] = 'e';
    labelBase[loopCount][4] = 'l';
    labelBase[loopCount][5] = (char) (loopCount / 100 + '0');
    labelBase[loopCount][6] = (char) ((loopCount % 100) / 10 + '0');
    labelBase[loopCount][7] = (char) (loopCount % 10 + '0');
    labelBase[loopCount][8] = '\0';
    return labelBase[loopCount++];
}

enum BranchType reverse(enum BranchType type) {
    switch (type) {
        case BEQ:
            return BNE;
        case BNE:
            return BEQ;
        case BGE:
            return BLT;
        case BGT:
            return BLE;
        case BLE:
            return BGT;
        case BLT:
            return BGE;
        default:
            return -1;
    }
}

void printCode() {
    struct Code *p;
    for (p = code; p != NULL; p = p->next) {
        switch (p->type) {
            case Func:
                printf("%s():\n", ((struct Func *) p->info)->name);
                break;
            case Push: {
                struct Push *push = p->info;
                if (push->kind == facInt) {
                    printf("Push %d\n", push->value);
                } else if (push->kind == facChar) {
                    printf("Push \'%c\'\n", push->value);
                } else {
                    printf("Push $t%d\n", push->value);
                }
                break;
            }
            case Call:
                printf("Call %s\n", ((struct Call *) p->info)->func->name);
                break;
            case Para:
                printf("Para $t%d\n", ((struct Para *) p->info)->reg);
                break;
            case Ret: {
                struct Ret *ret = p->info;
                if (ret->kind == facInt) {
                    printf("Return %d\n", ret->value);
                } else if (ret->kind == facChar) {
                    printf("Return \'%c\'\n", ret->value);
                } else {
                    printf("Return $t%d\n", ret->value);
                }
                break;
            }
            case ReadRet:
                printf("getReturn $t%d\n", ((struct ReadRet *) p->info)->reg);
                break;
            case Var: {
                struct Var *v = p->info;
                printf("Var %s $t%d %d\n", v->name, v->reg, v->size);
                break;
            }
            case Const: {
                struct Const *c = p->info;
                printf("Const %s $t%d = %d\n", c->name, c->reg, c->value);
                break;
            }
            case Tuple: {
                struct Tuple *t = p->info;
                char Op;
                switch (t->op) {
                    case PlusOp:
                        Op = '+';
                        break;
                    case MinuOp:
                        Op = '-';
                        break;
                    case MultOp:
                        Op = '*';
                        break;
                    case DiviOp:
                        Op = '/';
                        break;
                    default:
                        Op = '?';
                        break;
                }
                switch (t->factorKindA) {
                    case facInt:
                        printf("$t%d = %d", t->regC, t->valueA);
                        break;
                    case facChar:
                        printf("$t%d = \'%c\'", t->regC, t->valueA);
                        break;
                    case facReg:
                        printf("$t%d = $t%d", t->regC, t->valueA);
                        break;
                }
                printf(" %c ", Op);
                switch (t->factorKindB) {
                    case facInt:
                        printf("%d\n", t->valueB);
                        break;
                    case facChar:
                        printf("\'%c\'\n", t->valueB);
                        break;
                    case facReg:
                        printf("$t%d\n", t->valueB);
                        break;
                }
                break;
            }
            case Assig: {
                struct Assig *a = p->info;
                if (a->fromKind == facInt) {
                    printf("$t%d = %d\n", a->to, a->fromValue);
                } else if (a->fromKind == facChar) {
                    printf("$t%d = \'%c\'\n", a->to, a->fromValue);
                } else {
                    printf("$t%d = $t%d\n", a->to, a->fromValue);
                }
                break;
            }
            case Goto:
                printf("Goto %s\n", ((struct Goto *) (p->info))->label->name);
                break;
            case Bra: {
                struct Bra *b = p->info;
                char *Op = NULL;
                switch (b->type) {
                    case BEQ:
                        Op = "==";
                        break;
                    case BNE:
                        Op = "!=";
                        break;
                    case BGE:
                        Op = ">=";
                        break;
                    case BGT:
                        Op = ">";
                        break;
                    case BLE:
                        Op = "<=";
                        break;
                    case BLT:
                        Op = "<";
                        break;
                }
                printf("branch(%s) t%d %s\n", Op, b->reg, ((struct Label *) b->label)->name);
                break;
            }
            case Label:
                printf("%s:\n", ((struct Label *) p->info)->name);
                break;
            case ArrL: {
                struct ArrL *a = p->info;
                printf("$t%d = %s", a->to, a->label);
                if (a->offKind == facReg) {
                    printf("[$t%d]\n", a->offsetValue);
                } else {
                    printf("[%d]\n", a->offsetValue);
                }
                break;
            }
            case ArrS: {
                struct ArrS *a = p->info;
                printf("%s", a->label);
                if (a->offKind == facReg) {
                    printf("[$t%d] = ", a->offsetValue);
                } else {
                    printf("[%d] = ", a->offsetValue);
                }
                if (a->fromKind == facChar) {
                    printf("\'%c\'\n", a->fromValue);
                } else if (a->fromKind == facInt) {
                    printf("%d\n", a->fromValue);
                } else {
                    printf("$t%d\n", a->fromValue);
                }
                break;
            }
            case Read: {
                struct Read *r = p->info;
                printf("Read(%s) $t%d\n", r->type == INT ? "Int" : "Char", r->reg);
                break;
            }
            case Write: {
                struct Write *w = p->info;
                printf("Write ");
                switch (w->writeType) {
                    case STR_ONLY:
                        printf("\"%s\"\n", w->string);
                        break;
                    case REG_ONLY:
                        printf("$t%d\n", w->value);
                        break;
                    case VALUE_ONLY: {
                        if (w->type == INT)
                            printf("%d\n", w->value);
                        else
                            printf("\'%c\'\n", w->value);
                        break;
                    }
                    case STR_REG:
                        printf("\"%s\",$t%d\n", w->string, w->value);
                        break;
                    case STR_VALUE:
                        if (w->type == INT)
                            printf("\"%s\",%d\n", w->string, w->value);
                        else
                            printf("\"%s\",\'%c\'\n", w->string, w->value);
                        break;
                }
                break;
            }
            case SavEnv: {
                struct SavEnv *se = p->info;
                printf("Save Env(%1d){", se->isRecursion);
                int t;
                for (t = 0; t < se->regListSize; t++) {
                    if (t + $t0 + MIDREG > $t9) {
                        printf("%d,", -(t + MIDREG + $t0 - $t9));
                    } else {
                        printf("$%d,", t + $t0 + MIDREG);
                    }
                }
                printf("}\n");
                break;
            }
            case RevEnv: {
                struct RevEnv *re = p->info;
                printf("Revert Env(%1d){", re->isRecursion);
                int t;
                for (t = re->regListSize - 1; t >= 0; t--) {
                    if (t + $t0 + MIDREG > $t9) {
                        printf("%d,", -(t + MIDREG + $t0 - $t9));
                    } else {
                        printf("$%d,", t + $t0 + MIDREG);
                    }
                }
                printf("}\n");
                break;
            }
        }
    }

}
