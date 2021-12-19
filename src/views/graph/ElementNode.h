#pragma once

#include "views/graph/Node.h"

#include <gst/gstpad.h>

//Forward-declaration
class Element;

class ElementNode : public Node {
    public:
        static ElementNode* create(GraphViewer* parent, Element* element, int x, int y);
        static ElementNode* create(GraphViewer* parent, const gchar* element_name, int x, int y);

        void updateNode();

        bool operator==(Element*);

        virtual void select() override;
        virtual void deselect() override;

        Element* getElement() { return m_element; };
        Pad* searchForPeer(GstPad*);

    protected:
        ElementNode(GraphViewer* parent, Element* element, int x, int y);
    
    private:
        Element* m_element;
};
