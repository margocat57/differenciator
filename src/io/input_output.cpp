#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include "../core/tree_func.h"
#include "input_output.h"
#include "../utils/metki.h"
#include "../dump/graphviz_dump.h"
#include "../core/forest.h"
#include "../core/operator_func.h"

const size_t MAX_BUFFER_SIZE = 50;

//----------------------------------------------------------------------------
// Helping functions to find spaces

static void skip_space(char* str, size_t* pos){
    assert(str);
    assert(pos);

    char ch = str[(*pos)];
    while(isspace(ch) && ch != '\0'){
        (*pos)++;
        ch = str[(*pos)];
    }
}

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
// Creating buffer for reading akinator info from disk

static bool is_stat_err(const char *name_of_file, struct stat *all_info_about_file);

static char* read_file_to_string_array(const char *name_of_file){
    assert(name_of_file != NULL);
    FILE *fptr = fopen(name_of_file, "r");
    if(!fptr){
        fprintf(stderr, "Can't open file\n");
        return NULL;
    }

    struct stat file_info = {};
    if(is_stat_err(name_of_file, &(file_info))){
        return NULL;
    }

    char *all_strings_in_file = (char *)calloc(file_info.st_size + 1, sizeof(char));
    if(!all_strings_in_file){
        fprintf(stderr, "Array for strings allocation error\n");
        return NULL;
    }

    if(fread(all_strings_in_file, sizeof(char), file_info.st_size, fptr) != file_info.st_size){
        fprintf(stderr, "Can't read all symbols from file\n");
        return NULL;
    }

    fclose(fptr);
    return all_strings_in_file;
}

static bool is_stat_err(const char *name_of_file, struct stat *all_info_about_file){
    assert(name_of_file != NULL);
    assert(all_info_about_file != NULL);

    if (stat(name_of_file, all_info_about_file) == -1){
        perror("Stat error");
        fprintf(stderr, "Error code: %d\n", errno);
        return true;
    }
    return false;
}

static void buffer_free(char* buffer){
    if(buffer){
        memset(buffer, 0, strlen(buffer));
        free(buffer);
    }
}

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
// Make akinator tree

/*
    Grammar of language:
    G ::= "E$"
    E ::= T{[+,-] T}*
    T ::= F{[*,/,^] F}*
    F ::= ["sin", "cos", ... ] P | P
    P ::= (E) | N | V
*/

// do/while
/* 
    ! new Grammar of language:
    G ::= "E$"
    E ::= T{[+,-] T}*
    T ::= P{[*,/,^] P}*
    P ::= (E) | N | V | F
    N ::= ['0' - '9']+
    V ::= ['a' - 'z', _ ] ['a' - 'z', '0' - '9' , _ ]*
    F ::= ["sin", "cos", ... ] '(' E ')'
*/



static TreeNode_t* GetG(size_t* pos, char* buffer, metki *mtk, TreeErr_t* err);

static TreeNode_t* GetE(size_t* pos, char* buffer, metki *mtk, TreeErr_t* err);

static TreeNode_t* GetT(size_t* pos, char* buffer, metki *mtk, TreeErr_t* err);

static TreeNode_t* GetF(size_t* pos, char* buffer, metki *mtk, TreeErr_t* err);

static TreeNode_t* GetP(size_t* pos, char* buffer, metki *mtk, TreeErr_t* err);

TreeNode_t* GetN(size_t* pos, char* buffer, TreeErr_t* err);

TreeNode_t* GetV(size_t* pos, char* buffer, metki *mtk, TreeErr_t* err);

Forest_t* MakeDiffForest(const char *name_of_file){
    assert(name_of_file);

    char* buffer = read_file_to_string_array(name_of_file);
    if(!buffer){
        return NULL;
    }
    TreeHead_t* head = TreeCtor();
    Forest_t* forest = ForestCtor(10);
    size_t pos = 0;
    TreeErr_t err = NO_MISTAKE_T;
    head->root = GetG(&pos, buffer, forest->mtk, &err);
    if(!head->root){
        ForestDtor(forest);
        buffer_free(buffer);
        return NULL;
    }
    DEBUG_TREE(
    if(TreeVerify(head)){
        fprintf(stderr, "File is not correct - can't work with created tree\n");
        free(head); // потому что добавляем в лес уже после всех проверок
        ForestDtor(forest);
        buffer_free(buffer);
        return NULL;
    }
    )
    ForestAddElem(head, forest);
    buffer_free(buffer);
    return forest;
}

static TreeNode_t* GetG(size_t* pos, char* buffer, metki *mtk, TreeErr_t* err){
    skip_space(buffer, pos); 
    TreeNode_t* head = GetE(pos, buffer, mtk, err);
    skip_space(buffer, pos); 
    if(buffer[*pos] != '$'){
        fprintf(stderr, "Syntax error\n");
        *err = INCORR_FILE;
        TreeDelNodeRecur(head);
        return NULL;
    }
    tree_dump_func(head, "Before ret GetG node %s", __FILE__, __func__, __LINE__, mtk, buffer + *pos);
    return head;
}

static TreeNode_t* GetE(size_t* pos, char* buffer, metki *mtk, TreeErr_t* err){
    skip_space(buffer, pos); 
    TreeNode_t* left = GetT(pos, buffer, mtk, err);
    skip_space(buffer, pos); 
    while(buffer[*pos] == '+' || buffer[*pos] == '-'){
        int op = buffer[*pos];
        (*pos)++;
        skip_space(buffer, pos); 

        // короче глобально идея такая - вернуть ошибку может только GetP, вот и ее и надо проверять
        TreeNode_t* right = GetT(pos, buffer, mtk, err);
        if(*err){
            TreeDelNodeRecur(left);
            return NULL;
        }
        skip_space(buffer, pos); 

        TreeNode_t *new_node = NULL;
        if(op == '+'){
            new_node = NodeCtor(OPERATOR, (TreeElem_t){.op = OP_ADD}, NULL, left, right);
        }
        else if(op == '-'){
            new_node = NodeCtor(OPERATOR, (TreeElem_t){.op = OP_SUB}, NULL, left, right);
        }
        left->parent = new_node;
        right->parent = new_node;
        left = new_node;
    }
    tree_dump_func(left, "Before ret GetE node %s", __FILE__, __func__, __LINE__, mtk, buffer + *pos);
    return left;
}

static TreeNode_t* GetT(size_t* pos, char* buffer, metki *mtk, TreeErr_t* err){
    skip_space(buffer, pos); 
    TreeNode_t* left = GetF(pos, buffer, mtk, err);
    skip_space(buffer, pos); 

    while(buffer[*pos] == '*' || buffer[*pos] == '/' || buffer[*pos] == '^'){
        int op = buffer[*pos];
        (*pos)++;
        skip_space(buffer, pos); 

        TreeNode_t* right = GetF(pos, buffer, mtk, err);
        if(*err){
            TreeDelNodeRecur(left);
            return NULL;
        }
        skip_space(buffer, pos); 

        TreeNode_t *new_node = NULL;
        if(op == '*'){
            new_node = NodeCtor(OPERATOR, (TreeElem_t){.op = OP_MUL}, NULL, left, right);
        }
        else if(op == '/'){
            new_node = NodeCtor(OPERATOR, (TreeElem_t){.op = OP_DIV}, NULL, left, right);
        }
        else if(op == '^'){
            new_node = NodeCtor(OPERATOR, (TreeElem_t){.op = OP_DEG}, NULL, left, right);
        }
        left->parent = new_node;
        right->parent = new_node;
        left = new_node;
    }
    tree_dump_func(left, "Before ret GetT node %s", __FILE__, __func__, __LINE__, mtk, buffer + *pos);
    return left;
}

static bool FindOperator(size_t* pos, char* buffer, OPERATORS* op);

static TreeNode_t* GetF(size_t* pos, char* buffer, metki *mtk, TreeErr_t* err){
    TreeNode_t* val = NULL;
    OPERATORS op = INCORR;
    skip_space(buffer, pos); 

    if(FindOperator(pos, buffer, &op)){ 
        skip_space(buffer, pos); 
        TreeNode_t* left = GetP(pos, buffer, mtk, err);
        if(*err){
            TreeDelNodeRecur(left);
            return NULL;
        }

        skip_space(buffer, pos); 
        val = NodeCtor(OPERATOR, (TreeElem_t){.op = op}, NULL, left, NULL);
        left->parent = val;
    }
    else{
        return GetP(pos, buffer, mtk, err);
    }
    tree_dump_func(val, "Before ret GetF(val) node %s", __FILE__, __func__, __LINE__, mtk, buffer + *pos);
    return val;
}

static bool FindOperator(size_t* pos, char* buffer, OPERATORS* op){
    assert(pos); assert(buffer);  assert(op);
    size_t arr_num_of_elem = sizeof(OPERATORS_INFO) / sizeof(op_info);

    for(size_t idx = 1; idx < arr_num_of_elem; idx++){
        if(OPERATORS_INFO[idx].op == OP_ADD || OPERATORS_INFO[idx].op == OP_SUB || OPERATORS_INFO[idx].op == OP_MUL || OPERATORS_INFO[idx].op == OP_DIV || OPERATORS_INFO[idx].op == OP_DEG){
            continue;
        }
        if(!strncmp(buffer + *pos, OPERATORS_INFO[idx].op_name, OPERATORS_INFO[idx].num_of_symb)){
            *op = OPERATORS_INFO[idx].op;
            *pos += OPERATORS_INFO[idx].num_of_symb;
            return true;
        }
    }

    return false;
}

static TreeNode_t* GetP(size_t* pos, char* buffer, metki *mtk, TreeErr_t* err){
    TreeNode_t* val = NULL;
    skip_space(buffer, pos); 

    if(buffer[*pos] == '('){
        (*pos)++;
        skip_space(buffer, pos); 

        val = GetE(pos, buffer, mtk, err);
        skip_space(buffer, pos); 

        if(buffer[*pos] == ')'){
            (*pos)++;
        }
    }
    else if(isdigit(buffer[*pos])){
        return GetN(pos, buffer, err);
    }
    else if(isalpha(buffer[*pos])){
        return GetV(pos, buffer, mtk, err);
    }
    else{
        *err = INCORR_FILE;
    }
    tree_dump_func(val, "Before ret GetP(val) node %s", __FILE__, __func__, __LINE__, mtk, buffer + *pos);
    return val;
}

TreeNode_t* GetN(size_t* pos, char* buffer, TreeErr_t* err){
    skip_space(buffer, pos); 
    char *endptr = NULL;
    double val = strtod(buffer + *pos, &endptr);
    *pos += endptr - (buffer + *pos);
    return NodeCtor(CONST, (TreeElem_t){.const_value = val}, NULL, NULL, NULL);
}

static size_t FindVar(char dest, metki* mtk);

TreeNode_t* GetV(size_t* pos, char* buffer, metki* mtk, TreeErr_t* err){
    skip_space(buffer, pos); 
    char num_of_var = buffer[*pos];
    (*pos)++;

    return NodeCtor(VARIABLE, (TreeElem_t){.var_code = FindVar(num_of_var, mtk)}, NULL, NULL, NULL);
}


static size_t FindVar(char dest, metki* mtk){
    assert(dest); assert(mtk); 

    size_t metka_idx = find_var_in_mtk_arr(mtk, dest);
    if(metka_idx != SIZE_MAX){
        return metka_idx;
    }
    return metki_add_name(mtk, dest);
}
