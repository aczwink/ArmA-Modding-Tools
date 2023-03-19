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
//Corresponding header
#include <libBISMod/raP/raP.hpp>
//Libs
#include <StdXX.hpp>
//Local
#include <libBISMod/raP/CRapNode.hpp>
#include "CRapLexer.hpp"
#include "raPInternal.hpp"
//Namespaces
using namespace libBISMod;
using namespace StdXX;
using namespace StdXX::FileSystem;
//Definitions
#define RAPERROR(returnValue) pCtx->context = lexer.GetCurrentLine(); pCtx->lineNumber = lexer.GetCurrentLineNumber(); return returnValue;

struct SRapParseData
{
	BinaryTreeMap<String, uint32> *pIntDefs;
	BinaryTreeMap<String, float32> *pFloatDefs;
	BinaryTreeMap<String, SRapFunctionDefinition> functionDefs;
};

//Internal Prototypes
static void RapParseArrayInner(DynamicArray<CRapArrayValue> &arrayValues, CRapLexer &lexer, CRapTree *pRootNode, SRapParseData *pParseData, SRapErrorContext *pCtx);
static void RapParseDefinition(CRapLexer &lexer, CRapTree *pRootNode, SRapParseData *pParseData, SRapErrorContext *pCtx);
static void RapParseIdentifier(CRapNode *pNode, const String &identifier, CRapLexer &lexer, CRapTree *pRootNode, SRapParseData *pParseData, SRapErrorContext *pCtx);
static void RapParseNextToken(EToken t, CRapNode *pNode, CRapLexer &lexer, CRapTree *pRootNode, SRapParseData *pParseData, SRapErrorContext *pCtx);
static void RapParseValue(CRapNode *pNode, CRapLexer &lexer, const String &identifier, CRapTree *pRootNode, SRapParseData *pParseData, SRapErrorContext *pCtx);

//Local functions
bool AssignDefValue(String name, CRapNode &refNode, CRapTree *pRootNode, SRapParseData *pParseData)
{
	if(pParseData->pIntDefs->Find(name) != pParseData->pIntDefs->end())
	{
		refNode.SetVariableType(RAP_VARIABLETYPE_INT);
		refNode.SetValue((int32)(*pParseData->pIntDefs->Find(name)).value);

		return true;
	}
	if(pParseData->pFloatDefs->Find(name) != pParseData->pFloatDefs->end())
	{
		refNode.SetVariableType(RAP_VARIABLETYPE_FLOAT);
		refNode.SetValue((*pParseData->pFloatDefs->Find(name)).value);

		return true;
	}

	return false;
}

bool AssignDefValue(String name, CRapArrayValue &refValue, CRapTree *pRootNode, SRapParseData *pParseData)
{
	if(pParseData->pIntDefs->Find(name) != pParseData->pIntDefs->end())
	{
		refValue.SetType(RAP_ARRAYTYPE_INT);
		refValue.SetValue((int32)(*pParseData->pIntDefs->Find(name)).value);

		return true;
	}
	if(pParseData->pFloatDefs->Find(name) != pParseData->pFloatDefs->end())
	{
		refValue.SetType(RAP_ARRAYTYPE_FLOAT);
		refValue.SetValue((*pParseData->pFloatDefs->Find(name)).value);

		return true;
	}

	return false;
}

static void RapParseArray(CRapNode *pNode, const String &identifier, CRapLexer &lexer, CRapTree *pRootNode, SRapParseData *pParseData, SRapErrorContext *pCtx)
{
	CRapNode node;
	DynamicArray<CRapArrayValue> arrayValues;

	ASSERT_TRUE(lexer.GetNextToken() == TOKEN_SQUAREDBRACKETCLOSE);
	ASSERT_TRUE(lexer.GetNextToken() == TOKEN_ASSIGNMENT);
	ASSERT_TRUE(lexer.GetNextToken() == TOKEN_BRACEOPEN);

	node.SetPacketType(RAP_PACKETTYPE_ARRAY);
	node.SetName(identifier);
	RapParseArrayInner(arrayValues, lexer, pRootNode, pParseData, pCtx);
	for(uint32 i = 0; i < arrayValues.GetNumberOfElements(); i++)
		node.AddArrayValue(arrayValues[i]);
	pNode->AddNode(node);
}

static void RapParseArrayInner(DynamicArray<CRapArrayValue> &arrayValues, CRapLexer &lexer, CRapTree *pRootNode, SRapParseData *pParseData, SRapErrorContext *pCtx)
{
	EToken t;

	while((t = lexer.GetNextToken()) != TOKEN_END)
	{
		switch(t)
		{
			case TOKEN_LITERAL:
			{
				CRapArrayValue value;

				value.SetType(RAP_ARRAYTYPE_STRING);
				value.SetValue(lexer.GetCurrentTokenValue());
				value.SetValue(value.GetValueString().SubString(1, value.GetValueString().GetLength()-2));

				arrayValues.Push(value);
			}
				break;
			case TOKEN_NUMBERSIGN:
			{
				CRapArrayValue value;
				String buffer;
				EToken t;

				value.SetType(RAP_ARRAYTYPE_STRING);

				ASSERT_TRUE(lexer.GetNextToken() == TOKEN_BRACKETOPEN);

				while(t = lexer.GetNextToken())
				{
					if(t == TOKEN_BRACKETCLOSE)
					{
						value.SetValue(buffer);
						break;
					}
					buffer += lexer.GetCurrentTokenValue();
				}

				arrayValues.Push(value);
			}
				break;
			case TOKEN_COMMA:
				break;
			case TOKEN_INT:
			{
				CRapArrayValue value;

				value.SetType(RAP_ARRAYTYPE_INT);
				value.SetValue(lexer.GetCurrentTokenValue().ToInt32());

				arrayValues.Push(value);
			}
				break;
			case TOKEN_FLOAT:
			{
				CRapArrayValue value;

				value.SetType(RAP_ARRAYTYPE_FLOAT);
				value.SetValue(Float<float32>::Parse(lexer.GetCurrentTokenValue()));

				arrayValues.Push(value);
			}
				break;
			case TOKEN_IDENTIFIER:
			{
				CRapArrayValue value;

				if(!AssignDefValue(lexer.GetCurrentTokenValue(), value, pRootNode, pParseData))
				{
					pCtx->context = lexer.GetCurrentTokenValue();
					pCtx->lineNumber = lexer.GetCurrentLineNumber();

					NOT_IMPLEMENTED_ERROR; //TODO: implement me
					//return BISLIB_RAP_UNDEFINEDCONSTANT;
				}

				arrayValues.Push(value);
			}
				break;
			case TOKEN_BRACEOPEN:
			{
				CRapArrayValue value;
				DynamicArray<CRapArrayValue> arrayValues;

				value.SetType(RAP_ARRAYTYPE_EMBEDDEDARRAY);
				RapParseArrayInner(arrayValues, lexer, pRootNode, pParseData, pCtx);
				value.SetValue(arrayValues);

				arrayValues.Push(value);
			}
				break;
			case TOKEN_BRACECLOSE:
				if(lexer.GetNextToken() != TOKEN_SEMICOLON)
				{
					//RAPERROR(BISLIB_RAP_EXPECTEDSEMICOLON);
					NOT_IMPLEMENTED_ERROR; //TODO: implement me
				}
				break;
			default:
				NOT_IMPLEMENTED_ERROR; //TODO: implement me
				//RAPERROR(BISLIB_RAP_EXPECTEDARRAYVALUES);
				break;
		}
	}
}

static void RapParseClass(CRapNode *pNode, CRapLexer &lexer, CRapTree *pRootNode, SRapParseData *pParseData, SRapErrorContext *pCtx)
{
	CRapNode node;
	EToken t;

	ASSERT_TRUE(lexer.GetNextToken() == TOKEN_IDENTIFIER);

	node.SetPacketType(RAP_PACKETTYPE_CLASS);
	node.SetName(lexer.GetCurrentTokenValue());
	t = lexer.GetNextToken();

	if(t == TOKEN_COLON)
	{
		lexer.GetNextToken();
		node.SetInheritedClassName(lexer.GetCurrentTokenValue());
		t = lexer.GetNextToken();
	}

	ASSERT_TRUE(t == TOKEN_BRACEOPEN);

	while((t = lexer.GetNextToken()) != TOKEN_END)
	{
		switch(t)
		{
			case TOKEN_IDENTIFIER:
				RapParseIdentifier(&node, lexer.GetCurrentTokenValue(), lexer, pRootNode, pParseData, pCtx);
				break;
			case TOKEN_BRACECLOSE:
				if(lexer.GetNextToken() != TOKEN_SEMICOLON)
				{
					ASSERT(false, u8"Expected semicolon");
					//RAPERROR(BISLIB_RAP_EXPECTEDSEMICOLON);
				}
				pNode->AddNode(node);
				break;
			case TOKEN_NUMBERSIGN:
				ASSERT_TRUE(lexer.GetNextToken() == TOKEN_DEFINE);
				RapParseDefinition(lexer, pRootNode, pParseData, pCtx);
				break;
			case TOKEN_CLASS:
				RapParseClass(&node, lexer, pRootNode, pParseData, pCtx);
				break;
			default:
				NOT_IMPLEMENTED_ERROR; //TODO: implement me
				break;
		}
	}
}

static void RapParseData(CRapNode *pNode, CRapLexer &lexer, CRapTree *pRootNode, SRapParseData *pParseData, SRapErrorContext *pCtx)
{
	EToken t;

	while((t = lexer.GetNextToken()) != TOKEN_END)
	{
		RapParseNextToken(t, pNode, lexer, pRootNode, pParseData, pCtx);
	}
}

static void RapParseDefinition(CRapLexer &lexer, CRapTree *pRootNode, SRapParseData *pParseData, SRapErrorContext *pCtx)
{
	String buffer;
	EToken t;

	if(lexer.GetNextToken() != TOKEN_IDENTIFIER)
	{
		//RAPERROR(BISLIB_RAP_EXPECTEDIDENTIFIER);
		ASSERT(false, u8"Expected identifier");
	}

	buffer = lexer.GetCurrentTokenValue();
	t = lexer.GetNextToken();

	switch(t)
	{
		case TOKEN_INT:
			pParseData->pIntDefs->Insert(buffer, (uint32)lexer.GetCurrentTokenValue().ToUInt());
			break;
		case TOKEN_FLOAT:
			pParseData->pFloatDefs->Insert(buffer, Float<float32>::Parse(lexer.GetCurrentTokenValue()));
			break;
		case TOKEN_BRACKETOPEN:
			SRapFunctionDefinition def;
			bool identifierFollows = true;
			bool includeNextLine = false;

			while((t = lexer.GetNextToken()))
			{
				if(t == TOKEN_BRACKETCLOSE) break;

				switch(t)
				{
					case TOKEN_COMMA:
						ASSERT_TRUE(!identifierFollows);

						identifierFollows = true;
						break;
					case TOKEN_IDENTIFIER:
					ASSERT_TRUE(identifierFollows);

						def.params.Push(lexer.GetCurrentTokenValue());
						identifierFollows = false;
						break;
					default:
						NOT_IMPLEMENTED_ERROR; //TODO: implement me
						break;
				}
			}
			ASSERT_TRUE(!identifierFollows);

			while((t = lexer.GetNextToken(false)))
			{
				if(t == TOKEN_LINEFEED)
				{
					if(includeNextLine)
					{
						includeNextLine = false;
					}
					else
					{
						break;
					}
				}
				if(t == TOKEN_BACKSLASH)
				{
					includeNextLine = true;
				}
				else
				{
					def.body += lexer.GetCurrentTokenValue();
				}
			}

			pParseData->functionDefs[buffer] = def;
			break;
	}
}

static void RapParseFunctionDefinitionCall(CRapNode *pNode, const String &identifier, CRapLexer &lexer, CRapTree *pRootNode, SRapParseData *pParseData, SRapErrorContext *pCtx)
{
	CRapLexer bodyLexer;
	EToken t;
	String buffer;
	SRapFunctionDefinition f;
	bool paramFollows = false;

	ASSERT_TRUE(pParseData->functionDefs.Find(identifier) != pParseData->functionDefs.end());

	while(t = lexer.GetNextToken(false))
	{
		if(t == TOKEN_BRACKETCLOSE)
		{
			if(paramFollows)
			{
				f.params.Push(buffer);
			}
			if(lexer.GetNextToken() != TOKEN_SEMICOLON)
			{
				pCtx->context = lexer.GetCurrentLine();
				pCtx->lineNumber = lexer.GetCurrentLineNumber();

				//return BISLIB_RAP_EXPECTEDSEMICOLON;
				NOT_IMPLEMENTED_ERROR; //TODO: implement me
			}
			break;
		}
		else if(t == TOKEN_COMMA)
		{
			f.params.Push(buffer);
			buffer = "";
			paramFollows = true;
		}
		else
		{
			buffer += lexer.GetCurrentTokenValue();
		}
	}

	ASSERT_TRUE(f.params.GetNumberOfElements() == pParseData->functionDefs[identifier].params.GetNumberOfElements());

	buffer = pParseData->functionDefs[identifier].body;
	bodyLexer.Init(reinterpret_cast<const char *>(buffer.GetRawZeroTerminatedData()));

	while((t = bodyLexer.GetNextToken(false)) != TOKEN_END)
	{
		if(t == TOKEN_IDENTIFIER)
		{
			if(pParseData->functionDefs[identifier].params.IsInArray(bodyLexer.GetCurrentTokenValue()))
			{
				f.body += f.params[pParseData->functionDefs[identifier].params.Find(bodyLexer.GetCurrentTokenValue())];
				continue;
			}
		}
		if(t != TOKEN_DOUBLENUMBERSIGN)
		{
			f.body += bodyLexer.GetCurrentTokenValue();
		}
	}

	bodyLexer.Init(reinterpret_cast<const char *>(f.body.GetRawZeroTerminatedData()));
	return RapParseData(pNode, bodyLexer, pRootNode, pParseData, pCtx);
}

static void RapParseIdentifier(CRapNode *pNode, const String &identifier, CRapLexer &lexer, CRapTree *pRootNode, SRapParseData *pParseData, SRapErrorContext *pCtx)
{
	switch(lexer.GetNextToken())
	{
		case TOKEN_ASSIGNMENT:
			RapParseValue(pNode, lexer, identifier, pRootNode, pParseData, pCtx);
			break;
		case TOKEN_SQUAREDBRACKETOPEN:
			RapParseArray(pNode, identifier, lexer, pRootNode, pParseData, pCtx);
			break;
		case TOKEN_BRACKETOPEN:
			RapParseFunctionDefinitionCall(pNode, identifier, lexer, pRootNode, pParseData, pCtx);
			break;
	}
}

static void RapParseNextToken(EToken t, CRapNode *pNode, CRapLexer &lexer, CRapTree *pRootNode, SRapParseData *pParseData, SRapErrorContext *pCtx)
{
	switch(t)
	{
		case TOKEN_NUMBERSIGN:
			t = lexer.GetNextToken();

			if(t == TOKEN_DEFINE)
			{
				return RapParseDefinition(lexer, pRootNode, pParseData, pCtx);
			}
			break;
		case TOKEN_IDENTIFIER:
			return RapParseIdentifier(pNode, lexer.GetCurrentTokenValue(), lexer, pRootNode, pParseData, pCtx);
		case TOKEN_CLASS:
			return RapParseClass(pNode, lexer, pRootNode, pParseData, pCtx);
		default:
			NOT_IMPLEMENTED_ERROR; //TODO: implement me
	}
}

static void RapParseValue(CRapNode *pNode, CRapLexer &lexer, const String &identifier, CRapTree *pRootNode, SRapParseData *pParseData, SRapErrorContext *pCtx)
{
	CRapNode node;

	node.SetPacketType(RAP_PACKETTYPE_VARIABLE);
	node.SetName(identifier);

	switch(lexer.GetNextToken())
	{
		case TOKEN_LITERAL:
			node.SetVariableType(RAP_VARIABLETYPE_STRING);
			node.SetValue(lexer.GetCurrentTokenValue());
			node.SetValue(node.GetVariableValueString().SubString(1, node.GetVariableValueString().GetLength()-2));
			break;
		case TOKEN_NUMBERSIGN:
		{
			String buffer;
			EToken t;

			node.SetVariableType(RAP_VARIABLETYPE_STRING);

			ASSERT_TRUE(lexer.GetNextToken() == TOKEN_BRACKETOPEN);

			while(t = lexer.GetNextToken())
			{
				if(t == TOKEN_BRACKETCLOSE)
				{
					node.SetValue(buffer);
					break;
				}
				buffer += lexer.GetCurrentTokenValue();
			}
		}
			break;
		case TOKEN_INT:
			node.SetVariableType(RAP_VARIABLETYPE_INT);
			node.SetValue(lexer.GetCurrentTokenValue().ToInt32());
			break;
		case TOKEN_FLOAT:
			node.SetVariableType(RAP_VARIABLETYPE_FLOAT);
			node.SetValue(Float<float32>::Parse(lexer.GetCurrentTokenValue()));
			break;
		case TOKEN_IDENTIFIER:
			if(!AssignDefValue(lexer.GetCurrentTokenValue(), node, pRootNode, pParseData))
			{
				pCtx->context = lexer.GetCurrentTokenValue();
				pCtx->lineNumber = lexer.GetCurrentLineNumber();

				NOT_IMPLEMENTED_ERROR; //TODO: implement me
				//return BISLIB_RAP_UNDEFINEDCONSTANT;
			}
			break;
		default:
			pCtx->context = lexer.GetCurrentTokenValue();
			pCtx->lineNumber = lexer.GetCurrentLineNumber();

			NOT_IMPLEMENTED_ERROR; //TODO: implement me
			//return BISLIB_RAP_EXPECTEDVAR;
	}

	ASSERT_TRUE(lexer.GetNextToken() == TOKEN_SEMICOLON);

	pNode->AddNode(node);
}

//Namespace functions
void libBISMod::RapParse(String source, String rootName, CRapTree *pRootNode, SRapErrorContext *pCtx)
{
	CRapLexer lexer;
	SRapParseData parseInput;

	parseInput.pIntDefs = &pRootNode->intDefs;
	parseInput.pFloatDefs = &pRootNode->floatDefs;

	pRootNode->SetPacketType(RAP_PACKETTYPE_CLASS);
	pRootNode->SetName(rootName);
	lexer.Init(reinterpret_cast<const char *>(source.GetRawZeroTerminatedData()));

	return RapParseData(pRootNode, lexer, pRootNode, &parseInput, pCtx);
}

void libBISMod::RapParseFile(const Path &inputPath, CRapTree *pRootNode, SRapErrorContext *pCtx)
{
	char *pBuffer;
	uint32 bufferSize;

	FileInputStream fileInputStream(inputPath);

	bufferSize = fileInputStream.QueryRemainingBytes()+1;
	pBuffer = (char *)malloc(bufferSize);
	pBuffer[bufferSize-1] = '\0';
	fileInputStream.ReadBytes(pBuffer, bufferSize-1);
	RapParse(pBuffer, inputPath.GetName(), pRootNode, pCtx);
	free(pBuffer);
}