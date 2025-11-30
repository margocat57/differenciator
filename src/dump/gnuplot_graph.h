#ifndef GNUPLOT_DUMP_H
#define GNUPLOT_DUMP_H
#include "../core/forest.h"

char* DrawGraph(Forest_t *diff_forest, Forest_t *forest_taylor, size_t idx1, size_t idx2, TreeErr_t *err);

#endif //GNUPLOT_DUMP_H