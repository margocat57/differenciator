#include "src/matan_book.h"
#include "src/core/tree_func.h"

TreeNode_t* GetE(size_t* pos, char* buffer);

TreeNode_t* GetN(size_t* pos, char* buffer){
    int val = 0;
    while('0' <= buffer[*pos] && buffer[*pos] <= '9'){
        val = val * 10 + (buffer[*pos] - '0');
        (*pos)++;
    }
    return NodeCtor(CONST, (TreeElem_t){.const_value = val}, NULL, NULL, NULL);
}

TreeNode_t* GetP(size_t* pos, char* buffer){
    TreeNode_t* val = NULL;
    if(buffer[*pos] == '('){
        (*pos)++;
        val = GetE(pos, buffer);
        if(buffer[*pos] == ')'){
            (*pos)++;
        }
    }
    else{
        return GetN(pos, buffer);
    }
    return val;
}


TreeNode_t* GetT(size_t* pos, char* buffer){
    TreeNode_t* left = GetP(pos, buffer);
    while(buffer[*pos] == '*' || buffer[*pos] == '/'){
        int op = buffer[*pos];
        (*pos)++;
        TreeNode_t* right = GetP(pos, buffer);
        TreeNode_t *new_node = NULL;
        // потом просто заифать с ошибкой если не умножение или деление
        // ошибку передавать как параметр или делать с пойзоном - инкорректом
        if(op == '*'){
            new_node = NodeCtor(OPERATOR, (TreeElem_t){.op = OP_MUL}, NULL, left, right);
        }
        else{
            new_node = NodeCtor(OPERATOR, (TreeElem_t){.op = OP_DEG}, NULL, left, right);
        }
        left = new_node;
    }
    return left;
}


TreeNode_t* GetE(size_t* pos, char* buffer){
    TreeNode_t* left = GetT(pos, buffer);
    while(buffer[*pos] == '+' || buffer[*pos] == '-'){
        int op = buffer[*pos];
        (*pos)++;
        TreeNode_t* right = GetT(pos, buffer);
        TreeNode_t *new_node = NULL;
        // потом просто заифать с ошибкой если не умножение или деление
        // ошибку передавать как параметр или делать с пойзоном - инкорректом
        if(op == '+'){
            new_node = NodeCtor(OPERATOR, (TreeElem_t){.op = OP_ADD}, NULL, left, right);
        }
        else{
            new_node = NodeCtor(OPERATOR, (TreeElem_t){.op = OP_SUB}, NULL, left, right);
        }
        left = new_node;
    }
    return left;
}

TreeNode_t* GetG(size_t* pos, char* buffer){
    TreeNode_t* head = GetE(pos, buffer);
    if(buffer[*pos] != '$'){
        fprintf(stderr, "Syntax err");
        return NULL;
    }
    return head;
}


int main(){
    /*
    size_t pos = 0;
    char* buffer = "10*(30+20*10)+13$";
    TreeNode_t* x = GetG(&pos, buffer);
    tree_dump_func(x, "making recursive tree", __FILE__, __func__, __LINE__, NULL);
    */

    Forest_t* forest_diff = MakeDiffForest("tests/etest.txt");
    tree_dump_func(forest_diff->head_arr[0]->root, "making recursive tree", __FILE__, __func__, __LINE__, forest_diff->mtk);
    // Forest_t* forest_for_taylor = MakeDiffForest("tests/btest.txt");
    // Forest_t* forest_with_taylor = ForestCtor(10);
    // MatanBook(forest_diff, forest_for_taylor, forest_with_taylor, "output/diff.tex");
    // ForestDtor(forest_diff);
    // ForestDtor(forest_for_taylor);
    // ForestDtor(forest_with_taylor);
}
