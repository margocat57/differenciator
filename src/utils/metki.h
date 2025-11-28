#ifndef METKI_H
#define METKI_H
#include <stdlib.h>

const int MAX_NUMBER_OF_METKI = 20;

struct variables{
    char variable_name;
    double value;
};

struct metki{
    variables* var_info;
    size_t num_of_metki;
    size_t first_free;
};

metki* metki_init();

size_t metki_add_name(metki* mtk, char num_of_variable);

void metki_realloc(metki* mtk, size_t num_of_elem);

size_t find_var_in_mtk_arr(metki* mtk, char num_of_variable);

void metki_add_values(metki* mtk);

void metki_del_values(metki* mtk);

void metki_destroy(metki* mtk);

#endif //METKI_H