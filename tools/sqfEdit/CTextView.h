//SJC Libs
#include <SJCWinLib.h>
//Local
#include "ITextDocument.h"
//Definitions
#define CTEXTVIEW_MAXFONTS 10

namespace SJCWinLib
{
	/*
	Limitations:
		-fixed line height (always the one of the largest font)
	*/
	class CTextView : public CWindow
	{
		struct SFontData
		{
			bool isValid;
			CFont font;
			uint16 ascent;
			uint16 averageCharWidth;
			uint16 charHeight;
			uint16 externalLeading;
		};
		
		struct STextAttribute
		{
			uint16 fontDataIndex;
			CColor foreground;
			CColor background;
		};
	private:
		//Variables
		ITextDocument *pDoc;
		uint32 caretOffset;
		bool isCaretVisible;
		uint32 selectionStart;
		uint32 selectionEnd;
		bool selecting;
		bool isScrollTimerActive;
		//Layout Variables
		uint16 nFonts;
		SFontData fonts[CTEXTVIEW_MAXFONTS];
		uint16 lineHeight;
		uint16 lineSpacingUpper;
		uint16 lineSpacingLower;
		uint16 maxAscent;
		uint16 minAverageFontWidth;
		uint16 linesToDisplay;
		uint16 maxColumnsToDisplay;
		uint16 leftMarginWidth;
		uint32 vertScrollPos;
		uint32 vertScrollMax;
		uint32 horzScrollPos;
		uint32 horzScrollMax;
		bool showLineNumbers;
		//Color Variables
		CColor backgroundColor;
		CColor textColor;
		CColor selectionBackgroundColor;
		CColor selectionTextColor;
		//Callbacks
		void OnFocus(CWindow *pPreviousFocusedWindow);
		void OnHorizontalScroll(uint16 scrollCode);
		void OnKeyDown(uint16 characterCode, uint16 repeatCount, byte scanCode, bool isExtendedKey, bool previousKeyState);
		void OnLeftMouseButtonDown(uint16 keys, uint16 x, uint16 y);
		void OnLeftMouseButtonUp(uint16 keys, uint16 x, uint16 y);
		void OnLooseFocus(CWindow *pUpcomingFocusedWindow);
		void OnMouseMove(uint16 keys, int16 x, int16 y);
		void OnMouseWheel(int16 zDelta, uint16 keys);
		void OnPaint();
		bool OnRequestCursor(const CWindow *pContainingWindow, uint16 hitTest, uint16 msgId);
		void OnSize(WPARAM resizingType, uint32 newWidth, uint32 newHeight);
		void OnTimerExpires(uint32 timerId);
		void OnVerticalScroll(uint16 scrollCode);
		//Functions
		uint16 ApplyTextAttributes(uint32 docOffset, STextAttribute &refTextAttribute);
		uint32 CoordToOffset(uint32 x, uint32 y);
		void CreateCaret(uint16 height);
		void DrawLeftMargin(uint32 lineNumber, CDeviceContext &refDC);
		void DrawLine(uint32 lineNumber, CDeviceContext &refDC);
		void DrawLineText(uint32 lineNumber, CRect &refRcLine, CDeviceContext &refDC);
		uint16 DrawText(uint32 x, uint32 y, const SJCLib::CString &refText, const STextAttribute &refTextAttribute, CDeviceContext &refDC);
		uint16 GetLeftMarginWidth() const;
		uint32 GetTextPortionInfo(const SJCLib::CString &refString, uint32 lineOffset, uint32 &refWidth, const STextAttribute &refTextAttribute, const CDeviceContext &refDC);
		void InvalidateLine(uint32 lineNumber);
		void InvalidateRange(uint32 startOffset, uint32 endOffset);
		void MoveCaret();
		/*
		Returns true if the coord is visible on the screen, false otherwise
		*/
		bool OffsetToCoord(uint32 offset, uint32 &refX, uint32 &refY);
		void RecalcFontValues();
		void RefreshWindow();
		void RepositionCaret();
		void Scroll(int32 dx, int32 dy);
		void ShowCaret(bool show = true);
		void UpdateScrollBars();
		//Inline
		inline void GetNormalizedSelection(uint32 &refStart, uint32 &refEnd) const
		{
			if(this->selectionStart < this->selectionEnd)
			{
				refStart = this->selectionStart;
				refEnd = this->selectionEnd;
			}
			else
			{
				refStart = this->selectionEnd;
				refEnd = this->selectionStart;
			}
		}

		inline bool IsKeyPressed(uint16 virtualKey) const
		{
			return GetKeyState(virtualKey) < 0;
		}
	public:
		//Constructor
		CTextView(ITextDocument *pDoc);
		//Functions
		void Create(const CWindow &refParentWindow);
		void SetFont(uint16 idx, const CFont &refFont);
	};

	/*class CTextView : public CWindow
	{
		struct SFontData
		{
			uint16 charWidth;
		};
	private:
		//Display Variables
		uint32 selectionStart;
		uint32 selectionEnd;
		uint16 fontWidth;
		//Callbacks
		uint8 OnMouseActivate(CPopupWindow *pActivatedWnd, uint16 hitTest, uint16 mouseMessage);
		void OnPaint();
		void OnSetFont(const CFont &refFont);
		//Functions
		void CoordToTextOffset(uint16 x, uint16 y) const;
		bool PinToBottomCorner();
		void RecalcLineHeight();
		void UpdateMetrics();
	public:
		//Functions
		void SetFont(uint16 index, const CFont &refFont);
	};*/
}