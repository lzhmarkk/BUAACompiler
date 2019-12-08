#include "generate.h"
#include "string.h"
#include "reg.h"

#define COMMENT 1
//记录每个saveEnv的位置，便于从函数头开始找到所有的局部变量和revert时按照倒序恢复局部变量
struct Code *curCode[ENV_STACK_LENGTH];
int curCodeSp;
//函数头指针
struct Code *fhead;

void genMips() {
    hasEntry = 0;
    arrayCount = 0;
    printMips(".text");
    printMips("la $gp,glob");
    struct Code *p;
    for (p = code; p != NULL; p = p->next) {
        switch (p->type) {
            case Func: {
                if (!hasEntry) {
                    hasEntry = 1;
                    printMips("jal Main");
                    printMips("nop");
                    printMips("j END");
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
                fhead = p;
                break;
            }
            case Push:
                genPush(p->info);
                break;
            case Call:
                genCall(p->info);
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
            case SavEnv:
                curCode[curCodeSp] = p;
                saveEnv(((struct SavEnv *) p->info)->isRecursion);
                curCodeSp++;
                break;
            case RevEnv:
                curCodeSp--;
                revertEnv(((struct RevEnv *) p->info)->isRecursion);
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
        //如果是寄存器
        if (isRegGlo(p->value)) {
            //1001
            printMips("lw $v0,%d($gp)", (p->value - GLO) * 4);
            printMips("addi $sp,$sp,-4");
            printMips("sw $v0,0($sp)");
        } else if (isRegMem(p->value)) {
            //-2
            printMips("lw $v0,head+%d", -p->value * 4);
            printMips("addi $sp,$sp,-4");
            printMips("sw $v0,0($sp)");
        } else {
            //3
            printMips("addi $sp,$sp,-4");
            printMips("sw $%d,0($sp)", p->value + $t0);
        }
    } else {
        //如果是值
        printMips("li $v0,%d", p->value);
        printMips("addi $sp,$sp,-4");
        printMips("sw $v0,0($sp)");
    }
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
    if (isRegGlo(p->reg)) {
        printMips("lw $v0,0($sp)");
        printMips("addi $sp,$sp,4");
        printMips("sw $v0,%d($sp)", (p->reg - GLO) * 4);
    } else if (isRegMem(p->reg)) {
        printMips("lw $v0,0($sp)");
        printMips("addi $sp,$sp,4");
        printMips("sw $v0,head+%d", -p->reg * 4);
    } else {
        printMips("lw $%d,0($sp)", p->reg + $t0);
        printMips("addi $sp,$sp,4");
    }
}

/**
 * 生成读取返回值
 */
void genReadRet(struct ReadRet *p) {
    if (COMMENT)printMips("#读取返回值");
    printMips("move $%d,$v0", p->reg + $t0);
    //todo 直接v0运算
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
        //返回值
        printMips("li $v0,%d", p->value);
    } else {
        //返回寄存器
        if (isRegGlo(p->value)) {
            printMips("lw $v0,%d($gp)", (p->value - GLO) * 4);
        } else if (isRegMem(p->value)) {
            printMips("lw $v0,head+%d", -p->value * 4);
        } else {
            printMips("move $v0,$%d", p->value + $t0);
        }
    }
    printMips("jr $ra");
    printMips("nop");
    paraCount = 0;
}

/**
 * 生成变量定义（主要是分配数组空间）
 */
void genVar(struct Var *p) {
    if (p->size != 0 && p->reg == 0) {
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
    //常量实现传播，不再赋值
    //printMips("li $t0,%d", p->value);
    //printMips("sw $t0,%d($s0)", p->reg * 4);
}

/**
 * 生成四元式
 */
void genTuple(struct Tuple *p) {
    int regA, regB, regC;
    if (isRegGlo(p->regC) || isRegMem(p->regC)) {
        regC = $v0;
    } else {
        regC = p->regC + $t0;
    }
    if (COMMENT) {
        if (p->op == PlusOp)printMips("#加法");
        else if (p->op == MinuOp)printMips("#减法");
        else if (p->op == MultOp)printMips("#乘法");
        else if (p->op == DiviOp)printMips("#除法");
    }
    if (p->factorKindA == facReg) {
        //操作数A是寄存器
        if (isRegGlo(p->valueA)) {
            printMips("lw $v0,%d($gp)", (p->valueA - GLO) * 4);
            regA = $v0;
        } else if (isRegMem(p->valueA)) {
            printMips("lw $v0,head+%d", -p->valueA * 4);
            regA = $v0;
        } else {
            regA = p->valueA + $t0;
        }
    } else {
        //操作数A是值
        printMips("li $v0,%d", p->valueA);
        regA = $v0;
    }
    //以上将A的结果读到$v0
    switch (p->op) {
        case PlusOp: {
            if (p->factorKindB == facReg) {
                //操作数B是寄存器
                if (isRegGlo(p->valueB)) {
                    printMips("lw $v1,%d($gp)", (p->valueB - GLO) * 4);
                    regB = $v1;
                } else if (isRegMem(p->valueB)) {
                    printMips("lw $v1,head+%d", -p->valueB * 4);
                    regB = $v1;
                } else {
                    regB = p->valueB + $t0;
                }
                printMips("add $%d,$%d,$%d", regC, regA, regB);
            } else {
                //操作数B是值
                printMips("addi $%d,$%d,%d", regC, regA, p->valueB);
            }
            break;
        }
        case MinuOp: {
            if (p->factorKindB == facReg) {
                //操作数B是寄存器
                if (isRegGlo(p->valueB)) {
                    printMips("lw $v1,%d($gp)", (p->valueB - GLO) * 4);
                    regB = $v1;
                } else if (isRegMem(p->valueB)) {
                    printMips("lw $v1,head+%d", -p->valueB * 4);
                    regB = $v1;
                } else {
                    regB = p->valueB + $t0;
                }
                printMips("sub $%d,$%d,$%d", regC, regA, regB);
            } else {
                printMips("subi $%d,$%d,%d", regC, regA, p->valueB);
            }
            break;
        }
        case MultOp: {
            if (p->factorKindB == facReg) {
                //操作数B是寄存器
                if (isRegGlo(p->valueB)) {
                    printMips("lw $v1,%d($gp)", (p->valueB - GLO) * 4);
                    regB = $v1;
                } else if (isRegMem(p->valueB)) {
                    printMips("lw $v1,head+%d", -p->valueB * 4);
                    regB = $v1;
                } else {
                    regB = p->valueB + $t0;
                }
            } else {
                //操作数B是值
                printMips("li $v1,%d", p->valueB);
                regB = $v1;
            }
            printMips("mult $%d,$%d", regA, regB);
            printMips("mflo $%d", regC);
            break;
        }
        case DiviOp: {
            if (p->factorKindB == facReg) {
                //操作数B是寄存器
                if (isRegGlo(p->valueB)) {
                    printMips("lw $v1,%d($gp)", (p->valueB - GLO) * 4);
                    regB = $v1;
                } else if (isRegMem(p->valueB)) {
                    printMips("lw $v1,head+%d", -p->valueB * 4);
                    regB = $v1;
                } else {
                    regB = p->valueB + $t0;
                }
            } else {
                printMips("li $v1,%d", p->valueB);
                regB = $v1;
            }
            printMips("div $%d,$%d", regA, regB);
            printMips("mflo $%d", regC);
            break;
        }
    }
    //以上将运算结果写到regC
    if (isRegGlo(p->regC)) {
        printMips("sw $%d,%d($gp)", regC, (p->regC - GLO) * 4);
    } else if (isRegMem(p->regC)) {
        printMips("sw $%d,head+%d", regC, -p->regC * 4);
    }
    //这种情况下，已经写入了regC中
}

/**
 * 生成赋值语句
 */
void genAssig(struct Assig *p) {
    int regFrom;
    if (COMMENT)printMips("#赋值");
    if (p->fromKind == facReg) {
        if (isRegGlo(p->fromValue)) {
            printMips("lw $v0,%d($gp)", (p->fromValue - GLO) * 4);
            regFrom = $v0;
        } else if (isRegMem(p->fromValue)) {
            printMips("lw $v0,head+%d", -p->fromValue * 4);
            regFrom = $v0;
        } else {
            regFrom = p->fromValue + $t0;
        }
        //以上将结果读到regFrom
        if (isRegGlo(p->to)) {
            printMips("sw $%d,%d($gp)", regFrom, (p->to - GLO) * 4);
        } else if (isRegMem(p->to)) {
            printMips("sw $%d,head+%d", regFrom, -p->to * 4);
        } else {
            printMips("move $%d,$%d", p->to + $t0, regFrom);
        }
    } else {
        if (isRegGlo(p->to)) {
            printMips("li $v0,%d", p->fromValue);
            printMips("sw $v0,%d($gp)", (p->to - GLO) * 4);
        } else if (isRegMem(p->to)) {
            printMips("li $v0,%d", p->fromValue);
            printMips("sw $v0,head+%d", -p->to * 4);
        } else {
            printMips("li $%d,%d", p->to + $t0, p->fromValue);
        }
    }
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
    int inV0 = 0;
    if (isRegGlo(p->reg)) {
        printMips("lw $v0,%d($gp)", (p->reg - GLO) * 4);
        inV0 = 1;
    } else if (isRegMem(p->reg)) {
        printMips("lw $v0,head+%d", -p->reg * 4);
        inV0 = 1;
    }
    switch (p->type) {
        case BEQ: {
            if (inV0) {
                printMips("beq $v0,$0,%s", p->label->name);
            } else {
                printMips("beq $%d,$0,%s", p->reg + $t0, p->label->name);
            }
            break;
        }
        case BNE: {
            if (inV0) {
                printMips("bne $v0,$0,%s", p->label->name);
            } else {
                printMips("bne $%d,$0,%s", p->reg + $t0, p->label->name);
            }
            break;
        }
        case BGE: {
            if (inV0) {
                printMips("bgez $v0,%s", p->label->name);
            } else {
                printMips("bgez $%d,%s", p->reg + $t0, p->label->name);
            }
            break;
        }
        case BGT: {
            if (inV0) {
                printMips("bgtz $v0,%s", p->label->name);
            } else {
                printMips("bgtz $%d,%s", p->reg + $t0, p->label->name);
            }
            break;
        }
        case BLE: {
            if (inV0) {
                printMips("blez $v0,%s", p->label->name);
            } else {
                printMips("blez $%d,%s", p->reg + $t0, p->label->name);
            }
            break;
        }
        case BLT: {
            if (inV0) {
                printMips("bltz $v0,%s", p->label->name);
            } else {
                printMips("bltz $%d,%s", p->reg + $t0, p->label->name);
            }
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
        //偏移是寄存器
        if (isRegGlo(p->offsetValue)) {
            printMips("lw $v0,%d($gp)", (p->offsetValue - GLO) * 4);
            printMips("sll $v0,$v0,2");
        } else if (isRegMem(p->offsetValue)) {
            printMips("lw $v0,head+%d", -p->offsetValue * 4);
            printMips("sll $v0,$v0,2");
        } else {
            printMips("sll $v0,$%d,2", p->offsetValue + $t0);
        }
        if (isRegGlo(p->to)) {
            printMips("lw $v0,%s($v0)", p->label);
            printMips("sw $v0,%d($gp)", (p->to - GLO) * 4);
        } else if (isRegMem(p->to)) {
            printMips("lw $v0,%s($v0)", p->label);
            printMips("sw $v0,head+%d", -p->to * 4);
        } else {
            printMips("lw $%d,%s($v0)", p->to + $t0, p->label);
        }
    } else {
        //偏移是值
        if (isRegGlo(p->to)) {
            printMips("lw $v0,%s+%d", p->label, p->offsetValue * 4);
            printMips("sw $v0,%d($gp)", (p->to - GLO) * 4);
        } else if (isRegMem(p->to)) {
            printMips("lw $v0,%s+%d", p->label, p->offsetValue * 4);
            printMips("sw $v0,head+%d", -p->to * 4);
        } else {
            printMips("lw $%d,%s+%d", p->to + $t0, p->label, p->offsetValue * 4);
        }
    }
}

/**
 * 生成写数组
 */
void genArrS(struct ArrS *p) {
    int regFrom;
    if (COMMENT)printMips("#写数组");
    if (p->fromKind == facReg) {
        if (isRegGlo(p->fromValue)) {
            printMips("lw $v1,%d($gp)", (p->fromValue - GLO) * 4);
            regFrom = $v1;
        } else if (isRegMem(p->fromValue)) {
            printMips("lw $v1,head+%d", -p->fromValue * 4);
            regFrom = $v1;
        } else {
            regFrom = p->fromValue + $t0;
        }
    } else {
        printMips("li $v1,%d", p->fromValue);
        regFrom = $v1;
    }
    //以上将结果写入regFrom
    if (p->offKind == facReg) {
        if (isRegGlo(p->offsetValue)) {
            printMips("lw $v0,%d($gp)", (p->offsetValue - GLO) * 4);
            printMips("sll $v0,$v0,2");
        } else if (isRegMem(p->offsetValue)) {
            printMips("lw $v0,head+%d", -p->offsetValue * 4);
            printMips("sll $v0,$v0,2");
        } else {
            printMips("sll $v0,$%d,2", p->offsetValue + $t0);
        }
        printMips("sw $%d,%s($v0)", regFrom, p->label);
    } else {
        printMips("sw $%d,%s+%d", regFrom, p->label, p->offsetValue * 4);
    }
}

/**
 * 生成标准输入
 */
void genRead(struct Read *p) {
    if (COMMENT)printMips("#读语句%s", p->type == INT ? "Int" : "Char");
    printMips("li $v0,%d", p->type == INT ? 5 : 12);
    printMips("syscall");
    if (isRegGlo(p->reg)) {
        printMips("sw $v0,%d($gp)", (p->reg - GLO) * 4);
    } else if (isRegMem(p->reg)) {
        printMips("sw $v0,head+%d", -p->reg * 4);
    } else {
        printMips("move $%d,$v0", p->reg + $t0);
    }
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
            if (isRegGlo(p->value)) {
                printMips("lw $a0,%d($gp)", (p->value - GLO) * 4);
            } else if (isRegMem(p->value)) {
                printMips("lw $a0,head+%d", -p->value * 4);
            } else {
                printMips("move $a0,$%d", p->value + $t0);
            }
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
            if (isRegGlo(p->value)) {
                printMips("lw $a0,%d($gp)", (p->value - GLO) * 4);
            } else if (isRegMem(p->value)) {
                printMips("lw $a0,head+%d", -p->value * 4);
            } else {
                printMips("move $a0,$%d", p->value + $t0);
            }
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
    printMips("END:");
    printMips("li $v0,10");
    printMips("syscall");
    printMips("\n");
}

/**
 * 给虚拟寄存器和数组和字符串分配空间
 */
void initData() {
    printMips(".data");
    printMips("    head:.space %d", (regMemMax + 1) * 4);
    printMips("    glob:.space %d", (gloRegMemMax + 1) * 4);
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

/**
 * 保存局部变量(运行环境)
 */
void saveEnv(int isRecursion) {
    struct Code *c;
    printMips("#保存环境");

    //保存中间变量寄存器和局部变量寄存器
    int t;
    for (t = 0; t < MIDREG; t++) {
        printMips("addi $sp,$sp,-4 #Save reg");
        printMips("sw $%d,0($sp)", t + $t0);
    }
    for (c = fhead->next; c != curCode[curCodeSp]; c = c->next) {
        if (c->type == Para) {
            struct Para *pa = c->info;
            printMips("addi $sp,$sp,-4 #Save Para");
            if (isRegMem(pa->reg)) {
                printMips("lw $v1,head+%d", -pa->reg * 4);
                printMips("sw $v1,0($sp)");
            } else {
                printMips("sw $%d,0($sp)", pa->reg + $t0);
            }
        } else if (c->type == Var) {
            struct Var *v = c->info;
            if (v->size == 0) {
                //普通变量
                printMips("addi $sp,$sp,-4 #Save Var");
                if (isRegMem(v->reg)) {
                    printMips("lw $v1,head+%d", -v->reg * 4);
                    printMips("sw $v1,0($sp)");
                } else {
                    printMips("sw $%d,0($sp)", v->reg + $t0);
                }
            } else if (isRecursion) {
                //数组变量
                int r;
                for (r = 0; r < v->size; r++) {
                    printMips("lw $v1,%s+%d #Save %s[%d]", v->name, r * 4, v->name, r);
                    printMips("addi $sp,$sp,-4");
                    printMips("sw $v1,0($sp)");
                }
            }
        }
    }
    printMips("addi $sp,$sp,-4 #Save $ra");
    printMips("sw $ra,0($sp)");
}

/**
 * 恢复局部变量(运行环境)
 */
void revertEnv(int isRecursion) {
    struct Code *c;
    printMips("#恢复环境");
    printMips("lw $ra,0($sp) #Revert $ra");
    printMips("addi $sp,$sp,4");
    for (c = curCode[curCodeSp]; c != fhead; c = c->prev) {
        if (c->type == Para) {
            struct Para *pa = c->info;
            if (isRegMem(pa->reg)) {
                printMips("lw $v1,0($sp) #Revert Para");
                printMips("sw $v1,head+%d", -pa->reg * 4);
            } else {
                printMips("lw $%d,0($sp) #Revert Para", pa->reg + $t0);
            }
            printMips("addi $sp,$sp,4");
        } else if (c->type == Var) {
            struct Var *v = c->info;
            if (v->size == 0) {
                //普通变量
                if (isRegMem(v->reg)) {
                    printMips("lw $v1,0($sp) #Revert Var");
                    printMips("sw $v1,head+%d", -v->reg * 4);
                } else {
                    printMips("lw $%d,0($sp) #Revert Var", v->reg + $t0);
                }
                printMips("addi $sp,$sp,4");
            } else if (isRecursion) {
                //数组变量
                int t;
                for (t = v->size - 1; t >= 0; t--) {
                    printMips("lw $v1,0($sp) #Revert %s[%d]", v->name, t * 4);
                    printMips("addi $sp,$sp,4");
                    printMips("sw $v1,%s+%d", v->name, t * 4);
                }
            }
        }
    }
    int t;
    //恢复中间变量寄存器
    for (t = MIDREG - 1; t >= 0; t--) {
        printMips("lw $%d,0($sp) #Revert reg", t + $t0);
        printMips("addi $sp,$sp,4");
    }
}
//todo 赋值优化
//todo 参数优化
//todo 环境保存优化
//todo sp统一减