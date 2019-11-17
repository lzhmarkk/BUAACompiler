#include "generate.h"
#include "string.h"

#define COMMENT 1
int tParaCount;

void genMips() {
    hasEntry = 0;
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
                tParaCount = paraCount;
                break;
            }
            case Push:
                genPush(p->info);
                break;
            case Call:
                genCall(p->info);
                break;
            case Para:
                genPara(p->info);
                break;
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
            case Eql:
                genEql(p->info);
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
    if (strcmp(p->name, "main") != 0) {
        //若不是main
        if (COMMENT)printMips("#保存环境");
        //printMips("sw $sp,4($sp)");
        printMips("addi $sp,$sp,-4");
        printMips("sw $ra,0($sp)");
    }
}

/**
 * 生成参数入栈
 */
// todo:保存本函数涉及到的模拟寄存器的值
void genPush(struct Push *p) {
    if (COMMENT)printMips("#参数入栈");
    printMips("lw $t0,%d($s0)", p->reg * 4);
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
void genPara(struct Para *p) {
    if (COMMENT)printMips("#参数%d出栈", tParaCount);
    printMips("lw $t0,%d($sp)", (tParaCount - 1) * 4 + 4);
    printMips("sw $t0,%d($s0)", p->reg * 4);
    tParaCount--;
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
    if (COMMENT)printMips("#sp,ra出栈,并移除参数区");
    printMips("lw $ra,0($sp)");
    //printMips("lw $sp,4($sp)");
    printMips("addi $sp,$sp,4");
    printMips("addi $sp,$sp,%d", paraCount * 4);
    if (COMMENT)printMips("#返回语句");
    if (p->reg != 0)printMips("lw $v0,%d($s0)", p->reg * 4);
    printMips("jr $ra");
    printMips("nop");
    paraCount = 0;
}

/**
 * 生成变量定义（主要是分配数组空间）
 */
void genVar(struct Var *p) {
    if (p->size != 0) {
        //说明是一个数组
        strcpy(arrLabList[arrayCount], getArrLab(p->reg));
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
    printMips("lw $t0,%d($s0)", p->regA * 4);
    printMips("lw $t1,%d($s0)", p->regB * 4);
    switch (p->op) {
        case PlusOp: {
            if (COMMENT)printMips("#加法");
            printMips("add $t2,$t0,$t1");
            break;
        }
        case MinuOp: {
            if (COMMENT)printMips("#减法");
            printMips("sub $t2,$t0,$t1");
            break;
        }
        case MultOp: {
            if (COMMENT)printMips("#乘法");
            printMips("mult $t0,$t1");
            printMips("mflo $t2");
            break;
        }
        case DiviOp: {
            if (COMMENT)printMips("#除法");
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
    if (COMMENT)printMips("#复制");
    printMips("lw $t0,%d($s0)", p->from * 4);
    printMips("sw $t0,%d($s0)", p->to * 4);
}

/**
 * 生成赋值语句（立即数到模拟寄存器）
 */
void genEql(struct Eql *p) {
    if (COMMENT)printMips("#赋值");
    printMips("li $t0,%d", p->value);
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
    printMips("la $t0,%s", getArrLab(p->reg));
    printMips("lw $t1,%d($s0)", p->offset * 4);
    printMips("sll $t1,$t1,2");
    printMips("add $t2,$t0,$t1");
    printMips("lw $t3,0($t2)");
    printMips("sw $t3,%d($s0)", p->to * 4);
}

/**
 * 生成写数组
 */
void genArrS(struct ArrS *p) {
    if (COMMENT)printMips("#写数组");
    printMips("la $t0,%s", getArrLab(p->reg));
    printMips("lw $t1,%d($s0)", p->offset * 4);
    printMips("sll $t1,$t1,2");
    printMips("add $t2,$t0,$t1");
    printMips("lw $t3,%d($s0)", p->from * 4);
    printMips("sw $t3,0($t2)");
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
    if (p->string == NULL && p->reg != -1) {
        if (COMMENT)printMips("#写语句%s", p->type == INT ? "Int" : "Char");
        printMips("li $v0,%d", p->type == INT ? 1 : 11);
        printMips("lw $a0,%d($s0)", p->reg * 4);
        printMips("syscall");
    } else if (p->string != NULL && p->reg == -1) {
        if (COMMENT)printMips("#写语句String");
        printMips("li $v0,4");
        printMips("la $a0,%s", getStrLab(p->string));
        printMips("syscall");
    } else {
        if (COMMENT)printMips("#写语句String %s", p->type == INT ? "Int" : "Char");
        printMips("li $v0,4");
        printMips("la $a0,%s", getStrLab(p->string));
        printMips("syscall");
        printMips("li $v0,%d", p->type == INT ? 1 : 11);
        printMips("lw $a0,%d($s0)", p->reg * 4);
        printMips("syscall");
    }
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
        printMips("    %s:.ascii \"%s\\0\"", strLabel[strCount], strList[strCount]);
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

    strcpy(strList[strCount], str);
    return strLabel[strCount++];
}

/**
 * 返回该数组的标签
 */
char *getArrLab(int reg) {
    arrayLabel[0] = 'a';
    arrayLabel[1] = 'r';
    arrayLabel[2] = 'r';
    arrayLabel[3] = (char) (reg / 100 + '0');
    arrayLabel[4] = (char) ((reg / 10) % 10 + '0');
    arrayLabel[5] = (char) (reg % 10 + '0');
    arrayLabel[6] = '\0';

    return arrayLabel;
}
//todo 优化Tuple类型和Assig类型，删除掉Eql类
//todo 尽量使用寄存器