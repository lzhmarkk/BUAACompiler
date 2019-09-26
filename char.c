#include "word.h"

int isSpace(char ch) {
    return ch == ' ' || ch == '\f' || ch == '\n' || ch == '\r' || ch == '\t' || ch == '\v';
}

int isLetter(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch == '_');
}

int isDigit(char ch) {
    return ch >= '0' && ch <= '9';
}

int isComma(char ch) {
    return ch == ',';
}

int isSemi(char ch) {
    return ch == ';';
}

int isEqu(char ch) {
    return ch == '=';
}

int isPlus(char ch) {
    return ch == '+';
}

int isMinus(char ch) {
    return ch == '-';
}

int isDivi(char ch) {
    return ch == '/';
}

int isMult(char ch) {
    return ch == '*';
}

int isLpar(char ch) {
    return ch == '(';
}

int isRpar(char ch) {
    return ch == ')';
}

int isLbrack(char ch) {
    return ch == '[';
}

int isRbrack(char ch) {
    return ch == ']';
}

int isLbrace(char ch) {
    return ch == '{';
}

int isRbrace(char ch) {
    return ch == '}';
}

int isLss(char ch) {
    return ch == '<';
}

int isGre(char ch) {
    return ch == '>';
}

int isExcla(char ch) {
    return ch == '!';
}

int isDoubQuo(char ch) {
    return ch == '\"';
}

int isSiglQuo(char ch) {
    return ch == '\'';
}