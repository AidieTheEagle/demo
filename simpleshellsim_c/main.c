#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "scanner.h"
#include "shell.h"
#include "treeengineer.h"
#include "executor.h"

int main(int argc, char *argv[]) {

    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    // initialise flags
    // flags are not implemented very well at the moment.
    // one of the future improvements.
    bool endOfFileFlag = false;
    bool exitFlag = false;
    bool commandNotFoundFlag = false;

    char *inputLine;
    List tokenList;
    List tokenListParse;

    while (true) {

        // check if exit has been called.
        // also checks commandNotFoundFlag for a child process to allow for proper freeing.
        if (exitFlag || commandNotFoundFlag) {
            break;
        }

        inputLine = readInputLine(&endOfFileFlag);

        // sees if an EOF has been read and frees the inputline before breaking.
        if (endOfFileFlag) {
            free(inputLine);
            break;
        }

        tokenList = getTokenList(inputLine);
        tokenListParse = tokenList;

        bool parsedSuccessfully = parseInputLine(&tokenListParse);

        if (tokenListParse == NULL && parsedSuccessfully) {

            tokenListParse = tokenList;

            //TreeNode node;
            TNode treeNode;

            // loops while there are still tokens to parse.
            // here mainly to deal with ';' operator so that command execution continues.
            while (tokenListParse != NULL) {

                // check if exit has been called.
                // also checks commandNotFoundFlag for a child process to allow for proper freeing. 
                if (exitFlag || commandNotFoundFlag) {
                    break;
                }

                // get composition expression tree of commands from token list.
                treeNode = buildTree(&tokenListParse);

                // execute composition expression tree of commands.
                executeTree(treeNode, &exitFlag, &commandNotFoundFlag);

                // free tree for next command composition.
                freeTree(treeNode);
            }


            // Input was parsed successfully and can be accessed in "tokenList"

            // However, this is still a simple list of strings, it might be convenient
            // to build some intermediate structure representing the input line or a
            // command that you then construct in the parsing logic. It's up to you
            // to determine how to approach this!
        } else {
            printf("Error: invalid syntax!\n");
        }

        free(inputLine);
        freeTokenList(tokenList);
    }

    // checks if the commandNotFoundFlag has been set.
    // here for child process to exit in the appropriate manner.
    if (commandNotFoundFlag) {
        return 127;
    }
    
    return 0;
}
