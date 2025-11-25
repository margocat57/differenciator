#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "../differenciation/differenciator.h"
#include "../dump/graphviz_dump.h"
#include "../core/tree_func.h"
#include "../core/forest.h"

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
// DSL for calculating funt
#define CALCUL_TREE_CPP
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

/*
* Унарные операторы со значением в левом узле - снимаем предупреждение
*/
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
DEF_OP(Cos, cos(RES_L));
DEF_OP(Sin, sin(RES_L));
DEF_OP(Ln,  log(RES_L));
DEF_OP(Tg,  tan(RES_L));
DEF_OP(Ctg, (tan(RES_L) != 0) ? 1 / tan(RES_L) : 0);
DEF_OP(Sh,  sinh(RES_L));
DEF_OP(Ch,  cosh(RES_L));
DEF_OP(Th,  tanh(RES_L));
DEF_OP(Cth, (tanh(RES_L) != 0) ? 1 / tanh(RES_L) : 0);
#pragma GCC diagnostic pop

#include "../core/operator_func.h"

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
// First part of task - tree of expressions and main function for it

static TreeErr_t CalcTreeExpressionRecursive(metki* mtk, TreeNode_t* node, double* result);

TreeErr_t CalcTreeExpression(Forest_t* forest, size_t num_of_tree, double* result, bool is_taylor){
    assert(result);

    TreeErr_t err = NO_MISTAKE_T;
    DEBUG_TREE(err = TreeVerify(forest->head_arr[num_of_tree]);)
    if(err) return err;
    
    if(!is_taylor) metki_add_values(forest->mtk);
    CalcTreeExpressionRecursive(forest->mtk, forest->head_arr[num_of_tree]->root, result);
    if(!is_taylor) metki_del_values(forest->mtk);

    DEBUG_TREE(err = TreeVerify(forest->head_arr[num_of_tree]);)
    return err;
}

//--------------------------------------------------------------------------------------------
// Postfix tree calculating
// After will be verifying

static TreeErr_t CalcExpWithOperator(TreeNode_t *node, double *result, double *left_result, double *right_result);

static void CalcExpWithConst(TreeNode_t* node, double* result);

static void CalcExpWithVar(metki* mtk, TreeNode_t* node, double* result);


static TreeErr_t CalcTreeExpressionRecursive(metki* mtk, TreeNode_t* node, double* result){
    assert(result);

    double left_result = 0;
    double right_result = 0;

    if(!node){
        return NO_MISTAKE_T;
    }

    if(node->type == OPERATOR){
        CHECK_AND_RET_TREEERR(CalcTreeExpressionRecursive(mtk, node->left, &left_result));
        CHECK_AND_RET_TREEERR(CalcTreeExpressionRecursive(mtk, node->right, &right_result));
    }
    switch(node->type){
        case INCORR_VAL: return INCORR_TYPE;
        case OPERATOR: CHECK_AND_RET_TREEERR(CalcExpWithOperator(node, result, &left_result, &right_result)); break;
        case CONST:                          CalcExpWithConst(node, result);                                  break;
        case VARIABLE:                       CalcExpWithVar(mtk, node, result);                               break;
        default:        return INCORR_TYPE;
    }
    return NO_MISTAKE_T;
}
//--------------------------------------------------------------------------------

static TreeErr_t CalcExpWithOperator(TreeNode_t* node, double* result, double* left_result, double* right_result){
    assert(result); 
    size_t arr_num_of_elem = sizeof(OPERATORS_INFO) / sizeof(op_info);
    if(node->data.op >= arr_num_of_elem){
        return INCORR_OPERATOR;
    }
    if(OPERATORS_INFO[node->data.op].function_calc == NULL){
        return NULL_PTR_TO_FUNC;
    }
    OPERATORS_INFO[node->data.op].function_calc(result, left_result, right_result);
    return NO_MISTAKE_T;
}

static void CalcExpWithConst(TreeNode_t* node, double* result){
    *result = node->data.const_value;
}

static void CalcExpWithVar(metki* mtk, TreeNode_t* node, double* result){
    *result = mtk->var_info[node->data.var_code].value;
}
//---------------------------------------------------------------------------
// Optimization

static TreeErr_t TreeOptimizeConst(TreeNode_t *node, bool *is_optimized);

static TreeErr_t TreeOptimizeNeutral(TreeNode_t **result, TreeNode_t *node, bool *is_optimized);

TreeErr_t TreeOptimize(TreeNode_t **node){
    assert(node);
    bool is_optimized = false;
    do{
        is_optimized = false; 
        CHECK_AND_RET_TREEERR(TreeOptimizeConst(*node, &is_optimized));
        CHECK_AND_RET_TREEERR(TreeOptimizeNeutral(node, *node, &is_optimized));
    }while(is_optimized);

    return NO_MISTAKE_T;
}

static TreeErr_t TreeOptimizeConst(TreeNode_t *node, bool *is_optimized){
    assert(node); assert(is_optimized);
    if(node->left){
        CHECK_AND_RET_TREEERR(TreeOptimizeConst(node->left, is_optimized));
    }
    if(node->right){
        CHECK_AND_RET_TREEERR(TreeOptimizeConst(node->right, is_optimized));
    }
    if(node->left && node->right && node->left->type == CONST && node->right->type == CONST && node->type == OPERATOR){
        size_t arr_num_of_elem = sizeof(OPERATORS_INFO) / sizeof(op_info);
        if(node->data.op >= arr_num_of_elem){
            return INCORR_OPERATOR;
        }
        if(OPERATORS_INFO[node->data.op].function_calc == NULL){
            return NULL_PTR_TO_FUNC;
        }
        OPERATORS_INFO[node->data.op].function_calc(&(node->data.const_value), &(node->left->data.const_value), &(node->right->data.const_value));
        NodeDtor(node->left);
        NodeDtor(node->right);
        node->type = CONST;
        node->left = NULL;
        node->right = NULL;
        *is_optimized = true;
    }
    return NO_MISTAKE_T;
}

//--------------------------------------------------------------------------
// Optimizing neutrals

#define IS_ZERO(node) ((node) && (node)->type == CONST && (node)->data.const_value == 0)
#define IS_ONE(node) ((node) && (node)->type == CONST && (node)->data.const_value == 1)

static void ChangeKidParrentConn(TreeNode_t** result, TreeNode_t* node_for_change, TreeNode_t* new_node, bool* is_optimized);

static void TreeOptimizeNeutralAddSub(TreeNode_t** result, TreeNode_t* node, bool* is_optimized);

static void TreeOptimizeNeutralMul(TreeNode_t** result, TreeNode_t* node, bool* is_optimized);

static void TreeOptimizeNeutralDiv(TreeNode_t** result, TreeNode_t* node, bool* is_optimized);

static void TreeOptimizeNeutralDeg(TreeNode_t** result, TreeNode_t* node, bool* is_optimized);

static TreeErr_t TreeOptimizeNeutral(TreeNode_t **result, TreeNode_t *node, bool *is_optimized){
    if(node->left){
        CHECK_AND_RET_TREEERR(TreeOptimizeNeutral(result, node->left, is_optimized));
    }
    if(node->right){
        CHECK_AND_RET_TREEERR(TreeOptimizeNeutral(result, node->right, is_optimized));
    }

    if(node->type == OPERATOR){
        switch(node->data.op){
            case INCORR:              return INCORR_OPERATOR;
            case OP_ADD: case OP_SUB: TreeOptimizeNeutralAddSub(result, node, is_optimized); break;
            case OP_MUL:              TreeOptimizeNeutralMul(result, node, is_optimized);    break;
            case OP_DIV:              TreeOptimizeNeutralDiv(result, node, is_optimized);    break;
            case OP_DEG:              TreeOptimizeNeutralDeg(result, node, is_optimized);    break;
            default:                                                                         break;
            }
    }
    return NO_MISTAKE_T;
}

static void ChangeKidParrentConn(TreeNode_t** result, TreeNode_t* node_for_change, TreeNode_t* new_node, bool* is_optimized){
    if(node_for_change->parent){
        if(node_for_change == node_for_change->parent->left){
            node_for_change->parent->left = new_node;
        }
        else{
            node_for_change->parent->right = new_node;
        }
        new_node->parent = node_for_change->parent;
    }
    else{
        *result = new_node;
    }
    NodeDtor(node_for_change);
    *is_optimized = true;
}

static void TreeOptimizeNeutralAddSub(TreeNode_t** result, TreeNode_t* node, bool* is_optimized){
    if(IS_ZERO(node->left) && node->data.op != OP_SUB){
        TreeDelNodeRecur(node->left);
        ChangeKidParrentConn(result, node, node->right, is_optimized); 
    }
    else if(IS_ZERO(node->right)){
        TreeDelNodeRecur(node->right);
        ChangeKidParrentConn(result, node, node->left, is_optimized);
    }
}

static void TreeOptimizeNeutralMul(TreeNode_t** result, TreeNode_t* node, bool* is_optimized){
    if(IS_ZERO(node->left) || IS_ONE(node->right)){
        TreeDelNodeRecur(node->right);
        ChangeKidParrentConn(result, node, node->left, is_optimized);
    }
    else if(IS_ZERO(node->right) || IS_ONE(node->left)){
        TreeDelNodeRecur(node->left);
        ChangeKidParrentConn(result, node, node->right, is_optimized);
    }
}

static void TreeOptimizeNeutralDiv(TreeNode_t** result, TreeNode_t* node, bool* is_optimized){
    if(IS_ZERO(node->left) && !IS_ZERO(node->right)){
        TreeDelNodeRecur(node->right);
        ChangeKidParrentConn(result, node, node->left, is_optimized);
    }
    else if(IS_ONE(node->right)){
        TreeDelNodeRecur(node->left);
        ChangeKidParrentConn(result, node, node->right, is_optimized);
    }
}

static void TreeOptimizeNeutralDeg(TreeNode_t** result, TreeNode_t* node, bool* is_optimized){
    if((IS_ZERO(node->left) && !IS_ZERO(node->right))){
        TreeDelNodeRecur(node->right);
        ChangeKidParrentConn(result, node, node->left, is_optimized);
    }
    else if(IS_ONE(node->left)){
        TreeDelNodeRecur(node->right);
        ChangeKidParrentConn(result, node, node->left, is_optimized);
    }
    else if(IS_ZERO(node->right) && !IS_ZERO(node->left)){
        node->right->data.const_value += 1;
        TreeDelNodeRecur(node->left);
        ChangeKidParrentConn(result, node, node->right, is_optimized);
    }
    else if(IS_ONE(node->right)){
        TreeDelNodeRecur(node->right);
        ChangeKidParrentConn(result, node, node->left, is_optimized);
    }
}

//-----------------------------------------------------------------------------
// Undef dsl
#undef IS_ZERO
#undef IS_ONE
#undef RES_L
#undef RES_R
#undef DEF_OP
#undef CALCUL_TREE_CPP