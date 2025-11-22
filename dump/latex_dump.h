#ifndef LATEX_DUMP_H
#define LATEX_DUMP_H
#include <stdio.h>
#include "../data_struct/tree.h"
#include "../common/mistakes.h"
#include "../data_struct/forest.h"

void LatexDump(FILE* file, TreeNode_t* node, TreeNode_t* result, metki* mtk, const char* comment);

#endif //LATEX_DUMP_H