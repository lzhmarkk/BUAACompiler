#include "generate.h"
#include "string.h"
#include "reg.h"

#define COMMENT 1
//记录每个saveEnv的位置，便于从函数头开始找到所有的局部变量和revert时按照倒序恢复局部变量
struct Code *curCode[ENV_STACK_LENGTH];
int curCodeSp;
//函数头指针
struct Code *fhead;
int pushCount[ENV_STACK_LENGTH];

int delta;

void genMips() {
    hasEntry = 0;
    printMips(".text");
    printMips("la $gp,glob");
    struct Code *p;
    for (p = code; p != NULL; p = p->next) {
        switch (p->type) {
            case Func: {
                if (!hasEntry) {
                    hasEntry = 1;
                    printMips("jal Main");
                    //printMips("nop");
                    printMips("j END");
                    //printMips("nop");
                }
                genFunc(p->info);
                struct Code *t = p->next;
                while (t != NULL && t->type == Para) {
                    t = t->next;
                }
                fhead = p;
                delta = 0;
                arrListSize = 0;
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
                pushCount[curCodeSp] = 0;
                struct SavEnv *se = p->info;
                saveEnv(se->isRecursion, se->regListSize);
                curCodeSp++;
                break;
            case RevEnv:
                curCodeSp--;
                struct RevEnv *re = p->info;
                revertEnv(re->isRecursion, re->regListSize);
                break;
            case AlloSpa:
                allocateSpace(p->info);
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
}

/**
 * 生成参数入栈
 */
void genPush(struct Push *p) {
    if (COMMENT)printMips("#参数入栈");
    if (p->kind == facReg) {
        //如果是寄存器
        if (isRegGlo(p->value)) {
            printMips("lw $v0,%d($gp)", (p->value - GLO) * 4);
            if (pushCount[curCodeSp - 1] > 3) {
                printMips("addi $sp,$sp,-4");
                printMips("sw $v0,0($sp)");
            } else {
                printMips("move $%d,$v0", pushCount[curCodeSp - 1] + $a0);
            }
        } else if (isRegMem(p->value)) {
            printMips("lw $v0,%d($fp)", p->value * 4);
            if (pushCount[curCodeSp - 1] > 3) {
                printMips("addi $sp,$sp,-4");
                printMips("sw $v0,0($sp)");
            } else {
                printMips("move $%d,$v0", pushCount[curCodeSp - 1] + $a0);
            }
        } else {
            if (pushCount[curCodeSp - 1] > 3) {
                printMips("addi $sp,$sp,-4");
                printMips("sw $%d,0($sp)", p->value + $t0);
            } else {
                printMips("move $%d,$%d", pushCount[curCodeSp - 1] + $a0, p->value + $t0);
            }
        }
    } else {
        if (pushCount[curCodeSp - 1] > 3) {
            printMips("li $v0,%d", p->value);
            printMips("addi $sp,$sp,-4");
            printMips("sw $v0,0($sp)");
        } else {
            printMips("li $%d,%d", pushCount[curCodeSp - 1] + $a0, p->value);
        }
    }

    pushCount[curCodeSp - 1]++;
}

/**
 * 生成函数调用
 */
void genCall(struct Call *p) {
    if (COMMENT)printMips("#调用函数");
    printMips("jal %s", p->func->name);
    //printMips("nop");
}

/**
 * 生成参数出栈
 */
void genPara(struct Para *p, int index) {
    if (COMMENT)printMips("#参数%d出栈", index);
    int fromReg;
    if (index - 1 > 3) {
        printMips("lw $v0,0($sp)");
        printMips("addi $sp,$sp,4");
        fromReg = $v0;
    } else {
        fromReg = $a0 + index - 1;
    }
    if (isRegGlo(p->reg)) {
        printMips("sw $%d,%d($gp)", fromReg, (p->reg - GLO) * 4);
    } else if (isRegMem(p->reg)) {
        delta++;
        printMips("sw $%d,%d($fp)", fromReg, p->reg * 4);
    } else {
        printMips("move $%d,$%d", p->reg + $t0, fromReg);
    }//todo 可优化：统一减sp
}

/**
 * 生成读取返回值
 */
void genReadRet(struct ReadRet *p) {
    if (COMMENT)printMips("#读取返回值");
    printMips("move $%d,$v0", p->reg + $t0);
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
            printMips("lw $v0,%d($fp)", p->value * 4);
        } else {
            printMips("move $v0,$%d", p->value + $t0);
        }
    }
    printMips("addi $sp,$sp,%d #释放运行栈", delta * 4);
    printMips("jr $ra");
    //printMips("nop");
}

/**
 * 生成变量定义（主要是分配数组空间）
 */
void genVar(struct Var *p) {
    if (p->size != 0) {
        //说明是一个数组
        if (isRegMem(p->reg)) {
            //额外寄存器中的数组
            delta++;
        }
        arrList[arrListSize].size = p->size;
        arrList[arrListSize].reg = p->reg;
        arrListSize++;
    } else {
        //普通变量
        if (isRegMem(p->reg)) {
            delta++;
        }
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
            printMips("lw $v0,%d($fp)", p->valueA * 4);
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
                    printMips("lw $v1,%d($fp)", p->valueB * 4);
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
                    printMips("lw $v1,%d($fp)", p->valueB * 4);
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
                    printMips("lw $v1,%d($fp)", p->valueB * 4);
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
                    printMips("lw $v1,%d($fp)", p->valueB * 4);
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
        printMips("sw $%d,%d($fp)", regC, p->regC * 4);
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
            printMips("lw $v0,%d($fp)", p->fromValue * 4);
            regFrom = $v0;
        } else {
            regFrom = p->fromValue + $t0;
        }
        //以上将结果读到regFrom
        if (isRegGlo(p->to)) {
            printMips("sw $%d,%d($gp)", regFrom, (p->to - GLO) * 4);
        } else if (isRegMem(p->to)) {
            printMips("sw $%d,%d($fp)", regFrom, p->to * 4);
        } else {
            printMips("move $%d,$%d", p->to + $t0, regFrom);
        }
    } else {
        if (isRegGlo(p->to)) {
            printMips("li $v0,%d", p->fromValue);
            printMips("sw $v0,%d($gp)", (p->to - GLO) * 4);
        } else if (isRegMem(p->to)) {
            printMips("li $v0,%d", p->fromValue);
            printMips("sw $v0,%d($fp)", p->to * 4);
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
    //printMips("nop");
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
        printMips("lw $v0,%d($fp)", p->reg * 4);
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
    //printMips("nop");
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
            printMips("lw $v0,%d($fp)", p->offsetValue * 4);
            printMips("sll $v0,$v0,2");
        } else {
            printMips("sll $v0,$%d,2", p->offsetValue + $t0);
        }
        //以上将偏移读到$v0
        if (isRegGlo(p->head)) {
            printMips("lw $v1,%d($gp)", (p->head - GLO) * 4);
            printMips("add $v0,$v0,$v1");
            printMips("lw $v0,0($v0)");
        } else if (isRegMem(p->head)) {
            printMips("lw $v1,%d($fp)", p->head * 4);
            printMips("add $v0,$v0,$v1");
            printMips("lw $v0,0($v0)");
        } else {
            printMips("add $v0,$v0,$%d", p->head + $t0);
            printMips("lw $v0,0($v0)");
        }
        //以上将结果读到$v0
        if (isRegGlo(p->to)) {
            printMips("sw $v0,%d($gp)", (p->to - GLO) * 4);
        } else if (isRegMem(p->to)) {
            printMips("sw $v0,%d($fp)", p->to * 4);
        } else {
            printMips("move $%d,$v0", p->to + $t0);
        }//todo 略可优化
    } else {
        //偏移是值
        if (isRegGlo(p->head)) {
            printMips("lw $v1,%d($gp)", (p->head - GLO) * 4);
            printMips("lw $v0,%d($v1)", p->offsetValue * 4);
        } else if (isRegMem(p->head)) {
            printMips("lw $v1,%d($fp)", p->head * 4);
            printMips("lw $v0,%d($v1)", p->offsetValue * 4);
        } else {
            printMips("lw $v0,%d($%d)", p->offsetValue * 4, p->head + $t0);
        }
        //以上将结果读到$v0
        if (isRegGlo(p->to)) {
            printMips("sw $v0,%d($gp)", (p->to - GLO) * 4);
        } else if (isRegMem(p->to)) {
            printMips("sw $v0,%d($fp)", p->to * 4);
        } else {
            printMips("move $%d,$v0", p->to + $t0);
        }//todo 略可优化
    }
}

/**
 * 生成写数组
 */
void genArrS(struct ArrS *p) {
    int regFrom;
    if (COMMENT)printMips("#写数组");
    if (p->offKind == facReg) {
        //偏移是寄存器
        if (isRegGlo(p->offsetValue)) {
            printMips("lw $v0,%d($gp)", (p->offsetValue - GLO) * 4);
            printMips("sll $v0,$v0,2");
        } else if (isRegMem(p->offsetValue)) {
            printMips("lw $v0,%d($fp)", p->offsetValue * 4);
            printMips("sll $v0,$v0,2");
        } else {
            printMips("sll $v0,$%d,2", p->offsetValue + $t0);
        }
        //以上将偏移读到$v0
        if (isRegGlo(p->head)) {
            printMips("lw $v1,%d($gp)", (p->head - GLO) * 4);
            printMips("add $v0,$v0,$v1");
        } else if (isRegMem(p->head)) {
            printMips("lw $v1,%d($fp)", p->head * 4);
            printMips("add $v0,$v0,$v1");
        } else {
            printMips("add $v0,$v0,$%d", p->head + $t0);
        }
        //以上将地址读到$v0
        if (p->fromKind == facReg) {
            if (isRegGlo(p->fromValue)) {
                printMips("lw $v1,%d($gp)", (p->fromValue - GLO) * 4);
                regFrom = $v1;
            } else if (isRegMem(p->fromValue)) {
                printMips("lw $v1,%d($fp)", p->fromValue * 4);
                regFrom = $v1;
            } else {
                regFrom = p->fromValue + $t0;
            }
        } else {
            printMips("li $v1,%d", p->fromValue);
            regFrom = $v1;
        }
        //以上将结果写入regFrom
        printMips("sw $%d,0($v0)", regFrom);
    } else {
        //偏移是值
        if (p->fromKind == facReg) {
            if (isRegGlo(p->fromValue)) {
                printMips("lw $v1,%d($gp)", (p->fromValue - GLO) * 4);
                regFrom = $v1;
            } else if (isRegMem(p->fromValue)) {
                printMips("lw $v1,%d($fp)", p->fromValue * 4);
                regFrom = $v1;
            } else {
                regFrom = p->fromValue + $t0;
            }
        } else {
            printMips("li $v1,%d", p->fromValue);
            regFrom = $v1;
        }
        //以上将结果写入regFrom
        if (isRegGlo(p->head)) {
            printMips("lw $v0,%d($gp)", (p->head - GLO) * 4);
            printMips("sw $%d,%d($v0)", regFrom, p->offsetValue * 4);
        } else if (isRegMem(p->head)) {
            printMips("lw $v0,%d($fp)", p->head * 4);
            printMips("sw $%d,%d($v0)", regFrom, p->offsetValue * 4);
        } else {
            printMips("sw $%d,%d($%d)", regFrom, p->offsetValue * 4, p->head + $t0);
        }
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
        printMips("sw $v0,%d($fp)", p->reg * 4);
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
                printMips("lw $a0,%d($fp)", p->value * 4);
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
                printMips("lw $a0,%d($fp)", p->value * 4);
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
    printMips("    glob:.space %d", (gloRegMemMax + 1) * 4);
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
void saveEnv(int isRecursion, int regListSize) {
    struct Code *c;
    printMips("#保存环境");
    int offset = 1;

    //保存中间变量寄存器和局部变量寄存器
    int t;
    if (curCodeSp >= 1) {
        //说明存在参数是函数的情况
        for (t = 0; t < ((pushCount[curCodeSp - 1] > 4) ? 4 : pushCount[curCodeSp - 1]); t++) {
            //需要保存已经父函数写入的参数，保存$a
            printMips("sw $%d,%d($sp) #Save $a reg", t + $a0, -4 * offset++);
        }
    }
    for (t = 0; t < MIDREG; t++) {
        printMips("sw $%d,%d($sp) #Save midReg", t + $t0, -4 * offset++);
    }
    for (t = 0; t < regListSize; t++) {
        if (t + $t0 + MIDREG > $t9) {
            //[13,+]
            //printMips("lw $v1,head+%d #Save reg", (t + MIDREG + $t0 - $t9) * 4);
            //printMips("sw $v1,%d($sp)", -4 * offset++);
        } else {
            //[0,12]
            printMips("sw $%d,%d($sp) #Save reg", t + $t0 + MIDREG, -4 * offset++);
        }
    }
    /*
    if (isRecursion) {
        for (c = fhead->next; c != curCode[curCodeSp]; c = c->next) {
            if (c->type == Var) {
                struct Var *v = c->info;
                if (v->size != 0) {
                    //数组变量
                    int r;
                    for (r = 0; r < v->size; r++) {
                        printMips("lw $v1,%s+%d #Save %s[%d]", v->name, r * 4, v->name, r);
                        printMips("sw $v1,%d($sp)", -4 * offset++);
                    }
                }
            }
        }
    }
     */
    printMips("sw $fp,%d($sp) #Save $fp", -4 * offset++);
    printMips("sw $ra,%d($sp) #Save $ra", -4 * offset++);
    printMips("addi $sp,$sp,%d", -4 * (offset - 1));
}

/**
 * 恢复局部变量(运行环境)
 */
void revertEnv(int isRecursion, int regListSize) {
    struct Code *c;
    printMips("#恢复环境");
    int offset = 0;

    printMips("lw $ra,%d($sp) #Revert $ra", 4 * offset++);
    printMips("lw $fp,%d($sp) #Revert $fp", 4 * offset++);
    /*
    if (isRecursion) {
        for (c = curCode[curCodeSp]; c != fhead; c = c->prev) {
            if (c->type == Var) {
                struct Var *v = c->info;
                if (v->size != 0) {
                    //数组变量
                    int t;
                    for (t = v->size - 1; t >= 0; t--) {
                        printMips("lw $v1,%d($sp) #Revert %s[%d]", v->name, t * 4, 4 * offset++);
                        printMips("sw $v1,%s+%d", v->name, t * 4);
                    }
                }
            }
        }
    }
    */
    int t;
    for (t = regListSize - 1; t >= 0; t--) {
        if (t + $t0 + MIDREG > $t9) {
            //printMips("lw $v1,%d($sp) #Revert reg", 4 * offset++);
            //printMips("sw $v1,head+%d", (t + MIDREG + $t0 - $t9) * 4);
        } else {
            printMips("lw $%d,%d($sp) #Revert reg", t + $t0 + MIDREG, 4 * offset++);
        }
    }
    //恢复中间变量寄存器
    for (t = MIDREG - 1; t >= 0; t--) {
        printMips("lw $%d,%d($sp) #Revert midReg", t + $t0, 4 * offset++);
    }
    if (curCodeSp >= 1) {
        for (t = (pushCount[curCodeSp - 1] > 4) ? 3 : pushCount[curCodeSp - 1] - 1; t >= 0; t--) {
            //恢复a寄存器
            printMips("lw $%d,%d($sp) #Revert $a reg", t + $a0, 4 * offset++);
        }
    }
    printMips("addi $sp,$sp,%d", 4 * offset);
}

void allocateSpace(struct AlloSpa *as) {
    if (as->glo == 0) {
        printMips("move $fp,$sp #分配运行栈");
        printMips("addi $sp,$sp,%d", -delta * 4);
        while (arrListSize--) {
            delta += arrList[arrListSize].size;
            printMips("addi $sp,$sp,%d #数组[%d]", -4 * arrList[arrListSize].size, arrList[arrListSize].size);
            if (isRegGlo(arrList[arrListSize].reg)) {
                //头指针在全局变量中
                printf("panic in allocate space\n");
            } else if (isRegMem(arrList[arrListSize].reg)) {
                //数组头指针在额外寄存器中
                printMips("sw $sp,%d($fp)", 4 * arrList[arrListSize].reg);
            } else {
                printMips("move $%d,$sp", arrList[arrListSize].reg + $t0);
            }
        }
    } else {
        printMips("#分配全局区");
        while (arrListSize--) {
            printMips("addi $v0,$gp,%d #数组[%d]的地址", (gloRegMemMax + 1) * 4, arrList[arrListSize].size);
            printMips("sw $v0,%d($gp)", (arrList[arrListSize].reg - GLO) * 4);
            gloRegMemMax += arrList[arrListSize].size;
        }
    }
}
