#ifndef OPERATOR_FUNC_H
#define OPERATOR_FUNC_H
#include <string.h>
#include "tree.h"
#include <stdio.h>

#ifdef DIFFERENCIATOR_CPP
    #define DIFFERENCIATOR(...) __VA_ARGS__
#else 
    #define DIFFERENCIATOR(...)
#endif //DIFFERENCIATOR_CPP

#ifdef CALCUL_TREE_CPP
    #define CALCUL(...) __VA_ARGS__
#else 
    #define CALCUL(...)
#endif //CALCUL_TREE_CPP


struct op_info{
    // reading from file
    OPERATORS op;
    const char* op_name;
    size_t num_of_symb;
    // dump
    const char* name_for_graph_dump;
    int priority;
    const char* dump_start;
    const char* dump_cont;
    const char* dump_end;
    // differenciate
    DIFFERENCIATOR(TreeNode_t*(*function_diff)(TreeNode_t* node, const size_t var_id, FILE* file, metki* mtk);)
    // calculate expression
    CALCUL(void(*function_calc)(double* result, double* left_result, double* right_result);)
};





const op_info OPERATORS_INFO[] = {
    {INCORR},
    {OP_ADD, "+",   1, "OP_ADD", 1,  "" ,       " + ",      "",  DIFFERENCIATOR(DiffAdd,) CALCUL(CalcAdd)},
    {OP_SUB, "-",   1, "OP_SUB", 1,  "" ,       " - ",      "",  DIFFERENCIATOR(DiffSub,) CALCUL(CalcSub)},
    {OP_MUL, "*",   1, "OP_MUL", 2,  "" ,       " \\cdot ", "",  DIFFERENCIATOR(DiffMul,) CALCUL(CalcMul)},
    {OP_DIV, "/",   1, "OP_DIV", 2,  "\\frac{", "}{",       "}", DIFFERENCIATOR(DiffDiv,) CALCUL(CalcDiv)},
    {OP_DEG, "^",   1, "OP_DEG", 2,  "",        "^{",       "}", DIFFERENCIATOR(DiffDeg,) CALCUL(CalcDeg)},
    {OP_SIN, "sin", 3, "OP_SIN", 2,  "\\sin(" , NULL ,      ")", DIFFERENCIATOR(DiffSin,) CALCUL(CalcSin)},
    {OP_COS, "cos", 3, "OP_COS", 2,  "\\cos(" , NULL ,      ")", DIFFERENCIATOR(DiffCos,) CALCUL(CalcCos)},
    {OP_LN,  "ln",  2, "OP_LN",  2,  "\\ln(" ,  NULL ,      ")", DIFFERENCIATOR(DiffLn,)  CALCUL(CalcLn) },
    {OP_TG,  "tg",  2, "OP_TG",  2,  "\\tan(" , NULL ,      ")", DIFFERENCIATOR(DiffTg,)  CALCUL(CalcTg) },
    {OP_CTG, "ctg", 3, "OP_CTG", 2,  "\\cot(" , NULL ,      ")", DIFFERENCIATOR(DiffCtg,) CALCUL(CalcCtg)},
    {OP_SH,  "sh",  2, "OP_SH",  2,  "\\sinh(", NULL ,      ")", DIFFERENCIATOR(DiffSh,)  CALCUL(CalcSh) },
    {OP_CH,  "ch",  2, "OP_CH",  2,  "\\cosh(", NULL ,      ")", DIFFERENCIATOR(DiffCh,)  CALCUL(CalcCh) },
    {OP_TH,  "th",  2, "OP_TH",  2,  "\\tanh(", NULL ,      ")", DIFFERENCIATOR(DiffTh,)  CALCUL(CalcTh) },
    {OP_CTH, "cth", 3, "OP_CTH", 2,  "\\coth(", NULL ,      ")", DIFFERENCIATOR(DiffCth,) CALCUL(CalcCth)}
};

#endif //OPERATOR_FUNC_H