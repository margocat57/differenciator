#ifndef READ_EXPR_FROM_FILE_H
#define READ_EXPR_FROM_FILE_H
#include "../core/tree.h"
#include "../core/forest.h"
#include "../utils/mistakes.h"
#include <stdio.h>

Forest_t* MakeDiffForest(const char *name_of_file);


#endif // READ_EXPR_FROM_FILE_H