#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include "../differenciation/differenciator.h"
#include "graphviz_dump.h"
#include "../core/tree_func.h"
#include "../core/forest.h"
#include "../core/operator_func.h"
#include "../utils/check_sys.h"
#include "gnuplot_graph.h"
#include "latex_dump.h"

const char* const ALLOWED = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789&-_\t\n .";

#define CALL_FUNC_AND_CHECK_ERR(function)\
    do{\
        function;\
        if(*err){ \
            fprintf(stderr, "err = %llu, %s, %s, %d\n", *err, __FILE__, __func__, __LINE__); \
            return;                                                         \
        } \
    }while(0)

const size_t MAX_CMD_BUFFER = 2048;

//-------------------------------------------------------------
//--------------------------------------------------------------
// Dump

void CreateLatexTaylorDecompose(Forest_t *forest, FILE *file, TreeErr_t *err){
    if(*err) return;

    DEBUG_TREE(CALL_FUNC_AND_CHECK_ERR(*err = ForestVerify(forest);))

    fprintf(file, "{\\large \\textbf{Разложение Тейлора:}}\n\n");
    fprintf(file, "\\begin{dmath}\n");
    fprintf(file, "T( ");

    CALL_FUNC_AND_CHECK_ERR(PutInfoRecursive(file, forest->head_arr[0]->root, forest->mtk, err, DUMP_LATEX));

    fprintf(file, ") = ");

    CALL_FUNC_AND_CHECK_ERR(PutInfoRecursive(file, forest->head_arr[forest->first_free_place - 1]->root, forest->mtk, err, DUMP_LATEX));

    fprintf(file, "...");
    fprintf(file, "\\end{dmath}\n");

    DEBUG_TREE(CALL_FUNC_AND_CHECK_ERR(*err = ForestVerify(forest);))
}

void PutDerivativeToLatex(FILE* file, TreeNode_t* node, TreeNode_t* result, metki* mtk, const size_t var_id, TreeErr_t *err, const char* comment, ...){
    if(*err) return;

    DEBUG_TREE( CALL_FUNC_AND_CHECK_ERR(*err = TreeNodeVerify(node));
    if(result)  CALL_FUNC_AND_CHECK_ERR(*err = TreeNodeVerify(result)););

    if(comment){
        va_list args = {};
        va_start(args, comment);
        vfprintf(file, comment, args);
        va_end(args);
    }
    fprintf(file, "\\begin{dmath}\n");

    if(result) fprintf(file, "\\frac{df}{d%c}( \n", mtk->var_info[var_id].variable_name);

    CALL_FUNC_AND_CHECK_ERR(PutInfoRecursive(file, node, mtk, err, DUMP_LATEX)); 

    if(result) fprintf(file, " ) = ");

    if(result) CALL_FUNC_AND_CHECK_ERR(PutInfoRecursive(file, result, mtk, err, DUMP_LATEX));

    fprintf(file, "\\end{dmath}\n");

    DEBUG_TREE( CALL_FUNC_AND_CHECK_ERR(*err = TreeNodeVerify(node));
    if(result)  CALL_FUNC_AND_CHECK_ERR(*err = TreeNodeVerify(result)););
}


void CreateAndLatexGraphicsDerivatives(Forest_t *forest, FILE* latex_file, TreeErr_t *err){
    fprintf(latex_file, 
    "\\newpage\n"
    "\\section{Графики производных}\n\n");
    if(forest->first_free_place != 0){
        fprintf(latex_file, "{\\large \\textbf{График функции}}\n\n");
        CALL_FUNC_AND_CHECK_ERR(InsertGraphToLatex(forest, 0, latex_file, err, /*is_taylor*/ false));
    }

    for(size_t idx = 1; idx < forest->first_free_place; idx++){
        fprintf(latex_file, "{\\large \\textbf{График %zu производной}}\n\n", idx);
        CALL_FUNC_AND_CHECK_ERR(InsertGraphToLatex(forest, idx, latex_file, err, /*is_taylor*/ false));
    }
}

void CreateAndLatexTaylorGraphics(Forest_t *forest, FILE* latex_file, TreeErr_t *err){
    fprintf(latex_file, 
    "\\newpage\n"
    "\\section{График разложения Тейлора}\n\n");
    CALL_FUNC_AND_CHECK_ERR(InsertGraphToLatex(forest, 0, latex_file, err, /*is_taylor*/ true, forest->first_free_place - 1));
}

void InsertGraphToLatex(Forest_t *forest, size_t idx1, FILE* latex_file, TreeErr_t *err, bool is_taylor, size_t idx2){
    char* dump_picture = DrawGraph(forest, idx1, err, is_taylor, idx2);
    if(*err){
        free(dump_picture);
        return;
    }
    fprintf(latex_file, 
    "\\begin{figure}[H]\n"
    "\\includesvg[width=1.0\\textwidth,height=1.0\\textheight]{%s}\n" 
    "\\end{figure}\n\n", dump_picture + sizeof("output/") - 1);
    free(dump_picture);
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
\usepackage[russian]{babel} 
\usepackage{fontspec} 
\usepackage[inkscapepath=/Applications/Inkscape.app/Contents/MacOS/]{svg}
\usepackage{breqn}
\usepackage{float}
\usepackage{svg}
\usepackage{graphicx} 
\usepackage{hyperref}
\setmainfont{Times New Roman} 

\newtheorem{definition}{Определение}
\newtheorem{obviousfact}{Очевидный факт}

\title{Математический АНАЛиз для экономистов}
\author{Анонимный фанат матАНАЛа}

\begin{document}
\maketitle

\chapter*{Предисловие}
Данное пособие написано в помощь студентам-экономистам, изучающим базовый курс математического анализа. Оно обобщает весь курс математического анализа читаемого экономистам на лучшем бакалавриате по экономике в восточной Европе. 

Лекции включают в себя только необходимый материал, чтобы ребята, получившие несколько всероссов по экономике ни в коем случае не перетруждались и чтобы они чувствовали превосходство себя над остальным миром, ведь все это они успели заботать в детском саду( ну максимум в первом классе). Разбиение по лекциям в пособии достаточно хорошо соответствует реальной скорости чтения курса, который идет целый семестр. Почти все утверждения в курсе очевидны и их доказательство представляется читателю в качестве несложного упражнения.

\tableofcontents

    )");
    return latex_file;
}

void LatexCreateChapterDecimals(FILE* latex_file){
    assert(latex_file);
    fprintf(latex_file,     
R"(\chapter{Циферки}
\section{Основные классы циферок}
    
Cначала введем определения основных классов циферок, с которыми мы будем постоянно работать на курсе.
    
\begin{definition}
Натуральными называются циферки 1, 2, 3, . . . Обозначение для множества всех натуральных чисел: $\mathbb{N}$.
\end{definition}
    
\begin{definition}
Циферка называется целой, если оно равно … а вам это и не надо потому что все в экономике положительное.
\end{definition}
    
\begin{definition}
Циферка называется \textit{рациональным}, если оно может быть представлено в виде чего-то над палочкой и еще чего-то под палочкой. 
\end{definition}
    
\begin{definition}
Циферка называется \textit{иррациональным} если оно не является рациональным. 
\end{definition}
    
\begin{obviousfact}
Сумма всех натуральных циферок равна $-1/12$.
\end{obviousfact}

\textbf{Пример из детского сада:}
Если у Васи было 2 яблока, а Петя взял у него 1 яблоко, сколько яблок осталось у Васи? Ответ, очевидно, $-1/12$, как известно любому продвинутому математику.
    )");
}

void LatexCreateChapterDiff(FILE* latex_file){
    assert(latex_file);
    fprintf(latex_file, 
R"(\chapter{Производная}

\section{Basic derivatives}

\begin{definition}
Определение производной опущено, так как оно очевидно.
\end{definition}

Всё в этой главе настолько очевидно, что дополнительные объяснения не потребуются — мы сразу перейдём к разбору примера из детского сада.
    )");
}

void LatexCreateChapterTaylor(FILE* latex_file){
    assert(latex_file);
    fprintf(latex_file, 
R"(\chapter{Формула Тейлора}

\section{Формула Тейлора с остаточным членом (а зачем он нужен? Без него, все очевидно)}

\begin{definition}
Формула Тейлора очевидна, поэтому дополнительные объяснения не будут даны. Начнём сразу с примера.
\end{definition}

{\large \textbf{Сначала необходимо вычислить производные:}}
    )");
}

void LatexCreateAfterWord(FILE* latex_file){
    fprintf(latex_file,
R"(\chapter*{Послесловие}

Дорогие читатели, надеюсь вы смогли уделить минуточку внимания данному пособию и осознать его неимоверную очевидность. Теперь вы отлично сдадите экзамен, а если нет, то удачи в следующем году.

Также автор выражает большую благодарность в помощи с подготовкой данного пособия студентам и преподавателям Мфти, а именно DEDу, ментору Коле, соментору Артему за то, что вы активно искали кринж в коде, что несомненно улучшило качество материалов. За эту важную работу автор от всего сердца благодарит всех помощников. 

\section*{Список Литературы:}

- Учебники Г.И. Архипова, В.А. Садовничего и В.Н. Чубарикова 

- Учебник Дж. Стюарта 

- Учебник неизвестного автора «очевидность матана»

- Лекции А.Л.Лукашова о Бипках

- Лекции Д.А.Дагаева о поэзии мехмата
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
    snprintf(cmd_buffer, MAX_CMD_BUFFER, "cd output && xelatex -shell-escape %s && xelatex -shell-escape %s", latex_file, latex_file);

    if(!is_cmd_for_sys_correct(latex_file, ALLOWED)){
        return;
    }

    system(cmd_buffer);
}

//check_sys

#undef CALL_FUNC_AND_CHECK_ERR