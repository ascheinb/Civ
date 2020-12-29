#include <iostream>

CivWindow::CivWindow(SetupParameters& setup_params_in) :
  m_VBox(Gtk::ORIENTATION_VERTICAL, 5),
  m_HBox(Gtk::ORIENTATION_HORIZONTAL),
  m_SideVBox(Gtk::ORIENTATION_VERTICAL),
  m_ButtonBox(Gtk::ORIENTATION_HORIZONTAL),
  m_ButtonNext("Next turn"),
  m_ButtonStart("Run"),
  m_ButtonStop("Pause"),
  m_ButtonQuit("_Quit", /* mnemonic= */ true),
  m_ProgressBar(),
  m_ScrolledWindow(),
  m_TextView(),
  m_Dispatcher(),
  m_Worker(setup_params_in),
  m_YearView(m_Worker.model),
  m_MapBox(),
  m_HexMap(m_Worker.model),
  m_PlotView(m_Worker.model),
  m_rb0(trait_names[Extroversion]),
  m_rb1(trait_names[Agreeableness]),
  m_rb2(trait_names[Conscientiousness]),
  m_rb3(trait_names[Neuroticism]),
  m_rb4(trait_names[Openness]),
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

  // Add the horizontal box
  m_VBox.pack_start(m_HBox);

  // Add the HexMap
  m_HBox.pack_start(m_MapBox, Gtk::PACK_EXPAND_WIDGET);
  m_MapBox.add(m_HexMap);//, Gtk::PACK_EXPAND_WIDGET);
  //g_signal_connect(drawing_area, "button-press-event", G_CALLBACK(clicked), NULL)0;
  //m_MapBox.signal_clicked().connect(sigc::mem_fun(*this, &CivWindow::on_map_clicked));
  m_MapBox.set_events(Gdk::BUTTON_PRESS_MASK);
  m_MapBox.signal_button_press_event().connect(sigc::mem_fun(*this, &CivWindow::on_map_clicked));
  m_MapBox.show();
  m_HexMap.show();

  m_HBox.pack_start(m_SideVBox, Gtk::PACK_SHRINK);

  m_SideVBox.set_size_request(350,-1);
  // Add the TextView, inside a ScrolledWindow.
//  m_ScrolledWindow.add(m_TextView);

  // Only show the scrollbars when they are necessary.
//  m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

//  m_VBox.pack_start(m_ScrolledWindow);

//  m_TextView.set_editable(false);

  m_SideVBox.pack_start(m_YearView);
  m_YearView.show();

  // Add the buttons to the ButtonBox.
  m_SideVBox.pack_start(m_ButtonBox, Gtk::PACK_SHRINK);

  m_ButtonBox.pack_start(m_ButtonNext, Gtk::PACK_SHRINK);
  m_ButtonBox.pack_start(m_ButtonStart, Gtk::PACK_SHRINK);
  m_ButtonBox.pack_start(m_ButtonStop, Gtk::PACK_SHRINK);
  m_ButtonBox.pack_start(m_ButtonQuit, Gtk::PACK_SHRINK);
  m_ButtonBox.set_border_width(5);
  m_ButtonBox.set_spacing(5);
  m_ButtonBox.set_layout(Gtk::BUTTONBOX_END);

  // Connect the signal handlers to the buttons.
  m_ButtonNext.signal_clicked().connect(sigc::mem_fun(*this, &CivWindow::on_next_button_clicked));
  m_ButtonStart.signal_clicked().connect(sigc::mem_fun(*this, &CivWindow::on_start_button_clicked));
  m_ButtonStop.signal_clicked().connect(sigc::mem_fun(*this, &CivWindow::on_stop_button_clicked));
  m_ButtonQuit.signal_clicked().connect(sigc::mem_fun(*this, &CivWindow::on_quit_button_clicked));

  // Connect the handler to the dispatcher.
  m_Dispatcher.connect(sigc::mem_fun(*this, &CivWindow::on_notification_from_worker_thread));

  // Create a text buffer mark for use in update_widgets().
//  auto buffer = m_TextView.get_buffer();
//  buffer->create_mark("last_line", buffer->end(), /* left_gravity= */ true);

  update_start_stop_buttons();

//  show_all_children();

  m_SideVBox.pack_start(m_PlotView, Gtk::PACK_EXPAND_WIDGET);
  m_PlotView.show();

  m_rb1.join_group(m_rb0);
  m_rb2.join_group(m_rb0);
  m_rb3.join_group(m_rb0);
  m_rb4.join_group(m_rb0);
  m_SideVBox.pack_start(m_rb0, Gtk::PACK_SHRINK);
  m_SideVBox.pack_start(m_rb1, Gtk::PACK_SHRINK);//, Gtk::PACK_EXPAND_WIDGET);
  m_SideVBox.pack_start(m_rb2, Gtk::PACK_SHRINK);//, Gtk::PACK_EXPAND_WIDGET);
  m_SideVBox.pack_start(m_rb3, Gtk::PACK_SHRINK);//, Gtk::PACK_EXPAND_WIDGET);
  m_SideVBox.pack_start(m_rb4, Gtk::PACK_SHRINK);
  m_rb0.set_active();

  m_rb0.signal_clicked().connect(sigc::mem_fun(*this,&CivWindow::on_rb0_clicked));
  m_rb1.signal_clicked().connect(sigc::mem_fun(*this,&CivWindow::on_rb1_clicked));
  m_rb2.signal_clicked().connect(sigc::mem_fun(*this,&CivWindow::on_rb2_clicked));
  m_rb3.signal_clicked().connect(sigc::mem_fun(*this,&CivWindow::on_rb3_clicked));
  m_rb4.signal_clicked().connect(sigc::mem_fun(*this,&CivWindow::on_rb4_clicked));

  //m_trait_button.signal_clicked().connect(sigc::mem_fun(*this, &CivWindow::on_trait_button_clicked));
  //m_trait_button.show();

  setupw_ = new SetupWindow(setup_params, m_Worker.model);
  //setupw_->signal_hide().connect(sigc::mem_fun(*this, &CivWindow::setupWinClose));
  setupw_->show();

  show_all_children();
}

void CivWindow::on_next_button_clicked()
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
        m_Worker.do_one_turn(this);
      });
  }
  // update_start_stop_buttons();
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

void CivWindow::on_rb0_clicked()
{
    m_Worker.model.plot_trait = Extroversion;
    m_Worker.model.set_fracs(m_Worker.model.plot_fracs, m_Worker.model.plot_avg);
}

void CivWindow::on_rb1_clicked()
{
    m_Worker.model.plot_trait = Agreeableness;
    m_Worker.model.set_fracs(m_Worker.model.plot_fracs, m_Worker.model.plot_avg);
}

void CivWindow::on_rb2_clicked()
{
    m_Worker.model.plot_trait = Conscientiousness;
    m_Worker.model.set_fracs(m_Worker.model.plot_fracs, m_Worker.model.plot_avg);
}

void CivWindow::on_rb3_clicked()
{
    m_Worker.model.plot_trait = Neuroticism;
    m_Worker.model.set_fracs(m_Worker.model.plot_fracs, m_Worker.model.plot_avg);
}

void CivWindow::on_rb4_clicked()
{
    m_Worker.model.plot_trait = Openness;
    m_Worker.model.set_fracs(m_Worker.model.plot_fracs, m_Worker.model.plot_avg);
}

bool CivWindow::on_map_clicked(GdkEventButton* event)
{
    const float hex_radius=HEXRADIUS;
    const float sin60 = 0.8660254;

    // Check if the event is a left button click.
	if (event->button == 1)
	{
        int ncol = m_Worker.model.nature.ncol;
        float half_width = hex_radius*sin60;

		// Memorize pointer position
		int mouseX=event->x - 50.0f + half_width; // aligns with left edge of first col
		int mouseY=event->y - 50.0f + hex_radius; // aligns with top point of first row

        int xrnd = mouseX/half_width;
        int yrnd = mouseY/(hex_radius/2);
        int xhu = xrnd%2;
        int yhu = yrnd%3;
        if(yhu>1) yhu=1;
        int Left=0,Right=1;
        int Diag=0,Mid=1;

        int col = mouseX/(half_width*2);
        int row = mouseY/(hex_radius*1.5);
        if (yhu==Mid && xhu==Left){
            col -= row%2;
        }else if (yhu==Diag){
            if (row%2==1) xhu = (xhu+1)%2;
            if (xhu==Left && (mouseY-yrnd*(hex_radius/2))<hex_radius/2-sin60/2*(mouseX-xrnd*half_width)){
                row--;
                col -= row%2;
            }
            if (xhu==Right && (mouseY-yrnd*(hex_radius/2))>sin60/2*(mouseX-xrnd*half_width)){
                col -= row%2;
            }else if (xhu==Right){
                row--;
            }
        }


        // ID which tile
        //int col = mouseX/(half_width*2);
        //int row = mouseY/(hex_radius*1.5);
        //printf("\ncol: %d, row: %d", col, row);
        int new_highlighted = row*ncol + col;
        if (col>=0 && col<ncol && row>=0 && row<m_Worker.model.nature.nrow){
            if (m_HexMap.highlighted==new_highlighted){
                m_HexMap.highlighted = -1; // if already selected, deselect
            } else {
                m_HexMap.highlighted = new_highlighted;

                // Open info window
                pginfow_ = new PGInfoWindow(m_Worker.model, new_highlighted);
                //setupw_->signal_hide().connect(sigc::mem_fun(*this, &CivWindow::setupWinClose));
                pginfow_->show();
            }
        }
	}
    return true;
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

void CivWindow::setupWinClose()
{
    setupw_ = 0;
}
