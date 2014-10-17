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

#include "Filter.h"

#include <pcrecpp.h>

namespace titania {
namespace X3D {

std::string
get_name_from_uri (const basic::uri & uri)
{
	return get_name_from_string (uri .basename (false));
}

std::string
get_name_from_string (const std::string & name_)
{
	static const pcrecpp::RE Spaces ("\\s+");

	auto name = name_;

	Spaces .GlobalReplace ("_", &name);

	filter_non_id_characters (name);

	if (not name .empty ())
		return name;

	return "Unnamed";
}

void
filter_non_id_characters (std::string & string)
{
	static const pcrecpp::RE NonIdFirstChar (R"/(^[\x30-\x39\x00-\x20\x22\x23\x27\x2b\x2c\x2d\x2e\x5b\x5c\x5d\x7b\x7d\x7f])/");
	static const pcrecpp::RE NonIdChars (R"/([\x00-\x20\x22\x23\x27\x2c\x2e\x5b\x5c\x5d\x7b\x7d\x7f])/");

	NonIdChars .GlobalReplace ("", &string);

	while (NonIdFirstChar .Replace ("", &string))
		;
}

void
filter_control_characters (std::string & string)
{
	static const pcrecpp::RE ControlCharacters (R"/([\x00-\x08\x0B\x0C\x0E-\x1F])/", pcrecpp::RE_Options () .set_multiline (true));

	ControlCharacters .GlobalReplace ("", &string);
}

void
filter_bad_utf8_characters (std::string & string)
{
	static const pcrecpp::RE UTF8Characters (R"/(([\000-\177])/"
	                                         R"/(|[\300-\337][\200-\277])/"
	                                         R"/(|[\340-\357][\200-\277]{2})/"
	                                         R"/(|[\360-\367][\200-\277]{3})/"
	                                         R"/(|[\370-\373][\200-\277]{4})/"
	                                         R"/(|[\374-\375][\200-\277]{5})/"
	                                         R"/()|.)/",
	                                         pcrecpp::RE_Options () .set_multiline (true));

	UTF8Characters .GlobalReplace ("\\1", &string);
}

std::string
escape_cdata (std::string string)
{
	static const pcrecpp::RE cdata_end_pattern (R"/((\]\]\>))/");
	static const std::string cdata_end_subs (R"/(\\]\\]\\>)/");

	cdata_end_pattern .GlobalReplace (cdata_end_subs, &string);

	return string;
}

} // X3D
} // titania
