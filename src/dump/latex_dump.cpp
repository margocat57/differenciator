#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "../differenciation/differenciator.h"
#include "graphviz_dump.h"
#include "../core/tree_func.h"
#include "../core/forest.h"
#include "../core/operator_func.h"

const size_t MAX_CMD_BUFFER = 2048;

static TreeErr_t NeedStaples(TreeNode_t* node, bool* need_staples);

// Need to declare for dsl
static TreeErr_t LatexDumpRecursive(FILE* file, TreeNode_t* node, metki* mtk);

//------------------------------------
// Dumping operators

static TreeErr_t DumpSubtree(FILE* file, TreeNode_t* node, metki* mtk);

static TreeErr_t OperatorDump(FILE* file, TreeNode_t* node, metki* mtk){
    assert(file); assert(node); 
    size_t arr_num_of_elem = sizeof(OPERATORS_INFO) / sizeof(op_info);
    if(node->data.op >= arr_num_of_elem){
        return INCORR_OPERATOR;
    }

    fprintf(file, "%s", OPERATORS_INFO[node->data.op].dump_start); 
    
    CHECK_AND_RET_TREEERR(DumpSubtree(file, node->left, mtk));
    if(OPERATORS_INFO[node->data.op].dump_cont){
        fprintf(file, "%s", OPERATORS_INFO[node->data.op].dump_cont); 
    }

    if(node->right){
        CHECK_AND_RET_TREEERR(DumpSubtree(file, node->right, mtk));
    }

    fprintf(file, "%s", OPERATORS_INFO[node->data.op].dump_end); 
    return NO_MISTAKE_T; 
}

static TreeErr_t DumpSubtree(FILE* file, TreeNode_t* node, metki* mtk){
    bool staples = false; 
    CHECK_AND_RET_TREEERR(NeedStaples(node, &staples)); 
    if (staples) fprintf(file, "("); 
    CHECK_AND_RET_TREEERR(LatexDumpRecursive(file, node, mtk)); 
    if (staples) fprintf(file, ")"); 
    return NO_MISTAKE_T;
}

//-------------------------------------------------------------
//--------------------------------------------------------------
// Dump

TreeErr_t LatexDumpTaylor(FILE *file, Forest_t *forest_diff, Forest_t *forest){
    TreeErr_t err = NO_MISTAKE_T;
    DEBUG_TREE( err = ForestVerify(forest_diff);
                err = ForestVerify(forest);)
    if(err) return err;

    fprintf(file, "{\\large \\textbf{Taylor Series:}}\n\n");
    fprintf(file, "\\begin{dmath}\n");
    fprintf(file, "T( ");

    CHECK_AND_RET_TREEERR(LatexDumpRecursive(file, forest_diff->head_arr[0]->root, forest_diff->mtk));
    fprintf(file, ") = ");
    for(size_t idx = 0; idx < forest->first_free_place; idx++){
    tree_dump_func(forest->head_arr[idx]->root, "f", __FILE__, __func__, __LINE__,  forest_diff->mtk);
        if(forest->head_arr[idx]->root->type == CONST && forest->head_arr[idx]->root->data.const_value == 0){
            continue;
        }
        if(forest->head_arr[idx]->root->left && forest->head_arr[idx]->root->left->type == CONST && forest->head_arr[idx]->root->left->data.const_value >= 0){
            fprintf(file, " + ");
        }
        if(forest->head_arr[idx]->root->left && forest->head_arr[idx]->root->left->type == VARIABLE){
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

TreeErr_t LatexDump(FILE* file, TreeNode_t* node, TreeNode_t* result, metki* mtk, const char* comment, const size_t var_id){
    if(comment) fprintf(file, "%s" ,comment);
    fprintf(file, "\\begin{dmath}\n");

    if(result) fprintf(file, "\\frac{df}{d%c}( \n", mtk->var_info[var_id].variable_name);
    CHECK_AND_RET_TREEERR(LatexDumpRecursive(file, node, mtk));
    if(result) fprintf(file, " ) = ");

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
            if(node->data.var_code >= mtk->num_of_metki){
                return INCORR_IDX_IN_MTK;
            }
            fprintf(file, "%c" , mtk->var_info[node->data.var_code].variable_name);
            break;
        case OPERATOR:
            CHECK_AND_RET_TREEERR(OperatorDump(file, node, mtk));
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
    if(node->parent->data.op == OP_DEG){
        *need_staples = true;
        return NO_MISTAKE_T;
    }
    *need_staples = false;
    return NO_MISTAKE_T;
}

//---------------------------------------------------------------
// Dumping chapters

FILE* StartLatexDump(const char* filename){
    assert(filename);
    FILE* latex_file = fopen(filename, "w");
    fprintf(latex_file,  
R"(\documentclass[a4paper,12pt]{report}
\usepackage[utf8]{inputenc}
\usepackage{amsmath,amssymb}
\usepackage{geometry}
\usepackage{breqn}
\usepackage{hyperref}
\usepackage{bookmark}

\newtheorem{definition}{Definition}
\newtheorem{obviousfact}{Obvious Fact}

\title{MatematiCAL anal for economists}
\author{Anonymus fan of mat.anal}

\begin{document}
\maketitle

\tableofcontents

\chapter*{Preface}
This textbook is designed to assist economics students studying the basic course of mathematical analysis. 
It summarizes the entire mathematical analysis course taught to economists in the best undergraduate economics program in Eastern Europe.

The lectures include only the essential material, ensuring that students who have achieved top honors in national economics Olympiads are not overburdened and can maintain their sense of superiority over the rest of the world. 
After all, they likely mastered all this material in kindergarten (or at the latest, by first grade). The division of topics into lectures corresponds well to the actual pace of the course, which spans an entire semester. 
Almost all statements in the course are self-evident, and their proofs are left to the reader as straightforward exercises. 
    )");
    return latex_file;
}

void LatexDumpDecimals(FILE* latex_file){
    assert(latex_file);
    fprintf(latex_file, 
R"(\chapter{Numbers}
\section{Basic Classes of Numbers}
    
First, let's introduce the definitions of the basic classes of numbers that we will constantly work with throughout the course.
    
\begin{definition}
Numbers 1, 2, 3, \ldots are called \textit{natural numbers}. 
The notation for the set of all natural numbers is $\mathbb{N}$.
\end{definition}
    
\begin{definition}
A number is called an \textit{integer} if it is equal to\ldots 
but you don't need this because everything in economics is positive.
\end{definition}
    
\begin{definition}
A number is called \\textit{rational} if it can be represented as 
something above a line and something below a line.
\end{definition}
    
\begin{definition}
A number is called \\textit{irrational} if it is not rational.
\end{definition}
    
\begin{obviousfact}
The sum of all natural numbers equals $-1/12$.
\end{obviousfact}

\textbf{Kindergarten Example:}
If Vasya had 2 apples and Petya took 1 apple from him, how many apples does Vasya have left? The answer is obviously $-1/12$, as any advanced mathematician knows.
    )");
}

void LatexDumpChapterDiff(FILE* latex_file){
    assert(latex_file);
    fprintf(latex_file, 
R"(\chapter{Derivative}

\section{Basic derivatives}

\begin{definition}
The definition of derivative is omitted because it is obvious.
\end{definition}

Everything in this chapter is so obvious that no additional explanations will be provided - we'll immediately proceed to analyze an example from kindergarten.
    )");
}

void LatexDumpChapterTaylor(FILE* latex_file){
    assert(latex_file);
    fprintf(latex_file, 
R"(\chapter{Taylor}

\section{Taylor's formula with the remainder term (and why is it needed? Without it, everything is obvious)}

\begin{definition}
Taylor's formula is obvious, so no additional explanations will be given. Let's start straight with an example.
\end{definition}
    )");
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
    for(int i = 0; i < 2; i++){
        strncat(cmd_buffer, "pdflatex ", sizeof("pdflatex "));
        if(folder != len){
            strncat(cmd_buffer, latex_file + (folder + 1), len - (folder + 1));
        }
        else{
            strncat(cmd_buffer, latex_file, len);
        }
        if(i == 0){
            strncat(cmd_buffer, " && ", sizeof(" && "));
        }
    }
    system(cmd_buffer);
}
