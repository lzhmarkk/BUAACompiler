#include "syntax.h"
#include "word.h"
#include "symbleTable.h"
#include "error.h"
#include "mid.h"
#include "string.h"
#include "reg.h"

char *curFunc;

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
        clearReg();
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
        enum Type t = symbleList[wp] == INTTK ? INT : symbleList[wp] == CHARTK ? CHAR : VOID;
        printWord();
        while (1) {
            assert(symbleList[wp], IDENFR);
            idenDef();
            char *name = tokenList[wp - 1];
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
            if ((r = checkRedef(name, level)) != SUCCESS) {
                error(lines[wp - 1], r);
            } else {
                addToTable(name, CONST, t, level, 1, value);
            }
            emit(Const, 4, type, name, getReg(name, level), value);
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
        char *name = tokenList[wp - 1];
        if ((r = checkRedef(name, 0)) != SUCCESS) {
            error(lines[wp - 1], r);
        } else {
            addToTable(name, FUNC, VOID, 0, 2, 666, t);
            emit(Func, 1, name);
        }
        retType = t;
        curFunc = name;
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
    asserts(symbleList[wp], INTTK, CHARTK);
    enum Type t = symbleList[wp] == INTTK ? INT : symbleList[wp] == CHARTK ? CHAR : VOID;
    printWord();
    while (1) {
        int size = 0;
        idenDef();
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
        if (size == 0) {
            emit(Var, 4, t, name, getReg(name, level), size);
        } else {
            emit(Var, 4, t, getArrLabel(name, level), getReg(name, level), size);
        }
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
    curFunc = NULL;
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
        emit(Func, 1, name);
    }
    retType = VOID;
    curFunc = name;
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
    emit(Ret, 2, 0, facReg);//象征性返回0寄存器
    curFunc = NULL;
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
                emit(Para, 1, getReg(tokenList[wp - 1], level));
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
    addToTable("Main", FUNC, VOID, 0, 2, 0, VOID);
    emit(Func, 1, "Main");
    curFunc = "Main";

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
//todo:-4识别为-4而不是0-4
int *expressDef() {
    enum Type ret = INT;
    int valueA;
    enum factorKind factorRetA;
    if (symbleList[wp] == PLUS || symbleList[wp] == MINU) {
        //第一项前有+ -
        int minus = symbleList[wp] == MINU;
        printWord();
        int *r = itemDef();
        valueA = r[1];
        factorRetA = r[2];
        if (minus) {
            if (factorRetA == facReg && isRegTmp(valueA)) {
                clearRegTmp(valueA);
            }
            int regC = alloRegTmp();
            emit(Tuple, 6, MinuOp, 0, facInt, valueA, factorRetA, regC);
            valueA = regC;
            factorRetA = facReg;
        }
    } else {
        //第一项前没+ -
        int *r = itemDef();
        ret = r[0];
        valueA = r[1];
        factorRetA = r[2];
    }
    while (1) {
        if (symbleList[wp] == PLUS || symbleList[wp] == MINU) {
            int minus = symbleList[wp] == MINU;
            printWord();
            ret = INT;
            int *r = itemDef();
            int valueB = r[1];
            enum factorKind factorRetB = r[2];
            if (factorRetA == facReg && isRegTmp(valueA)) {
                clearRegTmp(valueA);
            }
            if (factorRetB == facReg && isRegTmp(valueB)) {
                clearRegTmp(valueB);
            }
            int regC = alloRegTmp();
            emit(Tuple, 6, minus ? MinuOp : PlusOp, valueA, factorRetA, valueB, factorRetB, regC);
            valueA = regC;
            factorRetA = facReg;
        } else {
            break;
        }
    }
    printSyntax("<表达式>");
    retArr[0] = ret;
    retArr[1] = valueA;
    retArr[2] = factorRetA;
    return retArr;
}

/**
 * 项
 */
int *itemDef() {
    enum Type ret;
    int *r = factorDef();
    ret = r[0];
    int valueA = r[1];
    enum factorKind factorRetA = r[2];
    while (1) {
        if (symbleList[wp] == MULT || symbleList[wp] == DIV) {
            int divu = symbleList[wp] == DIV;
            printWord();
            ret = INT;
            int *re = factorDef();
            int valueB = re[1];
            enum factorKind factorRetB = re[2];
            if (factorRetA == facReg && isRegTmp(valueA)) {
                clearRegTmp(valueA);
            }
            if (factorRetB == facReg && isRegTmp(valueB)) {
                clearRegTmp(valueB);
            }
            int regC = alloRegTmp();
            emit(Tuple, 6, divu ? DiviOp : MultOp, valueA, factorRetA, valueB, factorRetB, regC);
            valueA = regC;
            factorRetA = facReg;
        } else {
            break;
        }
    }
    printSyntax("<项>");
    retArr[0] = ret;
    retArr[1] = valueA;
    retArr[2] = factorRetA;
    return retArr;
}

/**
 * 因子
 */
int *factorDef() {
    int r;
    enum Type ret = INT;
    int value = 0;
    enum factorKind factorRet = facReg;
    if (symbleList[wp] == IDENFR && symbleList[wp + 1] != LPARENT) {
        value = idenDef();
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
            if (re[2] == facReg && isRegTmp(re[1])) {
                clearRegTmp(re[1]);
            }
            value = alloRegTmp();
            emit(ArrL, 4, getArrLabel(name, level), re[1], re[2], value);
            if (symbleList[wp] != RBRACK) {
                error(lines[wp], MISS_RBRACK);
            } else { printWord(); }
        }
        ret = getType(name, level);
        if (isConst(name, level)) {
            value = getConstValue(name, level);
            factorRet = ret == CHAR ? facChar : facInt;
        } else {
            factorRet = facReg;
        }
    } else if (symbleList[wp] == LPARENT) {
        printWord();
        int *re = expressDef();
        value = re[1];
        factorRet = re[2];
        if (symbleList[wp] != RPARENT) {
            error(lines[wp - 1], MISS_RPARENT);
        } else { printWord(); }
    } else if (symbleList[wp] == INTCON || symbleList[wp] == PLUS || symbleList[wp] == MINU) {
        value = intDef();
        factorRet = facInt;
    } else if (symbleList[wp] == CHARCON) {
        ret = CHAR;
        value = charDef();
        factorRet = facChar;
    } else if (symbleList[wp] == IDENFR && symbleList[wp + 1] == LPARENT) {
        char *name = tokenList[wp];
        if ((r = checkExist(name, 0)) < 0) {
            error(lines[wp - 1], r);
            wp += 2;
            assignParaDef(tokenList[wp - 2]);
            wp++;
        } else {
            if (isRetFunc(name)) {
                value = retFuncCallDef();
                ret = getType(name, 0);
            } else {
                error(lines[wp - 1], CALL_UNRET_FUNC);
                wp += 2;
                assignParaDef(tokenList[wp - 2]);
                wp++;
            }
        }
        factorRet = facReg;
    } else {
        error(lines[wp], NOT_A_FACTOR);
    }
    printSyntax("<因子>");
    retArr[0] = ret;
    retArr[1] = value;
    retArr[2] = factorRet;
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
    if (symbleList[wp] == ASSIGN) {
        printWord();
        int *re = expressDef();
        int fromValue = re[1];
        enum factorKind fromKind = re[2];
        if (fromKind == facReg && isRegTmp(fromValue)) {
            clearRegTmp(fromValue);
        }
        emit(Assig, 3, fromValue, fromKind, toReg);
    } else if (symbleList[wp] == LBRACK) {
        printWord();
        int *re = expressDef();
        int offset = re[1];
        enum factorKind offKind = re[2];
        if (re[0] != INT) {
            error(lines[wp - 1], OFFSET_NOT_INT);
        }
        if (symbleList[wp] != RBRACK) {
            error(lines[wp], MISS_RBRACK);
        } else { printWord(); }
        assert(symbleList[wp], ASSIGN);
        printWord();
        re = expressDef();
        int fromValue = re[1];
        enum factorKind fromKind = re[2];
        if (offKind == facReg && isRegTmp(offset)) {
            clearRegTmp(offset);
        }
        if (fromKind == facReg && isRegTmp(fromValue)) {
            clearRegTmp(fromValue);
        }
        emit(ArrS, 5, getArrLabel(name, level), offset, offKind, fromValue, fromKind);
    } else panic("assignSentDef");
    printSyntax("<赋值语句>");
}

/**
 * 条件语句
 */
void conditSentDef() {
    struct Code *branchP, *labelP1, *labelP2, *ifP;
    assert(symbleList[wp], IFTK);
    printWord();
    assert(symbleList[wp], LPARENT);
    printWord();
    branchP = conditDef();//跳转位置尚未填写,存在branchP中
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
void *conditDef() {
    struct Code *branchP = NULL;
    int valueA, valueB;
    enum factorKind valueKindA, valueKindB;
    int *r1 = expressDef();
    valueA = r1[1];
    valueKindA = r1[2];
    if (r1[0] != INT) {
        error(lines[wp - 1], CONDIT_ILLEGAL);
    }
    if (symbleList[wp] == LSS || symbleList[wp] == LEQ || symbleList[wp] == GRE || symbleList[wp] == GEQ ||
        symbleList[wp] == EQL || symbleList[wp] == NEQ) {
        enum SYMBLE op = symbleList[wp];
        printWord();
        int *r2 = expressDef();
        valueB = r2[1];
        valueKindB = r2[2];
        if (r2[0] != INT) {
            error(lines[wp - 1], CONDIT_ILLEGAL);
        }
        if (valueKindA == facReg && isRegTmp(valueA)) {
            clearRegTmp(valueA);
        }
        if (valueKindB == facReg && isRegTmp(valueB)) {
            clearRegTmp(valueB);
        }
        int regC = alloRegTmp();
        emit(Tuple, 6, MinuOp, valueA, valueKindA, valueB, valueKindB, regC);
        switch (op) {
            //这里都是反的，因为考虑到大部分都是“满足则执行”，而branch逻辑是“满足即跳转”
            case LSS:
                branchP = emit(Bra, 3, regC, reverse(BLT), NULL);
                break;
            case LEQ:
                branchP = emit(Bra, 3, regC, reverse(BLE), NULL);
                break;
            case GRE:
                branchP = emit(Bra, 3, regC, reverse(BGT), NULL);
                break;
            case GEQ:
                branchP = emit(Bra, 3, regC, reverse(BGE), NULL);
                break;
            case EQL:
                branchP = emit(Bra, 3, regC, reverse(BEQ), NULL);
                break;
            case NEQ:
                branchP = emit(Bra, 3, regC, reverse(BNE), NULL);
                break;
            default:
                break;
        }
        if (isRegTmp(regC)) {
            clearRegTmp(regC);
        }
    } else {
        if (valueKindA != facReg) {
            int regC = alloRegTmp();
            emit(Assig, 3, valueA, valueKindA, regC);
            valueA = regC;
        }
        branchP = emit(Bra, 3, valueA, BEQ, NULL);
        if (isRegTmp(valueA)) {
            clearRegTmp(valueA);
        }
    }
    printSyntax("<条件>");
    return branchP;
}

/**
 * 循环语句
 */
void loopDef() {
    int r;
    struct Code *branchP, *labelP1, *labelP2;
    if (symbleList[wp] == WHILETK) {
        printWord();
        assert(symbleList[wp], LPARENT);
        printWord();
        labelP1 = emit(Label, 1, genLabel());//创建循环头LOOP标签
        branchP = conditDef();//跳转位置尚未填写
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
            branchP = conditDef();//跳转位置尚未填写
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
        int *re = expressDef();
        int fromValue = re[1];
        enum factorKind fromKind = re[2];
        if (fromKind == facReg && isRegTmp(fromValue)) {
            clearRegTmp(fromValue);
        }
        emit(Assig, 3, fromValue, fromKind, toReg);//赋值
        if (symbleList[wp] != SEMICN) {
            error(lines[wp - 1], MISS_SEMI);
        } else { printWord(); }
        labelP1 = emit(Label, 1, genLabel());//循环头LOOP标签
        branchP = conditDef();//跳转位置尚未填写
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
        int step = stepDef();
        if (symbleList[wp] != RPARENT) {
            error(lines[wp - 1], MISS_RPARENT);
        } else { printWord(); }
        sentDef();
        if (isRegTmp(regA)) {
            clearRegTmp(regA);
        }
        emit(Tuple, 6, minus ? MinuOp : PlusOp, regA, facReg, step, facInt, regC);
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
    emit(SavEnv, 2, curFunc, name);
    assignParaDef(tokenList[wp - 2]);
    if (symbleList[wp] != RPARENT) {
        error(lines[wp - 1], MISS_RPARENT);
    } else { printWord(); }
    int reg = alloRegTmp();
    emit(Call, 1, getLabel(name)->info);
    emit(RevEnv, 2, curFunc, name);
    emit(ReadRet, 1, reg);//todo：有时候不用get return
    //todo:有返回值函数假如结果没有收录，请删除临时寄存器
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
    emit(SavEnv, 2, curFunc, name);
    assignParaDef(tokenList[wp - 2]);
    if (symbleList[wp] != RPARENT) {
        error(lines[wp - 1], MISS_RPARENT);
    } else { printWord(); }
    emit(Call, 1, getLabel(name)->info);
    emit(RevEnv, 2, curFunc, name);
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
            int value = re[1];
            enum factorKind kind = re[2];
            int r;
            paraIndex++;
            if ((r = checkParaType(fname, paraIndex, type)) != SUCCESS) {
                error(lines[wp - 1], r);
            }
            if (kind == facReg && isRegTmp(value)) {
                clearRegTmp(value);
            }
            emit(Push, 2, value, kind);
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
        emit(Read, 2, reg, getType(tokenList[wp - 1], level));
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
    assert(symbleList[wp], PRINTFTK);
    printWord();
    assert(symbleList[wp], LPARENT);
    printWord();
    if (symbleList[wp] == STRCON) {
        stringDef();
        char *str = tokenList[wp - 1];
        if (symbleList[wp] == COMMA) {
            printWord();
            int *re = expressDef();
            enum Type t = re[0];
            int value = re[1];
            if (re[2] == facReg) {
                if (isRegTmp(value)) {
                    clearRegTmp(value);
                }
                emit(Write, 4, STR_REG, str, value, t);
            } else {
                emit(Write, 4, STR_VALUE, str, value, t);
            }
        } else {
            emit(Write, 4, STR_ONLY, str, 0, 0);
        }
    } else {
        int *re = expressDef();
        enum Type t = re[0];
        int value = re[1];
        if (re[2] == facReg) {
            if (isRegTmp(value)) {
                clearRegTmp(value);
            }
            emit(Write, 4, REG_ONLY, NULL, value, t);
        } else {
            emit(Write, 4, VALUE_ONLY, NULL, value, t);
        }
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
        int value = r[1];
        enum factorKind kind = r[2];
        if (checkRet) {
            if (retType == VOID) {
                error(lines[wp], FORBID_RET);
            } else if (ret != retType) {
                error(lines[wp], MISMATCH_RET);
            }
        }
        if (kind == facReg && isRegTmp(value)) {
            clearRegTmp(value);
        }
        emit(Ret, 2, value, kind);
        if (symbleList[wp] != RPARENT) {
            error(lines[wp - 1], MISS_RPARENT);
        } else { printWord(); }
    } else if (checkRet && retType != VOID) {
        error(lines[wp], MISMATCH_RET);
    } else {
        emit(Ret, 2, 0, facReg);
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