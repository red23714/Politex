#include <stdio.h>

#define STATE_DEFAULT 0
#define STATE_IN_STRING 1
#define STATE_IN_CHAR 2
#define STATE_IN_SINGLE_COMMENT 3
#define STATE_IN_MULTI_COMMENT 4

void remove_comments(FILE* input, FILE* output) {
    int c, next;
    int state = STATE_DEFAULT;

    int count_sleshik = 0;
    while ((c = fgetc(input)) != EOF) {
        if (c == '\\')count_sleshik++;
        switch (state) {
        case STATE_DEFAULT:
            if (c == '/') {
                next = fgetc(input);
                if (next == EOF) {
                    fputc(c, output);
                    return;
                }
                if (next == '/') {
                    state = STATE_IN_SINGLE_COMMENT;
                }
                else if (next == '*') {
                    state = STATE_IN_MULTI_COMMENT;
                }
                else {
                    fputc(c, output);
                    fseek(input, -1, SEEK_CUR);
                }
            }
            else if (c == '"') {
                state = STATE_IN_STRING;
                fputc(c, output);
            }
            else if (c == '\'') {
                state = STATE_IN_CHAR;
                fputc(c, output);
            }
            else {
                fputc(c, output);
            }
            break;

        case STATE_IN_STRING:
            fputc(c, output);
            if (count_sleshik%2==0) {
                if (c == '"') {
                    state = STATE_DEFAULT;
                }
                else if (c == '\n') {
                    state = STATE_DEFAULT;
                }
            }
            break;

        case STATE_IN_CHAR:
            fputc(c, output);
            if (c == '\n') state = STATE_DEFAULT;
            else if (c == '\'' && (count_sleshik % 2 == 0)) state = STATE_DEFAULT;
            break;

        case STATE_IN_SINGLE_COMMENT:
             if (c == '\n') {
                 if (count_sleshik % 2 == 0) {
                     fputc('\n', output);
                     state = STATE_DEFAULT;
                 }
            }
             else if (c == EOF) {
                 return;
             }
            break;

        case STATE_IN_MULTI_COMMENT:
            if (c == '*') {
                next = fgetc(input);
                if (next == EOF)return;
                else if (next == '/') {
                    state = STATE_DEFAULT;
                }
                else {
                    fseek(input, -1, SEEK_CUR);
                }
               
            }
            break;

        default:
            state = STATE_DEFAULT;
            break;
        }
        if (c != '\\' || c == '\n')count_sleshik = 0;
    }
}
