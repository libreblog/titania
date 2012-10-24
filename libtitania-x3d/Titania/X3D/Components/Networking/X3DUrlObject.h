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

#ifndef __TITANIA_X3D_COMPONENTS_NETWORKING_X3DURL_OBJECT_H__
#define __TITANIA_X3D_COMPONENTS_NETWORKING_X3DURL_OBJECT_H__

#include "../../Basic/X3DBasicNode.h"
#include "../../Bits/Error.h"
#include "../../Bits/X3DConstants.h"
#include "../../Execution/Scene.h"
#include "../../Fields.h"

#include <Titania/Stream/InputFileStream.h>

namespace titania {
namespace X3D {

enum LoadState
{
	NOT_STARTED_STATE,
	IN_PROGRESS_STATE,
	COMPLETE_STATE,
	FAILED_STATE

};

typedef std::map <const basic::uri, basic::uri> URNIndex;

class X3DUrlObject :
	virtual public X3DBasicNode
{
public:

	///  @name Fields

	MFString url;
	MFString urlError;

	///  @name File operations

	virtual
	void
	requestImmediateLoad ();

	///  @name X3D Creation Handling

	///
	SFNode <Scene>
	createX3DFromString (const std::string &)
	throw (Error <INVALID_X3D>,
	       Error <NOT_SUPPORTED>,
	       Error <INVALID_OPERATION_TIMING>,
	       Error <DISPOSED>);

	///
	SFNode <Scene>
	createX3DFromStream (std::istream &)
	throw (Error <INVALID_X3D>,
	       Error <NOT_SUPPORTED>,
	       Error <INVALID_OPERATION_TIMING>,
	       Error <DISPOSED>);

	///
	SFNode <Scene>
	createX3DFromURL (const MFString &)
	throw (Error <INVALID_URL>,
	       Error <URL_UNAVAILABLE>,
	       Error <INVALID_X3D>);

	///
	void
	createX3DFromURL (const MFString &, const SFNode <X3DBaseNode> &, const basic::id &)
	throw (Error <INVALID_URL>,
	       Error <URL_UNAVAILABLE>,
	       Error <INVALID_X3D>);

	///  @name Stream Handling

	///  Returns the contents of @a URL
	std::string
	loadDocument (const SFString &)
	throw (Error <INVALID_URL>,
	       Error <URL_UNAVAILABLE>);

	///  Returns the contents of @a uri
	std::string
	loadDocument (const basic::uri &)
	throw (Error <INVALID_URL>,
	       Error <URL_UNAVAILABLE>);

	///  Returns a stream for @a URL
	basic::ifilestream
	loadStream (const SFString &)
	throw (Error <INVALID_URL>,
	       Error <URL_UNAVAILABLE>);

	///  Returns a stream for @a URL
	basic::ifilestream
	loadStream (const basic::uri &)
	throw (Error <INVALID_URL>,
	       Error <URL_UNAVAILABLE>);

	/// URN Handling

	static
	void
	addURN (const basic::uri &, const basic::uri &);

	static
	void
	removeURN (const basic::uri &);

	static
	const basic::uri &
	getURL (const basic::uri & uri);

	static
	const URNIndex &
	getURNs ();

	MFString
	transformURI (const MFString &);

	basic::uri
	transformURI (const basic::uri &);


protected:

	X3DUrlObject ();

	virtual
	void
	initialize ();

	///  @name Element Access

	virtual
	void
	setWorldURL (const basic::uri &);

	virtual
	const basic::uri &
	getWorldURL ();


private:

	static URNIndex URNCache;

	std::string userAgent;
	basic::uri  worldURL;

};

} // X3D
} // titania

#endif
