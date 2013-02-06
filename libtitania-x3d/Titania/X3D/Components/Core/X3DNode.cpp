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

#include "X3DNode.h"

#include "../../Browser/X3DBrowser.h"

namespace titania {
namespace X3D {

Matrix4f X3DNode::matrix; 
Matrix4f X3DNode::currentMatrix;
MatrixStack X3DNode::matrixStack;
MatrixStack X3DNode::currentMatrixStack;

X3DNode::X3DNode () :
	       X3DBaseNode (), 
	          metadata ()  // SFNode [in,out] metadata  NULL        [X3DMetadataObject]
{
	addNodeType (X3DConstants::X3DNode);
}

Scene*
X3DNode::getScene () const
{
	X3DExecutionContext* executionContext = getExecutionContext ();

	while (not executionContext -> isScene ())
		executionContext = executionContext -> getExecutionContext ();

	return static_cast <Scene*> (executionContext);
}

X3DLayerNode*
X3DNode::getCurrentLayer () const
{
	return getBrowser () -> getLayers () .top ();
}

std::deque <X3DLayerNode*>
X3DNode::getLayers () const
{
	return findClosestParents <Scene, X3DLayerNode> ();
}

X3DViewpointNode*
X3DNode::getCurrentViewpoint () const
{
	return getCurrentLayer () -> getViewpoint ();
}

const Matrix4f &
X3DNode::getMatrix ()
{
	return matrix;
}

const Matrix4f &
X3DNode::getCurrentMatrix ()
{
	return currentMatrix;
}

void
X3DNode::loadIdentity ()
{
	matrix = Matrix4f ();
}

void
X3DNode::multMatrix (const Matrix4f & value, const Matrix4f & currentValue)
{
	matrix        = value * matrix;
	currentMatrix = currentMatrix * currentValue;
}

void
X3DNode::pushMatrix ()
{
	matrixStack .emplace_back (matrix);
	currentMatrixStack .emplace_back (matrix);
}

void
X3DNode::popMatrix ()
{
	matrixStack .pop_back ();
	currentMatrixStack .pop_back ();
}

} // X3D
} // titania
