#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Global variables
int count;
char production[10][10];
char first_sets[10][10];
char follow_sets[10][10];
char non_terminals[10];
int nt_count = 0;

// Function prototypes
void calculate_first(char c);
void calculate_follow(char c);
void add_to_set(char* set, char c);
int get_nt_index(char c);
void print_set(char* set);

int main() {
    printf("Enter the number of productions: ");
    scanf("%d", &count);
    printf("Enter the productions (e.g., S->ACB or B-># for epsilon):\n");
    for (int i = 0; i < count; i++) {
        scanf("%s", production[i]);
    }

    // Populate unique non-terminals
    for (int i = 0; i < count; i++) {
        int exists = 0;
        for (int j = 0; j < nt_count; j++) {
            if (non_terminals[j] == production[i][0]) {
                exists = 1;
                break;
            }
        }
        if (!exists) {
            non_terminals[nt_count++] = production[i][0];
        }
    }

    // Calculate FIRST and FOLLOW sets for all non-terminals
    for (int i = 0; i < nt_count; i++) {
        calculate_first(non_terminals[i]);
    }
    for (int i = 0; i < nt_count; i++) {
        // Initialize follow sets to avoid issues in recursive calls
        follow_sets[i][0] = '\0';
    }
    for (int i = 0; i < nt_count; i++) {
        calculate_follow(non_terminals[i]);
    }

    // Print results
    printf("\n--- RESULTS ---\n");
    for (int i = 0; i < nt_count; i++) {
        char nt = non_terminals[i];
        printf("FIRST(%c) = ", nt);
        print_set(first_sets[get_nt_index(nt)]);
        printf("\n");
    }
    printf("\n");
    for (int i = 0; i < nt_count; i++) {
        char nt = non_terminals[i];
        printf("FOLLOW(%c) = ", nt);
        print_set(follow_sets[get_nt_index(nt)]);
        printf("\n");
    }

    return 0;
}

void calculate_first(char c) {
    int c_idx = get_nt_index(c);
    if (c_idx == -1 || first_sets[c_idx][0] != '\0') return;

    for (int i = 0; i < count; i++) {
        if (production[i][0] == c) {
            // FIX: Skip direct left-recursive rules to prevent infinite loops
            if (production[i][3] == c) {
                continue;
            }
            
            if (production[i][3] == '#') {
                add_to_set(first_sets[c_idx], '#');
                continue;
            }
            
            int all_rhs_nullable = 1;
            for (int j = 3; j < strlen(production[i]); j++) {
                char symbol = production[i][j];
                int has_epsilon = 0;

                if (!isupper(symbol)) {
                    add_to_set(first_sets[c_idx], symbol);
                    all_rhs_nullable = 0;
                    break;
                }
                
                calculate_first(symbol);
                int symbol_idx = get_nt_index(symbol);
                for (int k = 0; k < strlen(first_sets[symbol_idx]); k++) {
                    if (first_sets[symbol_idx][k] == '#') {
                        has_epsilon = 1;
                    } else {
                        add_to_set(first_sets[c_idx], first_sets[symbol_idx][k]);
                    }
                }
                
                if (!has_epsilon) {
                    all_rhs_nullable = 0;
                    break;
                }
            }
            if (all_rhs_nullable) {
                add_to_set(first_sets[c_idx], '#');
            }
        }
    }
}

void calculate_follow(char c) {
    int c_idx = get_nt_index(c);
    if(c_idx == -1) return;
    
    // Add $ to FOLLOW of start symbol only once
    if (c == non_terminals[0] && strchr(follow_sets[c_idx], '$') == NULL) {
        add_to_set(follow_sets[c_idx], '$');
    }

    for (int i = 0; i < count; i++) {
        for (int j = 3; j < strlen(production[i]); j++) {
            if (production[i][j] == c) {
                int k = j + 1;
                int follow_loop_done = 0;
                while(!follow_loop_done) {
                    if (k < strlen(production[i])) {
                        char next_symbol = production[i][k];
                        int has_epsilon = 0;

                        if (!isupper(next_symbol)) {
                            add_to_set(follow_sets[c_idx], next_symbol);
                            follow_loop_done = 1;
                        } else {
                            int next_idx = get_nt_index(next_symbol);
                            for(int l=0; l<strlen(first_sets[next_idx]); l++) {
                                if(first_sets[next_idx][l] == '#') {
                                    has_epsilon = 1;
                                } else {
                                    add_to_set(follow_sets[c_idx], first_sets[next_idx][l]);
                                }
                            }
                        }
                        if (!has_epsilon) {
                            follow_loop_done = 1;
                        }
                        k++;
                    } else {
                        if (production[i][0] != c) {
                            calculate_follow(production[i][0]);
                            int head_idx = get_nt_index(production[i][0]);
                            for(int l=0; l<strlen(follow_sets[head_idx]); l++) {
                                add_to_set(follow_sets[c_idx], follow_sets[head_idx][l]);
                            }
                        }
                        follow_loop_done = 1;
                    }
                }
            }
        }
    }
}

void print_set(char* set) {
    printf("{ ");
    for (int i = 0; i < strlen(set); i++) {
        printf("%c", set[i]);
        if (i < strlen(set) - 1) {
            printf(", ");
        }
    }
    printf(" }");
}

void add_to_set(char* set, char c) {
    if (strchr(set, c) == NULL) {
        int len = strlen(set);
        set[len] = c;
        set[len + 1] = '\0';
    }
}

int get_nt_index(char c) {
    for (int i = 0; i < nt_count; i++) {
        if (non_terminals[i] == c) return i;
    }
    return -1;
}

