#ifndef STACKENGINEER_H
#define STACKENGINEER_H

#include <stdbool.h>

#include "stackengineer.h"
#include "treeengineer.h"

typedef struct TreeNodeStackNode *SNode;

typedef struct TreeNodeStackNode {
    TNode treeNode;
    SNode next;
} TreeNodeStackNode;

typedef struct TreeNodeStack {
    SNode top;
} TreeNodeStack;

TreeNodeStack *getNewEmptyTreeNodeStack();

void push(TNode treeNode, TreeNodeStack *stack);

TNode pop(TreeNodeStack *stack);

bool stackIsEmpty(TreeNodeStack *stack);

void freeStack(TreeNodeStack *stack);

#endif
