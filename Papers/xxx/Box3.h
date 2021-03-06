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

#ifndef __TITANIA_MATH_GEOMETRY_BOX3_H__
#define __TITANIA_MATH_GEOMETRY_BOX3_H__

#include "../Geometry/Line3.h"
#include "../Geometry/Plane3.h"
#include "../Geometry/Sphere3.h"
#include "../Numbers/Matrix4.h"
#include "../Numbers/Vector3.h"
#include <array>

namespace titania {
namespace math {

template <class Type>
class box3
{
public:

	///  Value typedef.
	typedef Type value_type;

	///  @name Constructors

	///  Default constructor. Constructs a box of size 0 0 0 and center 0 0 0.
	constexpr
	box3 () :
		points ()
	{ }

	///  Copy constructor.
	template <class Up>
	constexpr
	box3 (const box3 <Up> & box) :
		points (box .points)
	{ }

	///  Constructs a box of size @a size and center @a size.
	box3 (const vector3 <Type> & size, const vector3 <Type> & center) :
      box3 (center - size * Type (0.5), center + size * Type (0.5), true)
	{ }

	///  Constructs a box of min @a min and max @a max.
	box3 (const vector3 <Type> & min, const vector3 <Type> & max, bool)
	{
		points [0] = vector3 <Type> (max .x (), min .y (), max .z ());
		points [1] = vector3 <Type> (max .x (), max .y (), max .z ());
		points [2] = vector3 <Type> (min .x (), max. y (), max .z ());
		points [3] = vector3 <Type> (min. x (), min .y (), max .z ());

		points [4] = vector3 <Type> (max .x (), min .y (), min .z ());
		points [5] = vector3 <Type> (max .x (), max .y (), min .z ());
		points [6] = vector3 <Type> (min .x (), max. y (), min .z ());
		points [7] = vector3 <Type> (min. x (), min .y (), min .z ());
	}

	///  @name Assignment operator

	///  Assign @a box3 to this box3.
	template <class Up>
	box3 &
	operator = (const box3 <Up> & box)
	{
		points = box .points;
		return *this;
	}

	///  @name Element access

	///  Return the minimum vector of this box.
	vector3 <Type>
	min () const
	{
		vector3 <Type> min = math::min (points [0], points [1]);
		min = math::min (min, points [2]);
		min = math::min (min, points [3]);
		min = math::min (min, points [4]);
		min = math::min (min, points [5]);
		min = math::min (min, points [6]);
		min = math::min (min, points [7]);

		return min;
	}

	///  Return the maximum vector of this box.
	vector3 <Type>
	max () const
	{
		vector3 <Type> max = math::max (points [0], points [1]);
		max = math::max (max, points [2]);
		max = math::max (max, points [3]);
		max = math::max (max, points [4]);
		max = math::max (max, points [5]);
		max = math::max (max, points [6]);
		max = math::max (max, points [7]);

		return max;
	}

	///  Return the size of this box.
	vector3 <Type>
	size () const
	{ return max () - min (); }

	///  Return the center of this box.
	const vector3 <Type>
	center () const
	{
		auto min = this -> min ();
		auto max = this -> max ();

		return (min + max) * Type (0.5);
	}

	///  @name  Arithmetic operations
	///  All these operators modify this box3 inplace.

	///  Add @a box3 to this box.
	template <class Up>
	box3 &
	operator += (const box3 <Up> & box)
	{
		vector3 <Type> lhs_min = this -> min ();
		vector3 <Type> lhs_max = this -> max ();

		vector3 <Type> rhs_min = box .min ();
		vector3 <Type> rhs_max = box .max ();

		if (lhs_min not_eq lhs_max)
		{
			if (rhs_min not_eq rhs_max)
				return *this = box3 (math::min (lhs_min, rhs_min), math::max (lhs_max, rhs_max), true);

			return *this;
		}

		return *this = box;
	}

	///  Translate this box by @a translation.
	template <class Up>
	box3 &
	operator += (const vector3 <Up> & translation)
	{
		for (auto & point : points)
			point += translation;
		return *this;
	}

	///  Translate this box by @a translation.
	template <class Up>
	box3 &
	operator -= (const vector3 <Up> & translation)
	{
		for (auto & point : points)
			point += translation;
		return *this;
	}

	///  Scale this box3 by @a scale.
	box3 &
	operator *= (const Type & scale)
	{
		return *this = box3 (size () * scale, center ());
	}

	///  Scale this box3 by @a scale.
	box3 &
	operator /= (const Type & scale)
	{
		return *this = box3 (size () / scale, center ());
	}

	///  Transform this box by @a matrix.
	box3 &
	operator *= (const matrix4 <Type> & matrix)
	{
		return multBoxMatrix (matrix);
	}

	///  Transform this box by @a matrix.
	box3 &
	multMatrixBox (const matrix4 <Type> & matrix)
	{
		for (auto & point : points)
			point = matrix .multMatrixVec (point);
		return *this;
	}

	///  Transform this box by @a matrix.
	box3 &
	multBoxMatrix (const matrix4 <Type> & matrix)
	{
		for (auto & point : points)
			point = matrix .multVecMatrix (point);
		return *this;
	}

	///  @name Intersection

	///  Returns true if @a point is inside this box3 min and max extend.
	bool
	intersect (const vector3 <Type> &) const;

	///  Returns true if @a line intersects with this box3.
	bool
	intersect (const line3 <Type> &, vector3 <Type> &) const;

	///  Returns true if @a sphere intersects with this box3.
	bool
	intersect (const sphere3 <Type> &) const;


private:

	std::array <vector3 <Type>, 8> points;

};

template <class Type>
bool
box3 <Type>::intersect (const vector3 <Type> & point) const
{
	vector3 <Type> min = this -> min ();
	vector3 <Type> max = this -> max ();

	return min .x () <= point .x () and
	       max .x () >= point .x () and
	       min .y () <= point .y () and
	       max .y () >= point .y () and
	       min .z () <= point .z () and
	       max .z () >= point .z ();
}

template <class Type>
bool
box3 <Type>::intersect (const line3 <Type> & line, vector3 <Type> & intersection) const
{
	vector3 <Type> min    = this -> min ();
	vector3 <Type> max    = this -> max ();
	vector3 <Type> center = (min + max) * Type (0.5);

	vector3 <Type> points [6] = {
		vector3 <Type> (center .x (), center .y (), max .z ()), // right
		vector3 <Type> (center .x (), center .y (), min .z ()), // left

		vector3 <Type> (center .x (), max .y (), center .z ()), // top
		vector3 <Type> (center .x (), min .y (), center .z ()), // bottom

		vector3 <Type> (max .x (), center .y (), center .z ()), // front
		vector3 <Type> (min .x (), center .y (), center .z ())  // back
	};

	static const vector3 <Type> normals [6] = {
		vector3 <Type> (0,  0,  1),
		vector3 <Type> (0,  0, -1),
		vector3 <Type> (0,  1,  0),
		vector3 <Type> (0, -1,  0),
		vector3 <Type> (1,  0,  0),
		vector3 <Type> (-1,  0,  0)
	};

	for (size_t i = 0; i < 6; ++ i)
	{
		if (plane3 <Type> (points [i], normals [i]) .intersect (line, intersection))
		{
			switch (i)
			{
				case 0:
				case 1:

					if (intersection .x () >= min .x () and intersection .x () <= max .x () and
					    intersection .y () >= min .y () and intersection .y () <= max .y ())
						return true;

					break;
				case 2:
				case 3:

					if (intersection .x () >= min .x () and intersection .x () <= max .x () and
					    intersection .z () >= min .z () and intersection .z () <= max .z ())
						return true;

					break;
				case 4:
				case 5:

					if (intersection .y () >= min .y () and intersection .y () <= max .y () and
					    intersection .z () >= min .z () and intersection .z () <= max .z ())
						return true;

					break;
			}
		}
	}

	return false;
}

template <class Type>
bool
box3 <Type>::intersect (const sphere3 <Type> & sphere) const
{
	vector3 <Type> min = this -> min ();
	vector3 <Type> max = this -> max ();

	vector3 <Type> center = sphere .center ();
	Type           radius = sphere .radius ();

	return min .x () <= center .x () + radius and
	       max .x () >= center .x () - radius and
	       min .y () <= center .y () + radius and
	       max .y () >= center .y () - radius and
	       min .z () <= center .z () + radius and
	       max .z () >= center .z () - radius;
}

///  @relates box3
///  @name Comparision operations

///  Compares two box3 numbers.
///  Return true if @a lhs is equal to @a rhs.
template <class Type>
constexpr bool
operator == (const box3 <Type> & lhs, const box3 <Type> & rhs)
{
	return
	   lhs .min () == rhs .min () and
	   lhs .max () == rhs .max ();
}

///  Compares two box3 numbers.
///  Return true if @a lhs is not equal to @a rhs.
template <class Type>
constexpr bool
operator not_eq (const box3 <Type> & lhs, const box3 <Type> & rhs)
{
	return
	   lhs .min () not_eq rhs .min () or
	   lhs .max () not_eq rhs .max ();
}

///  @relates box3
///  @name Arithmetic operations

///  Return new box3 value @a lhs plus @a rhs.
template <class Type>
inline
box3 <Type>
operator + (const box3 <Type> & lhs, const box3 <Type> & rhs)
{
	return box3 <Type> (lhs) += rhs;
}

///  Return new box value @a lhs translated @a rhs.
template <class Type>
inline
box3 <Type>
operator + (const box3 <Type> & lhs, const vector3 <Type> & rhs)
{
	return box3 <Type> (lhs) += rhs;
}

///  Return new box3 value @a rhs translated @a lhs.
template <class Type>
inline
box3 <Type>
operator + (const vector3 <Type> & lhs, const box3 <Type> & rhs)
{
	return box3 <Type> (rhs) += lhs;
}

///  Return new box3 value @a rhs translated @a lhs.
template <class Type>
inline
box3 <Type>
operator - (const vector3 <Type> & lhs, const box3 <Type> & rhs)
{
	return box3 <Type> (rhs) -= lhs;
}

///  Return new box3 value @a rhs transformed by matrix @a lhs.
template <class Type>
inline
box3 <Type>
operator * (const box3 <Type> & lhs, const Type & rhs)
{
	return box3 <Type> (lhs) *= rhs;
}

///  Return new box3 value @a rhs scaled @a lhs.
template <class Type>
inline
box3 <Type>
operator * (const Type & lhs, const box3 <Type> & rhs)
{
	return box3 <Type> (rhs) *= lhs;
}

///  Return new box3 value @a rhs scaled @a lhs.
template <class Type>
inline
box3 <Type>
operator / (const Type & lhs, const box3 <Type> & rhs)
{
	return box3 <Type> (rhs) /= lhs;
}

///  Return new box3 value @a rhs transformed by matrix @a lhs.
template <class Type>
inline
box3 <Type>
operator * (const box3 <Type> & lhs, const matrix4 <Type> & rhs)
{
	return box3 <Type> (lhs) .multBoxMatrix (rhs);
}

///  Return new box3 value @a rhs transformed by matrix @a lhs.
template <class Type>
inline
box3 <Type>
operator * (const matrix4 <Type> & lhs, const box3 <Type> & rhs)
{
	return box3 <Type> (rhs) .multMatrixBox (lhs);
}

///  @relates box3
///  @name Input/Output operations

///  Extraction operator for box3 values.
template <class CharT, class Traits, class Type>
std::basic_istream <CharT, Traits> &
operator >> (std::basic_istream <CharT, Traits> & istream, box3 <Type> & box)
{
	vector3 <Type> size;
	vector3 <Type> center;

	istream >> size >> center;

	if (istream)
		box = box3 <Type> (size, center);

	return istream;
}

///  Insertion operator for box3 values.
template <class CharT, class Traits, class Type>
std::basic_ostream <CharT, Traits> &
operator << (std::basic_ostream <CharT, Traits> & ostream, const box3 <Type> & box)
{
	return ostream << box .size () << ", " << box .center ();
}

extern template class box3 <float>;
extern template class box3 <double>;
extern template class box3 <long double>;

//
extern template std::istream & operator >> (std::istream &, box3 <float> &);
extern template std::istream & operator >> (std::istream &, box3 <double> &);
extern template std::istream & operator >> (std::istream &, box3 <long double> &);

//
extern template std::ostream & operator << (std::ostream &, const box3 <float> &);
extern template std::ostream & operator << (std::ostream &, const box3 <double> &);
extern template std::ostream & operator << (std::ostream &, const box3 <long double> &);

} // math
} // titania

#endif
