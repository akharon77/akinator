#include <string.h>
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
    Node *res = calloc(1, sizeof(Node));
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

