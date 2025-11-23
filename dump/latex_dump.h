#ifndef LATEX_DUMP_H
#define LATEX_DUMP_H
#include <stdio.h>
#include "../data_struct/tree.h"
#include "../common/mistakes.h"
#include "../data_struct/forest.h"

FILE* StartLatexDump(const char* filename);

void LatexDump(FILE* file, TreeNode_t* node, TreeNode_t* result, metki* mtk, const char* comment);

void LatexDumpDecimals(FILE* latex_file);

void LatexDumpChapterDiff(FILE* latex_file);

void EndLatexDump(FILE* latex_file);

#endif //LATEX_DUMP_H