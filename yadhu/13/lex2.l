%{
#include <stdio.h>
#include <string.h>

char *keywords[] = {
    "int","if","else","while","for","return","char","float","double","void"
};
int n_keywords = sizeof(keywords)/sizeof(keywords[0]);

int is_keyword(char *s) {
    for(int i=0; i<n_keywords; i++)
        if(strcmp(s, keywords[i])==0) return 1;
    return 0;
}
%}

%%

"++"|"--"|"=="|"!="|"<="|">="|"+="|"-="|"="|">"|"<"|"\+"|"-"|"*"|"/"  { 
    printf("operator: %s\n", yytext); 
}

[A-Za-z_][A-Za-z0-9_]* {
    if(is_keyword(yytext))
        printf("keyword: %s\n", yytext);
    else
        printf("identifier: %s\n", yytext);
}

[ \t\n]+       { /* ignore whitespace */ }

.              { /* ignore everything else */ }

%%

int yywrap() { return 1; }

int main() {
    FILE *fp = fopen("input_file.c","r");
    if(!fp){ perror("File open failed"); return 1; }
    yyin = fp;
    yylex();
    fclose(fp);
    return 0;
}

