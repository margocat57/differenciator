#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include "../differenciation/differenciator.h"
#include "graphviz_dump.h"
#include "../core/tree_func.h"
#include "../core/forest.h"
#include "../core/operator_func.h"
#include "gnuplot_graph.h"
#include "latex_dump.h"

const size_t MAX_CMD_BUFFER = 2048;

// Need to declare for dsl
static TreeErr_t LatexPutInfoRecursive(FILE* file, TreeNode_t* node, metki* mtk);

//------------------------------------
// Dumping operators

static TreeErr_t DumpSubtree(FILE* file, TreeNode_t* node, metki* mtk);

static TreeErr_t OperatorPutLatex(FILE* file, TreeNode_t* node, metki* mtk){
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
    CHECK_AND_RET_TREEERR(LatexPutInfoRecursive(file, node, mtk)); 
    if (staples) fprintf(file, ")"); 
    return NO_MISTAKE_T;
}

//-------------------------------------------------------------
//--------------------------------------------------------------
// Dump

TreeErr_t CreateLatexTaylorDecompose(Forest_t *forest, FILE *file){
    TreeErr_t err = NO_MISTAKE_T;
    DEBUG_TREE( err = ForestVerify(forest);)
    if(err) return err;

    fprintf(file, "{\\large \\textbf{Taylor Series:}}\n\n");
    fprintf(file, "\\begin{dmath}\n");
    fprintf(file, "T( ");

    CHECK_AND_RET_TREEERR(LatexPutInfoRecursive(file, forest->head_arr[0]->root, forest->mtk));
    fprintf(file, ") = ");
    CHECK_AND_RET_TREEERR(LatexPutInfoRecursive(file, forest->head_arr[forest->first_free_place - 1]->root, forest->mtk));
    fprintf(file, "...");
    fprintf(file, "\\end{dmath}\n");

    DEBUG_TREE( err = ForestVerify(forest);)
    return err;
}

TreeErr_t PutDerivativeToLatex(FILE* file, TreeNode_t* node, TreeNode_t* result, metki* mtk, const size_t var_id, const char* comment, ...){
    if(comment){
        va_list args = {};
        va_start(args, comment);
        vfprintf(file, comment, args);
        va_end(args);
    }
    fprintf(file, "\\begin{dmath}\n");

    if(result) fprintf(file, "\\frac{df}{d%c}( \n", mtk->var_info[var_id].variable_name);
    CHECK_AND_RET_TREEERR(LatexPutInfoRecursive(file, node, mtk));
    if(result) fprintf(file, " ) = ");

    if(result) CHECK_AND_RET_TREEERR(LatexPutInfoRecursive(file, result, mtk));
    fprintf(file, "\\end{dmath}\n");

    return NO_MISTAKE_T;
}

static TreeErr_t LatexPutInfoRecursive(FILE* file, TreeNode_t* node, metki* mtk){
    switch(node->type){
        case INCORR_VAL: return INCORR_TYPE;
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
            CHECK_AND_RET_TREEERR(OperatorPutLatex(file, node, mtk));
            break;
        default: return INCORR_TYPE;
    }
    return NO_MISTAKE_T;
}

TreeErr_t NeedStaples(TreeNode_t* node, bool* need_staples){
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

    if(node_priority < node_parent_priority && !OPERATORS_INFO[node->parent->data.op].is_unary_op){
        *need_staples = true;
        return NO_MISTAKE_T;
    }
    if(node->parent->data.op == OP_SUB){ // 1 - (a + b) and 1 - a + b 
        *need_staples = true;
        return NO_MISTAKE_T;
    }
    if(node->parent->data.op == OP_DEG){ //для случая 1 / (x^2^2^2)
        *need_staples = true;
        return NO_MISTAKE_T;
    }
    *need_staples = false;
    return NO_MISTAKE_T;
}

TreeErr_t CreateAndLatexGraphicsDerivatives(Forest_t *forest, FILE* latex_file){
    fprintf(latex_file, 
    "\\newpage\n"
    "\\section{Graphics of derivatives}\n\n");
    if(forest->first_free_place != 0){
        fprintf(latex_file, "{\\large \\textbf{Graphic of function}}\n\n");
        CHECK_AND_RET_TREEERR(InsertGraphToLatex(forest, 0, latex_file, /*is_taylor*/ false));
    }

    for(size_t idx = 1; idx < forest->first_free_place; idx++){
        fprintf(latex_file, "{\\large \\textbf{Graphic of %zu derivative}}\n\n", idx);
        CHECK_AND_RET_TREEERR(InsertGraphToLatex(forest, idx, latex_file, /*is_taylor*/ false));
        tree_dump_func(forest->head_arr[idx]->root, __FILE__, __func__, __LINE__, forest->mtk, "%zust derivative",idx);
    }

    return NO_MISTAKE_T;
}

TreeErr_t CreateAndLatexTaylorGraphics(Forest_t *forest, FILE* latex_file){
    fprintf(latex_file, 
    "\\newpage\n"
    "\\section{Taylor graphics}\n\n");
    CHECK_AND_RET_TREEERR(InsertGraphToLatex(forest, 0, latex_file, /*is_taylor*/ true, forest->first_free_place - 1));
    return NO_MISTAKE_T;
}

TreeErr_t InsertGraphToLatex(Forest_t *forest, size_t idx1, FILE* latex_file, bool is_taylor, size_t idx2){
    TreeErr_t err = NO_MISTAKE_T;
    char* dump_picture = DrawGraph(forest, idx1, &err, is_taylor, idx2);
    if(err){
        free(dump_picture);
        return err;
    }
    fprintf(latex_file, 
    "\\begin{figure}[H]\n"
    "\\includesvg[width=0.6\\textwidth,height=0.6\\textheight]{%s}\n" 
    "\\end{figure}\n\n", dump_picture + sizeof("output/") - 1);
    free(dump_picture);
    return err;
}

//---------------------------------------------------------------
// Dumping chapters

FILE* StartMatanBook(const char* filename){
    assert(filename);
    FILE* latex_file = fopen(filename, "w");
    if(!latex_file){
        return NULL;
    }
    fprintf(latex_file,  
R"(\documentclass[a4paper,12pt]{report}
\usepackage[utf8]{inputenc}
\usepackage{amsmath,amssymb}
\usepackage{geometry}
\usepackage[inkscapepath=/Applications/Inkscape.app/Contents/MacOS/]{svg}
\usepackage{breqn}
\usepackage{float}
\usepackage{svg}
\usepackage{graphicx} 
\usepackage{hyperref}
\usepackage{bookmark}

\newtheorem{definition}{Definition}
\newtheorem{obviousfact}{Obvious Fact}

\title{MatematiCAL anal for economists}
\author{Anonymus fan of mat.anal}

\begin{document}
\maketitle

\chapter*{Preface}
This textbook is designed to assist economics students studying the basic course of mathematical analysis. 
It summarizes the entire mathematical analysis course taught to economists in the best undergraduate economics program in Eastern Europe.

The lectures include only the essential material, ensuring that students who have achieved top honors in national economics Olympiads are not overburdened and can maintain their sense of superiority over the rest of the world. 
After all, they likely mastered all this material in kindergarten (or at the latest, by first grade). The division of topics into lectures corresponds well to the actual pace of the course, which spans an entire semester. 
Almost all statements in the course are self-evident, and their proofs are left to the reader as straightforward exercises. 

\tableofcontents

    )");
    return latex_file;
}

void LatexCreateChapterDecimals(FILE* latex_file){
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
A number is called \textit{rational} if it can be represented as 
something above a line and something below a line.
\end{definition}
    
\begin{definition}
A number is called \textit{irrational} if it is not rational.
\end{definition}
    
\begin{obviousfact}
The sum of all natural numbers equals $-1/12$.
\end{obviousfact}

\textbf{Kindergarten Example:}
If Vasya had 2 apples and Petya took 1 apple from him, how many apples does Vasya have left? The answer is obviously $-1/12$, as any advanced mathematician knows.
    )");
}

void LatexCreateChapterDiff(FILE* latex_file){
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

void LatexCreateChapterTaylor(FILE* latex_file){
    assert(latex_file);
    fprintf(latex_file, 
R"(\chapter{Taylor}

\section{Taylor's formula with the remainder term (and why is it needed? Without it, everything is obvious)}

\begin{definition}
Taylor's formula is obvious, so no additional explanations will be given. Let's start straight with an example.
\end{definition}

{\large \textbf{At first the derivatives must be calcutated:}}
    )");
}

void LatexCreateAfterWord(FILE* latex_file){
    fprintf(latex_file,
R"(\chapter*{Afterword}

Dear readers, I hope you have been able to spare a moment of your attention for this textbook and to realize its incredible obviousness. You will now excel in your exam, and if not, good luck next year.

The author also expresses great gratitude for the help in preparing this textbook to the students and professors of MIPT, namely to DED, mentor Kolya, and co-mentor Artyom, for actively seeking out the cringe in the code, which undoubtedly improved the quality of the materials. For this important work, the author wholeheartedly thanks all the assistants.

\section*{Bibliography:}

- Textbooks by G.I. Arkhipov, V.A. Sadovnichy, and V.N. Chubarikov

- Textbook by J. Stewart

- Textbook by an unknown author, "The Obviousness of Math"

- Lectures by A.L. Lukashov on Bipkas

- Lectures by D.A. Dagaev on the poetry of the Mechanics and Mathematics Faculty
)");
}

//--------------------------------------------------------------

void EndMatanBook(FILE* latex_file){
    assert(latex_file);
    fprintf(latex_file, "\\end{document}\n");
    fclose(latex_file);
}

void GeneratePdfFromTex(const char* latex_file){
    assert(latex_file);
    char cmd_buffer[MAX_CMD_BUFFER] = {};
    snprintf(cmd_buffer, MAX_CMD_BUFFER, "cd output && pdflatex -shell-escape %s && pdflatex -shell-escape %s", latex_file, latex_file);
    system(cmd_buffer);
}
