#include "forest.h"
#include "../common/metki.h"
#include "tree_func.h"
#include <stdio.h>

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// Forest Ctor

Forest_t* ForestCtor(size_t num_of_trees){
    Forest_t* forest = (Forest_t*)calloc(1, sizeof(Forest_t));
    if(!forest){
        fprintf(stderr, "Can't alloc memory for head_arr");
        return NULL;
    }

    TreeHead_t** head_arr = (TreeHead_t**)calloc(num_of_trees, sizeof(TreeHead_t*));
    if(!head_arr){
        fprintf(stderr, "Can't alloc memory for head_arr");
        return NULL;
    }
    forest->head_arr = head_arr;
    forest->num_of_trees = num_of_trees;
    forest->mtk = metki_init();
    forest->first_free_place = 0;
    return forest;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
// Forest add elem

void ForestAddElem(TreeHead_t* head_add, Forest_t* forest){
    if(forest->first_free_place >= forest->num_of_trees){
        ForestRealloc(forest, forest->num_of_trees * 2);
    }
    forest->head_arr[forest->first_free_place] = head_add;
    forest->first_free_place++;
}

void ForestRealloc(Forest_t* forest, size_t num_of_trees){
    TreeHead_t** head_arr = (TreeHead_t**)realloc(forest->head_arr, num_of_trees * sizeof(TreeHead_t*));
    if(!head_arr){
        fprintf(stderr, "Can't alloc realloc for head_arr");
        ForestDtor(forest);
        return;
    }
    forest->head_arr = head_arr;
    forest->num_of_trees = num_of_trees;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
// Forest dtor

void ForestDtor(Forest_t* forest){
    metki_destroy(forest->mtk);

    for(size_t idx = 0; idx < forest->first_free_place; idx++){
        if(!forest->head_arr[idx]){
            continue;
        }
        TreeDel(forest->head_arr[idx]);
    }

    free(forest);
}
