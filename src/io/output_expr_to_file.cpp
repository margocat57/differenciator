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

const double EPS = 1e-15;

//-----------------------------------------------------------------
// Output func

static TreeErr_t DumpToFileRecursive(FILE* file, TreeNode_t* node, metki* mtk);

static TreeErr_t OperatorDumpFile(FILE* file, TreeNode_t* node, metki* mtk);

TreeErr_t DumpToFile(FILE* file, TreeNode_t* node, metki* mtk){
    CHECK_AND_RET_TREEERR(DumpToFileRecursive(file, node, mtk));
    return NO_MISTAKE_T;
}

TreeErr_t DumpToFileTaylor(FILE* file, Forest_t* forest, Forest_t* forest_diff){
    for(size_t idx = 0; idx < forest->first_free_place; idx++){
        if(forest->head_arr[idx]->root->type == CONST && fabs(forest->head_arr[idx]->root->data.const_value) < EPS){
            continue;
        }
        if(forest->head_arr[idx]->root->left && forest->head_arr[idx]->root->left->type == CONST && forest->head_arr[idx]->root->left->data.const_value >= 0){
            fprintf(file, " + ");
        }
        if(forest->head_arr[idx]->root->left && forest->head_arr[idx]->root->left->type == VARIABLE){
            fprintf(file, " + ");
        }
        CHECK_AND_RET_TREEERR(DumpToFileRecursive(file, forest->head_arr[idx]->root, forest_diff->mtk));
    }
    return NO_MISTAKE_T;
}


static TreeErr_t DumpToFileRecursive(FILE* file, TreeNode_t* node, metki* mtk){
    switch(node->type){
        case INCORR_VAL: return INCORR_TYPE;
        case CONST:
            fprintf(file, "%lg" ,node->data.const_value);
            break;
        case VARIABLE:
            if(node->data.var_code >= mtk->num_of_metki){
                return INCORR_IDX_IN_MTK;
            }
            fprintf(file, "%c" , mtk->var_info[node->data.var_code].variable_name);
            break;
        case OPERATOR:
            CHECK_AND_RET_TREEERR(OperatorDumpFile(file, node, mtk));
            break;
        default: return INCORR_TYPE;
    }
    return NO_MISTAKE_T;
}

static TreeErr_t DumpSubtree(FILE* file, TreeNode_t* node, metki* mtk);

static TreeErr_t OperatorDumpFile(FILE* file, TreeNode_t* node, metki* mtk){
    assert(file); assert(node); 
    size_t arr_num_of_elem = sizeof(OPERATORS_INFO) / sizeof(op_info);
    if(node->data.op >= arr_num_of_elem){
        return INCORR_OPERATOR;
    }
    
    CHECK_AND_RET_TREEERR(DumpSubtree(file, node->left, mtk));
    if(OPERATORS_INFO[node->data.op].dump_cont && OPERATORS_INFO[node->data.op].op != OP_DEG){
        fprintf(file, "%s", OPERATORS_INFO[node->data.op].op_name); 
    }
    else if(OPERATORS_INFO[node->data.op].op == OP_DEG){
        fprintf(file, "**"); 
    }

    if(node->right){
        CHECK_AND_RET_TREEERR(DumpSubtree(file, node->right, mtk));
    }

    return NO_MISTAKE_T; 
}

static TreeErr_t DumpSubtree(FILE* file, TreeNode_t* node, metki* mtk){
    bool staples = false; 
    CHECK_AND_RET_TREEERR(NeedStaples(node, &staples)); 
    if (staples) fprintf(file, "("); 
    CHECK_AND_RET_TREEERR(DumpToFileRecursive(file, node, mtk)); 
    if (staples) fprintf(file, ")"); 
    return NO_MISTAKE_T;
}