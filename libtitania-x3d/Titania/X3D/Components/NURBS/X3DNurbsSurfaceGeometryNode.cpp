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

#include "X3DNurbsSurfaceGeometryNode.h"

#include "../../Bits/Cast.h"
#include "../Rendering/X3DCoordinateNode.h"
#include "../Texturing/X3DTextureCoordinateNode.h"

namespace titania {
namespace X3D {

X3DNurbsSurfaceGeometryNode::Fields::Fields () :
	uTessellation (new SFInt32 ()),
	vTessellation (new SFInt32 ()),
	      uClosed (new SFBool ()),
	      vClosed (new SFBool ()),
	        solid (new SFBool (true)),
	       uOrder (new SFInt32 (3)),
	       vOrder (new SFInt32 (3)),
	   uDimension (new SFInt32 ()),
	   vDimension (new SFInt32 ()),
	        uKnot (new MFDouble ()),
	        vKnot (new MFDouble ()),
	       weight (new MFDouble ()),
	     texCoord (new SFNode ()),
	 controlPoint (new SFNode ())
{ }

X3DNurbsSurfaceGeometryNode::X3DNurbsSurfaceGeometryNode () :
	X3DParametricGeometryNode (),
	                   fields (),
	                     type (0),
	                texCoords (),
	                  normals (),
	                 vertices ()
{
	addNodeType (X3DConstants::X3DNurbsSurfaceGeometryNode);
}

size_t
X3DNurbsSurfaceGeometryNode::getUTessellation () const
{
	if (uTessellation () > 0)
		return uTessellation ();

	return 3;
}

size_t
X3DNurbsSurfaceGeometryNode::getVTessellation () const
{
	if (vTessellation () > 0)
		return vTessellation ();

	return 3;
}

std::vector <float>
X3DNurbsSurfaceGeometryNode::getKnots (const MFDouble & knot, const SFInt32 & order, const SFInt32 & dimension) const
{
	std::vector <float> knots (knot .begin (), knot .end ());

	// check the knot-vectors. If they are not according to standard
	// default uniform knot vectors will be generated.

	bool generateUniform = true;

	if (knots .size () == size_t (dimension + order))
	{
		generateUniform = false;

		size_t consecutiveKnots = 0;

		for (size_t i = 1; i < knots .size (); ++ i)
		{
			if (knots [i] == knots [i - 1])
				++ consecutiveKnots;
			else
				consecutiveKnots = 0;

			if (consecutiveKnots > size_t (order - 1))
				generateUniform = true;

			if (knots [i - 1] > knots [i])
				generateUniform = true;
		}
	}

	if (generateUniform)
	{
		knots .resize (dimension + order);

		for (size_t i = 0, size = knots .size (); i < size; ++ i)
			knots [i] = (float) i / (size - 1);
	}

	return knots;
}

void
X3DNurbsSurfaceGeometryNode::build ()
{
	__LOG__ << getName () << std::endl;

	if (uOrder () < 2)
		return;

	if (vOrder () < 2)
		return;

	if (uDimension () < uOrder ())
		return;

	if (vDimension () < vOrder ())
		return;

	auto coordinate = x3d_cast <X3DCoordinateNode*> (controlPoint ());

	if (not coordinate)
		return;

	if (coordinate -> getSize () not_eq size_t (uDimension () * vDimension ()))
		return;

	// ControlPoints

	std::vector <Vector4f> controlPoints = std::move (coordinate -> getControlPoints (weight ()));

	// Knots

	std::vector <float> uKnots = std::move (getKnots (uKnot (), uOrder (), uDimension ()));
	std::vector <float> vKnots = std::move (getKnots (vKnot (), vOrder (), vDimension ()));

	// TextureCoordinate

	//	auto _textureCoordinate = x3d_cast <X3DTextureCoordinateNode*> (texCoord ());
	//
	//	if (_textureCoordinate)
	//		_textureCoordinate -> init (getTexCoord (), reserve);
	//	else
	getTexCoord () .emplace_back ();

	// Default unit square
	std::vector <Vector4f> texControlPoints;
	std::vector <float>    texUKnot;
	std::vector <float>    texVKnot;

	texControlPoints .emplace_back (0, 0, 0, 1);
	texControlPoints .emplace_back (1, 0, 0, 1);
	texControlPoints .emplace_back (0, 1, 0, 1);
	texControlPoints .emplace_back (1, 1, 0, 1);

	texUKnot .emplace_back (uKnots [0]);
	texUKnot .emplace_back (uKnots [0]);
	texUKnot .emplace_back (uKnots .back ());
	texUKnot .emplace_back (uKnots .back ());

	texVKnot .emplace_back (vKnots [0]);
	texVKnot .emplace_back (vKnots [0]);
	texVKnot .emplace_back (vKnots .back ());
	texVKnot .emplace_back (vKnots .back ());

	// Tessellation

	GLUnurbs* nurbsRenderer = gluNewNurbsRenderer ();

	gluNurbsProperty (nurbsRenderer, GLU_NURBS_MODE, GLU_NURBS_TESSELLATOR);

	gluNurbsCallbackData (nurbsRenderer, this);

	gluNurbsCallback (nurbsRenderer, GLU_NURBS_BEGIN_DATA,         _GLUfuncptr (&X3DNurbsSurfaceGeometryNode::tessBeginData));
	gluNurbsCallback (nurbsRenderer, GLU_NURBS_TEXTURE_COORD_DATA, _GLUfuncptr (&X3DNurbsSurfaceGeometryNode::tessTexCoordData));
	gluNurbsCallback (nurbsRenderer, GLU_NURBS_NORMAL_DATA,        _GLUfuncptr (&X3DNurbsSurfaceGeometryNode::tessNormalData));
	gluNurbsCallback (nurbsRenderer, GLU_NURBS_VERTEX_DATA,        _GLUfuncptr (&X3DNurbsSurfaceGeometryNode::tessVertexData));
	gluNurbsCallback (nurbsRenderer, GLU_NURBS_END_DATA,           _GLUfuncptr (&X3DNurbsSurfaceGeometryNode::tessEndData));
	gluNurbsCallback (nurbsRenderer, GLU_ERROR,                    _GLUfuncptr (&X3DNurbsSurfaceGeometryNode::tessError));

	// Options

	gluNurbsProperty (nurbsRenderer, GLU_SAMPLING_METHOD, GLU_DOMAIN_DISTANCE);
	gluNurbsProperty (nurbsRenderer, GLU_U_STEP, getUTessellation ());
	gluNurbsProperty (nurbsRenderer, GLU_V_STEP, getVTessellation ());
	glEnable (GL_AUTO_NORMAL);

	// Tessellation

	gluBeginSurface (nurbsRenderer);

	gluNurbsSurface (nurbsRenderer,
	                 4, texUKnot .data (),
	                 4, texVKnot .data (),
	                 4, 4 * 2,
	                 texControlPoints [0] .data (),
	                 2, 2,
	                 GL_MAP2_TEXTURE_COORD_4);

	gluNurbsSurface (nurbsRenderer,
	                 uKnots .size (), uKnots .data (),
	                 vKnots .size (), vKnots .data (),
	                 4, 4 * uDimension (),
	                 controlPoints [0] .data (),
	                 uOrder (), vOrder (),
	                 GL_MAP2_VERTEX_4);

	trim ();

	gluEndSurface (nurbsRenderer);

	glDisable (GL_AUTO_NORMAL);
	gluDeleteNurbsRenderer (nurbsRenderer);

	// End tessellation

	addElements (getVertexMode (4), getVertices () .size ());
	setSolid (solid ());
	setCCW (true);
	setTextureCoordinate (nullptr);
}

void
X3DNurbsSurfaceGeometryNode::tessBeginData (GLenum type, X3DNurbsSurfaceGeometryNode* self)
{
	self -> type = type;
	self -> texCoords .clear ();
	self -> normals .clear ();
	self -> vertices .clear ();
}

void
X3DNurbsSurfaceGeometryNode::tessTexCoordData (GLfloat* texCoord, X3DNurbsSurfaceGeometryNode* self)
{
	self -> texCoords .emplace_back (texCoord [0], texCoord [1], texCoord [2], texCoord [3]);
}

void
X3DNurbsSurfaceGeometryNode::tessNormalData (GLfloat* normal, X3DNurbsSurfaceGeometryNode* self)
{
	self -> normals .emplace_back (normal [0], normal [1], normal [2]);
}

void
X3DNurbsSurfaceGeometryNode::tessVertexData (GLfloat* vertex, X3DNurbsSurfaceGeometryNode* self)
{
	self -> vertices .emplace_back (vertex [0], vertex [1], vertex [2]);
}

void
X3DNurbsSurfaceGeometryNode::tessEndData (X3DNurbsSurfaceGeometryNode* self)
{
	switch (self -> type)
	{
		case GL_TRIANGLE_FAN:
			break;
		case GL_TRIANGLE_STRIP:
			break;
		case GL_TRIANGLES:
			break;
		case GL_QUAD_STRIP:
		{
			__LOG__
				<< self -> texCoords .size () << " : "
				<< self -> normals .size () << " : "
				<< self -> vertices .size () << " : "
				<< std::endl;

			for (size_t i = 0, size = self -> vertices .size () - 2; i < size; i += 2)
			{
				size_t i1 = i;
				size_t i2 = i + 1;
				size_t i3 = i + 3;
				size_t i4 = i + 2;

				self -> getTexCoord () [0] .emplace_back (self -> texCoords [i1]);
				self -> getTexCoord () [0] .emplace_back (self -> texCoords [i2]);
				self -> getTexCoord () [0] .emplace_back (self -> texCoords [i3]);
				self -> getTexCoord () [0] .emplace_back (self -> texCoords [i4]);

				self -> getNormals () .emplace_back (self -> normals [i1]);
				self -> getNormals () .emplace_back (self -> normals [i2]);
				self -> getNormals () .emplace_back (self -> normals [i3]);
				self -> getNormals () .emplace_back (self -> normals [i4]);

				self -> getVertices () .emplace_back (self -> vertices [i1]);
				self -> getVertices () .emplace_back (self -> vertices [i2]);
				self -> getVertices () .emplace_back (self -> vertices [i3]);
				self -> getVertices () .emplace_back (self -> vertices [i4]);
			}

			break;
		}
		default:
			break;
	}
}

void
X3DNurbsSurfaceGeometryNode::tessError (GLenum errorCode)
{
	__LOG__ << gluErrorString (errorCode) << std::endl;
}

} // X3D
} // titania
