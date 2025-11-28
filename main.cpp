#include "src/matan_book.h"

/*
! Осторожно КРИНЖ
Короче у меня не запускается гнуплот но есть идея:
Юзать матпротлиб и нампи:
то есть мы дампим дерево в файл - далее systemом заставляем работать питоновский код
Получаем график вставляем его в латех 
*/

int main(){
    Forest_t* forest_diff = MakeDiffForest("tests/ftest.txt");
    Forest_t* forest_for_taylor = MakeDiffForest("tests/etest.txt");
    Forest_t* forest_with_taylor = ForestCtor(10);
    MatanBook(forest_diff, forest_for_taylor, forest_with_taylor, "output/diff.tex");
    ForestDtor(forest_diff);
    ForestDtor(forest_for_taylor);
    ForestDtor(forest_with_taylor);

    return 0;
}

// проблемы - дамп степени в тех
// не особо понимаю как ее нормально задампить, потому что вроде как скобочки есть 
// но иногда с ними кринж
// графики - первая производная, вторая производная тейлор.
// и если некорректное считывание
