#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "../common/differenciator.h"
#include "graphviz_dump.h"
#include "../data_struct/tree_func.h"
#include "../data_struct/forest.h"

static bool NeedStaples(TreeNode_t* node);

// Need to declare for dsl
void LatexDumpRecursive(FILE* file, TreeNode_t* node, metki* mtk);

//----------------------------------------------------------------
// DSL for latex define

#define DEF_BIN_OP(Op, start, cont, end) \
static void BinaryOperatorDump##Op(FILE* file, TreeNode_t* node, metki* mtk){ \
    assert(file); assert(node); assert(cont);\
    fprintf(file, "%s", start); \
    \
    bool left_staples = NeedStaples(node->left); \
    if (left_staples) fprintf(file, "("); \
    LatexDumpRecursive(file, node->left, mtk); \
    if (left_staples) fprintf(file, ")"); \
    \
    fprintf(file, "%s", cont); \
    \
    bool right_staples = NeedStaples(node->right); \
    if (right_staples) fprintf(file, "("); \
    LatexDumpRecursive(file, node->right, mtk); \
    if (right_staples) fprintf(file, ")"); \
    \
    fprintf(file, "%s", end); \
}

DEF_BIN_OP(Add, "" , " + ", "")
DEF_BIN_OP(Sub, "" , " - ", "")
DEF_BIN_OP(Mul, "" , " \\cdot ", "")
DEF_BIN_OP(Div, "\\frac{", "}{", "}")
DEF_BIN_OP(Deg, "", "^{", "}")

#define DEF_UN_OP(Op, start, end) \
static void UnaryOperatorDump##Op(FILE* file, TreeNode_t* node, metki* mtk){ \
    assert(file); assert(node); \
    assert(start); assert(end); \
    \
    fprintf(file, "%s", start); \
    \
    bool left_staples = NeedStaples(node->left); \
    if (left_staples) fprintf(file, "("); \
    LatexDumpRecursive(file, node->left, mtk); \
    if (left_staples) fprintf(file, ")"); \
    \
    fprintf(file, "%s", end); \
}

DEF_UN_OP(Sin, "sin(" , ")")
DEF_UN_OP(Cos, "cos(" , ")")
DEF_UN_OP(Ln,  "ln(" , ")")
DEF_UN_OP(Tg,  "tg(" , ")")
DEF_UN_OP(Ctg, "ctg(" , ")")
DEF_UN_OP(Sh,  "sh(" , ")")
DEF_UN_OP(Ch,  "ch(" , ")")
DEF_UN_OP(Th,  "th(" , ")")
DEF_UN_OP(Cth, "cth(" , ")")

//----------------------------------------------------------------


struct operators_func{
    void(*function_dump)(FILE* file, TreeNode_t* node, metki* mtk);
    int priority;
};

const operators_func FUNC_FOR_OPERATORS[] = {
    {NULL, 0},
    {BinaryOperatorDumpAdd, 1},
    {BinaryOperatorDumpSub, 1},
    {BinaryOperatorDumpMul, 2},
    {BinaryOperatorDumpDiv, 2},
    {BinaryOperatorDumpDeg, 2},
    {UnaryOperatorDumpSin,  2},
    {UnaryOperatorDumpCos,  2},
    {UnaryOperatorDumpLn,   2},
    {UnaryOperatorDumpTg,   2},
    {UnaryOperatorDumpCtg,  2},
    {UnaryOperatorDumpSh,   2},
    {UnaryOperatorDumpCh,   2},
    {UnaryOperatorDumpTh,   2},
    {UnaryOperatorDumpCth,  2}
};

//-------------------------------------------------------------
//--------------------------------------------------------------
// Dump

void LatexDump(FILE* file, TreeNode_t* node, TreeNode_t* result, metki* mtk, const char* comment){
    fprintf(file, "%s" ,comment);
    fprintf(file, "\\begin{dmath}\n");
    if(result) fprintf(file, "(\n");
    LatexDumpRecursive(file, node, mtk);
    if(result) fprintf(file, ")' = \n");
    if(result) LatexDumpRecursive(file, result, mtk);
    fprintf(file, "\\end{dmath}\n");
}

void LatexDumpRecursive(FILE* file, TreeNode_t* node, metki* mtk){
    switch(node->type){
        case CONST:
            fprintf(file, "%.2lf" ,node->data.const_value);
            break;
        case VARIABLE:
            fprintf(file, "%s" , mtk->var_info[node->data.var_code].variable_name);
            break;
        case OPERATOR:
            size_t arr_num_of_elem = sizeof(FUNC_FOR_OPERATORS) / sizeof(operators_func);
            if(node->data.op >= arr_num_of_elem){
                return;
            }
            FUNC_FOR_OPERATORS[node->data.op].function_dump(file, node, mtk);
    }
}

static bool NeedStaples(TreeNode_t* node){
    if(node->type != OPERATOR || !node->parent || node->parent->type != OPERATOR || !node){
        return false;
    }
    int node_priority = FUNC_FOR_OPERATORS[node->data.op].priority;
    int node_parent_priority = FUNC_FOR_OPERATORS[node->parent->data.op].priority;
    if(node_priority < node_parent_priority){
        return true;
    }
    return false;
}

FILE* StartLatexDump(const char* filename){
    assert(filename);
    FILE* latex_file = fopen(filename, "w");
    fprintf(latex_file, "\\documentclass[10pt,a4paper]{article}\n\\usepackage[margin=0.5in]{geometry}\n\\usepackage{amsmath}\n\\usepackage{breqn}\n\\begin{document}\n\n");
    return latex_file;
}

//--------------------------------------------------------------

void EndLatexDump(FILE* latex_file){
    assert(latex_file);
    fprintf(latex_file, "\\end{document}\n");
    fclose(latex_file);
}

#undef DEF_BIN_OP
#undef DEF_UN_OP