#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_QUADS 100
#define MAX_VARS 50

typedef struct {
    char op[3];
    char o1[5];
    char o2[5];
    char res[5];
    int def_o1_idx;
    int def_o2_idx;
} Quadruple;

typedef struct {
    char var_name[5];
    int defining_quad_idx;
} VarDefinition;

VarDefinition var_defs[MAX_VARS];
int num_var_defs = 0;

int get_var_def_idx(const char *var_name) {
    for (int i = 0; i < num_var_defs; i++) {
        if (strcmp(var_defs[i].var_name, var_name) == 0) {
            return i;
        }
    }
    if (num_var_defs < MAX_VARS) {
        strcpy(var_defs[num_var_defs].var_name, var_name);
        var_defs[num_var_defs].defining_quad_idx = -1;
        return num_var_defs++;
    }
    fprintf(stderr, "Error: Too many variables.\n");
    exit(1);
}

void update_var_def(const char *var_name, int quad_idx) {
    int idx = get_var_def_idx(var_name);
    var_defs[idx].defining_quad_idx = quad_idx;
}

int get_defining_quad_index(const char *var_name) {
    if (var_name[0] == '#' || (var_name[0] >= '0' && var_name[0] <= '9')) {
        return -2;
    }
    for (int i = 0; i < num_var_defs; i++) {
        if (strcmp(var_defs[i].var_name, var_name) == 0) {
            return var_defs[i].defining_quad_idx;
        }
    }
    return -1;
}

int is_common_subexpression(Quadruple q[], int index, char res[]) {
    for (int i = 0; i < index; i++) {
        if (strcmp(q[i].op, q[index].op) == 0 &&
            strcmp(q[i].o1, q[index].o1) == 0 &&
            strcmp(q[i].o2, q[index].o2) == 0)
        {
            if (q[i].def_o1_idx == q[index].def_o1_idx &&
                q[i].def_o2_idx == q[index].def_o2_idx)
            {
                strcpy(res, q[i].res);
                return 1;
            }
        }
    }
    return 0;
}

void replace_redundant_result(Quadruple q[], int n, const char *old_res, const char *new_res) {
    for (int i = 0; i < n; i++) {
        if (strcmp(q[i].o1, old_res) == 0) {
            strcpy(q[i].o1, new_res);
        }
        if (strcmp(q[i].o2, old_res) == 0) {
            strcpy(q[i].o2, new_res);
        }
    }
}

int main() {
    FILE *inputFile, *outputFile;

    inputFile = fopen("in.txt", "r");
    if (inputFile == NULL) {
        printf("Error opening input file 'in.txt'!\n");
        return 1;
    }

    outputFile = fopen("out.txt", "w");
    if (outputFile == NULL) {
        printf("Error opening output file 'output.txt'!\n");
        fclose(inputFile);
        return 1;
    }

    Quadruple q[MAX_QUADS];
    int n = 0;

    char header[100];
    if (fgets(header, sizeof(header), inputFile) == NULL) {
        printf("Error reading header from input file.\n");
        fclose(inputFile);
        fclose(outputFile);
        return 1;
    }

    while (fscanf(inputFile, "%s %s %s %s", q[n].op, q[n].o1, q[n].o2, q[n].res) == 4) {
        if (strcmp(q[n].op, "=") == 0) {
            update_var_def(q[n].res, n);
        } else {
            q[n].def_o1_idx = get_defining_quad_index(q[n].o1);
            q[n].def_o2_idx = get_defining_quad_index(q[n].o2);
        }
        update_var_def(q[n].res, n);
        n++;
    }

    fclose(inputFile);

    Quadruple result[MAX_QUADS];
    int result_count = 0;

    for(int i = 0; i < num_var_defs; i++) {
        var_defs[i].defining_quad_idx = -1;
    }

    for (int i = 0; i < n; i++) {
        char common_res[5];
        if (is_common_subexpression(q, i, common_res)) {
            replace_redundant_result(q, n, q[i].res, common_res);
        } else {
            result[result_count] = q[i];
            if (strcmp(result[result_count].op, "=") != 0) {
                result[result_count].def_o1_idx = get_defining_quad_index(result[result_count].o1);
                result[result_count].def_o2_idx = get_defining_quad_index(result[result_count].o2);
            }
            update_var_def(result[result_count].res, result_count);
            result_count++;
        }
    }

    fprintf(outputFile, "OP\tO1\tO2\tRES\n");
    for (int i = 0; i < result_count; i++) {
        fprintf(outputFile, "%s\t%s\t%s\t%s\n", result[i].op, result[i].o1, result[i].o2, result[i].res);
    }

    fclose(outputFile);

    printf("Common subexpression elimination completed. Optimized output written to 'output.txt'.\n");

    return 0;
}
