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

#include "NodePropertiesEditor.h"

#include "../Browser/BrowserWindow.h"
#include "../Configuration/config.h"

namespace titania {
namespace puck {

const std::string NodePropertiesEditor::userDefinedFieldsDragDataType = "titania/node-properties/user-defined-field";

NodePropertiesEditor::NodePropertiesEditor (BrowserWindow* const browserWindow, const X3D::SFNode & node) :
	                X3DBaseInterface (browserWindow, browserWindow -> getBrowser ()),
	X3DNodePropertiesEditorInterface (get_ui ("Dialogs/NodePropertiesEditor.xml"), gconf_dir ()),
	                            node (node),
	        userDefinedFieldsColumns (),
	      userDefinedFieldsListStore (Gtk::ListStore::create (userDefinedFieldsColumns)),
	                userDefinedField (nullptr),
	               userDefinedFields (),
	                          fields (),
	                 fieldsToReplace (),
	                  fieldsToRemove (),
	            editUserDefinedField (false)
{
	getWindow () .set_transient_for (getBrowserWindow () -> getWindow ());

	getHeaderLabel ()   .set_text (node -> getTypeName () + " »" + node -> getName () + "«");
	getTypeNameEntry () .set_text (node -> getTypeName ());
	getNameEntry ()     .set_text (node -> getName ());

	// User defined fields

	getUserDefinedFieldsExpander () .set_visible (node -> hasUserDefinedFields ());
	getUserDefinedFieldsTreeView () .get_selection () -> set_mode (Gtk::SELECTION_SINGLE);
	getUserDefinedFieldsTreeView () .set_model (userDefinedFieldsListStore);

	for (const auto & field : node -> getPreDefinedFields ())
		addField (field);

	for (const auto & field : node -> getUserDefinedFields ())
		addUserDefinedField (field);

	getCellRendererAccessType () -> property_xalign () = 0;
	getCellRendererAccessType () -> property_xpad ()   = 4;

	getInitializeOnlyMenuItem () .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_access_type_activate), "initializeOnly"));
	getInputOnlyMenuItem ()      .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_access_type_activate), "inputOnly"));
	getOutputOnlyMenuItem ()     .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_access_type_activate), "outputOnly"));
	getInputOutputMenuItem ()    .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_access_type_activate), "inputOutput"));

	getSFBoolMenuItem ()      .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "SFBool"));
	getSFColorRGBAMenuItem () .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "SFColorRGBA"));
	getSFColorMenuItem ()     .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "SFColor"));
	getSFDoubleMenuItem ()    .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "SFDouble"));
	getSFFloatMenuItem ()     .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "SFFloat"));
	getSFImageMenuItem ()     .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "SFImage"));
	getSFInt32MenuItem ()     .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "SFInt32"));
	getSFMatrix3dMenuItem ()  .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "SFMatrix3d"));
	getSFMatrix3fMenuItem ()  .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "SFMatrix3f"));
	getSFMatrix4dMenuItem ()  .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "SFMatrix4d"));
	getSFMatrix4fMenuItem ()  .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "SFMatrix4f"));
	getSFNodeMenuItem ()      .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "SFNode"));
	getSFRotationMenuItem ()  .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "SFRotation"));
	getSFStringMenuItem ()    .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "SFString"));
	getSFTimeMenuItem ()      .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "SFTime"));
	getSFVec2dMenuItem ()     .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "SFVec2d"));
	getSFVec2fMenuItem ()     .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "SFVec2f"));
	getSFVec3dMenuItem ()     .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "SFVec3d"));
	getSFVec3fMenuItem ()     .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "SFVec3f"));
	getSFVec4dMenuItem ()     .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "SFVec4d"));
	getSFVec4fMenuItem ()     .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "SFVec4f"));

	getMFBoolMenuItem ()      .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "MFBool"));
	getMFColorRGBAMenuItem () .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "MFColorRGBA"));
	getMFColorMenuItem ()     .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "MFColor"));
	getMFDoubleMenuItem ()    .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "MFDouble"));
	getMFFloatMenuItem ()     .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "MFFloat"));
	getMFImageMenuItem ()     .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "MFImage"));
	getMFInt32MenuItem ()     .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "MFInt32"));
	getMFMatrix3dMenuItem ()  .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "MFMatrix3d"));
	getMFMatrix3fMenuItem ()  .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "MFMatrix3f"));
	getMFMatrix4dMenuItem ()  .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "MFMatrix4d"));
	getMFMatrix4fMenuItem ()  .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "MFMatrix4f"));
	getMFNodeMenuItem ()      .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "MFNode"));
	getMFRotationMenuItem ()  .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "MFRotation"));
	getMFStringMenuItem ()    .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "MFString"));
	getMFTimeMenuItem ()      .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "MFTime"));
	getMFVec2dMenuItem ()     .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "MFVec2d"));
	getMFVec2fMenuItem ()     .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "MFVec2f"));
	getMFVec3dMenuItem ()     .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "MFVec3d"));
	getMFVec3fMenuItem ()     .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "MFVec3f"));
	getMFVec4dMenuItem ()     .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "MFVec4d"));
	getMFVec4fMenuItem ()     .signal_activate () .connect (sigc::bind <std::string> (sigc::mem_fun (*this, &NodePropertiesEditor::on_field_type_activate), "MFVec4f"));

	//  Drag & Drop

	// Drag targets
	getUserDefinedFieldsTreeView () .enable_model_drag_source ({
	                                                              Gtk::TargetEntry (userDefinedFieldsDragDataType, Gtk::TARGET_SAME_WIDGET)
																				  }, Gdk::BUTTON1_MASK, Gdk::ACTION_MOVE);

	// Drop targets
	getUserDefinedFieldsTreeView () .enable_model_drag_dest ({
	                                                            Gtk::TargetEntry (userDefinedFieldsDragDataType, Gtk::TARGET_SAME_WIDGET)
																				}, Gdk::ACTION_MOVE);
}

void
NodePropertiesEditor::validateIdOnInsert (Gtk::Entry & entry, const Glib::ustring & insert, int position)
{
	const std::string text = entry .get_text () .insert (position, insert);

	if (not X3D::RegEx::Id .FullMatch (text))
		entry .signal_insert_text () .emission_stop ();
}

void
NodePropertiesEditor::validateIdOnDelete (Gtk::Entry & entry, int start_pos, int end_pos)
{
	const std::string text = entry .get_text () .erase (start_pos, end_pos - start_pos);

	if (text .length () and not X3D::RegEx::Id .FullMatch (text))
		entry .signal_delete_text () .emission_stop ();
}

void
NodePropertiesEditor::on_type_name_insert_text (const Glib::ustring & text, int* position)
{
	validateIdOnInsert (getTypeNameEntry (), text, *position);
}

void
NodePropertiesEditor::on_type_name_delete_text (int start_pos, int end_pos)
{
	validateIdOnDelete (getTypeNameEntry (), start_pos, end_pos);
}

void
NodePropertiesEditor::on_name_insert_text (const Glib::ustring & text, int* position)
{
	validateIdOnInsert (getNameEntry (), text, *position);
}

void
NodePropertiesEditor::on_name_delete_text (int start_pos, int end_pos)
{
	validateIdOnDelete (getNameEntry (), start_pos, end_pos);
}

void
NodePropertiesEditor::on_user_defined_field_changed ()
{
	getRemoveFieldButton () .set_sensitive (not getUserDefinedFieldsTreeView () .get_selection () -> get_selected_rows () .empty ());
}

void
NodePropertiesEditor::on_drag_data_received (const Glib::RefPtr <Gdk::DragContext> & context,
                                             int x, int y,
                                             const Gtk::SelectionData & selection_data,
                                             guint info,
                                             guint time)
{
	const auto   selected = getUserDefinedFieldsTreeView () .get_selection () -> get_selected ();
	const size_t index    = userDefinedFieldsListStore -> get_path (selected) .front ();

	X3D::X3DFieldDefinition* const field = userDefinedFields [index];

	Gtk::TreeModel::Path      destinationPath;
	Gtk::TreeViewDropPosition position;
	size_t                    dest = 0;

	if (getUserDefinedFieldsTreeView () .get_dest_row_at_pos (x, y, destinationPath, position))
	{
		auto destination = userDefinedFieldsListStore -> get_iter (destinationPath);
		dest = userDefinedFieldsListStore -> get_path (destination) .front ();

		switch (position)
		{
			case Gtk::TREE_VIEW_DROP_BEFORE:
			case Gtk::TREE_VIEW_DROP_INTO_OR_BEFORE:
				userDefinedFieldsListStore -> move (selected, destination);
				break;
			case Gtk::TREE_VIEW_DROP_AFTER:
			case Gtk::TREE_VIEW_DROP_INTO_OR_AFTER:
				userDefinedFieldsListStore -> move (selected, ++ destination);
				++ dest;
				break;
		}
	}
	else
	{
		const auto children = userDefinedFieldsListStore -> children ();
		userDefinedFieldsListStore -> move (selected, children .end ());
		dest = children .size ();
	}

	if (index < dest)
	{
		userDefinedFields .insert (userDefinedFields .begin () + dest, field);
		userDefinedFields .erase (userDefinedFields .begin () + index);
	}
	else
	{
		userDefinedFields .erase (userDefinedFields .begin () + index);
		userDefinedFields .insert (userDefinedFields .begin () + dest, field);
	}

	context -> drag_finish (false, false, time);
}

void
NodePropertiesEditor::on_user_defined_field_activated (const Gtk::TreeModel::Path & path, Gtk::TreeViewColumn* column)
{
	editUserDefinedField = true;
	userDefinedField     = userDefinedFields [path .front ()];

	getAccessTypeLabel () .set_text (X3D::Generator::X3DAccessTypes [userDefinedField]);
	getFieldTypeLabel () .set_text (userDefinedField -> getTypeName ());
	getFieldNameEntry () .set_text (userDefinedField -> getName ());

	getAddFieldDialog () .set_title (_ ("Edit User Defined Field"));
	getAddFieldDialog () .present ();

	getFieldNameEntry () .grab_focus ();
}

void
NodePropertiesEditor::on_add_field_clicked ()
{
	editUserDefinedField = false;

	getInitializeOnlyMenuItem () .activate ();
	getSFBoolMenuItem () .activate ();
	getFieldNameEntry () .set_text ("");

	getAddFieldDialog () .set_title (_ ("Add User Defined Field"));
	getAddFieldDialog () .present ();

	getFieldNameEntry () .grab_focus ();
}

void
NodePropertiesEditor::on_remove_field_clicked ()
{
	const auto selected = getUserDefinedFieldsTreeView () .get_selection () -> get_selected ();
	const auto index    = userDefinedFieldsListStore -> get_path (selected) .front ();

	X3D::X3DFieldDefinition* const field = userDefinedFields [index];

	// Erase from list store

	userDefinedFieldsListStore -> erase (selected);

	// Erase from index

	fields .erase (field -> getName ());

	if (field -> getAccessType () == X3D::inputOutput)
	{
		fields .erase ("set_" + field -> getName ());
		fields .erase (field -> getName () + "_changed");
	}

	// Erase from array

	const auto iter = std::find (userDefinedFields .begin (),
	                             userDefinedFields .end (),
	                             field);

	if (iter not_eq userDefinedFields .end ())
		userDefinedFields .erase (iter);

	// Store node field or delete field immediately

	try
	{
		fieldsToRemove .emplace_back (node -> getField (field -> getName ()));
	}
	catch (const X3D::X3DError &)
	{
		// Delete field immediately if field is not a child of node.
		delete field;
	}
}

void
NodePropertiesEditor::on_access_type_activate (const std::string & accessType)
{
	getAccessTypeLabel () .set_text (accessType);

	on_field_name_changed ();
}

void
NodePropertiesEditor::on_field_type_activate (const std::string & fieldType)
{
	getFieldTypeLabel () .set_text (fieldType);
}

void
NodePropertiesEditor::on_field_name_insert_text (const Glib::ustring & text, int* position)
{
	validateIdOnInsert (getFieldNameEntry (), text, *position);
}

void
NodePropertiesEditor::on_field_name_delete_text (int start_pos, int end_pos)
{
	validateIdOnDelete (getFieldNameEntry (), start_pos, end_pos);
}

void
NodePropertiesEditor::on_field_name_changed ()
{
	// Set Ok button sensitive or not.

	const std::string name   = getFieldNameEntry () .get_text ();
	bool              exists = existsField (name);

	if (getAccessTypeLabel () .get_text () == "inputOutput")
	{
		exists |= existsField ("set_" + name);
		exists |= existsField (name + "_changed");
	}

	getAddFieldOkButton () .set_sensitive (not name .empty () and not exists);
}

void
NodePropertiesEditor::on_add_field_ok_clicked ()
{
	static const std::map <std::string, X3D::AccessType> accessTypes = {
		std::make_pair ("initializeOnly", X3D::initializeOnly),
		std::make_pair ("inputOnly",      X3D::inputOnly),
		std::make_pair ("outputOnly",     X3D::outputOnly),
		std::make_pair ("inputOutput",    X3D::inputOutput)
	};

	getAddFieldDialog () .hide ();

	if (editUserDefinedField)
	{
		// Edit field

		X3D::X3DFieldDefinition* const field = X3D::getBrowser () -> getFieldType (getFieldTypeLabel () .get_text ()) -> create ();

		field -> setName (getFieldNameEntry () .get_text ());
		field -> setAccessType (accessTypes .at (getAccessTypeLabel () .get_text ()));

		replaceUserDefinedField (userDefinedField, field);
	}
	else
	{
		// Create field

		X3D::X3DFieldDefinition* const field = X3D::getBrowser () -> getFieldType (getFieldTypeLabel () .get_text ()) -> create ();

		field -> setName (getFieldNameEntry () .get_text ());
		field -> setAccessType (accessTypes .at (getAccessTypeLabel () .get_text ()));

		// Add user defined field

		addUserDefinedField (field);
	}
}

void
NodePropertiesEditor::on_add_field_cancel_clicked ()
{
	getAddFieldDialog () .hide ();
}

bool
NodePropertiesEditor::existsField (const std::string & name) const
{
	const auto iter = fields .find (name);

	if (iter not_eq fields .end ())
	{
		if (editUserDefinedField)
		{
			if (iter -> second not_eq userDefinedField)
				return true;
		}
		else
			return true;
	}

	return false;
}

void
NodePropertiesEditor::addField (X3D::X3DFieldDefinition* const field)
{
	fields .emplace (field -> getName (), field);

	if (field -> getAccessType () == X3D::inputOutput)
	{
		fields .emplace ("set_" + field -> getName (), field);
		fields .emplace (field -> getName () + "_changed", field);
	}
}

void
NodePropertiesEditor::addUserDefinedField (X3D::X3DFieldDefinition* const field)
{
	addField (field);

	userDefinedFields .emplace_back (field);

	const auto iter = userDefinedFieldsListStore -> append ();
	(*iter) [userDefinedFieldsColumns .type]       = Gdk::Pixbuf::create_from_file (get_ui ("icons/FieldType/" + field -> getTypeName () + ".svg"));
	(*iter) [userDefinedFieldsColumns .name]       = field -> getName ();
	(*iter) [userDefinedFieldsColumns .accessType] = Gdk::Pixbuf::create_from_file (get_ui ("icons/AccessType/" + X3D::Generator::X3DAccessTypes [field] + ".png"));
}

void
NodePropertiesEditor::replaceUserDefinedField (X3D::X3DFieldDefinition* const oldField, X3D::X3DFieldDefinition* const newField)
{
	const size_t index = userDefinedFields .begin () - std::find (userDefinedFields .begin (),
	                                                              userDefinedFields .end (),
	                                                              oldField);

	// Replace in list store

	const auto iter = userDefinedFieldsListStore -> children () [index];

	(*iter) [userDefinedFieldsColumns .type]       = Gdk::Pixbuf::create_from_file (get_ui ("icons/FieldType/" + newField -> getTypeName () + ".svg"));
	(*iter) [userDefinedFieldsColumns .name]       = newField -> getName ();
	(*iter) [userDefinedFieldsColumns .accessType] = Gdk::Pixbuf::create_from_file (get_ui ("icons/AccessType/" + X3D::Generator::X3DAccessTypes [newField] + ".png"));

	// Replace in index

	fields .erase (oldField -> getName ());

	if (oldField -> getAccessType () == X3D::inputOutput)
	{
		fields .erase ("set_" + oldField -> getName ());
		fields .erase (oldField -> getName () + "_changed");
	}

	fields .emplace (newField -> getName (), newField);

	if (newField -> getAccessType () == X3D::inputOutput)
	{
		fields .emplace ("set_" + newField -> getName (), newField);
		fields .emplace (newField -> getName () + "_changed", newField);
	}

	// Replace in array

	userDefinedFields [index] = newField;

	// Remember old existing node fields that should be replaced by newField.

	bool exists = false; // Already exist in node.

	try
	{
		exists = (oldField == node -> getField (oldField -> getName ()));
	}
	catch (const X3D::X3DError &)
	{ }

	if (exists)
	{
		fieldsToReplace [newField] = oldField;
		fieldsToRemove .emplace_back (oldField);
	}
	else
	{
		// Check if oldField should replace a node field.

		const auto iter = fieldsToReplace .find (oldField);

		if (iter not_eq fieldsToReplace .end ())
		{
			fieldsToReplace [newField] = iter -> second;
			fieldsToReplace .erase (iter);
		}

		delete oldField;
	}
}

void
NodePropertiesEditor::on_ok ()
{
	const auto undoStep = std::make_shared <UndoStep> (_ ("Edit Node Properties"));

	// Apply name change

	const std::string name = getNameEntry () .get_text ();

	if (name not_eq node -> getName ())
	{
		undoStep -> addUndoFunction (&NodePropertiesEditor::updateNamedNode,
		                             node -> getName (),
		                             node,
		                             getBrowserWindow ());

		undoStep -> addRedoFunction (&NodePropertiesEditor::updateNamedNode,
		                             name,
		                             node,
		                             getBrowserWindow ());

		updateNamedNode (name, node, getBrowserWindow ());
	}

	if (node -> getUserDefinedFields () not_eq userDefinedFields)
	{
		__LOG__ << std::endl;
	}

	getBrowserWindow () -> addUndoStep (undoStep);

	close ();
}

void
NodePropertiesEditor::on_cancel ()
{
	close ();
}

void
NodePropertiesEditor::updateNamedNode (const std::string & name, const X3D::SFNode & node, BrowserWindow* const browserWindow)
{
	node -> getExecutionContext () -> removeNamedNode (node -> getName ());

	if (not name .empty ())
		node -> getExecutionContext () -> updateNamedNode (node -> getExecutionContext () -> getUniqueName (name), node);

	browserWindow -> getOutlineTreeView () .queue_draw ();
}

NodePropertiesEditor::~NodePropertiesEditor ()
{
	__LOG__ << std::endl;
}

}  // puck

}    // titania
