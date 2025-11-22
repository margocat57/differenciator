#ifndef GRAPHVIZ_DUMP_H
#define GRAPHVIZ_DUMP_H
#include <string.h>
#include "../data_struct/tree.h"

const char* const LOG_FILE = "log.htm";

struct filenames_for_dump{
    char* dot_filename;
    char* svg_filename;
};

void tree_dump_func(const TreeNode_t* node, const TreeHead_t* head, const char* debug_msg, const char *file, const char *func, int line, metki* mtk, ...) __attribute__ ((format (printf, 4, 8)));

#endif // GRAPHVIZ_DUMP_H