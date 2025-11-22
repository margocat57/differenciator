#ifndef CALCUL_TREE_H
#define CALCUL_TREE_H
#include "../data_struct/tree.h"
#include "mistakes.h"
#include "../data_struct/forest.h"

TreeErr_t CalcTreeExpression(Forest_t* forest, size_t num_of_tree, double* result);

#endif //CALCUL_TREE_H