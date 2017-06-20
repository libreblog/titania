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
 * Copyright 1999, 2016 Holger Seelig <holger.seelig@yahoo.de>.
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

#include "X3DBaseFileSaveDialog.h"

#include "../../Browser/X3DBrowserWindow.h"
#include "../../Configuration/config.h"

namespace titania {
namespace puck {

X3DBaseFileSaveDialog::X3DBaseFileSaveDialog () :
	X3DFileSaveDialogInterface (get_ui ("Dialogs/FileSaveDialog.glade"))
{
	getWindow () .property_filter () .signal_changed () .connect (sigc::mem_fun (this, &X3DBaseFileSaveDialog::on_filter_changed));
}

bool
X3DBaseFileSaveDialog::run ()
{
	if (getBrowserWindow () -> getConfig () -> getBoolean ("addStandardMetaData"))
	{
		getOutputStyleBox ()    .set_visible (true);
		getOutputStyleButton () .set_active (getConfig () -> getInteger ("outputStyle"));
	}
	else
	{
		getOutputStyleBox ()    .set_visible (false);
		getOutputStyleButton () .set_active (0);
	}

	const auto responseId = getWindow () .run ();

	quit ();

	getConfig () -> setItem ("outputStyle", getOutputStyleButton () .get_active_row_number ());

	if (responseId == Gtk::RESPONSE_OK)
		return true;

	return false;
}

basic::uri
X3DBaseFileSaveDialog::getUrl () const
{
	const basic::uri url = getWindow () .get_file () -> get_path ();

	return url .add_file_scheme ();
}

void
X3DBaseFileSaveDialog::setSuffix (const std::string & suffix)
{
	basic::uri name (getWindow () .get_current_name ());

	getWindow () .set_current_name (name .basename (false) + suffix);
}

void
X3DBaseFileSaveDialog::on_filter_changed ()
{
	setSuffix (getSuffix ());
}

void
X3DBaseFileSaveDialog::on_response (int responseId)
{
	if (responseId not_eq Gtk::RESPONSE_OK)
		return;

	basic::uri name (getWindow () .get_current_name ());

	if (getKnownFileTypes () .count (name .suffix ()))
		return;

	name .suffix (getSuffix ());

	getWindow () .set_current_name (name .str ());
}

X3DBaseFileSaveDialog::~X3DBaseFileSaveDialog ()
{ }

} // puck
} // titania