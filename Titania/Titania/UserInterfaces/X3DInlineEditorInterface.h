/* -*- Mode: C++; coding: utf-8; tab-width: 3; indent-tabs-mode: tab; c-basic-offset: 3 -*-
 *******************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright create3000, Scheffelstraße 31a, Leipzig, Germany 2011.
 *
 * All rights reserved. Holger Seelig <holger.seelig@yahoo.de>.
 *
 * THIS IS UNPUBLISHED SOURCE CODE OF create3000.
 *
 * The copyright notice above does not evidence any actual of intended
 * publication of such source code, and is an unpublished work by create3000.
 * This material contains CONFIDENTIAL INFORMATION that is the property of
 * create3000.
 *
 * No permission is granted to copy, distribute, or create derivative works from
 * the contents of this software, in whole or in part, without the prior written
 * permission of create3000.
 *
 * NON-MILITARY USE ONLY
 *
 * All create3000 software are effectively free software with a non-military use
 * restriction. It is free. Well commented source is provided. You may reuse the
 * source in any way you please with the exception anything that uses it must be
 * marked to indicate is contains 'non-military use only' components.
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 1999, 2012 Holger Seelig <holger.seelig@yahoo.de>.
 *
 * This file is part of the Titania Project.
 *
 * Titania is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License version 3 only, as published by the
 * Free Software Foundation.
 *
 * Titania is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License version 3 for more
 * details (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version 3
 * along with Titania.  If not, see <http://www.gnu.org/licenses/gpl.html> for a
 * copy of the GPLv3 License.
 *
 * For Silvio, Joy and Adi.
 *
 ******************************************************************************/
#ifndef __TMP_GLAD2CPP_INLINE_EDITOR_H__
#define __TMP_GLAD2CPP_INLINE_EDITOR_H__

#include "../Base/X3DEditorInterface.h"
#include <gtkmm.h>
#include <string>

namespace titania {
namespace puck {

class X3DInlineEditorInterface :
	public X3DEditorInterface
{
public:

	X3DInlineEditorInterface () :
		X3DEditorInterface ()
	{ }

	template <class ... Arguments>
	X3DInlineEditorInterface (const std::string & filename, const Arguments & ... arguments) :
		X3DEditorInterface (m_widgetName, arguments ...),
		          filename (filename)
	{ create (filename); }

	const Glib::RefPtr <Gtk::Builder> &
	getBuilder () const { return m_builder; }

	const std::string &
	getWidgetName () const { return m_widgetName; }

	template <class Type>
	Type*
	createWidget (const std::string & name) const
	{
		getBuilder () -> add_from_file (filename, name);

		Type* widget = nullptr;
		m_builder -> get_widget (name, widget);
		return widget;
	}

	const Glib::RefPtr <Gtk::Adjustment> &
	getBBoxCenterXAdjustment () const
	{ return m_BBoxCenterXAdjustment; }

	const Glib::RefPtr <Gtk::Adjustment> &
	getBBoxCenterYAdjustment () const
	{ return m_BBoxCenterYAdjustment; }

	const Glib::RefPtr <Gtk::Adjustment> &
	getBBoxCenterZAdjustment () const
	{ return m_BBoxCenterZAdjustment; }

	const Glib::RefPtr <Gtk::Adjustment> &
	getBBoxSizeXAdjustment () const
	{ return m_BBoxSizeXAdjustment; }

	const Glib::RefPtr <Gtk::Adjustment> &
	getBBoxSizeYAdjustment () const
	{ return m_BBoxSizeYAdjustment; }

	const Glib::RefPtr <Gtk::Adjustment> &
	getBBoxSizeZAdjustment () const
	{ return m_BBoxSizeZAdjustment; }

	const Glib::RefPtr <Gtk::CellRendererText> &
	getURLCellRendererText () const
	{ return m_URLCellRendererText; }

	const Glib::RefPtr <Gtk::TreeViewColumn> &
	getURLChooserColumn () const
	{ return m_URLChooserColumn; }

	const Glib::RefPtr <Gtk::CellRendererPixbuf> &
	getURLCellrendererPixbuf () const
	{ return m_URLCellrendererPixbuf; }

	Gtk::Window &
	getWindow () const
	{ return *m_Window; }

	Gtk::Box &
	getWidget () const
	{ return *m_Widget; }

	Gtk::Box &
	getInlineActionBox () const
	{ return *m_InlineActionBox; }

	Gtk::Button &
	getNewInlineButton () const
	{ return *m_NewInlineButton; }

	Gtk::Button &
	getRemoveInlineButton () const
	{ return *m_RemoveInlineButton; }

	Gtk::Button &
	getIndexButton () const
	{ return *m_IndexButton; }

	Gtk::Button &
	getConvertMasterSelectionButton () const
	{ return *m_ConvertMasterSelectionButton; }

	Gtk::Switch &
	getUpdateBoundingBoxFieldsSwitch () const
	{ return *m_UpdateBoundingBoxFieldsSwitch; }

	Gtk::Expander &
	getInlineExpander () const
	{ return *m_InlineExpander; }

	Gtk::Grid &
	getInlineBox () const
	{ return *m_InlineBox; }

	Gtk::CheckButton &
	getLoadCheckButton () const
	{ return *m_LoadCheckButton; }

	Gtk::Button &
	getFoldBackIntoSceneButton () const
	{ return *m_FoldBackIntoSceneButton; }

	Gtk::Box &
	getBBoxSizeBox () const
	{ return *m_BBoxSizeBox; }

	Gtk::SpinButton &
	getBBoxSizeXSpinButton () const
	{ return *m_BBoxSizeXSpinButton; }

	Gtk::SpinButton &
	getBBoxSizeYSpinButton () const
	{ return *m_BBoxSizeYSpinButton; }

	Gtk::SpinButton &
	getBBoxSizeZSpinButton () const
	{ return *m_BBoxSizeZSpinButton; }

	Gtk::Box &
	getBBoxCenterBox () const
	{ return *m_BBoxCenterBox; }

	Gtk::SpinButton &
	getBBoxCenterXSpinButton () const
	{ return *m_BBoxCenterXSpinButton; }

	Gtk::SpinButton &
	getBBoxCenterYSpinButton () const
	{ return *m_BBoxCenterYSpinButton; }

	Gtk::SpinButton &
	getBBoxCenterZSpinButton () const
	{ return *m_BBoxCenterZSpinButton; }

	Gtk::Label &
	getLoadStateLabel () const
	{ return *m_LoadStateLabel; }

	Gtk::Box &
	getNameBox () const
	{ return *m_NameBox; }

	Gtk::Entry &
	getNameEntry () const
	{ return *m_NameEntry; }

	Gtk::Button &
	getRenameButton () const
	{ return *m_RenameButton; }

	Gtk::Box &
	getURLBox () const
	{ return *m_URLBox; }

	Gtk::TreeView &
	getURLTreeView () const
	{ return *m_URLTreeView; }

	Gtk::Button &
	getURLAddButton () const
	{ return *m_URLAddButton; }

	Gtk::Button &
	getURLRemoveButton () const
	{ return *m_URLRemoveButton; }

	Gtk::Button &
	getURLReloadButton () const
	{ return *m_URLReloadButton; }

	virtual
	void
	on_new_inline_clicked () = 0;

	virtual
	void
	on_remove_inline_clicked () = 0;

	virtual
	void
	on_index_clicked () = 0;

	virtual
	void
	on_convert_master_selection_clicked () = 0;

	virtual
	void
	on_fold_back_into_scene_clicked () = 0;

	virtual
	~X3DInlineEditorInterface ();


private:

	void
	create (const std::string &);

	static const std::string m_widgetName;

	std::string                            filename;
	Glib::RefPtr <Gtk::Builder>            m_builder;
	std::deque <sigc::connection>          m_connections;
	Glib::RefPtr <Gtk::Adjustment>         m_BBoxCenterXAdjustment;
	Glib::RefPtr <Gtk::Adjustment>         m_BBoxCenterYAdjustment;
	Glib::RefPtr <Gtk::Adjustment>         m_BBoxCenterZAdjustment;
	Glib::RefPtr <Gtk::Adjustment>         m_BBoxSizeXAdjustment;
	Glib::RefPtr <Gtk::Adjustment>         m_BBoxSizeYAdjustment;
	Glib::RefPtr <Gtk::Adjustment>         m_BBoxSizeZAdjustment;
	Glib::RefPtr <Gtk::CellRendererText>   m_URLCellRendererText;
	Glib::RefPtr <Gtk::TreeViewColumn>     m_URLChooserColumn;
	Glib::RefPtr <Gtk::CellRendererPixbuf> m_URLCellrendererPixbuf;
	Gtk::Window*                           m_Window;
	Gtk::Box*                              m_Widget;
	Gtk::Box*                              m_InlineActionBox;
	Gtk::Button*                           m_NewInlineButton;
	Gtk::Button*                           m_RemoveInlineButton;
	Gtk::Button*                           m_IndexButton;
	Gtk::Button*                           m_ConvertMasterSelectionButton;
	Gtk::Switch*                           m_UpdateBoundingBoxFieldsSwitch;
	Gtk::Expander*                         m_InlineExpander;
	Gtk::Grid*                             m_InlineBox;
	Gtk::CheckButton*                      m_LoadCheckButton;
	Gtk::Button*                           m_FoldBackIntoSceneButton;
	Gtk::Box*                              m_BBoxSizeBox;
	Gtk::SpinButton*                       m_BBoxSizeXSpinButton;
	Gtk::SpinButton*                       m_BBoxSizeYSpinButton;
	Gtk::SpinButton*                       m_BBoxSizeZSpinButton;
	Gtk::Box*                              m_BBoxCenterBox;
	Gtk::SpinButton*                       m_BBoxCenterXSpinButton;
	Gtk::SpinButton*                       m_BBoxCenterYSpinButton;
	Gtk::SpinButton*                       m_BBoxCenterZSpinButton;
	Gtk::Label*                            m_LoadStateLabel;
	Gtk::Box*                              m_NameBox;
	Gtk::Entry*                            m_NameEntry;
	Gtk::Button*                           m_RenameButton;
	Gtk::Box*                              m_URLBox;
	Gtk::TreeView*                         m_URLTreeView;
	Gtk::Button*                           m_URLAddButton;
	Gtk::Button*                           m_URLRemoveButton;
	Gtk::Button*                           m_URLReloadButton;

};

} // puck
} // titania

#endif
