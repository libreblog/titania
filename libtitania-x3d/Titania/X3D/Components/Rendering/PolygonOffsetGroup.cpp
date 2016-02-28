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

#include "PolygonOffsetGroup.h"

#include "../../Components/Layering/X3DLayerNode.h"
#include "../../Execution/X3DExecutionContext.h"
#include "../../Rendering/PolygonOffsetContainer.h"
#include "../../Tools/Grouping/GroupTool.h"

namespace titania {
namespace X3D {

const ComponentType PolygonOffsetGroup::component      = ComponentType::TITANIA;
const std::string   PolygonOffsetGroup::typeName       = "PolygonOffsetGroup";
const std::string   PolygonOffsetGroup::containerField = "children";

PolygonOffsetGroup::Fields::Fields () :
	  type (new SFString ("POLYGON_OFFSET_FILL")),
	factor (new SFFloat (0)),
	 units (new SFFloat (0))
{ }

PolygonOffsetGroup::PolygonOffsetGroup (X3DExecutionContext* const executionContext) :
	    X3DBaseNode (executionContext -> getBrowser (), executionContext),
	X3DGroupingNode (),
	     offsetType (GL_POLYGON_OFFSET_FILL)
{
	//addType (X3DConstants::PolygonOffsetGroup);

	addField (inputOutput,    "metadata",       metadata ());

	addField (inputOutput,    "type",           type ());
	addField (inputOutput,    "factor",         factor ());
	addField (inputOutput,    "units",          units ());

	addField (initializeOnly, "bboxSize",       bboxSize ());
	addField (initializeOnly, "bboxCenter",     bboxCenter ());
	addField (inputOnly,      "addChildren",    addChildren ());
	addField (inputOnly,      "removeChildren", removeChildren ());
	addField (inputOutput,    "children",       children ());
}

X3DBaseNode*
PolygonOffsetGroup::create (X3DExecutionContext* const executionContext) const
{
	return new PolygonOffsetGroup (executionContext);
}

void
PolygonOffsetGroup::initialize ()
{
	X3DGroupingNode::initialize ();

	type () .addInterest (this, &PolygonOffsetGroup::set_type);

	set_type ();
}

void
PolygonOffsetGroup::set_type ()
{
	if (type () == "POLYGON_OFFSET_POINT")
		offsetType = GL_POLYGON_OFFSET_POINT;

	else if (type () == "POLYGON_OFFSET_LINE")
		offsetType = GL_POLYGON_OFFSET_LINE;

	else
		offsetType = GL_POLYGON_OFFSET_FILL;
}

void
PolygonOffsetGroup::traverse (const TraverseType type)
{
	if (type == TraverseType::DISPLAY)
		getCurrentLayer () -> getLocalObjects () .emplace_back (new PolygonOffsetContainer (this));

	X3DGroupingNode::traverse (type);
}

void
PolygonOffsetGroup::addTool ()
{
	//X3DGroupingNode::addTool (new PolygonOffsetGroupTool (this));
}

} // X3D
} // titania