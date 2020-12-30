#include <iostream>

GroupWindow::GroupWindow(Model& model_in) :
        m_VBox2(Gtk::ORIENTATION_VERTICAL),
        model(&model_in)
{
        //Add the TreeView, inside a ScrolledWindow, with the button underneath:
        add(m_VBox2);

        //Only show the scrollbars when they are necessary:
        set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

        m_VBox2.pack_start(m_TextView);
        m_refTextBuffer1 = Gtk::TextBuffer::create();
        m_refTextBuffer1->set_text("\n\n\tSelect a person or a group.");
        m_TextView.set_buffer(m_refTextBuffer1);

        // Relationships

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
        //m_refTreeSelection_r->signal_changed().connect( sigc::mem_fun(*this, &PGInfoWindow::on_selection_changed) );

        //Add the TreeView's view columns:
        m_TreeView_r.append_column("Name", m_Columns_r.m_col_name);
        m_TreeView_r.append_column("Type", m_Columns_r.m_type);
        m_TreeView_r.append_column("Fondness for", m_Columns_r.m_f_to);
        m_TreeView_r.append_column("Fondness by", m_Columns_r.m_f_of);
        //m_TreeView_r.append_column("Image", m_Columns_r.m_pixbuf);
        m_refTreeModel_r->set_sort_column( m_Columns_r.m_f_to, Gtk::SORT_DESCENDING );   // Initial sorting column

        // Set sorting for each column added
        Gtk::TreeView::Column* pColumn_r = m_TreeView_r.get_column(0);
        pColumn_r->set_sort_column( m_Columns_r.m_col_name );

        pColumn_r = m_TreeView_r.get_column(1);
        pColumn_r->set_sort_column( m_Columns_r.m_type );

        pColumn_r = m_TreeView_r.get_column(2);
        pColumn_r->set_sort_column( m_Columns_r.m_f_to );

        pColumn_r = m_TreeView_r.get_column(3);
        pColumn_r->set_sort_column( m_Columns_r.m_f_of );


        // Memberships

        //Add the TreeView, inside a ScrolledWindow, with the button underneath:
        m_VBox2.pack_start(m_TreeView_m);

        //Create the Tree model:
        m_refTreeModel_m     = Gtk::ListStore::create(m_Columns_m);
        m_TreeView_m.set_model(m_refTreeModel_m);

        //All the items to be reordered with drag-and-drop:
        m_TreeView_m.set_reorderable();
        m_TreeView_m.set_rules_hint();
        m_TreeView_m.set_headers_clickable(true);
        m_TreeView_m.set_headers_visible(true);

        // Handle tree selections
        m_refTreeSelection_m = m_TreeView_m.get_selection();
        //m_refTreeSelection_m->signal_changed().connect( sigc::mem_fun(*this, &PGInfoWindow::on_selection_changed) );

        //Add the TreeView's view columns:
        m_TreeView_m.append_column("Name", m_Columns_m.m_col_name);
        m_TreeView_m.append_column("Loyalty to", m_Columns_m.m_l_to);
        m_refTreeModel_m->set_sort_column( m_Columns_m.m_l_to, Gtk::SORT_DESCENDING );   // Initial sorting column

        // Set sorting for each column added
        Gtk::TreeView::Column* pColumn_m = m_TreeView_m.get_column(0);
        pColumn_m->set_sort_column( m_Columns_m.m_col_name );

        pColumn_m = m_TreeView_m.get_column(1);
        pColumn_m->set_sort_column( m_Columns_m.m_l_to );

        //show_all_children();
}


GroupWindow::~GroupWindow()
{
}


void GroupWindow::add_entry_r( const char* name, const char* reltype, const int f_to, const int f_of)
{
        Gtk::TreeModel::Row row         = *(m_refTreeModel_r->append());
        row[m_Columns_r.m_col_name]     = name;
        row[m_Columns_r.m_type]         = reltype;
        row[m_Columns_r.m_f_to ]        = f_to;
        row[m_Columns_r.m_f_of ]        = f_of;
}

void GroupWindow::add_entry_m( const char* name, const int l_to)
{
        Gtk::TreeModel::Row row         = *(m_refTreeModel_m->append());
        row[m_Columns_m.m_col_name]       = name;
        row[m_Columns_m.m_l_to ]        = l_to;
}

void GroupWindow::fill_buffers(int gid)
{
    // Clear buffers before filling
    m_refTreeModel_r->clear();
    m_refTreeModel_m->clear();

    // Fill text
    std::string display_text("\n\nBasic info:");
    display_text += "\n\n\tName: " + gnames[model->p.groups[gid].name];

/*    display_text += "\n\nLifestyle:";
    display_text += "\n\n\tHome: Tile " + std::to_string(model->p.person[pid].home);
    display_text += "\n\tContentedness: " + std::to_string(int(model->p.person[pid].contentedness*100));
    display_text += "\n\tWork: " + work_names[model->p.person[pid].worktype];
    display_text += "\n\tWork Rate: " + std::to_string(int(model->p.person[pid].workrate*100)) + "%";
*/    display_text += "\n\tWealth: " + std::to_string(int(model->p.groups[gid].wealth));
/*    display_text += "\n\tLux Rate: " + std::to_string(int(model->p.person[pid].luxrate*100));

    display_text += "\n\nPersonality profile:";
    display_text += "\n\n\tExtroversion:\t\t\t" + std::to_string(model->p.person[pid].extroversion);
    display_text += "\n\tAgreeableness:\t\t" + std::to_string(model->p.person[pid].agreeableness);
    display_text += "\n\tConscientiousness:\t" + std::to_string(model->p.person[pid].conscientiousness);
    display_text += "\n\tNeuroticism:\t\t\t" + std::to_string(model->p.person[pid].neuroticism);
    display_text += "\n\tOpenness:\t\t\t" + std::to_string(model->p.person[pid].openness);

    display_text += "\n\nRelationships and Memberships:\n";
*/
    m_refTextBuffer1->set_text(display_text.c_str());
    m_TextView.set_buffer(m_refTextBuffer1);

    // Relationships
    for( int irship = 0; irship < model->p.groups[gid].memberlist.size(); irship++ )
    {
        //printf("\nirship: %d, mlist: %d, gid: %d", irship, model->p.groups[gid].memberlist[irship], gid);
        //int person_ind = model->p.id2ind[model->p.groups[gid].memberlist[irship]];
        int person_ind = model->p.groups[gid].memberlist[irship];
        //int fondness_to = model->p.person[pid].rships[irship].fondness_to;
        //int fondness_of = model->p.person[pid].rships[irship].fondness_of;
        //RelType reltype = model->p.person[pid].rships[irship].reltype;
        int i_name = model->p.person[person_ind].name;
        add_entry_r( names[i_name].c_str(), "X", 0, 0);
    }

    // Memberships
    /*
    for( int imship = 0; imship < model->p.person[pid].mships.size(); imship++ )
    {
        int group_id = model->p.person[pid].mships[imship].id;
        int loyalty_to = model->p.person[pid].mships[imship].loyalty_to;

        int i_name = model->p.groups[group_id].name;
        add_entry_m( gnames[i_name].c_str(), loyalty_to );
    }*/


}
