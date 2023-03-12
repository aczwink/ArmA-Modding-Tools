#pragma once

namespace SJCWinLib
{
	class ITextDocument
	{
	public:
		//Functions
		virtual uint32 GetLine(uint32 lineNumber, SJCLib::CString &refOutput) const = NULL;
		virtual uint32 GetLineNumber(uint32 offset) const = NULL;
		virtual uint32 GetLineOffset(uint32 lineNumber) const = NULL;
		virtual uint32 GetLongestLineLength() const = NULL;
		virtual uint32 GetNumberOfChars() const = NULL;
		virtual uint32 GetNumberOfLines() const = NULL;
	};
}