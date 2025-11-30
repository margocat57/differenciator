#include "src/matan_book.h"


int main(){
    Forest_t* forest_diff = MakeDiffForest("tests/ftest.txt");
    Forest_t* forest_for_taylor = MakeDiffForest("tests/etest.txt");
    Forest_t* forest_with_taylor = ForestCtor(10);
    MatanBook(forest_diff, forest_for_taylor, forest_with_taylor, "diff.tex");
    ForestDtor(forest_diff);
    ForestDtor(forest_for_taylor);
    ForestDtor(forest_with_taylor);

    // system("gnuplot a.gp"); - работает

    return 0;
}

// графики - первая производная, вторая производная тейлор.
// и если некорректное считывание
