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

static TreeErr_t ReadNode(size_t *pos, TreeNode_t *node_parent, char *buffer, TreeNode_t **node_to_write, metki *mtk);

Forest_t* MakeDiffForest(const char *name_of_file){
    assert(name_of_file);

    char* buffer = read_file_to_string_array(name_of_file);
    if(!buffer){
        return NULL;
    }
    TreeHead_t* head = TreeCtor();
    Forest_t* forest = ForestCtor(10);
    size_t pos = 0;
    if(ReadNode(&pos, NULL, buffer, &(head->root), forest->mtk)){
        free(head);
        ForestDtor(forest);
        buffer_free(buffer);
        return NULL;
    }
    DEBUG_TREE(
    if(TreeVerify(head)){
        fprintf(stderr, "File is not correct - can't work with created tree\n");
        free(head);
        ForestDtor(forest);
        buffer_free(buffer);
        return NULL;
    }
    )
    ForestAddElem(head, forest);
    buffer_free(buffer);
    return forest;
}
static TreeNode_t* DiffNodeCtor(char* buffer, size_t* pos, metki* mtk, TreeNode_t* node_parent);

static TreeErr_t ReadNode(size_t *pos, TreeNode_t *node_parent, char *buffer, TreeNode_t **node_to_write, metki *mtk)
{
    assert(pos); assert(buffer); assert(mtk);

    skip_space(buffer, pos);
    if(buffer[(*pos)] == '('){
        (*pos)++; //skip '('
        skip_space(buffer, pos);
        TreeNode_t* node = DiffNodeCtor(buffer, pos, mtk, node_parent);
        skip_space(buffer, pos);

        if(ReadNode(pos, node, buffer, &(node->left), mtk)){
            fprintf(stderr, "Incorr file\n");
            return INCORR_FILE;
        }

        if(ReadNode(pos, node, buffer, &(node->right), mtk)){
            fprintf(stderr, "Incorr file\n");
            return INCORR_FILE;
        }

        tree_dump_func(node, "After making left && right|%s", __FILE__, __func__, __LINE__, mtk, buffer + (*pos));

        skip_space(buffer, pos);
        if(buffer[(*pos)] != ')'){
            fprintf(stderr, "Incorr file no ) buffer %s\n", buffer + *pos);
            return INCORR_FILE;
        }
        (*pos)++; //skip ')'
        skip_space(buffer, pos);
        *node_to_write = node;
        return NO_MISTAKE_T;
    }
    else if(!strncmp(buffer + *pos, "nil", 3)){
        (*pos) += strlen("nil");
        *node_to_write = NULL;
        skip_space(buffer, pos);
        return NO_MISTAKE_T;
    }

    fprintf(stderr, "Incorr file undef symbol(%s is %c), pos(%zu)\n", buffer + *pos, buffer[*pos] , *pos);
    return INCORR_FILE;
}

//-------------------------------------------------------------------------------
//Parsing metki

static void BufScanfWord(char* buffer, size_t* pos, char* dest);

static bool FindOperator(char* dest, OPERATORS* op);

static bool FindConst(char* dest, double* const_val);

static bool FindVar(char* dest, metki* mtk, size_t *elem);

static TreeNode_t* DiffNodeCtor(char* buffer, size_t* pos, metki* mtk, TreeNode_t* node_parent){
    assert(buffer); assert(pos); assert(mtk); 

    VALUE_TYPE type = INCORR_VAL;
    OPERATORS op = INCORR;
    double const_val = 0;
    size_t metka_for_var = 0;
    char dest[MAX_BUFFER_SIZE] = {};
    BufScanfWord(buffer, pos, dest);
    if(FindOperator(dest, &op)){
        type = OPERATOR;
        return NodeCtor(type, (TreeElem_t){.op = op}, node_parent, NULL, NULL);
    }
    else if(FindConst(dest, &const_val)){
        type = CONST;
        return NodeCtor(type, (TreeElem_t){.const_value = const_val}, node_parent, NULL, NULL);
    }
    else if(FindVar(dest, mtk, &metka_for_var)){
        type = VARIABLE;
        return NodeCtor(type, (TreeElem_t){.var_code = metka_for_var}, node_parent, NULL, NULL);
    }
    return NULL;
}

static void BufScanfWord(char* buffer, size_t* pos, char* dest){
    assert(buffer); assert(pos); assert(dest);

    size_t symb_amt = strcspn(buffer + *pos, "(\r\v\f\n\t ");
    if(symb_amt > MAX_BUFFER_SIZE - 1){
        fprintf(stderr, "Can't read more symbols than buffer size %zu - will be read onnly buffer size\n", MAX_BUFFER_SIZE - 1);
        symb_amt = MAX_BUFFER_SIZE - 1;
    }
    strncpy(dest, buffer + *pos, symb_amt);
    (*pos) += symb_amt;
}

static bool FindOperator(char* dest, OPERATORS* op){
    assert(dest); assert(op);
    size_t arr_num_of_elem = sizeof(OPERATORS_INFO) / sizeof(op_info);

    for(size_t idx = 1; idx < arr_num_of_elem; idx++){
        if(!strncmp(dest, OPERATORS_INFO[idx].op_name, OPERATORS_INFO[idx].num_of_symb)){
            *op = OPERATORS_INFO[idx].op;
            return true;
        }
    }

    return false;
}

static bool FindConst(char* dest, double* const_val){
    assert(dest); assert(const_val);

    char *endptr = NULL;
    *const_val = strtod(dest, &endptr);
    if(endptr == dest){
        return false;
    }
    return true;
}

static bool FindVar(char* dest, metki* mtk, size_t *elem){
    assert(dest); assert(mtk); assert(elem);

    if(isalpha(*dest)){
        size_t metka_idx = find_var_in_mtk_arr(mtk, dest);
        if(metka_idx != SIZE_MAX){
            *elem = metka_idx;
            return true;
        }
        *elem = metki_add_name(mtk, dest);
        return true;
    }
    return false;
}
