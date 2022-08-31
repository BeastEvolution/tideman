#include <stdio.h>
#include <cs50.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

typedef struct node
{
    int value;
    struct node *next;
}
node;

void add_node(node **list, int value);
int valueOfNode(node *list, int IndexOfNode, int *value);


int main(void)
{
    node *list = NULL;

    add_node(&list, 1);
    add_node(&list, 2);
    int nodeValue;
    if (valueOfNode(list, 1, &nodeValue) != 0)
    {
        return 1;
    }


    printf("%i\n", nodeValue);
}

//Adds a node to the list
void add_node(node **list, int value)
{
    if(*list == NULL)
    {
        node new_node;
        new_node.value = value;
        new_node.next = NULL;
        *list = &new_node;
    }
    else
    {
        node *curent_node = *list;
        node new_node;
        new_node.value = value;
        new_node.next = NULL;
        while(true)
        {
            if ((*curent_node).next == NULL)
            {
                break;
            }
            curent_node = (*curent_node).next;
        }
        (*curent_node).next = &new_node;

    }
}

int valueOfNode(node *list, int IndexOfNode, int *value)
{
    node *current_node = list;

    if (list == NULL)
    {
        // List is empty.
        return 1;
    }
    for (int i = 0; i < IndexOfNode; i++)
    {
        if ((*current_node).next == NULL)
        {
            // There is no such node in the list(The list is shorter).
            return 2;
        }
        current_node = (*current_node).next;
    }

    *value = (*current_node).value;
    // Value of the node was written in variable value.
    return 0;
}
