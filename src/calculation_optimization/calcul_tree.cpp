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
    assert(result); \
    *result = (Result); \
}

DEF_OP(Add, RES_L + RES_R);
DEF_OP(Sub, RES_L - RES_R);
DEF_OP(Mul, RES_L * RES_R);
DEF_OP(Div, (fabs(RES_R) > EPS) ? RES_L / RES_R : 0);
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

static void CalcTreeExpressionRecursive(metki* mtk, TreeNode_t* node, double* result, TreeErr_t* err);

TreeErr_t CalcTreeExpression(TreeNode_t* node, metki* mtk, double* result, bool is_taylor){
    assert(result);

    TreeErr_t err = NO_MISTAKE;
    DEBUG_TREE(err = TreeNodeVerify(node);)
    if(err) return err;
    
    if(!is_taylor && !mtk->has_value) MetkiAddValues(mtk);
    CalcTreeExpressionRecursive(mtk, node, result, &err);
    if(err) return err;
    if(!is_taylor && !mtk->has_value) MetkiDelValues(mtk);

    DEBUG_TREE(err = TreeNodeVerify(node);)
    return err;
}

//--------------------------------------------------------------------------------------------
// Postfix tree calculating
// After will be verifying

static void CalcExpWithOperator(TreeNode_t *node, double *result, double *left_result, double *right_result, TreeErr_t* err);

static void CalcExpWithConst(TreeNode_t* node, double* result);

static void CalcExpWithVar(metki* mtk, TreeNode_t* node, double* result, TreeErr_t* err);

static void CalcTreeExpressionRecursive(metki* mtk, TreeNode_t* node, double* result, TreeErr_t* err){
    if(*err) return;
    assert(result);

    double left_result = 0;
    double right_result = 0;

    if(!node) return;

    if(node->type == OPERATOR){
        CalcTreeExpressionRecursive(mtk, node->left, &left_result, err);
        if(*err) return;

        CalcTreeExpressionRecursive(mtk, node->right, &right_result, err);
        if(*err) return;
    }
    switch(node->type){
        case INCORR_VAL: *err = INCORR_TYPE; return;
        case OPERATOR: CalcExpWithOperator(node, result, &left_result, &right_result, err); if(*err) return; break;
        case CONST:    CalcExpWithConst(node, result);                                                       break;
        case VARIABLE: CalcExpWithVar(mtk, node, result, err);                              if(*err) return; break;
        default:         *err = INCORR_TYPE; return;
    }


}
//--------------------------------------------------------------------------------

static void CalcExpWithOperator(TreeNode_t* node, double* result, double* left_result, double* right_result, TreeErr_t* err){
    if(*err) return;
    assert(result); 

    size_t arr_num_of_elem = sizeof(OPERATORS_INFO) / sizeof(op_info);
    if(node->data.op >= arr_num_of_elem){
        *err = INCORR_OPERATOR;
        return;
    }
    if(OPERATORS_INFO[node->data.op].function_calc == NULL){
        *err = NULL_PTR_TO_FUNC;
        return;
    }
    OPERATORS_INFO[node->data.op].function_calc(result, left_result, right_result);
}

static void CalcExpWithConst(TreeNode_t* node, double* result){
    *result = node->data.const_value;
}

static void CalcExpWithVar(metki* mtk, TreeNode_t* node, double* result, TreeErr_t* err){
    if(*err) return;
    assert(mtk); assert(result);

    if(node->data.var_code >= mtk->num_of_metki){
        *err = INCORR_IDX_IN_MTK;
        return;
    }
    if(!mtk->var_info[node->data.var_code].variable_name){
        *err = CANT_GET_VALUE_FOR_NULL_NAME_IN_MTK;
        return;
    }
    *result = mtk->var_info[node->data.var_code].value;
}
//---------------------------------------------------------------------------
// Optimization

static void TreeOptimizeConst(TreeNode_t *node, bool *is_optimized, TreeErr_t* err);

static void TreeOptimizeNeutral(TreeNode_t **result, TreeNode_t *node, bool *is_optimized, TreeErr_t* err);

TreeErr_t TreeOptimize(TreeNode_t **node){
    assert(node);

    TreeErr_t err = NO_MISTAKE;
    DEBUG_TREE(err = TreeNodeVerify(node);)
    if(err) return err;

    bool is_optimized = false;
    do{
        is_optimized = false; 

        TreeOptimizeConst(*node, &is_optimized, &err);
        if(err) return err;

        TreeOptimizeNeutral(node, *node, &is_optimized, &err);
        if(err) return err;
    }while(is_optimized);

    DEBUG_TREE(err = TreeNodeVerify(node);)
    return err;
}

static void TreeOptimizeConst(TreeNode_t *node, bool *is_optimized, TreeErr_t* err){
    if(*err) return;

    assert(node); 
    if(node->left){
        TreeOptimizeConst(node->left, is_optimized, err);
        if(*err) return;
    }
    if(node->right){
        TreeOptimizeConst(node->right, is_optimized, err);
        if(*err) return;
    }
    if(node->left && node->right && node->left->type == CONST && node->right->type == CONST && node->type == OPERATOR){
        size_t arr_num_of_elem = sizeof(OPERATORS_INFO) / sizeof(op_info);
        if(node->data.op >= arr_num_of_elem){
            *err = INCORR_OPERATOR;
            return;
        }
        if(OPERATORS_INFO[node->data.op].function_calc == NULL){
            *err = NULL_PTR_TO_FUNC;
            return;
        }
        OPERATORS_INFO[node->data.op].function_calc(&(node->data.const_value), &(node->left->data.const_value), &(node->right->data.const_value));
        NodeDtor(node->left);
        NodeDtor(node->right);
        node->type = CONST;
        node->left = NULL;
        node->right = NULL;
        *is_optimized = true;
    }
}

//--------------------------------------------------------------------------
// Optimizing neutrals

#define IS_EQUAL(node, value) ((node) && (node)->type == CONST && fabs((node)->data.const_value - value) < EPS)

static void ChangeKidParrentConn(TreeNode_t** result, TreeNode_t* node_for_change, TreeNode_t* new_node, bool* is_optimized);

static void TreeOptimizeNeutralAddSub(TreeNode_t** result, TreeNode_t* node, bool* is_optimized, TreeErr_t* err);

static void TreeOptimizeNeutralMul(TreeNode_t** result, TreeNode_t* node, bool* is_optimized, TreeErr_t* err);

static void TreeOptimizeNeutralDiv(TreeNode_t** result, TreeNode_t* node, bool* is_optimized, TreeErr_t* err);

static void TreeOptimizeNeutralDeg(TreeNode_t** result, TreeNode_t* node, bool* is_optimized, TreeErr_t* err);

static void TreeOptimizeNeutral(TreeNode_t **result, TreeNode_t *node, bool *is_optimized, TreeErr_t* err){
    if(*err) return;

    if(node->left){
        TreeOptimizeNeutral(result, node->left, is_optimized, err);
        if(*err) return;
    }
    if(node->right){
        TreeOptimizeNeutral(result, node->right, is_optimized, err);
        if(*err) return;
    }

    if(node->type == OPERATOR){
        switch(node->data.op){
            case INCORR:              *err = INCORR_OPERATOR; return;
            case OP_ADD: case OP_SUB: TreeOptimizeNeutralAddSub(result, node, is_optimized, err); if(*err) return; break;
            case OP_MUL:              TreeOptimizeNeutralMul(result, node, is_optimized, err);    if(*err) return; break;
            case OP_DIV:              TreeOptimizeNeutralDiv(result, node, is_optimized, err);    if(*err) return; break;
            case OP_DEG:              TreeOptimizeNeutralDeg(result, node, is_optimized, err);    if(*err) return; break;
            case OP_SIN:    case OP_COS:    case OP_TG:    case OP_CTG:   case OP_LN:
            case OP_SH:     case OP_CH:     case OP_TH:    case OP_CTH: 
            case OP_ARCSIN: case OP_ARCCOS: case OP_ARCTG: case OP_ARCCTG:                                          break;
            default:                  *err = INCORR_OPERATOR;
            }
    }
}

static void ChangeKidParrentConn(TreeNode_t** result, TreeNode_t* node_for_change, TreeNode_t* new_node, bool* is_optimized){
    new_node->parent = node_for_change->parent;
    if(node_for_change->parent){
        if(node_for_change == node_for_change->parent->left){
            node_for_change->parent->left = new_node;
        }
        else{
            node_for_change->parent->right = new_node;
        }
    }
    else{
        *result = new_node;
    }
    NodeDtor(node_for_change);
    *is_optimized = true;
}

static void TreeOptimizeNeutralAddSub(TreeNode_t** result, TreeNode_t* node, bool* is_optimized, TreeErr_t* err){
    if(*err) return;
    if(!node->left || !node->right){
        *err = NO_ELEM_FOR_BINARY_OP;
        return;
    }
    if(IS_EQUAL(node->left, 0) && node->data.op != OP_SUB){
        TreeDelNodeRecur(node->left);
        ChangeKidParrentConn(result, node, node->right, is_optimized); 
    }
    else if(IS_EQUAL(node->right, 0)){
        TreeDelNodeRecur(node->right);
        ChangeKidParrentConn(result, node, node->left, is_optimized);
    }
}

static void TreeOptimizeNeutralMul(TreeNode_t** result, TreeNode_t* node, bool* is_optimized, TreeErr_t* err){
    if(*err) return;
    if(!node->left || !node->right){
        *err = NO_ELEM_FOR_BINARY_OP;
        return;
    }
    if(IS_EQUAL(node->left, 0) || IS_EQUAL(node->right, 1)){
        TreeDelNodeRecur(node->right);
        ChangeKidParrentConn(result, node, node->left, is_optimized);
    }
    else if(IS_EQUAL(node->right, 0) || IS_EQUAL(node->left, 1)){
        TreeDelNodeRecur(node->left);
        ChangeKidParrentConn(result, node, node->right, is_optimized);
    }
}

static void TreeOptimizeNeutralDiv(TreeNode_t** result, TreeNode_t* node, bool* is_optimized, TreeErr_t* err){
    if(*err) return;
    if(!node->left || !node->right){
        *err = NO_ELEM_FOR_BINARY_OP;
        return;
    }
    if(IS_EQUAL(node->left, 0) && !IS_EQUAL(node->right, 0)){
        TreeDelNodeRecur(node->right);
        ChangeKidParrentConn(result, node, node->left, is_optimized);
    }
    else if(IS_EQUAL(node->right, 1)){
        TreeDelNodeRecur(node->right);
        ChangeKidParrentConn(result, node, node->left, is_optimized);
    }
}

static void TreeOptimizeNeutralDeg(TreeNode_t** result, TreeNode_t* node, bool* is_optimized, TreeErr_t* err){
    if(*err) return;
    if(!node->left || !node->right){
        *err = NO_ELEM_FOR_BINARY_OP;
        return;
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
}

//-----------------------------------------------------------------------------
// Undef dsl
#undef IS_EQUAL
#undef RES_L
#undef RES_R
#undef DEF_OP
#undef CALCUL_TREE_CPP