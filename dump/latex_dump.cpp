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

void LatexDumpTaylor(FILE *file, Forest_t *forest_diff, Forest_t *forest){
    fprintf(file, "{\\large \\textbf{Taylor Series:}}\n");
    fprintf(file, "\\begin{dmath}\n");
    fprintf(file, "T(");
    LatexDumpRecursive(file, forest_diff->head_arr[0]->root, forest_diff->mtk);
    fprintf(file, ") = ");
    for(size_t idx = 0; idx < forest->first_free_place; idx++){
        if(forest->head_arr[idx]->root->data.const_value == 0){
            continue;
        }
        LatexDumpRecursive(file, forest->head_arr[idx]->root, forest_diff->mtk);
        fprintf(file, " + ");
    }
    fprintf(file, "...");
    fprintf(file, "\\end{dmath}\n");
}

void LatexDump(FILE* file, TreeNode_t* node, TreeNode_t* result, metki* mtk, const char* comment){
    if(comment) fprintf(file, "%s" ,comment);
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
    if(!node || node->type != OPERATOR || !node->parent || node->parent->type != OPERATOR){
        return false;
    }
    int node_priority = FUNC_FOR_OPERATORS[node->data.op].priority;
    int node_parent_priority = FUNC_FOR_OPERATORS[node->parent->data.op].priority;
    if(node_priority < node_parent_priority){
        return true;
    }
    if(node->parent->data.op == OP_SUB){
        return true;
    }
    return false;
}


FILE* StartLatexDump(const char* filename){
    assert(filename);
    FILE* latex_file = fopen(filename, "w");
    fprintf(latex_file, "\\documentclass[a4paper,12pt]{report}\n\\usepackage[utf8]{inputenc}\n\\usepackage{amsmath,amssymb}\n\\usepackage{geometry}\n\\usepackage{breqn}\n\\newtheorem{definition}{Definition}\n\\newtheorem{obviousfact}{Obvious Fact}\n");
    fprintf(latex_file, "\\title{MatematiCAL anal for economists}\n");
    fprintf(latex_file, "\\author{Anonymus fan of mat.anal}\n\n");
    fprintf(latex_file, "\\begin{document}\n\n");
    fprintf(latex_file, "\\maketitle\n\n");
    fprintf(latex_file, "\\chapter*{Preface}\n\n");
    fprintf(latex_file, "This textbook is designed to assist economics students studying the basic course of mathematical analysis. It summarizes the entire mathematical analysis course taught to economists in the best undergraduate economics program in Eastern Europe.\n\n");
    fprintf(latex_file, "The lectures include only the essential material, ensuring that students who have achieved top honors in national economics Olympiads are not overburdened and can maintain their sense of superiority over the rest of the world. After all, they likely mastered all this material in kindergarten (or at the latest, by first grade). The division of topics into lectures corresponds well to the actual pace of the course, which spans an entire semester. Almost all statements in the course are self-evident, and their proofs are left to the reader as straightforward exercises.\n\n");

    return latex_file;
}

void LatexDumpDecimals(FILE* latex_file){
    assert(latex_file);
    fprintf(latex_file, "\\chapter{Numbers}\n\n");
    fprintf(latex_file, "\\section{Basic Classes of Numbers}\n\n");
    
    fprintf(latex_file, "First, let's introduce the definitions of the basic classes of numbers that ");
    fprintf(latex_file, "we will constantly work with throughout the course.\n\n");
    
    fprintf(latex_file, "\\begin{definition}\n");
    fprintf(latex_file, "Numbers 1, 2, 3, \\ldots are called \\textit{natural numbers}. ");
    fprintf(latex_file, "The notation for the set of all natural numbers is $\\mathbb{N}$.\n");
    fprintf(latex_file, "\\end{definition}\n\n");
    
    fprintf(latex_file, "\\begin{definition}\n");
    fprintf(latex_file, "A number is called an \\textit{integer} if it is equal to\\ldots ");
    fprintf(latex_file, "but you don't need this because everything in economics is positive.\n");
    fprintf(latex_file, "\\end{definition}\n\n");
    
    fprintf(latex_file, "\\begin{definition}\n");
    fprintf(latex_file, "A number is called \\textit{rational} if it can be represented as ");
    fprintf(latex_file, "something above a line and something below a line.\n");
    fprintf(latex_file, "\\end{definition}\n\n");
    
    fprintf(latex_file, "\\begin{definition}\n");
    fprintf(latex_file, "A number is called \\textit{irrational} if it is not rational.\n");
    fprintf(latex_file, "\\end{definition}\n\n");
    
    fprintf(latex_file, "\\begin{obviousfact}\n");
    fprintf(latex_file, "The sum of all natural numbers equals $-1/12$.\n");
    fprintf(latex_file, "\\end{obviousfact}\n");

    fprintf(latex_file, "\\textbf{Kindergarten Example:}\n");
    fprintf(latex_file, "If Vasya had 2 apples and Petya took 1 apple from him, how many apples does Vasya have left? The answer is obviously $-1/12$, as any advanced mathematician knows.\n");

}

void LatexDumpChapterDiff(FILE* latex_file){
    assert(latex_file);
    fprintf(latex_file, "\\chapter{Derivative}\n\n");
    fprintf(latex_file, "\\section{Basic derivatives}\n\n");
    fprintf(latex_file, "\\begin{definition}\n");
    fprintf(latex_file, "The definition of derivative is omitted because it is obvious.\n");
    fprintf(latex_file, "\\end{definition}\n");
    fprintf(latex_file, "Everything in this chapter is so obvious that no additional explanations will be provided - we'll immediately proceed to analyze an example from kindergarten.\n\n");
}

void LatexDumpChapterTaylor(FILE* latex_file){
    assert(latex_file);
    fprintf(latex_file, "\\chapter{Taylor}\n\n");
    fprintf(latex_file, "\\section{Taylor's formula with the remainder term (and why is it needed? Without it, everything is obvious)}\n\n");
    fprintf(latex_file, "\\begin{definition}\n");
    fprintf(latex_file, "Taylor's formula is obvious, so no additional explanations will be given. Let's start straight with an example.\n");
    fprintf(latex_file, "\\end{definition}\n");
}

//--------------------------------------------------------------

void EndLatexDump(FILE* latex_file){
    assert(latex_file);
    fprintf(latex_file, "\\end{document}\n");
    fclose(latex_file);
}

#undef DEF_BIN_OP
#undef DEF_UN_OP