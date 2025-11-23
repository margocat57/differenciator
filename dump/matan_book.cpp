#include "matan_book.h"
#include <stdio.h>


void MatanBook(Forest_t* forest, const char* book_file){
    FILE* latex_dump = StartLatexDump(book_file);
    LatexDumpDecimals(latex_dump);
    LatexDumpChapterDiff(latex_dump);
    CreateDiffTree("x", forest, latex_dump);
    EndLatexDump(latex_dump);
}