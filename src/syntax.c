#include "syntax.h"
#include "word.h"
#include "symbleTable.h"

/**
 * 程序
 */
void programDef() {
    if (symbleList[wp] == CONSTTK) {
        constExpln();
    }
    if ((symbleList[wp] == INTTK || symbleList[wp] == CHARTK) &&
        symbleList[wp + 1] == IDENFR &&
        (symbleList[wp + 2] == LBRACK || symbleList[wp + 2] == COMMA ||
         symbleList[wp + 2] == SEMICN)) {
        varyExpln();
    }
    while (symbleList[wp + 1] != MAINTK) {
        if ((symbleList[wp] == INTTK || symbleList[wp] == CHARTK) &&
            symbleList[wp + 1] == IDENFR &&
            symbleList[wp + 2] == LPARENT) {
            retFuncDef();
        }
        if (symbleList[wp] == VOIDTK &&
            symbleList[wp + 1] == IDENFR &&
            symbleList[wp + 2] == LPARENT) {
            unRetFuncDef();
        }
    }
    mainDef();
    printSyntax("<程序>");
}

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
        assert(symbleList[wp], SEMICN);
        printWord();
    }
    printSyntax("<常量说明>");
}

/**
 * 常量定义
 */
void constDef() {
    if (symbleList[wp] == INTTK) {
        printWord();
        while (1) {
            if (symbleList[wp] == IDENFR) {
                idenDef();
                addToTable(tokenList[wp - 1], CONST, INT, 0, 0);
            } else error("conDef");
            assert(symbleList[wp], ASSIGN);
            printWord();
            intDef();
            if (symbleList[wp] == COMMA) {
                printWord();
                continue;
            }
            break;
        }
    } else if (symbleList[wp] == CHARTK) {
        printWord();
        while (1) {
            if (symbleList[wp] == IDENFR) {
                idenDef();
                addToTable(tokenList[wp - 1], CONST, CHAR, 0, 0);
            } else error("constDef");
            assert(symbleList[wp], ASSIGN);
            printWord();
            if (symbleList[wp] == CHARCON) {
                charDef();
            } else error("constDef");
            if (symbleList[wp] == COMMA) {
                printWord();
                continue;
            }
            break;
        }
    } else error("constDef");
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
    if (symbleList[wp] == INTTK || symbleList[wp] == CHARTK) {
        enum Type t = symbleList[wp] == INTTK ? INT : symbleList[wp] == CHARTK ? CHAR : VOID;
        printWord();
        idenDef();
        addToTable(tokenList[wp - 1], FUNC, VOID, 0, 2, 666, t);
    } else {
        error("explnheadDef");
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
        assert(symbleList[wp], SEMICN);
        printWord();
    }
    printSyntax("<变量说明>");
}

/**
 * 变量定义
 */
void varyDef() {
    asserts(symbleList[wp], INTTK, CHARTK);
    enum Type t = symbleList[wp] == INTTK ? INT : symbleList[wp] == CHARTK ? CHAR : VOID;
    printWord();
    while (1) {
        idenDef();
        char *name = tokenList[wp - 1];
        if (symbleList[wp] == LBRACK) {
            printWord();
            unsgIntDef();
            assert(symbleList[wp], RBRACK);
            printWord();
            addToTable(name, VAR, ARRAY, 0, 3, t, &tokenList[wp - 2], 0);
        } else {
            addToTable(name, VAR, t, 0, 0);
        }
        if (symbleList[wp] == COMMA) {
            printWord();
            continue;
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
    assert(symbleList[wp], LPARENT);
    printWord();
    paraDef();
    assert(symbleList[wp], RPARENT);
    printWord();
    assert(symbleList[wp], LBRACE);
    printWord();
    mutiSentDef();
    assert(symbleList[wp], RBRACE);
    printWord();
    printSyntax("<有返回值函数定义>");
}

/**
 * 无返回值函数定义
 */
void unRetFuncDef() {
    assert(symbleList[wp], VOIDTK);
    printWord();
    idenDef();
    addToTable(tokenList[wp - 1], FUNC, VOID, 0, 2, 888, VOID);
    assert(symbleList[wp], LPARENT);
    printWord();
    paraDef();
    assert(symbleList[wp], RPARENT);
    printWord();
    assert(symbleList[wp], LBRACE);
    printWord();
    mutiSentDef();
    assert(symbleList[wp], RBRACE);
    printWord();
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
    if (symbleList[wp] != RPARENT) {
        while (1) {
            asserts(symbleList[wp], INTTK, CHARTK);
            enum Type t = symbleList[wp] == INTTK ? INT : symbleList[wp] == CHARTK ? CHAR : VOID;
            printWord();
            idenDef();
            addToTable(tokenList[wp - 1], PARA, t, 0, 0);
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
    assert(symbleList[wp], VOIDTK);
    printWord();
    assert(symbleList[wp], MAINTK);
    printWord();
    assert(symbleList[wp], LPARENT);
    printWord();
    assert(symbleList[wp], RPARENT);
    printWord();
    assert(symbleList[wp], LBRACE);
    printWord();
    mutiSentDef();
    assert(symbleList[wp], RBRACE);
    printWord();
    printSyntax("<主函数>");
}

/**
 * 表达式
 */
void expressDef() {
    if (symbleList[wp] == PLUS || symbleList[wp] == MINU) {
        printWord();
    }
    while (1) {
        itemDef();
        if (symbleList[wp] == PLUS || symbleList[wp] == MINU) {
            printWord();
            continue;
        }
        break;
    }
    printSyntax("<表达式>");
}

/**
 * 项
 */
void itemDef() {
    while (1) {
        factorDef();
        if (symbleList[wp] == MULT || symbleList[wp] == DIV) {
            printWord();
            continue;
        }
        break;
    }
    printSyntax("<项>");
}

/**
 * 因子
 */
void factorDef() {
    if (symbleList[wp] == IDENFR && symbleList[wp + 1] != LPARENT) {
        idenDef();
        if (symbleList[wp] == LBRACK) {
            printWord();
            expressDef();
            assert(symbleList[wp], RBRACK);
            printWord();
        }
    } else if (symbleList[wp] == LPARENT) {
        printWord();
        expressDef();
        assert(symbleList[wp], RPARENT);
        printWord();
    } else if (symbleList[wp] == INTCON || symbleList[wp] == PLUS || symbleList[wp] == MINU) {
        intDef();
    } else if (symbleList[wp] == CHARCON) {
        charDef();
    } else if (symbleList[wp] == IDENFR && symbleList[wp + 1] == LPARENT) {
        if (isRetFunc(tokenList[wp])) {
            retFuncCallDef();
        } else error("factorDef");
    } else { error("factorDef"); }
    printSyntax("<因子>");
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
        assert(symbleList[wp], SEMICN);
        printWord();
    } else if (symbleList[wp] == WHILETK || symbleList[wp] == DOTK || symbleList[wp] == FORTK) {
        loopDef();
    } else if (symbleList[wp] == IFTK) {
        conditSentDef();
    } else if (symbleList[wp] == RETURNTK) {
        retDef();
        assert(symbleList[wp], SEMICN);
        printWord();
    } else if (symbleList[wp] == PRINTFTK) {
        writeSentDef();
        assert(symbleList[wp], SEMICN);
        printWord();
    } else if (symbleList[wp] == SEMICN) {
        printWord();
    } else if (symbleList[wp] == IDENFR) {
        if (symbleList[wp + 1] == ASSIGN || symbleList[wp + 1] == LBRACK) {
            assignSentDef();
            assert(symbleList[wp], SEMICN);
            printWord();
        } else if (symbleList[wp + 1] == LPARENT) {
            if (isRetFunc(tokenList[wp])) {
                retFuncCallDef();
                assert(symbleList[wp], SEMICN);
                printWord();
            } else if (isUnRetFunc(tokenList[wp])) {
                unRetFuncCallDef();
                assert(symbleList[wp], SEMICN);
                printWord();
            } else error("sentDef");
        } else error("sentDef");
    } else error("sentDef");
    printSyntax("<语句>");
}

/**
 * 赋值语句
 */
void assignSentDef() {
    idenDef();
    if (symbleList[wp] == ASSIGN) {
        printWord();
        expressDef();
    } else if (symbleList[wp] == LBRACK) {
        printWord();
        expressDef();
        assert(symbleList[wp], RBRACK);
        printWord();
        assert(symbleList[wp], ASSIGN);
        printWord();
        expressDef();
    } else error("assignSentDef");
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
    assert(symbleList[wp], RPARENT);
    printWord();
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
    expressDef();
    if (symbleList[wp] == LSS || symbleList[wp] == LEQ || symbleList[wp] == GRE || symbleList[wp] == GEQ ||
        symbleList[wp] == EQL || symbleList[wp] == NEQ) {
        printWord();
        expressDef();
    }
    printSyntax("<条件>");

}

/**
 * 循环语句
 */
void loopDef() {
    if (symbleList[wp] == WHILETK) {
        printWord();
        assert(symbleList[wp], LPARENT);
        printWord();
        conditDef();
        assert(symbleList[wp], RPARENT);
        printWord();
        sentDef();
    } else if (symbleList[wp] == DOTK) {
        printWord();
        sentDef();
        assert(symbleList[wp], WHILETK);
        printWord();
        assert(symbleList[wp], LPARENT);
        printWord();
        conditDef();
        assert(symbleList[wp], RPARENT);
        printWord();
    } else if (symbleList[wp] == FORTK) {
        printWord();
        assert(symbleList[wp], LPARENT);
        printWord();
        idenDef();
        assert(symbleList[wp], ASSIGN);
        printWord();
        expressDef();
        assert(symbleList[wp], SEMICN);
        printWord();
        conditDef();
        assert(symbleList[wp], SEMICN);
        printWord();
        idenDef();
        assert(symbleList[wp], ASSIGN);
        printWord();
        idenDef();
        asserts(symbleList[wp], PLUS, MINU);
        printWord();
        stepDef();
        assert(symbleList[wp], RPARENT);
        printWord();
        sentDef();
    } else error("loopDef");
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
    idenDef();
    assert(symbleList[wp], LPARENT);
    printWord();
    assignParaDef();
    assert(symbleList[wp], RPARENT);
    printWord();
    printSyntax("<有返回值函数调用语句>");
}

/**
 * 无返回值函数调用语句
 */
void unRetFuncCallDef() {
    idenDef();
    assert(symbleList[wp], LPARENT);
    printWord();
    assignParaDef();
    assert(symbleList[wp], RPARENT);
    printWord();
    printSyntax("<无返回值函数调用语句>");
}

/**
 * 值参数表
 */
void assignParaDef() {
    if (symbleList[wp] != RPARENT) {
        while (1) {
            expressDef();
            if (symbleList[wp] == COMMA) {
                printWord();
                continue;
            }
            break;
        }
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
    assert(symbleList[wp], SCANFTK);
    printWord();
    assert(symbleList[wp], LPARENT);
    printWord();
    while (1) {
        idenDef();
        if (symbleList[wp] == COMMA) {
            printWord();
            continue;
        }
        break;
    }
    assert(symbleList[wp], RPARENT);
    printWord();
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
        if (symbleList[wp] == RPARENT) { ;
        } else if (symbleList[wp] == COMMA) {
            printWord();
            expressDef();
        } else error("writeSentDef");
    } else {
        expressDef();
    }
    assert(symbleList[wp], RPARENT);
    printWord();
    printSyntax("<写语句>");
}

/**
 * 返回语句
 */
void retDef() {
    assert(symbleList[wp], RETURNTK);
    printWord();
    if (symbleList[wp] == LPARENT) {
        printWord();
        expressDef();
        assert(symbleList[wp], RPARENT);
        printWord();
    }
    printSyntax("<返回语句>");
}

int error(char *msg) {
    printf("%d:panic at %s\n", wp, msg);
    while (1);
}

void assert(enum SYMBLE a, enum SYMBLE b) {
    if (a == b) {
        return;
    } else {
        error("assert failed");
    }
}

void asserts(enum SYMBLE a, enum SYMBLE b1, enum SYMBLE b2) {
    if (a == b1 || a == b2) {
        return;
    } else {
        error("assert failed");
    }
}