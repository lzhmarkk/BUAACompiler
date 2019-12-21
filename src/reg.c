#include "reg.h"
#include "stdio.h"

int alloReg() {
    if (reg < MIDREG) {
        reg = MIDREG;
    }

    if (reg < 18) {
        return reg++;
    } else {
        //寄存器不够用了，要用内存辅助
        int r = ++regMem;
        if (r > regMemMax) {
            regMemMax = r;
        }
        return -r;
    }
}

void clearReg() {
    reg = 0;
    regMem = 0;
}

int alloRegGlo() {
    int r = ++gloRegMem;
    if (r > gloRegMemMax) {
        gloRegMemMax = r;
    }
    return r + GLO;
}

int isRegTmp(int r) {
    return 0 <= r && r < MIDREG;
}

int isRegGlo(int r) {
    return r >= GLO;
}

int isRegMem(int r) {
    return r < 0;
}

int alloRegTmp() {
    int t;
    for (t = 0; t < MIDREG; t++) {
        if (regTmpBusy[t] == 0) {
            //is free
            regTmpBusy[t] = 1;
            return t;
        }
    }
    printf("Allocate temp reg error");
    while (1);
}

void clearRegTmp(int r) {
    if (isRegTmp(r)) {
        regTmpBusy[r] = 0;
    } else {
        printf("No need to clear");
    }
}