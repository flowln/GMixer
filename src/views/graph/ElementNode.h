#pragma once

#include "backend/ElementUtils.h"
#include "views/graph/Node.h"

class ElementNode : public Node {
    public:
        ElementNode(const element_info* element, int x, int y);
        virtual void select() override;
};
