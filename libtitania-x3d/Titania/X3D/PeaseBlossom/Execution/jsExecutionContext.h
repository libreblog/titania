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

#ifndef __TITANIA_X3D_PEASE_BLOSSOM_EXECUTION_JS_EXECUTION_CONTEXT_H__
#define __TITANIA_X3D_PEASE_BLOSSOM_EXECUTION_JS_EXECUTION_CONTEXT_H__

#include "../Base/jsChildType.h"
#include "../Base/jsInputStreamType.h"
#include "../Base/jsOutputStreamType.h"
#include "../Bits/Exception.h"
#include "../Primitives/Object.h"
#include "../Primitives/jsBasicFunction.h"
#include "../Values/var.h"

#include <Titania/Utility/Adapter.h>

#include <vector>

namespace titania {
namespace pb {

class jsExecutionContext :
	virtual public jsChildType,
	virtual public jsInputStreamType,
	virtual public jsOutputStreamType
{
public:

	/// @name Member access

	const basic_ptr <jsBasicObject> &
	getGlobalObject () const
	{ return globalObject; }

	/// @name Input/Output

	virtual
	void
	fromStream (std::istream & istream)
	throw (SyntaxError) final override;

	///  @name Destruction

	virtual
	void
	dispose () override;


protected:

	friend class Parser;
	friend class Identifier;
	friend class VariableDeclaration;

	///  @name Construction

	jsExecutionContext (jsExecutionContext* const executionContext, const basic_ptr <jsBasicObject> & globalObject) :
	         jsChildType (),
		jsInputStreamType (),
		 executionContext (executionContext),
		     globalObject (globalObject),
		   defaultObjects ({ globalObject }),
		      expressions ()
	{
		addChildren (*executionContext, this -> globalObject);
	}

	/// @name Function services

	void
	replaceFunction (const basic_ptr <jsBasicFunction> & function);

	/// @name Default object services

	void
	pushDefaultObject (const basic_ptr <jsBasicObject> & object)
	{ defaultObjects .emplace_back (object); }

	void
	popDefaultObject ()
	{ return defaultObjects .pop_back (); }

	const basic_ptr <jsBasicObject> &
	getDefaultObject ()
	{ return defaultObjects .back (); }

	/// @name Property access

	const var &
	getProperty (const std::string & name) const
	throw (ReferenceError);

	/// @name Expression access

	std::vector <var> &
	getExpressions ()
	{ return expressions; }

	const std::vector <var> &
	getExpressions () const
	{ return expressions; }

	///  @name Input/Output

	virtual
	void
	toStream (std::ostream & ostream) const override
	{ ostream << "[program Program]"; }


protected:

	///  @name Operation

	virtual
	var
	run ();


private:

	/// @name Members

	jsExecutionContext* const                           executionContext;
	std::map <std::string, basic_ptr <jsBasicFunction>> functions;
	basic_ptr <jsBasicObject>                           globalObject;
	std::vector <basic_ptr <jsBasicObject>>             defaultObjects;
	std::vector <var>                                   expressions;

};

} // pb
} // titania

#endif
