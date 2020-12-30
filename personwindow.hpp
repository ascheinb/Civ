#ifndef PERSONWINDOW_HPP
#define PERSONWINDOW_HPP

#include <glibmm.h>
#include <gdkmm.h>
#include <gtkmm.h>

#include "model.hpp"

class PersonWindow : public Gtk::ScrolledWindow
{
public:
        PersonWindow(Model& model_in);
        virtual ~PersonWindow();

        void fill_buffers(int pid);

private:
        void add_entry_r( const char* name, const char* reltype, const int f_to, const int f_of );
        void add_entry_m( const char* name, const int l_to );


        class ModelColumns_r : public Gtk::TreeModel::ColumnRecord
        {
                public:

                ModelColumns_r()
                { add(m_col_name); add(m_type); add(m_f_to); add(m_f_of); }

                Gtk::TreeModelColumn<Glib::ustring>     m_col_name;
                Gtk::TreeModelColumn<Glib::ustring>     m_type;
                Gtk::TreeModelColumn<int>               m_f_to;
                Gtk::TreeModelColumn<int>               m_f_of;
        };

        class ModelColumns_m : public Gtk::TreeModel::ColumnRecord
        {
                public:

                ModelColumns_m()
                { add(m_col_name); add(m_l_to); }

                Gtk::TreeModelColumn<Glib::ustring>     m_col_name;
                Gtk::TreeModelColumn<int>               m_l_to;
        };

        // Private members
        Gtk::Box m_VBox2;

        Gtk::TextView m_TextView;

        Glib::RefPtr<Gtk::TextBuffer> m_refTextBuffer1;


        // Relationships
        ModelColumns_r                          m_Columns_r;
        Gtk::ScrolledWindow                     m_ScrolledWindow_r;
        Gtk::TreeView                           m_TreeView_r;
        Glib::RefPtr<Gtk::ListStore>            m_refTreeModel_r;
        Glib::RefPtr<Gtk::TreeSelection>        m_refTreeSelection_r;

        // Memberships
        ModelColumns_m                          m_Columns_m;
        Gtk::ScrolledWindow                     m_ScrolledWindow_m;
        Gtk::TreeView                           m_TreeView_m;
        Glib::RefPtr<Gtk::ListStore>            m_refTreeModel_m;
        Glib::RefPtr<Gtk::TreeSelection>        m_refTreeSelection_m;

        Model* model;
};

#include "personwindow.impl.cpp"

#endif
