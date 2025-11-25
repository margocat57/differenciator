#include "src/matan_book.h"

int main(){
    Forest_t* forest_diff = MakeDiffForest("tests/btest.txt");
    Forest_t* forest_for_taylor = MakeDiffForest("tests/ctest.txt");
    Forest_t* forest_with_taylor = ForestCtor(10);
    MatanBook(forest_diff, forest_for_taylor, forest_with_taylor, "output/diff.tex");
    ForestDtor(forest_diff);
    ForestDtor(forest_for_taylor);
    ForestDtor(forest_with_taylor);
}
