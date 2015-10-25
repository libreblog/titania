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

#include "FileOpenDialog.h"

#include "../../Browser/X3DBrowserWindow.h"
#include "../../Configuration/config.h"

#include <Titania/OS.h>

namespace titania {
namespace puck {

FileOpenDialog::FileOpenDialog (X3DBrowserWindow* const browserWindow) :
	          X3DBaseInterface (browserWindow, browserWindow -> getBrowser ()),
	X3DFileOpenDialogInterface (get_ui ("Dialogs/FileOpenDialog.xml"), gconf_dir ())
{
	getFileFilterX3D   () -> set_name (_ ("X3D"));
	getFileFilterImage () -> set_name (_ ("Images"));
	getFileFilterAudio () -> set_name (_ ("Audio"));
	getFileFilterVideo () -> set_name (_ ("Videos"));
	getFileFilterFonts () -> set_name (_ ("Fonts"));
	getFileFilterAll   () -> set_name (_ ("All Files"));

	const auto worldURL = getScene () -> getWorldURL ();

	if (not worldURL .empty () and worldURL .is_local ())
		getWindow () .set_uri (worldURL .filename () .str ());

	else
		getWindow () .set_filename (os::home () + _ ("scene.x3dv"));

	getRelativePathSwitch () .set_active (getConfig () .getBoolean ("relativePath"));

	setup ();
}

void
FileOpenDialog::setMode (Mode mode)
{
	switch (mode)
	{
	   case Mode::X3D:
			getWindow () .add_filter (getFileFilterX3D ());
			getWindow () .add_filter (getFileFilterImage ());
			getWindow () .add_filter (getFileFilterAudio ());
			getWindow () .add_filter (getFileFilterVideo ());
			setFilter (getConfig () .getString ("filter"));
	      break;
	   case Mode::FONTS:
			getWindow () .add_filter (getFileFilterFonts ());
			getWindow () .set_filter (getFileFilterFonts ());
			break;
	}

}

void
FileOpenDialog::setFilter (const std::string & name)
{
	if (name == _("Images"))
		getWindow () .set_filter (getFileFilterImage ());
	else if (name == _("Audio"))
		getWindow () .set_filter (getFileFilterAudio ());
	else if (name == _("Videos"))
		getWindow () .set_filter (getFileFilterVideo ());
	else
		getWindow () .set_filter (getFileFilterX3D ());
}

void
FileOpenDialog::setURL (const basic::uri & URL)
{
	if (URL .is_local ())
		getWindow () .set_uri (URL .str ());
}

basic::uri
FileOpenDialog::getURL () const
{
	return Glib::uri_unescape_string (getWindow () .get_uri ());
}

void
FileOpenDialog::loadURL ()
{
	if (run ())
		getBrowserWindow () -> open (getURL ());
}

bool
FileOpenDialog::run ()
{
	if (getConfig () .hasItem ("currentFolder"))
		getWindow () .set_current_folder_uri (getConfig () .getString ("currentFolder"));
	else
		getWindow () .set_current_folder (os::home ());
	
	setMode (Mode::X3D);

	const auto responseId = getWindow () .run ();

	getConfig () .setItem ("currentFolder", getWindow () .get_current_folder_uri ());
	getConfig () .setItem ("relativePath", getRelativePathSwitch () .get_active ());

	if (getWindow () .get_filter ())
		getConfig () .setItem ("filter", getWindow () .get_filter () -> get_name ());
	
	quit ();

	if (responseId == Gtk::RESPONSE_OK)
		return true;

	return false;
}

bool
FileOpenDialog::font ()
{
	if (getConfig () .hasItem ("currentFolder"))
		getWindow () .set_current_folder_uri (getConfig () .getString ("currentFolder"));
	
	setMode (Mode::FONTS);

	const auto responseId = getWindow () .run ();

	getConfig () .setItem ("currentFolder", getWindow () .get_current_folder_uri ());
	getConfig () .setItem ("relativePath", getRelativePathSwitch () .get_active ());
	quit ();

	if (responseId == Gtk::RESPONSE_OK)
		return true;


	return false;
}

FileOpenDialog::~FileOpenDialog ()
{
	dispose ();
}

} // puck
} // titania
