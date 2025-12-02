#ifndef GNUPLOT_DUMP_H
#define GNUPLOT_DUMP_H
#include "../core/forest.h"
#include "../utils/taylor_enum.h"

char* DrawGraph(Forest_t *forest, size_t idx1, size_t idx2, TreeErr_t *err, IS_TAYLOR is_taylor);

#endif //GNUPLOT_DUMP_H