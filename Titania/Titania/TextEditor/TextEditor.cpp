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

#include "TextEditor.h"

#include "../Configuration/config.h"

#include <Titania/String.h>

namespace titania {
namespace puck {

TextEditor::TextEditor (BrowserWindow* const browserWindow) :
	          X3DBaseInterface (browserWindow, browserWindow -> getBrowser ()),
	    X3DTextEditorInterface (get_ui ("Dialogs/TextEditor.xml"), gconf_dir ()),
	                shapeNodes (),
	                      text (),
	                  undoStep (),
	                  changing (false)
{ }

void
TextEditor::initialize ()
{
	X3DTextEditorInterface::initialize ();

	getBrowser () -> getSelection () -> getChildren () .addInterest (this, &TextEditor::set_selection);

	set_selection ();
}

void
TextEditor::set_selection ()
{
	for (const auto & shapeNode : shapeNodes)
		shapeNode -> geometry () .removeInterest (this, &TextEditor::set_text);

	undoStep .reset ();

	// Find Appearances.

	auto selection = getBrowser () -> getSelection () -> getChildren ();

	shapeNodes .clear ();

	X3D::traverse (selection, [&] (X3D::SFNode & node)
	               {
	                  for (const auto & type: node -> getType ())
	                  {
	                     if (type == X3D::X3DConstants::X3DShapeNode)
	                     {
	                        shapeNodes .emplace_back (node);
	                        return true;
								}
							}

	                  return true;
						});

	for (const auto & shapeNode : shapeNodes)
		shapeNode -> geometry () .addInterest (this, &TextEditor::set_text);

	set_text ();
}

void
TextEditor::on_text_toggled ()
{
	if (changing)
		return;
		
	getTextCheckButton () .set_inconsistent (false);
	getTextBox () .set_sensitive (getTextCheckButton () .get_active ());

	if (not getTextCheckButton () .get_active ())
	{
		text = text -> copy (text -> getExecutionContext ());
		text -> getExecutionContext () -> addUninitializedNode (text);
		text -> getExecutionContext () -> realize ();
	}

	addUndoFunction <X3D::SFNode> (shapeNodes, "geometry", undoStep);

	for (const auto & shapeNode : shapeNodes)
	{
		try
		{
			auto & field = shapeNode -> geometry ();

			if (getTextCheckButton () .get_active ())
				field = text;
			else
				field = nullptr;

			field .removeInterest (this, &TextEditor::set_text);
			field .addInterest (this, &TextEditor::connectText);
		}
		catch (const X3D::X3DError &)
		{ }
	}

	addRedoFunction <X3D::SFNode> (shapeNodes, "geometry", undoStep);
}

void
TextEditor::set_text ()
{
	if (text)
	{
		text -> string ()    .removeInterest (this, &TextEditor::set_string);
		text -> maxExtent () .removeInterest (this, &TextEditor::set_maxExtent);
	}
	
	text = nullptr;

	const bool hasField = not shapeNodes .empty ();

	// Find last »geometry« field.

	int active = -1;

	for (const auto & shapeNode : basic::reverse_adapter (shapeNodes))
	{
		try
		{
			const X3D::X3DPtr <X3D::Text> field (shapeNode -> geometry ());
	
			if (active < 0)
			{
				text   = std::move (field);
				active = bool (text);
			}
			else if (field not_eq text)
			{
				if (not text)
					text = std::move (field);

				active = -1;
				break;
			}
		}
		catch (const X3D::X3DError &)
		{ }
	}

	if (not text)
	{
		text = new X3D::Text (getExecutionContext ());
		text -> getExecutionContext () -> addUninitializedNode (text);
		text -> getExecutionContext () -> realize ();
	}

	changing = true;

	getTextCheckButton () .set_sensitive (hasField);
	getTextCheckButton () .set_active (active > 0);
	getTextCheckButton () .set_inconsistent (active < 0);

	getTextBox () .set_sensitive (active > 0);

	changing = false;

	text -> string ()    .addInterest (this, &TextEditor::set_string);
	text -> maxExtent () .addInterest (this, &TextEditor::set_maxExtent);

	set_string ();
	set_maxExtent ();
}

void
TextEditor::connectText (const X3D::SFNode & field)
{
	field .removeInterest (this, &TextEditor::connectText);
	field .addInterest (this, &TextEditor::set_text);
}

void
TextEditor::on_string_changed ()
{
	if (changing)
		return;

	addUndoFunction (text, text -> string (), undoStep);

	text -> string () .clear ();

	const auto string = basic::split (getStringTextBuffer () -> get_text (), "\n");

	for (auto & value : string)
		text -> string () .emplace_back (std::move (value));

	text -> string () .removeInterest (this, &TextEditor::set_string);
	text -> string () .addInterest (this, &TextEditor::connectString);

	addRedoFunction (text -> string (), undoStep);
}

void
TextEditor::set_string ()
{
	changing = true;

	getStringTextBuffer () -> set_text ("");

	if (not text -> string () .empty ())
	{
		for (const auto & value : text -> string ())
		{
			getStringTextBuffer () -> insert (getStringTextBuffer () -> end (), value);
			getStringTextBuffer () -> insert (getStringTextBuffer () -> end (), "\n");
		}

		getStringTextBuffer () -> erase (-- getStringTextBuffer () -> end (), getStringTextBuffer () -> end ());
	}

	changing = false;
}

void
TextEditor::connectString (const X3D::MFString & field)
{
	field .removeInterest (this, &TextEditor::connectString);
	field .addInterest (this, &TextEditor::set_string);
}

void
TextEditor::on_maxExtent_changed ()
{
	if (changing)
		return;

	addUndoFunction (text, text -> maxExtent (), undoStep);

	text -> maxExtent () = getMaxExtentSpinButton () .get_value ();

	text -> maxExtent () .removeInterest (this, &TextEditor::set_maxExtent);
	text -> maxExtent () .addInterest (this, &TextEditor::connectMaxExtent);

	addRedoFunction (text -> maxExtent (), undoStep);
}

void
TextEditor::set_maxExtent ()
{
	changing = true;

	getMaxExtentSpinButton () .set_value (text -> maxExtent ());

	changing = false;
}

void
TextEditor::connectMaxExtent (const X3D::SFFloat & field)
{
	field .removeInterest (this, &TextEditor::connectMaxExtent);
	field .addInterest (this, &TextEditor::set_maxExtent);
}

TextEditor::~TextEditor ()
{ }

} // puck
} // titania
