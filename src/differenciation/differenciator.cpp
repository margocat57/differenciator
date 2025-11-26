#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "differenciator.h"
#include "../dump/graphviz_dump.h"
#include "../core/tree_func.h"
#include "../calculation_optimization/calcul_tree.h"
#include "../dump/latex_dump.h"
#include "../utils/rofl_matan.h"

// Need to declare for dsl

static TreeNode_t* Differenciate(TreeNode_t* node, const size_t var_id, FILE* file, metki* mtk);

static void ConnectWithParents(TreeNode_t *node);

//----------------------------------------------------------
// DSL define
#define DIFFERENCIATOR_CPP
#define DL_                     Differenciate(node->left, var_id, file, mtk)
#define DR_                     Differenciate(node->right, var_id, file, mtk)
#define CL_                     NodeCopy(node->left)
#define CR_                     NodeCopy(node->right)
#define ADD_(left, right)       NodeCtor(OPERATOR, (TreeElem_t){.op = OP_ADD}, NULL, left, right)
#define SUB_(left, right)       NodeCtor(OPERATOR, (TreeElem_t){.op = OP_SUB}, NULL, left, right)
#define MUL_(left, right)       NodeCtor(OPERATOR, (TreeElem_t){.op = OP_MUL}, NULL, left, right)
#define DIV_(left, right)       NodeCtor(OPERATOR, (TreeElem_t){.op = OP_DIV}, NULL, left, right)
#define DEG_(left, right)       NodeCtor(OPERATOR, (TreeElem_t){.op = OP_DEG}, NULL, left, right)
#define SIN_(left)              NodeCtor(OPERATOR, (TreeElem_t){.op = OP_SIN}, NULL, left, NULL)
#define COS_(left)              NodeCtor(OPERATOR, (TreeElem_t){.op = OP_COS}, NULL, left, NULL)
#define LN_(left)               NodeCtor(OPERATOR, (TreeElem_t){.op = OP_LN},  NULL, left, NULL)
#define SH_(left)               NodeCtor(OPERATOR, (TreeElem_t){.op = OP_SH},  NULL, left, NULL)
#define CH_(left)               NodeCtor(OPERATOR, (TreeElem_t){.op = OP_CH},  NULL, left, NULL)
#define TH_(left)               NodeCtor(OPERATOR, (TreeElem_t){.op = OP_TH},  NULL, left, NULL)
#define CTH_(left)              NodeCtor(OPERATOR, (TreeElem_t){.op = OP_CTH}, NULL, left, NULL)
#define NUM_(num)               NodeCtor(CONST, (TreeElem_t){.const_value = num}, NULL, NULL, NULL)
#define L                       node->left
#define R                       node->right
#define VAR_NODE(var_id)        NodeCtor(VARIABLE, (TreeElem_t){.var_code = var_id}, NULL, NULL, NULL)

#define DEF_OP(Op, Result)                                                                \
    static TreeNode_t *Diff##Op(TreeNode_t *node, const size_t var_id, FILE *file, metki *mtk){                                                                                          \
        assert(node);                                                                          \
        TreeNode_t *result = Result;                                                           \
        ConnectWithParents(result);                                                            \
        TreeOptimize(&result);                                                                 \
        const char *msg = GenerateRoflMsg();                                                   \
        LatexDump(file, node, result, mtk, msg);\
        return result;\
    }\


DEF_OP(Add, ADD_(DL_, DR_))
DEF_OP(Sub, SUB_(DL_, DR_))
DEF_OP(Mul, ADD_(MUL_(DL_, CR_), MUL_(CL_, DR_)))
DEF_OP(Div, DIV_(SUB_(MUL_(DL_, CR_), MUL_(CL_, DR_)), DEG_(CR_, NUM_(2))))
DEF_OP(Cos, MUL_(DL_, MUL_(NUM_(-1), SIN_(CL_))))
DEF_OP(Sin, MUL_(DL_, COS_(CL_)))
DEF_OP(Ln, MUL_(DL_, DIV_(NUM_(1), CL_)))
DEF_OP(Tg, DIV_(DL_, DEG_(COS_(CL_), NUM_(2))))
DEF_OP(Ctg, MUL_(DIV_(DL_, DEG_(SIN_(CL_), NUM_(2))), NUM_(-1)))
DEF_OP(Sh, MUL_(DL_, CH_(CL_)))
DEF_OP(Ch, MUL_(DL_, SH_(CL_)))
DEF_OP(Th, DIV_(DL_, DEG_(CH_(CL_), NUM_(2))))
DEF_OP(Cth, MUL_(DIV_(DL_, DEG_(SH_(CL_), NUM_(2))), NUM_(-1)))

//---------------------------------------------------------
// DSL in func. Я очень не хочу это писать в дефайнах, так что будет функция

static bool is_type_num(TreeNode_t *node){
    assert(node);
    return node->type == CONST;
}

// любой другой тип кроме числа
static bool is_type_not_num(TreeNode_t* node){
    assert(node);
    return (node->type == VARIABLE || node->type == OPERATOR);
}
//--------------------------------------------------
// And one function that is harder than DSL func
static TreeNode_t* DiffDeg(TreeNode_t* node, const size_t var_id, FILE* file, metki* mtk){
    assert(node);
    TreeNode_t* result = NULL;
    if(is_type_not_num(L) && is_type_num(R)){
        result = MUL_(MUL_(CR_, DEG_(CL_, SUB_(CR_, NUM_(1)))), DL_);
    }
    else if(is_type_num(L) && is_type_not_num(R)){
        result = MUL_(MUL_(LN_(CL_), MUL_(CL_, CR_)), DR_);
    }
    else if(is_type_not_num(L) && is_type_not_num(R)){
        result = MUL_(DEG_(CL_, CR_), ADD_(MUL_(DR_, LN_(CL_)), DIV_(MUL_(DL_, CR_), CL_)));
    }
    ConnectWithParents(result);
    TreeOptimize(&result); 
    LatexDump(file, node, result, mtk, "By the obvious theorem:\n");
    return result;
}
//----------------------------------------------------

#include "../core/operator_func.h"

//---------------------------------------------------------
//---------------------------------------------------------
// Function that differencates and dumpes

static TreeErr_t CreateDiffTree(const size_t var_id, Forest_t *forest, size_t idx, FILE *latex_dump);

static void AskAboutN(size_t *n);

static size_t FindVarCodeToDiff(metki *mtk);

static size_t FindVarByName(const char* var_name, metki* mtk);

TreeErr_t CreateDiffForest(Forest_t *forest, FILE *latex_dump){
    TreeErr_t err = NO_MISTAKE_T;
    DEBUG_TREE(err = ForestVerify(forest);)
    if (err) return err;

    size_t n = 0;
    AskAboutN(&n);

    size_t var_id = FindVarCodeToDiff(forest->mtk);
    if(var_id == SIZE_MAX){
        fprintf(stderr, "Incorr variable to differenciate");
        return INCORR_VAR_TO_DIFF;
    }

    for(size_t idx = 0; idx < n; idx++){
        CHECK_AND_RET_TREEERR(CreateDiffTree(var_id, forest, idx, latex_dump));
    }

    DEBUG_TREE(err = ForestVerify(forest);)
    return err;
}

static void AskAboutN(size_t *n){
    assert(n);
    printf("Which derivative do you want to calculate?\n");
    scanf("%zu", n);
}

static size_t FindVarByName(const char *var_name, metki *mtk){
    assert(var_name);
    for(size_t metka_idx = 0; metka_idx < mtk->first_free; metka_idx++){
        if(!strcmp(mtk->var_info[metka_idx].variable_name, var_name)){
            return metka_idx;
        }
    }
    return SIZE_MAX;
}

static size_t FindVarCodeToDiff(metki* mtk){
    char var_name[50] = {};
    printf("For which variable find the derivative\n");
    scanf("%49s", var_name);
    return FindVarByName(var_name, mtk);
}

static TreeErr_t CreateDiffTree(const size_t var_id, Forest_t *forest, size_t idx, FILE *latex_dump){
    TreeHead_t* head_new = TreeCtor();
    CHECK_AND_RET_TREEERR(LatexDump(latex_dump, forest->head_arr[idx]->root, NULL, forest->mtk, "\\textbf{Let's calculate a simple derivative:}\n"));
    head_new->root = Differenciate(forest->head_arr[idx]->root, var_id, latex_dump, forest->mtk);
    CHECK_AND_RET_TREEERR(ForestAddElem(head_new, forest));
    return NO_MISTAKE_T;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
// differenciate

static TreeNode_t* Differenciate(TreeNode_t* node, const size_t var_id, FILE* file, metki* mtk){
    assert(node); assert(file); assert(mtk);
    if(node->type == INCORR_VAL){
        return NULL;
    }
    if(node->type == CONST || (node->type == VARIABLE && node->data.var_code != var_id)){
        return NUM_(0);
    }
    if(node->type == VARIABLE && node->data.var_code == var_id){
        return NUM_(1);
    }
    size_t arr_num_of_elem = sizeof(OPERATORS_INFO) / sizeof(op_info);
    if(node->data.op >= arr_num_of_elem || OPERATORS_INFO[node->data.op].function_diff == NULL){
        return NULL;
    }
    return OPERATORS_INFO[node->data.op].function_diff(node, var_id, file, mtk);
}

//-------------------------------------------------------
// Connect parents - потому что на момент создания узла там будет кринж если передавать родителя

static void ConnectWithParents(TreeNode_t* node){
    if (!node) return;

    ConnectWithParents(node->left);
    ConnectWithParents(node->right);

    if(node->left != NULL){
        node->left->parent = node;
    }
    if(node->right != NULL){
        node->right->parent = node;
    }
}

//---------------------------------------------------------------
// Taylor - only for one variable(for two and more too complex)

static TreeErr_t CreateTaylorTree(size_t idx, Forest_t *forest_taylor, Forest_t *diff_forest, FILE *latex_dump);

TreeErr_t CreateTaylorForest(Forest_t *forest_taylor, Forest_t *diff_forest, FILE *latex_dump){
    assert(forest_taylor); assert(diff_forest); assert(latex_dump);

    TreeErr_t err = NO_MISTAKE_T;
    DEBUG_TREE( err = ForestVerify(diff_forest);
                err = ForestVerify(forest_taylor);)
    if(err) return err;

    if(diff_forest->mtk->first_free != 1){
        fprintf(latex_dump, "\\textbf{Can't create Taylor for two and more var}\n");
        return err;
    }

    printf("For Taylor formula at first the derivatives must be calcutated:\t");
    fprintf(latex_dump, "{\\large \\textbf{At first the derivatives must be calcutated:}}\n\n");
    CHECK_AND_RET_TREEERR(CreateDiffForest(diff_forest, latex_dump));

    metki_add_values(diff_forest->mtk);

    for(size_t idx = 0; idx < diff_forest->first_free_place; idx++){
        CHECK_AND_RET_TREEERR(CreateTaylorTree(idx, forest_taylor, diff_forest, latex_dump));
    }
    metki_del_values(diff_forest->mtk);

    DEBUG_TREE( err = ForestVerify(diff_forest);
                err = ForestVerify(forest_taylor);)
    return err;
}

static TreeErr_t CreateTaylorTree(size_t idx, Forest_t *forest_taylor, Forest_t *diff_forest, FILE *latex_dump){
    double result = 0;
    TreeHead_t * head_new = TreeCtor();
    CHECK_AND_RET_TREEERR(CalcTreeExpression(diff_forest, idx, &result, true));
    head_new->root = MUL_(DIV_(NUM_(result), NUM_(tgammaf(idx + 1))), DEG_(SUB_(VAR_NODE(0), NUM_(diff_forest->mtk->var_info[0].value)), NUM_(idx)));
    ConnectWithParents(head_new->root);
    CHECK_AND_RET_TREEERR(TreeOptimize(&(head_new->root)));
    CHECK_AND_RET_TREEERR(ForestAddElem(head_new, forest_taylor));
    return NO_MISTAKE_T;
}

//----------------------------------------------------------
// DSL undef

#undef  L
#undef  R
#undef  DL_
#undef  DR_
#undef  CL_
#undef  CR_
#undef  ADD_
#undef  SUB_
#undef  MUL_
#undef  DIV_
#undef  DEG_
#undef  SIN_
#undef  COS_
#undef  TG_
#undef  CTG_
#undef  SH_
#undef  CH_
#undef  TH_
#undef  CTH_
#undef  LN_
#undef  NUM_
#undef  DEF_OP
#undef  VAR_NODE
#undef  DIFFERENCIATOR_CPP
//----------------------------------------------------------
