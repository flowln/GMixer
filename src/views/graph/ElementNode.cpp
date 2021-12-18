#include "backend/ElementUtils.h"
#include "views/PipelineEditor.h"
#include "views/graph/ElementNode.h"
#include "views/graph/GraphViewer.h"

ElementNode::ElementNode(GraphViewer* parent, const element_info* element, int x, int y)
    : Node(parent, element->name, x, y)
{
    delete element;
}

void ElementNode::select()
{
    Node::select();

    switch(m_parent->getMode()){
    case(OperationMode::MODE_SELECT):
        //TODO: Open property tab on the side
        break;
    case(OperationMode::MODE_MOVE):
        break;
    }
}

void ElementNode::deselect()
{
    Node::deselect();

    switch(m_parent->getMode()){
    case(OperationMode::MODE_SELECT):
        //TODO: Close property tab on the side
        break;
    case(OperationMode::MODE_MOVE):
        break;
    }
}
