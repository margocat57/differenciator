#ifndef CALCUL_TREE_H
#define CALCUL_TREE_H
#include "../core/tree.h"
#include "../utils/mistakes.h"
#include "../core/forest.h"

TreeErr_t CalcTreeExpression(Forest_t* forest, size_t num_of_tree, double* result, bool is_taylor);

TreeErr_t TreeOptimize(TreeNode_t **node);

#endif //CALCUL_TREE_H