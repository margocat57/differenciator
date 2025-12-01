#ifndef OUTPUT_EXPR_TO_FILE_H
#define OUTPUT_EXPR_TO_FILE_H
#include "../core/tree.h"
#include "../core/forest.h"
#include "../utils/mistakes.h"
#include <stdio.h>

TreeErr_t DumpToFile(FILE* file, TreeNode_t* node, metki* mtk);

TreeErr_t DumpToFileTaylor(FILE* file, Forest_t* forest, Forest_t* forest_diff);

#endif // OUTPUT_EXPR_TO_FILE_H