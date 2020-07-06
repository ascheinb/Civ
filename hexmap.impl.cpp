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

void draw_hexline(const Cairo::RefPtr<Cairo::Context>& cr, float hex_center_x, float hex_center_y, Direction direction){
    const float hex_radius=29;
    const float sin60 = 0.8660254;
    const int linewidth = 2;

    cr->set_line_width(linewidth);
    cr->set_source_rgba(0.5, 0.1, 0.6, 0.4);   // purple
    cr->move_to(hex_center_x + 0.0f, hex_center_y + hex_radius); // Start at top (bottom?)
    if (direction==SE) cr->rel_line_to(hex_radius*sin60, -hex_radius*0.5f);
    else cr->rel_move_to(hex_radius*sin60, -hex_radius*0.5f);
    if (direction==E) cr->rel_line_to(0.0f,-hex_radius);
    else cr->rel_move_to(0.0f,-hex_radius);
    if (direction==NE) cr->rel_line_to(-hex_radius*sin60, -hex_radius*0.5f);
    else cr->rel_move_to(-hex_radius*sin60, -hex_radius*0.5f);
    if (direction==NW) cr->rel_line_to(-hex_radius*sin60, hex_radius*0.5f);
    else cr->rel_move_to(-hex_radius*sin60, hex_radius*0.5f);
    if (direction==W) cr->rel_line_to(0.0f,hex_radius);
    else cr->rel_move_to(0.0f,hex_radius);
    if (direction==SW) cr->rel_line_to(hex_radius*sin60, hex_radius*0.5f);
    else cr->rel_move_to(hex_radius*sin60, hex_radius*0.5f);
    cr->stroke();
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

void draw_waves(const Cairo::RefPtr<Cairo::Context>& cr, float hex_center_x, float hex_center_y){
    const float hex_radius=30;
    cr->set_line_width(3);
    cr->set_source_rgba(88.0f/256, 110.0f/256, 119.0f/256, 0.3); // Blue
    cr->move_to(hex_center_x - hex_radius*0.5, hex_center_y + hex_radius*0.5); // Start at lower left
    cr->rel_curve_to(0.25*hex_radius, -0.15*hex_radius, 0.25*hex_radius, 0.1*hex_radius, 0.5*hex_radius, -0.05*hex_radius);
    cr->move_to(hex_center_x + hex_radius*0.1, hex_center_y - hex_radius*0.25); // Start at upper right
    cr->rel_curve_to(0.25*hex_radius, -0.15*hex_radius, 0.25*hex_radius, 0.1*hex_radius, 0.5*hex_radius, -0.05*hex_radius);

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

    Cairo::TextExtents extents; // for centering text

    for (int j = 0; j < model.nature.nrow; j++){
        float offset = (j%2==0 ? 0 : hex_width/2);
        for (int i = 0; i < model.nature.ncol; i++){
            int itile = j*model.nature.ncol + i;
            float tile_center_x = 50.0f + hex_width*i + offset;
            float tile_center_y = 50.0f + 1.5*hex_radius*j;
            draw_hexagon(cr, tile_center_x, tile_center_y);

            // Fill with correct background color
            if(model.nature.map[itile].terrain==GRASS) cr->set_source_rgba(167.0f/256, 206.0f/256, 164.0f/256, 1.0); // Green
            if(model.nature.map[itile].terrain==WATER) cr->set_source_rgba(177.0f/256, 219.0f/256, 238.0f/256, 1.0); // Blue
            if(model.nature.map[itile].terrain==MOUNTAIN) cr->set_source_rgba(206.0f/256, 191.0f/256, 164.0f/256, 1.0); // Brown
            cr->fill();

            // Draw mountains or waves
            if(model.nature.map[itile].terrain==MOUNTAIN) draw_mountains(cr,tile_center_x, tile_center_y);
            if(model.nature.map[itile].terrain==WATER) draw_waves(cr,tile_center_x, tile_center_y);

            // Write population
            if(model.nature.map[itile].terrain==GRASS && tilepop[itile]>0){
                //cr->select_font_face("Purisa", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_BOLD);
                cr->set_source_rgba(0.0, 0.0, 0.0, 1.0);
                cr->set_font_size(13);
                string pop_str = std::to_string(tilepop[itile]);
                cr->get_text_extents(pop_str, extents);
                cr->move_to(tile_center_x - extents.width/2, tile_center_y+hex_radius*0.6f);

                cr->show_text(pop_str.c_str());
            }
/*
            // Draw population
            for (int i_pop = 0; i_pop < tilepop[itile]; i_pop++){
                // Choose a random point inside hexagon (the square part for now)
                float rand_x = (rand_f1()-0.5f)*hex_radius;
                float rand_y = (rand_f1()-0.5f)*hex_radius;
                cr->set_line_width(5);
                cr->set_source_rgba(0.0, 0.0, 0.0, 1.0);   // black
//                cr->move_to(tile_center_x + rand_x, tile_center_y + rand_y); // Start at top (bottom?)
//                cr->rel_line_to(hex_radius*0.01, 0);
                cr->arc(tile_center_x+rand_x, tile_center_y+rand_y,  1.0, 0, 2 * M_PI);
                cr->fill();
            }*/
        }
    }

    for (int j = 0; j < model.nature.nrow; j++){
        float offset = (j%2==0 ? 0 : hex_width/2);
        for (int i = 0; i < model.nature.ncol; i++){
            int itile = j*model.nature.ncol + i;
            float tile_center_x = 50.0f + hex_width*i + offset;
            float tile_center_y = 50.0f + 1.5*hex_radius*j;

            // Add borders
            int gid = model.nature.map[itile].owner;
            if (gid>=0){
                bool ul = true;
                int neighbor_tile = model.nature.neighbor(itile,W);
                if (neighbor_tile>=0){
                    if (model.nature.map[neighbor_tile].owner!=gid) draw_hexline(cr,tile_center_x, tile_center_y,W);
                    else ul = false;
                } else draw_hexline(cr,tile_center_x, tile_center_y,W);
                
                neighbor_tile = model.nature.neighbor(itile,NW);
                if (neighbor_tile>=0){
                    if (model.nature.map[neighbor_tile].owner!=gid) draw_hexline(cr,tile_center_x, tile_center_y,NW);
                    else ul = false;
                } else draw_hexline(cr,tile_center_x, tile_center_y,NW);
                
                neighbor_tile = model.nature.neighbor(itile,NE);
                if (neighbor_tile>=0){
                    if (model.nature.map[neighbor_tile].owner!=gid) draw_hexline(cr,tile_center_x, tile_center_y,NE);
                    else ul = false;
                } else draw_hexline(cr,tile_center_x, tile_center_y,NE);
                
                neighbor_tile = model.nature.neighbor(itile,E);
                if (neighbor_tile>=0){
                    if (model.nature.map[neighbor_tile].owner!=gid) draw_hexline(cr,tile_center_x, tile_center_y,E);
                } else draw_hexline(cr,tile_center_x, tile_center_y,E);
                
                neighbor_tile = model.nature.neighbor(itile,SE);
                if (neighbor_tile>=0){
                    if (model.nature.map[neighbor_tile].owner!=gid) draw_hexline(cr,tile_center_x, tile_center_y,SE);
                } else draw_hexline(cr,tile_center_x, tile_center_y,SE);
                
                neighbor_tile = model.nature.neighbor(itile,SW);
                if (neighbor_tile>=0){
                    if (model.nature.map[neighbor_tile].owner!=gid) draw_hexline(cr,tile_center_x, tile_center_y,SW);
                } else draw_hexline(cr,tile_center_x, tile_center_y,SW);

                if (ul){ // Quick way to clean up names - only put name in upper left corner tile
                    cr->set_source_rgba(0.0, 0.0, 0.0, 1.0);
                    cr->set_font_size(13);
                    string group_name = gnames[model.p.groups[gid].name];
                    cr->get_text_extents(group_name, extents);
                    cr->move_to(tile_center_x - extents.width/2, tile_center_y+hex_radius*0.0f);
                    cr->show_text(group_name.c_str());
                }
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

