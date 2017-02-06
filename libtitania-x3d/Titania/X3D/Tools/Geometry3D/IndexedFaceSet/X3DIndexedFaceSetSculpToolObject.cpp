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

#include "X3DIndexedFaceSetSculpToolObject.h"

#include "../../Rendering/CoordinateTool.h"

#include "../../../Components/PointingDeviceSensor/TouchSensor.h"
#include "../../../Components/Rendering/X3DCoordinateNode.h"

namespace titania {
namespace X3D {

///  Brush
///  SFString type      ["CIRCULAR", "SQUARED"]
///  SFDouble radius    [0, inf]
///  SFDouble height    [-inf, inf]
///  SFDouble warp      [0, 1]
///  SFDouble hardness  [0, 1]
///  SFDouble sharpness [0, 1]
///  SFDouble spacing   [0, inf]

X3DIndexedFaceSetSculpToolObject::X3DIndexedFaceSetSculpToolObject () :
	              IndexedFaceSet (getExecutionContext ()),
	X3DIndexedFaceSetBrushObject (),
	                 touchSensor (),
	                lastHitPoint (),
	             pointerDistance (0)
{
	//addType (X3DConstants::X3DIndexedFaceSetSculpToolObject);

	addChildObjects (touchSensor);
}

void
X3DIndexedFaceSetSculpToolObject::initialize ()
{
	getCoordinateTool () -> getInlineNode () -> checkLoadState () .addInterest (&X3DIndexedFaceSetSculpToolObject::set_loadState, this);

	set_loadState ();
}

void
X3DIndexedFaceSetSculpToolObject::set_loadState ()
{
	try
	{
		const auto & inlineNode = getCoordinateTool () -> getInlineNode ();

		touchSensor = inlineNode -> getExportedNode <TouchSensor> ("SculpBrushTouchSensor");

		touchSensor -> isActive ()         .addInterest (&X3DIndexedFaceSetSculpToolObject::set_touch_sensor_active,   this);
		touchSensor -> hitPoint_changed () .addInterest (&X3DIndexedFaceSetSculpToolObject::set_touch_sensor_hitPoint, this);
	}
	catch (const X3DError & error)
	{
		//__LOG__ << error .what () << std::endl;
	}
}

void
X3DIndexedFaceSetSculpToolObject::set_touch_sensor_active ()
{
	if (touchSensor -> isActive ())
	{
		lastHitPoint    = touchSensor -> getHitPoint ();
		pointerDistance = std::numeric_limits <double>::infinity ();
	}
}

void
X3DIndexedFaceSetSculpToolObject::set_touch_sensor_hitPoint ()
{
	try
	{
		__LOG__ << touchSensor -> getHitPoint () << std::endl;
	
		if (not touchSensor -> isActive ())
			return;
		
		if (not getCoord ())
			return;
	
		const auto & hitPoint = touchSensor -> getHitPoint ();

		pointerDistance += math::distance (hitPoint, lastHitPoint);

		if (pointerDistance > brush () -> getField <SFDouble> ("spacing"))
		{
			const auto & hitNormal = touchSensor -> getHitNormal ();
			const auto & radius    = brush () -> getField <SFDouble> ("radius") .getValue ();
		
			for (size_t i = 0, size = getCoord () -> getSize (); i < size; ++ i)
			{
				const auto point    = getCoord () -> get1Point (i);
				const auto distance = math::distance (hitPoint, point);
		
				if (distance < radius)
					getCoord () -> set1Point (i, point + getHeight (hitNormal, hitPoint, point));
			}

			pointerDistance = 0;
		}

		lastHitPoint = hitPoint;
	}
	catch (const X3DError & error)
	{
		__LOG__ << error .what () << std::endl;
	}
}

Vector3d
X3DIndexedFaceSetSculpToolObject::getHeight (const Vector3d & hitNormal, const Vector3d & hitPoint, const Vector3d & point)
{
	try
	{
		const auto h = brush () -> getField <SFDouble> ("height") ;
		const auto w = 1 + std::pow (brush () -> getField <SFDouble> ("warp") , 8) * 9999;
		const auto s = 2 + brush () -> getField <SFDouble> ("sharpness")  * 98;
		const auto e = std::pow (brush () -> getField <SFDouble> ("hardness") , 4) * 100;

		const auto p = (point - hitPoint) * Rotation4d (hitNormal, Vector3d (0, 0, 1));
		const auto v = Vector2d (p .x (), p .y ());

		const auto & type = brush () -> getField <SFString> ("type");

		if (type == "SQUARED")
			return hitNormal * getCircularHeight (v, w, h, s, e);

		return hitNormal * getCircularHeight (v, w, h, s, e);
	}
	catch (const X3DError & error)
	{
		__LOG__ << error .what () << std::endl;

		return Vector3d ();
	}
}

double
X3DIndexedFaceSetSculpToolObject::getCircularHeight (const Vector2d & v, const double w, const double h, const double s, const double e)
{
	const auto c = abs (v);

	return h * std::pow (w, -std::abs (std::pow (s * c, e)));
}

double
X3DIndexedFaceSetSculpToolObject::getSquaredHeight (const Vector2d & v, const double w, const double h, const double s, const double e)
{
	return (h * std::pow (w, -(std::abs (std::pow (s * std::abs (v .x ()), e)) +
	                           std::abs (std::pow (s * std::abs (v .y ()), e)))));
}

X3DIndexedFaceSetSculpToolObject::~X3DIndexedFaceSetSculpToolObject ()
{ }

} // X3D
} // titania
