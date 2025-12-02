#ifndef DIFF_PARAMS_H
#define DIFF_PARAMS_H
#include <stdlib.h>
#include "../utils/mistakes.h"
#include "../utils/metki.h"
#include "../core/tree.h"
#include "../utils/taylor_enum.h"
#include "../core/forest.h"

TreeErr_t CreateDiffParams(Forest_t* forest, IS_TAYLOR is_taylor);

#endif //DIFF_PARAMS_H 