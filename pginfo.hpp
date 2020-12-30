#ifndef GTKMM_EXAMPLEWINDOW_H
#define GTKMM_EXAMPLEWINDOW_H

#include <glibmm.h>
#include <gdkmm.h>
#include <gtkmm.h>

#include "model.hpp"
#include "personwindow.hpp"

class PGInfoWindow : public Gtk::Window
{
public:
        PGInfoWindow(Model& model_in, int tile_ind_in);
        virtual ~PGInfoWindow();

private:
        void add_entry( const int res_id, const char* name );
        void add_entry_g( const int res_id, const char* name );

        // Signal handlers
        virtual void on_button_quit();
        virtual void on_button_delete();
        virtual void on_selection_changed();
        virtual void on_selection_changed_g();

        // Types and classes
        typedef Glib::RefPtr<Gdk::Pixbuf> PixbufPtr;

        Gtk::Notebook m_Notebook;

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

        class ModelColumns_g : public Gtk::TreeModel::ColumnRecord
        {
                public:

                ModelColumns_g()
                { add(m_col_id); add(m_dice_id); add(m_pixbuf); add(m_col_name); }

                Gtk::TreeModelColumn<int>           m_col_id;
                Gtk::TreeModelColumn<int>           m_dice_id;
                Gtk::TreeModelColumn<PixbufPtr>         m_pixbuf;
                Gtk::TreeModelColumn<Glib::ustring>         m_col_name;
        };

        // Private members
        ModelColumns                            m_Columns;
        ModelColumns_g                          m_Columns_g;

        Gtk::VBox                               m_VBox;
        Gtk::Box                                m_HBox;

        Gtk::ScrolledWindow                     m_ScrolledWindow;
        Gtk::TreeView                           m_TreeView;
        Glib::RefPtr<Gtk::ListStore>            m_refTreeModel;
        Glib::RefPtr<Gtk::TreeSelection>        m_refTreeSelection;

        // For group list
        Gtk::ScrolledWindow                     m_ScrolledWindow_g;
        Gtk::TreeView                           m_TreeView_g;
        Glib::RefPtr<Gtk::ListStore>            m_refTreeModel_g;
        Glib::RefPtr<Gtk::TreeSelection>        m_refTreeSelection_g;

        Gtk::HButtonBox                         m_ButtonBox;
        Gtk::Button                             m_Button_Quit;
        Gtk::Button                             m_Button_Delete;


        PersonWindow m_PersonWindow;

        Gtk::Box m_ButtonBox2;

        Model* model;
        int tile_ind;
};

#include "pginfo.impl.cpp"

#endif
