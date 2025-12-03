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

#define CHECK_AND_RET_TREEERR_MATAN_BOOK(bad_condition, file)\
    do{\
    TreeErr_t err_macro = (bad_condition);\
        if(err_macro){ \
            fprintf(stderr, "err = %llu, %s, %s, %d\n", err_macro, __FILE__, __func__, __LINE__); \
            fclose(file);     \
            return err_macro; \
        } \
    }while(0) \

TreeErr_t MatanBook(Forest_t* forest_diff, Forest_t* forest_taylor, const char* book_file);

#endif //MATAN_BOOK_H