#ifndef FOREST_H
#define FOREST_H
#include "tree.h"
#include "../utils/mistakes.h"

struct Diff_params{
    size_t num_of_derivative;
    bool is_num_derivative_filled;
    size_t var_id;
    bool is_var_id_filled;
};

struct Gnuplot_dump{
    double x_min_dump;
    double x_max_dump;
    double y_min_dump;
    double y_max_dump;
};

struct Forest_t{
    TreeHead_t** head_arr; // array of top of tree
    metki* mtk;
    size_t num_of_trees;
    size_t first_free_place;
    struct Diff_params params;
    struct Gnuplot_dump x_y_range;
};

Forest_t* ForestCtor(size_t num_of_trees);

TreeErr_t ForestAddElem(TreeHead_t *head_add, Forest_t *forest);

TreeErr_t ForestRealloc(Forest_t *forest, size_t num_of_trees);

TreeErr_t ForestVerify(Forest_t *forest);

void ForestDtor(Forest_t *forest);

#endif //FOREST_H