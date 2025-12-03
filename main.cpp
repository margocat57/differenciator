#include "src/matan_book.h"

int main()
{
    Forest_t *forest_diff   = ReadAndCreateExpr("tests/btest.txt");
    Forest_t *forest_taylor = ReadAndCreateExpr("tests/etest.txt");
    MatanBook(forest_diff, forest_taylor, "diff.tex");
    ForestDtor(forest_diff);
    ForestDtor(forest_taylor);

    return 0;
}
