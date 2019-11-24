#include "generate.h"
#include "string.h"

#define COMMENT 1
struct Code *curCode;

void genMips() {
    hasEntry = 0;
    arrayCount = 0;
    printMips(".text");
    printMips("la $s0,head");
    struct Code *p;
    for (p = code; p != NULL; p = p->next) {
        switch (p->type) {
            case Func: {
                if (!hasEntry) {
                    hasEntry = 1;
                    printMips("j Main");
                    printMips("nop");
                }
                genFunc(p->info);
                struct Code *t = p->next;
                paraCount = 0;
                while (t != NULL && t->type == Para) {
                    paraCount++;
                    t = t->next;
                }
                ((struct Func *) p->info)->paraSize = paraCount;
                temP = p;
                break;
            }
            case Push:
                if (!startPush) {
                    //在父函数调用(第一个push)之前保存父环境
                    curCode = p;
                    saveEnv();
                    startPush = 1;
                }
                genPush(p->info);
                break;
            case Call:
                if (!startPush) {
                    //在父函数调用之前保存父环境
                    curCode = p;
                    saveEnv();
                    startPush = 1;
                }
                genCall(p->info);
                startPush = 0;
                //恢复父环境
                revertEnv();
                break;
            case Para: {
                //这里直接让p跳到最后一个参数的下一个，并从后往前反向读取参数
                //这里直接把所有参数一次性读完
                int t = 0;
                struct Code *tp;
                while (p->type == Para) {
                    p = p->next;
                    t++;
                }
                p = p->prev;
                for (tp = p; tp->type == Para; tp = tp->prev, t--) {
                    genPara(tp->info, t);
                }
                break;
            }
            case Ret:
                genRet(p->info);
                break;
            case ReadRet:
                genReadRet(p->info);
                break;
            case Var:
                genVar(p->info);
                break;
            case Const:
                genConst(p->info);
                break;
            case Tuple:
                genTuple(p->info);
                break;
            case Assig:
                genAssig(p->info);
                break;
            case Goto:
                genGoto(p->info);
                break;
            case Bra:
                genBra(p->info);
                break;
            case Label:
                genLab(p->info);
                break;
            case ArrL:
                genArrL(p->info);
                break;
            case ArrS:
                genArrS(p->info);
                break;
            case Read:
                genRead(p->info);
                break;
            case Write:
                genWrite(p->info);
                break;
        }
    }
    end();
    initData();
}

/**
 * 生成函数头及保护现场
 */
void genFunc(struct Func *p) {
    if (COMMENT)printMips("#函数头");
    printMips("%s:", p->name);
    /*if (strcmp(p->name, "main") != 0) {
        //若不是main
        if (COMMENT)printMips("#保存环境");
        //printMips("sw $sp,4($sp)");
        printMips("addi $sp,$sp,-4");
        printMips("sw $ra,0($sp)");
    }*/
}

/**
 * 生成参数入栈
 */
void genPush(struct Push *p) {
    if (COMMENT)printMips("#参数入栈");
    if (p->kind == facReg) {
        printMips("lw $t0,%d($s0)", p->value * 4);
    } else {
        printMips("li $t0,%d", p->value);
    }
    printMips("addi $sp,$sp,-4");
    printMips("sw $t0,0($sp)");
}

/**
 * 生成函数调用
 */
void genCall(struct Call *p) {
    if (COMMENT)printMips("#调用函数");
    printMips("jal %s", p->func->name);
    printMips("nop");
}

/**
 * 生成参数出栈
 */
void genPara(struct Para *p, int index) {
    if (COMMENT)printMips("#参数%d出栈", index);
    printMips("lw $t0,0($sp)");
    printMips("addi $sp,$sp,4");
    printMips("sw $t0,%d($s0)", p->reg * 4);
}

/**
 * 生成读取返回值
 */
void genReadRet(struct ReadRet *p) {
    if (COMMENT)printMips("#读取返回值");
    printMips("sw $v0,%d($s0)", p->reg * 4);
}

/**
 * 生成返回语句，并还原现场
 */
void genRet(struct Ret *p) {
    //if (COMMENT)printMips("#sp,ra出栈,并移除参数区");
    //printMips("lw $ra,0($sp)");
    //printMips("addi $sp,$sp,4");
    //printMips("addi $sp,$sp,%d", paraCount * 4);
    if (COMMENT)printMips("#返回语句");
    if (p->kind != facReg) {
        printMips("li $v0,%d", p->value);
    } else {
        if (p->value != 0) printMips("lw $v0,%d($s0)", p->value * 4);
    }
    printMips("jr $ra");
    printMips("nop");
    paraCount = 0;
}

/**
 * 生成变量定义（主要是分配数组空间）
 */
void genVar(struct Var *p) {
    if (p->size != 0 && p->reg == -1) {
        //说明是一个数组
        strcpy(arrLabList[arrayCount], p->name);
        arrSizeList[arrayCount] = p->size;
        arrayCount++;
    }
}

/**
 * 生成常量定义且初值确定
 */
void genConst(struct Const *p) {
    if (COMMENT)printMips("#常量赋值");
    printMips("li $t0,%d", p->value);
    printMips("sw $t0,%d($s0)", p->reg * 4);
}

/**
 * 生成四元式
 */
void genTuple(struct Tuple *p) {
    if (COMMENT) {
        if (p->op == PlusOp)printMips("#加法");
        else if (p->op == MinuOp)printMips("#减法");
        else if (p->op == MultOp)printMips("#乘法");
        else if (p->op == DiviOp)printMips("#除法");
    }
    if (p->factorKindA == facReg) {
        printMips("lw $t0,%d($s0)", p->valueA * 4);
    } else {
        printMips("li $t0,%d", p->valueA);
    }
    switch (p->op) {
        case PlusOp: {
            if (p->factorKindB == facReg) {
                printMips("lw $t1,%d($s0)", p->valueB * 4);
                printMips("add $t2,$t0,$t1");
            } else {
                printMips("addi $t2,$t0,%d", p->valueB);
            }
            break;
        }
        case MinuOp: {
            if (p->factorKindB == facReg) {
                printMips("lw $t1,%d($s0)", p->valueB * 4);
                printMips("sub $t2,$t0,$t1");
            } else {
                printMips("subi $t2,$t0,%d", p->valueB);
            }
            break;
        }
        case MultOp: {
            if (p->factorKindB == facReg) {
                printMips("lw $t1,%d($s0)", p->valueB * 4);
            } else {
                printMips("li $t1,%d", p->valueB);
            }
            printMips("mult $t0,$t1");
            printMips("mflo $t2");
            break;
        }
        case DiviOp: {
            if (p->factorKindB == facReg) {
                printMips("lw $t1,%d($s0)", p->valueB * 4);
            } else {
                printMips("li $t1,%d", p->valueB);
            }
            printMips("div $t0,$t1");
            printMips("mflo $t2");
            break;
        }
    }
    printMips("sw $t2,%d($s0)", p->regC * 4);
}

/**
 * 生成赋值语句（模拟寄存器到模拟寄存器）
 */
void genAssig(struct Assig *p) {
    if (COMMENT)printMips("#赋值");
    if (p->fromKind == facReg) {
        printMips("lw $t0,%d($s0)", p->fromValue * 4);
    } else {
        printMips("li $t0,%d", p->fromValue);
    }
    printMips("sw $t0,%d($s0)", p->to * 4);
}

/**
 * 生成无条件跳转语句
 */
void genGoto(struct Goto *p) {
    if (COMMENT)printMips("#跳转");
    printMips("j %s", p->label->name);
    printMips("nop");
}

/**
 * 生成有条件跳转语句
 */
void genBra(struct Bra *p) {
    if (COMMENT)printMips("#分支");
    printMips("lw $t0,%d($s0)", p->reg * 4);
    switch (p->type) {
        case BEQ: {
            printMips("beq $t0,$0,%s", p->label->name);
            break;
        }
        case BNE: {
            printMips("bne $t0,$0,%s", p->label->name);
            break;
        }
        case BGE: {
            printMips("bgez $t0,%s", p->label->name);
            break;
        }
        case BGT: {
            printMips("bgtz $t0,%s", p->label->name);
            break;
        }
        case BLE: {
            printMips("blez $t0,%s", p->label->name);
            break;
        }
        case BLT: {
            printMips("bltz $t0,%s", p->label->name);
            break;
        }
    }
    printMips("nop");
}

/**
 * 生成标签
 */
void genLab(struct Label *p) {
    printMips("%s:", p->name);
}

/**
 * 生成读数组
 */
void genArrL(struct ArrL *p) {
    if (COMMENT)printMips("#读数组");
    if (p->offKind == facReg) {
        printMips("lw $t0,%d($s0)", p->offsetValue * 4);
        printMips("sll $t0,$t0,2");
        printMips("lw $t1,%s($t0)", p->label);
    } else {
        printMips("lw $t1,%s+%d", p->label, p->offsetValue * 4);
    }
    printMips("sw $t1,%d($s0)", p->to * 4);
}

/**
 * 生成写数组
 */
void genArrS(struct ArrS *p) {
    if (COMMENT)printMips("#写数组");
    if (p->fromKind == facReg) {
        printMips("lw $t0,%d($s0)", p->fromValue * 4);
    } else {
        printMips("li $t0,%d", p->fromValue);
    }
    if (p->offKind == facReg) {
        printMips("lw $t1,%d($s0)", p->offsetValue * 4);
        printMips("sll $t1,$t1,2");
        printMips("sw $t0,%s($t1)", p->label);
    } else {
        printMips("sw $t0,%s+%d", p->label, p->offsetValue * 4);
    }
}

/**
 * 生成标准输入
 */
void genRead(struct Read *p) {
    if (COMMENT)printMips("#读语句%s", p->type == INT ? "Int" : "Char");
    printMips("li $v0,%d", p->type == INT ? 5 : 12);
    printMips("syscall");
    printMips("sw $v0,%d($s0)", p->reg * 4);
}

/**
 * 生成标准输出
 */
void genWrite(struct Write *p) {
    switch (p->writeType) {
        case STR_ONLY: {
            if (COMMENT)printMips("#写语句String");
            printMips("li $v0,4");
            printMips("la $a0,%s", getStrLab(p->string));
            printMips("syscall");
            break;
        }
        case REG_ONLY: {
            if (COMMENT)printMips("#写语句Reg(%s)", p->type == INT ? "Int" : "Char");
            printMips("li $v0,%d", p->type == INT ? 1 : 11);
            printMips("lw $a0,%d($s0)", p->value * 4);
            printMips("syscall");
            break;
        }
        case VALUE_ONLY: {
            if (COMMENT)printMips("#写语句Value(%s)", p->type == INT ? "Int" : "Char");
            printMips("li $v0,%d", p->type == INT ? 1 : 11);
            printMips("li $a0,%d", p->value);
            printMips("syscall");
            break;
        }
        case STR_REG: {
            if (COMMENT)printMips("#写语句String,Reg(%s)", p->type == INT ? "Int" : "Char");
            printMips("li $v0,4");
            printMips("la $a0,%s", getStrLab(p->string));
            printMips("syscall");
            printMips("li $v0,%d", p->type == INT ? 1 : 11);
            printMips("lw $a0,%d($s0)", p->value * 4);
            printMips("syscall");
            break;
        }
        case STR_VALUE: {
            if (COMMENT)printMips("#写语句Value,Value(%s)", p->type == INT ? "Int" : "Char");
            printMips("li $v0,4");
            printMips("la $a0,%s", getStrLab(p->string));
            printMips("syscall");
            printMips("li $v0,%d", p->type == INT ? 1 : 11);
            printMips("li $a0,%d", p->value);
            printMips("syscall");
            break;
        }
    }
    //输出\n
    printMips("li $v0,11");
    printMips("li $a0,10");
    printMips("syscall");
}

/**
 * main中生成return
 */
void end() {
    printMips("li $v0,10");
    printMips("syscall");
    printMips("\n");
}

/**
 * 给虚拟寄存器和数组和字符串分配空间
 */
void initData() {
    printMips(".data");
    printMips("    head:.space %d", (Register + 1) * 4);
    while (arrayCount--) {
        printMips("    %s:.space %d", arrLabList[arrayCount], arrSizeList[arrayCount] * 4);
    }
    while (strCount--) {
        printMips("    %s:.asciiz \"%s\"", strLabel[strCount], strList[strCount]);
    }
}

/**
 * 将该字符串写入表单，以备后续开辟空间
 * 返回该字符串的标签
 */
char *getStrLab(char *str) {
    strLabel[strCount][0] = 's';
    strLabel[strCount][1] = 't';
    strLabel[strCount][2] = 'r';
    strLabel[strCount][3] = (char) (strCount / 100 + '0');
    strLabel[strCount][4] = (char) ((strCount / 10) % 10 + '0');
    strLabel[strCount][5] = (char) (strCount % 10 + '0');
    strLabel[strCount][6] = '\0';

    int t;
    for (t = 0; *str; str++, t++) {
        if (*str == '\\') {
            strList[strCount][t] = '\\';
            t++;
        }
        strList[strCount][t] = *str;
    }
    //strcpy(strList[strCount], str);
    return strLabel[strCount++];
}

void saveEnv() {
    struct Code *c;
    printMips("#保存环境");
    for (c = temP->next; c != curCode; c = c->next) {
        //从函数标签开始，找到参数和变量，并将其保存
        if (c->type == Para) {
            struct Para *pa = c->info;
            printMips("lw $t0,%d($s0) #Save Para", pa->reg * 4);
            __inSp();
        } else if (c->type == Var) {
            struct Var *v = c->info;
            if (v->size == 0) {
                //普通变量
                printMips("lw $t0,%d($s0) #Save Var", v->reg * 4);
                __inSp();
            } else {
                //数组变量
                int t;
                for (t = 0; t < v->size; t++) {
                    printMips("lw $t0,%s+%d #Save %s[%d]", v->name, t * 4, v->name, t);
                    __inSp();
                }
            }
        } else if (c->type == Const) {
        }//这里还保存了所有可能涉及到的中间变量(可优化)
        else if (c->type == ReadRet) {
            struct ReadRet *r = c->info;
            printMips("lw $t0,%d($s0) #Save ReadRet", r->reg * 4);
            __inSp();
        } else if (c->type == Tuple) {
            struct Tuple *t = c->info;
            printMips("lw $t0,%d($s0) #Save Tuple", t->regC * 4);
            __inSp();
        } /*else if (c->type == Assig) {
            struct Assig *a = c->info;
            printMips("lw $t0,%d($s0) #Save Assig", a->to * 4);
            __inSp();
        }*/
        else if (c->type == ArrL) {
            struct ArrL *a = c->info;
            printMips("lw $t0,%d($s0) #Save ArrL", a->to * 4);
            __inSp();
        }
        //else break;
    }
    //将temP指向最后一个已存储的局部变量处,便于revertEnv
    temP = c->prev;
    printMips("addi $sp,$sp,-4 #Save $ra");
    printMips("sw $ra,0($sp)");
}

void revertEnv() {
    struct Code *c;
    printMips("#恢复环境");
    printMips("lw $ra,0($sp) #Revert $ra");
    printMips("addi $sp,$sp,4");
    for (c = temP; c->type != Func; c = c->prev) {
        //从temP开始，往回找到参数、常量和变量，并将其恢复
        if (c->type == Para) {
            struct Para *pa = c->info;
            __outSp("Para");
            printMips("sw $t0,%d($s0)", pa->reg * 4);
        } else if (c->type == Var) {
            struct Var *v = c->info;
            if (v->size == 0) {
                //普通变量
                __outSp("Var");
                printMips("sw $t0,%d($s0)", v->reg * 4);
            } else {
                //数组变量
                int t;
                for (t = v->size - 1; t >= 0; t--) {
                    __outSp("Revert array");
                    printMips("sw $t0,%s+%d", v->name, t * 4);
                }
            }
        } else if (c->type == Const) {
        }//这里恢复了所有可能涉及的中间变量(可优化)
        else if (c->type == ReadRet) {
            struct ReadRet *r = c->info;
            __outSp("ReadRet");
            printMips("sw $t0,%d($s0)", r->reg * 4);
        } else if (c->type == Tuple) {
            struct Tuple *t = c->info;
            __outSp("Tuple");
            printMips("sw $t0,%d($s0)", t->regC * 4);
        } /*else if (c->type == Assig) {
            struct Assig *a = c->info;
            __outSp("Assig");
            printMips("sw $t0,%d($s0)", a->to * 4);
        }*/
        else if (c->type == ArrL) {
            struct ArrL *a = c->info;
            __outSp("ArrL");
            printMips("sw $t0,%d($s0)", a->to * 4);
        }
        //else break;
    }
    //此时temP为函数label
    temP = c;
}

void __inSp() {
    printMips("addi $sp,$sp,-4");
    printMips("sw $t0,0($sp)");
}

void __outSp(char *msg) {
    printMips("lw $t0,0($sp) #Revert %s", msg);
    printMips("addi $sp,$sp,4");
}
//todo 尽量使用寄存器