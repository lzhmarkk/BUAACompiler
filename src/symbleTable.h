#ifndef WORDANALYZER_C_SYMBLETABLE_H
#define WORDANALYZER_C_SYMBLETABLE_H

#include "word.h"
#include "const.h"
#include "syntax.h"

void addToTable(int ret, char *toke);

int isRetFunc(char *func);

int isUnRetFunc(char *func);

#endif //WORDANALYZER_C_SYMBLETABLE_H
