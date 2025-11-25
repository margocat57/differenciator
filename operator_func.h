#ifndef OPERATOR_FUNC_H
#define OPERATOR_FUNC_H
#include <string.h>
#include "data_struct/tree.h"
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

#ifdef LATEX_DUMP_CPP
    #define LATEX_DUMP(...) __VA_ARGS__
#else 
    #define LATEX_DUMP(...)
#endif //LATEX_DUMP_CPP

struct op_info{
    // reading from file
    OPERATORS op;
    const char* op_name;
    size_t num_of_symb;
    // dump
    const char* name_for_graph_dump;
    int priority;
    LATEX_DUMP(void(*function_dump)(FILE* file, TreeNode_t* node, metki* mtk);)
    // differenciate
    DIFFERENCIATOR(TreeNode_t*(*function_diff)(TreeNode_t* node, const size_t var_id, FILE* file, metki* mtk);)
    // calculate expression
    CALCUL(void(*function_calc)(double* result, double* left_result, double* right_result);)
};

const op_info OPERATORS_INFO[] = {
    {OPERATORS::INCORR},
    {OPERATORS::OP_ADD, "+",   1, "OP_ADD", 1, LATEX_DUMP(BinaryOperatorDumpAdd,) DIFFERENCIATOR(DiffAdd,) CALCUL(CalcAdd)},
    {OPERATORS::OP_SUB, "-",   1, "OP_SUB", 1, LATEX_DUMP(BinaryOperatorDumpSub,) DIFFERENCIATOR(DiffSub,) CALCUL(CalcSub)},
    {OPERATORS::OP_MUL, "*",   1, "OP_MUL", 2, LATEX_DUMP(BinaryOperatorDumpMul,) DIFFERENCIATOR(DiffMul,) CALCUL(CalcMul)},
    {OPERATORS::OP_DIV, "/",   1, "OP_DIV", 2, LATEX_DUMP(BinaryOperatorDumpDiv,) DIFFERENCIATOR(DiffDiv,) CALCUL(CalcDiv)},
    {OPERATORS::OP_DEG, "^",   1, "OP_DEG", 2, LATEX_DUMP(BinaryOperatorDumpDeg,) DIFFERENCIATOR(DiffDeg,) CALCUL(CalcDeg)},
    {OPERATORS::OP_SIN, "sin", 3, "OP_SIN", 2, LATEX_DUMP(UnaryOperatorDumpSin,)  DIFFERENCIATOR(DiffSin,) CALCUL(CalcSin)},
    {OPERATORS::OP_COS, "cos", 3, "OP_COS", 2, LATEX_DUMP(UnaryOperatorDumpCos,)  DIFFERENCIATOR(DiffCos,) CALCUL(CalcCos)},
    {OPERATORS::OP_LN,  "ln",  2, "OP_LN",  2, LATEX_DUMP(UnaryOperatorDumpLn,)   DIFFERENCIATOR(DiffLn,)  CALCUL(CalcLn) },
    {OPERATORS::OP_TG,  "tg",  2, "OP_TG",  2, LATEX_DUMP(UnaryOperatorDumpTg,)   DIFFERENCIATOR(DiffTg,)  CALCUL(CalcTg) },
    {OPERATORS::OP_CTG, "ctg", 3, "OP_CTG", 2, LATEX_DUMP(UnaryOperatorDumpCtg,)  DIFFERENCIATOR(DiffCtg,) CALCUL(CalcCtg)},
    {OPERATORS::OP_SH,  "sh",  2, "OP_SH",  2, LATEX_DUMP(UnaryOperatorDumpSh,)   DIFFERENCIATOR(DiffSh,)  CALCUL(CalcSh) },
    {OPERATORS::OP_CH,  "ch",  2, "OP_CH",  2, LATEX_DUMP(UnaryOperatorDumpCh,)   DIFFERENCIATOR(DiffCh,)  CALCUL(CalcCh) },
    {OPERATORS::OP_TH,  "th",  2, "OP_TH",  2, LATEX_DUMP(UnaryOperatorDumpTh,)   DIFFERENCIATOR(DiffTh,)  CALCUL(CalcTh) },
    {OPERATORS::OP_CTH, "cth", 3, "OP_CTH", 2, LATEX_DUMP(UnaryOperatorDumpCth,)  DIFFERENCIATOR(DiffCth,) CALCUL(CalcCth)}
};

#endif //OPERATOR_FUNC_H