#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#include "datastructures.h"
#include "scanner.h"



/**
 * Creates a new empty tree node.
 * Sets some default values and sets the rest to NULL.
 * @return an empty tree node.
*/
TreeNode newEmptyTreeNode() {
    TreeNode node = malloc(sizeof(CompTreeNode));
    assert(node != NULL);
    node->complete = 0;
    node->sentenceSize = 2;
    node->nextWordIndex = 0;
    node->words = NULL;
    node->left = NULL;
    node->right = NULL;

    return node;
}


/**
 * Adds a new word to a node's list of words.
 * These words make up a command.
 * @param node pointer to a treeNode node that the word
 * is added to.
 * @param word char * word to add to the list of words
*/
void addWord(TreeNode node, char *word) {
    if (node->words == NULL) {
        // command is empty

        // allocate memory for words arr
        node->words = malloc((node->sentenceSize+1)*sizeof(char *));

        // allocate memory for first word, copy word and finish with null char
        int size = strlen(word);
        node->words[0] = malloc((size+1)*sizeof(char));
        strcpy(node->words[0],word);
        node->words[0][size] = '\0';

        // incrememt to point to next available index
        node->nextWordIndex++;
    } else {
        // command is not empty

        // allocate more memory if necessary
        if (node->nextWordIndex == node->sentenceSize) {
            node->sentenceSize *= 2;
            node->words = realloc(node->words,(node->sentenceSize+1)*sizeof(char *));
        }

        // allocate memory for new word, copy word and finish with null char
        int size = strlen(word);
        int index = node->nextWordIndex;
        node->words[index] = malloc((size+1)*sizeof(char));
        strcpy(node->words[index],word);
        node->words[index][size] = '\0';

        // incremement to point to next available index
        node->nextWordIndex++;
    }
}

/**
 * Determines which node should recieve the new word.
 * Walks through tree in order of left, parent, right.
 * Gives word to first incomplete node.
 * Current implementation should guarantee that it can not
 * reach beyond 'check current' if left node is NULL, but
 * if there is an edge case it has not been found yet.
 * @param node pointer to a tree from which to begin its
 * search.
 * @param word char * word to be added to the appropriate node.
*/
void addWordToNode(TreeNode node, char *word) {

    // check left
    if (node->left != NULL) {
        addWordToNode(node->left, word);
    }

    // check current
    if (node->complete == 0) {
        addWord(node, word);
        return;
    }

    // check right
    if (node->right != NULL) {
        addWordToNode(node->right, word);
    }

    // something went wrong
}

/**
 * Will complete the given node and all of its children.
 * We assume that when a node is completed then its children
 * will also not need to be further modified.
 * Walks down in order of left, right, parent.
 * @param node a pointer to a tree to be completed.
*/
void completeNode(TreeNode node) {

    if (node->left != NULL) {
        completeNode(node->left);
    }

    if (node->right != NULL) {
        completeNode(node->right);
    }

    if (node->complete == 1) {
        return;
    }

    if (node->words != NULL) {
        if (node->nextWordIndex == node->sentenceSize) {
            node->sentenceSize *= 2;
            node->words = realloc(node->words,(node->sentenceSize+1)*sizeof(char *));
        }

        int index = node->nextWordIndex;
        node->words[index] = NULL;  
    }

    node->complete = 1;
}

/**
 * Frees the data of a node as well as
 * freeing the data of all of its children, and
 * the children themselves.
 * Caller will need to make sure to free the provided node however.
 * Frees in order of parent data, left data, left, right data, right.
 * @param node a pointer to a tree whose data and children need
 * to be freed.
*/
void freeTreeNode(TreeNode node) {

    if (node == NULL) {
        return;
    }

    // free individual words
    if (node->words != NULL) {
        for (int i = 0; node->words[i] != NULL; i++) {
            free(node->words[i]);
        }
    }

    // free arrs of current node
    free(node->words);

    // free left node
    freeTreeNode(node->left);
    free(node->left);

    // free right node
    freeTreeNode(node->right);
    free(node->right);
    free(node);
}

/**
 * Prints a tree in order of left, parent, right.
 * @param node a pointer for the tree to be printed
*/
void printTreeNode(TreeNode node) {

    if (node->left != NULL) {
        printTreeNode(node->left);
    }

    printf("command: ");
    for (int i = 0; node->words[i] != NULL; i++) {
        printf("%s ", node->words[i]);
    }

    printf("\n");

    if (node->right != NULL) {
        printTreeNode(node->right);
    }

}

/**
 * Creates a TreeNode node for an operator.
 * Adds the operator as a word to the node and completes it.
 * Makes it the parent of a given node and creates a new working node
 * as its right child.
 * @param node a pointer to a TreeNode pointer that needs to be made a
 * left child of an operator node.
 * @param operator char * of the operator to add to the operator node.
*/
void addOperator(TreeNode *node, char *operator) {

    TreeNode newNode = newEmptyTreeNode();

    addWordToNode(newNode, operator);
    completeNode(newNode);
    newNode->left = (*node);
    newNode->right = newEmptyTreeNode();

    (*node) = newNode;
}

/**
 * Builds a tree from a given token list.
 * Only supports building trees for first lab as of now.
 * Extending it should be straightforward however.
 * @param lp List type of tokens to be parsed in
 * order to construct the tree.
*/
TreeNode buildCommand(List *lp, char **in, char **out) {

    TreeNode node = newEmptyTreeNode();

    while ((*lp) != NULL) {

        if (strcmp((*lp)->t, "&&") == 0) {

            completeNode(node);
            addOperator(&node, (*lp)->t);

        } else if (strcmp((*lp)->t, "||") == 0) {

            completeNode(node);
            addOperator(&node, (*lp)->t);

        } else if (strcmp((*lp)->t, "<") == 0) {

            // set input
            (*lp) = (*lp)->next;
            int size = strlen((*lp)->t);
            if (*in == NULL) {
                *in = malloc((size+1)*sizeof(char));
                strcpy(*in, (*lp)->t);
                (*in)[size] = '\0';
            }


        } else if (strcmp((*lp)->t, ">") == 0) {

            // set output
            (*lp) = (*lp)->next;
            int size = strlen((*lp)->t);
            if (*out == NULL) {
                *out = malloc((size+1)*sizeof(char));
                strcpy(*out, (*lp)->t);
                (*out)[size] = '\0';
            }


        } else if (strcmp((*lp)->t, ";") == 0) {
            (*lp) = (*lp)->next;
            break;

        } else {

            addWordToNode(node, (*lp)->t);
        }

        (*lp) = (*lp)->next;
    }

    completeNode(node);

    return node;
}
