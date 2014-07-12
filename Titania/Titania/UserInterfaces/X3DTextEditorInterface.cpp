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
#include "X3DTextEditorInterface.h"

namespace titania {
namespace puck {

const std::string X3DTextEditorInterface::m_widgetName = "TextEditor";

void
X3DTextEditorInterface::create (const std::string & filename)
{
	// Create Builder.
	m_builder = Gtk::Builder::create_from_file (filename);

	// Get objects.
	m_MaxExtentAdjustment = Glib::RefPtr <Gtk::Adjustment>::cast_dynamic (m_builder -> get_object ("MaxExtentAdjustment"));
	m_StringTextBuffer    = Glib::RefPtr <Gtk::TextBuffer>::cast_dynamic (m_builder -> get_object ("StringTextBuffer"));

	// Get widgets.
	m_builder -> get_widget ("Window", m_Window);
	m_Window -> set_name ("Window");
	m_builder -> get_widget ("Widget", m_Widget);
	m_Widget -> set_name ("Widget");
	m_builder -> get_widget ("TextCheckButton", m_TextCheckButton);
	m_TextCheckButton -> set_name ("TextCheckButton");
	m_builder -> get_widget ("TextBox", m_TextBox);
	m_TextBox -> set_name ("TextBox");
	m_builder -> get_widget ("MaxExtentSpinButton", m_MaxExtentSpinButton);
	m_MaxExtentSpinButton -> set_name ("MaxExtentSpinButton");
	m_builder -> get_widget ("StringTextView", m_StringTextView);
	m_StringTextView -> set_name ("StringTextView");
	m_builder -> get_widget ("LenghtBox", m_LenghtBox);
	m_LenghtBox -> set_name ("LenghtBox");

	// Connect object Gtk::TextBuffer with id 'StringTextBuffer'.
	m_StringTextBuffer -> signal_changed () .connect (sigc::mem_fun (*this, &X3DTextEditorInterface::on_string_changed));

	// Connect object Gtk::CheckButton with id 'TextCheckButton'.
	m_TextCheckButton -> signal_toggled () .connect (sigc::mem_fun (*this, &X3DTextEditorInterface::on_text_toggled));

	// Connect object Gtk::SpinButton with id 'MaxExtentSpinButton'.
	m_MaxExtentSpinButton -> signal_value_changed () .connect (sigc::mem_fun (*this, &X3DTextEditorInterface::on_maxExtent_changed));

	// Call construct handler of base class.
	construct ();
}

X3DTextEditorInterface::~X3DTextEditorInterface ()
{
	delete m_Window;
}

} // puck
} // titania
