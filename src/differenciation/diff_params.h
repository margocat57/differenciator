#ifndef DIFF_PARAMS_H
#define DIFF_PARAMS_H
#include <stdlib.h>
#include "../utils/mistakes.h"
#include "../utils/metki.h"
#include "../core/tree.h"
#include "../utils/taylor_enum.h"

struct diff_params{
    size_t num_of_derivative;
    size_t var_id;
    double var_to_count_diff;
};

TreeErr_t CreateDiffParams(diff_params *params, metki* mtk, IS_TAYLOR is_taylor);

#endif //DIFF_PARAMS_H 