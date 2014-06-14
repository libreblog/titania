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

#ifndef __TITANIA_X3D_PEASE_BLOSSOM_PARSER_PARSER_H__
#define __TITANIA_X3D_PEASE_BLOSSOM_PARSER_PARSER_H__

#include "../Parser/AssignmentOperatorType.h"
#include "../Primitives.h"

#include <iostream>
#include <stack>
#include <tuple>

namespace titania {
namespace pb {

class jsExecutionContext;

class Parser
{
public:

	///  @namjsExecutionContexte Construction

	Parser (jsExecutionContext* const, std::istream & istream);

	///  @name Operations

	void
	parseIntoScope ()
	throw (SyntaxError);


private:

	using State = std::tuple <std::ios_base::iostate, std::istream::pos_type>;

	// Operations

	void
	pushExecutionContext (jsExecutionContext* const executionContext)
	{ executionContexts .emplace (executionContext); }

	void
	popExecutionContext ()
	{ executionContexts .pop (); }

	jsExecutionContext*
	getExecutionContext () const
	{ return executionContexts .top (); }

	void
	setState (const State & value);

	State
	getState ();

	// A.1 Lexical Grammar

	void
	comments ();

	bool
	identifier (std::string &);

	bool
	identifierName (std::string &);

	bool
	identifierStart (std::string &);

	bool
	identifierPart (std::string &);

	bool
	reservedWord (const std::string &);

	bool
	literal (var &);

	bool
	nullLiteral (var &);

	bool
	booleanLiteral (var &);

	bool
	numericLiteral (var &);

	bool
	decimalLiteral (var &);

	bool
	binaryIntegerLiteral (var &);

	bool
	octalIntegerLiteral (var &);

	bool
	hexIntegerLiteral (var &);

	bool
	stringLiteral (var &);

	// A.2 Number Conversions

	// A.3 Expressions

	bool
	primaryExpression (var &);

	bool
	memberExpression (var &);

	bool
	newExpression (var &);

	bool
	leftHandSideExpression (var &);

	bool
	postfixExpression (var &);

	bool
	unaryExpression (var &);

	bool
	multiplicativeExpression (var &);

	bool
	additiveExpression (var &);

	bool
	shiftExpression (var &);

	bool
	relationalExpression (var &);

	bool
	equalityExpression (var &);

	bool
	bitwiseANDExpression (var &);

	bool
	bitwiseXORExpression (var &);

	bool
	bitwiseORExpression (var &);

	bool
	logicalANDExpression (var &);

	bool
	logicalORExpression (var &);

	bool
	conditionalExpression (var &);

	bool
	assignmentExpression (var &);

	bool
	assignmentOperator (AssignmentOperatorType &);

	bool
	expression (var &);

	// A.4 Statements

	bool
	statement ();

	bool
	variableStatement ();

	bool
	variableDeclarationList ();

	bool
	variableDeclaration ();

	bool
	initialiser (var &);

	bool
	expressionStatement ();

	bool
	emptyStatement ();

	// A.5 Functions and Programs

	bool
	functionDeclaration ();

	bool
	functionExpression (var &);

	bool
	formalParameterList (std::vector <std::string> &);

	void
	functionBody ();

	void
	program ();

	void
	sourceElements ();

	bool
	sourceElement ();

	///  @name Members

	jsExecutionContext* const        rootContext;
	std::stack <jsExecutionContext*> executionContexts;
	std::istream &                   istream;
	std::string                      whiteSpaces;

};

} // pb
} // titania

#endif
