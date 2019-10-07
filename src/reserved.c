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
        case IDENFR:
            return "IDENFR";
        case INTCON:
            return "INTCON";
        case CHARCON:
            return "CHARCON";
        case STRCON:
            return "STRCON";
        case CONSTTK:
            return "CONSTTK";
        case INTTK:
            return "INTTK";
        case CHARTK:
            return "CHARTK";
        case VOIDTK:
            return "VOIDTK";
        case MAINTK:
            return "MAINTK";
        case IFTK:
            return "IFTK";
        case ELSETK:
            return "ELSETK";
        case DOTK:
            return "DOTK";
        case WHILETK:
            return "WHILETK";
        case FORTK:
            return "FORTK";
        case SCANFTK:
            return "SCANFTK";
        case PRINTFTK:
            return "PRINTFTK";
        case RETURNTK:
            return "RETURNTK";
        case PLUS:
            return "PLUS";
        case MINU:
            return "MINU";
        case MULT:
            return "MULT";
        case DIV:
            return "DIV";
        case LSS:
            return "LSS";
        case LEQ:
            return "LEQ";
        case GRE:
            return "GRE";
        case GEQ:
            return "GEQ";
        case EQL:
            return "EQL";
        case NEQ:
            return "NEQ";
        case ASSIGN:
            return "ASSIGN";
        case SEMICN:
            return "SEMICN";
        case COMMA:
            return "COMMA";
        case LPARENT:
            return "LPARENT";
        case RPARENT:
            return "RPARENT";
        case LBRACK:
            return "LBRACK";
        case RBRACK:
            return "RBRACK";
        case LBRACE:
            return "LBRACE";
        case RBRACE:
            return "RBRACE";
        default:
            return "";
    }
}