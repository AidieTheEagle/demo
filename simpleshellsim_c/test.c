#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

typedef struct CommandNode *CNode;

typedef struct CommandNode {
    char **command;
    CNode pipe_to;
} CommandNode;

typedef struct PIDLobby {
    int size;
    int nextFreeIndex;
    pid_t *pids;
} PIDLobby;

int *createPipe() {
    int *fd = malloc(3*sizeof(int));
    pipe(fd);

    /*
        fd[1] : output / where things are written to
        fd[0] : input / where things are read from
    */
    return fd;
}

// stdin  : 0
// stdout : 1
// stderr : 2

/*
    fd[1] : output / where things are written to
    fd[0] : input / where things are read from
*/

void runPipe2(CNode *commandNode) {
    pid_t pid;
    //int wstatus;

    int *old_fds;
    int *new_fds;
    bool previous = false;
    bool next = false;
    bool multiple = false;

    while ((*commandNode) != NULL) {

        if ((*commandNode)->pipe_to != NULL) {
            // more piping to do
            new_fds = createPipe();
            next = true;
            multiple = true;
        }

        pid = fork();
        if (pid < 0) {
            // failed ot make child process
            printf("failed to create child process for pipe\n");
            exit(1);
        } else if (pid == 0) {
            // this is the child process
            printf("this is the child process\n");

            if (previous) {
                // there was a previous command
                dup2(old_fds[0], 0);
                close(old_fds[0]);
                close(old_fds[1]);
            }

            if (next) {
                // there will be another command after this one
                close(new_fds[0]);
                dup2(new_fds[1], 1);
                close(new_fds[1]);
            }

            execvp((*commandNode)->command[0], (*commandNode)->command);

            exit(127);
        } else {
            // this is the parent process
            printf("this is parent process\n");
            waitpid(pid, NULL, WNOHANG);

            if (previous) {
                // there was a previous command
                close(old_fds[0]);
                close(old_fds[1]);
            }

            if (next) {
                old_fds = new_fds;
            }
        }
        //printf("hello!\n");
        if (next) {
            previous = true;
        }
        next = false;
        (*commandNode) = (*commandNode)->pipe_to;
    }

    if (multiple) {
        close(old_fds[0]);
        close(old_fds[1]);
    }
    
}

void runPipe(int *pfd, char **writingCommandarr, char **readingCommandarr) {
    pid_t pid;
    pid = fork();
    //int wstatus;
    
    if (pid < 0) {
        // failed to make child process
        printf("failed to make child for pipe\n");
        exit(1);
    } else if (pid == 0) {
        // this is the child process
        // takes reading command

        dup2(pfd[0], 0);
        close(pfd[1]);

        execvp(readingCommandarr[0], readingCommandarr);


    } else {
        // this is the parent process
        // takes writing command

        dup2(pfd[1], 1);
        close(pfd[0]);

        //execvp(writingCommandarr[0], writingCommandarr);
        //pid_t wpid = waitpid(pid, &wstatus, 0);
    }
}


int main(int argc, char const *argv[])
{

    //char *commandarr1[] = {"/bin/ls", "-a", "/", NULL};
    int size1 = 4, size2 = 4, size3 = 4;
    char **commandarr1 = malloc((size1 + 1)*sizeof(char *));
    commandarr1[0] = malloc((strlen("/bin/ls") + 1) * sizeof(char));
    strcpy(commandarr1[0], "/bin/ls\0");
    commandarr1[1] = malloc((strlen("-al") + 1) * sizeof(char));
    strcpy(commandarr1[1], "-al\0");
    commandarr1[2] = malloc((strlen("/") + 1) * sizeof(char));
    strcpy(commandarr1[2], "/\0");
    commandarr1[3] = NULL;

    char **commandarr2 = malloc((size2 + 1)*sizeof(char *));
    commandarr2[0] = malloc((strlen("/usr/bin/tr") + 1) * sizeof(char));
    strcpy(commandarr2[0], "/usr/bin/tr\0");
    commandarr2[1] = malloc((strlen("a-z") + 1) * sizeof(char));
    strcpy(commandarr2[1], "a-z\0");
    commandarr2[2] = malloc((strlen("A-Z") + 1) * sizeof(char));
    strcpy(commandarr2[2], "A-Z\0");
    commandarr2[3] = NULL;

    char **commandarr3 = malloc((size3 + 1)*sizeof(char *));
    commandarr3[0] = malloc((strlen("/usr/bin/tr") + 1) * sizeof(char));
    strcpy(commandarr3[0], "/usr/bin/tr\0");
    commandarr3[1] = malloc((strlen("A-Z") + 1) * sizeof(char));
    strcpy(commandarr3[1], "A-Z\0");
    commandarr3[2] = malloc((strlen("a-z") + 1) * sizeof(char));
    strcpy(commandarr3[2], "a-z\0");
    commandarr3[3] = NULL;

    CNode commandNode = malloc(sizeof(CommandNode));
    commandNode->command = commandarr1;
    commandNode->pipe_to = malloc(sizeof(CommandNode));
    commandNode->pipe_to->command = commandarr2;
    commandNode->pipe_to->pipe_to = malloc(sizeof(CommandNode));
    commandNode->pipe_to->pipe_to->command = commandarr3;
    commandNode->pipe_to->pipe_to->pipe_to = NULL;


    // int *pfd = createPipe();

    // pid_t pid;
    // pid = fork();
    // int wstatus;

    // if (pid < 0) {
    //     // failed to make child process
    //     printf("failed to make child\n");
    //     exit(1);
    // } else if (pid == 0) {
    //     // this is the child process
    //     printf("inside the child process\n");
    //     //execvp(commandarr[0], commandarr);

    //     runPipe(pfd, commandarr1, commandarr2);

    //     printf("Error: command not found!\n");
    //     exit(127);
    // } else {
    //     printf("inside the parent process\n");
    //     // this is the parent process
    //     pid_t wpid = waitpid(pid, &wstatus, 0);
    // }

    PIDLobby *pidLobby = malloc(sizeof(PIDLobby));
    pidLobby->size = 2;
    pidLobby->nextFreeIndex = 0;
    pidLobby->pids = malloc((pidLobby->size + 1) * sizeof(pid_t));

    CNode commandNodeCopy = commandNode;

    runPipe2(&commandNodeCopy);

    //int wstatus;

    // for (int i = 0; i < pidLobby->nextFreeIndex; i++) {
    //     waitpid(pidLobby->pids[i], &wstatus, 0);
    //     printf("exit status of %d: %d\n", pidLobby->pids[i], WEXITSTATUS(wstatus));
    // }


    //free(pfd);
    ////// freeing //////
    for (int i = 0; commandarr1[i] != NULL; i++) {
        free(commandarr1[i]);
    }
    free(commandarr1);

    for (int i = 0; commandarr2[i] != NULL; i++) {
        free(commandarr2[i]);
    }
    free(commandarr2);

     for (int i = 0; commandarr3[i] != NULL; i++) {
        free(commandarr3[i]);
    }
    free(commandarr3);

    free(commandNode->pipe_to->pipe_to);
    free(commandNode->pipe_to);
    free(commandNode);

    free(pidLobby->pids);
    free(pidLobby);
   
    return 0;
}
