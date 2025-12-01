#ifndef CALCUL_TREE_H
#define CALCUL_TREE_H
#include "../core/tree.h"
#include "../utils/mistakes.h"
#include "../core/forest.h"
#include "../utils/taylor_enum.h"

TreeErr_t CalcTreeExpression(TreeNode_t* node, metki* mtk, double* result, IS_TAYLOR is_taylor);

TreeErr_t TreeOptimize(TreeNode_t **node);

#endif //CALCUL_TREE_H