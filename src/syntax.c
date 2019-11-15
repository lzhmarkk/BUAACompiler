#include "syntax.h"
#include "word.h"
#include "symbleTable.h"
#include "error.h"
#include "generate.h"

/**
 * 程序
 */
void programDef() {
    if (symbleList[wp] == CONSTTK) {
        constExpln();
    }
    if ((symbleList[wp] == INTTK || symbleList[wp] == CHARTK) &&
        symbleList[wp + 1] == IDENFR &&
        symbleList[wp + 2] != LPARENT) {
        varyExpln();
    }
    while (symbleList[wp + 1] != MAINTK) {
        if ((symbleList[wp] == INTTK || symbleList[wp] == CHARTK) &&
            symbleList[wp + 1] == IDENFR &&
            symbleList[wp + 2] == LPARENT) {
            level++;
            retFuncDef();
        } else if (symbleList[wp] == VOIDTK &&
                   symbleList[wp + 1] == IDENFR &&
                   symbleList[wp + 2] == LPARENT) {
            level++;
            unRetFuncDef();
        } else {
            break;
        }
    }
    level++;
    mainDef();
    printSyntax("<程序>");
}

/**
 * 字符
 */
int charDef() {
    assert(symbleList[wp], CHARCON);
    printWord();
    return tokenList[wp - 1][0];
}

/**
 * 字符串
 */
void stringDef() {
    assert(symbleList[wp], STRCON);
    printWord();
    printSyntax("<字符串>");
}

/**
 * 常量说明
 */
void constExpln() {
    while (symbleList[wp] == CONSTTK) {
        printWord();
        constDef();
        if (symbleList[wp] != SEMICN) {
            error(lines[wp - 1], MISS_SEMI);
        } else { printWord(); }
    }
    printSyntax("<常量说明>");
}

/**
 * 常量定义
 */
void constDef() {
    int r;
    if (symbleList[wp] == INTTK || symbleList[wp] == CHARTK) {
        int reg;
        enum Type t = symbleList[wp] == INTTK ? INT : symbleList[wp] == CHARTK ? CHAR : VOID;
        printWord();
        while (1) {
            assert(symbleList[wp], IDENFR);
            reg = idenDef();
            char *name = tokenList[wp - 1];
            if ((r = checkRedef(name, level)) != SUCCESS) {
                error(lines[wp - 1], r);
            } else {
                addToTable(name, CONST, t, level, 0);
            }
            assert(symbleList[wp], ASSIGN);
            printWord();

            int value;
            enum Type type;
            if (symbleList[wp] == INTCON || symbleList[wp] == PLUS || symbleList[wp] == MINU) {
                value = intDef();
                type = INT;
            } else if (symbleList[wp] == CHARCON) {
                value = charDef();
                type = CHAR;
            }
            emit(Const, 3, type, name, value);
            if (symbleList[wp] == COMMA) {
                printWord();
                continue;
            } else if (symbleList[wp] == SEMICN) {
                break;
            } else {
                error(lines[wp - 1], CONST_NOT_INTCHAR);
                nextLine(wp - 1);
            }
            break;
        }
    } else panic("constDef");
    printSyntax("<常量定义>");
}

/**
 * 整数
 */
int intDef() {
    int minus = 0;
    int ret;
    if (symbleList[wp] == PLUS || symbleList[wp] == MINU) {
        minus = symbleList[wp] == MINU;
        printWord();
    }
    ret = unsgIntDef();
    printSyntax("<整数>");
    return minus ? -ret : ret;
}

/**
 * 无符号整数
 */
int unsgIntDef() {
    assert(symbleList[wp], INTCON);
    printWord();
    printSyntax("<无符号整数>");
    return __str2int(tokenList[wp - 1]);
}

/**
 * 标识符
 */
int idenDef() {
    assert(symbleList[wp], IDENFR);
    printWord();
    return getReg(tokenList[wp - 1], level);
}

/**
 * 声明头部
 */
void explnheadDef() {
    int r;
    if (symbleList[wp] == INTTK || symbleList[wp] == CHARTK) {
        enum Type t = symbleList[wp] == INTTK ? INT : symbleList[wp] == CHARTK ? CHAR : VOID;
        printWord();
        idenDef();
        if ((r = checkRedef(tokenList[wp - 1], 0)) != SUCCESS) {
            error(lines[wp - 1], r);
        } else {
            addToTable(tokenList[wp - 1], FUNC, VOID, 0, 2, 666, t);
            emit(Label, 1, tokenList[wp - 1]);
        }
        retType = t;
    } else {
        panic("explnheadDef");
    }
    printSyntax("<声明头部>");
}

/**
 * 变量说明
 */
void varyExpln() {
    while ((symbleList[wp] == INTTK || symbleList[wp] == CHARTK) &&
           symbleList[wp + 1] == IDENFR && symbleList[wp + 2] != LPARENT) {
        varyDef();
        if (symbleList[wp] != SEMICN) {
            error(lines[wp - 1], MISS_SEMI);
        } else { printWord(); }
    }
    printSyntax("<变量说明>");
}

/**
 * 变量定义
 */
void varyDef() {
    int r;
    int reg;
    asserts(symbleList[wp], INTTK, CHARTK);
    enum Type t = symbleList[wp] == INTTK ? INT : symbleList[wp] == CHARTK ? CHAR : VOID;
    printWord();
    while (1) {
        int size = 0;
        reg = idenDef();
        char *name = tokenList[wp - 1];
        if (symbleList[wp] == LBRACK) {
            printWord();
            size = unsgIntDef();
            if (symbleList[wp] != RBRACK) {
                error(lines[wp], MISS_RBRACK);
            } else { printWord(); }
            if ((r = checkRedef(name, level)) != SUCCESS) {
                error(lines[wp - 1], r);
            } else {
                addToTable(name, VAR, ARRAY, level, 3, t, &tokenList[wp - 2], 0);
            }
        } else {
            if ((r = checkRedef(name, level)) != SUCCESS) {
                error(lines[wp - 1], r);
            } else {
                addToTable(name, VAR, t, level, 0);
            }
        }
        emit(Var, 3, t, name, size);
        if (symbleList[wp] == COMMA) {
            printWord();
            continue;
        } else if (symbleList[wp] == SEMICN) {
            break;
        } else {
            nextLine(wp - 1);
        }
        break;
    }
    printSyntax("<变量定义>");
}

/**
 * 有返回值函数定义
 */
void retFuncDef() {
    explnheadDef();
    checkRet = 1;
    hasRet = 0;
    assert(symbleList[wp], LPARENT);
    printWord();
    paraDef();
    if (symbleList[wp] != RPARENT) {
        error(lines[wp], MISS_RPARENT);
    } else { printWord(); }
    assert(symbleList[wp], LBRACE);
    printWord();
    mutiSentDef();
    assert(symbleList[wp], RBRACE);
    printWord();
    if (hasRet == 0) {
        error(lines[wp - 1], MISS_RET);
    }
    checkRet = 0;
    printSyntax("<有返回值函数定义>");
}

/**
 * 无返回值函数定义
 */
void unRetFuncDef() {
    int r;
    assert(symbleList[wp], VOIDTK);
    printWord();
    idenDef();
    char *name = tokenList[wp - 1];
    if ((r = checkRedef(name, 0)) != SUCCESS) {
        error(lines[wp - 1], r);
    } else {
        addToTable(name, FUNC, VOID, 0, 2, 888, VOID);
        emit(Label, 1, name);
    }
    retType = VOID;
    checkRet = 1;
    hasRet = 0;
    assert(symbleList[wp], LPARENT);
    printWord();
    paraDef();
    if (symbleList[wp] != RPARENT) {
        error(lines[wp - 1], MISS_RPARENT);
    } else { printWord(); }
    assert(symbleList[wp], LBRACE);
    printWord();
    mutiSentDef();
    assert(symbleList[wp], RBRACE);
    printWord();
    checkRet = 0;
    emit(Ret, 1, 0);
    printSyntax("<无返回值函数定义>");
}

/**
 * 复合语句
 */
void mutiSentDef() {
    if (symbleList[wp] == CONSTTK) {
        constExpln();
    }
    if (symbleList[wp] == INTTK || symbleList[wp] == CHARTK) {
        varyExpln();
    }
    sentsDef();
    printSyntax("<复合语句>");
}

/**
 * 参数表
 */
void paraDef() {
    int r;
    if (symbleList[wp] == INTTK || symbleList[wp] == CHARTK) {
        while (1) {
            asserts(symbleList[wp], INTTK, CHARTK);
            enum Type t = symbleList[wp] == INTTK ? INT : symbleList[wp] == CHARTK ? CHAR : VOID;
            printWord();
            idenDef();
            //如果参数出现重复，忽略该参数
            if ((r = checkRedef(tokenList[wp - 1], level)) != SUCCESS) {
                error(lines[wp - 1], r);
            } else {
                addToTable(tokenList[wp - 1], PARA, t, level, 0);
            }
            if (symbleList[wp] == COMMA) {
                printWord();
                continue;
            }
            break;
        }
    }
    printSyntax("<参数表>");
}

/**
 * 主函数
 */
void mainDef() {
    retType = VOID;
    checkRet = 1;
    hasRet = 0;
    assert(symbleList[wp], VOIDTK);
    printWord();
    assert(symbleList[wp], MAINTK);
    printWord();
    assert(symbleList[wp], LPARENT);
    printWord();
    if (symbleList[wp] != RPARENT) {
        error(lines[wp - 1], MISS_RPARENT);
    } else { printWord(); }
    assert(symbleList[wp], LBRACE);
    printWord();
    mutiSentDef();
    assert(symbleList[wp], RBRACE);
    printWord();
    checkRet = 0;
    printSyntax("<主函数>");
}

/**
 * 表达式
 */
int *expressDef() {
    enum Type ret = INT;
    int reg;
    if (symbleList[wp] == PLUS || symbleList[wp] == MINU) {
        int minus = symbleList[wp] == MINU;
        printWord();
        reg = itemDef()[1];
        if (minus) {
            int regC = newRegister();
            emit(Tuple, 4, MinuOp, 0, reg, regC);
            reg = regC;
        }
    } else {
        int *r = itemDef();
        ret = r[0];
        reg = r[1];
    }
    while (1) {
        if (symbleList[wp] == PLUS || symbleList[wp] == MINU) {
            int minus = symbleList[wp] == MINU;
            printWord();
            ret = INT;
            int regB = itemDef()[1];
            int regC = newRegister();
            emit(Tuple, 4, minus ? MinuOp : PlusOp, reg, regB, regC);
            reg = regC;
        } else {
            break;
        }
    }
    printSyntax("<表达式>");
    retArr[0] = ret;
    retArr[1] = reg;
    return retArr;
}

/**
 * 项
 */
int *itemDef() {
    enum Type ret;
    int reg;
    int *r = factorDef();
    ret = r[0];
    reg = r[1];
    while (1) {
        if (symbleList[wp] == MULT || symbleList[wp] == DIV) {
            int divu = symbleList[wp] == DIV;
            printWord();
            ret = INT;
            int regB = factorDef()[1];
            int regC = newRegister();
            emit(Tuple, 4, divu ? DiviOp : MultOp, reg, regB, regC);
            reg = regC;
        } else {
            break;
        }
    }
    printSyntax("<项>");
    retArr[0] = ret;
    retArr[1] = reg;
    return retArr;
}

/**
 * 因子
 */
int *factorDef() {
    int r;
    enum Type ret = INT;
    int reg = 0;
    if (symbleList[wp] == IDENFR && symbleList[wp + 1] != LPARENT) {
        reg = idenDef();
        char *name = tokenList[wp - 1];
        if ((r = checkExist(name, level)) != SUCCESS) {
            error(lines[wp - 1], r);
        }
        if (symbleList[wp] == LBRACK) {
            printWord();
            int *re = expressDef();
            if (re[0] != INT) {
                error(lines[wp - 1], OFFSET_NOT_INT);
            }
            reg = newRegister();
            emit(ArrL, 3, name, re[1], reg);
            if (symbleList[wp] != RBRACK) {
                error(lines[wp], MISS_RBRACK);
            } else { printWord(); }
        }
        ret = getType(name, level);
    } else if (symbleList[wp] == LPARENT) {
        printWord();
        reg = expressDef()[1];
        if (symbleList[wp] != RPARENT) {
            error(lines[wp - 1], MISS_RPARENT);
        } else { printWord(); }
    } else if (symbleList[wp] == INTCON || symbleList[wp] == PLUS || symbleList[wp] == MINU) {
        int value = intDef();
        reg = newRegister();
        emit(Eql, 2, value, reg);
    } else if (symbleList[wp] == CHARCON) {
        ret = CHAR;
        int value = charDef();
        reg = newRegister();
        emit(Eql, 2, value, reg);
    } else if (symbleList[wp] == IDENFR && symbleList[wp + 1] == LPARENT) {
        char *name = tokenList[wp];
        if ((r = checkExist(name, 0)) < 0) {
            error(lines[wp - 1], r);
            wp += 2;
            assignParaDef(tokenList[wp - 2]);
            wp++;
        } else {
            if (isRetFunc(name)) {
                reg = retFuncCallDef();
                ret = getType(name, 0);
            } else {
                error(lines[wp - 1], CALL_UNRET_FUNC);
                wp += 2;
                assignParaDef(tokenList[wp - 2]);
                wp++;
            }
        }
    } else {
        error(lines[wp], NOT_A_FACTOR);
    }
    printSyntax("<因子>");
    retArr[0] = ret;
    retArr[1] = reg;
    return retArr;
}

/**
 * 语句
 */
void sentDef() {
    if (symbleList[wp] == LBRACE) {
        printWord();
        sentsDef();
        assert(symbleList[wp], RBRACE);
        printWord();
    } else if (symbleList[wp] == SCANFTK) {
        readSentDef();
        if (symbleList[wp] != SEMICN) {
            error(lines[wp - 1], MISS_SEMI);
        } else { printWord(); }
    } else if (symbleList[wp] == WHILETK || symbleList[wp] == DOTK || symbleList[wp] == FORTK) {
        loopDef();
    } else if (symbleList[wp] == IFTK) {
        conditSentDef();
    } else if (symbleList[wp] == RETURNTK) {
        retDef();
        if (symbleList[wp] != SEMICN) {
            error(lines[wp - 1], MISS_SEMI);
        } else { printWord(); }
    } else if (symbleList[wp] == PRINTFTK) {
        writeSentDef();
        if (symbleList[wp] != SEMICN) {
            error(lines[wp - 1], MISS_SEMI);
        } else { printWord(); }
    } else if (symbleList[wp] == SEMICN) {
        printWord();
    } else if (symbleList[wp] == IDENFR) {
        if (symbleList[wp + 1] == ASSIGN || symbleList[wp + 1] == LBRACK) {
            assignSentDef();
            if (symbleList[wp] != SEMICN) {
                error(lines[wp - 1], MISS_SEMI);
            } else { printWord(); }
        } else if (symbleList[wp + 1] == LPARENT) {
            if (isRetFunc(tokenList[wp])) {
                retFuncCallDef();
                if (symbleList[wp] != SEMICN) {
                    error(lines[wp - 1], MISS_SEMI);
                } else { printWord(); }
            } else if (isUnRetFunc(tokenList[wp])) {
                unRetFuncCallDef();
                if (symbleList[wp] != SEMICN) {
                    error(lines[wp - 1], MISS_SEMI);
                } else { printWord(); }
            } else {
                error(lines[wp], NOTDEFINED);
                while (symbleList[wp] != SEMICN)wp++;
            }
        }
    } else panic("sentDef");
    printSyntax("<语句>");
}

/**
 * 赋值语句
 */
void assignSentDef() {
    int r;
    int toReg = idenDef();
    char *name = tokenList[wp - 1];
    if ((r = checkExist(name, level)) != SUCCESS) {
        error(lines[wp - 1], r);
    } else if (isConst(name, level)) {
        error(lines[wp - 1], CANT_CHANGE_CONST);
    }
    int fromReg;
    if (symbleList[wp] == ASSIGN) {
        printWord();
        fromReg = expressDef()[1];
        emit(Assig, 2, fromReg, toReg);
    } else if (symbleList[wp] == LBRACK) {
        printWord();
        int *re = expressDef();
        int offset = re[1];
        if (re[0] != INT) {
            error(lines[wp - 1], OFFSET_NOT_INT);
        }
        if (symbleList[wp] != RBRACK) {
            error(lines[wp], MISS_RBRACK);
        } else { printWord(); }
        assert(symbleList[wp], ASSIGN);
        printWord();
        fromReg = expressDef()[1];
        emit(ArrS, 3, name, offset, fromReg);
    } else panic("assignSentDef");
    printSyntax("<赋值语句>");
}

/**
 * 条件语句
 */
void conditSentDef() {
    assert(symbleList[wp], IFTK);
    printWord();
    assert(symbleList[wp], LPARENT);
    printWord();
    conditDef();//跳转位置尚未填写,存在branchP
    if (symbleList[wp] != RPARENT) {
        error(lines[wp - 1], MISS_RPARENT);
    } else { printWord(); }
    sentDef();
    if (symbleList[wp] == ELSETK) {
        printWord();
        //if结束，跳到最后；跳转位置尚未填写,存在ifP
        ifP = emit(Goto, 1, NULL);
        labelP1 = emit(Label, 1, genLabel());//if结束END1标签
        sentDef();
        labelP2 = emit(Label, 1, genLabel());//else结束END2标签
        ((struct Goto *) ifP->info)->label = labelP2->info;//回填END2至if中
    } else {
        labelP1 = emit(Label, 1, genLabel());//if结束END1标签
    }
    ((struct Bra *) branchP->info)->label = labelP1->info;//回填END1至if中
    printSyntax("<条件语句>");
}

/**
 * 条件
 */
void conditDef() {
    int regA, regB;
    int *r1 = expressDef();
    regA = r1[1];
    if (r1[0] != INT) {
        error(lines[wp - 1], CONDIT_ILLEGAL);
    }
    if (symbleList[wp] == LSS || symbleList[wp] == LEQ || symbleList[wp] == GRE || symbleList[wp] == GEQ ||
        symbleList[wp] == EQL || symbleList[wp] == NEQ) {
        enum SYMBLE op = symbleList[wp];
        printWord();
        int *r2 = expressDef();
        regB = r2[1];
        if (r2[0] != INT) {
            error(lines[wp - 1], CONDIT_ILLEGAL);
        }
        int reg = newRegister();
        emit(Tuple, 4, MinuOp, regA, regB, reg);
        switch (op) {
            //这里都是反的，因为考虑到大部分都是“满足则执行”，而branch逻辑是“满足即跳转”
            case LSS:
                branchP = emit(Bra, 3, reg, reverse(BLT), NULL);
                break;
            case LEQ:
                branchP = emit(Bra, 3, reg, reverse(BLE), NULL);
                break;
            case GRE:
                branchP = emit(Bra, 3, reg, reverse(BGT), NULL);
                break;
            case GEQ:
                branchP = emit(Bra, 3, reg, reverse(BGE), NULL);
                break;
            case EQL:
                branchP = emit(Bra, 3, reg, reverse(BEQ), NULL);
                break;
            case NEQ:
                branchP = emit(Bra, 3, reg, reverse(BNE), NULL);
                break;
            default:
                break;
        }
    } else {
        branchP = emit(Bra, 3, regA, BEQ, NULL);
    }
    printSyntax("<条件>");
}

/**
 * 循环语句
 */
void loopDef() {
    int r;
    if (symbleList[wp] == WHILETK) {
        printWord();
        assert(symbleList[wp], LPARENT);
        printWord();
        labelP1 = emit(Label, 1, genLabel());//创建循环头LOOP标签
        conditDef();//跳转位置尚未填写
        if (symbleList[wp] != RPARENT) {
            error(lines[wp - 1], MISS_RPARENT);
        } else { printWord(); }
        sentDef();
        emit(Goto, 1, labelP1->info);//创建循环跳转，跳到LOOP
        labelP2 = emit(Label, 1, genLabel());//创建循环尾END标签;
        ((struct Bra *) branchP->info)->label = labelP2->info;//回填END至beq中
    } else if (symbleList[wp] == DOTK) {
        printWord();
        labelP1 = emit(Label, 1, genLabel());//创建循环头LOOP标签
        sentDef();
        if (symbleList[wp] != WHILETK) {
            error(lines[wp], MISS_WHILE);
        } else {
            printWord();
        }
        if (symbleList[wp] == LPARENT) {
            assert(symbleList[wp], LPARENT);
            printWord();
            conditDef();//跳转位置尚未填写
            if (symbleList[wp] != RPARENT) {
                error(lines[wp - 1], MISS_RPARENT);
            } else { printWord(); }
            struct Bra *b = branchP->info;
            b->label = labelP1->info;//回填跳转位置到LOOP
            b->type = reverse(b->type);//因为do-while是“满足则跳转”，所以要再反一波
        }
    } else if (symbleList[wp] == FORTK) {
        printWord();
        assert(symbleList[wp], LPARENT);
        printWord();
        int toReg = idenDef();
        char *name = tokenList[wp - 1];
        if ((r = checkExist(name, level)) != SUCCESS) {
            error(lines[wp - 1], r);
        } else if (isConst(name, level)) {
            error(lines[wp - 1], CANT_CHANGE_CONST);
        }
        assert(symbleList[wp], ASSIGN);
        printWord();
        int fromReg = expressDef()[1];
        emit(Assig, 2, fromReg, toReg);//赋值
        if (symbleList[wp] != SEMICN) {
            error(lines[wp - 1], MISS_SEMI);
        } else { printWord(); }
        labelP1 = emit(Label, 1, genLabel());//循环头LOOP标签
        conditDef();//跳转位置尚未填写
        if (symbleList[wp] != SEMICN) {
            error(lines[wp - 1], MISS_SEMI);
        } else { printWord(); }
        int regC = idenDef();
        if ((r = checkExist(tokenList[wp - 1], level)) != SUCCESS) {
            error(lines[wp - 1], r);
        } else if (isConst(tokenList[wp - 1], level)) {
            error(lines[wp - 1], CANT_CHANGE_CONST);
        }
        assert(symbleList[wp], ASSIGN);
        printWord();
        int regA = idenDef();
        if ((r = checkExist(tokenList[wp - 1], level)) != SUCCESS) {
            error(lines[wp - 1], r);
        }
        asserts(symbleList[wp], PLUS, MINU);
        int minus = symbleList[wp] == MINU;
        printWord();
        int value = stepDef();
        if (symbleList[wp] != RPARENT) {
            error(lines[wp - 1], MISS_RPARENT);
        } else { printWord(); }
        sentDef();
        int regB = newRegister();
        emit(Eql, 2, value, regB);
        emit(Tuple, 4, minus ? MinuOp : PlusOp, regA, regB, regC);
        emit(Goto, 1, labelP1->info);//跳转到函数头
        labelP2 = emit(Label, 1, genLabel());//函数尾END
        ((struct Bra *) branchP->info)->label = labelP2->info;//回填跳转位置到END
    } else panic("loopDef");
    printSyntax("<循环语句>");
}

/**
 * 步长
 */
int stepDef() {
    int ret = unsgIntDef();
    printSyntax("<步长>");
    return ret;
}

/**
 * 有返回值函数调用语句
 */
int retFuncCallDef() {
    int r;
    idenDef();
    char *name = tokenList[wp - 1];
    if ((r = checkExist(name, 0)) != SUCCESS) {
        error(lines[wp - 1], r);
    }
    assert(symbleList[wp], LPARENT);
    printWord();
    assignParaDef(tokenList[wp - 2]);
    if (symbleList[wp] != RPARENT) {
        error(lines[wp - 1], MISS_RPARENT);
    } else { printWord(); }
    int reg = newRegister();
    emit(Call, 1, getLabel(name)->info);
    emit(ReadRet, 1, reg);
    printSyntax("<有返回值函数调用语句>");
    return reg;
}

/**
 * 无返回值函数调用语句
 */
void unRetFuncCallDef() {
    int r;
    idenDef();
    char *name = tokenList[wp - 1];
    if ((r = checkExist(name, level)) != SUCCESS) {
        error(lines[wp - 1], r);
    }
    assert(symbleList[wp], LPARENT);
    printWord();
    assignParaDef(tokenList[wp - 2]);
    if (symbleList[wp] != RPARENT) {
        error(lines[wp - 1], MISS_RPARENT);
    } else { printWord(); }
    emit(Call, 1, getLabel(name)->info);
    printSyntax("<无返回值函数调用语句>");
}

/**
 * 值参数表
 */
void assignParaDef(char *fname) {
    int paraIndex = 0;
    if (symbleList[wp] != RPARENT && symbleList[wp] != SEMICN) {
        while (1) {
            int *re = expressDef();
            enum Type type = re[0];
            int reg = re[1];
            int r;
            paraIndex++;
            if ((r = checkParaType(fname, paraIndex, type)) != SUCCESS) {
                error(lines[wp - 1], r);
            }
            emit(Push, 1, reg);
            if (symbleList[wp] == COMMA) {
                printWord();
                continue;
            }
            break;
        }
    }
    int r;
    if ((r = checkParaSize(fname, paraIndex)) != SUCCESS) {
        error(lines[wp], r);
    }
    printSyntax("<值参数表>");
}

/**
 * 语句列
 */
void sentsDef() {
    while (1) {
        if (symbleList[wp] == RBRACE) {
            break;
        }
        sentDef();
    }
    printSyntax("<语句列>");
}

/**
 * 读语句
 */
void readSentDef() {
    int r;
    assert(symbleList[wp], SCANFTK);
    printWord();
    assert(symbleList[wp], LPARENT);
    printWord();
    while (1) {
        int reg = idenDef();
        if ((r = checkExist(tokenList[wp - 1], level)) != SUCCESS) {
            error(lines[wp - 1], r);
        }
        emit(Read, 1, reg);
        if (symbleList[wp] == COMMA) {
            printWord();
            continue;
        }
        break;
    }
    if (symbleList[wp] != RPARENT) {
        error(lines[wp - 1], MISS_RPARENT);
    } else { printWord(); }
    printSyntax("<读语句>");
}

/**
 * 写语句
 */
void writeSentDef() {
    int reg;
    assert(symbleList[wp], PRINTFTK);
    printWord();
    assert(symbleList[wp], LPARENT);
    printWord();
    if (symbleList[wp] == STRCON) {
        stringDef();
        emit(Write, 3, 0, tokenList[wp - 1], -1);
        if (symbleList[wp] == COMMA) {
            printWord();
            reg = expressDef()[1];
            emit(Write, 3, 1, NULL, reg);
        }
    } else {
        reg = expressDef()[1];
        emit(Write, 3, 1, NULL, reg);
    }
    if (symbleList[wp] != RPARENT) {
        error(lines[wp - 1], MISS_RPARENT);
    } else { printWord(); }
    printSyntax("<写语句>");
}

/**
 * 返回语句
 */
void retDef() {
    assert(symbleList[wp], RETURNTK);
    printWord();
    hasRet = 1;
    if (symbleList[wp] == LPARENT) {
        printWord();
        int *r = expressDef();
        enum Type ret = r[0];
        if (checkRet) {
            if (retType == VOID) {
                error(lines[wp], FORBID_RET);
            } else if (ret != retType) {
                error(lines[wp], MISMATCH_RET);
            }
        }
        emit(Ret, 1, r[1]);
        if (symbleList[wp] != RPARENT) {
            error(lines[wp - 1], MISS_RPARENT);
        } else { printWord(); }
    } else if (checkRet && retType != VOID) {
        error(lines[wp], MISMATCH_RET);
    }
    printSyntax("<返回语句>");
}

int panic(char *msg) {
    printf("%d:panic at %s\n", wp, msg);
    while (1);
}

void assert(enum SYMBLE a, enum SYMBLE b) {
    if (a == b) {
        return;
    } else {
        panic("assert failed");
    }
}

void asserts(enum SYMBLE a, enum SYMBLE b1, enum SYMBLE b2) {
    if (a == b1 || a == b2) {
        return;
    } else {
        panic("assert failed");
    }
}

void nextLine(int p) {
    int curLine = lines[p];
    for (; lines[p] == curLine; p++) {
        if (symbleList[p] == CONSTTK || symbleList[p] == INTTK || symbleList[p] == CHARTK || symbleList[p] == VOIDTK ||
            symbleList[p] == IFTK || symbleList[p] == DOTK || symbleList[p] == WHILETK || symbleList[p] == FORTK ||
            symbleList[p] == SCANFTK ||
            symbleList[p] == PRINTFTK || symbleList[p] == RETURNTK || symbleList[p] == LBRACE ||
            symbleList[p] == SEMICN) {
            break;
        }
    }
    wp = p;
}