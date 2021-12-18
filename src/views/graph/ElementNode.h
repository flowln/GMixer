#pragma once

#include "views/graph/Node.h"

//Forward-declaration
struct element_info;

class ElementNode : public Node {
    public:
        ElementNode(GraphViewer* parent, const element_info* element, int x, int y);
        virtual void select() override;
        virtual void deselect() override;
};
