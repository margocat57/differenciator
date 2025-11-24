#include "matan_book.h"
#include <stdio.h>


void MatanBook(Forest_t* forest_diff, Forest_t* forest_diff_taylor, Forest_t* forest_taylor, const char* book_file){
    FILE* latex_dump = StartLatexDump(book_file);
    LatexDumpDecimals(latex_dump);

    LatexDumpChapterDiff(latex_dump);
    CreateDiffForest(forest_diff, latex_dump);

    LatexDumpChapterTaylor(latex_dump);
    CreateTaylorForest(forest_taylor, forest_diff, latex_dump);
    LatexDumpTaylor(latex_dump,forest_diff_taylor, forest_taylor);

    EndLatexDump(latex_dump);
}