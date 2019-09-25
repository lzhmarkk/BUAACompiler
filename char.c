#include "word.h"

int isSpace(char ch) {
    return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n' || ch == '\v';
}

int isLetter(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

int isDigit(char ch) {
    return ch >= '0' && ch <= '9';
}

int isColon(char ch) {
    return ch == ':';
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

int isStar(char ch) {
    return ch == '*';
}

int isLpar(char ch) {
    return ch == '(';
}

int isRpar(char ch) {
    return ch == ')';
}