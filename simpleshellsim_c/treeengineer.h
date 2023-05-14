#ifndef TREEENGINEER_H
#define TREEENGINEER_H

#include "treeengineer.h"
#include "scanner.h"

typedef struct Command {
    int sentenceSize;
    int nextWordIndex;
    char **words;
} Command;

typedef struct CommandNode * CNode;

typedef struct CommandNode {
    Command *command;
    CNode pipe_to;
} CommandNode;

typedef struct ChainNode {
    CNode commnadNode;
    char *in_redirection;
    char *out_redirection;
} ChainNode;

typedef struct TreeNode * TNode;

typedef struct TreeNode {
    char *operator;
    ChainNode *chainNode;
    TNode left;
    TNode right;
} TreeNode;

TNode buildTree(List *lp);

void printTree(TNode treeNode);

void freeTree(TNode treeNode);

#endif
