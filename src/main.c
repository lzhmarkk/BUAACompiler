#include <stdio.h>
#include "word.h"
#include <string.h>
#include "syntax.h"
#include "error.h"
#include "mid.h"

#define DEBUG 1
int j;//token length
FILE *fpI = NULL, *fpO = NULL, *fpE = NULL, *fpM = NULL;

int main() {
    if (!DEBUG) {
        fpI = fopen("testfile.txt", "r");
        fpO = fopen("output.txt", "w");
        fpE = fopen("error.txt", "w");
    } else {
        fpI = fopen("../test/testfile.txt", "r");
        fpO = fopen("../test/output.txt", "w");
        fpE = fopen("../test/error.txt", "w");
        fpM = fopen("../test/mips.txt", "w");
    }
    int l = 0;
    while (fgets(buf, MAXLENGTH, fpI)) {
        i = 0;
        l++;
        while (i < strlen(buf)) {
            symble = getSymble(buf, l);
            if (symble == END) {
                break;
            } else if (symble == UNDEFINED) {
                i++;
                continue;
            }
            //fprintf(fpO, "%s %s\n", getReserved(symble), token);
            symbleList[wp] = symble;
            lines[wp] = l;
            strcpy(tokenList[wp], token);
            wp++;
        }
    }
    wp = 0;
    programDef();
    printTable();
    printError();
    printCode();
    fclose(fpI);
    fclose(fpO);
    fclose(fpE);
    fclose(fpM);
    return 0;
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

void printWord() {
    fprintf(fpO, "%s %s\n", getReserved(symbleList[wp]), tokenList[wp]);
    wp++;
}

void printSyntax(char *msg) {
    fprintf(fpO, "%s\n", msg);
}

void printError() {
    struct ErrMsg *e;
    for (e = err; e != NULL; e = e->next) {
        fprintf(fpE, "%d %c\n", e->line, e->code);
        printf("%d %c : %s\n", e->line, e->code, e->msg);
    }
}

void printMips(char *msg) {
    fprintf(fpM, "%s\n", msg);
}