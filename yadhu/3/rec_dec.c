#include <stdio.h>
#include <string.h>
#include <ctype.h>

char input[100];
int i,err;

void E();
void Eprime();
void T();
void Tprime();
void F();

void E(){
	T();
	Eprime();

}

void Eprime(){
	if((input[i]=='+') || (input[i]=='-')){
		i++;
		T();
		Eprime();
	}
	else if ((input[i]=='^') || (input[i]=='%')){
		err=4;
		
	}
}

void T(){
	F();
	Tprime();
}

void Tprime(){
	if((input[i]=='*') || (input[i]=='/')){
		i++;
		F();
		Tprime();
	}
	else if ((input[i]=='^') || (input[i]=='%')){
		err=4;
		
	}
}

void F(){
	if(isalnum(input[i])){
		i++;
	}
	else if (input[i]=='('){
		i++;
		E();
		if (input[i]==')'){
			i++;
		}
		else{
			err=1;
		}
	}
	else if ((input[i]=='+') || (input[i]=='-') || (input[i]=='*') || (input[i]=='/')){
			err=2;	
	}
	else{
		err=3;
	}
}

int main(){
	i=0;
	err=0;
	printf("Enter an expression:");
	fgets(input,sizeof(input),stdin);
	input[strcspn(input,"\n")]='\0';
	E();
	if (strlen(input)==i && err==0){
		printf("%s is accepted \n",input);
	}
	else if (err==1){
		printf("%s is not accepted \n",input);
		printf("Missing Paranthesis ')'\n");
	}
	else if (err==2){
		printf("%s is not accepted \n",input);
		printf("Invalid Operator\n");
	}
	else if (err==3){
		printf("%s is not accepted \n",input);
		printf("Missing operand / Not Possible to parse\n");
	}
	else if (err==4){
		printf("%s is not accepted \n",input);
		printf("Invalid Operator\n");
	}
	else{
		printf("Invalid Expression\n");
	}
	return 0;
}

