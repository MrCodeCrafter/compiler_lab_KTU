#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

char *input, *converted;
int i = 0;
char lasthandle[6], stack[50], handles[][7] = {")E(", "E*E", "E+E", "i", "E^E", "E-E", "E/E"};
int top = 0, l;

char prec[9][9] = {
    /*input symbols →     +    -    *    /    ^    i    (    )    $  */
    /*stack symbols ↓ */
    /*  + */           '>', '>', '<', '<', '<', '<', '<', '>', '>',
    /*  - */           '>', '>', '<', '<', '<', '<', '<', '>', '>',
    /*  * */           '>', '>', '>', '>', '<', '<', '<', '>', '>',
    /*  / */           '>', '>', '>', '>', '<', '<', '<', '>', '>',
    /*  ^ */           '>', '>', '>', '>', '<', '<', '<', '>', '>',
    /*  i */           '>', '>', '>', '>', '>', 'e', 'e', '>', '>',
    /*  ( */           '<', '<', '<', '<', '<', '<', '<', '>', 'e',
    /*  ) */           '>', '>', '>', '>', '>', 'e', 'e', '>', '>',
    /*  $ */           '<', '<', '<', '<', '<', '<', '<', '<', '>',
};

int getindex(char c) {
    switch (c) {
        case '+': return 0;
        case '-': return 1;
        case '*': return 2;
        case '/': return 3;
        case '^': return 4;
        case 'i': return 5;
        case '(': return 6;
        case ')': return 7;
        case '$': return 8;
    }
    return -1; // error case
}

int shift() {
    stack[++top] = *(converted + i++);
    stack[top + 1] = '\0';
}

int reduce() {
    int i, len, found, t;
    for (i = 0; i < 7; i++) {
        len = strlen(handles[i]);
        if (stack[top] == handles[i][0] && top + 1 >= len) {
            found = 1;
            for (t = 0; t < len; t++) {
                if (stack[top - t] != handles[i][t]) {
                    found = 0;
                    break;
                }
            }
            if (found == 1) {
                stack[top - t + 1] = 'E';
                top = top - t + 1;
                strcpy(lasthandle, handles[i]);
                stack[top + 1] = '\0';
                return 1;
            }
        }
    }
    return 0;
}

void dispstack() {
    int j;
    for (j = 0; j <= top; j++)
        printf("%c", stack[j]);
}

void dispinput() {
    int j;
    for (j = i; j < l; j++)
        printf("%c", *(converted + j));
}

void tokenize_input(char *raw) {
    converted = (char *)malloc(100 * sizeof(char));
    int j = 0;
    int len = strlen(raw);
    for (int k = 0; k < len; ) {
        if (isalpha(raw[k])) {
            // valid variable or identifier
            while (isalnum(raw[k])) k++;  // skip full identifier
            converted[j++] = 'i';
        } else if (isdigit(raw[k])) {
            while (isdigit(raw[k])) k++;  // numbers treated as identifiers
            converted[j++] = 'i';
        } else if (strchr("+-*/^()$", raw[k])) {
            converted[j++] = raw[k++];
        } else {
            printf("\nInvalid character '%c' in input. Exiting.\n", raw[k]);
            exit(1);
        }
    }
    converted[j++] = '$';
    converted[j] = '\0';
}

void main() {
    char raw_input[100];
    input = (char *)malloc(100 * sizeof(char));
    
    printf("\nEnter the expression:\n");
    scanf("%s", raw_input);

    tokenize_input(raw_input);  // convert all variables to 'i'
    l = strlen(converted);
    
    strcpy(stack, "$");
    printf("\nSTACK\tINPUT\tACTION");

    while (i <= l) {
        shift();
        printf("\n");
        dispstack();
        printf("\t");
        dispinput();
        printf("\tShift");
        if (prec[getindex(stack[top])][getindex(converted[i])] == '>') {
            while (reduce()) {
                printf("\n");
                dispstack();
                printf("\t");
                dispinput();
                printf("\tReduced: E->%s", lasthandle);
            }
        }
    }

    if (strcmp(stack, "$E$") == 0)
        printf("\nAccepted \n");
    else
        printf("\nNot Accepted\n");
}
