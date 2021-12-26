#include <sstream>
#include <chrono>

ModelThread::ModelThread(SetupParameters& setup_params_in) :
  m_Mutex(),
  m_shall_stop(false),
  m_has_stopped(false),
  m_fraction_done(0.0),
  m_message(),
  model(setup_params_in)
{
}

// Accesses to these data are synchronized by a mutex.
// Some microseconds can be saved by getting all data at once, instead of having
// separate get_fraction_done() and get_message() methods.
void ModelThread::get_data(double* fraction_done, Glib::ustring* message) const
{
  std::lock_guard<std::mutex> lock(m_Mutex);

  if (fraction_done)
    *fraction_done = m_fraction_done;

  if (message)
    *message = m_message;
}

void ModelThread::stop_work()
{
  std::lock_guard<std::mutex> lock(m_Mutex);
  m_shall_stop = true;
}

bool ModelThread::has_stopped() const
{
  std::lock_guard<std::mutex> lock(m_Mutex);
  return m_has_stopped;
}

void ModelThread::do_one_turn(CivWindow* caller)
{
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_has_stopped = false;
    m_message = "";
  } // The mutex is unlocked here by lock's destructor.

      model.advance();
      model.i_turn++;
      if (model.p.person.size()==0) {printf("\nEnding simulation early...");}

    {
      std::lock_guard<std::mutex> lock(m_Mutex);

      m_fraction_done = float(model.i_turn)/float(model.n_turns);

    }

    caller->notify();

  // Finished
  if (model.i_turn > model.n_turns) model.conclusions();

  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_shall_stop = false;
    m_has_stopped = true;
  }

  caller->notify();
}

void ModelThread::do_work(CivWindow* caller)
{
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_has_stopped = false;
    m_fraction_done = 0.0;
    m_message = "";
  } // The mutex is unlocked here by lock's destructor.

  int next_milestone=0;
  int milestone_dist=5;

  // Advance to next turn
  while (model.i_turn <= model.n_turns)
  {
      model.advance();
      model.i_turn++;
      if (model.p.person.size()==0) {printf("\nEnding simulation early..."); break;}
//    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    {
      std::lock_guard<std::mutex> lock(m_Mutex);

      m_fraction_done = float(model.i_turn)/float(model.n_turns);

      if (int(100*m_fraction_done) >= next_milestone)
      {
        std::ostringstream ostr;
        ostr << next_milestone << "% done\n";
        m_message += ostr.str();
        next_milestone+=milestone_dist;
      }

      if (m_fraction_done >= 1.0)
      {
        m_message += "Finished";
        break;
      }
      if (m_shall_stop)
      {
        m_message += "Stopped";
        break;
      }
    }

    caller->notify();

  }

  // Finished
  if (model.i_turn > model.n_turns) model.conclusions();

  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_shall_stop = false;
    m_has_stopped = true;
  }

  caller->notify();
}
