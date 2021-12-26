#include <iostream>

PGInfoWindow::PGInfoWindow(Model& model_in, int tile_ind_in) :
        m_Button_Quit("Quit"),
        m_Button_Delete("Delete"),
        m_HBox(Gtk::Orientation::HORIZONTAL),
        m_PersonWindow(model_in),
        m_GroupWindow(model_in),
        model(&model_in),
        tile_ind(tile_ind_in)
{
        set_title("Tile Residents and Groups");
        //set_border_width(5);
        set_default_size(800, 600);

        set_child(m_VBox);

        // Add the horizontal box
        m_VBox.append(m_HBox);

        //Add the TreeView, inside a ScrolledWindow, with the button underneath:
        m_ScrolledWindow.set_child(m_TreeView);

        m_ScrolledWindow_g.set_child(m_TreeView_g);

        //Only show the scrollbars when they are necessary:
        m_ScrolledWindow.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
        m_ScrolledWindow_g.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);

        m_HBox.append(m_Notebook);
        m_Notebook.append_page(m_ScrolledWindow, "Residents");
        m_Notebook.append_page(m_ScrolledWindow_g, "Groups");
        m_Notebook.signal_switch_page().connect(sigc::mem_fun(*this, &PGInfoWindow::on_notebook_switch_page) );

        m_VBox.append(m_ButtonBox);//, Gtk::PACK_SHRINK);

        m_ButtonBox.append(m_Button_Quit);//, Gtk::PACK_SHRINK);
        m_ButtonBox.append(m_Button_Delete);//, Gtk::PACK_SHRINK);
        //m_ButtonBox.set_border_width(5);
        //m_ButtonBox.set_layout(Gtk::BUTTONBOX_END);
        m_Button_Quit.signal_clicked().connect( sigc::mem_fun(*this, &PGInfoWindow::on_button_quit) );
        m_Button_Delete.signal_clicked().connect( sigc::mem_fun(*this, &PGInfoWindow::on_button_delete) );
        m_Button_Delete.set_sensitive( false ); // Only activate if row selected


        /*** Resident list  ***/

        m_refTreeModel     = Gtk::ListStore::create(m_Columns);
        m_TreeView.set_model(m_refTreeModel);

        //All the items to be reordered with drag-and-drop:
        m_TreeView.set_reorderable();
        //m_TreeView.set_rules_hint();
        m_TreeView.set_headers_clickable(true);
        m_TreeView.set_headers_visible(true);

        // Handle tree selections
        m_refTreeSelection = m_TreeView.get_selection();
        m_refTreeSelection->signal_changed().connect( sigc::mem_fun(*this, &PGInfoWindow::on_selection_changed) );

        //Add the TreeView's view columns:
        m_TreeView.append_column("Name", m_Columns.m_col_name);

        // Set sorting for each column added
        Gtk::TreeView::Column* pColumn = m_TreeView.get_column(0);
        pColumn->set_sort_column( m_Columns.m_col_name );

        // Fill the TreeView's model
        for( int ires = 0; ires < model->nature.map[tile_ind].residents.size(); ires++ )
        {
            int res_id = model->nature.map[tile_ind].residents[ires];
            int i_name = model->p.person[res_id].name;
            add_entry( res_id, names[i_name].c_str() );
        }

        /*** Group list  ***/

        m_refTreeModel_g     = Gtk::ListStore::create(m_Columns_g);
        m_TreeView_g.set_model(m_refTreeModel_g);

        //All the items to be reordered with drag-and-drop:
        m_TreeView_g.set_reorderable();
        //m_TreeView_g.set_rules_hint();
        m_TreeView_g.set_headers_clickable(true);
        m_TreeView_g.set_headers_visible(true);

        // Handle tree selections
        m_refTreeSelection_g = m_TreeView_g.get_selection();
        m_refTreeSelection_g->signal_changed().connect( sigc::mem_fun(*this, &PGInfoWindow::on_selection_changed_g) );

        //Add the TreeView's view columns:
        m_TreeView_g.append_column("Name", m_Columns_g.m_col_name);

        // Set sorting for each column added
        Gtk::TreeView::Column* pColumn_g = m_TreeView_g.get_column(0);
        pColumn_g->set_sort_column( m_Columns_g.m_col_name );

        // Fill the TreeView's model
        std::vector<int> gids;
        for( int ires = 0; ires < model->nature.map[tile_ind].residents.size(); ires++ )
        {
            int res_id = model->nature.map[tile_ind].residents[ires];
            // Loop through memberships of each resident
            for( int imship = 0; imship < model->p.person[res_id].mships.size(); imship++ )
            {
                int group_id = model->p.person[res_id].mships[imship].id;

                // Check no duplicate groups
                // todo: fix bad scaling ALS
                for (int eid = 0; eid<gids.size(); eid++)
                    if (group_id==gids[eid]) group_id=-1;
                if (group_id==-1) continue;

                gids.push_back(group_id);

                int i_name = model->p.groups[group_id].name;
                add_entry_g( group_id, gnames[i_name].c_str() );
            }
        }

        /**** Person Window *****/
        m_HBox.append(m_PersonWindow);

        // Populate with initial data
        Gtk::TreeModel::iterator iter = m_refTreeSelection->get_selected();
        int res_id = (*iter)[m_Columns.m_col_id];

        // Show initially selected person if there is anyone
        if (model->nature.map[tile_ind].residents.size() > 0)
            m_PersonWindow.fill_buffers(res_id);

        m_HBox.append(m_GroupWindow);
        // Populate with initial data
        //Gtk::TreeModel::iterator iter = m_refTreeSelection_g->get_selected();
        //int g_id = (*iter)[m_Columns_g.m_col_id];

        // Show initially selected group if there is one
        //if (model->nature.map[tile_ind].residents.size() > 0)
        //    m_GroupWindow.fill_buffers(res_id);

        //show_all_children();
        m_GroupWindow.hide();
}


PGInfoWindow::~PGInfoWindow()
{
}


void PGInfoWindow::add_entry(const int res_id, const char* name )
{
        Gtk::TreeModel::Row row         = *(m_refTreeModel->append());
//        row[m_Columns.m_pixbuf]         = Gdk::Pixbuf::create_from_file( filename );
        row[m_Columns.m_col_id] = res_id;
        row[m_Columns.m_col_name]       = name;
}

void PGInfoWindow::add_entry_g(const int res_id, const char* name )
{
        Gtk::TreeModel::Row row         = *(m_refTreeModel_g->append());
//        row[m_Columns.m_pixbuf]         = Gdk::Pixbuf::create_from_file( filename );
        row[m_Columns_g.m_col_id] = res_id;
        row[m_Columns_g.m_col_name]       = name;
}

void PGInfoWindow::on_button_quit()
{
        //hide();
}


void PGInfoWindow::on_button_delete()
{
        Gtk::TreeModel::iterator store_iter = m_refTreeSelection->get_selected();
        m_refTreeModel->erase( store_iter );
}

void PGInfoWindow::on_notebook_switch_page(Gtk::Widget* /* page */, guint page_num)
{
  switch (page_num){
      case 1:
          m_PersonWindow.hide();
          m_GroupWindow.show();
          break;
      case 0:
          m_GroupWindow.hide();
          m_PersonWindow.show();
          break;
  }
  //You can also use m_Notebook.get_current_page() to get this index.
}

void PGInfoWindow::on_selection_changed()
{
    //m_Button_Delete.set_sensitive( m_refTreeSelection->count_selected_rows() > 0 );
    Gtk::TreeModel::iterator iter = m_refTreeSelection->get_selected();
    int res_id = (*iter)[m_Columns.m_col_id];

    m_PersonWindow.fill_buffers(res_id);
}

void PGInfoWindow::on_selection_changed_g()
{
    //m_Button_Delete.set_sensitive( m_refTreeSelection->count_selected_rows() > 0 );
    Gtk::TreeModel::iterator iter = m_refTreeSelection_g->get_selected();
    int group_id = (*iter)[m_Columns_g.m_col_id];

    m_GroupWindow.fill_buffers(group_id);
}
