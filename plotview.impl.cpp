#include <ctime>
#include <cmath>
#include <cairomm/context.h>
#include <glibmm/main.h>

PlotView::PlotView(Model& model_in)
: m_radius(100), m_line_width(3)
{
  model = &(model_in); // Point the hexmap to the model
  Glib::signal_timeout().connect( sigc::mem_fun(*this, &PlotView::on_timeout), 100 );
}

PlotView::~PlotView()
{
}

void draw_hexagon_plot(const Cairo::RefPtr<Cairo::Context>& cr, float hex_center_x, float hex_center_y){
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

void draw_hexline_plot(const Cairo::RefPtr<Cairo::Context>& cr, float hex_center_x, float hex_center_y, Direction direction){
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

void draw_plot(const Cairo::RefPtr<Cairo::Context>& cr, Model& model){
    const float plot_height = 120;
    const float top_padding = 30;
    cr->set_line_width(1);
    cr->set_source_rgba(0.0, 0.0, 0.0, 1.0);   // black

    // x-axis
    cr->move_to(0,plot_height+top_padding);
    cr->rel_line_to(330,0);
    cr->stroke();

    float maxheight=0.0f;
    for (int i=0;i<model.plot_fracs.size();i++){
        if (model.plot_fracs[i]>maxheight) maxheight=model.plot_fracs[i];
    }

    cr->move_to(0,plot_height+top_padding);
    for (int i = 0; i<model.plot_fracs.size(); i++){
        cr->rel_line_to(0,-plot_height*model.plot_fracs[i]/maxheight);
        cr->rel_line_to(10,0);
        cr->rel_line_to(0,plot_height*model.plot_fracs[i]/maxheight);
    }
    cr->stroke();

    Cairo::TextExtents extents; // for centering text
    //cr->select_font_face("Purisa", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_BOLD);
    cr->set_font_size(13);
    string pop_str = std::to_string(model.plot_avg);
    pop_str = pop_str.substr(0,4);
    cr->get_text_extents(pop_str, extents);
    cr->move_to(10*model.plot_avg - extents.width/2, plot_height + top_padding + 20);
    cr->show_text(pop_str.c_str());

    // Line marker
    cr->set_source_rgba(0.6, 0.0, 0.0, 1.0);
    cr->move_to(10*model.plot_avg, plot_height+ top_padding);
    cr->rel_line_to(0,8);
    cr->stroke();

    cr->set_source_rgba(0.0, 0.0, 0.0, 1.0);   // black
    //cr->get_text_extents("Extroversion", extents);
    //cr->move_to(265 - extents.width/2, plot_height + 20);
    cr->move_to(10, plot_height + top_padding + 20);
    cr->show_text("Extroversion");
}

bool PlotView::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
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


  draw_plot(cr, *model);

  return true;
}


bool PlotView::on_timeout()
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

