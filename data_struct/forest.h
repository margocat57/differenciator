#ifndef FOREST_H
#define FOREST_H
#include "tree.h"

struct Forest_t{
    TreeHead_t** head_arr; // array of top of tree
    metki* mtk;
    size_t num_of_trees;
    size_t first_free_place;
};

Forest_t* ForestCtor(size_t num_of_trees);

void ForestAddElem(TreeHead_t* head_add, Forest_t* forest);

void ForestRealloc(Forest_t* forest, size_t num_of_trees);

void ForestDtor(Forest_t* forest);

#endif //FOREST_H