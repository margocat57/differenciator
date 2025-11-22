#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "differenciator.h"
#include "../dump/graphviz_dump.h"
#include "../data_struct/tree_func.h"
#include "../data_struct/forest.h"

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
// DSL for calculating funt

#define RES_L *left_result
#define RES_R *right_result
#define DEF_OP(Op, Result) \
static void Calc##Op(double* result, double* left_result, double* right_result){ \
    *result = (Result); \
}

DEF_OP(Add, RES_L + RES_R);
DEF_OP(Sub, RES_L - RES_R);
DEF_OP(Mul, RES_L * RES_R);
DEF_OP(Div, (RES_R != 0) ? RES_L / RES_R : 0);
DEF_OP(Deg, pow(RES_L, RES_R));
DEF_OP(Cos, cos(RES_L + RES_R));
DEF_OP(Sin, sin(RES_L + RES_R));
DEF_OP(Ln,  logf(RES_L + RES_R));
DEF_OP(Tg,  tan(RES_L + RES_R));
DEF_OP(Ctg, (RES_R != 0) ? 1 / tan(RES_L + RES_R) : 0);
DEF_OP(Sh,  sinh(RES_L + RES_R));
DEF_OP(Ch,  cosh(RES_L + RES_R));
DEF_OP(Th,  tanh(RES_L + RES_R));
DEF_OP(Cth, (RES_R != 0) ? 1 / tanh(RES_L + RES_R) : 0);

struct operators_func{
    void(*function_calc)(double* result, double* left_result, double* right_result);
};

const operators_func FUNC_FOR_OPERATORS[] = {
    NULL,
    CalcAdd,
    CalcSub,
    CalcMul,
    CalcDiv,
    CalcDeg,
    CalcSin,
    CalcCos,
    CalcLn ,
    CalcTg ,
    CalcCtg,
    CalcSh ,
    CalcCh ,
    CalcTh ,
    CalcCth
};


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
// First part of task - tree of expressions and main function for it

static TreeErr_t CalcTreeExpressionRecursive(metki* mtk, TreeNode_t* node, double* result);

TreeErr_t CalcTreeExpression(Forest_t* forest, size_t num_of_tree, double* result){
    assert(result);

    TreeErr_t err = NO_MISTAKE_T;
    DEBUG_TREE(err = TreeVerify(forest->head_arr[num_of_tree]);)
    if(err) return err;
    
    metki_add_values(forest->mtk);
    CalcTreeExpressionRecursive(forest->mtk, forest->head_arr[num_of_tree]->root, result);
    metki_del_values(forest->mtk);

    DEBUG_TREE(err = TreeVerify(forest->head_arr[num_of_tree]);)
    return err;
}

//--------------------------------------------------------------------------------------------
// Postfix tree calculating
// After will be verifying

static void CalcExpWithOperator(TreeNode_t* node, double* result, double* left_result, double* right_result);

static void CalcExpWithConst(TreeNode_t* node, double* result);

static void CalcExpWithVar(metki* mtk, TreeNode_t* node, double* result);


static TreeErr_t CalcTreeExpressionRecursive(metki* mtk, TreeNode_t* node, double* result){
    assert(result);

    double left_result = 0;
    double right_result = 0;

    if(node->type == OPERATOR){
        CalcTreeExpressionRecursive(mtk, node->left, &left_result);
        CalcTreeExpressionRecursive(mtk, node->right, &right_result);
    }
    switch(node->type){
        case OPERATOR: CalcExpWithOperator(node, result, &left_result, &right_result); break;
        case CONST:    CalcExpWithConst(node, result);                                 break;
        case VARIABLE: CalcExpWithVar(mtk, node, result);                              break;
        default: return INCORR_OPERATOR;
    }
    return NO_MISTAKE_T;
}
//--------------------------------------------------------------------------------

static void CalcExpWithOperator(TreeNode_t* node, double* result, double* left_result, double* right_result){
    assert(result); 
    size_t arr_num_of_elem = sizeof(FUNC_FOR_OPERATORS) / sizeof(operators_func);
    if(node->data.op >= arr_num_of_elem){
        return;
    }
    FUNC_FOR_OPERATORS[node->data.op].function_calc(result, left_result, right_result);
}

static void CalcExpWithConst(TreeNode_t* node, double* result){
    *result = node->data.const_value;
}

static void CalcExpWithVar(metki* mtk, TreeNode_t* node, double* result){
    *result = mtk->var_info[node->data.var_code].value;
}

//-----------------------------------------------------------------------------
// Undef dsl
#undef RES_L
#undef RES_R
#undef DEF_OP