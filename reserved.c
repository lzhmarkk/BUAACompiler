#include "word.h"
#include <string.h>

int isReserved(char *ch) {
    if (!strcmp(ch, "const")) {
        return CONSTTK;
    } else if (!strcmp(ch, "int")) {
        return INTTK;
    } else if (!strcmp(ch, "char")) {
        return CHARTK;
    } else if (!strcmp(ch, "void")) {
        return VOIDTK;
    } else if (!strcmp(ch, "main")) {
        return MAINTK;
    } else if (!strcmp(ch, "if")) {
        return IFTK;
    } else if (!strcmp(ch, "else")) {
        return ELSETK;
    } else if (!strcmp(ch, "do")) {
        return DOTK;
    } else if (!strcmp(ch, "while")) {
        return WHILETK;
    } else if (!strcmp(ch, "for")) {
        return FORTK;
    } else if (!strcmp(ch, "scanf")) {
        return SCANFTK;
    } else if (!strcmp(ch, "printf")) {
        return PRINTFTK;
    } else if (!strcmp(ch, "return")) {
        return RETURNTK;
    }
    return UNDEFINED;
}

char *getReserved(int s) {
    switch (s) {
        case 1:
            return "IDENFR";
        case 2:
            return "INTCON";
        case 3:
            return "CHARCON";
        case 4:
            return "STRCON";
        case 5:
            return "CONSTTK";
        case 6:
            return "INTTK";
        case 7:
            return "CHARTK";
        case 8:
            return "VOIDTK";
        case 9:
            return "MAINTK";
        case 10:
            return "IFTK";
        case 11:
            return "ELSETK";
        case 12:
            return "DOTK";
        case 13:
            return "WHILETK";
        case 14:
            return "FORTK";
        case 15:
            return "SCANFTK";
        case 16:
            return "PRINTFTK";
        case 17:
            return "RETURNTK";
        case 18:
            return "PLUS";
        case 19:
            return "MINU";
        case 20:
            return "MULT";
        case 21:
            return "DIV";
        case 22:
            return "LSS";
        case 23:
            return "LEQ";
        case 24:
            return "GRE";
        case 25:
            return "GEQ";
        case 26:
            return "EQL";
        case 27:
            return "NEQ";
        case 28:
            return "ASSIGN";
        case 29:
            return "SEMICN";
        case 30:
            return "COMMA";
        case 31:
            return "LPARENT";
        case 32:
            return "RPARENT";
        case 33:
            return "LBRACK";
        case 34:
            return "RBRACK";
        case 35:
            return "LBRACE";
        case 36:
            return "RBRACE";
        default:
            return "";
    }
}