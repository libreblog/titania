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

#ifndef __TITANIA_X3D_JAVA_SCRIPT_PEASE_BLOSSOM_X3DFIELD_H__
#define __TITANIA_X3D_JAVA_SCRIPT_PEASE_BLOSSOM_X3DFIELD_H__

#include "../../Basic/X3DField.h"

#include "Arguments.h"
#include "ObjectType.h"

#include <Titania/PeaseBlossom/pb.h>

namespace titania {
namespace X3D {
namespace peaseblossom {

class X3DField
{
protected:

	///  @name Construction

	template <class T>
	static
	pb::var
	create (const pb::ptr <pb::pbExecutionContext> &, typename T::internal_type* const);

	template <class T>
	static
	void
	setUserData (const pb::ptr <pb::pbExecutionContext> &, const pb::var &, typename T::internal_type* const);

	///  @name Functions

	template <class T>
	static
	pb::var
	toString (const pb::ptr <pb::pbExecutionContext> &, const pb::var &, const std::vector <pb::var> &);


private:

	///  @name Destruction

	template <class T>
	static
	void
	dispose (pb::pbObject* const);

};

template <class T>
pb::var
X3DField::create (const pb::ptr <pb::pbExecutionContext> & ec, typename T::internal_type* const field)
{
	const auto context = getContext (ec);
	const auto object  = context -> getClass (T::getType ()) -> createInstance (ec);

	setUserData <T> (ec, object, field);

	return object;
}

template <class T>
void
X3DField::setUserData (const pb::ptr <pb::pbExecutionContext> & ec, const pb::var & value, typename T::internal_type* const field)
{
	const auto   context  = getContext (ec);
	const auto & object   = value .getObject ();
	auto &       userData = object -> getUserData ();

	userData .reserve (3);

	userData .emplace_back ((void*) T::getType ());
	userData .emplace_back (field);
	userData .emplace_back (context);
	object -> setDisposeCallback (dispose <T>);

	field -> addParent (context);
}

template <class T>
pb::var
X3DField::toString (const pb::ptr <pb::pbExecutionContext> & ec, const pb::var & object, const std::vector <pb::var> &)
{
	try
	{
		return getThis <T> (object) -> toString ();
	}
	catch (const std::invalid_argument &)
	{
		throw pb::TypeError ("X3DField.prototype.toString is not generic.");
	}
}

template <class T>
void
X3DField::dispose (pb::pbObject* const object)
{
	const auto context = getContext (object);
	const auto field   = getObject <typename T::internal_type> (object);

	field -> removeParent (context);
}

} // peaseblossom
} // X3D
} // titania

#endif
