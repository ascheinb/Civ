#include <iostream>

PGInfoWindow::PGInfoWindow(Model& model_in, int tile_ind_in) :
        m_Button_Quit("Quit"),
        m_Button_Delete("Delete"),
        m_HBox(Gtk::ORIENTATION_HORIZONTAL),
        m_ScrolledWindow2(model_in),
        model(&model_in),
        tile_ind(tile_ind_in)
{
        set_title("Tile Residents and Groups");
        set_border_width(5);
        set_default_size(800, 600);

        add(m_VBox);

        // Add the horizontal box
        m_VBox.pack_start(m_HBox);

        //Add the TreeView, inside a ScrolledWindow, with the button underneath:
        m_ScrolledWindow.add(m_TreeView);

        //Only show the scrollbars when they are necessary:
        m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

        m_HBox.pack_start(m_ScrolledWindow);
        m_VBox.pack_start(m_ButtonBox, Gtk::PACK_SHRINK);

        m_ButtonBox.pack_start(m_Button_Quit, Gtk::PACK_SHRINK);
        m_ButtonBox.pack_start(m_Button_Delete, Gtk::PACK_SHRINK);
        m_ButtonBox.set_border_width(5);
        m_ButtonBox.set_layout(Gtk::BUTTONBOX_END);
        m_Button_Quit.signal_clicked().connect( sigc::mem_fun(*this, &PGInfoWindow::on_button_quit) );
        m_Button_Delete.signal_clicked().connect( sigc::mem_fun(*this, &PGInfoWindow::on_button_delete) );
        m_Button_Delete.set_sensitive( false ); // Only activate if row selected

        //Create the Tree model:
        m_refTreeModel     = Gtk::ListStore::create(m_Columns);
        m_TreeView.set_model(m_refTreeModel);

        //All the items to be reordered with drag-and-drop:
        m_TreeView.set_reorderable();
        m_TreeView.set_rules_hint();
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


        /**** Person Window *****/
        m_HBox.pack_start(m_ScrolledWindow2);

        // Populate with initial data
        Gtk::TreeModel::iterator iter = m_refTreeSelection->get_selected();
        int res_id = (*iter)[m_Columns.m_col_id];

        // Show initially selected person if there is anyone
        if (model->nature.map[tile_ind].residents.size() > 0)
            m_ScrolledWindow2.fill_buffers(res_id);

        show_all_children();
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


void PGInfoWindow::on_button_quit()
{
        hide();
}


void PGInfoWindow::on_button_delete()
{
        Gtk::TreeModel::iterator store_iter = m_refTreeSelection->get_selected();
        m_refTreeModel->erase( store_iter );
}


void PGInfoWindow::on_selection_changed()
{
    //m_Button_Delete.set_sensitive( m_refTreeSelection->count_selected_rows() > 0 );
    Gtk::TreeModel::iterator iter = m_refTreeSelection->get_selected();
    int res_id = (*iter)[m_Columns.m_col_id];

    m_ScrolledWindow2.fill_buffers(res_id);
}
