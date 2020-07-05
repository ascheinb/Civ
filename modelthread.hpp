#ifndef MODELTHREAD_HPP
#define MODELTHREAD_HPP

#include <gtkmm.h>
#include <thread>
#include <mutex>
#include "model.hpp"

class CivWindow;

class ModelThread
{
public:

  ModelThread(int initial_n_ppl, int n_years, float min_food_gen, float max_food_gen, int climate_type, int mapsize, int mapwidth);

  // Thread function.
  void do_work(CivWindow* caller);

  void get_data(double* fraction_done, Glib::ustring* message) const;
  void stop_work();
  bool has_stopped() const;

private:
  // Synchronizes access to member data.
  mutable std::mutex m_Mutex;

  // Data used by both GUI thread and worker thread.
  bool m_shall_stop;
  bool m_has_stopped;
  double m_fraction_done;
  Glib::ustring m_message;

  Model model;
};

#endif
