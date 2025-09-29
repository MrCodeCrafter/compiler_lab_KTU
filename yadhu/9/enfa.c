#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_STATES 26
#define MAX_SYMBOLS 10

int n;
int symbol_count;
char symbols[MAX_SYMBOLS];
int transitions[MAX_STATES][MAX_SYMBOLS+1][MAX_STATES];

void find_epsilon_closure(int state, int closure[], int n){
    if (closure[state]) return;
    closure[state] = 1;
    for (int i = 0; i < n; i++) {
        if (transitions[state][0][i]) {
            find_epsilon_closure(i, closure, n);
        }
    }
}

void print_epsilon_closures(){
    printf("\nEpsilon Closures Table:\n");
    printf("State | Epsilon Closure\n");
    printf("-----------------------\n");
    for (int i = 0; i < n; i++) {
        int closure[MAX_STATES] = {0};
        find_epsilon_closure(i, closure, n);
        printf("  %c   | {", 'A' + i);
        int first = 1;
        for (int j = 0; j < n; j++) {
            if (closure[j]) {
                if (!first) printf(", ");
                printf("%c", 'A' + j);
                first = 0;
            }
        }
        printf("}\n");
    }
}

int symbol_to_index(char c) {
    for (int i = 1; i <= symbol_count; i++) {
        if (symbols[i-1] == c) return i;
    }
    return -1;
}

int main() {
    printf("Enter the number of states (max 26): ");
    scanf("%d", &n);
    if (n > MAX_STATES) {
        printf("Max states allowed is 26.\n");
        return 1;
    }
    printf("Enter the number of input symbols (excluding epsilon): ");
    scanf("%d", &symbol_count);
    printf("Enter the input symbols (without spaces, e.g. abcd): ");
    scanf("%s", symbols);

    for (int i = 0; i < MAX_STATES; i++) {
        for (int j = 0; j <= MAX_SYMBOLS; j++) {
            for (int k = 0; k < MAX_STATES; k++) {
                transitions[i][j][k] = 0;
            }
        }
    }

    int t;
    printf("Enter the number of transitions: ");
    scanf("%d", &t);

    printf("Enter transitions in format: State + symbol -> State\n");
    printf("Use uppercase letters for states (A-Z) and 'e' for epsilon.\n");
    for (int i = 0; i < t; i++) {
        char from, arrow[3], symbol, to;
        scanf(" %c + %c %2s %c", &from, &symbol, arrow, &to);

        int from_index = from - 'A';
        int to_index = to - 'A';

        int sym_index = (symbol == 'e') ? 0 : symbol_to_index(symbol);
        if (sym_index == -1) {
            printf("Invalid symbol %c. Try again.\n", symbol);
            i--;
            continue;
        }
        transitions[from_index][sym_index][to_index] = 1;

        if (symbol == 'e')
            printf("Recorded epsilon transition: %c -> %c\n", from, to);
        else
            printf("Recorded transition: %c + %c -> %c\n", from, symbol, to);
    }

    print_epsilon_closures();

    return 0;
}

/*Enter the number of states (max 26): 3
Enter the number of input symbols (excluding epsilon): 2
Enter the input symbols (without spaces, e.g. abcd): ab
Enter the number of transitions: 4
Enter transitions in format: State + symbol -> State
Use uppercase letters for states (A-Z) and 'e' for epsilon.
A + a -> B
Recorded transition: A + a -> B
B + e -> C
Recorded epsilon transition: B -> C
C + b -> A
Recorded transition: C + b -> A
A + e -> C
Recorded epsilon transition: A -> C

Epsilon Closures Table:
State | Epsilon Closure
-----------------------
  A   | {A, C}
  B   | {B, C}
  C   | {C}
*/

