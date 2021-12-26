#ifndef SETUP_HPP
#define SETUP_HPP

#include <gtkmm.h>

#include "setup_params.hpp"
#include "model.hpp"

class SetupWindow : public Gtk::Window
{
public:
  SetupWindow(SetupParameters& setup_params_in, Model& model_in);
  virtual ~SetupWindow();

protected:
  //Signal handlers:
  void on_checkbutton_toggled();
  void on_combo_position();
  void on_adjustment1_value_changed();
  void on_adjustment2_value_changed();
  void on_button_quit();

  //Child widgets:
  Gtk::Box m_VBox_Top, m_VBox2, m_VBox_HScale;
  Gtk::Box m_HBox_Scales, m_HBox_Combo, m_HBox_Digits, m_HBox_PageSize;

  Glib::RefPtr<Gtk::Adjustment> m_hadjustment, m_hadjustment_digits, m_hadjustment_pagesize;
  Glib::RefPtr<Gtk::Adjustment> m_vadjustment, m_vadjustment_digits, m_vadjustment_pagesize;

  Gtk::Scale m_VScale;
  Gtk::Scale m_HScale, m_Scale_Digits, m_Scale_PageSize;

  Gtk::Separator m_Separator;

  Gtk::CheckButton m_CheckButton;

  Gtk::Scrollbar m_Scrollbar;

  //Tree model columns:
  class ModelColumns : public Gtk::TreeModel::ColumnRecord
  {
  public:

    ModelColumns()
    { add(m_col_position_type); add(m_col_title); }

    Gtk::TreeModelColumn<Gtk::PositionType> m_col_position_type;
    Gtk::TreeModelColumn<Glib::ustring> m_col_title;
  };

  ModelColumns m_Columns;

  //Child widgets:
  Gtk::ComboBox m_ComboBox_Position;
  Glib::RefPtr<Gtk::ListStore> m_refTreeModel;

  Gtk::Button m_Button_Quit;

private:
  SetupParameters* setup_params;
  Model* model;
};

#include "setup.impl.cpp"

#endif
