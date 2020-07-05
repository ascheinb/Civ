#ifndef CIVWINDOW_HPP
#define CIVWINDOW_HPP

#include <gtkmm.h>
#include "modelthread.hpp"

class CivWindow : public Gtk::Window
{
public:

  CivWindow(int initial_n_ppl, int n_years, float min_food_gen, float max_food_gen, int climate_type, int mapsize, int mapwidth);

  // Called from the worker thread.
  void notify();

private:
  // Signal handlers.
  void on_start_button_clicked();
  void on_stop_button_clicked();
  void on_quit_button_clicked();

  void update_start_stop_buttons();
  void update_widgets();

  // Dispatcher handler.
  void on_notification_from_worker_thread();

  // Member data.
  Gtk::Box m_VBox;
  Gtk::ButtonBox m_ButtonBox;
  Gtk::Button m_ButtonStart;
  Gtk::Button m_ButtonStop;
  Gtk::Button m_ButtonQuit;
  Gtk::ProgressBar m_ProgressBar;
  Gtk::ScrolledWindow m_ScrolledWindow;
  Gtk::TextView m_TextView;

  Glib::Dispatcher m_Dispatcher;
  ModelThread m_Worker;
  std::thread* m_WorkerThread;
};

#include "modelthread.impl.cpp"
#include "civwindow.impl.cpp"

#endif
