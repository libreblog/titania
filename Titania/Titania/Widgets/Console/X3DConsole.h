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

#ifndef __TITANIA_CONSOLE_X3D_CONSOLE_H__
#define __TITANIA_CONSOLE_X3D_CONSOLE_H__

#include "../../UserInterfaces/X3DConsoleInterface.h"

namespace titania {
namespace puck {

class X3DConsole :
	virtual public X3DConsoleInterface
{
public:

	///  @name Construction

	virtual
	void
	setup () override;

	///  @name Operations

	void
	print (const Glib::ustring & string);

	void
	log (const Glib::ustring & string);

	void
	warn (const Glib::ustring & string);

	void
	error (const Glib::ustring & string);

	///  @name Destruction
	
	virtual
	~X3DConsole () override;


protected:

	///  @name Construction
	
	X3DConsole ();

	///  @name Member access

	void
	setScrollToEnd (const bool value)
	{ scrollToEnd = value; }

	bool
	getScrollToEnd () const
	{ return scrollToEnd; }


private:

	///  @name Event handlers

	void
	on_scoll_to_end ();

	///  @name Operations

	void
	append (const Glib::ustring & string, const std::vector <Glib::ustring> & tags);

	Gdk::Color
	getColor (const std::string & value) const;

	///  @name Members

	bool scrollToEnd;

};

} // puck
} // titania

#endif
