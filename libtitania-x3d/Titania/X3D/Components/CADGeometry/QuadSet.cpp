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

#include "QuadSet.h"

#include "../../Bits/Cast.h"
#include "../../Execution/X3DExecutionContext.h"
#include "../Rendering/Color.h"
#include "../Rendering/ColorRGBA.h"
#include "../Rendering/Coordinate.h"
#include "../Rendering/Normal.h"
#include "../Texturing/TextureCoordinate.h"
#include "../Texturing/TextureCoordinateGenerator.h"

namespace titania {
namespace X3D {

QuadSet::QuadSet (X3DExecutionContext* const executionContext) :
	            X3DBaseNode (executionContext -> getBrowser (), executionContext), 
	X3DComposedGeometryNode ()                                                     
{
	setComponent ("CADGeometry");
	setTypeName ("QuadSet");

	addField (inputOutput,    "metadata",        metadata);

	addField (initializeOnly, "solid",           solid);
	addField (initializeOnly, "ccw",             ccw);
	addField (initializeOnly, "colorPerVertex",  colorPerVertex);
	addField (initializeOnly, "normalPerVertex", normalPerVertex);

	addField (inputOutput,    "attrib",          attrib);
	addField (inputOutput,    "fogCoord",        fogCoord);
	addField (inputOutput,    "texCoord",        texCoord);
	addField (inputOutput,    "color",           color);
	addField (inputOutput,    "normal",          normal);
	addField (inputOutput,    "coord",           coord);
}

X3DBaseNode*
QuadSet::create (X3DExecutionContext* const executionContext) const
{
	return new QuadSet (executionContext);
}

void
QuadSet::build ()
{
	auto _coord = x3d_cast <Coordinate*> (coord .getValue ());

	// Build arrays

	if (not _coord or not _coord -> point .size ())
		return;

	size_t reserve = _coord -> point .size () / 4 * 4;

	// Color

	auto _color     = x3d_cast <Color*> (color .getValue ());
	auto _colorRGBA = x3d_cast <ColorRGBA*> (color .getValue ());

	if (_color)
	{
		_color -> resize (colorPerVertex ? reserve : reserve / 4);
		getColors () .reserve (reserve);
	}

	else if (_colorRGBA)
	{
		_colorRGBA -> resize (colorPerVertex ? reserve : reserve / 4);
		getColorsRGBA () .reserve (reserve);
	}

	// TextureCoordinate

	auto _textureCoordinate          = x3d_cast <TextureCoordinate*> (texCoord .getValue ());
	auto _textureCoordinateGenerator = x3d_cast <TextureCoordinateGenerator*> (texCoord .getValue ());

	if (_textureCoordinate)
		_textureCoordinate -> resize (reserve);

	if (_textureCoordinate or not _textureCoordinateGenerator)
		getTexCoord () .reserve (reserve);

	// Normal

	auto _normal = x3d_cast <Normal*> (normal .getValue ());
	
	if (_normal)
		_normal -> resize (normalPerVertex ? reserve : reserve / 4);

	getNormals () .reserve (reserve);

	// Vertices

	getVertices () .reserve (reserve);

	// Fill GeometryNode

	for (size_t index = 0, face = 0; index < reserve; ++ face)
	{
		Vector3f    faceNormal;
		SFColor     faceColor;
		SFColorRGBA faceColorRGBA;

		if (not colorPerVertex)
		{
			if (_color)
				faceColor = _color -> color [face];

			else if (_colorRGBA)
				faceColorRGBA = _colorRGBA -> color [face];
		}

		if (_normal)
		{
			if (not normalPerVertex)
				faceNormal = _normal -> vector [face];
		}

		for (size_t i = 0; i < 4; ++ i, ++ index)
		{
			if (_color)
			{
				if (colorPerVertex)
					getColors () .emplace_back (_color -> color [index]);

				else
					getColors () .emplace_back (faceColor);
			}
			else if (_colorRGBA)
			{
				if (colorPerVertex)
					getColorsRGBA () .emplace_back (_colorRGBA -> color [index]);

				else
					getColorsRGBA () .emplace_back (faceColorRGBA);
			}

			if (_textureCoordinate)
			{
				getTexCoord () .emplace_back (_textureCoordinate -> point [index]);
			}

			if (_normal)
			{
				if (normalPerVertex)
					getNormals () .emplace_back (_normal -> vector [index]);

				else
					getNormals () .emplace_back (faceNormal);
			}

			getVertices () .emplace_back (_coord -> point [index]);
		}
	}

	// Autogenerate normal and texCoord if not specified

	if (not _textureCoordinate and not _textureCoordinateGenerator)
		buildTexCoord ();

	if (not _normal)
		buildNormals ();

	setTextureCoordinateGenerator (_textureCoordinateGenerator);
	setVertexMode (GL_QUADS);
	setSolid (solid);
}

void
QuadSet::buildNormals ()
{
	auto _coord = x3d_cast <Coordinate*> (coord .getValue ());
	
	size_t reserve = _coord -> point .size ();
	
	for (size_t index = 0; index < reserve; index += 4)
	{
		// Determine polygon normal.
		Vector3f normal = math::normal <float> (_coord -> point [index],
			                                     _coord -> point [index + 0],
			                                     _coord -> point [index + 1],
			                                     _coord -> point [index + 2]);
		
		getNormals () .resize (getNormals () .size () + 4, normal);
	}
}

} // X3D
} // titania
