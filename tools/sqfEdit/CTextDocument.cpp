//Class Header
#include "CTextDocument.h"
//Global
#include <iostream>
//Namespaces
using namespace SJCLib;
using namespace SJCWinLib;

//Constructor
CTextDocument::CTextDocument()
{
	this->maxLinesInLineBuffer = 0;
	this->nLines = 0;
	this->pLineBuffer = NULL;
}

//Destructor
CTextDocument::~CTextDocument()
{
	this->Release();
}

//Public Functions
uint32 CTextDocument::GetLine(uint32 lineNumber, CString &refOutput) const
{
	if(lineNumber >= this->nLines)
	{
		refOutput = CString();
		return this->GetNumberOfChars();
	}

	refOutput = this->text.SubString(this->pLineBuffer[lineNumber], this->pLineBuffer[lineNumber+1] - this->pLineBuffer[lineNumber] - 1);

	return this->pLineBuffer[lineNumber];
}

uint32 CTextDocument::GetLineNumber(uint32 offset) const
{
	uint32 i;
	for(i = 0; i <= this->nLines; i++)
	{
		if(this->pLineBuffer[i] > offset)
		{
			if(i == 0)
				return 0;
			return i-1;
		}
	}

	return UINT32_MAX;
}

uint32 CTextDocument::GetLineOffset(uint32 lineNumber) const
{
	if(lineNumber >= this->nLines)
	{
		return this->GetNumberOfChars();
	}
	
	return this->pLineBuffer[lineNumber];
}

uint32 CTextDocument::GetLongestLineLength() const
{
	uint32 longestLength, offset, lineLength;
	
	longestLength = 0;
	offset = 0;
	
	repeat(this->nLines, i)
	{
		lineLength = 0;
		repeat(this->pLineBuffer[i+1] - this->pLineBuffer[i], j)
		{
			if(this->text[offset] == '\t')
				offset += 3;
			offset++;
			lineLength++;
		}
		
		longestLength = MAX(longestLength, lineLength);
	}
	
	return longestLength;
}

uint32 CTextDocument::GetNumberOfChars() const
{
	return this->text.GetLength();
}

uint32 CTextDocument::GetNumberOfLines() const
{
	return this->nLines;
}

void CTextDocument::Release()
{
	this->text.Release();
	memfreesafe(this->pLineBuffer);
}

void CTextDocument::SetText(const CString &refText)
{
	this->Release();
	
	this->nLines = 0;
	
	this->maxLinesInLineBuffer = refText.GetLength();
	this->pLineBuffer = (uint32 *)malloc(this->maxLinesInLineBuffer * sizeof(*this->pLineBuffer));
	
	this->pLineBuffer[this->nLines++] = 0;
	repeat(refText.GetLength(), i)
	{
		if(refText[i] == '\r') //ignore
			continue;
		if(refText[i] == '\n')
		{
			this->text += refText[i];
			this->pLineBuffer[this->nLines++] = this->text.GetLength();
			continue;
		}
		this->text += refText[i];
	}
	this->pLineBuffer[this->nLines] = this->text.GetLength();
}