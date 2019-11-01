#include <stdlib.h>
#include "error.h"
#include "symbleTable.h"

void error(int line, int code) {
    struct ErrMsg *new = (struct ErrMsg *) malloc(sizeof(struct ErrMsg));
    new->line = line;
    new->next = NULL;
    switch (code) {
        case UNRECOGNIZED:
            new->code = 'z';
            break;
        case REDEFINED_GLO :
            new->code = 'a';break;
        case REDEFINED_LOC :
            new->code = 'b';break;
        case NOTDEFINED :
            new->code = 'c';break;
        case PARASIZE_MISMATCH :
            new->code = 'd';break;
        case PARATYPE_MISMATCH :
            new->code = 'e';break;
        case CONDIT_ILLEGAL :
            new->code = 'f';break;
        case FORBID_RET :
            new->code = 'g';break;
        case MISS_RET :
            new->code = 'h';break;
        case MISMATCH_RET :
            new->code = 'i';break;
        case OFFSET_NOT_INT :
            new->code = 'j';break;
        case CANT_CHANGE_CONST :
            new->code = 'k';break;
        case MISS_SEMI :
            new->code = 'l';break;
        case MISS_RPARENT :
            new->code = 'm';break;
        case MISS_RBRACK :
            new->code = 'n';break;
        case MISS_WHILE :
            new->code = 'o';break;
        case NOT_INT_OR_CHAR :
            new->code = 'p';break;
        case CALL_UNRET_FUNC :
            new->code = 'q';break;
        case NOT_A_FACTOR :
            new->code = 'r';break;
        case MISS_SIGLGQUO :
            new->code = 's';break;
        case MISS_DOUBQUO :
            new->code = 't';break;
        case MISS_EQL :
            new->code = 'u';break;
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
        for (e = err; e->next != NULL; e = e->next) { ;
        }
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