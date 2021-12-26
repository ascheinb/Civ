#include <ctime>
#include <cmath>
#include <cairomm/context.h>
#include <glibmm/main.h>

YearView::YearView(Model& model_in)
: m_radius(100), m_line_width(3)
{
  model = &(model_in); // Point the hexmap to the model
  Glib::signal_timeout().connect( sigc::mem_fun(*this, &YearView::on_timeout), 100 );
}

YearView::~YearView()
{
}

bool YearView::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
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


    cr->select_font_face("Purisa", Cairo::ToyFontFace::Slant::NORMAL, Cairo::ToyFontFace::Weight::BOLD);

    // Year
    cr->set_source_rgba(0.0, 0.0, 0.0, 1.0);
    cr->set_font_size(36);
    string pop_str = "Year " + std::to_string((model->i_turn-1)/4+1);
    cr->move_to(100,100);
    cr->show_text(pop_str.c_str());

    // Seasons
    int season = model->i_turn%4;
    float w_col = .8;
    float sp_col = .8;
    float su_col = .8;
    float f_col = .8;
    if (season==1) w_col = 0.2;
    if (season==2) sp_col = 0.2;
    if (season==3) su_col = 0.2;
    if (season==0) f_col = 0.2;
    cr->set_font_size(20);

    cr->set_source_rgba(w_col, w_col, .8, 1.0);
    cr->move_to(30,150);
    cr->show_text("Winter");

    cr->set_source_rgba(sp_col, sp_col, .8, 1.0);
    cr->move_to(105,150);
    cr->show_text("Spring");

    cr->set_source_rgba(su_col, su_col, .8, 1.0);
    cr->move_to(180,150);
    cr->show_text("Summer");

    cr->set_source_rgba(f_col, f_col, .8, 1.0);
    cr->move_to(275,150);
    cr->show_text("Fall");

    // Population

    cr->set_source_rgba(0.0, 0.0, 0.0, 1.0);
    cr->set_font_size(36);
    pop_str = "Population: " + std::to_string(model->p.person.size());
    cr->move_to(50,250);
    cr->show_text(pop_str.c_str());

  return true;
}


bool YearView::on_timeout()
{
    // force our program to redraw the entire clock.
    queue_draw();
    /*auto win = get_window();
    if (win)
    {
        Gdk::Rectangle r(0, 0, get_allocation().get_width(),
                get_allocation().get_height());
        win->invalidate_rect(r, false);
    }*/
    return true;
}

