#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "scanner.h"

/**
 * The function acceptToken checks whether the current token matches a target identifier,
 * and goes to the next token if this is the case.
 * @param lp List pointer to the start of the tokenlist.
 * @param ident target identifier
 * @return a bool denoting whether the current token matches the target identifier.
 */
bool acceptToken(List *lp, char *ident) {
    if (*lp != NULL && strcmp(((*lp)->t), ident) == 0) {
        *lp = (*lp)->next;;
        return true;
    }
    return false;
}


/**
 * The function parseExtension parses the extension of a file
 * Should only be letters or numbers
 * @param lp List pointer to the start of the tokenlist.
 * @param start start check chars from this index.
 * @return a bool denoting whether the extension was parsed successfully
*/
bool parseExtension(List *lp, int start) {
    char *extension = (*lp)->t;

    for (int i = start; i < strlen(extension); i++) {
        if ((extension[i] >= 'a' && extension[i] <= 'z') || 
            (extension[i] >= 'A' && extension[i] <= 'Z' ) ||
            (extension[i] >= '0' && extension[i] <= '9')) {
            continue;
        }
        return false;
    }
    *lp = (*lp)->next;
    return true;
}

/**
 * file should be of form
 * (1) <file>       ::=     <filename>                  |
 * (2)                      <filename> "." <extension>
 * The function parseFileName parses a filename.
 * @param lp List pointer to the start of the tokenlist.
 * @param start start checking chars from this index.
 * @return a bool denoting whether the filename was parsed successfully.
 */
bool parseFileName(List *lp, int start) {
    //printf("entering parseFileName\n");
    //TODO: Process the file name appropriately
    char *fileName = (*lp)->t;
    bool hasName = false;
    bool forwardSlash = false;
    bool dot = false;

    for (int i = start; i < strlen(fileName); i++) {
        
        if (dot && fileName[i] == '/') {
            dot = false;
            continue;
        }
        
        if (dot && fileName[i] != '/') {
            return false;
        }
        
        if (!forwardSlash && fileName[i] == '/') {
            forwardSlash = true;
            continue;
        }

        if (!hasName && fileName[i] == '.') {
            dot = true;
            continue;
        }

        if ((fileName[i] >= 'a' && fileName[i] <= 'z') || 
            (fileName[i] >= 'A' && fileName[i] <= 'Z' ) ||
            (fileName[i] >= '0' && fileName[i] <= '9')) {
            hasName = true;
            forwardSlash = false;
            continue;
        }

        if (hasName && fileName[i] == '.') {
            return parseExtension(lp,i+1);
        }
        return false;
    }

    (*lp) = (*lp)->next;
    return true;
}

/**
 * exectuable should be of the form:
 * (1) <executable> ::=     "./" <nestedfile>           |
 * (2)                      "/" <nestedfile>            |
 * (3)                      <file>
 * (4) <nestedfile> ::=     <dir> "/" <nestedfile>      |
 * (5)                      <file>

 * The function parseExecutable parses an executable.
 * @param lp List pointer to the start of the tokenlist.
 * @return a bool denoting whether the executable was parsed successfully.
 */
bool parseExecutable(List *lp) {
    char *executable = (*lp)->t;
    //printf("%s\n", executable);
    if (executable[0] == '.' && executable[1] == '/') {
        // skip first index to avoid parsing '.'
        return parseFileName(lp, 1);
    }

    if (!parseFileName(lp, 0)) {
        return false;
    }
    return true;
}

/**
 * Checks whether the input string \param s is an operator.
 * @param s input string.
 * @return a bool denoting whether the current string is an operator.
 */
bool isOperator(char *s) {
    // NULL-terminated array makes it easy to expand this array later
    // without changing the code at other places.
    char *operators[] = {
            "&",
            "&&",
            "||",
            ";",
            "<",
            ">",
            "|",
            NULL
    };

    for (int i = 0; operators[i] != NULL; i++) {
        if (strcmp(s, operators[i]) == 0) return true;
    }
    return false;
}

/**
 * The function parseOptions parses options.
 * @param lp List pointer to the start of the tokenlist.
 * @return a bool denoting whether the options were parsed successfully.
 */
bool parseOptions(List *lp) {
    //TODO: store each (*lp)->t as an option, if any exist
    // not entirely sure how to use this function so left it for now
    // will attempt again with next lab
    while (*lp != NULL && !isOperator((*lp)->t)) {
        (*lp) = (*lp)->next;
    }
    return true;
}

/**
 * The function parseRedirections parses a command according to the grammar:
 *
 * <command>        ::= <executable> <options>
 *
 * @param lp List pointer to the start of the tokenlist.
 * @return a bool denoting whether the command was parsed successfully.
 */
bool parseCommand(List *lp) {
    return parseExecutable(lp) && parseOptions(lp);
}

/**
 * The function parsePipeline parses a pipeline according to the grammar:
 *
 * <pipeline>           ::= <command> "|" <pipeline>
 *                       | <command>
 *
 * @param lp List pointer to the start of the tokenlist.
 * @return a bool denoting whether the pipeline was parsed successfully.
 */
bool parsePipeline(List *lp) {
    if (!parseCommand(lp)) {
        return false;
    }


    if (acceptToken(lp, "|")) {
        return parsePipeline(lp);
    }

    return true;
}

/**
 * The function parseRedirections parses redirections according to the grammar:
 *
 * <redirections>       ::= <pipeline> <redirections>
 *                       |  <builtin> <options>
 *
 * @param lp List pointer to the start of the tokenlist.
 * @return a bool denoting whether the redirections were parsed successfully.
 */
bool parseRedirections(List *lp) {
    if (isEmpty(*lp)) {
        return true;
    }

    if (acceptToken(lp, "<")) {
        if (!parseFileName(lp,0)) return false;
        if (acceptToken(lp, ">")) return parseFileName(lp,0);
        else return true;
    } else if (acceptToken(lp, ">")) {
        if (!parseFileName(lp,0)) return false;
        if (acceptToken(lp, "<")) return parseFileName(lp,0);
        else return true;
    }
    return true;
}

/**
 * The function parseBuiltIn parses a builtin.
 * @param lp List pointer to the start of the tokenlist.
 * @return a bool denoting whether the builtin was parsed successfully.
 */
bool parseBuiltIn(List *lp) {
    // NULL-terminated array makes it easy to expand this array later
    // without changing the code at other places.
    char *builtIns[] = {
            "exit",
            "status",
            NULL
    };

    for (int i = 0; builtIns[i] != NULL; i++) {
        if (acceptToken(lp, builtIns[i])) return true;
    }

    return false;
}

/**
 * The function parseChain parses a chain according to the grammar:
 *
 * <chain>              ::= <pipeline> <redirections>
 *                       |  <builtin> <options>
 *
 * @param lp List pointer to the start of the tokenlist.
 * @return a bool denoting whether the chain was parsed successfully.
 */
bool parseChain(List *lp) {
    if (parseBuiltIn(lp)) {
        return parseOptions(lp);
    }
    if (parsePipeline(lp)) {
        return parseRedirections(lp);
    }
    return false;
}

/**
 * The function parseInputLine parses an inputline according to the grammar:
 *
 * <inputline>      ::= <chain> & <inputline>
 *                   | <chain> && <inputline>
 *                   | <chain> || <inputline>
 *                   | <chain> ; <inputline>
 *                   | <chain>
 *                   | <empty>
 *
 * @param lp List pointer to the start of the tokenlist.
 * @return a bool denoting whether the inputline was parsed successfully.
 */
bool parseInputLine(List *lp) {
    if (isEmpty(*lp)) {
        return true;
    }

    if (!parseChain(lp)) {
        return false;
    }

    if (acceptToken(lp, "&") || acceptToken(lp, "&&")) {
        return parseInputLine(lp);
    } else if (acceptToken(lp, "||")) {
        return parseInputLine(lp);
    } else if (acceptToken(lp, ";")) {
        return parseInputLine(lp);
    }
    return true;
}
