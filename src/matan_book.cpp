#include "matan_book.h"
#include <stdio.h>


TreeErr_t MatanBook(Forest_t* forest_diff, Forest_t* forest_diff_taylor, Forest_t* forest_taylor, const char* book_file){
    TreeErr_t err = NO_MISTAKE_T;
    DEBUG_TREE( err = ForestVerify(forest_diff);
                err = ForestVerify(forest_diff_taylor);
                err = ForestVerify(forest_taylor);)
    if(err) return err;

    FILE* latex_dump = StartLatexDump(book_file);
    if(!latex_dump){
        return INCORR_FILE;
    }
    LatexDumpDecimals(latex_dump);

    LatexDumpChapterDiff(latex_dump);
    CHECK_AND_RET_TREEERR(CreateDiffForest(forest_diff, latex_dump))

    LatexDumpChapterTaylor(latex_dump);
    CHECK_AND_RET_TREEERR(CreateTaylorForest(forest_taylor, forest_diff_taylor, latex_dump))
    CHECK_AND_RET_TREEERR(LatexDumpTaylor(latex_dump, forest_diff_taylor, forest_taylor))

    EndLatexDump(latex_dump);
    GeneratePdfFromTex(book_file);

    DEBUG_TREE( err = ForestVerify(forest_diff);
                err = ForestVerify(forest_diff_taylor);
                err = ForestVerify(forest_taylor);)
    return err;
}