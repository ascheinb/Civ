#ifndef YEARVIEW_HPP
#define YEARVIEW_HPP

#include <gtkmm/drawingarea.h>

#include "model.hpp"

class YearView : public Gtk::DrawingArea
{
public:
  YearView(Model& model_in);
  virtual ~YearView();

protected:
  //Override default signal handler:
  bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);

  bool on_timeout();

  double m_radius;
  double m_line_width;

  Model* model;

};

#include "yearview.impl.cpp"

#endif

