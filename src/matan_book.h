#ifndef MATAN_BOOK_H
#define MATAN_BOOK_H

#include "core/tree.h"
#include "io/read_expr_from_file.h"
#include "utils/metki.h"
#include "core/tree_func.h"
#include "dump/graphviz_dump.h"
#include "dump/latex_dump.h"
#include "differenciation/differenciator.h"
#include "core/forest.h"
#include "differenciation/diff_params.h"
#include "utils/taylor_enum.h"

TreeErr_t MatanBook(Forest_t* forest_diff, Forest_t* forest_diff_taylor, Forest_t* forest_taylor, const char* book_file);

#endif //MATAN_BOOK_H