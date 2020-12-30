#ifndef GTKMM_EXAMPLEWINDOW_H
#define GTKMM_EXAMPLEWINDOW_H

#include <glibmm.h>
#include <gdkmm.h>
#include <gtkmm.h>

#include "model.hpp"

class PGInfoWindow : public Gtk::Window
{
public:
        PGInfoWindow(Model& model_in, int tile_ind_in);
        virtual ~PGInfoWindow();

private:
        void add_entry( const int res_id, const char* name );
        void add_entry_r( const char* name, const char* reltype, const int f_to, const int f_of );
        void add_entry_m( const char* name, const int l_to );

        void fill_buffers(int pid);

        // Signal handlers
        virtual void on_button_quit();
        virtual void on_button_delete();
        virtual void on_selection_changed();

        virtual void on_button_buffer1();
        virtual void on_button_buffer2();

        // Types and classes
        typedef Glib::RefPtr<Gdk::Pixbuf> PixbufPtr;

        class ModelColumns : public Gtk::TreeModel::ColumnRecord
        {
                public:

                ModelColumns()
                { add(m_col_id); add(m_dice_id); add(m_pixbuf); add(m_col_name); }

                Gtk::TreeModelColumn<int>			m_col_id;
                Gtk::TreeModelColumn<int>			m_dice_id;
                Gtk::TreeModelColumn<PixbufPtr> 		m_pixbuf;
                Gtk::TreeModelColumn<Glib::ustring>   		m_col_name;
        };

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
        ModelColumns                            m_Columns;

        Gtk::VBox                               m_VBox;
        Gtk::Box                                m_HBox;

        Gtk::ScrolledWindow                     m_ScrolledWindow;
        Gtk::TreeView                           m_TreeView;
        Glib::RefPtr<Gtk::ListStore>            m_refTreeModel;
        Glib::RefPtr<Gtk::TreeSelection>        m_refTreeSelection;

        Gtk::HButtonBox                         m_ButtonBox;
        Gtk::Button                             m_Button_Quit;
        Gtk::Button                             m_Button_Delete;

        Gtk::Box m_VBox2;

        Gtk::ScrolledWindow m_ScrolledWindow2;
        Gtk::TextView m_TextView;

        Glib::RefPtr<Gtk::TextBuffer> m_refTextBuffer1, m_refTextBuffer2;

        Gtk::Box m_ButtonBox2;
        Gtk::Button m_Button_Buffer1, m_Button_Buffer2;
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
        int tile_ind;
};

#include "pginfo.impl.cpp"

#endif
