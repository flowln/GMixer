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

        virtual GMixer::PropertyList* getProperties() override;
        virtual bool updateProperty(GMixer::Property*, const std::string&, const std::string&) override;

        Element* getElement() { return m_element; };
        Pad* searchForPeer(GstPad*);

    protected:
        ElementNode(GraphViewer* parent, Element* element, int x, int y);
    
    private:
        Element* m_element;
};
