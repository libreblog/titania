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

#ifndef __TITANIA_X3D_TOOLS_NETWORKING_ANCHOR_TOOL_H__
#define __TITANIA_X3D_TOOLS_NETWORKING_ANCHOR_TOOL_H__

#include "../Grouping/X3DGroupingNodeTool.h"

#include "../../Components/Networking/Anchor.h"
#include "../Networking/X3DUrlObjectTool.h"
#include "../ToolColors.h"

namespace titania {
namespace X3D {

class AnchorTool :
	public X3DGroupingNodeTool <Anchor>, public X3DUrlObjectTool <Anchor>
{
public:

	///  @name Construction

	AnchorTool (Anchor* const node) :
		                 X3DBaseNode (node -> getExecutionContext () -> getBrowser (), node -> getExecutionContext ()),
		        X3DBaseTool <Anchor> (node),
		X3DGroupingNodeTool <Anchor> (ToolColors::LILA),
		   X3DUrlObjectTool <Anchor> ()
	{
		addType (X3DConstants::AnchorTool);
	}

	///  @name Fields

	virtual
	SFString &
	description () final override
	{ return getNode () -> description (); }

	virtual
	const SFString &
	description () const final override
	{ return getNode () -> description (); }

	virtual
	MFString &
	parameter () final override
	{ return getNode () -> parameter (); }

	virtual
	const MFString &
	parameter () const final override
	{ return getNode () -> parameter (); }

	///  @name Event handlers

	virtual
	void
	set_over (const bool value) final override
	{ return getNode () -> set_over (value); }

	virtual
	void
	set_active (const bool value) final override
	{ return getNode () -> set_over (value); }

	///  @name Operations

	virtual
	void
	traverse (const TraverseType type) final override
	{
		X3DGroupingNodeTool <Anchor>::traverse (type);
		X3DUrlObjectTool <Anchor>::traverse (type);
	}

	///  @name Destruction

	virtual
	void
	dispose () final override
	{
		X3DUrlObjectTool <Anchor>::dispose ();
		X3DGroupingNodeTool <Anchor>::dispose ();
	}


private:

	using X3DGroupingNodeTool <Anchor>::getNode;

	///  @name Destruction

	virtual
	void
	initialize () final override
	{
		X3DGroupingNodeTool <Anchor>::initialize ();
		X3DUrlObjectTool <Anchor>::initialize ();
	}

};

} // X3D
} // titania

#endif
