#ifndef DIFFERENCIATOR_H
#define DIFFERENCIATOR_H
#include "../core/tree.h"
#include "../utils/mistakes.h"
#include "../core/forest.h"
#include "diff_params.h"

TreeErr_t CreateDiffForest(Forest_t *forest, FILE *latex_dump, diff_params *params);

TreeErr_t CreateTaylorForest(Forest_t *forest_taylor, Forest_t *diff_forest, FILE *latex_dump, diff_params *params);

#endif //DIFFERENCIATOR_H