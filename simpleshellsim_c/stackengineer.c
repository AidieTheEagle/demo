#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "stackengineer.h"
#include "treeengineer.h"

TreeNodeStack *getNewEmptyTreeNodeStack() {
    TreeNodeStack *newTreeNodeStack = malloc(sizeof(TreeNodeStack));
    newTreeNodeStack->top = NULL;
    return newTreeNodeStack;
}

void push(TNode treeNode, TreeNodeStack *stack) {
    SNode stackNode = malloc(sizeof(TreeNodeStackNode));

    stackNode->treeNode = treeNode;
    stackNode->next = NULL;

    if (stack->top != NULL) {
        // stack is not empty
        stackNode->next = stack->top;
    }
    stack->top = stackNode;
}

TNode pop(TreeNodeStack *stack) {
    if (stack->top == NULL) {
        // stack is empty
        return NULL;
    }

    SNode stackNode = stack->top;

    if (stackNode->next == NULL) {
        // last element
        stack->top = NULL;
    } else {
        // at least one other element
        stack->top = stackNode->next;
    }

    stackNode->next = NULL;

    TNode treeNode = stackNode->treeNode;

    free(stackNode);

    return treeNode;
}

bool stackIsEmpty(TreeNodeStack *stack) {
    if (stack->top == NULL) {
        return true;
    }
    return false;
}

void freeStackNode(SNode stackNode) {
    if (stackNode == NULL) {
        return;
    }
    freeStackNode(stackNode->next);
    free(stackNode);
}

void freeStack(TreeNodeStack *stack) {
    freeStackNode(stack->top);
    free(stack);
}
