#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "../differenciation/differenciator.h"
#include "../dump/graphviz_dump.h"
#include "../core/tree_func.h"
#include "../core/forest.h"


const double EPS = 1e-15;

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
// DSL for calculating funt
#define CALCUL_TREE_CPP
#define RES_L *left_result
#define RES_R *right_result
#define DEF_OP(Op, Result) \
static void Calc##Op(double* result, double* left_result, double* right_result){ \
    assert(result); \
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
DEF_OP(Arcsin,  (-1 <= RES_L && RES_L <= 1) ? asin(RES_L) : 0);
DEF_OP(Arccos,  (-1 <= RES_L && RES_L <= 1) ? acos(RES_L) : 0);
DEF_OP(Arctg,   atan(RES_L));
DEF_OP(Arcctg,  M_PI_2 - atan(RES_L));
#pragma GCC diagnostic pop

#include "../core/operator_func.h"

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
// First part of task - tree of expressions and main function for it

static TreeErr_t CalcTreeExpressionRecursive(metki* mtk, TreeNode_t* node, double* result);

TreeErr_t CalcTreeExpression(TreeNode_t* node, metki* mtk, double* result, IS_TAYLOR is_taylor){
    assert(result);

    TreeErr_t err = NO_MISTAKE_T;
    DEBUG_TREE(err = TreeNodeVerify(node);)
    if(err) return err;
    
    if(is_taylor == NO) metki_add_values(mtk);
    CHECK_AND_RET_TREEERR(CalcTreeExpressionRecursive(mtk, node, result));
    if(is_taylor == NO) metki_del_values(mtk);

    DEBUG_TREE(err = TreeNodeVerify(node);)
    return err;
}

//--------------------------------------------------------------------------------------------
// Postfix tree calculating
// After will be verifying

static TreeErr_t CalcExpWithOperator(TreeNode_t *node, double *result, double *left_result, double *right_result);

static TreeErr_t CalcExpWithConst(TreeNode_t* node, double* result);

static TreeErr_t CalcExpWithVar(metki* mtk, TreeNode_t* node, double* result);

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
        case CONST:    CHECK_AND_RET_TREEERR(CalcExpWithConst(node, result));                                 break;
        case VARIABLE: CHECK_AND_RET_TREEERR(CalcExpWithVar(mtk, node, result));                              break;
        default:         return INCORR_TYPE;
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

static TreeErr_t CalcExpWithConst(TreeNode_t* node, double* result){
    *result = node->data.const_value;
    return NO_MISTAKE_T;
}

static TreeErr_t CalcExpWithVar(metki* mtk, TreeNode_t* node, double* result){
    if(!result){
        return NULL_RES_PTR;
    }
    if(!mtk){
        return NULL_PTR_TO_MTK;
    }
    if(node->data.var_code >= mtk->num_of_metki){
        return INCORR_IDX_IN_MTK;
    }
    if(!mtk->var_info[node->data.var_code].variable_name){
        return CANT_GET_VALUE_FOR_NULL_NAME_IN_MTK;
    }
    *result = mtk->var_info[node->data.var_code].value;
    return NO_MISTAKE_T;
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
    assert(node); 
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
        if(!is_optimized){
            return NULL_IS_OPT_PTR;
        }
        *is_optimized = true;
    }
    return NO_MISTAKE_T;
}

//--------------------------------------------------------------------------
// Optimizing neutrals

#define IS_EQUAL(node, value) ((node) && (node)->type == CONST && fabs((node)->data.const_value - value) < EPS)

static void ChangeKidParrentConn(TreeNode_t** result, TreeNode_t* node_for_change, TreeNode_t* new_node, bool* is_optimized);

static TreeErr_t TreeOptimizeNeutralAddSub(TreeNode_t** result, TreeNode_t* node, bool* is_optimized);

static TreeErr_t TreeOptimizeNeutralMul(TreeNode_t** result, TreeNode_t* node, bool* is_optimized);

static TreeErr_t TreeOptimizeNeutralDiv(TreeNode_t** result, TreeNode_t* node, bool* is_optimized);

static TreeErr_t TreeOptimizeNeutralDeg(TreeNode_t** result, TreeNode_t* node, bool* is_optimized);

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
            case OP_ADD: case OP_SUB: CHECK_AND_RET_TREEERR(TreeOptimizeNeutralAddSub(result, node, is_optimized)); break;
            case OP_MUL:              CHECK_AND_RET_TREEERR(TreeOptimizeNeutralMul(result, node, is_optimized));    break;
            case OP_DIV:              CHECK_AND_RET_TREEERR(TreeOptimizeNeutralDiv(result, node, is_optimized));    break;
            case OP_DEG:              CHECK_AND_RET_TREEERR(TreeOptimizeNeutralDeg(result, node, is_optimized));    break;
            case OP_SIN: case OP_COS: case OP_TG: case OP_CTG: case OP_LN:
            case OP_SH:  case OP_CH:  case OP_TH: case OP_CTH:                                                      break;
            default:                  return INCORR_OPERATOR;
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

static TreeErr_t TreeOptimizeNeutralAddSub(TreeNode_t** result, TreeNode_t* node, bool* is_optimized){
    if(!node->left || !node->right){
        return NO_ELEM_FOR_BINARY_OP;
    }
    if(IS_EQUAL(node->left, 0) && node->data.op != OP_SUB){
        TreeDelNodeRecur(node->left);
        ChangeKidParrentConn(result, node, node->right, is_optimized); 
    }
    else if(IS_EQUAL(node->right, 0)){
        TreeDelNodeRecur(node->right);
        ChangeKidParrentConn(result, node, node->left, is_optimized);
    }
    return NO_MISTAKE_T;
}

static TreeErr_t TreeOptimizeNeutralMul(TreeNode_t** result, TreeNode_t* node, bool* is_optimized){
    if(!node->left || !node->right){
        return NO_ELEM_FOR_BINARY_OP;
    }
    if(IS_EQUAL(node->left, 0) || IS_EQUAL(node->right, 1)){
        TreeDelNodeRecur(node->right);
        ChangeKidParrentConn(result, node, node->left, is_optimized);
    }
    else if(IS_EQUAL(node->right, 0) || IS_EQUAL(node->left, 1)){
        TreeDelNodeRecur(node->left);
        ChangeKidParrentConn(result, node, node->right, is_optimized);
    }
    return NO_MISTAKE_T;
}

static TreeErr_t TreeOptimizeNeutralDiv(TreeNode_t** result, TreeNode_t* node, bool* is_optimized){
    if(!node->left || !node->right){
        return NO_ELEM_FOR_BINARY_OP;
    }
    if(IS_EQUAL(node->left, 0) && !IS_EQUAL(node->right, 0)){
        TreeDelNodeRecur(node->right);
        ChangeKidParrentConn(result, node, node->left, is_optimized);
    }
    else if(IS_EQUAL(node->right, 1)){
        TreeDelNodeRecur(node->right);
        ChangeKidParrentConn(result, node, node->left, is_optimized);
    }
    return NO_MISTAKE_T;
}

static TreeErr_t TreeOptimizeNeutralDeg(TreeNode_t** result, TreeNode_t* node, bool* is_optimized){
    if(!node->left || !node->right){
        return NO_ELEM_FOR_BINARY_OP;
    }
    if((IS_EQUAL(node->left, 0) && !IS_EQUAL(node->right, 0))){
        TreeDelNodeRecur(node->right);
        ChangeKidParrentConn(result, node, node->left, is_optimized);
    }
    else if(IS_EQUAL(node->left, 1)){
        TreeDelNodeRecur(node->right);
        ChangeKidParrentConn(result, node, node->left, is_optimized);
    }
    else if(IS_EQUAL(node->right, 0) && !IS_EQUAL(node->left, 0)){
        node->right->data.const_value += 1;
        TreeDelNodeRecur(node->left);
        ChangeKidParrentConn(result, node, node->right, is_optimized);
    }
    else if(IS_EQUAL(node->right, 1)){
        TreeDelNodeRecur(node->right);
        ChangeKidParrentConn(result, node, node->left, is_optimized);
    }
    return NO_MISTAKE_T;
}

//-----------------------------------------------------------------------------
// Undef dsl
#undef IS_EQUAL
#undef RES_L
#undef RES_R
#undef DEF_OP
#undef CALCUL_TREE_CPP