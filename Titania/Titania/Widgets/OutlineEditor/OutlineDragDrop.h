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

#ifndef __TITANIA_OUTLINE_EDITOR_OUTLINE_DRAG_DROP_H__
#define __TITANIA_OUTLINE_EDITOR_OUTLINE_DRAG_DROP_H__

#include "../../Base/X3DBaseInterface.h"

#include <gtkmm.h>

namespace titania {
namespace puck {

class OutlineTreeViewEditor;

class OutlineDragDrop
{
public:

	OutlineDragDrop (OutlineTreeViewEditor* const);


private:

	/// @name Member access

	X3DBrowserWindow*
	getBrowserWindow () const;

	///  @name Event handlers

	bool
	on_button_press_event (GdkEventButton*);

	bool
	on_drag_motion (const Glib::RefPtr <Gdk::DragContext> &, int, int, guint);
	
	bool
	on_drag_motion_extern_proto (const Glib::RefPtr <Gdk::DragContext> &, int, int, guint);

	bool
	on_drag_motion_base_node (const Glib::RefPtr <Gdk::DragContext> &, int, int, guint);

	void
	on_drag_data_received (const Glib::RefPtr <Gdk::DragContext>&, int, int, const Gtk::SelectionData &, guint info, guint);

	void
	on_drag_data_extern_proto_received (const Glib::RefPtr <Gdk::DragContext>&, int, int, const Gtk::SelectionData &, guint info, guint);

	void
	on_drag_data_base_node_insert_into_node_received (const Glib::RefPtr <Gdk::DragContext>&, int, int, const Gtk::SelectionData &, guint info, guint);

	void
	on_drag_data_base_node_on_field_received (const Glib::RefPtr <Gdk::DragContext>&, int, int, const Gtk::SelectionData &, guint info, guint);

	void
	on_drag_data_base_node_insert_into_array_received (const Glib::RefPtr <Gdk::DragContext>&, int, int, const Gtk::SelectionData &, guint info, guint);

	void
	remove_source_node (X3D::SFNode* const, X3D::X3DFieldDefinition* const, size_t, const bool, const X3D::UndoStepPtr &);

	const char*
	get_node_action_string (Gdk::DragAction) const;

	///  @name Static members

	static const std::string dragDataType;

	///  @name Members

	OutlineTreeViewEditor* const treeView;
	Gtk::TreePath                sourcePath;

};

} // puck
} // titania

#endif
