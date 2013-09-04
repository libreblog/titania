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

#include "ProgramShader.h"

#include "../../Bits/Cast.h"
#include "../../Execution/X3DExecutionContext.h"
#include "../Shaders/ShaderProgram.h"

namespace titania {
namespace X3D {

// http://www.opengl.org/wiki/GLSL_Object#Program_separation
// http://www.opengl.org/wiki/GLAPI/glGenProgramPipelines

ProgramShader::Fields::Fields () :
	programs (new MFNode ())
{ }

ProgramShader::ProgramShader (X3DExecutionContext* const executionContext) :
	  X3DBaseNode (executionContext -> getBrowser (), executionContext),
	X3DShaderNode (),
	       fields (),
	   pipelineId (0)
{
	setComponent ("Shaders");
	setTypeName ("ProgramShader");

	addField (inputOutput,    "metadata",   metadata ());
	addField (inputOnly,      "activate",   activate ());
	addField (outputOnly,     "isSelected", isSelected ());
	addField (outputOnly,     "isValid",    isValid ());
	addField (initializeOnly, "language",   language ());
	addField (inputOutput,    "programs",   programs ());
}

X3DBaseNode*
ProgramShader::create (X3DExecutionContext* const executionContext) const
{
	return new ProgramShader (executionContext);
}

void
ProgramShader::initialize ()
{
	X3DShaderNode::initialize ();

	if (glXGetCurrentContext ())
	{
		activate () .addInterest (this, &ProgramShader::set_activate);

		requestExplicitRelink ();
	}
}

GLint
ProgramShader::getProgramStageBit (const String & type)
{
	if (type == "VERTEX")
		return GL_VERTEX_SHADER_BIT;

	if (type == "TESS_CONTROL")
		return GL_TESS_CONTROL_SHADER_BIT;

	if (type == "TESS_EVALUATION")
		return GL_TESS_EVALUATION_SHADER_BIT;

	if (type == "GEOMETRY")
		return GL_GEOMETRY_SHADER_BIT;

	if (type == "FRAGMENT")
		return GL_FRAGMENT_SHADER_BIT;

	#ifdef GL_COMPUTE_SHADER_BIT

	// Requires GL 4.3 or ARB_compute_shader
	if (type == "COMPUTE")
		return GL_COMPUTE_SHADER_BIT;

	#endif

	return GL_VERTEX_SHADER_BIT;
}

void
ProgramShader::requestExplicitRelink ()
{
	if (pipelineId)
	{
		glDeleteProgramPipelines (1, &pipelineId);
		pipelineId = 0;
	}

	if (language () == "GLSL")
	{
		glGenProgramPipelines (1, &pipelineId);

		glBindProgramPipeline (pipelineId);

		for (const auto & program : programs ())
		{
			auto _program = x3d_cast <ShaderProgram*> (program);

			if (_program)
				glUseProgramStages (pipelineId, getProgramStageBit (_program -> type ()), _program -> getShaderProgramId ());
		}

		isValid () = true;

		glBindProgramPipeline (0);
	}
	else
		isValid () = false;

	if (not isValid () and isSelected ())
		isSelected () = false;

	X3DChildObject::addEvent ();
}

void
ProgramShader::set_activate ()
{
	if (not activate ())
		return;

	requestExplicitRelink ();
}

void
ProgramShader::draw ()
{
	X3DShaderNode::draw ();

	glBindProgramPipeline (pipelineId);
}

void
ProgramShader::dispose ()
{
	if (pipelineId)
		glDeleteProgramPipelines (1, &pipelineId);

	X3DShaderNode::dispose ();
}

} // X3D
} // titania
