#pragma once

#include "views/graph/Node.h"

//Forward-declaration
class Element;

class ElementNode : public Node {
    public:
        ElementNode(GraphViewer* parent, Element* element, int x, int y);
        ElementNode(GraphViewer* parent, const gchar* element_name, int x, int y);
        virtual void select() override;
        virtual void deselect() override;

        Element* getElement() { return m_element; };
    
    private:
        Element* m_element;
};
