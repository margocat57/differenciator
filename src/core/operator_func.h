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
    // for staples
    bool is_unary_op;
    // dump graphviz
    const char* name_for_graph_dump;
    // dump latex
    int priority;
    const char* dump_start;
    const char* dump_cont;
    const char* dump_end;
    // dump gnuplot
    const char* dump_gnuplot_start;
    const char* dump_gnuplot_cont;
    const char* dump_gnuplot_end;
    // differenciate
    DIFFERENCIATOR(TreeNode_t*(*function_diff)(TreeNode_t* node, const size_t var_id, FILE* file, metki* mtk, TreeErr_t *err);)
    // calculate expression
    CALCUL(void(*function_calc)(double* result, double* left_result, double* right_result);)
};





const op_info OPERATORS_INFO[] = {
    {INCORR},
    {OP_ADD,    "+",      1, false, "OP_ADD",    1,  "" ,                       " + ",      "", "" ,            " + ",      "",  DIFFERENCIATOR(DiffAdd,)    CALCUL(CalcAdd)},
    {OP_SUB,    "-",      1, false, "OP_SUB",    1,  "" ,                       " - ",      "", "" ,            " - ",      "",  DIFFERENCIATOR(DiffSub,)    CALCUL(CalcSub)},
    {OP_MUL,    "*",      1, false, "OP_MUL",    2,  "" ,                       " \\cdot ", "", "" ,            " * ",      "",  DIFFERENCIATOR(DiffMul,)    CALCUL(CalcMul)},
    {OP_DIV,    "/",      1, false, "OP_DIV",    2,  "\\frac{",                 "}{",       "}","" ,            " / ",      "",  DIFFERENCIATOR(DiffDiv,)    CALCUL(CalcDiv)},
    {OP_DEG,    "^",      1, false, "OP_DEG",    3,  "",                        "^{",       "}","" ,            " ** ",     "",  DIFFERENCIATOR(DiffDeg,)    CALCUL(CalcDeg)},
    {OP_SIN,    "sin",    3, true,  "OP_SIN",    2,  "\\sin(" ,                 NULL ,      ")","sin(" ,        NULL ,      ")", DIFFERENCIATOR(DiffSin,)    CALCUL(CalcSin)},
    {OP_COS,    "cos",    3, true,  "OP_COS",    2,  "\\cos(" ,                 NULL ,      ")","cos(" ,        NULL ,      ")", DIFFERENCIATOR(DiffCos,)    CALCUL(CalcCos)},
    {OP_LN,     "ln",     2, true,  "OP_LN",     2,  "\\ln(" ,                  NULL ,      ")","log(" ,        NULL ,      ")", DIFFERENCIATOR(DiffLn,)     CALCUL(CalcLn) },
    {OP_TG,     "tg",     2, true,  "OP_TG",     2,  "\\tan(" ,                 NULL ,      ")","tan(" ,        NULL ,      ")", DIFFERENCIATOR(DiffTg,)     CALCUL(CalcTg) },
    {OP_CTG,    "ctg",    3, true,  "OP_CTG",    2,  "\\cot(" ,                 NULL ,      ")","1/tan(" ,      NULL ,      ")", DIFFERENCIATOR(DiffCtg,)    CALCUL(CalcCtg)},
    {OP_SH,     "sh",     2, true,  "OP_SH",     2,  "\\sinh(",                 NULL ,      ")","sinh(",        NULL ,      ")", DIFFERENCIATOR(DiffSh,)     CALCUL(CalcSh) },
    {OP_CH,     "ch",     2, true,  "OP_CH",     2,  "\\cosh(",                 NULL ,      ")","cosh(",        NULL ,      ")", DIFFERENCIATOR(DiffCh,)     CALCUL(CalcCh) },
    {OP_TH,     "th",     2, true,  "OP_TH",     2,  "\\tanh(",                 NULL ,      ")","tanh(",        NULL ,      ")", DIFFERENCIATOR(DiffTh,)     CALCUL(CalcTh) },
    {OP_CTH,    "cth",    3, true,  "OP_CTH",    2,  "\\coth(",                 NULL ,      ")","1/tanh(",      NULL ,      ")", DIFFERENCIATOR(DiffCth,)    CALCUL(CalcCth)},
    {OP_ARCSIN, "arcsin", 6, true,  "OP_ARCSIN", 2,  "\\arcsin(",               NULL ,      ")","asin(",        NULL ,      ")", DIFFERENCIATOR(DiffArcsin,) CALCUL(CalcArcsin)},
    {OP_ARCCOS, "arccos", 6, true,  "OP_ARCCOS", 2,  "\\arccos(",               NULL ,      ")","acos(",        NULL ,      ")", DIFFERENCIATOR(DiffArccos,) CALCUL(CalcArccos)},
    {OP_ARCTG,  "arctg",  5, true,  "OP_ARCTG",  2,  "\\arctan(",               NULL ,      ")","atan(",        NULL ,      ")", DIFFERENCIATOR(DiffArctg,)  CALCUL(CalcArctg)},
    {OP_ARCCTG, "arcctg", 6, true,  "OP_ARCCTG", 2,  "\\operatorname{arccot}(", NULL ,      ")","pi/2 - atan(", NULL ,      ")", DIFFERENCIATOR(DiffArcctg,) CALCUL(CalcArcctg)}
};

#endif //OPERATOR_FUNC_H