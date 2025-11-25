#ifndef LATEX_DUMP_H
#define LATEX_DUMP_H
#include <stdio.h>
#include "../core/tree.h"
#include "../utils/mistakes.h"
#include "../core/forest.h"

FILE* StartLatexDump(const char* filename);

void LatexDumpTaylor(FILE *file, Forest_t *forest_diff, Forest_t *forest);

void LatexDump(FILE* file, TreeNode_t* node, TreeNode_t* result, metki* mtk, const char* comment);

void LatexDumpDecimals(FILE* latex_file);

void LatexDumpChapterDiff(FILE* latex_file);

void LatexDumpChapterTaylor(FILE* latex_file);

void EndLatexDump(FILE* latex_file);

void GeneratePdfFromTex(const char* latex_file);

#endif //LATEX_DUMP_H