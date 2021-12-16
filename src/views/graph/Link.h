#pragma once

#include "views/graph/Node.h"

struct Link {
    Node*   input_node;
    int     index_input_pad;
    Node*   output_node;
    int     index_output_pad;
    static int cursor_pos_x, cursor_pos_y;

    Link(Node* input_node, int index_input_pad, Node* output_node, int index_output_pad);

    void draw(const Cairo::RefPtr<Cairo::Context>& cr) const;
    void select();
    static void updateCursorPos(int new_x, int new_y);

};
