#ifndef DIFFERENCIATOR_H
#define DIFFERENCIATOR_H
#include "../data_struct/tree.h"
#include "mistakes.h"
#include "../data_struct/forest.h"


void CreateDiffTree(const char* var_name, Forest_t* forest, FILE* latex_dump);

void CreateTaylorTree(Forest_t *forest_taylor, Forest_t* diff_forest, FILE* latex_dump);

#endif //DIFFERENCIATOR_H