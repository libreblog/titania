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

#include <cassert>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <set>
#include <string>

#include "Test.h"

//#include "Tests/Rotation.h"
//#include "Tests/AddAndRemoveNode.h"
//#include "Tests/Hierarchy.h"
//#include "Tests/FieldInterest.h"

//////#include "Tests/AddAndRemoveNode.h"

#include <Titania/X3D.h>
#include <gtkmm.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace titania;
using namespace titania::Test;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Type>
inline
bool
operator == (const X3D::Array <X3D::X3DField <Type>> & lhs, const X3D::Array <X3D::X3DField <Type>> & rhs)
{
	__LOG__ << std::endl;

	return lhs .size () == rhs .size () &&
	       std::equal (lhs .begin (), lhs .end (),
	                   rhs .begin (),
	                   [ ] (const Type * a, const Type * b){ return *a == *b; });
}

int
main (int argc, char* argv [ ])
{
	install_signal_hander ();
	
	gtk_init (0, 0);
	Gtk::Main::init_gtkmm_internals ();
	
	std::clog << std::boolalpha << std::endl;

	std::clog << "Test started ..." << std::endl << std::endl;
	{
		std::clog << "Start of block ..." << std::endl;

		{
			auto browser = X3D::getBrowser ();
			
			X3D::Array <X3D::SFInt32> a1;
			X3D::Array <X3D::SFInt32> a2;
			
			std::clog << (a1 == a2) << std::endl;
			
//			X3D::MFInt32 o;
//
//			for (int i = 0; i < 10; ++ i)
//			{
//				{
//					X3D::MFInt32 a1;
//
//					a1 .resize (2000000);
//				
//					__LOG__ << a1 .size () << std::endl;
//				}
//
//				o .getGarbageCollector () .dispose ();
//	
//				sleep (1);
//			}
		}

		std::clog << "End of block ..." << std::endl;
	}
	std::clog << "Test done ..." << std::endl;
}

// SAIAction
// SAIBoolean
// SAIBrowserApp
// SAIBrowserName
// SAIBrowserRef
// SAIBrowserVersion
// SAIComponentDeclaration
// SAIComponent
// SAIEncoding
// SAIExecutionContext
// SAIFieldAccess
// SAIFieldDeclaration
// SAIField
// SAIFieldName
// SAIFieldType
// SAIFieldValue
// SAIFrameRate
// SAILayerID
// SAILoadState
// SAIMatrix
// SAINavSpeed
// SAINode
// SAINodeType
// SAIParameterLIst
// SAIProfileDeclaration
// SAIPropertyList
// SAIProtoDeclaration
// SAIRequester
// SAIRoute
// SAIScene
// SAIScript
// SAIScriptImplementation
// SAIStream
// SAIString
// SAIURL
// NULL
