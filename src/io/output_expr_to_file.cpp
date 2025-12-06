#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include "../core/tree_func.h"
#include "output_expr_to_file.h"
#include "../utils/metki.h"
#include "../dump/graphviz_dump.h"
#include "../core/forest.h"
#include "../core/operator_func.h"
#include "../dump/latex_dump.h"

#define CALL_FUNC_AND_CHECK_ERR(function)\
    do{\
        function;\
        if(*err){ \
            fprintf(stderr, "err = %llu, %s, %s, %d\n", *err, __FILE__, __func__, __LINE__); \
            return;                                                         \
        } \
    }while(0)

//-----------------------------------------------------------------
// Output func

static void OperatorPutInfo(FILE* file, TreeNode_t* node, metki* mtk, TreeErr_t* err, DumpMode mode);

void DumpToFileGp(FILE* file, TreeNode_t* node, metki* mtk, TreeErr_t *err){
    if(*err) return;
    DEBUG_TREE(CALL_FUNC_AND_CHECK_ERR(*err = TreeNodeVerify(node));)

    CALL_FUNC_AND_CHECK_ERR(PutInfoRecursive(file, node, mtk, err, DUMP_GNUPLOT));

    DEBUG_TREE(CALL_FUNC_AND_CHECK_ERR(*err = TreeNodeVerify(node));)
}


void PutInfoRecursive(FILE* file, TreeNode_t* node, metki* mtk, TreeErr_t* err, DumpMode mode){
    if(*err) return;
    switch(node->type){
        case INCORR_VAL: *err = INCORR_TYPE; return;
        case CONST:
            fprintf(file, "%lg" ,node->data.const_value);
            break;
        case VARIABLE:
            if(node->data.var_code >= mtk->num_of_metki){
                *err = INCORR_IDX_IN_MTK;
                return;
            }
            fprintf(file, "%c" , mtk->var_info[node->data.var_code].variable_name);
            break;
        case OPERATOR:
            CALL_FUNC_AND_CHECK_ERR(OperatorPutInfo(file, node, mtk, err, mode));
            if(*err) return;
            break;
        default: *err = INCORR_TYPE; return;
    }
}

static void PutSubtree(FILE* file, TreeNode_t* node, metki* mtk, TreeErr_t* err, DumpMode mode);

static void OperatorPutInfo(FILE* file, TreeNode_t* node, metki* mtk, TreeErr_t* err, DumpMode mode){
    if(*err) return;

    assert(file); assert(node); 
    size_t arr_num_of_elem = sizeof(OPERATORS_INFO) / sizeof(op_info);
    if(node->data.op >= arr_num_of_elem){
        *err = INCORR_OPERATOR;
        return;
    }
    
    fprintf(file, "%s", OPERATORS_INFO[node->data.op].dump_info_arr[mode].dump_start);

    CALL_FUNC_AND_CHECK_ERR(PutSubtree(file, node->left, mtk, err, mode));

    if(OPERATORS_INFO[node->data.op].dump_info_arr[mode].dump_cont){
        fprintf(file, "%s", OPERATORS_INFO[node->data.op].dump_info_arr[mode].dump_cont); 
    }

    if(node->right){
        CALL_FUNC_AND_CHECK_ERR(PutSubtree(file, node->right, mtk, err, mode));
    }

    fprintf(file, "%s", OPERATORS_INFO[node->data.op].dump_info_arr[mode].dump_end); 
}


static void PutSubtree(FILE* file, TreeNode_t* node, metki* mtk, TreeErr_t* err, DumpMode mode){
    if(*err) return;
    bool staples = false;

    CALL_FUNC_AND_CHECK_ERR(NeedStaples(node, &staples, err)); 

    if (staples) fprintf(file, "("); 

    CALL_FUNC_AND_CHECK_ERR(PutInfoRecursive(file, node, mtk, err, mode)); 

    if (staples) fprintf(file, ")"); 
}

void NeedStaples(TreeNode_t* node, bool* need_staples, TreeErr_t* err){
    assert(need_staples);
    if(*err) return;
    if(!node || node->type != OPERATOR || !node->parent || node->parent->type != OPERATOR){
        *need_staples = false;
        return;
    }

    size_t arr_num_of_elem = sizeof(OPERATORS_INFO) / sizeof(op_info);
    if(node->data.op >= arr_num_of_elem || node->parent->data.op >= arr_num_of_elem || node->data.op == INCORR || node->parent->data.op == INCORR){
        *err = INCORR_OPERATOR;
        return;
    }
    int node_priority = OPERATORS_INFO[node->data.op].priority;
    int node_parent_priority = OPERATORS_INFO[node->parent->data.op].priority;

    if(node_priority < node_parent_priority && !OPERATORS_INFO[node->parent->data.op].is_unary_op){
        *need_staples = true;
        return;
    }
    if(node->parent->data.op == OP_SUB){ // 1 - (a + b) and 1 - a + b 
        *need_staples = true;
        return;
    }
    if(node->parent->data.op == OP_DEG){ //для случая 1 / (x^2^2^2)
        *need_staples = true;
        return;
    }
    *need_staples = false;
}