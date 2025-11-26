#include "src/matan_book.h"

int GetE(size_t* pos, char* buffer);

int GetN(size_t* pos, char* buffer){
    int val = 0;
    while('0' <= buffer[*pos] && buffer[*pos] <= '9'){
        val = val * 10 + (buffer[*pos] - '0');
        (*pos)++;
    }
    return val;
}

int GetP(size_t* pos, char* buffer){
    int val = 0;
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


int GetT(size_t* pos, char* buffer){
    int val = GetP(pos, buffer);
    while(buffer[*pos] == '*' || buffer[*pos] == '/'){
        int op = buffer[*pos];
        (*pos)++;
        int val2 = GetP(pos, buffer);
        if(op == '*'){
            val *= val2;
        }
        if(op == '/'){
            val /= val2;
        }
    }
    return val;
}


int GetE(size_t* pos, char* buffer){
    int val = GetT(pos, buffer);
    while(buffer[*pos] == '+' || buffer[*pos] == '-'){
        int op = buffer[*pos];
        (*pos)++;
        int val2 = GetT(pos, buffer);
        if(op == '+'){
            val += val2;
        }
        if(op == '-'){
            val -= val2;
        }
    }
    return val;
}

int GetG(size_t* pos, char* buffer){
    int val = GetE(pos, buffer);
    if(buffer[*pos] != '$'){
        fprintf(stderr, "Syntax err");
        return 0;
    }
    return val;
}



int main(){
    size_t pos = 0;
    char* buffer = "10*(30+20*10)+13$";
    int x = GetG(&pos, buffer);
    printf("res = %d\n", x);

    Forest_t* forest_diff = MakeDiffForest("tests/btest.txt");
    Forest_t* forest_for_taylor = MakeDiffForest("tests/btest.txt");
    Forest_t* forest_with_taylor = ForestCtor(10);
    MatanBook(forest_diff, forest_for_taylor, forest_with_taylor, "output/diff.tex");
    ForestDtor(forest_diff);
    ForestDtor(forest_for_taylor);
    ForestDtor(forest_with_taylor);
}
