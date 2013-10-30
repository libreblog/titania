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

#include "Viewpoint.h"

#include "../../Browser/X3DBrowser.h"
#include "../../Execution/X3DExecutionContext.h"

namespace titania {
namespace X3D {

const std::string Viewpoint::componentName  = "Navigation";
const std::string Viewpoint::typeName       = "Viewpoint";
const std::string Viewpoint::containerField = "children";

Viewpoint::Fields::Fields () :
	   position (new SFVec3f (0, 0, 10)),
	fieldOfView (new SFFloat (0.785398))
{ }

Viewpoint::Viewpoint (X3DExecutionContext* const executionContext) :
	     X3DBaseNode (executionContext -> getBrowser (), executionContext),
	X3DViewpointNode (),
	          fields ()
{
	addField (inputOutput, "metadata",          metadata ());
	addField (inputOutput, "description",       description ());
	addField (inputOnly,   "set_bind",          set_bind ());
	addField (inputOutput, "position",          position ());
	addField (inputOutput, "orientation",       orientation ());
	addField (inputOutput, "centerOfRotation",  centerOfRotation ());
	addField (inputOutput, "fieldOfView",       fieldOfView ());
	addField (inputOutput, "jump",              jump ());
	addField (inputOutput, "retainUserOffsets", retainUserOffsets ());
	addField (outputOnly,  "isBound",           isBound ());
	addField (outputOnly,  "bindTime",          bindTime ());
}

X3DBaseNode*
Viewpoint::create (X3DExecutionContext* const executionContext) const
{
	return new Viewpoint (executionContext);
}

Vector3d
Viewpoint::getScreenScale (double distance, const Vector4i & viewport) const
{
	int    width  = viewport [2];
	int    height = viewport [3];
	double size   = distance * std::tan (fieldOfView () / 2) * 2;

	if (width > height)
		size /= height;

	else
		size /= width;

	return Vector3d (size, size, size);
}

Vector3f
Viewpoint::getLookAtPositionOffset (Box3f bbox) const
{
	if (getBrowser () -> getActiveLayer ())
	{
		float distance    = (abs (bbox .size ()) / 2) / std::tan (fieldOfView () / 2);
		float minDistance = getBrowser () -> getActiveLayer () -> getNavigationInfo () -> getNearPlane () * 2;

		return bbox .center ()
		       + getUserOrientation () * Vector3f (0, 0, std::max (distance, minDistance))
		       - position ();
	}

	return Vector3f ();
}

void
Viewpoint::reshape (const float zNear, const float zFar)
{
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();

	Vector4i viewport = Viewport4i ();

	size_t width  = viewport [2];
	size_t height = viewport [3];

	float fov = fieldOfView () > 0 and fieldOfView () < M_PI ? fieldOfView () :
						M_PI / 4;
	float ratio = std::tan (fov / 2) * zNear;

	if (width > height)
	{
		float aspect = width * ratio / height;
		glFrustum (-aspect, aspect, -ratio, ratio, zNear, zFar);
	}
	else
	{
		float aspect = height * ratio / width;
		glFrustum (-ratio, ratio, -aspect, aspect, zNear, zFar);
	}

	glMatrixMode (GL_MODELVIEW);
}

} // X3D
} // titania
