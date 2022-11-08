#ifndef TREE_H
#define TREE_H

struct Node
{
    char *str;

    Node *ancstr;
    Node *left;
    Node *right;
};

void TreeCtor(Node *tree);
void TreeDtor(Node *tree);


#undef  // TREE_H
