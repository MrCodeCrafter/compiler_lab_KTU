%{
#include <stdio.h>
#include <string.h>

typedef struct {
    char name[50];
    int count;
} CountEntry;

CountEntry keywords[] = {
    {"int",0},{"if",0},{"return",0},{"for",0},{"while",0},{"else",0}
};
int n_keywords = sizeof(keywords)/sizeof(keywords[0]);

CountEntry identifiers[1000];
int n_identifiers = 0;

CountEntry operators[50];
int n_operators = 0;

int is_keyword(char *s) {
    for(int i=0;i<n_keywords;i++)
        if(strcmp(s, keywords[i].name)==0) {
            keywords[i].count++;
            return 1;
        }
    return 0;
}

void add_identifier(char *s) {
    for(int i=0;i<n_identifiers;i++)
        if(strcmp(identifiers[i].name,s)==0) {
            identifiers[i].count++;
            return;
        }
    strcpy(identifiers[n_identifiers].name,s);
    identifiers[n_identifiers].count = 1;
    n_identifiers++;
}

void add_operator(char *s) {
    for(int i=0;i<50;i++)
        if(strcmp(operators[i].name,s)==0) {
            operators[i].count++;
            return;
        }
    strcpy(operators[n_operators].name,s);
    operators[n_operators].count = 1;
    n_operators++;
}
%}

%%

"++"|"--"|"=="|"!="|"<="|">="  { add_operator(yytext); }
"+"|"-"|"*"|"/"|"="|">"|"<"|"%"  { add_operator(yytext); }

[A-Za-z_][A-Za-z0-9_]*  {
    if(!is_keyword(yytext)) add_identifier(yytext);
}

[ \t\n]+    { /* ignore whitespace */ }

.           { /* ignore everything else */ }

%%

int yywrap() { return 1; }

int main() {
    FILE *fp = fopen("input_file.c","r");
    if(!fp){ perror("File open failed"); return 1;}
    yyin = fp;
    yylex();
    fclose(fp);

    printf("KEYWORDS:\n");
    for(int i=0;i<n_keywords;i++)
        if(keywords[i].count>0)
            printf("%-10s : %d\n", keywords[i].name, keywords[i].count);

    printf("\nIDENTIFIERS:\n");
    for(int i=0;i<n_identifiers;i++)
        printf("%-10s : %d\n", identifiers[i].name, identifiers[i].count);

    printf("\nOPERATORS:\n");
    for(int i=0;i<n_operators;i++)
        printf("%-5s : %d\n", operators[i].name, operators[i].count);

    return 0;
}

