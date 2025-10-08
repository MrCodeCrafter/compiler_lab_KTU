
#include <stdio.h>
#include <string.h>

#define STATES 20
#define SYMBOLS 10

int N_DFA_states, N_symbols;
char symbols[SYMBOLS];
char DFA_finals[STATES];
char DFAtab[STATES][SYMBOLS];

int reachable[STATES];
int distinguishable[STATES][STATES];

void input_DFA() {
    int i, j;
    printf("Enter number of DFA states: ");
    scanf("%d", &N_DFA_states);
    printf("Enter state names (like A B C ...): ");
    for (i = 0; i < N_DFA_states; i++) {
        getchar(); // to consume newline
        scanf("%c", &DFAtab[i][0]); // store state label in DFAtab[i][0]
    }

    printf("Enter number of input symbols: ");
    scanf("%d", &N_symbols);
    printf("Enter the input symbols (like a b ...): ");
    for (i = 0; i < N_symbols; i++)
        scanf(" %c", &symbols[i]);

    printf("\nEnter transition table in format <state> <dest1> <dest2> ...\n");
    for (i = 0; i < N_DFA_states; i++) {
        printf("%c ", 'A' + i);
        for (j = 0; j < N_symbols; j++) {
            scanf(" %c", &DFAtab[i][j]);
        }
    }

    printf("\nEnter number of final states: ");
    int f;
    scanf("%d", &f);
    printf("Enter names of final states: ");
    for (i = 0; i < f; i++)
        scanf(" %c", &DFA_finals[i]);
    DFA_finals[f] = '\0';
}

int is_final(char s) {
    return strchr(DFA_finals, s) != NULL;
}

void print_transition_table() {
    int i, j;
    printf("\nMINIMIZED DFA TRANSITION TABLE:\n");
    printf("STATE ");
    for (i = 0; i < N_symbols; i++)
        printf("%c   ", symbols[i]);
    printf("\n----------------------\n");
    for (i = 0; i < N_DFA_states; i++) {
        printf(" %c  | ", 'A' + i);
        for (j = 0; j < N_symbols; j++)
            printf("%c   ", DFAtab[i][j]);
        printf("\n");
    }
    printf("Final states: %s\n", DFA_finals);
}

void minimize_DFA() {
    int i, j, k;
    // Initialize distinguishability table
    for (i = 0; i < N_DFA_states; i++)
        for (j = 0; j < N_DFA_states; j++)
            distinguishable[i][j] = 0;

    // Mark pairs (p, q) where one is final and the other is not
    for (i = 0; i < N_DFA_states; i++) {
        for (j = i + 1; j < N_DFA_states; j++) {
            if (is_final('A' + i) != is_final('A' + j))
                distinguishable[i][j] = 1;
        }
    }

    int changed = 1;
    while (changed) {
        changed = 0;
        for (i = 0; i < N_DFA_states; i++) {
            for (j = i + 1; j < N_DFA_states; j++) {
                if (distinguishable[i][j]) continue;
                for (k = 0; k < N_symbols; k++) {
                    int p = DFAtab[i][k] - 'A';
                    int q = DFAtab[j][k] - 'A';
                    if (distinguishable[p][q] || distinguishable[q][p]) {
                        distinguishable[i][j] = 1;
                        changed = 1;
                        break;
                    }
                }
            }
        }
    }

    // Print groups
    int group[STATES];
    int group_count = 0;
    for (i = 0; i < N_DFA_states; i++) group[i] = -1;

    printf("\nMINIMIZED DFA GROUPS:\n");
    for (i = 0; i < N_DFA_states; i++) {
        if (group[i] == -1) {
            group[i] = group_count;
            printf("%c = { %c", 'A' + group_count, 'A' + i);
            for (j = i + 1; j < N_DFA_states; j++) {
                if (!distinguishable[i][j]) {
                    group[j] = group_count;
                    printf(", %c", 'A' + j);
                }
            }
            printf(" }\n");
            group_count++;
        }
    }

    // Print minimized table
    printf("\nMINIMIZED DFA TRANSITION TABLE:\n");
    printf("STATE ");
    for (i = 0; i < N_symbols; i++)
        printf("%c   ", symbols[i]);
    printf("\n----------------------\n");

    for (i = 0; i < group_count; i++) {
        printf(" %c  | ", 'A' + i);
        for (j = 0; j < N_symbols; j++) {
            // Find transition from any state in the group
            int from_state = -1, to_state = -1;
            for (k = 0; k < N_DFA_states; k++) {
                if (group[k] == i) {
                    from_state = k;
                    break;
                }
            }
            to_state = DFAtab[from_state][j] - 'A';
            printf("%c   ", 'A' + group[to_state]);
        }
        printf("\n");
    }

    printf("\nFINAL STATES IN MINIMIZED DFA:\n");
    for (i = 0; i < group_count; i++) {
        for (j = 0; j < N_DFA_states; j++) {
            if (group[j] == i && is_final('A' + j)) {
                printf("%c ", 'A' + i);
                break;
            }
        }
    }
    printf("\n");
}

int main() {
    input_DFA();
    printf("\n==============================\n");
    print_transition_table();
    minimize_DFA();
    return 0;
}

