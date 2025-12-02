#ifndef TREE_H
#define TREE_H
#include <stdint.h>
#include <string.h>
#include "../utils/metki.h"

const char* const TREE_SIGNATURE = "My_tree";
const int POISON = -6666;

enum OPERATORS{
    INCORR,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_DEG,
    OP_SIN,
    OP_COS,
    OP_LN,
    OP_TG,
    OP_CTG,
    OP_SH,
    OP_CH,
    OP_TH,
    OP_CTH,
    OP_ARCSIN,
    OP_ARCCOS,
    OP_ARCTG,
    OP_ARCCTG
};

union TreeElem_t{
    enum OPERATORS op;
    double const_value;
    size_t var_code; // вычисляется по таблице имен в момент чтения из файла - это больно делать
};

#ifdef _DEBUG_SMALL_TREE
    #define DEBUG_TREE(...) __VA_ARGS__
#else 
    #define DEBUG_TREE(...)
#endif //_DEBUG_SMALL_TREE


#define CHECK_AND_RET_TREEERR(bad_condition)\
    do{\
    TreeErr_t err_macro = (bad_condition);\
        if(err_macro){ \
            fprintf(stderr, "err = %llu, %s, %s, %d\n", err_macro, __FILE__, __func__, __LINE__); \
            return err_macro; \
        } \
    }while(0); \


enum VALUE_TYPE{
    INCORR_VAL,
    OPERATOR,
    VARIABLE,
    CONST
};

struct TreeNode_t{
    VALUE_TYPE type;
    TreeElem_t data;
    TreeNode_t* left;
    TreeNode_t* right;
    TreeNode_t* parent;
    const char* signature;
};

struct TreeHead_t{
    TreeNode_t* root;
};


#endif //TREE_H