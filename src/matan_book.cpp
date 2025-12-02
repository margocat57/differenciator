#include "matan_book.h"
#include <stdio.h>

TreeErr_t MatanBook(Forest_t *forest_diff, Forest_t *forest_taylor, const char *book_file)
{
    if (!forest_diff || !forest_taylor)
    {
        fprintf(stderr, "Can't work with null forest ptr\n");
        return NULL_FOREST_PTR;
    }

    TreeErr_t err = NO_MISTAKE_T;
    DEBUG_TREE(err = ForestVerify(forest_diff);
               err = ForestVerify(forest_taylor);)
    if (err)
        return err;

    char buffer[300] = {};
    strncpy(buffer, "output/", sizeof("output/"));
    strncat(buffer, book_file, strlen(book_file));

    FILE *latex_dump = StartLatexDump(buffer);
    if (!latex_dump)
    {
        return INCORR_FILE;
    }
    LatexDumpDecimals(latex_dump);

    LatexDumpChapterDiff(latex_dump);
    CHECK_AND_RET_TREEERR(CreateDiffParams(forest_diff, NO))
    CHECK_AND_RET_TREEERR(CreateForestWithNDerivatives(forest_diff, latex_dump))

    LatexDumpChapterTaylor(latex_dump);
    CHECK_AND_RET_TREEERR(CreateDiffParams(forest_taylor, YES))
    CHECK_AND_RET_TREEERR(CreateForestWithTaylorDecompose(forest_taylor, latex_dump))
    CHECK_AND_RET_TREEERR(LatexDumpTaylor(latex_dump, forest_taylor))
    CHECK_AND_RET_TREEERR(DumpGraphLatex(forest_taylor, 0, forest_taylor->first_free_place - 1, latex_dump, YES))
    metki_del_values(forest_taylor->mtk);

    LatexDumpAfterWord(latex_dump);

    EndLatexDump(latex_dump);
    GeneratePdfFromTex(book_file);

    DEBUG_TREE(err = ForestVerify(forest_diff);
               err = ForestVerify(forest_taylor);)
    return err;
}