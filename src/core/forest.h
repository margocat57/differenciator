#ifndef FOREST_H
#define FOREST_H
#include "tree.h"
#include "../utils/mistakes.h"

struct Forest_t{
    TreeHead_t** head_arr; // array of top of tree
    metki* mtk;
    size_t num_of_trees;
    size_t first_free_place;
};

Forest_t* ForestCtor(size_t num_of_trees);

TreeErr_t ForestAddElem(TreeHead_t *head_add, Forest_t *forest);

TreeErr_t ForestRealloc(Forest_t *forest, size_t num_of_trees);

TreeErr_t ForestVerify(Forest_t *forest);

void ForestDtor(Forest_t *forest);

#endif //FOREST_H