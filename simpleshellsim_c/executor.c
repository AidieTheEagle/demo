#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "treeengineer.h"
#include "executor.h"

int status[] = {0};

/**
 * Where the exit built-in is executed.
 * Simply sets a flag that is checked in other parts of the program
 * @param exitFlag bool * flag set to true and read by other parts of the program
*/
void executeExitBuiltin(bool *exitFlag) {
    (*exitFlag) = true;
}

/**
 * Where the staus built-in is executed.
 * Simply prints the current status which is a global variable
 * checked by interested parts of the program
*/
void executeStatusBuiltin() {
    printf("The most recent exit code is: %d\n", (*status));
}

void changeDirectoryBuiltin(char **commandarr) {
    if (commandarr[1] == NULL) {
        printf("Error: cd requires folder to navigate to!\n");
        (*status) = 2;
        return;
    }
    if (chdir(commandarr[1]) != 0) {
        printf("Error: cd directory not found!\n");
        (*status) = 2;
    }
}

bool executeBuiltins(CNode commandNode, bool *exitFlag) {

    char **commandarr = commandNode->command->words;

    if (strcmp("exit", commandarr[0]) == 0) {
        executeExitBuiltin(exitFlag);
        return true;
    } else if (strcmp("status", commandarr[0]) == 0) {
        executeStatusBuiltin();
        return true;
    } else if (strcmp("cd", commandarr[0]) == 0) {
        changeDirectoryBuiltin(commandarr);
        return true;
    }

    return false;
}

int *createPipe() {
    int *fd = malloc(3*sizeof(int));
    pipe(fd);

    /*
        fd[1] : output / where things are written to
        fd[0] : input / where things are read from
    */
    return fd;
}

void executePipeline(CNode commandNode, bool *commandNotFoundFlag) {
    pid_t pid;

    int *oldPipe;
    int *newPipe;
    bool previous = false;
    bool next = false;
    bool multiple = false;

    int pipesArrSize = 2;
    int nextPipeIndex = 0;
    int **pipes = malloc((pipesArrSize + 1) * sizeof(int *));

    int pidsArrSize = 2;
    int nextPidIndex = 0;
    pid_t *pids = malloc((pidsArrSize + 1) * sizeof(pid_t));

    CNode workingCommandNode = commandNode;
    while (workingCommandNode != NULL) {

        if (workingCommandNode->pipe_to != NULL) {
            // more piping to do
            newPipe = createPipe();
            if (nextPipeIndex == pipesArrSize) {
                pipesArrSize *= 2;
                pipes = realloc(pipes, (pipesArrSize + 1) * sizeof(int *)); 
            }
            pipes[nextPipeIndex] = newPipe;
            nextPipeIndex++;
            next = true;
            multiple = true;
        }

        pid = fork();
        if (pid < 0) {
            // failed ot make child process
            exit(1);
        } else if (pid == 0) {
            // this is the child process

            if (previous) {
                // there was a previous command
                dup2(oldPipe[0], 0);
                close(oldPipe[0]);
                close(oldPipe[1]);
            }

            if (next) {
                // there will be another command after this one
                close(newPipe[0]);
                dup2(newPipe[1], 1);
                close(newPipe[1]);
            }

            execvp(workingCommandNode->command->words[0], workingCommandNode->command->words);

            printf("Error: command not found!\n");
            (*commandNotFoundFlag) = true;

            for (int i = 0; i < nextPipeIndex; i++) {
                free(pipes[i]);
            }
            free(pipes);
            free(pids);

            return;
        } else {
            // this is the parent process
            if (nextPidIndex == pidsArrSize) {
                pidsArrSize *= 2;
                pids = realloc(pids, (pidsArrSize + 1) * sizeof(pid_t));
            }

            pids[nextPidIndex] = pid;
            nextPidIndex++;

            if (previous) {
                // there was a previous command
                close(oldPipe[0]);
                close(oldPipe[1]);
            }

            if (next) {
                oldPipe = newPipe;
            }
        }
        if (next) {
            previous = true;
        }
        next = false;
        workingCommandNode = workingCommandNode->pipe_to;
    }

    int wstatus;
    for (int i = 0; i < nextPidIndex; i++) {
        waitpid(pids[i], &wstatus, WUNTRACED);;
        (*status) = WEXITSTATUS(wstatus);
    }

    if (multiple) {
        close(oldPipe[0]);
        close(oldPipe[1]);
    }

    free(pids);

    for (int i = 0; i < nextPipeIndex; i++) {
        free(pipes[i]);
    }
    free(pipes);
}

bool handleRedirections(ChainNode *chainNode, int *stdinCopy, int *stdoutCopy) {

    if (chainNode->in_redirection != NULL && chainNode->out_redirection != NULL) {
        if (strcmp(chainNode->in_redirection, chainNode->out_redirection) == 0) {
            // input and output are the same
            printf("Error: input and output files cannot be equal!\n");
            return false;
        }
    }

    if (chainNode->in_redirection != NULL) {
        // input needs to be redirected

        // store copy of stdin for resetting later
        (*stdinCopy) = dup(0);

        // open file for reading
        char *inFileName = chainNode->in_redirection;
        int inFile = open(inFileName, O_RDONLY);

        if (dup2(inFile, 0) < 0) {
            // failed to redirect input
            (*status) = 1;
            return false;
        }

        close(inFile);
    }

    if (chainNode->out_redirection != NULL) {
        // output needs to be redirected

        // store copy of stdin for resetting later
        (*stdoutCopy) = dup(1);

        // open file for reading
        char *outFileName = chainNode->out_redirection;
        int outFile = open(outFileName, O_WRONLY | O_APPEND);

        if (dup2(outFile, 1) < 0) {
            // failed to redirect input
            (*status) = 1;
            return false;
        }

        close(outFile);
    }

    return true;
}

void revertIO(ChainNode *chainNode, int *stdinCopy, int *stdoutCopy) {
    if (chainNode->in_redirection != NULL) {
        // set input back to stdin
        if (dup2((*stdinCopy), 0) < 0) {
            // failed to revert input
            (*status) = 1;
        }
    }

    if (chainNode->out_redirection != NULL) {
        // set output back to stdout
        if (dup2((*stdoutCopy), 1) < 0) {
            // failed to revert output
            (*status) = 1;
        }
    }
}

void executeChain(ChainNode *chainNode, bool *exitFlag, bool *commnadNotFoundFlag) {
    // handle redirections
    // stdin  : 0
    // stdout : 1
    // stderr : 2
    int stdinCopy, stdoutCopy;
    bool successfullyRedirected = handleRedirections(chainNode, &stdinCopy, &stdoutCopy);

    if (!successfullyRedirected) {
        (*status) = 1;
        return;
    }

    if (!executeBuiltins(chainNode->commnadNode, exitFlag)) {
        executePipeline(chainNode->commnadNode, commnadNotFoundFlag);
    }

    // set stdin and stdout back to default
    revertIO(chainNode, &stdinCopy, &stdoutCopy);
}

void executeTree(TNode treeNode, bool *exitFlag, bool *commandNotFoundFlag) {

    if ((*exitFlag) || (*commandNotFoundFlag)) {
        return;
    }

    // check on left node
    if (treeNode->left != NULL) {
        executeTree(treeNode->left, exitFlag, commandNotFoundFlag);
    }

    if ((*exitFlag) || (*commandNotFoundFlag)) {
        return;
    }

    if (treeNode->operator != NULL) {
        if (strcmp(treeNode->operator, "&&") == 0) {
            // check that last command succeeded

            if ((*status) == 0) {
                // last command succeeded
                executeTree(treeNode->right, exitFlag, commandNotFoundFlag);
            }

        } else if (strcmp(treeNode->operator, "||") == 0) {
            // check that last command failed
            if ((*status) != 0) {
                // last command failed
                executeTree(treeNode->right, exitFlag, commandNotFoundFlag);
            }

        }
    } else {

        executeChain(treeNode->chainNode, exitFlag, commandNotFoundFlag);

    }

}
