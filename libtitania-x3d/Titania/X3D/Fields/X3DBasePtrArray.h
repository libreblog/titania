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

#ifndef __TITANIA_X3D_FIELDS_X3DBASE_PTR_ARRAY_H__
#define __TITANIA_X3D_FIELDS_X3DBASE_PTR_ARRAY_H__

#include "../Basic/X3DArrayField.h"
#include "../InputOutput/Generator.h"

#include <Titania/Utility/Range.h>

namespace titania {
namespace X3D {

template <class ValueType>
class X3DBasePtrArray :
	public X3DArrayField <ValueType>
{
public:

	///  @name Member types

	using value_type = ValueType;

	using X3DArrayField <ValueType>::operator =;
	using X3DArrayField <ValueType>::getValue;
	using X3DArrayField <ValueType>::assign;
	using X3DArrayField <ValueType>::front;
	using X3DArrayField <ValueType>::back;
	using X3DArrayField <ValueType>::cbegin;
	using X3DArrayField <ValueType>::cend;
	using X3DArrayField <ValueType>::empty;
	using X3DArrayField <ValueType>::size;
	using X3DArrayField <ValueType>::addInterest;

	///  @name Construction

	///  Constructs new X3DBasePtrArray.
	X3DBasePtrArray () :
		X3DArrayField <ValueType> (),
		               cloneCount (0)
	{ }

	///  Constructs new X3DBasePtrArray.
	X3DBasePtrArray (const X3DBasePtrArray & other) :
		X3DArrayField <ValueType> (other),
		               cloneCount (0)
	{ }

	///  Constructs new X3DBasePtrArray.
	X3DBasePtrArray (X3DBasePtrArray && other) :
		X3DArrayField <ValueType> (std::move (other)),
		               cloneCount (0)
	{ }

	///  Constructs new X3DBasePtrArray.
	template <class Up, std::enable_if_t <std::is_base_of <typename ValueType::value_type, typename Up::value_type>::value, bool> = false>
	X3DBasePtrArray (const X3DBasePtrArray <Up> & other) :
		X3DArrayField <ValueType> (other .begin (), other .end ()),
		               cloneCount (0)
	{ }

	///  Constructs new X3DBasePtrArray.
	template <class Up, std::enable_if_t <not std::is_base_of <typename ValueType::value_type, typename Up::value_type>::value, bool> = true>
	explicit
	X3DBasePtrArray (const X3DBasePtrArray <Up> & other) :
		X3DArrayField <ValueType> (other .begin (), other .end ()),
		               cloneCount (0)
	{ }

	///  Constructs new X3DBasePtrArray.
	template <class Up, std::enable_if_t <std::is_base_of <typename ValueType::value_type, typename Up::value_type>::value, bool> = false>
	X3DBasePtrArray (X3DBasePtrArray <Up> && other) :
		X3DArrayField <ValueType> (),
		               cloneCount (0)
	{ *this = std::move (other); }

	///  Constructs new X3DBasePtrArray.
	template <class Up, std::enable_if_t <not std::is_base_of <typename ValueType::value_type, typename Up::value_type>::value, bool> = true>
	explicit
	X3DBasePtrArray (X3DBasePtrArray <Up> && other) :
		X3DArrayField <ValueType> (),
		               cloneCount (0)
	{ *this = std::move (other); }

	//	///  Constructs new X3DBasePtrArray.
	//	X3DBasePtrArray (std::initializer_list <ValueType> initializer_list) :
	//		X3DArrayField <ValueType> (initializer_list),
	 //		              cloneCount (0)
	                           //	{ }

	///  Constructs new X3DBasePtrArray.
	X3DBasePtrArray (std::initializer_list <const typename ValueType::internal_type> initializer_list) :
		X3DArrayField <ValueType> (initializer_list),
		               cloneCount (0)
	{ }

	///  Constructs new X3DBasePtrArray.
	template <class InputIterator>
	X3DBasePtrArray (InputIterator first, InputIterator last) :
		X3DArrayField <ValueType> (first, last),
		               cloneCount (0)
	{ }

	///  Constructs new X3DBasePtrArray.
	virtual
	X3DBasePtrArray*
	create () const final override
	{ return new X3DBasePtrArray (); }

	///  Constructs new X3DBasePtrArray.
	virtual
	X3DBasePtrArray*
	copy (const CopyType) const
	throw (Error <INVALID_NAME>,
	       Error <NOT_SUPPORTED>) final override;

	///  Constructs new X3DBasePtrArray.
	virtual
	X3DBasePtrArray*
	copy (X3DExecutionContext* const executionContext, const CopyType) const
	throw (Error <INVALID_NAME>,
	       Error <NOT_SUPPORTED>) final override;

	///  Constructs new X3DBasePtrArray.
	virtual
	void
	copy (X3DExecutionContext* const, X3DFieldDefinition* const, const CopyType) const
	throw (Error <INVALID_NAME>,
	       Error <NOT_SUPPORTED>) final override;

	///  @name Assignment operators

	///  Assigns the X3DBasePtrArray and propagates an event.
	X3DBasePtrArray &
	operator = (const X3DBasePtrArray & field)
	{
		X3DArrayField <ValueType>::operator = (field);
		return *this;
	}

	///  Assigns the X3DBasePtrArray and propagates an event.
	X3DBasePtrArray &
	operator = (X3DBasePtrArray && field)
	{
		X3DArrayField <ValueType>::operator = (std::move (field));
		return *this;
	}

	///  Assigns the X3DBasePtrArray and propagates an event.
	template <class Up>
	X3DBasePtrArray &
	operator = (const X3DBasePtrArray <Up> & other)
	{
		assign (other .begin (), other .end ());
		return *this;
	}

	///  Assigns the X3DBasePtrArray and propagates an event.
	template <class Up>
	X3DBasePtrArray &
	operator = (X3DBasePtrArray <Up> &&);

	///  @name Common members

	///  Returns the type name of the object.
	virtual
	const std::string &
	getTypeName () const
	throw (Error <DISPOSED>) final override
	{ return typeName; }

	///  Returns the type of the object.
	virtual
	X3DConstants::FieldType
	getType () const final override
	{ return X3DConstants::MFNode; }

	///  @name Clone handling

	///  Increments the clone count of its children by @a count.
	virtual
	void
	addClones (const size_t count) final override
	{
		cloneCount += count;

		for (const auto & value : getValue ())
			value -> addClones (count);
	}

	///  Decrements the clone count of its children by @a count.
	virtual
	void
	removeClones (const size_t count) final override
	{
		cloneCount -= count;

		for (const auto & value : getValue ())
			value -> removeClones (count);
	}

	///  @name Interest service

	///  Adds an interest to this object.  The requester is then notified about a change of this object.
	template <class Class>
	void
	addInterest (void (Class::* memberFunction) (const X3DBasePtrArray &), Class* const object) const
	{ addInterest (memberFunction, object, std::cref (*this)); }

	///  Adds an interest to this object.  The requester is then notified about a change of this object.
	template <class Class>
	void
	addInterest (void (Class::* memberFunction) (const X3DBasePtrArray &), Class & object) const
	{ addInterest (memberFunction, object, std::cref (*this)); }

	///  @name Input/Output

	///  Not supported.
	virtual
	void
	fromStream (std::istream &)
	throw (Error <INVALID_X3D>,
	       Error <NOT_SUPPORTED>,
	       Error <INVALID_OPERATION_TIMING>,
	       Error <DISPOSED>) final override;

	///  Inserts this object into @a ostream in VRML Classic Encoding style.
	virtual
	void
	toStream (std::ostream &) const final override;

	///  Inserts this object into @a ostream in X3D XML Encoding style.
	virtual
	void
	toXMLStream (std::ostream &) const final override;

	///  Inserts this object into @a ostream in X3D JSON Encoding style.
	virtual
	void
	toJSONStream (std::ostream &) const final override;


private:

	using X3DArrayField <ValueType>::get;
	using X3DArrayField <ValueType>::addChild;
	using X3DArrayField <ValueType>::removeChildren;

	///  @name Element handling

	virtual
	void
	addChild (value_type* value) final override
	{
		X3DArrayField <ValueType>::addChild (value);

		value -> addClones (cloneCount);
	}

	virtual
	void
	moveChild (X3DArrayField <ValueType> & other, value_type* value) final override
	{
		X3DArrayField <ValueType>::moveChild (other, value);

		value -> addClones (cloneCount);
		value -> removeClones (other .getCloneCount ());
	}

	virtual
	void
	removeChild (value_type* value) final override
	{
		value -> removeClones (cloneCount);

		X3DArrayField <ValueType>::removeChild (value);
	}

	virtual
	size_t
	getCloneCount () const final override
	{ return cloneCount; }


private:

	///  @name Static members

	static const std::string typeName;

	///  @name Members

	size_t cloneCount;

};

template <class ValueType>
const std::string X3DBasePtrArray <ValueType>::typeName ("MFNode");

template <class ValueType>
X3DBasePtrArray <ValueType>*
X3DBasePtrArray <ValueType>::copy (const CopyType type) const
throw (Error <INVALID_NAME>,
       Error <NOT_SUPPORTED>)
{
	if (type == FLAT_COPY)
		return new X3DBasePtrArray (*this);

	const auto field = new X3DBasePtrArray ();

	for (const auto & value : *this)
	{
		if (value)
			field -> emplace_back (value -> copy (type));
		else
			field -> emplace_back ();
	}

	return field;
}

template <class ValueType>
X3DBasePtrArray <ValueType>*
X3DBasePtrArray <ValueType>::copy (X3DExecutionContext* const executionContext, const CopyType type) const
throw (Error <INVALID_NAME>,
       Error <NOT_SUPPORTED>)
{
	X3DBasePtrArray* const field = new X3DBasePtrArray ();

	copy (executionContext, field, type);

	return field;
}

template <class ValueType>
void
X3DBasePtrArray <ValueType>::copy (X3DExecutionContext* const executionContext, X3DFieldDefinition* const fieldDefinition, const CopyType type) const
throw (Error <INVALID_NAME>,
       Error <NOT_SUPPORTED>)
{
	X3DBasePtrArray* const field = static_cast <X3DBasePtrArray*> (fieldDefinition);

	if (type == FLAT_COPY)
	{
		*field = *this;
	}
	else
	{
		for (const auto & value : *this)
		{
			if (value)
				field -> emplace_back (value -> copy (executionContext, type));

			else
				field -> emplace_back ();
		}
	}
}

template <class ValueType>
template <class Up>
X3DBasePtrArray <ValueType> &
X3DBasePtrArray <ValueType>::operator = (X3DBasePtrArray <Up> && other)
{
	auto       first   = other .begin ();
	const auto last    = other .end ();
	auto       current = this -> begin ();

	for (const auto end = this -> end (); first not_eq last && current not_eq end; ++ current, ++ first)
		*current = std::move (*first);

	if (first == last)
	{
		// Remove trailing fields

		const size_t count = current - this -> begin ();

		removeChildren (get () .begin () + count, get () .end ());

		get () .resize (count);
	}
	else
	{
		// Insert at end

		for (; first not_eq last; ++ first)
		{
			const auto field = new ValueType (std::move (*first));

			get () .emplace_back (field);

			addChild (field);
		}

		this -> addEvent ();
	}

	other .clear ();

	return *this;
}

template <class ValueType>
void
X3DBasePtrArray <ValueType>::fromStream (std::istream & istream)
throw (Error <INVALID_X3D>,
       Error <NOT_SUPPORTED>,
       Error <INVALID_OPERATION_TIMING>,
       Error <DISPOSED>)
{ }

template <class ValueType>
void
X3DBasePtrArray <ValueType>::toStream (std::ostream & ostream) const
{
	switch (size ())
	{
		case 0:
		{
			ostream << Generator::EmptyBrackets;
			return;
		}
		case 1:
		{
			Generator::EnterScope (ostream);
			ostream << front ();
			Generator::LeaveScope (ostream);

			return;
		}
		default:
		{
			Generator::EnterScope (ostream);

			ostream
				<< '['
				<< Generator::TidyBreak
				<< Generator::IncIndent;

			for (const auto & field : std::make_pair (cbegin (), cend () - 1))
			{
				ostream
					<< Generator::Indent
					<< field
					<< Generator::Break;
			}

			ostream
				<< Generator::Indent
				<< back ()
				<< Generator::TidyBreak
				<< Generator::DecIndent
				<< Generator::Indent
				<< ']';

			Generator::LeaveScope (ostream);

			return;
		}
	}
}

template <class ValueType>
void
X3DBasePtrArray <ValueType>::toXMLStream (std::ostream & ostream) const
{
	if (not empty ())
	{
		Generator::EnterScope (ostream);

		for (const auto & value : std::make_pair (cbegin (), cend () - 1))
		{
			if (value)
			{
				ostream
					<< XMLEncode (value)
					<< Generator::TidyBreak;
			}
			else
			{
				ostream
					<< Generator::Indent
					<< "<!-- NULL -->"
					<< Generator::TidyBreak;
			}
		}

		if (back ())
			ostream << XMLEncode (back ());

		else
		{
			ostream
				<< Generator::Indent
				<< "<!-- NULL -->";
		}

		Generator::LeaveScope (ostream);
	}
}

template <class ValueType>
void
X3DBasePtrArray <ValueType>::toJSONStream (std::ostream & ostream) const
{
	if (empty ())
	{
		ostream
			<< '['
			<< Generator::TidySpace
			<< ']';
	}
	else
	{
		Generator::EnterScope (ostream);

		ostream
			<< '['
			<< Generator::TidyBreak
			<< Generator::IncIndent;

		for (const auto & value : std::make_pair (cbegin (), cend () - 1))
		{
			if (value)
			{
				ostream
					<< Generator::Indent
					<< JSONEncode (value)
					<< ','
					<< Generator::TidyBreak;
			}
			else
			{
				ostream
					<< Generator::Indent
					<< "null"
					<< ','
					<< Generator::TidyBreak;
			}
		}

		if (back ())
		{
			ostream
				<< Generator::Indent
				<< JSONEncode (back ());
		}
		else
		{
			ostream
				<< Generator::Indent
				<< "null";
		}

		ostream
			<< Generator::TidyBreak
			<< Generator::DecIndent
			<< Generator::Indent
			<< ']';

		Generator::LeaveScope (ostream);
	}
}

} // X3D
} // titania

#endif