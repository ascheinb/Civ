#ifndef CIVWINDOW_HPP
#define CIVWINDOW_HPP

#include <gtkmm.h>
#include "modelthread.hpp"
#include "hexmap.hpp"
#include "yearview.hpp"
#include "plotview.hpp"
#include "setup_params.hpp"
#include "setup.hpp"
#include "pginfo.hpp"

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
  bool on_play_timeout();
  void on_Play0_clicked();
  void on_Play1_clicked();
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
  Gtk::Notebook m_Notebook;
  Gtk::Box m_StatsBox;
  Gtk::Box m_PlayBox;
  Gtk::Box m_ButtonBox;
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

  // Play interface
  Gtk::TextView m_PlayText;
  Glib::RefPtr<Gtk::TextBuffer> m_PlayTextBuffer;
  Gtk::RadioButton m_Play0, m_Play1;

  // Slider
  Glib::RefPtr<Gtk::Adjustment> m_hadjustment, m_hadjustment_digits, m_hadjustment_pagesize;
  Gtk::Scale m_HScale, m_Scale_Digits, m_Scale_PageSize;
  Gtk::Scrollbar m_Scrollbar;

  Glib::Dispatcher m_Dispatcher;
  ModelThread m_Worker;
  std::thread* m_WorkerThread;

  SetupWindow* setupw_;
  PGInfoWindow* pginfow_;
};

#include "modelthread.impl.cpp"
#include "civwindow.impl.cpp"

#endif
