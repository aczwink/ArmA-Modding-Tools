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
//Class header
#include "RapParser.hpp"

//Public methods
void RapParser::Parse()
{
	while(!this->Accept(TOKEN_END))
	{
		auto topLevel = this->ParseNextTopLevel();
		if(!topLevel.IsNull())
			this->tree->rootNode->AddChild(Move(topLevel));
	}
}

//Private methods
UniquePointer<RapNode> RapParser::ParseArrayLiteral(const String& identifier)
{
	this->Expect(TOKEN_BRACEOPEN);

	UniquePointer<RapNode> node = new RapNode;
	node->SetPacketType(RAP_PACKETTYPE_ARRAY);
	node->SetName(identifier);

	while(!this->Accept(TOKEN_BRACECLOSE))
	{
		auto value = this->ParseArrayValue();
		node->AddArrayValue(value);

		if(this->Accept(TOKEN_BRACECLOSE))
			break;
		else
			this->Expect(TOKEN_COMMA);
	}

	return node;
}

RapArrayValue RapParser::ParseArrayValue()
{
	RapArrayValue value;
	if(this->Accept(TOKEN_FLOAT_LITERAL))
		value.SetValue(Float<float32>::Parse(lexer.GetCurrentTokenValue()));
	else if(this->Accept(TOKEN_INT_LITERAL))
		value.SetValue(lexer.GetCurrentTokenValue().ToInt32());
	else if(this->Accept(TOKEN_STRING_LITERAL))
	{
		String tokenValue = this->lexer.GetCurrentTokenValue();
		value.SetValue(tokenValue);
	}
	else if(this->Accept(TOKEN_TEXT))
	{
		String identifier = this->lexer.GetCurrentTokenValue();
		if(!this->tree->IsEnumDefined(identifier))
			this->ReportFeedback(RapParseFeedback::UnquotedString, identifier);

		value.SetValue(identifier);
	}
	else if(this->PeekNextToken() == TOKEN_COMMA)
	{
		//empty array literal values are allowed
		value.SetValue(String());
	}
	else if(this->PeekNextToken() == TOKEN_BRACEOPEN)
	{
		//TODO: fix this
		String TODO;
		auto literalNode = this->ParseArrayLiteral(TODO);

		value.SetValue(Move(literalNode->ArrayValue()));
	}
	else
		throw raPParseException(u8"Illegal syntax in array literal", this->lexer.CurrentContext());

	return value;
}

UniquePointer<RapNode> RapParser::ParseClass()
{
	String name = this->Expect(TOKEN_TEXT);

	UniquePointer<RapNode> classNode = new RapNode;
	classNode->SetPacketType(RAP_PACKETTYPE_CLASS);
	classNode->SetName(name);

	if(this->Accept(TOKEN_COLON))
	{
		this->Expect(TOKEN_TEXT);
		classNode->InheritedClassName(this->lexer.GetCurrentTokenValue());
	}

	this->Expect(TOKEN_BRACEOPEN);

	while(!this->Accept(TOKEN_BRACECLOSE))
	{
		String tokenValue;
		if(this->Accept(TOKEN_TEXT, tokenValue))
		{
			UniquePointer<RapNode> child;
			if(tokenValue == u8"class")
				child = this->ParseClass();
			else
				child = this->ParseClassMemberPropertySet(tokenValue);
			classNode->AddChild(Move(child));
		}
		else if(this->Accept(TOKEN_SEMICOLON))
		{
			//engine accepts extra semicolons
			if(!this->Accept(TOKEN_SEMICOLON))
				this->ReportFeedback(RapParseFeedback::ExtraSemicolon, name);
		}
		else
		{
			NOT_IMPLEMENTED_ERROR; //TODO: implement me
		}
	}

	if(!this->Accept(TOKEN_SEMICOLON))
		this->ReportFeedback(RapParseFeedback::MissingSemicolonAtClassEnd, name);

	return classNode;
}

UniquePointer<RapNode> RapParser::ParseClassMemberPropertySet(const String& identifier)
{
	if(this->Accept(TOKEN_ASSIGNMENT))
	{
		this->skipLineBreaks = false;

		auto result = this->ParseValue(identifier);

		if(this->Accept(TOKEN_LINEBREAK))
			this->ReportFeedback(RapParseFeedback::MissingSemicolonAtPropertyAssignmentEnd, identifier);
		else
			this->Expect(TOKEN_SEMICOLON);

		this->skipLineBreaks = true;

		return result;
	}
	else if(this->Accept(TOKEN_ARRAY_MARKER))
	{
		this->Expect(TOKEN_ASSIGNMENT);

		auto result = this->ParseArrayLiteral(identifier);

		this->Expect(TOKEN_SEMICOLON);

		return result;
	}
	else
		throw raPParseException(u8"Illegal syntax encountered in property value assignment to '" + identifier + u8"'", this->lexer.CurrentContext());
}

void RapParser::ParseEnum()
{
	this->Accept(TOKEN_TEXT); //name of the enum

	this->Expect(TOKEN_BRACEOPEN);

	uint32 value = 0;
	while(!this->Accept(TOKEN_BRACECLOSE))
	{
		String name = this->Expect(TOKEN_TEXT);
		this->tree->DefineEnumValue(name, value++);

		if(this->Accept(TOKEN_BRACECLOSE))
			break;
		else
			this->Expect(TOKEN_COMMA);
	}

	this->Expect(TOKEN_SEMICOLON);
}

UniquePointer<RapNode> RapParser::ParseNextTopLevel()
{
	String tokenValue = this->Expect(TOKEN_TEXT);

	if(tokenValue == u8"class")
		return this->ParseClass();
	else if(tokenValue == u8"enum")
		this->ParseEnum();
	else
	{
		NOT_IMPLEMENTED_ERROR; //TODO: implement me
	}

	return nullptr;
}

UniquePointer<RapNode> RapParser::ParseValue(const String& identifier)
{
	UniquePointer<RapNode> node = new RapNode;
	node->SetPacketType(RAP_PACKETTYPE_VARIABLE);
	node->SetName(identifier);

	String tokenValue;
	if(this->Accept(TOKEN_INT_LITERAL, tokenValue))
		node->SetValue(tokenValue.ToInt32());
	else if(this->Accept(TOKEN_FLOAT_LITERAL, tokenValue))
		node->SetValue(Float<float32>::Parse(tokenValue));
	else if(this->Accept(TOKEN_STRING_LITERAL, tokenValue))
		node->SetValue(tokenValue);
	else if(this->Accept(TOKEN_TEXT, tokenValue))
	{
		if(!this->tree->IsEnumDefined(tokenValue))
			this->ReportFeedback(RapParseFeedback::UnquotedString, tokenValue);

		node->SetValue(tokenValue);
	}
	else if(this->PeekNextToken() == TOKEN_SEMICOLON)
	{
		if(!this->tree->IsEnumDefined(tokenValue))
			this->ReportFeedback(RapParseFeedback::UnquotedString, tokenValue);

		//empty string is allowed
		node->SetValue(u8"");
	}
	else
		throw raPParseException(u8"Illegal syntax encountered in property value assignment to '" + identifier + u8"'", this->lexer.CurrentContext());

	//check for math formulas
	String tokenValue2;
	if(this->Accept(TOKEN_TEXT, tokenValue2) || this->Accept(TOKEN_COLON, tokenValue2))
	{
		tokenValue += u8" " + tokenValue2;

		while(this->Accept(TOKEN_INT_LITERAL, tokenValue2) || this->Accept(TOKEN_FLOAT_LITERAL, tokenValue2) || this->Accept(TOKEN_TEXT, tokenValue2))
			tokenValue += u8" " + tokenValue2;

		node->SetValue(tokenValue);

		this->ReportFeedback(RapParseFeedback::UnquotedString, tokenValue);
	}

	return node;
}