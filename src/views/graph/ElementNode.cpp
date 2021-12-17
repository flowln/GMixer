#include "views/PipelineEditor.h"
#include "views/graph/ElementNode.h"

ElementNode::ElementNode(GraphViewer* parent, const element_info* element, int x, int y)
    : Node(parent, element->name, x, y)
{
    delete element;
}

void ElementNode::select()
{
    switch(m_parent->getMode()){
    case(OperationMode::MODE_SELECT):
        break;
    case(OperationMode::MODE_MOVE):
        Node::select();
        break;
    }
}
