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
    if(err) return err;

    char buffer[300] = "output/";
    strncat(buffer, book_file, sizeof(buffer) - 1);

    FILE *latex_dump = StartMatanBook(buffer);
    if (!latex_dump){
        return INCORR_FILE;
    }
    LatexCreateChapterDecimals(latex_dump);

    LatexCreateChapterDiff(latex_dump); 
    CHECK_AND_RET_TREEERR_MATAN_BOOK(CreateDiffParams(forest_diff), latex_dump); 
    CHECK_AND_RET_TREEERR_MATAN_BOOK(CreateForestWithNDerivatives(forest_diff, latex_dump), latex_dump);
    CHECK_AND_RET_TREEERR_MATAN_BOOK(CreateAndLatexGraphicsDerivatives(forest_diff, latex_dump), latex_dump);

    LatexCreateChapterTaylor(latex_dump);
    CHECK_AND_RET_TREEERR_MATAN_BOOK(CreateDiffParams(forest_taylor), latex_dump);
    MetkiAddValues(forest_taylor->mtk);
    CHECK_AND_RET_TREEERR_MATAN_BOOK(CreateForestWithTaylorDecompose(forest_taylor, latex_dump), latex_dump);
    CHECK_AND_RET_TREEERR_MATAN_BOOK(CreateLatexTaylorDecompose(forest_taylor, latex_dump), latex_dump);
    CHECK_AND_RET_TREEERR_MATAN_BOOK(CreateAndLatexTaylorGraphics(forest_taylor, latex_dump), latex_dump);
    MetkiDelValues(forest_taylor->mtk);

    LatexCreateAfterWord(latex_dump);

    EndMatanBook(latex_dump);
    GeneratePdfFromTex(book_file);

    DEBUG_TREE(err = ForestVerify(forest_diff);
               err = ForestVerify(forest_taylor);)
    return err;
}