#ifndef LATEX_DUMP_H
#define LATEX_DUMP_H
#include <stdio.h>
#include "../core/tree.h"
#include "../utils/mistakes.h"
#include "../core/forest.h"

FILE* StartLatexDump(const char* filename);

TreeErr_t NeedStaples(TreeNode_t* node, bool* need_staples);

TreeErr_t LatexDumpTaylor(FILE *file, Forest_t *forest_diff, Forest_t *forest);

TreeErr_t LatexDump(FILE* file, TreeNode_t* node, TreeNode_t* result, metki* mtk, const char* comment, const size_t var_id);

TreeErr_t DumpGraphLatex(TreeNode_t* node1, TreeNode_t* node2, metki* mtk, FILE* latex_file);

void LatexDumpDecimals(FILE* latex_file);

void LatexDumpChapterDiff(FILE* latex_file);

void LatexDumpChapterTaylor(FILE* latex_file);

void EndLatexDump(FILE* latex_file);

void GeneratePdfFromTex(const char* latex_file);

#endif //LATEX_DUMP_H