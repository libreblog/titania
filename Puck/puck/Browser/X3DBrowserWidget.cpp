/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: tab; c-basic-offset: 3 -*- */
/*******************************************************************************
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
 ******************************************************************************/

#include "X3DBrowserWidget.h"

#include "../Browser/ViewpointMenuItem.h"

#include <Titania/Bits/String.h>
#include <Titania/Chrono/Now.h>

#include <Titania/gzstream.h>
#include <fstream>

#include "Image.h"

namespace titania {
namespace puck {

X3DBrowserWidget::X3DBrowserWidget (const std::string & sessionKey, X3DBrowserInterface* const browserWidget) :
	X3DBrowserUserInterface (sessionKey, browserWidget), 
	               loadTime (0)
{ }

void
X3DBrowserWidget::initialize ()
{
	X3DBrowserUserInterface::initialize ();

	__LOG__ << this << std::endl;

	// Splash Screen.
	
	// Connect to map_event
	map_event = getSurface () .signal_map_event () .connect (sigc::mem_fun (*this, &X3DBrowserWidget::on_map_event));

	// Insert Surface, this will initialize the Browser.
	getSurfaceBox () .pack_start (getSurface (), true, true, 0);

	// Show Surface and start the X3D Main Loop.
	getSurface () .show ();
}

bool
X3DBrowserWidget::on_map_event (GdkEventAny* event)
{
	__LOG__ << this << std::endl;
	
	map_event .disconnect ();

	// Register browser interests.
	getBrowser () -> urlError .addInterest (this, &X3DBrowserWidget::set_urlError);
	getBrowser () -> world .addInterest    (this, &X3DBrowserWidget::set_initialized);

	try
	{
		// Load Splash Screen.
		loadURL ({ "about:splash" });
	}
	catch (...)
	{ }
		
	return false; // Propagate the event further. 
}

void
X3DBrowserWidget::set_initialized ()
{
	__LOG__ << this << std::endl;
	
	updateLocation    ();
	updateIcon        ();

	// The Splash Screen is loaded.

	getBrowser () -> world .removeInterest (this, &X3DBrowserWidget::set_initialized);
	getBrowser () -> world .addInterest    (this, &X3DBrowserWidget::set_world);

	loadURL ({ getConfig () .string ("worldURL") });
}

void
X3DBrowserWidget::blank ()
{
	if (not getSurface () .makeCurrent ())
		return;

	getBrowser () -> replaceWorld (getBrowser () -> createScene ());
}

void
X3DBrowserWidget::home ()
{
	loadURL ({ "about:home" });
}

void
X3DBrowserWidget::setDescription (const std::string & value)
throw (X3D::Error <X3D::INVALID_OPERATION_TIMING>,
       X3D::Error <X3D::DISPOSED>)
{
	getBrowserWidget () -> setDescription (value);
}

void
X3DBrowserWidget::loadURL (const X3D::MFString & url, const X3D::MFString & parameter)
throw (X3D::Error <X3D::INVALID_URL>,
       X3D::Error <X3D::URL_UNAVAILABLE>,
       X3D::Error <X3D::INVALID_X3D>)
{ }

void
X3DBrowserWidget::loadURL (const X3D::MFString & url)
throw (X3D::Error <X3D::INVALID_URL>,
       X3D::Error <X3D::URL_UNAVAILABLE>,
       X3D::Error <X3D::INVALID_X3D>)
{
	pushStatusBar ("Opening file " + url .toString () + ".");

	loadTime = chrono::now ();
	getBrowser () -> loadURL (url);
}

void
X3DBrowserWidget::save (const basic::uri & worldURL)
{
	if (getSaveCompressedButton () .get_active ())
	{
		ogzstream file (worldURL);
		file << X3D::CleanStyle << getBrowser () -> world << std::flush;
		file .close ();
	}
	else
	{
		std::ofstream file (worldURL);
		file << X3D::CompactStyle << getBrowser () -> world << std::flush;
		file .close ();
	}

	//update_location ();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// StatusBar

void
X3DBrowserWidget::pushStatusBar (const std::string & string)
{
	getStatusBar () .push (string);
}

void
X3DBrowserWidget::popStatusBar ()
{
	//	getStatusBar () .pop ();
}

// Statistics

void
X3DBrowserWidget::printStatistics () const
{
//	try
//	{
//		std::string name = getBrowser () -> getExecutionContext () -> getMetaData ("title");
//
//		std::cout << "Rendering statistics for: " << name << std::endl;
//
//		std::cout << "Load Time: " << loadTime << std::endl;
//
//		for (auto const & line : getSurface () .statistics -> string)
//			std::cout << "  " << line .toString () << std::endl;
//	}
//	catch (const X3D::Error <X3D::INVALID_NAME> &)
//	{ }
}

// EventIn's

void
X3DBrowserWidget::set_urlError ()
{
	if (getBrowser () -> urlError .empty ())
		return;
	
	getMessageDialog () .set_message ("Invalid X3D");
	getMessageDialog () .set_secondary_text ("<span font_desc=\"mono\">"
	                                         + basic::join (getBrowser () -> urlError .cbegin (),
	                                                        getBrowser () -> urlError .cend (),
	                                                        "\n")
	                                         + "</span>",
	                                         true);
	getMessageDialog () .show ();
}

void
X3DBrowserWidget::set_world (/* X3D::SFNode <X3D::World> & world */)
{
	loadTime = chrono::now () - loadTime;
	std::cout << "Load Time: " << loadTime << std::endl;

	saveSession ();

	// Update browser widget.

	updateDescription ();
	updateLocation    ();
	updateIcon        ();
	updateViewpoints  ();

	// Clear statusbar.

	popStatusBar ();
}

void
X3DBrowserWidget::updateDescription ()
{
	std::string description;
	try
	{
		description = getBrowser () -> getExecutionContext () -> getMetaData ("title");
	}
	catch (const X3D::Error <X3D::INVALID_NAME> &)
	{
		description = getExecutionContext () -> getWorldURL ();
	}

	setDescription (description);
}

void
X3DBrowserWidget::updateLocation ()
{
	const auto & worldURL = getExecutionContext () -> getWorldURL ();

	getLocationEntry () .set_text (worldURL .str ());

	if (worldURL .length () and worldURL .is_local ())
	{
		getFileOpenDialog () .set_current_folder_uri (worldURL .base () .str ());
		getFileSaveDialog () .set_current_folder_uri (worldURL .base () .str ());
	}

}

void
X3DBrowserWidget::updateIcon ()
{
	const auto & worldURL = getExecutionContext () -> getWorldURL ();

	Gtk::StockID                stockId = Gtk::StockID (worldURL .str ());
	Glib::RefPtr <Gtk::IconSet> iconSet;

	try
	{
		// Create Stock Icon

		basic::uri uri;

		try
		{
			uri = getBrowser () -> getExecutionContext () -> getMetaData ("icon");
		}
		catch (const X3D::Error <X3D::INVALID_NAME> &)
		{
			uri = "/favicon.ico";
		}

		titania::Image icon (getBrowser () -> loadDocument (uri));

		iconSet = Gtk::IconSet::create (Gdk::Pixbuf::create_from_data (icon .getData (),
		                                                               Gdk::COLORSPACE_RGB,
		                                                               icon .getTransparency (),
		                                                               sizeof (Image::value_type) * 8,
		                                                               icon .getWidth (),
		                                                               icon .getHeight (),
		                                                               icon .getWidth () * icon .getComponents ()) -> copy ());
	}
	catch (const std::exception & error)
	{
		__LOG__ << "Couldn't load icon: using blank icon: " << error .what () << std::endl;
		iconSet = Gtk::IconSet::lookup_default (Gtk::StockID ("BlankIcon"));
	}

	getIconFactory () -> add (stockId, iconSet);
	Gtk::Stock::add (Gtk::StockItem (stockId, worldURL .str ()));

	getLocationEntry () .property_primary_icon_stock () .set_value (stockId);
}

void
X3DBrowserWidget::updateViewpoints (/* X3D::SFNode <X3D::World> & world */)
{
	Gtk::Menu* submenu = Gtk::manage (new Gtk::Menu ());

	for (auto const & viewpoint : getBrowser () -> getExecutionContext () -> getViewpoints ())
	{
		X3D::X3DViewpointNode* _viewpoint = dynamic_cast <X3D::X3DViewpointNode*> (viewpoint .getValue ());

		if (_viewpoint -> description .length ())
			submenu -> append (*Gtk::manage (new ViewpointMenuItem (viewpoint)));
	}

	submenu -> show_all ();

	getViewpointsMenuItem () .set_submenu (*submenu);
}

// Destructor

X3DBrowserWidget::~X3DBrowserWidget ()
{ }

} // puck
} // titania
