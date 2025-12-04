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

//-----------------------------------------------------------------
// Output func

static void DumpToFileRecursive(FILE* file, TreeNode_t* node, metki* mtk, TreeErr_t* err);

static void OperatorDumpFile(FILE* file, TreeNode_t* node, metki* mtk, TreeErr_t* err);

TreeErr_t DumpToFile(FILE* file, TreeNode_t* node, metki* mtk){
    TreeErr_t err = NO_MISTAKE;
    DEBUG_TREE(err = TreeNodeVerify(node);)
    if(err) return err;

    CALL_FUNC_AND_CHECK_ERR_VALUE(DumpToFileRecursive(file, node, mtk, &err), err);

    DEBUG_TREE(err = TreeNodeVerify(node);)
    return err;
}


static void DumpToFileRecursive(FILE* file, TreeNode_t* node, metki* mtk, TreeErr_t* err){
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
            OperatorDumpFile(file, node, mtk, err);
            if(*err) return;
            break;
        default: *err = INCORR_TYPE; return;
    }
}

static void DumpSubtreeGp(FILE* file, TreeNode_t* node, metki* mtk, TreeErr_t* err);

static void OperatorDumpFile(FILE* file, TreeNode_t* node, metki* mtk, TreeErr_t* err){
    if(*err) return;

    assert(file); assert(node); 
    size_t arr_num_of_elem = sizeof(OPERATORS_INFO) / sizeof(op_info);
    if(node->data.op >= arr_num_of_elem){
        *err = INCORR_OPERATOR;
        return;
    }
    
    fprintf(file, "%s", OPERATORS_INFO[node->data.op].dump_gnuplot_start);

    DumpSubtreeGp(file, node->left, mtk, err);
    if(*err) return;

    if(OPERATORS_INFO[node->data.op].dump_cont){
        fprintf(file, "%s", OPERATORS_INFO[node->data.op].dump_gnuplot_cont); 
    }

    if(node->right){
        DumpSubtreeGp(file, node->right, mtk, err);
        if(*err) return;
    }

    fprintf(file, "%s", OPERATORS_INFO[node->data.op].dump_gnuplot_end); 
}


static void DumpSubtreeGp(FILE* file, TreeNode_t* node, metki* mtk, TreeErr_t* err){
    if(*err) return;
    bool staples = false;

    NeedStaples(node, &staples, err); 
    if(*err) return;

    if (staples) fprintf(file, "("); 

    DumpToFileRecursive(file, node, mtk, err); 
    if(*err) return;

    if (staples) fprintf(file, ")"); 
}