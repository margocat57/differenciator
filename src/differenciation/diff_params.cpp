#include <stdio.h>
#include <assert.h>
#include "diff_params.h"


#define CALL_FUNC_AND_CHECK_ERR(function)\
    do{\
        function;\
        if(*err){ \
            fprintf(stderr, "err = %llu, %s, %s, %d\n", *err, __FILE__, __func__, __LINE__); \
            return;                                                         \
        } \
    }while(0)

static void AskAboutN(size_t *n, TreeErr_t *err);

static size_t FindVarCodeToDiff(metki *mtk);

static size_t FindVarByName(char var_name, metki* mtk);

void CreateDiffParams(Forest_t* forest, TreeErr_t *err){
    DEBUG_TREE(CALL_FUNC_AND_CHECK_ERR( *err = ForestVerify(forest));)

    if(!(forest->params.is_num_derivative_filled)){
        CALL_FUNC_AND_CHECK_ERR(AskAboutN(&(forest->params.num_of_derivative), err));
    }

    if(!(forest->params.is_var_id_filled)){
        forest->params.var_id = FindVarCodeToDiff(forest->mtk);
        if(forest->params.var_id == SIZE_MAX){
            fprintf(stderr, "Incorr variable to differenciate");
            forest->params.num_of_derivative = 0;
            *err = INCORR_VAR_TO_DIFF;
            return;
        }
    }

    DEBUG_TREE(CALL_FUNC_AND_CHECK_ERR(*err = ForestVerify(forest));)
}

static void AskAboutN(size_t *n, TreeErr_t *err){
    assert(n);
    printf("Which derivative do you want to calculate?\n");
    if(scanf("%zu", n) != 1){
        *err = INCORR_USER_INPUT_VALUE;
    }
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