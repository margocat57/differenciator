#ifndef DIFFERENCIATOR_H
#define DIFFERENCIATOR_H
#include "../core/tree.h"
#include "../utils/mistakes.h"
#include "../core/forest.h"
#include "diff_params.h"

void CreateForestWithNDerivatives(Forest_t *forest, FILE *latex_dump, TreeErr_t* err);

void CreateForestWithTaylorDecompose(Forest_t *forest_taylor, FILE *latex_dump, TreeErr_t* err);

#endif // DIFFERENCIATOR_H