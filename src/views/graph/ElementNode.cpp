#include "views/graph/ElementNode.h"

ElementNode::ElementNode(const element_info* element, int x, int y)
    : Node(element->name, x, y)
{
    delete element;
}

void ElementNode::select()
{
    printf("hi!\n");
}
