#ifndef LATEX_DUMP_H
#define LATEX_DUMP_H
#include <stdio.h>
#include "../core/tree.h"
#include "../utils/mistakes.h"
#include "../core/forest.h"
#include "../io/output_expr_to_file.h"
#include <string.h>

FILE* StartMatanBook(const char* filename);

void NeedStaples(TreeNode_t* node, bool* need_staples, TreeErr_t *err);

void CreateLatexTaylorDecompose(Forest_t *forest, FILE *file, TreeErr_t *err);

void PutDerivativeToLatex(FILE* file, TreeNode_t* node, TreeNode_t* result, metki* mtk, const size_t var_id, TreeErr_t *err, const char* comment, ...)  __attribute__ ((format (printf, 7, 8)));

void CreateAndLatexGraphicsDerivatives(Forest_t *forest, FILE* latex_file, TreeErr_t *err);

void CreateAndLatexTaylorGraphics(Forest_t *forest, FILE* latex_file, TreeErr_t *err);

void InsertGraphToLatex(Forest_t *forest, size_t idx1, FILE* latex_file, TreeErr_t *err, bool is_taylor, size_t idx2 = 0);

void LatexCreateChapterDecimals(FILE* latex_file);

void LatexCreateChapterDiff(FILE* latex_file);

void LatexCreateChapterTaylor(FILE* latex_file);

void LatexCreateAfterWord(FILE* latex_file);

void EndMatanBook(FILE* latex_file);

void GeneratePdfFromTex(const char* latex_file);

#endif //LATEX_DUMP_H