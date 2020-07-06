#include <ctime>
#include <cmath>
#include <cairomm/context.h>
#include <glibmm/main.h>

HexMap::HexMap(Model& model_in)
: m_radius(100), m_line_width(3)
{
  model = &(model_in); // Point the hexmap to the model
  Glib::signal_timeout().connect( sigc::mem_fun(*this, &HexMap::on_timeout), 100 );
}

HexMap::~HexMap()
{
}

void draw_hexagon(const Cairo::RefPtr<Cairo::Context>& cr, float hex_center_x, float hex_center_y){
    const float hex_radius=30;
    const float sin60 = 0.8660254;

    cr->set_line_width(1);
    cr->set_source_rgba(0.0, 0.0, 0.0, 1.0);   // black
    cr->move_to(hex_center_x + 0.0f, hex_center_y + hex_radius); // Start at top (bottom?)
    cr->rel_line_to(hex_radius*sin60, -hex_radius*0.5f);
    cr->rel_line_to(0.0f,-hex_radius);
    cr->rel_line_to(-hex_radius*sin60, -hex_radius*0.5f);
    cr->rel_line_to(-hex_radius*sin60, hex_radius*0.5f);
    cr->rel_line_to(0.0f,hex_radius);
    cr->rel_line_to(hex_radius*sin60, hex_radius*0.5f);
//    cr->stroke_preserve();
}

void draw_mountains(const Cairo::RefPtr<Cairo::Context>& cr, float hex_center_x, float hex_center_y){
    const float hex_radius=30;
    cr->set_line_width(3);
    cr->set_source_rgba(103.0f/256, 95.0f/256, 82.0f/256, 1.0); // Brown
    cr->move_to(hex_center_x - hex_radius*0.7, hex_center_y + hex_radius*0.5); // Start at lower left
    cr->rel_line_to(hex_radius*0.4f, -hex_radius*0.8f); // mtn 1 up
    cr->rel_line_to(hex_radius*0.4f, hex_radius*0.8f); // mtn 1 down
    cr->rel_move_to(-hex_radius*0.1f, -hex_radius*0.2f); // go back a little bit
    cr->rel_line_to(hex_radius*0.3f, -hex_radius*0.6f); // mtn 2 up
    cr->rel_line_to(hex_radius*0.35f, hex_radius*0.7f); // mtn 2 down
    cr->rel_move_to(-hex_radius*0.35f, -hex_radius*0.7f); // go back to peak
    cr->rel_move_to(-hex_radius*0.1f, hex_radius*0.2f); // go back a little more
    cr->rel_line_to(-hex_radius*0.2f, -hex_radius*0.4f); // mtn 3 up (in reverse)
    cr->rel_line_to(-hex_radius*0.2f, hex_radius*0.4f); // mtn 3 down (in reverse)
    cr->stroke();
}

void draw_hexmap(const Cairo::RefPtr<Cairo::Context>& cr, Model& model){
    const float hex_radius=30;
    const float sin60 = 0.8660254;
    const float hex_width = hex_radius*sin60*2;

    // Determine # ppl/tile
    vector<int> tilepop(model.nature.map.size(),0);
    for (int i=0;i<model.p.person.size();i++){
        //if (group_focus==-1){
            tilepop[model.p.person[i].home]+=1;
        //} else { // Count only members of group_focus
        //    if (p.person[i].is_member(group_focus)) tilepop[p.person[i].home]+=1;
        //}
    }
    Cairo::TextExtents extents;

    for (int j = 0; j < model.nature.ncol; j++){
        float offset = (j%2==0 ? 0 : hex_width/2);
        for (int i = 0; i < model.nature.nrow; i++){
            int itile = j*model.nature.nrow + i;
            float tile_center_x = 50.0f + hex_width*i + offset;
            float tile_center_y = 50.0f + 1.5*hex_radius*j;
            draw_hexagon(cr, tile_center_x, tile_center_y);

            // Fill with correct background color
            if(model.nature.map[itile].terrain==GRASS) cr->set_source_rgba(167.0f/256, 206.0f/256, 164.0f/256, 1.0); // Green
            if(model.nature.map[itile].terrain==WATER) cr->set_source_rgba(177.0f/256, 219.0f/256, 238.0f/256, 1.0); // Blue
            if(model.nature.map[itile].terrain==MOUNTAIN) cr->set_source_rgba(206.0f/256, 191.0f/256, 164.0f/256, 1.0); // Brown
            cr->fill();

            // Draw mountains
            if(model.nature.map[itile].terrain==MOUNTAIN) draw_mountains(cr,tile_center_x, tile_center_y);

            // Write population
            if(model.nature.map[itile].terrain==GRASS && tilepop[itile]>0){
                //cr->select_font_face("Purisa", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_BOLD);
                cr->set_source_rgba(0.0, 0.0, 0.0, 1.0);
                cr->set_font_size(13);
                string pop_str = std::to_string(tilepop[itile]);
                cr->get_text_extents(pop_str, extents);
                cr->move_to(tile_center_x - extents.width/2, tile_center_y+hex_radius*0.1f);

                cr->show_text(pop_str.c_str());
            }
        }
    }
}

bool HexMap::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
  Gtk::Allocation allocation = get_allocation();
  const int width = 200;
  const int height = 200; //allocation.get_height()/2;

  // scale to unit square and translate (0, 0) to be (0.5, 0.5), i.e.
  // the center of the window
//  cr->scale(width, height);
//  cr->translate(0.5, 0.5);

//  cr->fill_preserve();
//  cr->restore();
//  cr->stroke_preserve();
//  cr->clip();


  draw_hexmap(cr, *model);

  return true;
}


bool HexMap::on_timeout()
{
    // force our program to redraw the entire clock.
    auto win = get_window();
    if (win)
    {
        Gdk::Rectangle r(0, 0, get_allocation().get_width(),
                get_allocation().get_height());
        win->invalidate_rect(r, false);
    }
    return true;
}

