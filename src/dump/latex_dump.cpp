#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "../differenciation/differenciator.h"
#include "graphviz_dump.h"
#include "../core/tree_func.h"
#include "../core/forest.h"

const size_t MAX_CMD_BUFFER = 2048;

static TreeErr_t NeedStaples(TreeNode_t* node, bool* need_staples);

// Need to declare for dsl
static TreeErr_t LatexDumpRecursive(FILE* file, TreeNode_t* node, metki* mtk);

//----------------------------------------------------------------
// DSL for latex define

//! to func
//! макрос который вызывает функцию
#define LATEX_DUMP_CPP
#define DEF_BIN_OP(Op, start, cont, end) \
static TreeErr_t BinaryOperatorDump##Op(FILE* file, TreeNode_t* node, metki* mtk){ \
    assert(file); assert(node); \
    fprintf(file, "%s", start); \
    \
    bool left_staples = false; \
    CHECK_AND_RET_TREEERR(NeedStaples(node->left, &left_staples)); \
    if (left_staples) fprintf(file, "("); \
    CHECK_AND_RET_TREEERR(LatexDumpRecursive(file, node->left, mtk)); \
    if (left_staples) fprintf(file, ")"); \
    \
    fprintf(file, "%s", cont); \
    \
    bool right_staples = false; \
    CHECK_AND_RET_TREEERR(NeedStaples(node->right, &right_staples)); \
    if (right_staples) fprintf(file, "("); \
    CHECK_AND_RET_TREEERR(LatexDumpRecursive(file, node->right, mtk)); \
    if (right_staples) fprintf(file, ")"); \
    \
    fprintf(file, "%s", end); \
    return NO_MISTAKE_T; \
}

DEF_BIN_OP(Add, "" , " + ", "")
DEF_BIN_OP(Sub, "" , " - ", "")
DEF_BIN_OP(Mul, "" , " \\cdot ", "")
DEF_BIN_OP(Div, "\\frac{", "}{", "}")
DEF_BIN_OP(Deg, "", "^{", "}")

#define DEF_UN_OP(Op, start, end) \
static TreeErr_t UnaryOperatorDump##Op(FILE* file, TreeNode_t* node, metki* mtk){ \
    assert(file); assert(node); \
    \
    fprintf(file, "%s", start); \
    \
    bool left_staples = false; \
    CHECK_AND_RET_TREEERR(NeedStaples(node->left, &left_staples)); \
    if (left_staples) fprintf(file, "("); \
    CHECK_AND_RET_TREEERR(LatexDumpRecursive(file, node->left, mtk)); \
    if (left_staples) fprintf(file, ")"); \
    \
    fprintf(file, "%s", end); \
    return NO_MISTAKE_T; \
}

DEF_UN_OP(Sin, "\\sin(" , ")")
DEF_UN_OP(Cos, "\\cos(" , ")")
DEF_UN_OP(Ln,  "\\ln(" , ")")
DEF_UN_OP(Tg,  "\\tan(" , ")")
DEF_UN_OP(Ctg, "\\cot(" , ")")
DEF_UN_OP(Sh,  "\\sh(" , ")")
DEF_UN_OP(Ch,  "\\ch(" , ")")
DEF_UN_OP(Th,  "\\th(" , ")")
DEF_UN_OP(Cth, "\\cth(" , ")")

#include "../core/operator_func.h"

//-------------------------------------------------------------
//--------------------------------------------------------------
// Dump

TreeErr_t LatexDumpTaylor(FILE *file, Forest_t *forest_diff, Forest_t *forest){
    TreeErr_t err = NO_MISTAKE_T;
    DEBUG_TREE( err = ForestVerify(forest_diff);
                err = ForestVerify(forest);)
    if(err) return err;

    fprintf(file, "{\\large \\textbf{Taylor Series:}}\n");
    fprintf(file, "\\begin{dmath}\n");
    fprintf(file, "T(");
    CHECK_AND_RET_TREEERR(LatexDumpRecursive(file, forest_diff->head_arr[0]->root, forest_diff->mtk));
    fprintf(file, ") = ");
    for(size_t idx = 0; idx < forest->first_free_place; idx++){
        // tree_dump_func(forest->head_arr[idx]->root, "Dump Taylor %zu Taylor Tree", __FILE__, __func__, __LINE__, forest_diff->mtk, idx);
        if(forest->head_arr[idx]->root->type == CONST && forest->head_arr[idx]->root->data.const_value == 0){
            continue;
        }
        if(forest->head_arr[idx]->root->left &&  forest->head_arr[idx]->root->type == CONST && forest->head_arr[idx]->root->left->data.const_value >= 0){
            fprintf(file, " + ");
        }
        CHECK_AND_RET_TREEERR(LatexDumpRecursive(file, forest->head_arr[idx]->root, forest_diff->mtk));
    }
    fprintf(file, "...");
    fprintf(file, "\\end{dmath}\n");

    DEBUG_TREE( err = ForestVerify(forest_diff);
                err = ForestVerify(forest);)
    return err;
}

TreeErr_t LatexDump(FILE* file, TreeNode_t* node, TreeNode_t* result, metki* mtk, const char* comment){
    if(comment) fprintf(file, "%s" ,comment);
    fprintf(file, "\\begin{dmath}\n");

    if(result) fprintf(file, "(\n");
    CHECK_AND_RET_TREEERR(LatexDumpRecursive(file, node, mtk));
    if(result) fprintf(file, ")' = \n");

    if(result) CHECK_AND_RET_TREEERR(LatexDumpRecursive(file, result, mtk));
    fprintf(file, "\\end{dmath}\n");

    return NO_MISTAKE_T;
}

static TreeErr_t LatexDumpRecursive(FILE* file, TreeNode_t* node, metki* mtk){
    size_t arr_num_of_elem = sizeof(OPERATORS_INFO) / sizeof(op_info);
    switch(node->type){
        case CONST:
            fprintf(file, "%lg" ,node->data.const_value);
            break;
        case VARIABLE:
            // not vilezli
            fprintf(file, "%s" , mtk->var_info[node->data.var_code].variable_name);
            break;
        case OPERATOR:
            if(node->data.op >= arr_num_of_elem){
                return INCORR_OPERATOR;
            }
            if(OPERATORS_INFO[node->data.op].function_dump == NULL){
                return NULL_PTR_TO_FUNC;
            }
            CHECK_AND_RET_TREEERR(OPERATORS_INFO[node->data.op].function_dump(file, node, mtk));
            break;
        default: return INCORR_TYPE;
    }
    return NO_MISTAKE_T;
}

static TreeErr_t NeedStaples(TreeNode_t* node, bool* need_staples){
    assert(need_staples);
    if(!node || node->type != OPERATOR || !node->parent || node->parent->type != OPERATOR){
        *need_staples = false;
        return NO_MISTAKE_T;
    }

    size_t arr_num_of_elem = sizeof(OPERATORS_INFO) / sizeof(op_info);
    if(node->data.op >= arr_num_of_elem || node->parent->data.op >= arr_num_of_elem || node->data.op == INCORR || node->parent->data.op == INCORR){
        return INCORR_OPERATOR;
    }
    int node_priority = OPERATORS_INFO[node->data.op].priority;
    int node_parent_priority = OPERATORS_INFO[node->parent->data.op].priority;

    if(node_priority < node_parent_priority){
        *need_staples = true;
        return NO_MISTAKE_T;
    }
    if(node->parent->data.op == OP_SUB){
        *need_staples = true;
        return NO_MISTAKE_T;
    }
    *need_staples = false;
    return NO_MISTAKE_T;
}


// в один принтф
// писать без каычек на каждой строчке
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

void GeneratePdfFromTex(const char* latex_file){
    assert(latex_file);
    // cd output && pdflatex 
    char cmd_buffer[MAX_CMD_BUFFER] = "";
    size_t len = strlen(latex_file);
    size_t folder = strcspn(latex_file, "/");
    if(folder != len){
        strncat(cmd_buffer, "cd ", sizeof("cd "));
        strncat(cmd_buffer, latex_file, folder);
        strncat(cmd_buffer, " && ", sizeof(" && "));
    }
    strncat(cmd_buffer, "pdflatex ", sizeof("pdflatex "));
    if(folder != len){
        strncat(cmd_buffer, latex_file + (folder + 1), len - (folder + 1));
    }
    else{
        strncat(cmd_buffer, latex_file, len);
    }
    system(cmd_buffer);
    printf("%s\n", cmd_buffer);
}

#undef DEF_BIN_OP
#undef DEF_UN_OP
#undef LATEX_DUMP_CPP