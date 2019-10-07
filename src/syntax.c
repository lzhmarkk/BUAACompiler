#include "syntax.h"
#include "word.h"

/**
 * 程序
 */
void programDef() {
    if (symbleList[wp] == CONSTTK) {
        constDef();
    }
    if ((symbleList[wp] == INTTK || symbleList[wp] == CHARTK) &&
        symbleList[wp + 1] == IDENFR &&
        (symbleList[wp + 2] == LBRACK || symbleList[wp + 2] == COMMA ||
         symbleList[wp + 2] == SEMICN)) {
        varyExplnDef();
    }
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
    mainDef();
}

/**
 * 字符
 */
void charDef() {
    wp++;
}

/**
 * 字符串
 */
void stringDef() {
    wp++;
}

/**
 * 常量说明
 */
void constExpln() {
    while (symbleList[wp] == CONSTTK) {
        wp++;
        constDef();
        assert(symbleList[wp], SEMICN);
        wp++;
    }
}

/**
 * 常量定义
 */
void constDef() {
    if (symbleList[wp] == INTTK) {
        wp++;
        while (1) {
            if (symbleList[wp] == IDENFR) {
                idenDef();
            } else error();
            assert(symbleList[wp], ASSIGN);
            if (symbleList[wp] == INTCON) {
                intDef();
            } else error();
            if (symbleList[wp] == COMMA) {
                wp++;
                continue;
            }
            break;
        }
    } else if (symbleList[wp] == CHARTK) {
        wp++;
        while (1) {
            if (symbleList[wp] == IDENFR) {
                idenDef();
            } else error();
            assert(symbleList[wp], ASSIGN);
            if (symbleList[wp] == INTCON) {
                intDef();
            } else error();
            if (symbleList[wp] == COMMA) {
                wp++;
                continue;
            }
            break;
        }
    } else error();
}

/**
 * 整数
 */
void intDef() {
    if (symbleList[wp] == PLUS || symbleList[wp] == MINU) {
        wp++;
    }
    unsgIntDef();
}

/**
 * 无符号整数
 */
void unsgIntDef() {
    assert(symbleList[wp], INTCON);
    wp++;
}

/**
 * 标识符
 */
void idenDef() {
    assert(symbleList[wp], IDENFR);
    wp++;
}

/**
 * 声明头部
 */
void explnheadDef() {
    if (symbleList[wp] == INTTK || symbleList[wp] == CHARTK) {
        wp++;
        idenDef();
    } else {
        error();
    }
}

/**
 * 变量说明
 */
void varyExplnDef() {
    while ((symbleList[wp] == INTTK || symbleList[wp] == CHARTK) &&
           symbleList[wp + 1 == IDENFR && symbleList[wp + 2] != LPARENT]) {
        varyDef();
        assert(symbleList[wp], SEMICN);
        wp++;
    }
}

/**
 * 变量定义
 */
void varyDef() {
    if (symbleList[wp] != INTTK && symbleList[wp] != CHARTK)error();
    wp++;
    while (1) {
        idenDef();
        if (symbleList[wp] == LBRACK) {
            wp++;
            unsgIntDef();
            assert(symbleList[wp], RBRACK);
            wp++;
        } else if (symbleList[wp] == COMMA) {
            wp++;
            continue;
        }
        break;
    }
}

/**
 * 有返回值函数定义
 */
void retFuncDef() {
    explnheadDef();
    assert(symbleList[wp], LPARENT);
    wp++;
    paraDef();
    assert(symbleList[wp], RPARENT);
    wp++;
    assert(symbleList[wp], LBRACE);
    wp++;
    mutiSentDef();
    assert(symbleList[wp], RBRACE);
    wp++;
}

/**
 * 无返回值函数定义
 */
void unRetFuncDef() {
    assert(symbleList[wp], VOIDTK);
    wp++;
    idenDef();
    assert(symbleList[wp], LPARENT);
    wp++;
    paraDef();
    assert(symbleList[wp], RPARENT);
    wp++;
    assert(symbleList[wp], LBRACE);
    wp++;
    mutiSentDef();
    assert(symbleList[wp], RBRACE);
    wp++;
}

/**
 * 复合语句
 */
void mutiSentDef() {
    if (symbleList[wp] == CONSTTK) {
        wp++;
        constExpln();
    }
    if (symbleList[wp] == INTTK || symbleList[wp] == CHARTK) {
        wp++;
        varyExplnDef();
    }
}

/**
 * 参数表
 */
void paraDef() {
    while (1) {
        if (symbleList[wp] != INTTK && symbleList[wp] != CHARTK)error();
        wp++;
        idenDef();
        if (symbleList[wp] == COMMA) {
            wp++;
            continue;
        }
        break;
    }
}

/**
 * 主函数
 */
void mainDef() {
    assert(symbleList[wp], VOIDTK);
    wp++;
    assert(symbleList[wp], MAINTK);
    wp++;
    assert(symbleList[wp], LPARENT);
    wp++;
    assert(symbleList[wp], RPARENT);
    wp++;
    assert(symbleList[wp], LBRACE);
    wp++;
    mutiSentDef();
    assert(symbleList[wp], RBRACE);
    wp++;
}

/**
 * 表达式
 */
void expressDef() {
    if (symbleList[wp] == PLUS || symbleList[wp] == MINU) {
        wp++;
    }
    while (1) {
        itemDef();
        if (symbleList[wp] == PLUS || symbleList[wp] == MINU) {
            wp++;
            continue;
        }
        break;
    }
}

/**
 * 项
 */
void itemDef() {
    while (1) {
        factorDef();
        if (symbleList[wp] == MULT || symbleList[wp] == DIV) {
            wp++;
            continue;
        }
        break;
    }
}

/**
 * 因子
 */
void factorDef() {
    if (symbleList[wp] == IDENFR && symbleList[wp + 1] != LPARENT) {
        idenDef();
        wp++;
        if (symbleList[wp] == LBRACK) {
            wp++;
            expressDef();
        }
        assert(symbleList[wp], RBRACK);
        wp++;
    } else if (symbleList[wp] == LPARENT) {
        wp++;
        expressDef();
        assert(symbleList[wp], RPARENT);
        wp++;
    } else if (symbleList[wp] == INTCON) {
        intDef();
    } else if (symbleList[wp] == CHARCON) {
        charDef();
    } else if (symbleList[wp] == IDENFR && symbleList[wp + 1] == LPARENT) {
        if (isRetFunc(tokenList[wp])) {
            retFuncCallDef();
        } else error();
    } else { error(); }
}

/**
 * 语句
 */
void sentDef() {
    if (symbleList[wp] == LBRACE) {
        wp++;
        sentsDef();
        assert(symbleList[wp], RBRACE);
        wp++;
    } else if (symbleList[wp] == SCANFTK) {
        readSentDef();
        assert(symbleList[wp], SEMICN);
        wp++;
    } else if (symbleList[wp] == WHILETK || symbleList[wp] == DOTK || symbleList[wp] == FORTK) {
        loopDef();
    } else if (symbleList[wp] == IFTK) {
        conditSentDef();
    } else if (symbleList[wp] == RETURNTK) {
        retDef();
        assert(symbleList[wp], SEMICN);
        wp++;
    } else if (symbleList[wp] == PRINTFTK) {
        writeSentDef();
        assert(symbleList[wp], SEMICN);
        wp++;
    } else if (symbleList[wp] == SEMICN) {
        wp++;
    } else if (symbleList[wp] == IDENFR) {
        if (symbleList[wp + 1] == ASSIGN || symbleList[wp + 1] == LBRACK) {
            assignSentDef();
            assert(symbleList[wp], SEMICN);
            wp++;
        } else if (symbleList[wp + 1] == LPARENT) {
            if (isRetFunc(tokenList[wp])) {
                retFuncCallDef();
                assert(symbleList[wp], SEMICN);
                wp++;
            } else if (isUnRetFunc(tokenList[wp])) {
                unRetFuncCallDef();
                assert(symbleList[wp], SEMICN);
                wp++;
            } else error();
        } else error();
    } else error();
}

/**
 * 赋值语句
 */
void assignSentDef() {
    idenDef();
    if (symbleList[wp] == ASSIGN) {
        wp++;
        expressDef();
    } else if (symbleList[wp] == LBRACK) {
        expressDef();
        assert(symbleList[wp], RBRACK);
        wp++;
        assert(symbleList[wp], ASSIGN);
        wp++;
        expressDef();
    } else error();
}

/**
 * 条件语句
 */
void conditSentDef() {
    assert(symbleList[wp], IFTK);
    wp++;
    assert(symbleList[wp], LPARENT);
    wp++;
    conditDef();
    assert(symbleList[wp], RPARENT);
    wp++;
    sentDef();
    if (symbleList[wp] == ELSETK) {
        wp++;
        sentDef();
    }
}

/**
 * 条件
 */
void conditDef() {
    expressDef();
    if (symbleList[wp] == LSS || symbleList[wp] == LEQ || symbleList[wp] == GRE || symbleList[wp] == GEQ ||
        symbleList[wp] == EQL || symbleList[wp] == NEQ) {
        wp++;
        expressDef();
    }
}

/**
 * 循环语句
 */
void loopDef() {
    if (symbleList[wp] == WHILETK) {
        assert(symbleList[wp], LPARENT);
        wp++;
        conditDef();
        assert(symbleList[wp], RPARENT);
        wp++;
        sentDef();
    } else if (symbleList[wp] == DOTK) {
        sentDef();
        assert(symbleList[wp], WHILETK);
        wp++;
        assert(symbleList[wp], LPARENT);
        wp++;
        conditDef();
        assert(symbleList[wp], RPARENT);
        wp++;
    } else if (symbleList[wp] == FORTK) {
        assert(symbleList[wp], LPARENT);
        wp++;
        idenDef();
        assert(symbleList[wp], ASSIGN);
        wp++;
        expressDef();
        assert(symbleList[wp], SEMICN);
        wp++;
        conditDef();
        assert(symbleList[wp], SEMICN);
        wp++;
        idenDef();
        assert(symbleList[wp], ASSIGN);
        wp++;
        idenDef();
        asserts(symbleList[wp], PLUS, MINU);
        wp++;
        stepDef();
        assert(symbleList[wp], RPARENT);
        wp++;
        sentDef();
    } else error();
}

/**
 * 步长
 */
void stepDef() {
    unsgIntDef();
}

/**
 * 有返回值函数调用语句
 */
void retFuncCallDef() {
    idenDef();
    assert(symbleList[wp], LPARENT);
    wp++;
    assignParaDef();
    assert(symbleList[wp], RPARENT);
    wp++;
}

/**
 * 无返回值函数调用语句
 */
void unRetFuncCallDef() {
    idenDef();
    assert(symbleList[wp], LPARENT);
    wp++;
    assignParaDef();
    assert(symbleList[wp], RPARENT);
    wp++;
}

/**
 * 值参数表
 */
void assignParaDef() {
    if (symbleList[wp] == RPARENT) {
        return;
    }
    while (1) {
        expressDef();
        if (symbleList[wp] == COMMA) {
            wp++;
            continue;
        }
        break;
    }
}

/**
 * 语句列
 */
void sentsDef() {
    while (1) {
        sentDef();
        if (symbleList[wp] == RBRACE) {
            break;
        }
    }
}

/**
 * 读语句
 */
void readSentDef() {
    assert(symbleList[wp], SCANFTK);
    wp++;
    assert(symbleList[wp], LPARENT);
    wp++;
    while (1) {
        idenDef();
        if (symbleList[wp] == COMMA) {
            wp++;
            continue;
        }
        break;
    }
    assert(symbleList[wp], RPARENT);
    wp++;
}

/**
 * 写语句
 */
void writeSentDef() {
    assert(symbleList[wp], PRINTFTK);
    wp++;
    assert(symbleList[wp], LPARENT);
    wp++;
    if (symbleList[wp] == STRCON) {
        stringDef();
        if (symbleList[wp] == RPARENT) {
            wp++;
        } else if (symbleList[wp] == COMMA) {
            wp++;
            expressDef();
        } else error();
    } else {
        expressDef();
    }
    assert(symbleList[wp], RPARENT);
    wp++;
}

/**
 * 返回语句
 */
void retDef() {
    assert(symbleList[wp], RETURNTK);
    wp++;
    if (symbleList[wp] == LPARENT) {
        wp++;
        expressDef();
        assert(symbleList[wp], RPARENT);
        wp++;
    }
}

int error() {
    printf("error\n");
    return 0;
}

void assert(enum SYMBLE a, enum SYMBLE b) {
    if (a == b) {
        return;
    } else {
        error();
    }
}

void asserts(enum SYMBLE a, enum SYMBLE b1, enum SYMBLE b2) {
    if (a == b1 || a == b2) {
        return;
    } else {
        error();
    }
}