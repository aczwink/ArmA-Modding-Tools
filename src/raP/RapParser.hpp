/*
 * Copyright (c) 2023 Amir Czwink (amir130@hotmail.de)
 *
 * This file is part of ArmA-Modding-Tools.
 *
 * ArmA-Modding-Tools is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ArmA-Modding-Tools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ArmA-Modding-Tools.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <libBISMod/raP/RapNode.hpp>
#include <libBISMod/raP/raPParseException.hpp>
#include "RapLexer.hpp"
using namespace libBISMod;

class RapParser
{
public:
	//Constructor
	inline RapParser(RapLexer& lexer, const String& rootName, const Function<void(const RapParseFeedback&, const String&, const RapParseContext& context)>& reportFeedback) : lexer(lexer), reportFeedback(reportFeedback)
	{
		this->tree = new RapTree;
		this->tree->SetPacketType(RAP_PACKETTYPE_CLASS);
		this->tree->SetName(rootName);

		this->lookahead = this->lexer.GetNextToken();
		this->skipLineBreaks = true;
	}

	//Methods
	void Parse();

	//Inline
	inline UniquePointer<RapTree> TakeRoot()
	{
		return Move(this->tree);
	}

private:
	//State
	bool skipLineBreaks;
	RapLexer lexer;
	RapToken lookahead;
	UniquePointer<RapTree> tree;
	const Function<void(const RapParseFeedback&, const String&, const RapParseContext& context)>& reportFeedback;

	//Private methods
	void ParseArrayLiteral(const String& identifier, RapNode& parent);
	RapArrayValue ParseArrayValue();
	void ParseClass(RapNode& parent);
	void ParseClassMemberPropertySet(const String& identifier, RapNode& parent);
	void ParseEnum();
	void ParseNextTopLevel();
	void ParseValue(const String& identifier, RapNode& parent);

	//Inline
	inline bool Accept(RapToken token)
	{
		String tmp;
		return this->Accept(token, tmp);
	}

	inline bool Accept(RapToken token, String& tokenValue)
	{
		if(this->skipLineBreaks)
		{
			while(this->lookahead == TOKEN_LINEBREAK)
				this->lookahead = this->lexer.GetNextToken();
		}
		if(this->lookahead == token)
		{
			tokenValue = this->lexer.GetCurrentTokenValue();
			this->lookahead = this->lexer.GetNextToken();
			return true;
		}
		return false;
	}

	inline String Expect(RapToken expectedToken)
	{
		String tokenValue;
		if(!this->Accept(expectedToken, tokenValue))
			throw raPParseException(u8"Illegal syntax encountered", this->lexer.CurrentContext());
		return tokenValue;
	}

	inline RapToken PeekNextToken()
	{
		return this->lookahead;
	}

	inline void ReportFeedback(RapParseFeedback feedback, const String& context)
	{
		this->reportFeedback(feedback, context, this->lexer.CurrentContext());
	}
};