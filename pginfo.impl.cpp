#include <iostream>

namespace
{
        struct TreeEntry
        {
                int             m_id;
                int             m_dice_id;
                const char*     m_filename;
                const char*     m_name;

                TreeEntry() : m_id(-1), m_dice_id(-1), m_filename(0), m_name(0) {}
                TreeEntry( const int id, const int dice_id, const char* fn, const char* name )
                        : m_id(id), m_dice_id(dice_id), m_filename(fn), m_name(name) {}
        };

        TreeEntry g_entries[] = 
        {
                TreeEntry(  1,  1, "gnome-dice-1.svg", "Billy Bob"        ),
                TreeEntry( 11,  2, "gnome-dice-2.svg", "Billy Bob Junior" ),
                TreeEntry( 12,  3, "gnome-dice-3.svg", "Sue Bob"          ),
                TreeEntry(  2,  4, "gnome-dice-4.svg", "Joey Jojo"        ),
                TreeEntry(  3,  5, "gnome-dice-5.svg", "Rob McRoberts"    ),
                TreeEntry( 31,  6, "gnome-dice-6.svg", "Xavier McRoberts" ),
                TreeEntry( 53,  1, "gnome-dice-1.svg", "Ian McClintock"   ),
                TreeEntry( 27,  2, "gnome-dice-2.svg", "John Roberts"     )
        };
}


PGInfoWindow::PGInfoWindow(Model& model_in, int tile_ind_in) :
        m_Button_Quit("Quit"),
        m_Button_Delete("Delete"),
        m_VBox2(Gtk::ORIENTATION_VERTICAL),
        m_HBox(Gtk::ORIENTATION_HORIZONTAL),
        m_Button_Buffer1("Use buffer 1"),
        m_Button_Buffer2("Use buffer 2"),
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
        //
        m_refTreeSelection = m_TreeView.get_selection();
        m_refTreeSelection->signal_changed().connect( sigc::mem_fun(*this, &PGInfoWindow::on_selection_changed) );

        //Add the TreeView's view columns:
        m_TreeView.append_column("ID", m_Columns.m_col_id);
        m_TreeView.append_column("Image", m_Columns.m_pixbuf);
        m_TreeView.append_column("Name", m_Columns.m_col_name);
        m_refTreeModel->set_sort_column( m_Columns.m_col_name, Gtk::SORT_ASCENDING );   // Initial sorting column

        // Set sorting for each column added
        Gtk::TreeView::Column* pColumn = m_TreeView.get_column(0);
        pColumn->set_sort_column( m_Columns.m_col_id );

        pColumn = m_TreeView.get_column(1);
        pColumn->set_sort_column( m_Columns.m_dice_id );

        pColumn = m_TreeView.get_column(2);
        pColumn->set_sort_column( m_Columns.m_col_name );

        // Fill the TreeView's model
        for( int ires = 0; ires < model->nature.map[tile_ind].residents.size(); ires++ )
        {
            int res_id = model->nature.map[tile_ind].residents[ires];
            int i_name = model->p.person[res_id].name;
            add_entry( 0, 1, "hello", names[i_name].c_str() );
        }


        /**** Text part *****/
        m_HBox.pack_start(m_ScrolledWindow2);

        //Add the TreeView, inside a ScrolledWindow, with the button underneath:
        m_ScrolledWindow2.add(m_VBox2);

        //Only show the scrollbars when they are necessary:
        m_ScrolledWindow2.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

        m_VBox2.pack_start(m_TextView);
        //m_VBox2.pack_start(m_ScrolledWindow2);

        // Relationships & Memberships

        //Add the TreeView, inside a ScrolledWindow, with the button underneath:
        m_VBox2.pack_start(m_TreeView_r);

        //Create the Tree model:
        m_refTreeModel_r     = Gtk::ListStore::create(m_Columns_r);
        m_TreeView_r.set_model(m_refTreeModel_r);

        //All the items to be reordered with drag-and-drop:
        m_TreeView_r.set_reorderable();
        m_TreeView_r.set_rules_hint();
        m_TreeView_r.set_headers_clickable(true);
        m_TreeView_r.set_headers_visible(true);

        // Handle tree selections
        m_refTreeSelection_r = m_TreeView_r.get_selection();
        m_refTreeSelection_r->signal_changed().connect( sigc::mem_fun(*this, &PGInfoWindow::on_selection_changed) );

        //Add the TreeView's view columns:
        m_TreeView_r.append_column("ID", m_Columns_r.m_col_id);
        m_TreeView_r.append_column("Image", m_Columns_r.m_pixbuf);
        m_TreeView_r.append_column("Name", m_Columns_r.m_col_name);
        m_refTreeModel_r->set_sort_column( m_Columns_r.m_col_name, Gtk::SORT_ASCENDING );   // Initial sorting column

        // Set sorting for each column added
        Gtk::TreeView::Column* pColumn_r = m_TreeView_r.get_column(0);
        pColumn_r->set_sort_column( m_Columns_r.m_col_id );

        pColumn_r = m_TreeView_r.get_column(1);
        pColumn_r->set_sort_column( m_Columns_r.m_dice_id );

        pColumn_r = m_TreeView_r.get_column(2);
        pColumn_r->set_sort_column( m_Columns_r.m_col_name );

        // Fill the TreeView's model
        if (model->nature.map[tile_ind].residents.size()>0){
            int res_id = model->nature.map[tile_ind].residents[0];
            for( int irship = 0; irship < model->p.person[res_id].rships.size(); irship++ )
            {
                int person_ind = model->p.id2ind[model->p.person[res_id].rships[irship].person_id];
                int fondness_to = model->p.person[res_id].rships[irship].fondness_to;
                int fondness_of = model->p.person[res_id].rships[irship].fondness_of;
                RelType reltype = model->p.person[res_id].rships[irship].reltype;
                int i_name = model->p.person[person_ind].name;
                add_entry_r( fondness_to, 1, "hello", names[i_name].c_str() );
            }
        }

        //append buttons:
        //m_VBox2.pack_start(m_ButtonBox2);

        m_Button_Buffer1.set_hexpand(true);
        //m_Button_Buffer1.set_halign(Gtk::Align::END);
        m_ButtonBox2.pack_start(m_Button_Buffer1);
        m_ButtonBox2.pack_start(m_Button_Buffer2);
        //m_ButtonBox.set_margin(5);
        //m_ButtonBox.set_spacing(5);

        //Connect signals:
        m_Button_Buffer1.signal_clicked().connect(sigc::mem_fun(*this, &PGInfoWindow::on_button_buffer1) );
        m_Button_Buffer2.signal_clicked().connect(sigc::mem_fun(*this, &PGInfoWindow::on_button_buffer2) );

        fill_buffers();
        on_button_buffer1();

        show_all_children();
}


PGInfoWindow::~PGInfoWindow()
{
}


void PGInfoWindow::add_entry( const int id, const int dice_id, const char* filename, const char* name )
{
        Gtk::TreeModel::Row row         = *(m_refTreeModel->append());
        row[m_Columns.m_col_id]         = id;
        row[m_Columns.m_dice_id]        = dice_id;
//        row[m_Columns.m_pixbuf]         = Gdk::Pixbuf::create_from_file( filename );
        row[m_Columns.m_col_name]       = name;
}

void PGInfoWindow::add_entry_r( const int id, const int dice_id, const char* filename, const char* name )
{
        Gtk::TreeModel::Row row         = *(m_refTreeModel_r->append());
        row[m_Columns_r.m_col_id]         = id;
        row[m_Columns_r.m_dice_id]        = dice_id;
//        row[m_Columns.m_pixbuf]         = Gdk::Pixbuf::create_from_file( filename );
        row[m_Columns_r.m_col_name]       = name;
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
        m_Button_Delete.set_sensitive(
		m_refTreeSelection->count_selected_rows() > 0 );
}

void PGInfoWindow::fill_buffers()
{
    /*
    // changing

    int home;
    int worktype;
    float contentedness;
    float workrate;
    float luxrate;

    int employer;

    // Personality
    int extroversion; // Controls branch-out socialization
    int agreeableness; // Controls theft and caretaking
    int conscientiousness; // Controls group loyalty degradation rate
    int neuroticism; // Controls savings vs luxury spending
    int openness; // Controls willingness to move

    // Relationships
    vector<Relationship> rships;
    vector<Membership> mships;
*/
    int pid = 0;
    std::string display_text("Basic info:");
    display_text += "\n\n\tName: " + names[model->p.person[pid].name];
    display_text += "\n\tAge: " + std::to_string(model->p.person[pid].age/4);
    std::string gender("male");
    if (model->p.person[pid].female) gender = "female";
    display_text += "\n\tSex: " + gender;

    display_text += "\n\nLifestyle:";
    display_text += "\n\n\tHome: Tile " + std::to_string(model->p.person[pid].home);
    display_text += "\n\tContentedness: " + std::to_string(int(model->p.person[pid].contentedness*100));
    display_text += "\n\tWork: " + std::to_string(model->p.person[pid].worktype);
    display_text += "\n\tWork Rate: " + std::to_string(int(model->p.person[pid].workrate*100)) + "%";
    display_text += "\n\tWealth: " + std::to_string(int(model->p.person[pid].wealth));
    display_text += "\n\tLux Rate: " + std::to_string(int(model->p.person[pid].luxrate*100));

    display_text += "\n\nPersonality profile:";
    display_text += "\n\n\tExtroversion:\t\t\t" + std::to_string(model->p.person[pid].extroversion);
    display_text += "\n\tAgreeableness:\t\t" + std::to_string(model->p.person[pid].agreeableness);
    display_text += "\n\tConscientiousness:\t" + std::to_string(model->p.person[pid].conscientiousness);
    display_text += "\n\tNeuroticism:\t\t\t" + std::to_string(model->p.person[pid].neuroticism);
    display_text += "\n\tOpenness:\t\t\t" + std::to_string(model->p.person[pid].openness);

    display_text += "\n\nRelationships:";

    m_refTextBuffer1 = Gtk::TextBuffer::create();
    m_refTextBuffer1->set_text(display_text.c_str());

    //m_refTextBuffer2 = Gtk::TextBuffer::create();
    //m_refTextBuffer2->set_text(
    //      "This is some alternative text, from TextBuffer #2.");

}

void PGInfoWindow::on_button_buffer1()
{
  m_TextView.set_buffer(m_refTextBuffer1);
}

void PGInfoWindow::on_button_buffer2()
{
  m_TextView.set_buffer(m_refTextBuffer2);
}
