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

#ifndef __TITANIA_X3D_COMPONENTS_SOUND_X3DSOUND_SOURCE_NODE_H__
#define __TITANIA_X3D_COMPONENTS_SOUND_X3DSOUND_SOURCE_NODE_H__

#include "../Time/X3DTimeDependentNode.h"

#include <Titania/Basic/URI.h>
#include <glibmm/refptr.h>
#include <memory>

namespace titania {
namespace X3D {

class MediaStream;

class X3DSoundSourceNode :
	public X3DTimeDependentNode
{
public:

	///  @name Construction

	virtual
	void
	setExecutionContext (X3DExecutionContext* const executionContext) override;

	///  @name Fields

	virtual
	SFBool &
	enabled () final override
	{ return fields .enabled; }

	virtual
	const SFBool &
	enabled () const final override
	{ return fields .enabled; }

	SFString &
	description ()
	{ return *fields .description; }

	const SFString &
	description () const
	{ return *fields .description; }

	virtual
	SFFloat &
	speed () = 0;

	virtual
	const SFFloat &
	speed () const = 0;

	SFFloat &
	pitch ()
	{ return *fields .pitch; }

	const SFFloat &
	pitch () const
	{ return *fields .pitch; }

	virtual
	SFBool &
	isActive () final override
	{ return *fields .isActive; }

	virtual
	const SFBool &
	isActive () const final override
	{ return *fields .isActive; }

	SFTime &
	duration_changed ()
	{ return *fields .duration_changed; }

	const SFTime &
	duration_changed () const
	{ return *fields .duration_changed; }

	///  @name Modifiers

	void
	setVolume (const float);

	///  @name Destruction

	virtual
	void
	dispose () override;

	virtual
	~X3DSoundSourceNode () override;


protected:

	///  @name Construction

	X3DSoundSourceNode (const bool video);

	virtual
	void
	initialize () override;

	///  @name Opertions

	const std::unique_ptr <MediaStream> &
	getStream () const;

	void
	setMedia (const bool value);

	///  @name Event handlers

	virtual
	void
	set_time () override;


private:

	///  @name Event handlers

	void
	on_end ();

	void
	set_speed ();

	void
	set_pitch ();

	virtual
	void
	set_start () final override;

	virtual
	void
	set_stop () final override;

	virtual
	void
	set_resume (const time_type) final override;

	virtual
	void
	set_pause () final override;

	void
	set_end ();

	void
	on_duration_changed ();

	///  @name Fields

	struct Fields
	{
		Fields ();

		SFBool enabled;
		SFString* const description;
		SFFloat* const pitch;
		SFBool* const isActive;
		SFTime* const duration_changed;
	};

	Fields fields;

	///  @name Members

	SFTime                        end;
	std::unique_ptr <MediaStream> mediaStream;
	bool                          media;
	const bool                    video;

};

} // X3D
} // titania

#endif
