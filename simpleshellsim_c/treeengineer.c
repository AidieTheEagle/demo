#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "treeengineer.h"
#include "stackengineer.h"
#include "scanner.h"

Command *getNewEmptyCommand() {
    Command *newCommand = malloc(sizeof(Command));

    int sentenceSize = 2;

    // set defualt values
    newCommand->nextWordIndex = 0;
    newCommand->sentenceSize = sentenceSize;

    // setup initial words arr
    newCommand->words = malloc((sentenceSize+1)*sizeof(char *));

    return newCommand;
}

CNode getNewEmptyCommandNode() {
    CNode newCommandNode = malloc(sizeof(CommandNode));

    newCommandNode->command = getNewEmptyCommand();
    newCommandNode->pipe_to = NULL;

    return newCommandNode;
}

ChainNode *getNewEmptyChainNode() {
    ChainNode *newChainNode = malloc(sizeof(ChainNode));

    newChainNode->commnadNode = getNewEmptyCommandNode();
    newChainNode->in_redirection = NULL;
    newChainNode->out_redirection = NULL;

    return newChainNode;
}

TNode getNewOperatorTreeNode(char *operator) {
    TNode newOperatorTreeNode = malloc(sizeof(TreeNode));
    
    // assign operator
    int operatorSize = strlen(operator);
    newOperatorTreeNode->operator = malloc((operatorSize+1)*sizeof(char));
    strcpy(newOperatorTreeNode->operator, operator);
    newOperatorTreeNode->operator[operatorSize] = '\0';

    // chain node set to NULL. It will not be used
    newOperatorTreeNode->chainNode = NULL;

    // set children to NULL
    newOperatorTreeNode->left = NULL;
    newOperatorTreeNode->right = NULL;

    return newOperatorTreeNode;
}

TNode getNewEmptyTreeNode() {
    TNode newTreeNode = malloc(sizeof(TreeNode));

    // set operator to NULL. It will not be used
    newTreeNode->operator = NULL;

    // initialise chainNode
    newTreeNode->chainNode = getNewEmptyChainNode();

    // set children to NULL
    newTreeNode->left = NULL;
    newTreeNode->right = NULL;

    return newTreeNode;
}

void commpleteWaitingOperator(TreeNodeStack *operatorStack, TreeNodeStack *chainStack) {
    // get waiting operator node
    TNode waitingOperatorNode = pop(operatorStack);
    //printf("waiting operator is: %s\n", waitingOperatorNode->operator);

    // complete waiting operator node
    waitingOperatorNode->right = pop(chainStack);
    waitingOperatorNode->left = pop(chainStack);

    // add completed operator node to chain stack
    push(waitingOperatorNode, chainStack);
}

void handleNewOperator(char *operator, TreeNodeStack *operatorStack, TreeNodeStack *chainStack) {
    // create new operator node
    TNode operatorNode = getNewOperatorTreeNode(operator);

    // check if operator stack is empty
    if (!stackIsEmpty(operatorStack)) {
        // there is an operator waiting and it can be completed
        commpleteWaitingOperator(operatorStack, chainStack);    
    }
    // current operator needs to wait for next TreeNode to finish
    // add to operator stack
    push(operatorNode, operatorStack);
}



void addRedirection(char **redirection, char *file) {
    int size = strlen(file);
    *redirection = malloc((size+1)*sizeof(char));
    strcpy((*redirection), file);
    (*redirection)[size] = '\0';
}

void addWordToCommand(Command *command, char *word) {
    // allocate more memory if necessary
    if (command->nextWordIndex == command->sentenceSize) {
        command->sentenceSize *= 2;
        command->words = realloc(command->words,(command->sentenceSize+1)*sizeof(char *));
    }

    // allocate memory for new word, copy word and finish with null char
    int wordSize = strlen(word);
    int freeIndex = command->nextWordIndex;
    command->words[freeIndex] = malloc((wordSize+1)*sizeof(char));
    strcpy(command->words[freeIndex],word);
    command->words[freeIndex][wordSize] = '\0';

    // incremement to point to next available index
    command->nextWordIndex++;
}

void completeCommand(Command *command) {
    // allocate more memory if necessary
    if (command->nextWordIndex == command->sentenceSize) {
        command->sentenceSize *= 2;
        command->words = realloc(command->words,(command->sentenceSize+1)*sizeof(char *));
    }

    int freeIndex = command->nextWordIndex;
    command->words[freeIndex] = NULL;

    command->nextWordIndex++;
}

TNode buildTree(List *lp) {

    TNode workingTreeNode = getNewEmptyTreeNode();
    ChainNode *workingChainNode = workingTreeNode->chainNode;
    CNode workingCommandNode = workingChainNode->commnadNode;

    TreeNodeStack *chainStack = getNewEmptyTreeNodeStack();
    TreeNodeStack *operatorStack = getNewEmptyTreeNodeStack();

    while ((*lp) != NULL) {

        if (strcmp((*lp)->t, "&") == 0) {
            // background process operator?
            // for future implementation (perhaps)
        } else if (strcmp((*lp)->t, "&&") == 0) {
            // AND operator

            // complete current working command
            completeCommand(workingCommandNode->command);

            push(workingTreeNode, chainStack);
            handleNewOperator("&&", operatorStack, chainStack);
            workingTreeNode = getNewEmptyTreeNode();
            workingChainNode = workingTreeNode->chainNode;
            workingCommandNode = workingChainNode->commnadNode;

        } else if (strcmp((*lp)->t, "||") == 0) {
            // OR operator

            // complete current working command
            completeCommand(workingCommandNode->command);

            push(workingTreeNode, chainStack);
            handleNewOperator("||", operatorStack, chainStack);
            workingTreeNode = getNewEmptyTreeNode();
            workingChainNode = workingTreeNode->chainNode;
            workingCommandNode = workingChainNode->commnadNode;

        } else if (strcmp((*lp)->t, "<") == 0) {
            // input redirection

            (*lp) = (*lp)->next;
            addRedirection(&workingChainNode->in_redirection, (*lp)->t);

        } else if (strcmp((*lp)->t, ">") == 0) {
            // output redirection

            (*lp) = (*lp)->next;
            addRedirection(&workingChainNode->out_redirection, (*lp)->t);

        } else if (strcmp((*lp)->t, "|") == 0) {
            // pipe

            // complete current working command
            completeCommand(workingCommandNode->command);

            // initialise pipe-to command node
            workingCommandNode->pipe_to = getNewEmptyCommandNode();

            // set working command node to pip-to command node
            workingCommandNode = workingCommandNode->pipe_to;

        } else if (strcmp((*lp)->t, ";") == 0) {
            // end of the line (for now)
            (*lp) = (*lp)->next;
            break;
        } else {
            // command word

            addWordToCommand(workingCommandNode->command, (*lp)->t);

        }

        (*lp) = (*lp)->next;
    }

    // complete current working command
    completeCommand(workingCommandNode->command);

    if (!stackIsEmpty(operatorStack)) {
        // need to finish handling operator on stack
        push(workingTreeNode, chainStack);
        commpleteWaitingOperator(operatorStack, chainStack);
        workingTreeNode = pop(chainStack);
    }

    freeStack(operatorStack);
    freeStack(chainStack);

    return workingTreeNode;
}

/////////////////////// freeing /////////////////////////////

void freeCommand(Command *command) {
    for (int i = 0; i < command->nextWordIndex; i++) {
        free(command->words[i]);
    }
    free(command->words);
    free(command);
}

void freeCommandNode(CNode commandNode) {
    if (commandNode == NULL) {
        return;
    }
    freeCommand(commandNode->command);
    freeCommandNode(commandNode->pipe_to);
    free(commandNode);
}

void freeChainNode(ChainNode *chainNode) {
    if (chainNode == NULL) {
        return;
    }

    freeCommandNode(chainNode->commnadNode);

    free(chainNode->in_redirection);
    free(chainNode->out_redirection);
    free(chainNode);
}

void freeTree(TNode treeNode) {
    if (treeNode == NULL) {
        return;
    }
    freeTree(treeNode->left);
    freeTree(treeNode->right);

    freeChainNode(treeNode->chainNode);

    free(treeNode->operator);
    free(treeNode);
}

/////////////////////// printing /////////////////////////////

void printCommand(Command *command) {
    if (command == NULL) {
        return;
    }
    for (int i = 0; i < command->nextWordIndex; i++) {
        printf("%s ", command->words[i]);
    }
}

void printCommandNode(CNode commandNode) {
    if (commandNode == NULL) {
        return;
    }
    printCommand(commandNode->command);
    if (commandNode->pipe_to != NULL) {
        printf("| ");
        printCommandNode(commandNode->pipe_to);
    }
}

void printChainNode(ChainNode *chainNode) {
    printCommandNode(chainNode->commnadNode);
    printf("\n");
    printf("Input redirection: %s\n", chainNode->in_redirection);
    printf("Output redirection: %s\n", chainNode->out_redirection);
}

void printTree(TNode treeNode) {
    if (treeNode == NULL) {
        return;
    }
    printTree(treeNode->left);

    if (treeNode->operator != NULL) {
        printf("Operator: %s\n", treeNode->operator);
    } else {
        printChainNode(treeNode->chainNode);
    }

    printTree(treeNode->right);
}
