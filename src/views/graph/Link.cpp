#include "views/graph/Link.h"

int Link::cursor_pos_x = 0;
int Link::cursor_pos_y = 0;

Link::Link(Node* input_node, int index_input_pad, Node* output_node, int index_output_pad)
    : input_node(input_node)
    , index_input_pad(index_input_pad)
    , output_node(output_node)
    , index_output_pad(index_output_pad)
    {}

void Link::draw(const Cairo::RefPtr<Cairo::Context>& cr) const
{
    cr->save();
    cr->set_source_rgba(0,0,0,0.8);

    if(!input_node){
        std::vector<double> dashes = {10.0};
        cr->set_dash(dashes, 0.0);

        auto position = output_node->IOPadToPosition(false, index_output_pad);
        cr->move_to(position.first, position.second);
        cr->line_to(cursor_pos_x, cursor_pos_y);
    }
    else if(!output_node){
        std::vector<double> dashes = {10.0};
        cr->set_dash(dashes, 0.0);

        auto position = input_node->IOPadToPosition(true, index_input_pad);
        cr->move_to(position.first, position.second);
        cr->line_to(cursor_pos_x, cursor_pos_y);
    }
    else{
        auto position_input = input_node->IOPadToPosition(true, index_input_pad);
        auto position_output = output_node->IOPadToPosition(false, index_output_pad);
        cr->move_to(position_input.first, position_input.second);
        cr->line_to(position_output.first, position_output.second);    
    }

    cr->stroke();

    cr->restore();
}
