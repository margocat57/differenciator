#ifndef MATAN_BOOK_H
#define MATAN_BOOK_H

#include "core/tree.h"
#include "io/input_output.h"
#include "utils/metki.h"
#include "core/tree_func.h"
#include "dump/graphviz_dump.h"
#include "dump/latex_dump.h"
#include "differenciation/differenciator.h"
#include "core/forest.h"

TreeErr_t MatanBook(Forest_t* forest_diff, Forest_t* forest_diff_taylor, Forest_t* forest_taylor, const char* book_file);

#endif //MATAN_BOOK_H