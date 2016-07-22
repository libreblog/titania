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

#include "X3DIndexedFaceSetCutObject.h"

#include "../../Rendering/CoordinateTool.h"

#include "../../../Browser/X3DBrowser.h"
#include "../../../Components/Geometry2D/Arc2D.h"
#include "../../../Components/Grouping/Transform.h"
#include "../../../Components/Grouping/Switch.h"
#include "../../../Components/PointingDeviceSensor/TouchSensor.h"
#include "../../../Components/PointingDeviceSensor/PlaneSensor.h"
#include "../../../Components/NURBS/CoordinateDouble.h"
#include "../../../Editing/Selection/FaceSelection.h"
#include "../../../Rendering/ViewVolume.h"

namespace titania {
namespace X3D {

static constexpr double SELECTION_DISTANCE = 8; // in pixel

X3DIndexedFaceSetCutObject::Fields::Fields () :
	 cutSnapping (new SFBool (true))
{ }

X3DIndexedFaceSetCutObject::X3DIndexedFaceSetCutObject () :
	                  IndexedFaceSet (getExecutionContext ()),
	X3DIndexedFaceSetSelectionObject (),
	                          fields ()
{
	//addType (X3DConstants::X3DIndexedFaceSetCutObject);
}

/**
 *  Cuts a face into two connected faces. Run rebuildIndices, rebuildColor, rebuildTexCoord, rebuildNormal and
 *  rebuildCoord after this operation to remove degenerated faces.
 *
 *  @param  cutFace      Index of face to cut
 *  @param  cutEdge      Coordinates of edge to cut
 *  @param  startPoints  One or two points, depending on whether start from point or edge
 *  @param  endPoints    One or two points, depending on whether end at point or edge
 */

std::vector <int32_t>
X3DIndexedFaceSetCutObject::cut (const size_t cutFace,
                                 const std::pair <Vector3d, Vector3d> & cutEdge,
                                 const std::vector <int32_t> & startPoints,
                                 const std::vector <int32_t> & endPoints)
{
	const auto numFaces   = getFaceSelection () -> getNumFaces ();
	const auto faceNumber = getFaceSelection () -> getFaceNumber (cutFace);
	const auto vertices   = getFaceSelection () -> getFaceVertices (cutFace);
	const auto startEdge  = getFaceSelection () -> getClosestEdge (cutEdge .first,  vertices);
	const auto endEdge    = getFaceSelection () -> getClosestEdge (cutEdge .second, vertices);

	std::vector <int32_t> selection;

	int32_t startColor   = -1;
	int32_t endColor     = -1;
	int32_t startTex     = -1;
	int32_t endTex       = -1;
	int32_t startNormal  = -1;
	int32_t endNormal    = -1;
	int32_t startPoint   = -1;
	int32_t endPoint     = -1;
	size_t  startVertex1 = -1;
	size_t  startVertex2 = -1;
	size_t  endVertex1   = -1;
	size_t  endVertex2   = -1;

	std::vector <int32_t> colorFace1;
	std::vector <int32_t> colorFace2;
	std::vector <int32_t> texFace1;
	std::vector <int32_t> texFace2;
	std::vector <int32_t> normalFace1;
	std::vector <int32_t> normalFace2;
	std::vector <int32_t> face1;
	std::vector <int32_t> face2;

	std::vector <size_t> adjacentFaces1;
	std::vector <size_t> adjacentFaces2;

	switch (startPoints .size ())
	{
		case 1:
		{
			startVertex1 = getFaceSelection () -> getClosestVertex (getCoord () -> get1Point (startPoints [0]), vertices);
			endVertex2   = startVertex1;
			break;
		}
		case 2:
		{
			const auto  point1  = getCoord () -> get1Point (coordIndex () .get1Value (startEdge .index0));
			const auto  point2  = getCoord () -> get1Point (coordIndex () .get1Value (startEdge .index1));
			const auto  segment = LineSegment3d (point1, point2);
			const float t       = abs (cutEdge .first - point1) / abs (point2 - point1);

			if (not segment .is_between (cutEdge .first))
				return selection;

			if (colorIndex () .size () and getColor ())
			{
			   if (colorPerVertex ())
			   {
					const auto color1 = getColor () -> get1Color (colorIndex () .get1Value (startEdge .index0));
					const auto color2 = getColor () -> get1Color (colorIndex () .get1Value (startEdge .index1));

					startColor = getColor () -> getSize ();
					getColor () -> set1Color (startColor, clerp (color1, color2, t));
				}
			}

			if (normalIndex () .size () and getNormal ())
			{
			   if (normalPerVertex ())
			   {
					const auto normal1 = getNormal () -> get1Vector (normalIndex () .get1Value (startEdge .index0));
					const auto normal2 = getNormal () -> get1Vector (normalIndex () .get1Value (startEdge .index1));

					startNormal = getNormal () -> getSize ();
					getNormal () -> set1Vector (startNormal, lerp (normal1, normal2, t));
				}
			}

			if (texCoordIndex () .size () and getTexCoord ())
			{
				const auto tex1 = getTexCoord () -> get1Point (texCoordIndex () .get1Value (startEdge .index0));
				const auto tex2 = getTexCoord () -> get1Point (texCoordIndex () .get1Value (startEdge .index1));

				startTex = getTexCoord () -> getSize ();
				getTexCoord () -> set1Point (startTex, lerp (tex1, tex2, t));
			}
			
			startPoint = getCoord () -> getSize ();
			getCoord () -> set1Point (startPoint, cutEdge .first);

			startVertex1   = startEdge .index1;
			endVertex2     = startEdge .index0;
			adjacentFaces1 = getFaceSelection () -> getAdjacentFaces (startEdge);
			break;
		}
		default:
			return selection;
	}

	switch (endPoints .size ())
	{
		case 1:
		{
			endVertex1   = getFaceSelection () -> getClosestVertex (getCoord () -> get1Point (endPoints [0]), vertices);
			startVertex2 = endVertex1;
			break;
		}
		case 2:
		{
			const auto  point1  = getCoord () -> get1Point (coordIndex () .get1Value (endEdge .index0));
			const auto  point2  = getCoord () -> get1Point (coordIndex () .get1Value (endEdge .index1));
			const auto  segment = LineSegment3d (point1, point2);
			const float t       = abs (cutEdge .second - point1) / abs (point2 - point1);

			if (not segment .is_between (cutEdge .second))
				return selection;

			if (colorIndex () .size () and getColor ())
			{
			   if (colorPerVertex ())
			   {
					const auto color1 = getColor () -> get1Color (colorIndex () .get1Value (endEdge .index0));
					const auto color2 = getColor () -> get1Color (colorIndex () .get1Value (endEdge .index1));

					endColor = getColor () -> getSize ();
					getColor () -> set1Color (endColor, clerp (color1, color2, t));
				}
			}

			if (normalIndex () .size () and getNormal ())
			{
			   if (normalPerVertex ())
			   {
					const auto normal1 = getNormal () -> get1Vector (normalIndex () .get1Value (endEdge .index0));
					const auto normal2 = getNormal () -> get1Vector (normalIndex () .get1Value (endEdge .index1));

					endNormal = getNormal () -> getSize ();
					getNormal () -> set1Vector (endNormal, lerp (normal1, normal2, t));
				}
			}
			
			if (texCoordIndex () .size () and getTexCoord ())
			{
				const auto tex1 = getTexCoord () -> get1Point (texCoordIndex () .get1Value (endEdge .index0));
				const auto tex2 = getTexCoord () -> get1Point (texCoordIndex () .get1Value (endEdge .index1));

				endTex = getTexCoord () -> getSize ();
				getTexCoord () -> set1Point (endTex, lerp (tex1, tex2, t));
			}
			
			endPoint = getCoord () -> getSize ();
			getCoord () -> set1Point (endPoint, cutEdge .second);

			endVertex1     = endEdge .index0;
			startVertex2   = endEdge .index1;
			adjacentFaces2 = getFaceSelection () -> getAdjacentFaces (endEdge);
			break;
		}
		default:
			return selection;
	}

	// Rewrite indices.

	startVertex1 = std::find (vertices .begin (), vertices .end (), startVertex1) - vertices .begin ();
	startVertex2 = std::find (vertices .begin (), vertices .end (), startVertex2) - vertices .begin ();
	endVertex1   = std::find (vertices .begin (), vertices .end (), endVertex1)   - vertices .begin ();
	endVertex2   = std::find (vertices .begin (), vertices .end (), endVertex2)   - vertices .begin ();

	if (endVertex1 < startVertex1)
		endVertex1 += vertices .size ();

	if (endVertex2 < startVertex2)
		endVertex2 += vertices .size ();	

	// Add start points.

	if (startPoints .size () == 2)
	{
		if (colorIndex () .size () and getColor ())
			if (colorPerVertex ())
				colorFace1 .emplace_back (startColor);

		if (texCoordIndex () .size () and getTexCoord ())
			texFace1 .emplace_back (startTex);

		if (normalIndex () .size () and getNormal ())
			if (normalPerVertex ())
				normalFace1 .emplace_back (startNormal);

		face1 .emplace_back (startPoint);
	}

	if (endPoints .size () == 2)
	{
		if (colorIndex () .size () and getColor ())
			if (colorPerVertex ())
				colorFace2 .emplace_back (endColor);

		if (texCoordIndex () .size () and getTexCoord ())
			texFace2 .emplace_back (endTex);

		if (normalIndex () .size () and getNormal ())
			if (normalPerVertex ())
				normalFace2 .emplace_back (endNormal);

		face2 .emplace_back (endPoint);
	}

	// Add face points.

	for (size_t i = startVertex1; i <= endVertex1; ++ i)
	{
		const auto vertex = vertices [i % vertices .size ()];

		if (colorIndex () .size () and getColor ())
			colorFace1 .emplace_back (colorIndex () .get1Value (vertex));

		if (texCoordIndex () .size () and getTexCoord ())
			texFace1 .emplace_back (texCoordIndex () .get1Value (vertex));

		if (normalIndex () .size () and getNormal ())
			normalFace1 .emplace_back (normalIndex () .get1Value (vertex));

		face1 .emplace_back (coordIndex () .get1Value (vertex));
	}

	for (size_t i = startVertex2; i <= endVertex2; ++ i)
	{
		const auto vertex = vertices [i % vertices .size ()];

		if (colorIndex () .size () and getColor ())
			colorFace2 .emplace_back (colorIndex () .get1Value (vertex));

		if (texCoordIndex () .size () and getTexCoord ())
			texFace2 .emplace_back (texCoordIndex () .get1Value (vertex));

		if (normalIndex () .size () and getNormal ())
			normalFace2 .emplace_back (normalIndex () .get1Value (vertex));

		face2 .emplace_back (coordIndex () .get1Value (vertex));
	}

	// Add end points.
	
	if (endPoints .size () == 2)
	{
		if (colorIndex () .size () and getColor ())
			if (colorPerVertex ())
				colorFace1 .emplace_back (endColor);

		if (texCoordIndex () .size () and getTexCoord ())
			texFace1 .emplace_back (endTex);

		if (normalIndex () .size () and getNormal ())
			if (normalPerVertex ())
				normalFace1 .emplace_back (endNormal);

		face1 .emplace_back (endPoint);
	}
	
	if (startPoints .size () == 2)
	{
		if (colorIndex () .size () and getColor ())
			if (colorPerVertex ())
				colorFace2 .emplace_back (startColor);

		if (texCoordIndex () .size () and getTexCoord ())
			texFace2 .emplace_back (startTex);

		if (normalIndex () .size () and getNormal ())
			if (normalPerVertex ())
				normalFace2 .emplace_back (startNormal);

		face2 .emplace_back (startPoint);
	}

	// Check if there are degenerated faces.

	if (face1 .size () < 3)
		return selection;

	if (face2 .size () < 3)
		return selection;

	// Add new color faces.

	if (colorIndex () .size () and getColor ())
	{
		if (colorPerVertex ())
		{
			for (const auto & point : colorFace1)
				colorIndex () .emplace_back (point);

			colorIndex () .emplace_back (-1);

			for (const auto & point : colorFace2)
				colorIndex () .emplace_back (point);

			colorIndex () .emplace_back (-1);
		}
		else
		{
			colorIndex () .set1Value (numFaces + 0, colorIndex () .get1Value (faceNumber));
			colorIndex () .set1Value (numFaces + 1, colorIndex () .get1Value (faceNumber));
		}
	}

	// Add new tex faces.

	if (texCoordIndex () .size () and getTexCoord ())
	{
		for (const auto & point : texFace1)
			texCoordIndex () .emplace_back (point);

		texCoordIndex () .emplace_back (-1);

		for (const auto & point : texFace2)
			texCoordIndex () .emplace_back (point);

		texCoordIndex () .emplace_back (-1);
	}

	// Add new normal faces.

	if (normalIndex () .size () and getNormal ())
	{
		if (normalPerVertex ())
		{
			for (const auto & vector : normalFace1)
				normalIndex () .emplace_back (vector);

			normalIndex () .emplace_back (-1);

			for (const auto & vector : normalFace2)
				normalIndex () .emplace_back (vector);

			normalIndex () .emplace_back (-1);
		}
		else
		{
			normalIndex () .set1Value (numFaces + 0, normalIndex () .get1Value (faceNumber));
			normalIndex () .set1Value (numFaces + 1, normalIndex () .get1Value (faceNumber));
		}
	}

	// Add new faces.

	for (const auto & point : face1)
		coordIndex () .emplace_back (point);

	coordIndex () .emplace_back (-1);

	for (const auto & point : face2)
		coordIndex () .emplace_back (point);

	coordIndex () .emplace_back (-1);

	// AdjacentFaces

	if (startPoints .size () == 2)
	{
		for (const auto & face : adjacentFaces1)
			addPoint (cutFace, face, startPoint, cutEdge .first);
	}

	if (endPoints .size () == 2)
	{
		for (const auto & face : adjacentFaces2)
			addPoint (cutFace, face, endPoint, cutEdge .second);
	}

	// Invalidate old face.

	const size_t begin = vertices .front ();
	const size_t end   = vertices .back () + 1;

	std::fill (coordIndex () .begin () + begin, coordIndex () .begin () + end, vertices .front ());

	// 

	return { startPoint, endPoint };
}

void
X3DIndexedFaceSetCutObject::addPoint (const size_t cutFace, const size_t face, const int32_t index, const Vector3d & point)
{
	if (face == cutFace)
		return;

	const auto vertices   = getFaceSelection () -> getFaceVertices (face);
	const auto edge       = getFaceSelection () -> getClosestEdge (point, vertices);
	const auto faceNumber = getFaceSelection () -> getFaceNumber (vertices .front ());

	for (const auto & vertex : vertices)
	{
		if (colorIndex () .size () and getColor ())
		{
			if (colorPerVertex ())
				colorIndex () .emplace_back (colorIndex () [vertex]);
		}

		if (texCoordIndex () .size () and getTexCoord ())
			texCoordIndex () .emplace_back (texCoordIndex () [vertex]);

		if (normalIndex () .size () and getNormal ())
		{
			if (normalPerVertex ())
				normalIndex () .emplace_back (normalIndex () [vertex]);
		}

		coordIndex () .emplace_back (coordIndex () [vertex]);

		if (vertex == edge .index0)
		{
			const auto  point1  = getCoord () -> get1Point (coordIndex () .get1Value (edge .index0));
			const auto  point2  = getCoord () -> get1Point (coordIndex () .get1Value (edge .index1));
			const float t       = abs (point - point1) / abs (point2 - point1);

			if (colorIndex () .size () and getColor ())
			{
			   if (colorPerVertex ())
			   {
					const auto color1 = getColor () -> get1Color (colorIndex () .get1Value (edge .index0));
					const auto color2 = getColor () -> get1Color (colorIndex () .get1Value (edge .index1));

					const auto color = getColor () -> getSize ();
					getColor () -> set1Color (color, clerp (color1, color2, t));
					colorIndex () .emplace_back (color);
				}
			}

			if (texCoordIndex () .size () and getTexCoord ())
			{
				const auto texCoord1 = getTexCoord () -> get1Point (texCoordIndex () .get1Value (edge .index0));
				const auto texCoord2 = getTexCoord () -> get1Point (texCoordIndex () .get1Value (edge .index1));

				const auto texCoord = getTexCoord () -> getSize ();
				getTexCoord () -> set1Point (texCoord, lerp (texCoord1, texCoord2, t));
				texCoordIndex () .emplace_back (texCoord);
			}

			if (normalIndex () .size () and getNormal ())
			{
			   if (normalPerVertex ())
			   {
					const auto normal1 = getNormal () -> get1Vector (normalIndex () .get1Value (edge .index0));
					const auto normal2 = getNormal () -> get1Vector (normalIndex () .get1Value (edge .index1));

					const auto normal = getNormal () -> getSize ();
					getNormal () -> set1Vector (normal, lerp (normal1, normal2, t));
					normalIndex () .emplace_back (normal);
				}
			}

			coordIndex () .emplace_back (index);
		}
	}

	if (colorIndex () .size () and getColor ())
	{
		if (colorPerVertex ())
			colorIndex () .emplace_back (-1);
		else
			colorIndex () .emplace_back (colorIndex () .get1Value (faceNumber));
	}

	if (texCoordIndex () .size () and getTexCoord ())
		texCoordIndex () .emplace_back (-1);

	if (normalIndex () .size () and getNormal ())
	{
	   if (normalPerVertex ())
			normalIndex () .emplace_back (-1);
		else
			normalIndex () .emplace_back (normalIndex () .get1Value (faceNumber));
	}

	coordIndex () .emplace_back (-1);

	// Invalidate old face.

	const size_t begin = vertices .front ();
	const size_t end   = vertices .back () + 1;

	std::fill (coordIndex () .begin () + begin, coordIndex () .begin () + end, vertices .front ());
}

X3DIndexedFaceSetCutObject::~X3DIndexedFaceSetCutObject ()
{ }

} // X3D
} // titania
