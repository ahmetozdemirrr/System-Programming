#include "stack.h"
#include "systemHelper.h"

StackNode * createStackNode(DirPaths dirPaths)
{
    StackNode * newNode = (StackNode *)malloc(sizeof(StackNode));

    if (newNode == NULL)
    {
        perror("Failed to allocate memory for stack node");
        exit(EXIT_FAILURE);
    }
    newNode->dirPaths = dirPaths;
    newNode->next = NULL;

    return newNode;
}

void push(StackNode ** stack, DirPaths dirPaths)
{
    StackNode * newNode = createStackNode(dirPaths);

    newNode->next = *stack;
    *stack = newNode;
}

DirPaths pop(StackNode ** stack)
{
    if (isStackEmpty(*stack))
    {
        perror("Attempt to pop from empty stack");
        exit(EXIT_FAILURE);
    }

    StackNode * temp = *stack;
    DirPaths dirPaths = temp->dirPaths;

    *stack = (*stack)->next;
    free(temp);

    return dirPaths;
}

int isStackEmpty(StackNode *stack)
{
    return stack == NULL;
}

void clearStack(StackNode **stack)
{
    while (!isStackEmpty(*stack))
    {
        pop(stack);
    }
}
