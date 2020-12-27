#ifndef CIVWINDOW_HPP
#define CIVWINDOW_HPP

#include <gtkmm.h>
#include "modelthread.hpp"
#include "hexmap.hpp"
#include "yearview.hpp"
#include "plotview.hpp"
#include "setup_params.hpp"
#include "setup.hpp"

class CivWindow : public Gtk::Window
{
public:

  CivWindow(SetupParameters& setup_params_in);

  // Called from the worker thread.
  void notify();

protected:
  void setupWinClose();

  SetupParameters setup_params;

private:
  // Signal handlers.
  void on_next_button_clicked();
  void on_start_button_clicked();
  void on_stop_button_clicked();
  void on_quit_button_clicked();
  void on_rb0_clicked();
  void on_rb1_clicked();
  void on_rb2_clicked();
  void on_rb3_clicked();
  void on_rb4_clicked();
  bool on_map_clicked(GdkEventButton*);

  void update_start_stop_buttons();
  void update_widgets();

  // Dispatcher handler.
  void on_notification_from_worker_thread();

  // Member data.
  Gtk::Box m_VBox;
  Gtk::Box m_HBox;
  Gtk::EventBox m_MapBox;
  Gtk::Box m_SideVBox;
  Gtk::ButtonBox m_ButtonBox;
  Gtk::Button m_ButtonNext;
  Gtk::Button m_ButtonStart;
  Gtk::Button m_ButtonStop;
  Gtk::Button m_ButtonQuit;
  Gtk::ProgressBar m_ProgressBar;
  Gtk::ScrolledWindow m_ScrolledWindow;
  Gtk::TextView m_TextView;
  HexMap m_HexMap;
  YearView m_YearView;
  PlotView m_PlotView;
  Gtk::RadioButton m_rb0, m_rb1, m_rb2, m_rb3, m_rb4;

  Glib::Dispatcher m_Dispatcher;
  ModelThread m_Worker;
  std::thread* m_WorkerThread;

  SetupWindow* setupw_;
};

#include "modelthread.impl.cpp"
#include "civwindow.impl.cpp"

#endif
