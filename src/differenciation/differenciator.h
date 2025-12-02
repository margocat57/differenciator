#ifndef DIFFERENCIATOR_H
#define DIFFERENCIATOR_H
#include "../core/tree.h"
#include "../utils/mistakes.h"
#include "../core/forest.h"
#include "diff_params.h"

TreeErr_t CreateForestWithNDerivatives(Forest_t *forest, FILE *latex_dump);

TreeErr_t CreateForestWithTaylorDecompose(Forest_t *forest_taylor, FILE *latex_dump);

#endif // DIFFERENCIATOR_H