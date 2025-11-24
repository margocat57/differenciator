#include "dump/matan_book.h"

int main(){
    Forest_t* forest_diff = MakeDiffForest("btest.txt");
    Forest_t* forest_for_taylor = MakeDiffForest("ctest.txt");
    Forest_t* forest_with_taylor = ForestCtor(10);
    MatanBook(forest_diff, forest_for_taylor, forest_with_taylor, "diff.tex");
    ForestDtor(forest_diff);
    ForestDtor(forest_for_taylor);
    ForestDtor(forest_with_taylor);
}

// проблемы
// отдельные массивы для каждого файла - в одном указатели на функции, в другом const char* - как то сложно и добавлять новую команду сложно