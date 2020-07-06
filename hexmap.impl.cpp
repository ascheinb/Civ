#include <ctime>
#include <cmath>
#include <cairomm/context.h>
#include <glibmm/main.h>

HexMap::HexMap(Model& model_in)
: m_radius(100), m_line_width(3)
{
  model = &(model_in); // Point the hexmap to the model
  Glib::signal_timeout().connect( sigc::mem_fun(*this, &HexMap::on_timeout), 1000 );
}

HexMap::~HexMap()
{
}

void draw_hexagon(const Cairo::RefPtr<Cairo::Context>& cr, float hex_center_x, float hex_center_y){
    const float hex_radius=40;
    const float sin60 = 0.8660254;

    cr->set_line_width(3);
    cr->set_source_rgba(0.617, 0.137, 0.612, 0.9);   // purple
    cr->move_to(hex_center_x + 0.0f, hex_center_y + hex_radius); // Start at top (bottom?)
    cr->rel_line_to(hex_radius*sin60, -hex_radius*0.5f);
    cr->rel_line_to(0.0f,-hex_radius);
    cr->rel_line_to(-hex_radius*sin60, -hex_radius*0.5f);
    cr->rel_line_to(-hex_radius*sin60, hex_radius*0.5f);
    cr->rel_line_to(0.0f,hex_radius);
    cr->rel_line_to(hex_radius*sin60, hex_radius*0.5f);
    cr->stroke();
}

void draw_hexmap(const Cairo::RefPtr<Cairo::Context>& cr, Model& model){
    const float hex_radius=40;
    const float sin60 = 0.8660254;
    const float hex_width = hex_radius*sin60*2;
    for (int j = 0; j < model.nature.ncol; j++){
        float offset = (j%2==0 ? 0 : hex_width/2);
        for (int i = 0; i < model.nature.nrow; i++){
            draw_hexagon(cr, 100.0f + hex_width*i + offset, 100.0f + 1.5*hex_radius*j);
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
  cr->set_line_width(m_line_width);

  cr->save();
  cr->set_source_rgba(0.337, 0.612, 0.117, 0.9);   // green
  cr->paint();
  cr->restore();
  cr->arc(0, 0, m_radius, 0, 2 * M_PI);
  cr->save();
  cr->set_source_rgba(1.0, 1.0, 1.0, 0.8);
  cr->fill_preserve();
  cr->restore();
  cr->stroke_preserve();
//  cr->clip();

  //clock ticks
  for (int i = 0; i < 12; i++)
  {
    double inset = 0.05;

    cr->save();
    cr->set_line_cap(Cairo::LINE_CAP_ROUND);

    if(i % 3 != 0)
    {
      inset *= 0.8;
      cr->set_line_width(0.03);
    }

    cr->move_to(
      (m_radius - inset) * cos (i * M_PI / 6),
      (m_radius - inset) * sin (i * M_PI / 6));
    cr->line_to (
      m_radius * cos (i * M_PI / 6),
      m_radius * sin (i * M_PI / 6));
    cr->stroke();
    cr->restore(); /* stack-pen-size */
  }

  // store the current time
  time_t rawtime;
  time(&rawtime);
  struct tm * timeinfo = localtime (&rawtime);

  // compute the angles of the indicators of our clock
  double minutes = timeinfo->tm_min * M_PI / 30;
  double hours = timeinfo->tm_hour * M_PI / 6;
  double seconds= timeinfo->tm_sec * M_PI / 30;

  cr->save();
  cr->set_line_cap(Cairo::LINE_CAP_ROUND);

  // draw the seconds hand
  cr->save();
  cr->set_line_width(m_line_width / 3);
  cr->set_source_rgba(0.7, 0.7, 0.7, 0.8); // gray
  cr->move_to(0, 0);
  cr->line_to(sin(seconds) * (m_radius * 0.9),
    -cos(seconds) * (m_radius * 0.9));
  cr->stroke();
  cr->restore();

  // draw the minutes hand
  cr->set_source_rgba(0.117, 0.337, 0.612, 0.9);   // blue
  cr->move_to(0, 0);
  cr->line_to(sin(minutes + seconds / 60) * (m_radius * 0.8),
    -cos(minutes + seconds / 60) * (m_radius * 0.8));
  cr->stroke();

  draw_hexmap(cr, *model);

  // draw the hours hand
  cr->set_source_rgba(0.337, 0.612, 0.117, 0.9);   // green
  cr->move_to(0, 0);
  cr->line_to(sin(hours + minutes / 12.0) * (m_radius * 0.5),
    -cos(hours + minutes / 12.0) * (m_radius * 0.5));
  cr->stroke();
  cr->restore();

  // draw a little dot in the middle
  cr->arc(0, 0, m_line_width / 3.0, 0, 2 * M_PI);
  cr->fill();

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

