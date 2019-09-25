#include <stdio.h>
#include "word.h"
#include <string.h>

char buf[MAXLENGTH] = {};
char token[TOKENLENGTH] = {};
int i;//line pointer
int j;//token length

int main() {
    FILE *fpI = NULL, *fpO = NULL;
    fpI = fopen("../testfile.txt", "r");
    fpO = fopen("../output.txt", "w");
    while (fgets(buf, MAXLENGTH, fpI)) {
        i = 0;
        //while (i < strlen(buf)) {
        while (i < strlen(buf) - 1) {
        //while (buf[i] != EOF && buf[i] != '\n' && buf[i] != '\0') {
            symble = getSymble(buf);
            if (symble == UNDEFINED) {
                return 1;
            }
            printf("%s %s\n", getReserved(symble), token);
            fprintf(fpO, "%s %s\n", getReserved(symble), token);
        }
    }
    fclose(fpI);
    fclose(fpO);
    return 0;
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
        int r = isReserved(token);
        return r == UNDEFINED ? IDENFR : r;
    } else if (isDigit(str[i])) {
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
        catToken(str[i]);
        i++;
        if (isSiglQuo(str[i])) {
            i++;
            return CHARCON;
        }
        return UNDEFINED;
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
    return UNDEFINED;
}

void clearToken() {
    j = 0;
    int k;
    for (k = 0; k < TOKENLENGTH; k++) {
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