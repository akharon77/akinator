#ifndef TREE_H
#define TREE_H

struct Node
{
    char *str;

    Node *left;
    Node *right;
};

void TreeCtor(Tree *tree);
void TreeDtor(Tree *tree);


#undef  // TREE_H
