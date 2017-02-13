/* -*- Mode: C++; coding: utf-8; tab-width: 3; indent-tabs-mode: tab; c-basic-offset: 3 -*-
 *******************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright create3000, Scheffelstra�e 31a, Leipzig, Germany 2011.
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

#ifndef __TITANIA_X3D_PARSER_VRML1_NODES_TEXTURE2TRANSFORM_H__
#define __TITANIA_X3D_PARSER_VRML1_NODES_TEXTURE2TRANSFORM_H__

#include "VRML1Node.h"

namespace titania {
namespace X3D {
namespace VRML1 {

class Texture2Transform :
	public VRML1Node
{
public:

	///  @name Construction

	Texture2Transform (X3D::X3DExecutionContext* const executionContext);

	///  @name Common members

	virtual
	ComponentType
	getComponent () const
	throw (X3D::Error <X3D::DISPOSED>) final override
	{ return component; }

	virtual
	const std::string &
	getTypeName () const
	throw (X3D::Error <X3D::DISPOSED>) final override
	{ return typeName; }

	virtual
	const std::string &
	getContainerField () const
	throw (X3D::Error <X3D::DISPOSED>) final override
	{ return containerField; }

	///  @name Fields

	SFVec2f &
	translation ()
	{ return *fields .translation; }

	const SFVec2f &
	translation () const
	{ return *fields .translation; }

	SFFloat &
	rotation ()
	{ return *fields .rotation; }

	const SFFloat &
	rotation () const
	{ return *fields .rotation; }

	SFVec2f &
	scaleFactor ()
	{ return *fields .scaleFactor; }

	const SFVec2f &
	scaleFactor () const
	{ return *fields .scaleFactor; }

	SFVec2f &
	center ()
	{ return *fields .center; }

	const SFVec2f &
	center () const
	{ return *fields .center; }

	///  @name Operations

	virtual
	void
	push (Converter* const converter) final override;

	virtual
	void
	convert (Converter* const converter) final override;

	///  @name Desstruction

	~Texture2Transform ();


private:

	///  @name Construction

	virtual
	X3D::X3DBaseNode*
	create (X3D::X3DExecutionContext* const) const final override;

	///  @name Static members

	static const ComponentType component;
	static const std::string   typeName;
	static const std::string   containerField;

	///  @name Members

	struct Fields
	{
		Fields ();

		X3D::SFVec2f* const translation;
		X3D::SFFloat* const rotation;
		X3D::SFVec2f* const scaleFactor;
		X3D::SFVec2f* const center;
	};

	Fields fields;

};

} // VRML1
} // X3D
} // titania

#endif