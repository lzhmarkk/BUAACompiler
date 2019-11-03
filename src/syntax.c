#include "syntax.h"
#include "word.h"
#include "symbleTable.h"
#include "error.h"

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

//todo check discuss
/**
 * 字符
 */
void charDef() {
    assert(symbleList[wp], CHARCON);
    printWord();
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
            if (symbleList[wp] == IDENFR) {
                idenDef();
                if ((r = checkRedef(tokenList[wp - 1], level)) != SUCCESS) {
                    error(lines[wp - 1], r);
                } else {
                    addToTable(tokenList[wp - 1], CONST, t, level, 0);
                }
            } else panic("conDef");
            assert(symbleList[wp], ASSIGN);
            printWord();

            if (symbleList[wp] == INTCON || symbleList[wp] == PLUS || symbleList[wp] == MINU) {
                intDef();
            } else if (symbleList[wp] == CHARCON) {
                charDef();
            }
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
void intDef() {
    if (symbleList[wp] == PLUS || symbleList[wp] == MINU) {
        printWord();
    }
    unsgIntDef();
    printSyntax("<整数>");
}

/**
 * 无符号整数
 */
void unsgIntDef() {
    assert(symbleList[wp], INTCON);
    printWord();
    printSyntax("<无符号整数>");
}

/**
 * 标识符
 */
void idenDef() {
    assert(symbleList[wp], IDENFR);
    printWord();
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
    asserts(symbleList[wp], INTTK, CHARTK);
    enum Type t = symbleList[wp] == INTTK ? INT : symbleList[wp] == CHARTK ? CHAR : VOID;
    printWord();
    while (1) {
        idenDef();
        char *name = tokenList[wp - 1];
        if (symbleList[wp] == LBRACK) {
            printWord();
            unsgIntDef();
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
        if (symbleList[wp] == COMMA) {
            printWord();
            continue;
        } else if (symbleList[wp] == SEMICN) {
            break;
        } else {
            //error(lines[wp - 1], MISS_SEMI);
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
    if ((r = checkRedef(tokenList[wp - 1], 0)) != SUCCESS) {
        error(lines[wp - 1], r);
    } else {
        addToTable(tokenList[wp - 1], FUNC, VOID, 0, 2, 888, VOID);
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
enum Type expressDef() {
    enum Type ret = INT;
    if (symbleList[wp] == PLUS || symbleList[wp] == MINU) {
        printWord();
        itemDef();
    } else {
        ret = itemDef();
    }
    while (1) {
        if (symbleList[wp] == PLUS || symbleList[wp] == MINU) {
            printWord();
            ret = INT;
            itemDef();
        } else {
            break;
        }
    }
    printSyntax("<表达式>");
    return ret;
}

/**
 * 项
 */
enum Type itemDef() {
    enum Type ret;
    ret = factorDef();
    while (1) {
        if (symbleList[wp] == MULT || symbleList[wp] == DIV) {
            printWord();
            ret = INT;
            factorDef();
        } else {
            break;
        }
    }
    printSyntax("<项>");
    return ret;
}

/**
 * 因子
 */
enum Type factorDef() {
    int r;
    enum Type ret = INT;
    if (symbleList[wp] == IDENFR && symbleList[wp + 1] != LPARENT) {
        idenDef();
        char *name = tokenList[wp - 1];
        if ((r = checkExist(name, level)) != SUCCESS) {
            error(lines[wp - 1], r);
        }
        if (symbleList[wp] == LBRACK) {
            printWord();
            if (expressDef() != INT) {
                error(lines[wp - 1], OFFSET_NOT_INT);
            }
            if (symbleList[wp] != RBRACK) {
                error(lines[wp], MISS_RBRACK);
            } else { printWord(); }
        }
        ret = getType(name, level);
    } else if (symbleList[wp] == LPARENT) {
        printWord();
        expressDef();
        if (symbleList[wp] != RPARENT) {
            error(lines[wp - 1], MISS_RPARENT);
        } else { printWord(); }
    } else if (symbleList[wp] == INTCON || symbleList[wp] == PLUS || symbleList[wp] == MINU) {
        intDef();
    } else if (symbleList[wp] == CHARCON) {
        charDef();
        ret = CHAR;
    } else if (symbleList[wp] == IDENFR && symbleList[wp + 1] == LPARENT) {
        char *name = tokenList[wp];
        if ((r = checkExist(name, 0)) < 0) {
            error(lines[wp - 1], r);
            wp += 2;
            assignParaDef(tokenList[wp - 2]);
            wp++;
        } else {
            if (isRetFunc(name)) {
                retFuncCallDef();
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
    return ret;
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
    idenDef();
    if ((r = checkExist(tokenList[wp - 1], level)) != SUCCESS) {
        error(lines[wp - 1], r);
    } else if (isConst(tokenList[wp - 1], level)) {
        error(lines[wp - 1], CANT_CHANGE_CONST);
    }
    if (symbleList[wp] == ASSIGN) {
        printWord();
        expressDef();
    } else if (symbleList[wp] == LBRACK) {
        printWord();
        if (expressDef() != INT) {
            error(lines[wp - 1], OFFSET_NOT_INT);
        }
        if (symbleList[wp] != RBRACK) {
            error(lines[wp], MISS_RBRACK);
        } else { printWord(); }
        assert(symbleList[wp], ASSIGN);
        printWord();
        expressDef();
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
    conditDef();
    if (symbleList[wp] != RPARENT) {
        error(lines[wp - 1], MISS_RPARENT);
    } else { printWord(); }
    sentDef();
    if (symbleList[wp] == ELSETK) {
        printWord();
        sentDef();
    }
    printSyntax("<条件语句>");
}

/**
 * 条件
 */
void conditDef() {
    if (expressDef() != INT) {
        error(lines[wp - 1], CONDIT_ILLEGAL);
    }
    if (symbleList[wp] == LSS || symbleList[wp] == LEQ || symbleList[wp] == GRE || symbleList[wp] == GEQ ||
        symbleList[wp] == EQL || symbleList[wp] == NEQ) {
        printWord();
        if (expressDef() != INT) {
            error(lines[wp - 1], CONDIT_ILLEGAL);
        }
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
        conditDef();
        if (symbleList[wp] != RPARENT) {
            error(lines[wp - 1], MISS_RPARENT);
        } else { printWord(); }
        sentDef();
    } else if (symbleList[wp] == DOTK) {
        printWord();
        sentDef();
        if (symbleList[wp] != WHILETK) {
            error(lines[wp], MISS_WHILE);
        } else {
            printWord();
        }
        if (symbleList[wp] == LPARENT) {
            assert(symbleList[wp], LPARENT);
            printWord();
            conditDef();
            if (symbleList[wp] != RPARENT) {
                error(lines[wp - 1], MISS_RPARENT);
            } else { printWord(); }
        }
    } else if (symbleList[wp] == FORTK) {
        printWord();
        assert(symbleList[wp], LPARENT);
        printWord();
        idenDef();
        if ((r = checkExist(tokenList[wp - 1], level)) != SUCCESS) {
            error(lines[wp - 1], r);
        } else if (isConst(tokenList[wp - 1], level)) {
            error(lines[wp - 1], CANT_CHANGE_CONST);
        }
        assert(symbleList[wp], ASSIGN);
        printWord();
        expressDef();
        if (symbleList[wp] != SEMICN) {
            error(lines[wp - 1], MISS_SEMI);
        } else { printWord(); }
        conditDef();
        if (symbleList[wp] != SEMICN) {
            error(lines[wp - 1], MISS_SEMI);
        } else { printWord(); }
        idenDef();
        if ((r = checkExist(tokenList[wp - 1], level)) != SUCCESS) {
            error(lines[wp - 1], r);
        } else if (isConst(tokenList[wp - 1], level)) {
            error(lines[wp - 1], CANT_CHANGE_CONST);
        }
        assert(symbleList[wp], ASSIGN);
        printWord();
        idenDef();
        if ((r = checkExist(tokenList[wp - 1], level)) != SUCCESS) {
            error(lines[wp - 1], r);
        }
        asserts(symbleList[wp], PLUS, MINU);
        printWord();
        stepDef();
        if (symbleList[wp] != RPARENT) {
            error(lines[wp - 1], MISS_RPARENT);
        } else { printWord(); }
        sentDef();
    } else panic("loopDef");
    printSyntax("<循环语句>");
}

/**
 * 步长
 */
void stepDef() {
    unsgIntDef();
    printSyntax("<步长>");
}

/**
 * 有返回值函数调用语句
 */
void retFuncCallDef() {
    int r;
    idenDef();
    if ((r = checkExist(tokenList[wp - 1], 0)) != SUCCESS) {
        error(lines[wp - 1], r);
    }
    assert(symbleList[wp], LPARENT);
    printWord();
    assignParaDef(tokenList[wp - 2]);
    if (symbleList[wp] != RPARENT) {
        error(lines[wp - 1], MISS_RPARENT);
    } else { printWord(); }
    printSyntax("<有返回值函数调用语句>");
}

/**
 * 无返回值函数调用语句
 */
void unRetFuncCallDef() {
    int r;
    idenDef();
    if ((r = checkExist(tokenList[wp - 1], level)) != SUCCESS) {
        error(lines[wp - 1], r);
    }
    assert(symbleList[wp], LPARENT);
    printWord();
    assignParaDef(tokenList[wp - 2]);
    if (symbleList[wp] != RPARENT) {
        error(lines[wp - 1], MISS_RPARENT);
    } else { printWord(); }
    printSyntax("<无返回值函数调用语句>");
}

/**
 * 值参数表
 */
void assignParaDef(char *fname) {
    int paraIndex = 0;
    if (symbleList[wp] != RPARENT && symbleList[wp] != SEMICN) {
        while (1) {
            enum Type type = expressDef();
            int r;
            paraIndex++;
            if ((r = checkPara1(fname, paraIndex, type)) != SUCCESS) {
                error(lines[wp - 1], r);
            }
            if (symbleList[wp] == COMMA) {
                printWord();
                continue;
            }
            break;
        }
    }
    int r;
    if ((r = checkPara2(fname, paraIndex)) != SUCCESS) {
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
        idenDef();
        if ((r = checkExist(tokenList[wp - 1], level)) != SUCCESS) {
            error(lines[wp - 1], r);
        }
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
        if (symbleList[wp] == COMMA) {
            printWord();
            expressDef();
        }
    } else {
        expressDef();
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
        enum Type ret = expressDef();
        if (checkRet) {
            if (retType == VOID) {
                error(lines[wp], FORBID_RET);
            } else if (ret != retType) {
                error(lines[wp], MISMATCH_RET);
            }
        }
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