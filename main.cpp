#include "dump/matan_book.h"

int main(){
    Forest_t* forest = MakeDiffForest("atest.txt");;
    MatanBook(forest, "diff.tex");
    ForestDtor(forest);
}

// проблемы
// отдельные массивы для каждого файла - в одном указатели на функции, в другом const char* - как то сложно и добавлять новую команду сложно