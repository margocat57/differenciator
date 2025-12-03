#ifndef GNUPLOT_DUMP_H
#define GNUPLOT_DUMP_H
#include "../core/forest.h"

char* DrawGraph(Forest_t *forest, size_t idx1, TreeErr_t *err, bool is_taylor, size_t idx2);

#endif //GNUPLOT_DUMP_H