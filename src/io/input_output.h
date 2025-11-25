#ifndef INPUT_OUTPUT_H
#define INPUT_OUTPUT_H
#include "../core/tree.h"
#include "../core/forest.h"
#include "../utils/mistakes.h"

Forest_t* MakeDiffForest(const char *name_of_file);

// TreeErr_t PutDiffFile(const char* file_name, TreeNode_t *node, const TreeHead_t* head);

#endif // INPUT_OUTPUT_H