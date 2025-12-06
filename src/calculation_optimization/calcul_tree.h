#ifndef CALCUL_TREE_H
#define CALCUL_TREE_H
#include "../core/tree.h"
#include "../utils/mistakes.h"
#include "../core/forest.h"

void CalcTreeExpression(TreeNode_t* node, metki* mtk, double* result, bool is_taylor, TreeErr_t* err);

void TreeOptimize(TreeNode_t **node, TreeErr_t* err);

#endif //CALCUL_TREE_H