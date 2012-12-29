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
 ******************************************************************************/

#include "OrthoViewpoint.h"

#include "../../Browser/Browser.h"
#include "../../Execution/X3DExecutionContext.h"

namespace titania {
namespace X3D {

OrthoViewpoint::OrthoViewpoint (X3DExecutionContext* const executionContext, bool addToList) :
	     X3DBaseNode (executionContext -> getBrowser (), executionContext), 
	X3DViewpointNode (addToList),                                           
	        position (0, 0, 10),                                            // SFVec3f [in,out] position           0 0 10              (-∞,∞)
	     fieldOfView ({ -1 })                                               // MFFloat [in,out] fieldOfView        -1, -1, 1, 1        (-∞,∞)
{
	setComponent ("Navigation");
	setTypeName ("OrthoViewpoint");

	addField (inputOutput, "metadata",          metadata);
	addField (inputOutput, "description",       description);
	addField (inputOnly,   "set_bind",          set_bind);
	addField (inputOutput, "position",          position);
	addField (inputOutput, "orientation",       orientation);
	addField (inputOutput, "centerOfRotation",  centerOfRotation);
	addField (inputOutput, "fieldOfView",       fieldOfView);
	addField (inputOutput, "jump",              jump);
	addField (inputOutput, "retainUserOffsets", retainUserOffsets);
	addField (outputOnly,  "bindTime",          bindTime);
	addField (outputOnly,  "isBound",           isBound);
}

void
OrthoViewpoint::initialize ()
{
	X3DViewpointNode::initialize ();

	timeSensor                  = new TimeSensor (getExecutionContext ());
	timeSensor -> stopTime      = 1;
	timeSensor -> cycleInterval = 0.2;
	timeSensor -> setup ();

	positionInterpolator        = new PositionInterpolator (getExecutionContext ());
	positionInterpolator -> key = { 0, 1 };
	positionInterpolator -> setup ();

	timeSensor           -> fraction_changed .addInterest (positionInterpolator -> set_fraction);
	timeSensor           -> isActive         .addInterest (this, &OrthoViewpoint::set_active);
	positionInterpolator -> value_changed    .addInterest (positionOffset);
}

void
OrthoViewpoint::set_active (const bool & value)
{
	if (not value)
	{
		for (const auto & layer : getLayers ())
			layer -> navigationInfoStack .top () -> transitionComplete = getCurrentTime ();
	}
}

X3DBaseNode*
OrthoViewpoint::create (X3DExecutionContext* const executionContext) const
{
	return new OrthoViewpoint (executionContext, true);
}

Vector3f
OrthoViewpoint::getPosition () const
{
	return position;
}

std::array <float, 4>
OrthoViewpoint::getFieldOfView ()
{

	switch (fieldOfView .size ())
	{
		case 0:
			return std::array <float, 4> {{ -1, -1, 1, 1 }};
			
		case 1:
			return std::array <float, 4> {{ fieldOfView [0], -1,
			                                1, 1 }};
		case 2:
			return std::array <float, 4> {{ fieldOfView [0], fieldOfView [1],
			                                1, 1 }};
		case 3:
			return std::array <float, 4> {{ fieldOfView [0], fieldOfView [1],
			                                fieldOfView [2], 1 }};
		default:
			return std::array <float, 4> {{ fieldOfView [0], fieldOfView [1],
			                                fieldOfView [2], fieldOfView [3] }};
	}
}

void
OrthoViewpoint::lookAt (Box3f bbox)
{
 	std::clog << "Look at using viewpoint: " << description << "." << std::endl;

	bbox *= ~getModelViewMatrix ();

	Vector3f positionOffset = bbox .center ()
	                          + getUserOrientation () * (Vector3f (0, 0, bbox .greater_radius () + 10))
	                          - position;

	timeSensor -> startTime          = 1;
	positionInterpolator -> keyValue = { this -> positionOffset, positionOffset };

	centerOfRotation       = bbox .center ();
	centerOfRotationOffset = Vector3f ();
	set_bind               = true;

	std::clog << getTypeName () << " {" << std::endl;
	std::clog << "  position " << getUserPosition () << std::endl;
	std::clog << "  orientation " << getUserOrientation () << std::endl;
	std::clog << "  centerOfRotation " << getUserCenterOfRotation () << std::endl;
	std::clog << "}" << std::endl;
}

void
OrthoViewpoint::reshape (const float zNear, const float zFar)
{
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();

	GLfloat viewport [4];
	glGetFloatv (GL_VIEWPORT, viewport);

	GLfloat width  = viewport [2];
	GLfloat height = viewport [3];

	std::array <float, 4> fieldOfView = getFieldOfView ();

	float size_x = abs (getModelViewMatrix () .multDirMatrix (Vector3f (fieldOfView [2] - fieldOfView [0], 0, 0))) / 2;
	float size_y = abs (getModelViewMatrix () .multDirMatrix (Vector3f (0, fieldOfView [3] - fieldOfView [1], 0))) / 2;

	if (width > height)
	{
		float x = width * size_x / height;
		float y = size_y;

		glOrtho (-x, x, -y, y, zNear, zFar);
	}
	else
	{
		float x = size_x;
		float y = height * size_y / width;

		glOrtho (-x, x, -y, y, zNear, zFar);
	}

	glMatrixMode (GL_MODELVIEW);
}

} // X3D
} // titania
