//SJCLib
#include <SJCLib.h>
//Local
#include "ITextDocument.h"

namespace SJCWinLib
{
	class CTextDocument : public ITextDocument
	{
	private:
		//Variables
		SJCLib::CString text;
		uint32 *pLineBuffer;
		uint32 maxLinesInLineBuffer;
		uint32 nLines;
	public:
		//Constructor
		CTextDocument();
		//Destructor
		~CTextDocument();
		//Functions
		uint32 GetLine(uint32 lineNumber, SJCLib::CString &refOutput) const;
		uint32 GetLineNumber(uint32 offset) const;
		uint32 GetLineOffset(uint32 lineNumber) const;
		uint32 GetLongestLineLength() const;
		uint32 GetNumberOfChars() const;
		uint32 GetNumberOfLines() const;
		void Release();
		void SetText(const SJCLib::CString &refText);
	};
}