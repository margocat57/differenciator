#ifndef INPUT_OUTPUT_H
#define INPUT_OUTPUT_H
#include "../core/tree.h"
#include "../core/forest.h"
#include "../utils/mistakes.h"
#include <stdio.h>

TreeErr_t DumpToFile(FILE* file, TreeNode_t* node, metki* mtk, const size_t var_id);

Forest_t* MakeDiffForest(const char *name_of_file);

#endif // INPUT_OUTPUT_H