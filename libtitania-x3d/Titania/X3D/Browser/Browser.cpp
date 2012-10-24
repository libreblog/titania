/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: tab; c-basic-offset: 3 -*- */
/*******************************************************************************
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

#include "Browser.h"

#include "../Components/EnvironmentalEffects/Fog.h"
#include "../Components/EnvironmentalEffects/X3DBackgroundNode.h"
#include "../Components/Navigation/NavigationInfo.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>

namespace titania {
namespace X3D {

Browser::Browser () :
	    X3DBasicNode (this, this), 
	      X3DBrowser (),           
	objectFrontAlpha (1),          
	 objectBackAlpha (1),          
	               x (0),          
	               y (0)
{
}

X3DBasicNode*
Browser::create (X3DExecutionContext* const) const
{
	return new Browser ();
}

void
Browser::initialize ()
{
	std::clog << "Initializing Browser ..." << std::endl;

	X3DBrowser::initialize ();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GLfloat light_model_ambient [ ] = { 0, 0, 0, 1 };

	glLightModelfv (GL_LIGHT_MODEL_AMBIENT,       light_model_ambient);
	glLightModeli  (GL_LIGHT_MODEL_LOCAL_VIEWER,  0);
	glLightModeli  (GL_LIGHT_MODEL_TWO_SIDE,      GL_TRUE);
	//glLightModeli  (GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);

	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColorMaterial (GL_FRONT_AND_BACK, GL_DIFFUSE);

	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glHint (GL_GENERATE_MIPMAP_HINT,        GL_NICEST);
	glHint (GL_FOG_HINT,                    GL_NICEST);

	glClearColor (0, 0, 0, 0);

	glClearDepth (1);
	glDepthFunc (GL_LEQUAL);       // important

	glEnable (GL_NORMALIZE);       // important

	//glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	//glEnable(GL_POINT_SMOOTH);
	//glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_POLYGON_SMOOTH);

	glLineWidth (1);
	glPointSize (1);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::clog
		<< "\tDone initializing Browser." << std::endl
		<< std::endl;
}

// texture transparency

void
Browser::setObjectAlpha (const float _objectFrontAlpha, const float _objectBackAlpha)
{
	objectFrontAlpha = _objectFrontAlpha;
	objectBackAlpha  = _objectBackAlpha;
}

void
Browser::getObjectAlpha (float & _objectFrontAlpha, float & _objectBackAlpha)
{
	_objectFrontAlpha = objectFrontAlpha;
	_objectBackAlpha  = objectBackAlpha;
}

// texture

GLuint
Browser::getTexture (const std::string & URL)
{
	TextureMap::const_iterator iter = textures .find (URL);

	if (iter not_eq textures .end ())
	{
		++ textureRefCounts [iter -> second];
		return iter -> second;
	}

	return 0;
}

void
Browser::addTexture (const std::string & URL, GLuint textureId)
{
	textures [URL]               = textureId;
	textureRefCounts [textureId] = 1;
}

bool
Browser::removeTexture (const std::string & URL, GLuint textureId)
{
	-- textureRefCounts [textureId];

	if (textureRefCounts [textureId] == 0)
	{
		textures .erase (URL);
		textureRefCounts .erase (textureId);
		return true;
	}

	return false;
}

//

void
Browser::setXY (const size_t _x, const size_t _y)
{
	x = _x;
	y = _y;
}

// selection

void
Browser::pick (const size_t x, const size_t y)
{
	selectionBegin ();
	setXY (x, y);
	world -> select ();
	selectionEnd ();
}

void
Browser::selectionBegin ()
{
	clearHits ();
}

void
Browser::selectionEnd ()
{
	sensitiveNodes .clear ();
	std::sort (hits .begin (), hits .end (), hitComp);
}

Line3f
Browser::getHitRay () const
{
	GLint                viewport [4]; // x, y, width, heigth
	Matrix4d::array_type modelview, projection;

	glGetIntegerv (GL_VIEWPORT, viewport);
	glGetDoublev (GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev (GL_PROJECTION_MATRIX, projection);

	GLdouble px, py, pz;

	gluUnProject (x, y, 0, modelview, projection, viewport, &px, &py, &pz);
	Vector3f position = Vector3f (px, py, pz);

	gluUnProject (x, y, 1, modelview, projection, viewport, &px, &py, &pz);
	Vector3f point = Vector3f (px, py, pz);

	return Line3f (position, point);
}

void
Browser::clearHits ()
{
	for (const auto & hit : hits)
		delete hit;

	hits .clear ();
}

//

void
Browser::replaceWorld (const SFNode <Scene> & scene)
throw (Error <INVALID_SCENE>)
{
	clearHits ();

	rendererStack .clear ();

	X3DBrowser::replaceWorld (scene);
}

Browser::~Browser ()
{ }

} // X3D
} // titania
