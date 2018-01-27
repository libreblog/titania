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

#include "BrowserApplication.h"

#include "../../Browser/BrowserWindow.h"
#include "../../Configuration/config.h"

namespace titania {
namespace puck {

X3DBrowserApplication::X3DBrowserApplication () :  
	 Gtk::Application (),
	    browserWindow ()
{ }

const std::shared_ptr <BrowserWindow> &
X3DBrowserApplication::getBrowserWindow () const
{
	const_cast <X3DBrowserApplication*> (this) -> realize ();

	return browserWindow;
}

void
X3DBrowserApplication::realize ()
{
	if (browserWindow)
		return;

	browserWindow .reset (new BrowserWindow (X3D::createBrowser ({ get_ui ("Logo.x3dv") })));

	add_window (browserWindow -> getWindow ());

	browserWindow -> present ();
}

void
X3DBrowserApplication::on_activate ()
{
	if (browserWindow)
	{
		browserWindow -> blank ();
		browserWindow -> present ();
	}
	else
	{
		realize ();
	}
}

X3DBrowserApplication::~X3DBrowserApplication ()
{ }

} // puck
} // titania