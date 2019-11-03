//
// Created by lzhmark on 2019/10/31.
//

#ifndef WORDANALYZER_C_ERROR_H
#define WORDANALYZER_C_ERROR_H

#include "const.h"
#include "symbleTable.h"

#define SUCCESS        0
#define UNRECOGNIZED  -1
#define REDEFINED_GLO -2//
#define REDEFINED_LOC -3//
#define NOTDEFINED -4//
#define PARASIZE_MISMATCH -5//
#define PARATYPE_MISMATCH -6//
#define CONDIT_ILLEGAL -7//
#define FORBID_RET -8
#define MISS_RET -9
#define MISMATCH_RET -10
#define OFFSET_NOT_INT -11//
#define CANT_CHANGE_CONST -12
#define MISS_SEMI -13
#define MISS_RPARENT -14
#define MISS_RBRACK -15
#define MISS_WHILE -16
#define CONST_NOT_INTCHAR -17
#define CALL_UNRET_FUNC -18
#define NOT_A_FACTOR -19
#define MISS_SIGLGQUO -20
#define MISS_DOUBQUO -21
#define MISS_EQL -22

struct ErrMsg {
    int line;
    char code;
    char *msg;
    struct ErrMsg *next;
};

void error(int line, int code);

void insertError(struct ErrMsg *new);

void printError();

struct ErrMsg *err;
enum Type retType;
int checkRet;
int hasRet;
#endif //WORDANALYZER_C_ERROR_H