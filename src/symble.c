#include "word.h"
#include "string.h"
#include "error.h"

int getSymble(char *str, int l) {
    clearToken();
    while (isSpace(str[i])) {
        i++;
    }
    if (i >= strlen(str)) {
        return END;
    }
    if (isLetter(str[i])) {
        while (isLetter(str[i]) || isDigit(str[i])) {
            catToken(str[i]);
            i++;
        }
        int r = isReserved(token);
        return r == UNDEFINED ? IDENFR : r;
    } else if (isDigit(str[i])) {
        if (str[i] == '0' && isDigit(str[i + 1])) {
            error(l, ZERO_AHEAD);
        }
        while (isDigit(str[i])) {
            catToken(str[i]);
            i++;
        }
        return INTCON;
    } else if (isLss(str[i])) {
        catToken(str[i]);
        if (isEqu(str[++i])) {
            catToken(str[i]);
            i++;
            return LEQ;
        }
        return LSS;
    } else if (isGre(str[i])) {
        catToken(str[i]);
        if (isEqu(str[++i])) {
            catToken(str[i]);
            i++;
            return GEQ;
        }
        return GRE;
    } else if (isEqu(str[i])) {
        catToken(str[i]);
        if (isEqu(str[++i])) {
            catToken(str[i]);
            i++;
            return EQL;
        }
        return ASSIGN;
    } else if (isExcla(str[i])) {
        catToken(str[i]);
        if (isEqu(str[++i])) {
            catToken(str[i]);
            i++;
        } else { error(l, MISS_EQL); }
        return NEQ;
    } else if (isDoubQuo(str[i])) {
        i++;
        while (!isDoubQuo(str[i])) {
            if (!(str[i] == 32 || str[i] == 33 || (str[i] >= 35 && str[i] <= 126))) {
                error(l, STR_ILLEGAL_CHAR);
            }
            catToken(str[i]);
            i++;
            if (i >= strlen(str)) {
                //如果没有右双引号，则直接全部认为字符串
                error(l, MISS_DOUBQUO);
                break;
            }
        }
        if (isDoubQuo(str[i])) {
            i++;
        }
        return STRCON;
    } else if (isSiglQuo(str[i])) {
        i++;
        if (isPlus(str[i]) || isMinus(str[i]) || isMult(str[i]) || isDivi(str[i]) || isLetter(str[i]) ||
            isDigit(str[i])) {
            catToken(str[i]);
        } else {
            error(l, UNRECOGNIZED);
            catToken(str[i]);
        }
        i++;
        if (isSiglQuo(str[i])) {
            i++;
        } else {
            error(l, MISS_SIGLGQUO);
        }
        return CHARCON;
    } else if (isPlus(str[i])) {
        catToken(str[i]);
        i++;
        return PLUS;
    } else if (isMinus(str[i])) {
        catToken(str[i]);
        i++;
        return MINU;
    } else if (isMult(str[i])) {
        catToken(str[i]);
        i++;
        return MULT;
    } else if (isDivi(str[i])) {
        catToken(str[i]);
        i++;
        return DIV;
    } else if (isSemi(str[i])) {
        catToken(str[i]);
        i++;
        return SEMICN;
    } else if (isComma(str[i])) {
        catToken(str[i]);
        i++;
        return COMMA;
    } else if (isLpar(str[i])) {
        catToken(str[i]);
        i++;
        return LPARENT;
    } else if (isRpar(str[i])) {
        catToken(str[i]);
        i++;
        return RPARENT;
    } else if (isLbrack(str[i])) {
        catToken(str[i]);
        i++;
        return LBRACK;
    } else if (isRbrack(str[i])) {
        catToken(str[i]);
        i++;
        return RBRACK;
    } else if (isLbrace(str[i])) {
        catToken(str[i]);
        i++;
        return LBRACE;
    } else if (isRbrace(str[i])) {
        catToken(str[i]);
        i++;
        return RBRACE;
    }
    error(l, UNRECOGNIZED);
    return UNDEFINED;
}