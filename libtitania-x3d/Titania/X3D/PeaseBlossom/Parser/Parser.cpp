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

#include "Parser.h"

#include "../Expressions.h"
#include "../Parser/Grammar.h"

#include <Titania/Math/Utility/strtol.h>

namespace titania {
namespace pb {

Parser::Parser (jsScope* const scope, std::istream & istream) :
	       root (scope),
	     scopes ({ scope }),
	    istream (istream),
	whiteSpaces ()
{ }

void
Parser::parseIntoScope ()
throw (SyntaxError)
{
	//__LOG__ << std::endl;

	try
	{
		istream .imbue (std::locale::classic ());

		program ();
	}
	catch (const jsException & error)
	{
		throw;
	}
}

void
Parser::setState (const State & value)
{
	istream .clear (std::get <0> (value));
	istream .seekg (std::get <1> (value) - istream .tellg (), std::ios_base::cur);
}

Parser::State
Parser::getState ()
{
	return State (istream .rdstate (), istream .tellg ());
}

// A.1 Lexical Grammar

void
Parser::comments ()
{
	Grammar::WhiteSpaces (istream, whiteSpaces);
}

bool
Parser::identifier ()
{
	//__LOG__ << std::endl;

	comments ();

	const auto state = getState ();

	std::string identifierNameCharacters;

	if (identifierName (identifierNameCharacters))
	{
		if (not reservedWord (identifierNameCharacters))
		{
			return true;
		}

		setState (state);
	}

	return false;
}

bool
Parser::identifierName (std::string & identifierNameCharacters)
{
	//__LOG__ << std::endl;

	if (identifierStart (identifierNameCharacters))
	{
		while (identifierStart (identifierNameCharacters) or identifierPart (identifierNameCharacters))
			;

		return true;
	}

	return false;
}

bool
Parser::identifierStart (std::string & identifierStartCharacters)
{
	//__LOG__ << std::endl;

	// ...

	static const io::sequence IdentifierStart ("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ$_");

	return IdentifierStart (istream, identifierStartCharacters);
}

bool
Parser::identifierPart (std::string & identifierPartCharacters)
{
	//__LOG__ << std::endl;

	// ...

	static const io::sequence UnicodeDigit ("1234567890");

	bool result = false;

	while (identifierStart (identifierPartCharacters) or UnicodeDigit (istream, identifierPartCharacters))
		result = true;

	return result;
}

bool
Parser::reservedWord (const std::string & string)
{
	//__LOG__ << std::endl;

	if (Grammar::Keyword .count (string))
		return true;

	if (Grammar::FutureReservedWord .count (string))
		return true;

	static const std::set <std::string> primitives = {
		"null",
		"false",
		"true"
	};

	if (primitives .count (string))
		return true;

	return false;
}

bool
Parser::literal (var & value)
{
	if (nullLiteral (value))
		return true;

	if (booleanLiteral (value))
		return true;

	if (numericLiteral (value))
		return true;

	if (stringLiteral (value))
		return true;

	//if (regularExpressionLiteral ())
	//	return true;

	return false;
}

bool
Parser::nullLiteral (var & value)
{
	//__LOG__ << std::endl;

	comments ();

	if (Grammar::null (istream))
	{
		value = null ();
		return true;
	}

	return false;
}

bool
Parser::booleanLiteral (var & value)
{
	//__LOG__ << std::endl;

	comments ();

	if (Grammar::_true (istream))
	{
		value = True ();
		return true;
	}

	if (Grammar::_false (istream))
	{
		value = False ();
		return true;
	}

	return false;
}

bool
Parser::numericLiteral (var & value)
{
	//__LOG__ << std::endl;

	if (binaryIntegerLiteral (value))
		return true;

	if (octalIntegerLiteral (value))
		return true;

	if (hexIntegerLiteral (value))
		return true;

	if (decimalLiteral (value))
		return true;

	return false;
}

bool
Parser::decimalLiteral (var & value)
{
	//__LOG__ << std::endl;

	comments ();

	double number;

	if (istream >> std::dec >> number)
	{
		value .reset (new Number (number));
		return true;
	}

	istream .clear ();

	return false;
}

bool
Parser::binaryIntegerLiteral (var & value)
{
	//__LOG__ << std::endl;

	comments ();

	if (Grammar::bin (istream) or Grammar::BIN (istream))
	{
		std::string digits;

		if (Grammar::BinaryDigits (istream, digits))
		{
			value .reset (new Number (math::strtoul (digits .c_str (), 2)));
			return true;
		}

		istream .clear ();
	}

	return false;
}

bool
Parser::octalIntegerLiteral (var & value)
{
	//__LOG__ << std::endl;

	comments ();

	if (Grammar::oct (istream) or Grammar::OCT (istream))
	{
		uint32_t number;

		if (istream >> std::oct >> number)
		{
			value .reset (new Number (number));
			return true;
		}

		istream .clear ();
	}

	return false;
}

bool
Parser::hexIntegerLiteral (var & value)
{
	//__LOG__ << std::endl;

	comments ();

	if (Grammar::hex (istream) or Grammar::HEX (istream))
	{
		uint32_t number;

		if (istream >> std::hex >> number)
		{
			value .reset (new Number (number));
			return true;
		}

		istream .clear ();
	}

	return false;
}

bool
Parser::stringLiteral (var & value)
{
	static const io::quoted_string doubleQuotedString ('"');
	static const io::quoted_string singleQuotedString ('\'');

	comments ();

	std::string characters;

	if (doubleQuotedString (istream, characters))
	{
		value .reset (new String (characters));
		return true;
	}

	if (singleQuotedString (istream, characters))
	{
		value .reset (new String (characters));
		return true;
	}

	return false;
}

// A.2 Number Conversions

// A.3 Expressions

bool
Parser::primaryExpression (var & value)
{
	//__LOG__ << std::endl;

	comments ();

	if (Grammar::_this (istream))
	{
		// ...
		return true;
	}

	if (identifier ())
		return true;

	if (literal (value))
		return true;

	//if (arrayLiteral (value))
	//	return true;

	//if (objectLiteral (value))
	//	return true;

	if (Grammar::OpenParenthesis (istream))
	{
		if (expression (value))
		{
			comments ();

			if (Grammar::CloseParenthesis (istream))
				return true;

			throw SyntaxError ("Expected ')' after expression.");
		}

		throw SyntaxError ("Expected expression after '('.");
	}

	return false;
}

bool
Parser::memberExpression (var & value)
{
	//__LOG__ << std::endl;

	if (primaryExpression (value))
		return true;

	// ...

	return false;
}

bool
Parser::newExpression (var & value)
{
	//__LOG__ << std::endl;

	if (memberExpression (value))
		return true;

	// ...

	return false;
}

bool
Parser::leftHandSideExpression (var & value)
{
	//__LOG__ << std::endl;

	if (newExpression (value))
		return true;

	//if (callExpression ())
	//	return true;

	return false;
}

bool
Parser::postfixExpression (var & value)
{
	//__LOG__ << std::endl;

	if (leftHandSideExpression (value))
		return true;

	// ...

	return false;
}

bool
Parser::unaryExpression (var & value)
{
	//__LOG__ << std::endl;

	if (postfixExpression (value))
		return true;

	// ...

	return false;
}

bool
Parser::multiplicativeExpression (var & lhs)
{
	//__LOG__ << std::endl;

	if (unaryExpression (lhs))
	{
		comments ();

		if (Grammar::Multiplication (istream))
		{
			var rhs;

			if (multiplicativeExpression (rhs))
			{
				lhs = multiplication (lhs, rhs);
				return true;
			}

			throw SyntaxError ("Expected expression after '*'.");
		}

		if (Grammar::Division (istream))
		{
			var rhs;

			if (multiplicativeExpression (rhs))
			{
				lhs = division (lhs, rhs);
				return true;
			}

			throw SyntaxError ("Expected expression after '/'.");
		}

		if (Grammar::Remainder (istream))
		{
			var rhs;

			if (multiplicativeExpression (rhs))
			{
				lhs = remainder (lhs, rhs);
				return true;
			}

			throw SyntaxError ("Expected expression after '/'.");
		}

		return true;
	}

	return false;
}

bool
Parser::additiveExpression (var & lhs)
{
	//__LOG__ << std::endl;

	if (multiplicativeExpression (lhs))
	{
		comments ();

		if (Grammar::Addition (istream))
		{
			var rhs;

			if (additiveExpression (rhs))
			{
				lhs = addition (lhs, rhs);
				return true;
			}

			throw SyntaxError ("Expected expression after '+'.");
		}

		if (Grammar::Subtraction (istream))
		{
			var rhs;

			if (additiveExpression (rhs))
			{
				lhs = subtraction (lhs, rhs);
				return true;
			}

			throw SyntaxError ("Expected expression after '-'.");
		}

		return true;
	}

	return false;
}

bool
Parser::shiftExpression (var & lhs)
{
	//__LOG__ << std::endl;

	if (additiveExpression (lhs))
	{
		comments ();

		if (Grammar::LeftShift (istream))
		{
			var rhs;

			if (shiftExpression (rhs))
			{
				return true;
			}

			throw SyntaxError ("Expected expression after '<<'.");
		}

		if (Grammar::UnsignedRightShift (istream))
		{
			var rhs;

			if (shiftExpression (rhs))
			{
				return true;
			}

			throw SyntaxError ("Expected expression after '>>>'.");
		}

		if (Grammar::RightShift (istream))
		{
			var rhs;

			if (shiftExpression (rhs))
			{
				return true;
			}

			throw SyntaxError ("Expected expression after '>>'.");
		}

		return true;
	}

	return false;
}

bool
Parser::relationalExpression (var & lhs)
{
	//__LOG__ << std::endl;

	if (shiftExpression (lhs))
	{
		comments ();

		if (Grammar::LessEqual (istream))
		{
			var rhs;

			if (relationalExpression (rhs))
			{
				return true;
			}

			throw SyntaxError ("Expected expression after '<='.");
		}

		if (Grammar::GreaterEqual (istream))
		{
			var rhs;

			if (relationalExpression (rhs))
			{
				return true;
			}

			throw SyntaxError ("Expected expression after '>='.");
		}

		if (Grammar::Less (istream))
		{
			var rhs;

			if (relationalExpression (rhs))
			{
				return true;
			}

			throw SyntaxError ("Expected expression after '<'.");
		}

		if (Grammar::Greater (istream))
		{
			var rhs;

			if (relationalExpression (rhs))
			{
				return true;
			}

			throw SyntaxError ("Expected expression after '>'.");
		}

		if (Grammar::instanceof (istream))
		{
			var rhs;

			if (relationalExpression (rhs))
			{
				return true;
			}

			throw SyntaxError ("Expected expression after 'instanceof'.");
		}

		if (Grammar::in (istream))
		{
			var rhs;

			if (relationalExpression (rhs))
			{
				return true;
			}

			throw SyntaxError ("Expected expression after 'in'.");
		}

		return true;
	}

	return false;
}

bool
Parser::equalityExpression (var & lhs)
{
	//__LOG__ << std::endl;

	if (relationalExpression (lhs))
	{
		comments ();

		if (Grammar::StrictEqual (istream))
		{
			var rhs;

			if (equalityExpression (rhs))
			{
				return true;
			}

			throw SyntaxError ("Expected expression after '==='.");
		}

		if (Grammar::StrictNotEqual (istream))
		{
			var rhs;

			if (equalityExpression (rhs))
			{
				return true;
			}

			throw SyntaxError ("Expected expression after '!=='.");
		}

		if (Grammar::Equal (istream))
		{
			var rhs;

			if (equalityExpression (rhs))
			{
				return true;
			}

			throw SyntaxError ("Expected expression after '=='.");
		}

		if (Grammar::NotEqual (istream))
		{
			var rhs;

			if (equalityExpression (rhs))
			{
				return true;
			}

			throw SyntaxError ("Expected expression after '!='.");
		}

		return true;
	}

	return false;
}

bool
Parser::bitwiseANDExpression (var & lhs)
{
	//__LOG__ << std::endl;

	if (equalityExpression (lhs))
	{
		comments ();

		if (Grammar::LogicalAND .lookahead (istream))
			return true;

		if (Grammar::BitwiseAND (istream))
		{
			var rhs;

			if (bitwiseANDExpression (rhs))
			{
				return true;
			}

			throw SyntaxError ("Expected expression after '&'.");
		}

		return true;
	}

	return false;
}

bool
Parser::bitwiseXORExpression (var & lhs)
{
	//__LOG__ << std::endl;

	if (bitwiseANDExpression (lhs))
	{
		comments ();

		if (Grammar::BitwiseXOR (istream))
		{
			var rhs;

			if (bitwiseXORExpression (rhs))
			{
				return true;
			}

			throw SyntaxError ("Expected expression after '^'.");
		}

		return true;
	}

	return false;
}

bool
Parser::bitwiseORExpression (var & lhs)
{
	//__LOG__ << std::endl;

	if (bitwiseXORExpression (lhs))
	{
		comments ();

		if (Grammar::LogicalOR .lookahead (istream))
			return true;

		if (Grammar::BitwiseOR (istream))
		{
			var rhs;

			if (bitwiseORExpression (rhs))
			{
				return true;
			}

			throw SyntaxError ("Expected expression after '|'.");
		}

		return true;
	}

	return false;
}

bool
Parser::logicalANDExpression (var & lhs)
{
	//__LOG__ << std::endl;

	if (bitwiseORExpression (lhs))
	{
		comments ();

		if (Grammar::LogicalAND (istream))
		{
			var rhs;

			if (logicalANDExpression (rhs))
			{
				return true;
			}

			throw SyntaxError ("Expected expression after '&&'.");
		}

		return true;
	}

	return false;
}

bool
Parser::logicalORExpression (var & lhs)
{
	//__LOG__ << std::endl;

	if (logicalANDExpression (lhs))
	{
		comments ();

		if (Grammar::LogicalOR (istream))
		{
			var rhs;

			if (logicalORExpression (rhs))
			{
				return true;
			}

			throw SyntaxError ("Expected expression after '||'.");
		}

		return true;
	}

	return false;
}

bool
Parser::conditionalExpression (var & first)
{
	//__LOG__ << std::endl;

	if (logicalORExpression (first))
	{
		comments ();

		if (Grammar::QuestionMark (istream))
		{
			var second;

			if (assignmentExpression (second))
			{
				comments ();

				if (Grammar::Colon (istream))
				{
					var third;

					if (assignmentExpression (third))
					{
						return true;
					}
				}

				throw SyntaxError ("Expected expression after ':'.");
			}

			throw SyntaxError ("Expected expression after '?'.");
		}

		return true;
	}

	return false;
}

bool
Parser::assignmentExpression (var & value)
{
	//__LOG__ << std::endl;

	const auto state = getState ();

	if (leftHandSideExpression (value))
	{
		comments ();

		if (Grammar::Assignment (istream))
		{
			if (assignmentExpression (value))
				return true;

			throw SyntaxError ("Expected expression after '='.");
		}

		AssignmentOperatorType type;

		if (assignmentOperator (type))
		{
			if (assignmentExpression (value))
				return true;

			throw SyntaxError ("Expected expression after '" + to_string (type) + "'."); // XXX
		}

		setState (state);
	}

	if (conditionalExpression (value))
		return true;

	return false;
}

bool
Parser::assignmentOperator (AssignmentOperatorType & type)
{
	comments ();

	if (Grammar::MultiplicationAssigment (istream))
	{
		type = AssignmentOperatorType::MULTIPLICATION_ASSIGMENT;
		return true;
	}

	if (Grammar::DivisionAssignment (istream))
	{
		type = AssignmentOperatorType::DIVISION_ASSIGNMENT;
		return true;
	}

	if (Grammar::RemainderAssignment (istream))
	{
		type = AssignmentOperatorType::REMAINDER_ASSIGNMENT;
		return true;
	}

	if (Grammar::AdditionAssignment (istream))
	{
		type = AssignmentOperatorType::ADDITION_ASSIGNMENT;
		return true;
	}

	if (Grammar::SubtractionAssignment (istream))
	{
		type = AssignmentOperatorType::SUBTRACTION_ASSIGNMENT;
		return true;
	}

	if (Grammar::LeftShiftAssignment (istream))
	{
		type = AssignmentOperatorType::LEFT_SHIFT_ASSIGNMENT;
		return true;
	}

	if (Grammar::RightShiftAssignment (istream))
	{
		type = AssignmentOperatorType::RIGHT_SHIFT_ASSIGNMENT;
		return true;
	}

	if (Grammar::UnsignedRightShiftAssignment (istream))
	{
		type = AssignmentOperatorType::UNSIGNED_RIGHT_SHIFT_ASSIGNMENT;
		return true;
	}

	if (Grammar::BitwiseANDAssignment (istream))
	{
		type = AssignmentOperatorType::BITWISE_AND_ASSIGNMENT;
		return true;
	}

	if (Grammar::BitwiseXORAssignment (istream))
	{
		type = AssignmentOperatorType::BITWISE_XOR_ASSIGNMENT;
		return true;
	}

	if (Grammar::BitwiseORAssignment (istream))
	{
		type = AssignmentOperatorType::BITWISE_OR_ASSIGNMENT;
		return true;
	}

	return false;
}

bool
Parser::expression (var & value)
{
	//__LOG__ << std::endl;

	if (assignmentExpression (value))
	{
		for ( ; ;)
		{
			comments ();

			if (Grammar::Comma (istream))
			{
				if (assignmentExpression (value))
				{
					continue;
				}

				throw SyntaxError ("Expected expression after ','.");
			}

			return true;
		}
	}

	return false;
}

// A.4 Statements

bool
Parser::statement ()
{
	//__LOG__ << std::endl;

	if (expressionStatement ())
		return true;

	if (emptyStatement ())
		return true;

	return false;
}

bool
Parser::expressionStatement ()
{
	//__LOG__ << std::endl;

	// [lookahead ? {{, function}]

	var value;

	if (expression (value))
	{
		if (Grammar::Semicolon (istream))
		{
			getScope () -> getExpressions () .emplace_back (std::move (value));
			return true;
		}

		throw SyntaxError ("Expected ';' after expression.");
	}

	return false;
}

bool
Parser::emptyStatement ()
{
	//__LOG__ << std::endl;

	comments ();

	if (Grammar::Semicolon (istream))
		return true;

	return false;
}

// A.5 Functions and Programs

bool
Parser::functionDeclaration ()
{
	//__LOG__ << std::endl;

	return false;
}

void
Parser::program ()
{
	//__LOG__ << std::endl;

	sourceElements ();
}

void
Parser::sourceElements ()
{
	//__LOG__ << std::endl;

	while (sourceElement ())
		;
}

bool
Parser::sourceElement ()
{
	//__LOG__ << std::endl;

	if (statement ())
		return true;

	if (functionDeclaration ())
		return true;

	return false;
}

} // pb
} // titania
