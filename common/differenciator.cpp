#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "differenciator.h"
#include "../dump/graphviz_dump.h"
#include "../data_struct/tree_func.h"
#include "calcul_tree.h"
#include "../dump/latex_dump.h"

// Need to declare for dsl

TreeNode_t* Differenciate(TreeNode_t* node, const size_t var_id, FILE* file, metki* mtk);

//----------------------------------------------------------
// DSL define

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

#define DEF_OP(Op, Result, msg) \
static TreeNode_t* Diff##Op(TreeNode_t* node, const size_t var_id, FILE* file, metki* mtk){ \
    assert(node); \
    TreeNode_t* result = Result; \
    LatexDump(file, node, result, mtk, msg); \
    return result; \
}

DEF_OP(Add, ADD_(DL_, DR_), "It is obvious that:\n")
DEF_OP(Sub, SUB_(DL_, DR_), "It is easy to see:\n")
DEF_OP(Mul, ADD_(MUL_(DL_, CR_), MUL_(CL_, DR_)), "Understanding this transformation is left to the reader as a simple exercise:\n")
DEF_OP(Div, DIV_(SUB_(MUL_(DL_, CR_), MUL_(CL_, DR_) ), MUL_(node->right, node->right)) , "Should be known from school:\n")
DEF_OP(Cos, MUL_(DL_, MUL_(NUM_(-1), SIN_(CL_))), "According to the theorem (which number?) from paragraph ??:\n")
DEF_OP(Sin, MUL_(DL_, COS_(CL_)), "It is common knowledge:\n")
DEF_OP(Ln,  MUL_(DL_, DIV_(NUM_(1), CL_)), "As already shown earlier:\n")
DEF_OP(Tg,  DIV_(DL_, DEG_(COS_(CL_), NUM_(2))), " A similar one can be proved:\n")
DEF_OP(Ctg, MUL_(DIV_(DL_, DEG_(SIN_(CL_), NUM_(2))), NUM_(-1)), "If this is not obvious to you, try attending a lecture for a change:")
DEF_OP(Sh,  MUL_(DL_, CH_(CL_)), "Let's imagine this household as:\n")
DEF_OP(Ch,  MUL_(DL_, SH_(CL_)), "Plus a constant:\n")
DEF_OP(Th,  DIV_(DL_, DEG_(CH_(CL_), NUM_(2))), "A good, solid task?\n")
DEF_OP(Cth, MUL_(DIV_(DL_, DEG_(SH_(CL_), NUM_(2))), NUM_(-1)), "If you don't understand this obvious transformation, then you need to go into a program where they don't study mathematical analys:\n")

//---------------------------------------------------------
// DSL in func. Я очень не хочу это писать в дефайнах, так что будет функция

static bool is_type_num(TreeNode_t* node){
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
    LatexDump(file, node, result, mtk, "By the obvious theorem:\n");
    return result;
}
//----------------------------------------------------
struct operators_func{
    TreeNode_t*(*function_diff)(TreeNode_t* node, const size_t var_id, FILE* file, metki* mtk);
};

const operators_func FUNC_FOR_OPERATORS[] = {
    NULL,
    DiffAdd,
    DiffSub,
    DiffMul,
    DiffDiv,
    DiffDeg,
    DiffSin,
    DiffCos,
    DiffLn,
    DiffTg,
    DiffCtg,
    DiffSh,
    DiffCh,
    DiffTh,
    DiffCth
};


//---------------------------------------------------------
//---------------------------------------------------------
// Function that differencates and dumpes

FILE* StartLatexDump(const char* filename);

TreeNode_t* Differenciate(TreeNode_t* node, const size_t var_id, FILE* file, metki* mtk);

static size_t FindVarByName(const char* var_name, metki* mtk);

void ConnectWithParents(TreeNode_t* node);

void EndLatexDump(FILE* latex_file);

// цикл - сканфим какую производную хотим посчитать и ее считаем
void CreateDiffTree(const char* file_name, const char* var_name, Forest_t* forest){
    assert(file_name);
    FILE* latex_dump = StartLatexDump(file_name);
    size_t n = 0;
    printf("Which derivative do you want to calculate?\n");
    scanf("%zu", &n);

    for(size_t idx = 0; idx < n; idx++){
        TreeHead_t* head_new = TreeCtor();
        LatexDump(latex_dump, forest->head_arr[idx]->root, NULL, forest->mtk, "\\textbf{Let's calculate a simple derivative:}\n");
        head_new->root = Differenciate(forest->head_arr[idx]->root, FindVarByName(var_name, forest->mtk), latex_dump, forest->mtk); 
        ConnectWithParents(head_new->root);
        ForestAddElem(head_new, forest);
        tree_dump_func(head_new->root, head_new, "diff tree dump", __FILE__, __func__, __LINE__, forest->mtk);
    }

    // пока не работает - почему то ругается что нет \end
    // system("pdflatex -interaction=nonstopmode diff.tex");

    EndLatexDump(latex_dump);
}

static size_t FindVarByName(const char* var_name, metki* mtk){
    assert(var_name);
    for(size_t metka_idx = 0; metka_idx < mtk->first_free; metka_idx++){
        if(!strcmp(mtk->var_info[metka_idx].variable_name,var_name)){
            return metka_idx;
        }
    }
    return SIZE_MAX;
}


//--------------------------------------------------------------
//--------------------------------------------------------------
// differenciate

TreeNode_t* Differenciate(TreeNode_t* node, const size_t var_id, FILE* file, metki* mtk){
    if(node->type == INCORR_VAL){
        return NULL;
    }
    if(node->type == CONST || (node->type == VARIABLE && node->data.var_code != var_id)){
        return NUM_(0);
    }
    if(node->type == VARIABLE && node->data.var_code == var_id){
        return NUM_(1);
    }
    size_t arr_num_of_elem = sizeof(FUNC_FOR_OPERATORS) / sizeof(operators_func);
    if(node->data.op >= arr_num_of_elem){
        return NULL;
    }
    return FUNC_FOR_OPERATORS[node->data.op].function_diff(node, var_id, file, mtk);
}

//-------------------------------------------------------
// Connect parents - потому что на момент создания узла там будет кринж если передавать родителя

void ConnectWithParents(TreeNode_t* node){
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

//----------------------------------------------------------