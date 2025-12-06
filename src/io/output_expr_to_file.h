#ifndef OUTPUT_EXPR_TO_FILE_H
#define OUTPUT_EXPR_TO_FILE_H
#include "../core/tree.h"
#include "../core/forest.h"
#include "../utils/mistakes.h"
#include "../core/dump_mode.h"
#include <stdio.h>

void PutInfoRecursive(FILE* file, TreeNode_t* node, metki* mtk, TreeErr_t* err, DumpMode mode);

void DumpToFileGp(FILE* file, TreeNode_t* node, metki* mtk, TreeErr_t *err);

void NeedStaples(TreeNode_t* node, bool* need_staples, TreeErr_t *err);

#endif // OUTPUT_EXPR_TO_FILE_H