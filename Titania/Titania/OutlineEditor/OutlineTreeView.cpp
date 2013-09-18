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

#include "OutlineTreeView.h"

#include "../Browser/BrowserWindow.h"
#include "../Configuration/config.h"
#include "../OutlineEditor/CellRenderer/OutlineCellRenderer.h"
#include "../OutlineEditor/OutlineTreeModel.h"

namespace titania {
namespace puck {

OutlineTreeView::OutlineTreeView (BrowserWindow* const browserWindow) :
	           Glib::ObjectBase (typeid (OutlineTreeView)),
	           X3DBaseInterface (browserWindow),
	              Gtk::TreeView (),
	X3DOutlineTreeViewInterface (get_ui ("OutlineTreeView.ui"), gconf_dir ()),
	                      model (),
	                  selection (browserWindow, this),
	                expandLevel (0)
{
	// Options

	set_headers_visible (true);
	set_enable_search (false);
	Gtk::TreeView::get_selection () -> set_mode (Gtk::SELECTION_NONE);
	set_name ("OutlineTreeView");

	// Color

	auto selectedForegroundColor = get_style_context () -> get_color (Gtk::STATE_FLAG_SELECTED);
	auto selectedBackgroundColor = get_style_context () -> get_background_color (Gtk::STATE_FLAG_SELECTED);

	// Columns

	Gtk::TreeViewColumn* treeviewcolumn_name = Gtk::manage (new Gtk::TreeViewColumn ("Tree"));
	treeviewcolumn_name -> set_expand (false);

	// CellRenderer
	OutlineCellRenderer* cellrenderer = Gtk::manage (new OutlineCellRenderer (getBrowser ()));
	treeviewcolumn_name -> pack_start (*cellrenderer, false);
	treeviewcolumn_name -> add_attribute (*cellrenderer, "tree-data",           OutlineTreeModel::DATA_COLUMN);
	treeviewcolumn_name -> add_attribute (*cellrenderer, "foreground-set",      OutlineTreeModel::SELECTED_COLUMN);
	treeviewcolumn_name -> add_attribute (*cellrenderer, "cell-background-set", OutlineTreeModel::SELECTED_COLUMN);
	cellrenderer -> property_foreground_rgba ()      = selectedForegroundColor;
	cellrenderer -> property_cell_background_rgba () = selectedBackgroundColor;
	cellrenderer -> signal_edited () .connect (sigc::mem_fun (this, &OutlineTreeView::on_edited));

	// Append column

	append_column (*treeviewcolumn_name);

	// Register browser interest

	getBrowser () -> initialized () .addInterest (this, &OutlineTreeView::set_world);
}

void
OutlineTreeView::set_model (const Glib::RefPtr <OutlineTreeModel> & value)
{
	model = value;
	Gtk::TreeView::set_model (value);
}

std::deque <Gtk::TreeModel::iterator>
OutlineTreeView::get_iters (X3D::X3DChildObject* const object) const
{
	return get_model () -> get_iters (object);
}

OutlineUserDataPtr
OutlineTreeView::get_user_data (const Gtk::TreeModel::iterator & iter) const
{
	return get_model () -> get_user_data (iter);
}

OutlineUserDataPtr
OutlineTreeView::get_user_data (X3D::X3DChildObject* const object) const
{
	return get_model () -> get_user_data (object);
}

OutlineIterType
OutlineTreeView::get_data_type (const Gtk::TreeModel::iterator & iter) const
{
	return get_model () -> get_data_type (iter);
}

X3D::X3DChildObject*
OutlineTreeView::get_object (const Gtk::TreeModel::iterator & iter) const
{
	return get_model () -> get_object (iter);
}

void
OutlineTreeView::set_path (const Gtk::TreeModel::iterator & iter, const Gtk::TreeModel::Path & value)
{
	auto userData = get_user_data (iter);

	if (userData)
		userData -> path = value;
}

Gtk::TreeModel::Path
OutlineTreeView::get_path (const Gtk::TreeModel::iterator & iter) const
{
	auto userData = get_user_data (iter);

	if (userData)
		return userData -> path;

	return Gtk::TreeModel::Path ();
}

void
OutlineTreeView::set_expanded (const Gtk::TreeModel::iterator & iter, bool value)
{
	auto userData = get_user_data (iter);

	if (userData)
		userData -> expanded = value;
}

bool
OutlineTreeView::get_expanded (const Gtk::TreeModel::iterator & iter) const
{
	auto userData = get_user_data (iter);

	if (userData)
		return userData -> expanded;

	return false;
}

void
OutlineTreeView::set_all_expanded (const Gtk::TreeModel::iterator & iter, bool value)
{
	auto userData = get_user_data (iter);

	if (userData)
		userData -> all_expanded = value;
}

bool
OutlineTreeView::get_all_expanded (const Gtk::TreeModel::iterator & iter) const
{
	auto userData = get_user_data (iter);

	if (userData)
		return userData -> all_expanded;

	return false;
}

bool
OutlineTreeView::get_shift_key ()
{
	return expandLevel == 0 and getBrowserWindow () -> getKeys () .shift ();
}

void
OutlineTreeView::set_world ()
{
	selection .clear ();

	set_model (OutlineTreeModel::create (getBrowserWindow ()));

	getBrowser () -> getExecutionContext () -> getRootNodes () .addInterest (this, &OutlineTreeView::set_rootNodes);

	set_rootNodes ();
}

void
OutlineTreeView::set_rootNodes ()
{
	//__LOG__ << std::endl;

	for (const auto & column : get_columns ())
		column -> set_visible (false);

	for (const auto & child : get_model () -> children ())
		unwatch_tree (child);

	unset_model ();
	get_model () -> clear ();

	for (auto & rootNode : getBrowser () -> getExecutionContext () -> getRootNodes ())
		get_model () -> append (OutlineIterType::X3DBaseNode, &rootNode);

	set_model (get_model ());

	++ expandLevel; // Disable shift key

	for (auto & iter : get_model () -> children ())
	{
		// Expand row again if it was previously expanded.

		if (get_expanded (iter))
			expand_row (get_model () -> get_path (iter), false);
	}

	-- expandLevel; // Enable shift key

	for (const auto & column : get_columns ())
		column -> set_visible (true);
}

void
OutlineTreeView::on_rename_node_activate ()
{
	//__LOG__ << std::endl;

	//
	//	Gtk::TreeModel::Path path;
	//	Gtk::TreeViewColumn* column = nullptr;
	//	int                  cell_x = 0;
	//	int                  cell_y = 0;
	//
	//	get_path_at_pos (x, y, path, column, cell_x, cell_y);
	//
	//	if (path .size ())
	//	{
	//		Gtk::TreeModel::iterator iter = get_model () -> get_iter (path);
	//
	//		if (get_data_type (iter) == OutlineIterType::X3DBaseNode)
	//		{
	//			set_cursor (path, *column, true);
	//			return;
	//		}
	//	}
}

void
OutlineTreeView::on_edited (const Glib::ustring & string_path, const Glib::ustring & text)
{
	Gtk::TreeModel::Path     path (string_path);
	Gtk::TreeModel::iterator iter = get_model () -> get_iter (path);

	get_model () -> row_changed (path, iter);
	watch (iter, path);

	getBrowserWindow () -> setEdited (true);
}

bool
OutlineTreeView::on_key_press_event (GdkEventKey* event)
{
	return Gtk::TreeView::on_key_press_event (event);
}

bool
OutlineTreeView::on_key_release_event (GdkEventKey* event)
{
	return Gtk::TreeView::on_key_release_event (event);
}

bool
OutlineTreeView::on_button_press_event (GdkEventButton* event)
{
	switch (event -> button)
	{
		case 1:
		{
			if (select_field (event -> x, event -> y))
				return true;

			break;
		}
		case 3:
		{
			//			getPopupMenu () .popup (event -> button, event -> time);
			//			return true;
			break;
		}
		default:
			break;
	}

	return Gtk::TreeView::on_button_press_event (event);
}

void
OutlineTreeView::on_row_activated (const Gtk::TreeModel::Path & path, Gtk::TreeViewColumn* column)
{
	select_node (get_model () -> get_iter (path), path);

	set_cursor (path, *column, true);
}

bool
OutlineTreeView::on_test_expand_row (const Gtk::TreeModel::iterator & iter, const Gtk::TreeModel::Path & path)
{
	collapse_clone (iter);

	expand_row (iter); // (model expand_row)

	// Return false to allow expansion, true to reject.
	return false;
}

void
OutlineTreeView::on_row_expanded (const Gtk::TreeModel::iterator & iter, const Gtk::TreeModel::Path & path)
{
	// Set expanded first to prevent loop with clones.
	set_expanded (iter, true);
	set_path (iter, path);

	watch_expanded (iter, path);

	auto_expand (iter);
}

bool
OutlineTreeView::on_test_collapse_row (const Gtk::TreeModel::iterator & iter, const Gtk::TreeModel::Path & path)
{
	// Return false to allow collapse, true to reject.
	return false;
}

void
OutlineTreeView::on_row_collapsed (const Gtk::TreeModel::iterator & iter, const Gtk::TreeModel::Path & path)
{
	//__LOG__ << std::endl;

	unwatch_tree (iter);
	get_model () -> clear (iter);

	set_expanded (iter, false);
	set_all_expanded (iter, false);

	toggle_expand (iter, path);
}

void
OutlineTreeView::watch_expanded (const Gtk::TreeModel::iterator & iter, const Gtk::TreeModel::Path & path)
{
	switch (get_data_type (iter))
	{
		case OutlineIterType::X3DField:
		{
			auto field = static_cast <X3D::X3DFieldDefinition*> (get_object (iter));

			if (get_all_expanded (iter))
			{
				field -> getInputRoutes ()  .addInterest (this, &OutlineTreeView::toggle_field, path);
				field -> getOutputRoutes () .addInterest (this, &OutlineTreeView::toggle_field, path);
			}

			switch (field -> getType ())
			{
				case X3D::X3DConstants::SFNode:
				{
					field -> addInterest (this, &OutlineTreeView::update_field, path);
					break;
				}
				case X3D::X3DConstants::MFNode:
				{
					field -> addInterest (this, &OutlineTreeView::update_field, path);
					break;
				}
				default:
					break;
			}

			break;
		}
		default:
			break;
	}

	watch_children (iter);
}

void
OutlineTreeView::watch_children (const Gtk::TreeModel::iterator & parent)
{
	for (const auto & child : parent -> children ())
		watch (child, get_model () -> get_path (child));
}

void
OutlineTreeView::watch (const Gtk::TreeModel::iterator & iter, const Gtk::TreeModel::Path & path)
{
	switch (get_data_type (iter))
	{
		case OutlineIterType::X3DInputRoute:
		case OutlineIterType::X3DOutputRoute:
			break;

		case OutlineIterType::X3DFieldValue:
		{
			auto field = static_cast <X3D::X3DFieldDefinition*> (get_object (iter));

			field -> addInterest (this, &OutlineTreeView::on_row_changed, path);

			break;
		}
		case OutlineIterType::X3DField:
		{
			auto field = static_cast <X3D::X3DFieldDefinition*> (get_object (iter));

			switch (field -> getType ())
			{
				case X3D::X3DConstants::MFNode:
				{
					X3D::MFNode* mfnode = static_cast <X3D::MFNode*> (field);

					mfnode -> addInterest (this, &OutlineTreeView::on_row_has_child_toggled, path);

					break;
				}
				default:
					break;
			}

			break;
		}
		case OutlineIterType::X3DBaseNode:
			break;
	}
}

void
OutlineTreeView::unwatch_tree (const Gtk::TreeModel::iterator & iter, bool root)
{
	unwatch (iter, root);

	for (const auto & child : iter -> children ())
		unwatch_tree (child, false);
}

void
OutlineTreeView::unwatch (const Gtk::TreeModel::iterator & iter, bool root)
{
	switch (get_data_type (iter))
	{
		case OutlineIterType::X3DInputRoute:
		case OutlineIterType::X3DOutputRoute:
			break;

		case OutlineIterType::X3DFieldValue:
		{
			auto field = static_cast <X3D::X3DFieldDefinition*> (get_object (iter));

			field -> removeInterest (this, &OutlineTreeView::on_row_changed);

			break;
		}
		case OutlineIterType::X3DField:
		{
			auto field = static_cast <X3D::X3DFieldDefinition*> (get_object (iter));

			field -> getInputRoutes ()  .removeInterest (this, &OutlineTreeView::toggle_field);
			field -> getOutputRoutes () .removeInterest (this, &OutlineTreeView::toggle_field);

			switch (field -> getType ())
			{
				case X3D::X3DConstants::SFNode:
				{
					field -> removeInterest (this, &OutlineTreeView::update_field);
					break;
				}
				case X3D::X3DConstants::MFNode:
				{
					field -> removeInterest (this, &OutlineTreeView::update_field);

					if (not root)
						field -> removeInterest (this, &OutlineTreeView::on_row_has_child_toggled);

					break;
				}
				default:
					break;
			}

			break;
		}
		case OutlineIterType::X3DBaseNode:
		{
			set_path (iter, Gtk::TreeModel::Path ());
			break;
		}
	}
}

void
OutlineTreeView::on_row_has_child_toggled (const Gtk::TreeModel::Path & path)
{
	get_model () -> row_has_child_toggled (path, get_model () -> get_iter (path));
}

void
OutlineTreeView::on_row_changed (const Gtk::TreeModel::Path & path)
{
	//__LOG__ << X3D::SFTime (chrono::now ()) << std::endl;

	get_model () -> row_changed (path, get_model () -> get_iter (path));
}

void
OutlineTreeView::update_field (const Gtk::TreeModel::Path & path)
{
	//__LOG__ << std::endl;

	Gtk::TreeModel::iterator iter         = get_model () -> get_iter (path);
	bool                     all_expanded = get_all_expanded (iter);

	for (const auto & column : get_columns ())
		column -> set_visible (false);

	collapse_row (path);

	get_model () -> row_has_child_toggled (path, iter);

	++ expandLevel; // Disable shift key

	set_all_expanded (iter, all_expanded);
	expand_row (path, false);

	-- expandLevel; // Enable shift key

	for (const auto & column : get_columns ())
		column -> set_visible (true);
}

void
OutlineTreeView::toggle_field (const Gtk::TreeModel::Path & path)
{
	//__LOG__ << std::endl;

	Gtk::TreeModel::iterator iter = get_model () -> get_iter (path);

	Gtk::TreeView::collapse_row (path);
	get_model () -> row_has_child_toggled (path, iter);

	++ expandLevel; // Disable shift key

	set_all_expanded (iter, true);
	expand_row (path, false);

	-- expandLevel; // Enable shift key
}

void
OutlineTreeView::collapse_clone (const Gtk::TreeModel::iterator & iter)
{
	if (get_data_type (iter) == OutlineIterType::X3DBaseNode)
		collapse_row (get_path (iter));
}

void
OutlineTreeView::collapse_row (const Gtk::TreeModel::Path & path)
{
	if (row_expanded (path))
		Gtk::TreeView::collapse_row (path);
}

void
OutlineTreeView::expand_row (const Gtk::TreeModel::iterator & iter)
{
	switch (get_data_type (iter))
	{
		case OutlineIterType::X3DInputRoute:
		case OutlineIterType::X3DOutputRoute:
		case OutlineIterType::X3DFieldValue:
			break;

		case OutlineIterType::X3DField:
		{
			auto field = static_cast <X3D::X3DFieldDefinition*> (get_object (iter));

			if (get_shift_key () or get_all_expanded (iter))
			{
				expand_routes (iter, field);
				set_all_expanded (iter, true);
			}
			else
				set_all_expanded (iter, false);

			switch (field -> getType ())
			{
				case X3D::X3DConstants::SFNode:
				{
					get_model () -> append (iter, OutlineIterType::X3DBaseNode, field);
					break;
				}
				case X3D::X3DConstants::MFNode:
				{
					auto mfnode = static_cast <X3D::MFNode*> (field);

					if (mfnode -> empty () and not get_all_expanded (iter))
						expand_routes (iter, field);

					for (auto & value :* mfnode)
						get_model () -> append (iter, OutlineIterType::X3DBaseNode, &value);

					break;
				}
				default:
				{
					get_model () -> append (iter, OutlineIterType::X3DFieldValue, field);
					break;
				}
			}

			break;
		}
		case OutlineIterType::X3DBaseNode:
		{
			auto nodeUserData = get_user_data (iter);

			if (nodeUserData)
			{
				auto sfnode = static_cast <X3D::SFNode*> (get_object (iter));
				auto node   = sfnode -> getValue ();

				if (node)
				{
					// Select visible fields

					if (get_shift_key () or get_all_expanded (iter))
					{
						for (const auto & field : get_fields (node))
							get_model () -> append (iter, OutlineIterType::X3DField, field);

						set_all_expanded (iter, true);
					}
					else
					{
						bool visibles = false;

						for (const auto & field : get_fields (node))
						{
							if (not field -> isInitializeable () or node -> isDefaultValue (field))
							{
								if (field -> getInputRoutes () .empty () and field -> getOutputRoutes () .empty ())
									continue;
							}

							get_model () -> append (iter, OutlineIterType::X3DField, field);
							visibles = true;
						}

						if (not visibles)
						{
							for (const auto & field : get_fields (node))
								get_model () -> append (iter, OutlineIterType::X3DField, field);
						}

						set_all_expanded (iter, false);
					}
				}
			}
		}
	}
}

X3D::FieldDefinitionArray
OutlineTreeView::get_fields (X3D::X3DBaseNode* const node) const
{
	auto fields            = node -> getPreDefinedFields ();
	auto userDefinedFields = node -> getUserDefinedFields ();

	if (dynamic_cast <X3D::X3DNode*> (node))
		fields .insert (fields .begin () + 1, userDefinedFields .begin (), userDefinedFields .end ());

	else
		fields .insert (fields .begin (), userDefinedFields .begin (), userDefinedFields .end ());

	return fields;
}

void
OutlineTreeView::expand_routes (const Gtk::TreeModel::iterator & iter, X3D::X3DFieldDefinition* field)
{
	for (const auto & route : field -> getInputRoutes ())
		get_model () -> append (iter, OutlineIterType::X3DInputRoute, route);

	for (const auto & route : field -> getOutputRoutes ())
		get_model () -> append (iter, OutlineIterType::X3DOutputRoute, route);
}

void
OutlineTreeView::toggle_expand (const Gtk::TreeModel::iterator & iter, const Gtk::TreeModel::Path & path)
{
	switch (get_data_type (iter))
	{
		case OutlineIterType::X3DField:
		case OutlineIterType::X3DBaseNode:
		{
			if (get_shift_key ())
				expand_row (path, false);

			break;
		}
		default:
			break;
	}
}

void
OutlineTreeView::auto_expand (const Gtk::TreeModel::iterator & parent)
{
	++ expandLevel;

	switch (get_data_type (parent))
	{
		case OutlineIterType::X3DInputRoute:
		case OutlineIterType::X3DOutputRoute:
		case OutlineIterType::X3DFieldValue:
			break;

		case OutlineIterType::X3DField:
		{
			for (const auto & child : parent -> children ())
			{
				switch (get_data_type (child))
				{
					case OutlineIterType::X3DBaseNode :
						{
							if (get_expanded (child))
							{
								if (get_path (child) .empty () or not get_path (child) .is_ancestor (get_model () -> get_path (child)))
								{
									expand_row (Gtk::TreePath (child), false);
								}
							}

							break;
						}
					default:
						break;
				}
			}

			break;
		}
		case OutlineIterType::X3DBaseNode:
		{
			for (const auto & child : parent -> children ())
			{
				auto field = static_cast <X3D::X3DFieldDefinition*> (get_object (child));

				switch (field -> getType ())
				{
					case X3D::X3DConstants::SFNode:
					{
						auto sfnode = static_cast <X3D::SFNode*> (field);

						if ((field -> isInitializeable () and * sfnode) or get_expanded (child))
						{
							expand_row (Gtk::TreePath (child), false);
						}

						break;
					}
					case X3D::X3DConstants::MFNode:
					{
						auto mfnode = static_cast <X3D::MFNode*> (field);

						if (mfnode -> size () and (field -> isInitializeable () or get_expanded (child)))
						{
							expand_row (Gtk::TreePath (child), false);
						}

						break;
					}
					default:
					{
						if (get_expanded (child))
						{
							expand_row (Gtk::TreePath (child), false);
						}

						break;
					}
				}
			}
		}
	}

	-- expandLevel;
}

void
OutlineTreeView::select_node (const Gtk::TreeModel::iterator & iter, const Gtk::TreeModel::Path & path)
{
	if (get_data_type (iter) == OutlineIterType::X3DBaseNode)
	{
		selection .set_select_multiple (get_shift_key ());

		selection .select (*static_cast <X3D::SFNode*> (get_object (iter)));
	}
}

bool
OutlineTreeView::select_field (int x, int y)
{
	Gtk::TreeModel::Path path;
	Gtk::TreeViewColumn* column = nullptr;
	int                  cell_x = 0;
	int                  cell_y = 0;

	get_path_at_pos (x, y, path, column, cell_x, cell_y);

	if (path .size ())
	{
		Gtk::TreeModel::iterator iter = get_model () -> get_iter (path);

		if (get_data_type (iter) == OutlineIterType::X3DFieldValue)
		{
			unwatch_tree (iter);
			set_cursor (path, *column, true);
			return true;
		}
	}

	return false;
}

OutlineTreeView::~OutlineTreeView ()
{ }

} // puck
} // titania
