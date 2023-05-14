#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <stdbool.h>

#include "treeengineer.h"

void executeTree(TNode treeNode, bool *exitFlag, bool *commandNotFoundFlag);

#endif
