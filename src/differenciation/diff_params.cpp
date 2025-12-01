#include <stdio.h>
#include <assert.h>
#include "diff_params.h"

static TreeErr_t AskAboutN(size_t *n);

static size_t FindVarCodeToDiff(metki *mtk);

static size_t FindVarByName(char var_name, metki* mtk);

TreeErr_t CreateDiffParams(diff_params *params, metki* mtk, IS_TAYLOR is_taylor){
    assert(params);

    CHECK_AND_RET_TREEERR(AskAboutN(&(params->num_of_derivative)));

    params->var_id = FindVarCodeToDiff(mtk);
    if(params->var_id == SIZE_MAX){
        fprintf(stderr, "Incorr variable to differenciate");
        params->num_of_derivative = 0;
        return INCORR_VAR_TO_DIFF;
    }

    if(is_taylor){
        metki_add_values(mtk);
    }

    return NO_MISTAKE_T;
}

static TreeErr_t AskAboutN(size_t *n){
    assert(n);
    printf("Which derivative do you want to calculate?\n");
    if(scanf("%zu", n) != 1){
        return INCORR_USER_INPUT_VALUE;
    }
    return NO_MISTAKE_T;
}

static size_t FindVarByName(char var_name, metki *mtk){
    assert(var_name);
    for(size_t metka_idx = 0; metka_idx < mtk->first_free; metka_idx++){
        if(mtk->var_info[metka_idx].variable_name == var_name){
            return metka_idx;
        }
    }
    return SIZE_MAX;
}

static size_t FindVarCodeToDiff(metki* mtk){
    char var_name = '0';
    printf("For which variable find the derivative\n");
    scanf(" %c", &var_name);
    return FindVarByName(var_name, mtk);
}