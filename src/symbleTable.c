#include "word.h"
#include "syntax.h"
#include <string.h>

char retFuncToken[TOKENLENGTH][FUNCCOUNT];
char unRetFuncToken[TOKENLENGTH][FUNCCOUNT];
int retFuncCount = 0;
int unRetFuncCount = 0;

void addToTable(int ret, char *toke) {
    if (ret) {
        strcpy(retFuncToken[retFuncCount++], toke);
    } else {
        strcpy(unRetFuncToken[unRetFuncCount++], toke);
    }
}

int isRetFunc(char *func) {
    int tmp;
    for (tmp = 0; tmp < retFuncCount; tmp++) {
        if (!strcmp(func, retFuncToken[tmp])) {
            return 1;
        }
    }
    return 0;
}

int isUnRetFunc(char *func) {
    int tmp;
    for (tmp = 0; tmp < unRetFuncCount; tmp++) {
        if (!strcmp(func, unRetFuncToken[tmp])) {
            return 1;
        }
    }
    return 0;
}