#include "data_struct/tree.h"
#include "input_and_output/input_output.h"
#include "common/metki.h"
#include "data_struct/tree_func.h"
#include "dump/graphviz_dump.h"
#include "common/differenciator.h"
#include "data_struct/forest.h"

int main(){
    Forest_t* forest = MakeDiffForest("atest.txt");
    CreateDiffTree("diff.tex", "x", forest);
    ForestDtor(forest);
}

// проблемы
// отдельные массивы для каждого файла - в одном указатели на функции, в другом const char* - как то сложно и добавлять новую команду сложно