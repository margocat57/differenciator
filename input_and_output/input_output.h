#ifndef INPUT_OUTPUT_H
#define INPUT_OUTPUT_H
#include "../data_struct/tree.h"
#include "../data_struct/forest.h"
#include "../common/mistakes.h"

Forest_t* MakeDiffForest(const char *name_of_file);

// TreeErr_t PutDiffFile(const char* file_name, TreeNode_t *node, const TreeHead_t* head);

#endif // INPUT_OUTPUT_H