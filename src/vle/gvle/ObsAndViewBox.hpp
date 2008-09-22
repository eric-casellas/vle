/**
 * @file vle/gvle/ObsAndViewBox.hpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef GUI_OBSANDVIEWBOX_HPP
#define GUI_OBSANDVIEWBOX_HPP

#include <vle/gvle/ObsAndViewBox.hpp>
#include <vle/gvle/TreeViewObservable.hpp>
#include <vle/vpz/Observable.hpp>
#include <vle/vpz/Views.hpp>
#include <gtkmm.h>
#include <libglademm.h>

namespace vle
{
namespace gvle {

class Modeling;

class ObsAndViewBox
{
public:
    ObsAndViewBox(Glib::RefPtr<Gnome::Glade::Xml> xml);
    ~ObsAndViewBox();

    void show(vpz::Observables& obs, std::string name, vpz::Views& views);

protected:
class ModelColumnsObs : public Gtk::TreeModel::ColumnRecord
    {
    public:
        ModelColumnsObs() {
            add(m_col_name);
            add(m_col_type);
        }
        Gtk::TreeModelColumn<Glib::ustring>      m_col_name;
        Gtk::TreeModelColumn<vpz::Base::type>    m_col_type;
    };

class ModelColumnsView : public Gtk::TreeModel::ColumnRecord
    {
    public:
        ModelColumnsView() {
            add(m_col_name);
        }
        Gtk::TreeModelColumn<Glib::ustring>      m_col_name;
    };

private:
    Glib::RefPtr<Gnome::Glade::Xml>       mXml;
    Gtk::Dialog*                          mDialog;

    //Data
    vpz::Observables*                     mAll_Obs;
    vpz::Observable*                      mObs;
    vpz::Views*                           mViews;

    //Backup
    vpz::Observables*                     mAll_Obs_backup;
    vpz::Views*                           mViews_backup;

    //Observable
    Gtk::TreeView*                        mTreeViewObs;
    Glib::RefPtr<Gtk::TreeStore>          mRefTreeObs;
    ModelColumnsObs                       mColumnsObs;
    Gtk::Button*                          mButton_Add_Obs;
    Gtk::Button*                          mButton_Del_Obs;

    //Views
    Gtk::TreeView*                        mTreeViewViews;
    Glib::RefPtr<Gtk::TreeStore>          mRefTreeViews;
    ModelColumnsView                      mColumnsViews;
    Gtk::Button*                          mButton_Add_View;
    Gtk::Button*                          mButton_Del_View;

    //Buttons
    Gtk::Button*                          mButtonApply;
    Gtk::Button*                          mButtonCancel;

    bool                                  mSelected;

    void on_apply();
    void on_cancel();

    void on_drag_end(const Glib::RefPtr<Gdk::DragContext >&);

    void on_data_received(const Glib::RefPtr<Gdk::DragContext>&, int,
                          int, const Gtk::SelectionData&,
                          guint, guint);
    void on_view_activated(const Gtk::TreeModel::Path&, Gtk::TreeViewColumn*);

    void makeObs();
    void makeViews();

    void on_add_port();
    void on_del_port();

    void on_add_view();
    void on_del_view();
};

}
} // namespace vle gvle

#endif
