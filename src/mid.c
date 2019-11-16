#include "mid.h"
#include "string.h"
#include "stdarg.h"
#include "stdlib.h"

struct Code *emit(enum CodeType t, int size, ...) {
    struct Code *new = (struct Code *) malloc(sizeof(struct Code));
    new->type = t;
    new->prev = NULL;
    new->next = NULL;

    va_list vl;
    va_start(vl, size);
    if (t == Push && size == 1) {
        struct Push *p = (struct Push *) malloc(sizeof(struct Push));
        p->reg = va_arg(vl, int);
        new->info = p;
    } else if (t == Call && size == 1) {
        struct Call *p = (struct Call *) malloc(sizeof(struct Call));
        p->label = va_arg(vl, struct Label*);
        new->info = p;
    } else if (t == Para && size == 1) {
        struct Para *p = (struct Para *) malloc(sizeof(struct Para));
        p->reg = va_arg(vl, int);
        new->info = p;
    } else if (t == ReadRet && size == 1) {
        struct ReadRet *p = (struct ReadRet *) malloc(sizeof(struct ReadRet));
        p->reg = va_arg(vl, int);
        new->info = p;
    } else if (t == Ret && size == 1) {
        struct Ret *p = (struct Ret *) malloc(sizeof(struct Ret));
        p->reg = va_arg(vl, int);
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
    } else if (t == Tuple && size == 4) {
        struct Tuple *p = (struct Tuple *) malloc(sizeof(struct Tuple));
        p->op = va_arg(vl, enum Op);
        p->regA = va_arg(vl, int);
        p->regB = va_arg(vl, int);
        p->regC = va_arg(vl, int);
        new->info = p;
    } else if (t == Assig && size == 2) {
        struct Assig *p = (struct Assig *) malloc(sizeof(struct Assig));
        p->from = va_arg(vl, int);
        p->to = va_arg(vl, int);
        new->info = p;
    } else if (t == Eql && size == 2) {
        struct Eql *p = (struct Eql *) malloc(sizeof(struct Eql));
        p->value = va_arg(vl, int);
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
        p->name = va_arg(vl, char*);
        p->reg = va_arg(vl, int);
        p->offset = va_arg(vl, int);
        p->to = va_arg(vl, int);
        new->info = p;
    } else if (t == ArrS && size == 4) {
        struct ArrS *p = (struct ArrS *) malloc(sizeof(struct ArrS));
        p->name = va_arg(vl, char*);
        p->reg = va_arg(vl, int);
        p->offset = va_arg(vl, int);
        p->from = va_arg(vl, int);
        new->info = p;
    } else if (t == Read && size == 1) {
        struct Read *p = (struct Read *) malloc(sizeof(struct Read));
        p->reg = va_arg(vl, int);
        new->info = p;
    } else if (t == Write && size == 3) {
        struct Write *p = (struct Write *) malloc(sizeof(struct Write));
        p->isReg = va_arg(vl, int);
        p->string = va_arg(vl, char*);
        p->reg = va_arg(vl, int);
        new->info = p;
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

int newRegister() {
    return ++Register;
}

void saveRegister() {
    saveReg = Register;
}

void revertRegister() {
    //Register = saveReg;
}

/**
 * 查找符号表，找到变量分配的的寄存器
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
        if (p->type == Label) {
            struct Label *l = p->info;
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
    }
}

void printCode() {
    struct Code *p;
    for (p = code; p != NULL; p = p->next) {
        switch (p->type) {
            case Push:
                printf("Push t%d\n", ((struct Push *) p->info)->reg);
                break;
            case Call:
                printf("Call %s\n", ((struct Call *) p->info)->label->name);
                break;
            case Para:
                printf("Para t%d\n", ((struct Para *) p->info)->reg);
                break;
            case Ret:
                printf("Return t%d\n", ((struct Ret *) p->info)->reg);
                break;
            case ReadRet:
                printf("getReturn t%d\n", ((struct ReadRet *) p->info)->reg);
                break;
            case Var: {
                struct Var *v = p->info;
                printf("Var %s t%d %d\n", v->name, v->reg, v->size);
                break;
            }
            case Const: {
                struct Const *c = p->info;
                printf("Const %s t%d = %d\n", c->name, c->reg, c->value);
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
                }
                printf("t%d = t%d %c t%d\n", t->regC, t->regA, Op, t->regB);
                break;
            }
            case Eql: {
                struct Eql *e = p->info;
                printf("t%d = %d\n", e->to, e->value);
                break;
            }
            case Assig: {
                struct Assig *a = p->info;
                printf("t%d = t%d\n", a->to, a->from);
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
                printf("t%d = t%d[t%d]\n", a->to, a->reg, a->offset);
                break;
            }
            case ArrS: {
                struct ArrS *a = p->info;
                printf("t%d[t%d] = t%d\n", a->reg, a->offset, a->from);
                break;
                case Read:
                    printf("Read t%d\n", ((struct Read *) p->info)->reg);
                break;
                case Write: {
                    struct Write *w = p->info;
                    if (w->isReg) {
                        printf("Write t%d\n", w->reg);
                    } else {
                        printf("Write \"%s\"\n", w->string);
                    }
                    break;
                }
            }
        }
    }
}

