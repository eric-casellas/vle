/**
 * @file vle/gvle/ImportClassesBox.hpp
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


#ifndef VLE_GVLE_IMPORTCLASSESBOX_HPP
#define VLE_GVLE_IMPORTCLASSESBOX_HPP

#include <gtkmm.h>
#include <libglademm.h>

namespace vle {
    namespace vpz {
	class Class;
	class Classes;
	class Vpz;
    }
    namespace graph {
	class AtomicModel;
	class CoupledModel;
    }

namespace gvle {

class Modeling;

class ImportClassesBox {
private:
    class ModelColumnsClasses : public Gtk::TreeModel::ColumnRecord
    {
    public:
        ModelColumnsClasses() {
            add(m_col_name);
        }

        Gtk::TreeModelColumn<Glib::ustring> m_col_name;
    };

    class ClassesTreeView : public Gtk::TreeView
    {
    public:
	ClassesTreeView(BaseObjectType* cobject,
			const Glib::RefPtr<Gnome::Glade::Xml>& /*refGlade*/);
	virtual ~ClassesTreeView();

	void build(vpz::Classes* classes);
	void clear();
	Glib::ustring getSelected()
	    { return (*mRefTreeSelection
		      ->get_selected())[mColumns.m_col_name]; }
	void setParent(ImportClassesBox* parent)
	    { mParent = parent; }

    private:
	ImportClassesBox* mParent;
	ModelColumnsClasses mColumns;
	vpz::Classes* mClasses;
	Glib::RefPtr<Gtk::ListStore> mRefTreeModel;
	Glib::RefPtr<Gtk::TreeSelection> mRefTreeSelection;

	//Signal handlers:
	virtual void on_drag_data_get(
	    const Glib::RefPtr<Gdk::DragContext>& context,
	    Gtk::SelectionData& selection_data, guint, guint);
	virtual void on_drag_data_received(
	    const Glib::RefPtr<Gdk::DragContext>& context, int x, int y,
	    const Gtk::SelectionData& selection_data, guint, guint);
    };

public:
    ImportClassesBox(Glib::RefPtr<Gnome::Glade::Xml> xml, Modeling* modeling);

    void show(vpz::Vpz* src);
    void importClass(std::string& className, std::string newClassName = "");

protected:
private:
    Glib::RefPtr<Gnome::Glade::Xml>      mXml;
    Modeling*                            mModeling;
    vpz::Vpz*                            mSrc;

    Gtk::Dialog*                         mDialog;
    Gtk::Button*                         mButtonClose;

    ClassesTreeView*                     mProjectClasses;
    ClassesTreeView*                     mVpzClasses;

    void add_atomic_model_to_class(graph::AtomicModel* model,
				   vpz::Class& srcClass,
				   vpz::Class& destClass);
    void add_coupled_model_to_class(graph::CoupledModel* model,
				    vpz::Class& srcClass,
				    vpz::Class& destClass);
    void onClose();
};

}} // namespace vle gvle
#endif
