#include <stdio.h>
#include "word.h"
#include <string.h>

char buf[MAXLENGTH] = {};
char token[TOKENLENGTH] = {};
int i;
int j;//token length

int main() {
    FILE *fpI = NULL, *fpO = NULL;
    if (1) {
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
            fprintf(fpO, "%s %s\n", getReserved(symble), token);
        }
    }
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