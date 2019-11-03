#include <stdlib.h>
#include "error.h"
#include "symbleTable.h"

void error(int line, int code) {
    struct ErrMsg *new = (struct ErrMsg *) malloc(sizeof(struct ErrMsg));
    new->line = line;
    new->next = NULL;
    switch (code) {
        case UNRECOGNIZED:
            new->code = 'a';
            new->msg = "Unrecognized symble";
            break;
        case REDEFINED_GLO :
            new->code = 'b';
            new->msg = "Redefined";
            break;
        case REDEFINED_LOC :
            new->code = 'b';
            new->msg = "Redefined";
            break;
        case NOTDEFINED :
            new->code = 'c';
            new->msg = "Undefined";
            break;
        case PARASIZE_MISMATCH :
            new->code = 'd';
            new->msg = "Parameter Size Mismatch";
            break;
        case PARATYPE_MISMATCH :
            new->code = 'e';
            new->msg = "Parameter Type Mismatch";
            break;
        case CONDIT_ILLEGAL :
            new->code = 'f';
            new->msg = "Condition Illegal";
            break;
        case FORBID_RET :
            new->code = 'g';
            new->msg = "Forbid Return";
            break;
        case MISS_RET :
            new->code = 'h';
            new->msg = "Miss Return";
            break;
        case MISMATCH_RET :
            new->code = 'h';
            new->msg = "Mismatch Return";
            break;
        case OFFSET_NOT_INT :
            new->code = 'i';
            new->msg = "Offset Not Integer";
            break;
        case CANT_CHANGE_CONST :
            new->code = 'j';
            new->msg = "Can't Change Const";
            break;
        case MISS_SEMI :
            new->code = 'k';
            new->msg = "Miss Semicon";
            break;
        case MISS_RPARENT :
            new->code = 'l';
            new->msg = "Miss Rparent";
            break;
        case MISS_RBRACK :
            new->code = 'm';
            new->msg = "Miss Rbrack";
            break;
        case MISS_WHILE :
            new->code = 'n';
            new->msg = "Miss While";
            break;
        case CONST_NOT_INTCHAR :
            new->code = 'o';
            new->msg = "Const Value Illegal";
            break;
        case CALL_UNRET_FUNC :
            new->code = 'z';
            new->msg = "**Use No Return Function";
            break;
        case NOT_A_FACTOR :
            new->code = 'z';
            new->msg = "**Not A Factor";
            break;
        case MISS_SIGLGQUO :
            new->code = 'a';
            new->msg = "Miss Sinqle Quote";
            break;
        case MISS_DOUBQUO :
            new->code = 'a';
            new->msg = "Miss Double Quote";
            break;
        case MISS_EQL :
            new->code = 'a';
            new->msg = "Miss Equal";
            break;
        case ZERO_AHEAD:
            new->code = 'a';
            new->msg = "NUMBER WITH ZERO AHEAD";
            break;
        case STR_ILLEGAL_CHAR:
            new->code = 'a';
            new->msg = "ILLEGAL CHAR IN STRING";
            break;
        default:
            break;
    }
    insertError(new);
}

void insertError(struct ErrMsg *new) {
    struct ErrMsg *e;
    if (err == NULL) {
        err = new;
    } else {
        e = err;
        while (e->next != NULL) { e = e->next; }
        e->next = new;
        /*if (new->line >= e->line) {
            if (e->next == NULL) {
                e->next = new;
                break;
            } else if (new->line <= e->next->line) {
                new->next = e->next;
                e->next = new;
                break;
            }
        }else{

        }
    }*/
    }
}