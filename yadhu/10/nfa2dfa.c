// enfa_to_dfa.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXN 50      // maximum NFA states (1..MAXN)
#define MAXA 20      // maximum alphabet symbols (excluding epsilon)
#define MAXDFA 1000  // maximum DFA states (subsets)
 
/* linked list node for NFA transitions */
struct node {
    int st;
    struct node *link;
};

/* DFA state as a set of NFA states (1..nostate) */
struct DState {
    int has[MAXN+1]; // 0/1 membership
};

struct node *transition[MAXN+1][MAXA+1]; // last column index noalpha -> epsilon

/* globals */
int nostate, noalpha, notransition, nofinal, start;
char alphabet[MAXA+1];            // real symbols (no 'e' here)
int finalstate[MAXN+1];
int eclosure[MAXN+1][MAXN+1];     // eclosure[q] as list: eclosure[q][0] = count, then elems
struct DState dfa[MAXDFA];
int complete = -1;                // last DFA index
int usedDFA[MAXDFA];              // 0/1 used markers (for printing)
 
/* helper prototypes */
void insert_transition(int r, char sym, int s);
int find_alpha_index(char c); // returns 0..noalpha-1, or noalpha if 'e', or -1 if invalid
void compute_eclosures(void);
void add_to_eclosure(int q, int v);
int insert_dfa_state(struct DState *st);
int compare_dstate(struct DState *a, struct DState *b);
void print_dstate(const struct DState *st);
void find_and_print_final_dstates(void);

int main(void)
{
    int i,j,k,m,src,dest;
    char sym;
    struct DState startD={0}, tmp={0};
    
    for(i=0;i<=MAXN;i++) for(j=0;j<=MAXA;j++) transition[i][j] = NULL;

    printf("Enter the number of alphabets? (do NOT include 'e' for epsilon)\n");
    if(scanf("%d",&noalpha)!=1) return 0;
    if(noalpha < 0 || noalpha > MAXA) { printf("bad alphabet count\n"); return 0; }
    getchar();
    printf("NOTE:- [ use letter 'e' as epsilon in transitions ]\n");
    printf("Enter alphabets (one per line):\n");
    for(i=0;i<noalpha;i++){
        alphabet[i] = getchar();
        getchar(); // consume newline
    }

    printf("Enter the number of states?\n");
    scanf("%d",&nostate);
    if(nostate < 1 || nostate > MAXN) { printf("bad state count\n"); return 0; }

    printf("Enter the start state?\n");
    scanf("%d",&start);
    if(start < 1 || start > nostate) { printf("bad start state\n"); return 0; }

    printf("Enter the number of final states?\n");
    scanf("%d",&nofinal);
    printf("Enter the final states? (one per line)\n");
    for(i=0;i<nofinal;i++){
        scanf("%d",&finalstate[i]);
    }

    printf("Enter number of transitions?\n");
    scanf("%d",&notransition);
    printf("NOTE:- [Transition is in the form: source_symbol dest  e.g. 1 a 2 or 1 e 2]\n");
    printf("Enter transitions (one per line):\n");
    for(i=0;i<notransition;i++){
        // space before %c to skip whitespace/newline
        if(scanf("%d %c %d",&src,&sym,&dest) != 3){
            printf("invalid transition input\n");
            return 0;
        }
        insert_transition(src, sym, dest);
    }

    /* compute epsilon-closures */
    compute_eclosures();

    /* initialize DFA table */
    for(i=0;i<MAXDFA;i++){
        memset(dfa[i].has,0,sizeof(dfa[i].has));
        usedDFA[i]=0;
    }
    complete = -1;

    /* Start DFA state is epsilon-closure of start */
    for(i=1;i<=nostate;i++) startD.has[i]=0;
    for(i=1;i<=eclosure[start][0];i++){
        int st = eclosure[start][i];
        startD.has[st] = 1;
    }
    insert_dfa_state(&startD);

    printf("\nEquivalent DFA.....\n");
    printf("Transitions of DFA:\n");

    /* Subset construction */
    int idx = 0;
    while(idx <= complete){
        struct DState cur = dfa[idx];
        /* for each actual input symbol (0..noalpha-1) */
        for(k=0;k<noalpha;k++){
            /* compute union T = U for q in cur: delta(q, symbol k), then epsilon-closure of each dest */
            for(j=1;j<=nostate;j++) tmp.has[j]=0;
            for(j=1;j<=nostate;j++){
                if(cur.has[j]){
                    struct node *p = transition[j][k]; // transitions on symbol k from state j
                    while(p){
                        /* include epsilon-closure of destination p->st */
                        int destState = p->st;
                        for(m=1;m<=eclosure[destState][0];m++){
                            int e = eclosure[destState][m];
                            tmp.has[e] = 1;
                        }
                        p = p->link;
                    }
                }
            }
            /* Ensure tmp gets included as DFA state even if empty set (dead state) */
            int inserted = insert_dfa_state(&tmp);
            /* print transition */
            print_dstate(&cur);
            printf("%c\t", alphabet[k]);
            print_dstate(&tmp);
            printf("\n");
        }
        idx++;
    }

    /* Print DFA states */
    printf("\nStates of DFA:\n");
    for(i=0;i<=complete;i++){
        print_dstate(&dfa[i]);
    }

    printf("\nAlphabets:\n");
    for(i=0;i<noalpha;i++) printf("%c\t", alphabet[i]);
    printf("\nStart State:\n");
    print_dstate(&dfa[0]); // start index 0
    printf("\n\nFinal states:\n");
    find_and_print_final_dstates();

    return 0;
}

/* Insert NFA transition into transition[src][sym_index] */
/* sym 'e' -> epsilon (stored at index noalpha), else find symbol index */
void insert_transition(int r, char sym, int s)
{
    if(r < 1 || r > nostate || s < 1 || s > nostate){
        printf("error: bad state in transition\n");
        exit(0);
    }
    int idx = find_alpha_index(sym);
    if(idx == -1){
        printf("error: symbol %c not in alphabet and not 'e'\n", sym);
        exit(0);
    }
    struct node *tmp = (struct node *) malloc(sizeof(struct node));
    tmp->st = s;
    tmp->link = transition[r][idx];
    transition[r][idx] = tmp;
}

/* return index in transition table:
   0..noalpha-1 -> real symbols
   noalpha -> epsilon
   -1 -> invalid symbol
*/
int find_alpha_index(char c)
{
    if(c == 'e') return noalpha; // epsilon slot
    for(int i=0;i<noalpha;i++) if(alphabet[i] == c) return i;
    return -1;
}

/* Compute epsilon closures for all states using DFS/stack */
void compute_eclosures(void)
{
    int i;
    for(i=1;i<=nostate;i++){
        int visited[MAXN+1] = {0};
        int stack[MAXN+1], top = -1;
        eclosure[i][0] = 0;
        /* push i */
        visited[i] = 1;
        eclosure[i][++eclosure[i][0]] = i;
        stack[++top] = i;

        while(top >= 0){
            int v = stack[top--];
            struct node *p = transition[v][noalpha]; // epsilon transitions stored at noalpha
            while(p){
                if(!visited[p->st]){
                    visited[p->st] = 1;
                    eclosure[i][++eclosure[i][0]] = p->st;
                    stack[++top] = p->st;
                }
                p = p->link;
            }
        }
    }
}

/* Insert DFA state if new, return 1 if inserted or 0 if already existed.
   Note: an empty set (all zeros) is a valid DFA state (dead state)
*/
int insert_dfa_state(struct DState *st)
{
    int i;
    for(i=0;i<=complete;i++){
        if(compare_dstate(&dfa[i], st)) return 0;
    }
    if(complete+1 >= MAXDFA){
        printf("Exceeded max DFA states\n");
        exit(1);
    }
    complete++;
    for(i=1;i<=nostate;i++) dfa[complete].has[i] = st->has[i];
    usedDFA[complete]=1;
    return 1;
}

/* Compare two DFA sets */
int compare_dstate(struct DState *a, struct DState *b)
{
    for(int i=1;i<=nostate;i++) if(a->has[i] != b->has[i]) return 0;
    return 1;
}

/* Print DFA state as {q1,q2,...} or { } for empty */
void print_dstate(const struct DState *st)
{
    int first = 1;
    printf("{");
    for(int i=1;i<=nostate;i++){
        if(st->has[i]){
            if(!first) printf(",");
            printf("q%d", i);
            first = 0;
        }
    }
    if(first) printf(" "); // show empty set visibly
    printf("}\t");
}

/* Determine and print DFA final states (those which include any NFA final) */
void find_and_print_final_dstates(void)
{
    int i, j, k;
    for(i=0;i<=complete;i++){
        int isFinal = 0;
        for(j=1;j<=nostate && !isFinal;j++){
            if(dfa[i].has[j]){
                for(k=0;k<nofinal;k++){
                    if(finalstate[k] == j){
                        isFinal = 1;
                        break;
                    }
                }
            }
        }
        if(isFinal){
            print_dstate(&dfa[i]);
            printf("\n");
        }
    }
}

