#ifndef PLOTVIEW_HPP
#define PLOTVIEW_HPP

#include <gtkmm/drawingarea.h>

#include "model.hpp"

class PlotView : public Gtk::DrawingArea
{
public:
  PlotView(Model& model_in);
  virtual ~PlotView();

protected:
  //Override default signal handler:
  bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

  bool on_timeout();

  double m_radius;
  double m_line_width;

  Model* model;

};

#include "plotview.impl.cpp"

#endif

