#include <stdio.h>
#include "word.h"
#include <string.h>

char buf[MAXLENGTH] = {};
char token[TOKENLENGTH] = {};
int i;
int j;

int main() {
    FILE *fpI = NULL, *fpO = NULL;
    fpI = fopen("/testfile.txt", "r");
    fpO = fopen("/output.txt", "w");
    while (fgets(buf, MAXLENGTH, fpI)) {
        int index = i;
        symble = getSymble(buf);
        if (symble == UNDEFINED) {
            return 1;
        }
        char pr[TOKENLENGTH] = {};
        strncpy(pr, buf + i, j);
        fprintf(fpO, "%s %s", pr, getReserved(symble));
    }
    fclose(fpI);
    fclose(fpO);
}

int getSymble(char *str) {
    clearToken();
    while (isSpace(str[i])) {
        i++;
    }
    if (isLetter(str[i])) {
        while (isLetter(str[i]) || isDigit(str[i])) {
            catToken(str[i]);
            i++;
        }
        i--;
        int r = isReserved(token);
        return r == 0 ? IDENFR : r;
    } else if (isDigit(str[i])) {
        while (isDigit(str[i])) {
            catToken(str[i]);
            i++;
        }
        i--;
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
            return EQL;
        }
        return ASSIGN;
    } else if (isExcla(str[i])) {
        catToken(str[i]);
        if (isEqu(str[++i])) {
            catToken(str[i]);
            return NEQ;
        }
        return UNDEFINED;
    } else if (isDoubQuo(str[i])) {
        i++;
        while (!isDoubQuo(str[i])) {
            catToken(str[i]);
            i++;
        }
        if (isDoubQuo(str[i])) {
            i++;
            return STRCON;
        }
        return UNDEFINED;
    } else if (isSiglQuo(str[i])) {
        i++;
        if (isPlus(str[i]) || isMult(str[i]) || isLetter(str[i]) || isDigit(str[i])) {
            catToken(str[i]);
            i++;
        }
        if (isSiglQuo(str[i])) {
            i++;
            return CHARCON;
        }
        return UNDEFINED;
    } else if (isPlus(str[i])) {
        catToken(str[i]);
        return PLUS;
    } else if (
            isMinus(str[i])
            ) {
        catToken(str[i]);
        return
                MINU;
    } else if (
            isMult(str[i])
            ) {
        catToken(str[i]);
        return
                MULT;
    } else if (
            isDivi(str[i])
            ) {
        catToken(str[i]);
        return
                DIV;
    } else if (
            isSemi(str[i])
            ) {
        catToken(str[i]);
        return
                SEMICN;
    } else if (
            isComma(str[i])
            ) {
        catToken(str[i]);
        return
                COMMA;
    } else if (
            isLpar(str[i])
            ) {
        catToken(str[i]);
        return
                LPARENT;
    } else if (
            isRpar(str[i])
            ) {
        catToken(str[i]);
        return
                RPARENT;
    } else if (
            isLbrack(str[i])
            ) {
        catToken(str[i]);
        return
                LBRACK;
    } else if (
            isRbrack(str[i])
            ) {
        catToken(str[i]);
        return
                RBRACK;
    } else if (
            isLbrace(str[i])
            ) {
        catToken(str[i]);
        return
                LBRACE;
    } else if (
            isRbrace(str[i])
            ) {
        catToken(str[i]);
        return
                RBRACE;
    }
    return UNDEFINED;
}

void clearToken() {
    j = 0;
    int k;
    for (k = 0; k < 20; k++) {
        token[k] = 0;
    }
}

void catToken(char c) {
    token[j++] = c;
}

long long transNum(char *str) {
    long long num = 0;
    while (*str) {
        num = num * 10 + (*str - '0');
        str++;
    }
    return num;
}