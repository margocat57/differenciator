#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "tree_func.h"
#include "../dump/graphviz_dump.h"
#include "../utils/mistakes.h"
#include "operator_func.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
// Tree and node constructors

TreeHead_t* TreeCtor(){
    TreeHead_t* head = (TreeHead_t*)calloc(1, sizeof(TreeHead_t));
    head->root = NULL;
    
    return head;
}

TreeNode_t* NodeCtor(VALUE_TYPE type, TreeElem_t data, TreeNode_t* parent, TreeNode_t* left, TreeNode_t* right){
    TreeNode_t* node = (TreeNode_t*)calloc(1, sizeof(TreeNode_t));
    if(!node){
        fprintf(stderr, "Can't alloc data for node");
        return NULL;
    }
    node->type = type;
    node->data = data; 
    node->left = left;
    node->right = right;
    node->parent = parent;
    node->signature = TREE_SIGNATURE;

    return node;
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
// Node Copy

TreeNode_t* NodeCopy(TreeNode_t* node){
    if(!node) return NULL;
    return NodeCtor(node->type, node->data, node->parent, NodeCopy(node->left), NodeCopy(node->right));
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
// Verifying (recursive algorithm for verifying subtree)

TreeErr_t TreeVerify(const TreeHead_t* head){
    return TreeNodeVerify(head->root);
}

TreeErr_t TreeNodeVerify(const TreeNode_t *node){
    if(!node){
        fprintf(stderr, "NULL node ptr\n");
        return NULL_NODE_PTR;
    }
    if(node->signature != TREE_SIGNATURE){
        fprintf(stderr, "Incorr signature\n");
        return INCORRECT_SIGN;
    }

    if(node->left && node != node->left->parent){
        fprintf(stderr, "Incorr connection between parent(%p) and LEFT child(%p) nodes\n", node, node->left);
        return INCORR_LEFT_CONNECT;
    }
    if(node->right && node != node->right->parent){
        fprintf(stderr, "Incorr connection between parent(%p) and RIGHT child(%p) nodes\n", node, node->right);
        return INCORR_RIGHT_CONNECT;
    }
    if(node->left && node->left == node->right){
        fprintf(stderr, "LOOPED NODE - same connection for left and right");
        return LOOPED_NODE;
    }
    if(node->type == OPERATOR && (node->data.op == OP_ADD || node->data.op == OP_SUB || node->data.op == OP_DIV || node->data.op == OP_MUL || node->data.op == OP_DEG) && (!node->left || !node->right)){
        fprintf(stderr, "No element for binary operator");
        return NO_ELEM_FOR_BINARY_OP;
    }

    if(node->left){
        TreeErr_t err = NO_MISTAKE_T;
        DEBUG_TREE(err = TreeNodeVerify(node->left);)
        if (err) return err;
    }
    if(node->right){
        TreeErr_t err = NO_MISTAKE_T;
        DEBUG_TREE(err = TreeNodeVerify(node->right);)
        if(err) return err;
    }

    return NO_MISTAKE_T;
}


//----------------------------------------------------------------
// Dumping tree

static void PrintNodeConnect(const TreeNode_t* node, const TreeNode_t* node_child, FILE* dot_file, int* rank);

TreeErr_t PrintNode(const TreeNode_t* node, FILE* dot_file, int* rank, metki* mtk){
    assert(node);
    assert(rank);

    if(node->left){
        PrintNodeConnect(node, node->left, dot_file, rank);
        CHECK_AND_RET_TREEERR(PrintNode(node->left, dot_file, rank, mtk))
    }

    if(node->type == OPERATOR){
        size_t arr_num_of_elem = sizeof(OPERATORS_INFO) / sizeof(op_info);
        if(node->data.op >= arr_num_of_elem){
            return INCORR_OPERATOR;
        }
        fprintf(dot_file, " node_%p[shape=\"Mrecord\", style=\"filled\", fillcolor=\"#00FFFF\", rank=%d, color = \"#964B00\", penwidth=1.0, label=\"{{type = OPERATOR} | {val = %d(%s)} | {L | R }} \"];\n", node, *rank, node->data.op, OPERATORS_INFO[node->data.op].name_for_graph_dump);
    }
    else{
        if(node->type == CONST){
            fprintf(dot_file, " node_%p[shape=\"Mrecord\", style=\"filled\", fillcolor=\"#98FB98\", rank=%d, color = \"#964B00\", penwidth=1.0, label=\"{{type = CONST_VAR} | {val = %.2lf} | {0 | 0}} \"];\n", node, *rank, node->data.const_value);
        }
        else if(node->type == VARIABLE){
            fprintf(dot_file, " node_%p[shape=\"Mrecord\", style=\"filled\", fillcolor=\"#DAA520\", rank=%d, color = \"#964B00\", penwidth=1.0, label=\"{{type = VARIABLE} | {val = %zu(%s)} | {0 | 0}} \"];\n", node, *rank, node->data.var_code, mtk->var_info[node->data.var_code].variable_name);
        }
    }

    if(node->right){
        PrintNodeConnect(node, node->right, dot_file, rank);
        CHECK_AND_RET_TREEERR(PrintNode(node->right, dot_file, rank, mtk))
    }
    (*rank)--;
    return NO_MISTAKE_T;
}

static void PrintNodeConnect(const TreeNode_t* node, const TreeNode_t* node_child, FILE* dot_file, int* rank){
    assert(rank);
    assert(node_child);

    if(node_child->parent == node){
        fprintf(dot_file, " node_%p -> node_%p[color = \"#964B00\", dir = both];\n", node, node_child); 
    }
    else{
        fprintf(dot_file, " node_%p[shape=\"Mrecord\", style=\"filled\", fillcolor=\"#ff0000\", rank=%d, color = \"#964B00\", penwidth=1.0, label=\"{{INCORRECT NODE} | {INCORR! | INCORR!}} \"];\n", node_child->parent, *rank);
        fprintf(dot_file, " node_%p -> node_%p[color = \"#0000FF\"];\n", node, node_child);
        if(node_child->parent){
            fprintf(dot_file, " node_%p -> node_%p[color = \"#FF4F00\"];\n", node_child->parent, node_child);
        }
    }
    (*rank)++;
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
// Tree and node destructors


TreeErr_t TreeDel(TreeHead_t* head){
    assert(head);

    TreeErr_t err = NO_MISTAKE_T;
    DEBUG_TREE(err = TreeVerify(head);)
    if(err) return err;

    CHECK_AND_RET_TREEERR(TreeDelNodeRecur(head->root))

    memset(head, 0, sizeof(TreeHead_t));
    free(head);

    return err;
}

TreeErr_t TreeDelNodeRecur(TreeNode_t* node){
    TreeErr_t err = NO_MISTAKE_T;
    DEBUG_TREE(err = TreeNodeVerify(node);)
    if(err) return err;

    TreeNode_t* parent = node->parent;

    if(node->left){
        CHECK_AND_RET_TREEERR(TreeDelNodeRecur(node->left))
    }
    if(node->right){
        CHECK_AND_RET_TREEERR(TreeDelNodeRecur(node->right))
    }

    NodeDtor(node);

    return err;
}

void NodeDtor(TreeNode_t* node){
    if(node){
        memset(node, 0, sizeof(TreeNode_t));
        free(node);
    }
}