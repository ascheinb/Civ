#include <iostream>

CivWindow::CivWindow(int initial_n_ppl, int n_years, float min_food_gen, float max_food_gen, int climate_type, int mapsize, int mapwidth) :
  m_VBox(Gtk::ORIENTATION_VERTICAL, 5),
  m_ButtonBox(Gtk::ORIENTATION_HORIZONTAL),
  m_ButtonStart("Start work"),
  m_ButtonStop("Stop work"),
  m_ButtonQuit("_Quit", /* mnemonic= */ true),
  m_ProgressBar(),
  m_ScrolledWindow(),
  m_TextView(),
  m_Dispatcher(),
  m_Worker(initial_n_ppl, n_years, min_food_gen, max_food_gen, climate_type, mapsize, mapwidth),
  m_HexMap(m_Worker.model),
  m_WorkerThread(nullptr)
{
  set_title("HomoSapiens");
  set_border_width(5);
  set_default_size(1400, 750);

  add(m_VBox);

  // Add the ProgressBar.
  m_VBox.pack_start(m_ProgressBar, Gtk::PACK_SHRINK);

  m_ProgressBar.set_text("Fraction done");
  m_ProgressBar.set_show_text();

  // Add the HexMap
  m_VBox.pack_start(m_HexMap);
  m_HexMap.show();

  // Add the TextView, inside a ScrolledWindow.
//  m_ScrolledWindow.add(m_TextView);

  // Only show the scrollbars when they are necessary.
//  m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

//  m_VBox.pack_start(m_ScrolledWindow);

//  m_TextView.set_editable(false);

  // Add the buttons to the ButtonBox.
  m_VBox.pack_start(m_ButtonBox, Gtk::PACK_SHRINK);

  m_ButtonBox.pack_start(m_ButtonStart, Gtk::PACK_SHRINK);
  m_ButtonBox.pack_start(m_ButtonStop, Gtk::PACK_SHRINK);
  m_ButtonBox.pack_start(m_ButtonQuit, Gtk::PACK_SHRINK);
  m_ButtonBox.set_border_width(5);
  m_ButtonBox.set_spacing(5);
  m_ButtonBox.set_layout(Gtk::BUTTONBOX_END);

  // Connect the signal handlers to the buttons.
  m_ButtonStart.signal_clicked().connect(sigc::mem_fun(*this, &CivWindow::on_start_button_clicked));
  m_ButtonStop.signal_clicked().connect(sigc::mem_fun(*this, &CivWindow::on_stop_button_clicked));
  m_ButtonQuit.signal_clicked().connect(sigc::mem_fun(*this, &CivWindow::on_quit_button_clicked));

  // Connect the handler to the dispatcher.
  m_Dispatcher.connect(sigc::mem_fun(*this, &CivWindow::on_notification_from_worker_thread));

  // Create a text buffer mark for use in update_widgets().
//  auto buffer = m_TextView.get_buffer();
//  buffer->create_mark("last_line", buffer->end(), /* left_gravity= */ true);

  update_start_stop_buttons();

  show_all_children();
}

void CivWindow::on_start_button_clicked()
{
  if (m_WorkerThread)
  {
    std::cout << "Can't start a worker thread while another one is running." << std::endl;
  }
  else
  {
    // Start a new worker thread.
    m_WorkerThread = new std::thread(
      [this]
      {
        m_Worker.do_work(this);
      });
  }
  update_start_stop_buttons();
}

void CivWindow::on_stop_button_clicked()
{
  if (!m_WorkerThread)
  {
    std::cout << "Can't stop a worker thread. None is running." << std::endl;
  }
  else
  {
   // Order the worker thread to stop.
    m_Worker.stop_work();
    m_ButtonStop.set_sensitive(false);
  }
}

void CivWindow::update_start_stop_buttons()
{
  const bool thread_is_running = m_WorkerThread != nullptr;

  m_ButtonStart.set_sensitive(!thread_is_running);
  m_ButtonStop.set_sensitive(thread_is_running);
}

void CivWindow::update_widgets()
{
  double fraction_done;
  Glib::ustring message_from_worker_thread;
  m_Worker.get_data(&fraction_done, &message_from_worker_thread);

  m_ProgressBar.set_fraction(fraction_done);

  /*
  if (message_from_worker_thread != m_TextView.get_buffer()->get_text())
  {
    auto buffer = m_TextView.get_buffer();
    buffer->set_text(message_from_worker_thread);

    // Scroll the last inserted line into view. That's somewhat complicated.
    Gtk::TextIter iter = buffer->end();
    iter.set_line_offset(0); // Beginning of last line
    auto mark = buffer->get_mark("last_line");
    buffer->move_mark(mark, iter);
    m_TextView.scroll_to(mark);
    // TextView::scroll_to(iter) is not perfect.
    // We do need a TextMark to always get the last line into view.
  }*/
}

void CivWindow::on_quit_button_clicked()
{
  if (m_WorkerThread)
  {
    // Order the worker thread to stop and wait for it to stop.
    m_Worker.stop_work();
    if (m_WorkerThread->joinable())
      m_WorkerThread->join();
  }
  hide();
}

// notify() is called from ModelThread::do_work(). It is executed in the worker
// thread. It triggers a call to on_notification_from_worker_thread(), which is
// executed in the GUI thread.
void CivWindow::notify()
{
  m_Dispatcher.emit();
}

void CivWindow::on_notification_from_worker_thread()
{
  if (m_WorkerThread && m_Worker.has_stopped())
  {
    // Work is done.
    if (m_WorkerThread->joinable())
      m_WorkerThread->join();
    delete m_WorkerThread;
    m_WorkerThread = nullptr;
    update_start_stop_buttons();
  }
  update_widgets();
}
