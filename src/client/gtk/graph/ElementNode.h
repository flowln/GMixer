#pragma once

#include "client/gtk/graph/Node.h"
#include "gstreamer/Element.h"

#include <gst/gstpad.h>

class ElementNode : public Node {
   public:
    static ElementNode* create(GraphViewer* parent, Element* element, int x, int y);
    static ElementNode* create(GraphViewer* parent, const gchar* element_name, int x, int y);

    ~ElementNode();

    void updateNode();

    bool operator==(Element*);

    std::vector<Element::Property*>& getProperties();

    Element* getElement() { return m_element; };
    Pad* searchForPeer(GstPad*);

   protected:
    ElementNode(GraphViewer* parent, Element* element, int x, int y);

   private:
    Element* m_element;
};
