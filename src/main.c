#include <stdio.h>
#include "word.h"
#include <string.h>
#include "syntax.h"

int j;//token length

int main() {
    FILE *fpI = NULL, *fpO = NULL;
    if (0) {
        fpI = fopen("testfile.txt", "r");
        fpO = fopen("output.txt", "w");
    } else {
        fpI = fopen("../test/testfile.txt", "r");
        fpO = fopen("../test/output.txt", "w");
    }
    while (fgets(buf, MAXLENGTH, fpI)) {
        i = 0;
        while (i < strlen(buf)) {
            symble = getSymble(buf);
            if (symble == UNDEFINED) {
                break;
            }
            //fprintf(fpO, "%s %s\n", getReserved(symble), token);
            symbleList[wp] = symble;
            strcpy(tokenList[wp], token);
            wp++;
        }
    }
    wp = 0;
    programDef();
    fclose(fpI);
    fclose(fpO);
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