#pragma once

/* stack.h */

#ifndef STACK_H
#define STACK_H

#include "systemHelper.h"

typedef struct StackNode 
{
    DirPaths dirPaths;
    struct StackNode * next;
} 
StackNode;

StackNode * createStackNode(DirPaths dirPaths);
DirPaths pop(StackNode ** stack);
void push(StackNode ** stack, DirPaths dirPaths);
void clearStack();
int isStackEmpty(StackNode * stack);

#endif /* STACK_H */
