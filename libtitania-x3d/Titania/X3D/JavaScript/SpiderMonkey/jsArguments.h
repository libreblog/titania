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

#ifndef __TITANIA_X3D_JAVA_SCRIPT_SPIDER_MONKEY_JS_ARGUMENTS_H__
#define __TITANIA_X3D_JAVA_SCRIPT_SPIDER_MONKEY_JS_ARGUMENTS_H__

#include "jsContext.h"
#include "jsString.h"

#include <jsapi.h>
#include <string>
#include <type_traits>
#include <stdexcept>

namespace titania {
namespace X3D {
namespace MozillaSpiderMonkey {

template <class Type>
inline
bool
instanceOf (JSContext* const cx, JSObject* const obj)
{
	const auto context     = getContext (cx);
	const auto proto       = context -> getProto (Type::getId ());
	const auto constructor = JS_GetConstructor (cx, proto);
	jsval      value       = OBJECT_TO_JSVAL (obj);
	JSBool     boolean     = false;

	if (JS_HasInstance (cx, constructor, value, &boolean))
		return boolean;

	return false;
}

template <class Type>
inline
Type
getObject (JSObject* const obj)
{
	return static_cast <Type> (JS_GetPrivate (obj));
}

// Version for functions.
template <class Type>
inline
typename Type::internal_type*
getThis (JSContext* const cx, const JS::CallArgs & args)
throw (std::invalid_argument)
{
	if (args .thisv () .isObjectOrNull ())
	{
		const auto self = args .thisv () .toObjectOrNull ();

		if (self and instanceOf <Type> (cx, self))
		{
			const auto object = getObject <typename Type::internal_type*> (self);

			if (object)
				return object;
		}
	}

	throw std::invalid_argument ("function must be called with object of type '" + std::string (Type::getClass () -> name) + "'");
}

// Version for properties.
template <class Type>
inline
typename Type::internal_type*
getThis (JSContext* const cx, JSObject* const obj)
throw (std::invalid_argument)
{
	const auto object = getObject <typename Type::internal_type*> (obj);

	if (object)
		return object;

	throw std::invalid_argument ("function must be called with object of type '" + std::string (Type::getClass () -> name) + "'");
}

template <class Type>
typename std::enable_if <
   not (std::is_integral <Type>::value or
        std::is_floating_point <Type>::value or
        std::is_same <Type, std::string>::value or
        std::is_same <Type, X3D::String>::value),
   typename Type::internal_type*
>::type
getArgument (JSContext* const cx, const JS::Value & value, const size_t index)
throw (std::invalid_argument,
       std::domain_error)
{
	if (value .isObjectOrNull ())
	{
		const auto obj = value .toObjectOrNull ();

		if (not obj)
			throw std::domain_error ("type of argument " + std::to_string (index + 1) + " is invalid, must be '" + std::string (Type::getClass () -> name) + "' but is null");

		if (instanceOf <Type> (cx, obj))
			return getObject <typename Type::internal_type*> (obj);
	}

	throw std::invalid_argument ("type of argument " + std::to_string (index + 1) + " is invalid, must be '" + std::string (Type::getClass () -> name) + "'");
}

template <class Type>
typename std::enable_if <
   not (std::is_integral <Type>::value or
        std::is_floating_point <Type>::value or
        std::is_same <Type, std::string>::value or
        std::is_same <Type, X3D::String>::value),
   typename Type::internal_type*
>::type
getArgument (JSContext* const cx, const JS::CallArgs & args, const size_t index)
throw (std::invalid_argument,
       std::domain_error)
{
	return getArgument <Type> (cx, args [index], index);
}

template <class Type>
typename std::enable_if <
   std::is_integral <Type>::value or
   std::is_floating_point <Type>::value or
   std::is_same <Type, std::string>::value or
   std::is_same <Type, X3D::String>::value,
   Type
>::type
getArgument (JSContext* const cx, const JS::Value & value, const size_t index)
throw (std::invalid_argument,
       std::domain_error)
{
	throw std::invalid_argument ("getArgument");
}

template <>
inline
bool
getArgument <bool> (JSContext* const cx, const JS::Value & value, const size_t index)
throw (std::invalid_argument,
       std::domain_error)
{
	JSBool boolean = false;

	if (JS_ValueToBoolean (cx, value, &boolean))
		return boolean;

	throw std::invalid_argument ("type of argument " + std::to_string (index + 1) + " is invalid, must be a 'Boolean'");
}

template <>
inline
double
getArgument <double> (JSContext* const cx, const JS::Value & value, const size_t index)
throw (std::invalid_argument,
       std::domain_error)
{
	double number = 0;

	if (JS::ToNumber (cx, value, &number))
		return number;

	throw std::invalid_argument ("type of argument " + std::to_string (index + 1) + " is invalid, must be a 'Number'");
}

template <>
inline
float
getArgument <float> (JSContext* const cx, const JS::Value & value, const size_t index)
throw (std::invalid_argument,
       std::domain_error)
{
	double number = 0;

	if (JS::ToNumber (cx, value, &number))
		return number;

	throw std::invalid_argument ("type of argument " + std::to_string (index + 1) + " is invalid, must be a 'Number'");
}

template <>
inline
int32_t
getArgument <int32_t> (JSContext* const cx, const JS::Value & value, const size_t index)
throw (std::invalid_argument,
       std::domain_error)
{
	int32_t number = 0;

	if (JS::ToInt32 (cx, value, &number))
		return number;

	throw std::invalid_argument ("type of argument " + std::to_string (index + 1) + " is invalid, must be a 'Integer'");
}

template <>
inline
uint32_t
getArgument <uint32_t> (JSContext* const cx, const JS::Value & value, const size_t index)
throw (std::invalid_argument,
       std::domain_error)
{
	uint32_t number = 0;

	if (JS::ToUint32 (cx, value, &number))
		return number;

	throw std::invalid_argument ("type of argument " + std::to_string (index + 1) + " is invalid, must be a 'Integer'");
}

template <>
inline
std::string
getArgument <std::string> (JSContext* const cx, const JS::Value & value, const size_t index)
throw (std::invalid_argument,
       std::domain_error)
{
	return to_string (cx, value);
}

template <>
inline
X3D::String
getArgument <X3D::String> (JSContext* const cx, const JS::Value & value, const size_t index)
throw (std::invalid_argument,
       std::domain_error)
{
	return to_string (cx, value);
}

template <class Type>
typename std::enable_if <
   std::is_integral <Type>::value or
   std::is_floating_point <Type>::value or
   std::is_same <Type, std::string>::value or
   std::is_same <Type, X3D::String>::value,
   Type
>::type
getArgument (JSContext* const cx, const JS::CallArgs & args, const size_t index)
throw (std::invalid_argument,
       std::domain_error)
{
	return getArgument <Type> (cx, args [index], index);
}


} // MozillaSpiderMonkey
} // X3D
} // titania

#endif
