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

#ifndef __TITANIA_X3D_JAVA_SCRIPT_SPIDER_MONKEY_JS_CONTEXT_H__
#define __TITANIA_X3D_JAVA_SCRIPT_SPIDER_MONKEY_JS_CONTEXT_H__

#include "../../Components/Networking/X3DUrlObject.h"
#include "../../Components/Scripting/Script.h"
#include "../X3DJavaScriptContext.h"
#include "jsMemory.h"
#include "jsObjectType.h"

#include <jsapi.h>

#include <map>
#include <mutex>

namespace titania {
namespace X3D {

class SceneLoader;

namespace MozillaSpiderMonkey {

class jsContext :
	public X3DJavaScriptContext
{
public:

	///  @name Construction

	jsContext (Script* const, const std::string &, const basic::uri &);

	virtual
	X3DBaseNode*
	create (X3DExecutionContext* const) const final override;

	virtual
	void
	setExecutionContext (X3DExecutionContext* const)
	throw (Error <INVALID_OPERATION_TIMING>,
	       Error <DISPOSED>) final override;

	///  @name Common members

	virtual
	ComponentType
	getComponent () const
	throw (Error <DISPOSED>) final override
	{ return component; }

	virtual
	const std::string &
	getTypeName () const
	throw (Error <DISPOSED>) final override
	{ return typeName; }

	virtual
	const std::string &
	getContainerField () const
	throw (Error <DISPOSED>) final override
	{ return containerField; }

	///  @name Member access

	std::string
	getWorldUrl () const;

	JSObject*
	getGlobal () const
	{ return global; }

	JSObject*
	getProto (const ObjectType type) const
	{ return protos [size_t (type)]; }

	void
	addObject (X3DFieldDefinition* const, JSObject* const)
	throw (std::invalid_argument);

	void
	removeObject (X3DFieldDefinition* const);

	JSObject*
	getObject (X3DFieldDefinition* const field) const
	throw (std::out_of_range)
	{ return objects .at (field); }

	std::unique_ptr <SceneLoader> &
	getFuture ()
	{ return future; }

	///  @name Functions

	JSBool
	require (const basic::uri &, JS::Value &);

	///  @name Destruction

	virtual
	void
	dispose () final override;

	virtual
	~jsContext ();


private:

	void
	addClasses ()
	throw (std::runtime_error);

	void
	addProto (const ObjectType type, JSObject* const proto)
	{ protos [size_t (type)] = proto; }

	void
	addUserDefinedFields ();

	void
	defineProperty (JSObject* const, X3DFieldDefinition* const, const std::string &, const uint32_t);

	bool
	evaluate (const std::string &, const std::string &);

	bool
	evaluate (const std::string &, const std::string &, JS::Value &);

	static
	JSBool
	setProperty (JSContext*, JS::HandleObject, JS::HandleId, JSBool, JS::MutableHandleValue);

	static
	JSBool
	getBuildInProperty (JSContext*, JS::HandleObject, JS::HandleId, JS::MutableHandleValue);

	static
	JSBool
	getProperty (JSContext*, JS::HandleObject, JS::HandleId, JS::MutableHandleValue);

	virtual
	void
	initialize () final override;

	void
	set_live ();

	void
	prepareEvents (const JS::Value &);

	void
	set_field (X3DFieldDefinition* const, const JS::Value &);

	void
	eventsProcessed (const JS::Value &);

	void
	set_shutdown ();

	void
	finish ();

	JS::Value
	getFunction (const std::string &) const
	throw (std::invalid_argument);

	void
	call (const std::string &);

	void
	call (const JS::Value &);

	static
	void
	error (JSContext*, const char*, JSErrorReport*);

	///  @name Static members

	static const ComponentType component;
	static const std::string   typeName;
	static const std::string   containerField;

	static JSClass globalClass;

	///  @name Members

	std::unique_ptr <JSRuntime, RuntimeDeleter> rt;
	std::unique_ptr <JSContext, ContextDeleter> cx;
	JSObject*                                   global;
	std::vector <basic::uri>                    worldURL;

	std::vector <JSObject*>                   protos;
	std::map <std::string, RootedValue>       fields;
	std::map <X3DFieldDefinition*, JSObject*> objects;
	std::map <basic::uri, RootedValue>        files;

	std::unique_ptr <SceneLoader> future;

	size_t frame;

};

inline
jsContext*
getContext (JSRuntime* const rt)
{
	return static_cast <jsContext*> (JS_GetRuntimePrivate (rt));
}

inline
jsContext*
getContext (JSContext* const cx)
{
	return static_cast <jsContext*> (JS_GetContextPrivate (cx));
}

} // MozillaSpiderMonkey
} // X3D
} // titania

#endif
