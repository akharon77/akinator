#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "stack_debug.h"
#include "tree.h"

void TreeDtor(Node *vertex)
{
    
}

Node* TreeInsert(Node *anch, const char *str)
{
    Node *new_node = NodeNew();

}

Node *NodeNew()
{
    Node *res = (Node*) calloc(1, sizeof(Node));
    ASSERT(res != NULL);

    *res = 
        {
            .str    = NULL,
            .ancstr = NULL,
            .left   = NULL,
            .right  = NULL
        };

    return res;
}

void NodeCtor(Node *node, const char *str)
{
    ASSERT(node != NULL);

    *node = 
        {
            .str    = strdup(str),
            .ancstr = NULL,
            .left   = NULL,
            .right  = NULL
        };
}

bool NodeIsLeaf(Node *node)
{
    return node->left == NULL && node->right == NULL;
}
