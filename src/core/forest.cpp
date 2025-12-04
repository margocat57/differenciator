#include "forest.h"
#include "../utils/metki.h"
#include "tree_func.h"
#include <stdio.h>
#include <assert.h>

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
    forest->mtk = MetkiInit();
    forest->first_free_place = 0;
    return forest;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
// Forest add elem

TreeErr_t ForestAddElem(TreeHead_t *head_add, Forest_t *forest){
    TreeErr_t err = NO_MISTAKE;
    DEBUG_TREE(err = ForestVerify(forest);)
    if(err) return err;

    if(forest->first_free_place >= forest->num_of_trees){
        CHECK_AND_RET_TREEERR(ForestRealloc(forest, forest->num_of_trees * 2));
    }
    forest->head_arr[forest->first_free_place] = head_add;
    forest->first_free_place++;

    DEBUG_TREE(err = ForestVerify(forest);)
    return err;
}

TreeErr_t ForestRealloc(Forest_t *forest, size_t num_of_trees){
    TreeErr_t err = NO_MISTAKE;
    DEBUG_TREE(err = ForestVerify(forest);)
    if (err) return err;

    TreeHead_t** head_arr = (TreeHead_t**)realloc(forest->head_arr, num_of_trees * sizeof(TreeHead_t*));
    if(!head_arr){
        fprintf(stderr, "Can't alloc realloc for head_arr");
        ForestDtor(forest);
        return ALLOC_ERR;
    }
    forest->head_arr = head_arr;
    forest->num_of_trees = num_of_trees;

    DEBUG_TREE(err = ForestVerify(forest);)
    return err;
}

//-------------------------------------------------------------
//--------------------------------------------------------------
// Forest verify

TreeErr_t ForestVerify(Forest_t* forest){
    assert(forest);
    TreeErr_t err = NO_MISTAKE;
    for (size_t idx = 0; idx < forest->num_of_trees; idx++){
        err = TreeVerify(forest->head_arr[idx]);
        if(err) return err;
    }
    return err;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
// Forest dtor

void ForestDtor(Forest_t* forest){
    if(!forest){
        return;
    }

    MetkiDestroy(forest->mtk);

    for(size_t idx = 0; idx < forest->first_free_place; idx++){
        if(!forest->head_arr[idx]){
            continue;
        }
        TreeDel(forest->head_arr[idx]);
    }
    if(forest->head_arr){
        free(forest->head_arr);
    }

    free(forest);
}
