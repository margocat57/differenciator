#ifndef MATAN_BOOK_H
#define MATAN_BOOK_H

#include "../data_struct/tree.h"
#include "../input_and_output/input_output.h"
#include "../common/metki.h"
#include "../data_struct/tree_func.h"
#include "../dump/graphviz_dump.h"
#include "../dump/latex_dump.h"
#include "../common/differenciator.h"
#include "../data_struct/forest.h"

void MatanBook(Forest_t* forest, const char* book_fil);

#endif //MATAN_BOOK_H