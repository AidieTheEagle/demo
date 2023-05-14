#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#include "scanner.h"

typedef struct CompTreeNode *TreeNode;

typedef struct CompTreeNode {
    int complete;
    int sentenceSize;
    int nextWordIndex;
    char **words;
    TreeNode left;
    TreeNode right; 
} CompTreeNode;

void freeTreeNode(TreeNode node);

void printTreeNode(TreeNode node);

TreeNode buildCommand(List *lp, char **in, char **out);


#endif
