#ifndef HEXMAP_HPP
#define HEXMAP_HPP

#include <gtkmm/drawingarea.h>

#include "model.hpp"

class HexMap : public Gtk::DrawingArea
{
public:
  HexMap(Model& model_in);
  virtual ~HexMap();

protected:
  //Override default signal handler:
  bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

  bool on_timeout();

  double m_radius;
  double m_line_width;

  Model* model;

};

#include "hexmap.impl.cpp"

#endif

