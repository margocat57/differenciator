#include "metki.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

metki* metki_init(){
    metki* mtk = (metki*)calloc(sizeof(metki), 1);

    variables* metki_arr = (variables*)calloc(sizeof(variables), MAX_NUMBER_OF_METKI); 
    if(!metki_arr){
        fprintf(stderr, "Can't allocate memory for metki array");
        return mtk;
    }

    mtk->num_of_metki = MAX_NUMBER_OF_METKI;
    mtk->var_info = metki_arr;
    mtk->first_free = 0;

    return mtk;
}

size_t metki_add_name(metki* mtk, char num_of_variable){
    assert(mtk);
    assert(num_of_variable);

    mtk->var_info[mtk->first_free].variable_name = num_of_variable;
    mtk->first_free++;
    if(mtk->first_free > mtk->num_of_metki){
        metki_realloc(mtk, mtk->num_of_metki*2);
    }
    return mtk->first_free - 1;
}

void metki_realloc(metki* mtk, size_t num_of_elem){
    variables* metki_arr_copy = (variables*)realloc(mtk->var_info, num_of_elem * sizeof(variables));
    if(!metki_arr_copy){
        fprintf(stderr, "Can't alloc metki arr\n");
        return;
    }
    mtk->var_info = metki_arr_copy;
    mtk->num_of_metki = num_of_elem;
    for(size_t metka = mtk->first_free; metka < mtk->num_of_metki; metka++){
        mtk->var_info[metka].variable_name = NULL;
        mtk->var_info[metka].value = 0;
    }
}

size_t find_var_in_mtk_arr(metki* mtk, char num_of_variable){
    for(size_t metka = 0; metka < mtk->first_free; metka++){
        if(mtk->var_info[metka].variable_name == num_of_variable){
            return metka;
        }
    }
    return SIZE_MAX;
}

void metki_add_values(metki* mtk){
    double value = 0;
    for(size_t idx = 0; idx < mtk->first_free; idx++){
        printf("Input value for %c\n", mtk->var_info[idx].variable_name);
        scanf("%lf", &value);
        mtk->var_info[idx].value = value;
    }
}

void metki_del_values(metki* mtk){
    for(size_t idx = 0; idx < mtk->first_free; idx++){
        mtk->var_info[idx].value = 0;
    }
}


void metki_destroy(metki* mtk){
    if(mtk){
        if(mtk->var_info){
            free(mtk->var_info);
        }
        free(mtk);
    }
}