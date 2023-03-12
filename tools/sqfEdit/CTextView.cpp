//Class Header
#include "CTextView.h"
//Namespaces
using namespace SJCLib;
using namespace SJCWinLib;
//Definitions
#define CTEXTVIEW_DEFAULTFONTINDEX 0
#define CTEXTVIEW_NTABCHARS 4
#define CTEXTVIEW_TIMER_SCROLL_ID 1
#define CTEXTVIEW_TIMER_SCROLL_TIMEOUT 25
#define INRANGE(x, start, end) (x >= start && x < end)

//Constructor
CTextView::CTextView(ITextDocument *pDoc)
{
	this->pDoc = pDoc;
	this->caretOffset = 0;
	this->isCaretVisible = false;
	this->selectionStart = 0;
	this->selectionEnd = 0;
	this->selecting = false;
	this->isScrollTimerActive = false;

	//Layout Variables
	this->nFonts = 0;
	repeat(CTEXTVIEW_MAXFONTS, i)
	{
		this->fonts[i].isValid = false;
	}
	this->lineHeight = 0;
	this->lineSpacingUpper = 1;
	this->lineSpacingLower = 1;
	this->maxAscent = 0;
	this->minAverageFontWidth = 0;
	this->linesToDisplay = 0;
	this->maxColumnsToDisplay = 0;
	this->leftMarginWidth = 5;
	this->vertScrollPos = 0;
	this->vertScrollMax = 0;
	this->horzScrollPos = 0;
	this->horzScrollMax = 0;
	this->showLineNumbers = true;

	//Color Variables
	this->backgroundColor.Set(255, 255, 255); //White
	this->textColor.Set(0, 0, 0); //Black
	this->selectionBackgroundColor.Set(173, 214, 255); //light blue
	this->selectionTextColor.Set(255, 255, 255); //White
}

//Callbacks
void CTextView::OnFocus(CWindow *pPreviousFocusedWindow)
{
	this->CreateCaret(this->lineHeight);
	this->RepositionCaret();
}

void CTextView::OnHorizontalScroll(uint16 scrollCode)
{
	uint32 oldPos;

	oldPos = this->horzScrollPos;

	switch(scrollCode)
	{
	case SB_LINELEFT:
		this->Scroll(-1, 0);
		break;
	case SB_LINERIGHT:
		this->Scroll(1, 0);
		break;
	case SB_PAGELEFT:
		this->Scroll(-this->maxColumnsToDisplay, 0);
		break;
	case SB_PAGERIGHT:
		this->Scroll(this->maxColumnsToDisplay, 0);
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		{
			this->horzScrollPos = this->GetScrollTrackPos(false);
			this->RefreshWindow();
		}
		break;
	case SB_LEFT:
		{
			this->horzScrollPos = 0;
			this->RefreshWindow();
		}
		break;
	case SB_RIGHT:
		{
			this->horzScrollPos = this->horzScrollMax;
			this->RefreshWindow();
		}
		break;
	}

	if(oldPos != this->horzScrollPos)
	{
		this->UpdateScrollBars();
		this->RepositionCaret();
	}
}

void CTextView::OnKeyDown(uint16 characterCode, uint16 repeatCount, byte scanCode, bool isExtendedKey, bool previousKeyState)
{
	switch(characterCode)
	{
	case VK_TAB:
		{
		}
		break;
	case VK_PRIOR:
		{
		}
		break;
	case VK_NEXT:
		{
		}
		break;
	case VK_END:
		{
			uint32 lineNumber;

			lineNumber = this->pDoc->GetLineNumber(this->caretOffset);
			if(lineNumber+1 == this->pDoc->GetNumberOfLines())
			{
				this->caretOffset = this->pDoc->GetNumberOfChars()-1;
			}
			else
			{
				this->caretOffset = this->pDoc->GetLineOffset(lineNumber + 1) - 1;
			}
		}
		break;
	case VK_HOME:
		{
			if(this->caretOffset)
			{
				this->caretOffset = this->pDoc->GetLineOffset(this->pDoc->GetLineNumber(this->caretOffset));
			}
		}
		break;
	case VK_LEFT:
		{
			if(this->caretOffset)
			{
				this->caretOffset--;
			}
		}
		break;
	case VK_UP:
		{
		}
		break;
	case VK_RIGHT:
		{
			if(this->caretOffset < this->pDoc->GetNumberOfChars()-1)
			{
				this->caretOffset++;
			}
		}
		break;
	case VK_DOWN:
		{
		}
		break;
	default:
		return;
	}

	this->MoveCaret();
}

void CTextView::OnLeftMouseButtonDown(uint16 keys, uint16 x, uint16 y)
{
	this->caretOffset = this->CoordToOffset(x, y);
	this->MoveCaret();

	this->selecting = true;
	this->SetCapture();
}

void CTextView::OnLeftMouseButtonUp(uint16 keys, uint16 x, uint16 y)
{
	this->selecting = false;

	if(this->isScrollTimerActive)
	{
		this->KillTimer(CTEXTVIEW_TIMER_SCROLL_ID);
		this->isScrollTimerActive = false;
	}
	ReleaseCapture();
}

void CTextView::OnLooseFocus(CWindow *pUpcomingFocusedWindow)
{
	this->ShowCaret(false);
	DestroyCaret();
}

void CTextView::OnMouseMove(uint16 keys, int16 x, int16 y)
{
	if(this->selecting)
	{
		POINT pt;
		CRect rcClient;

		pt.x = x;
		pt.y = y;
		this->GetClientRect(rcClient);

		//Clip coordinates
		if(x < this->GetLeftMarginWidth())
			x = this->GetLeftMarginWidth();
		if(y < 0)
			y = 0;
		if(x > (int16)rcClient.GetWidth())
			x = rcClient.GetWidth();
		if(y > (int16)rcClient.GetHeight())
			y = rcClient.GetHeight();

		//Check if the mouse is outside of the view
		if(rcClient.PointInRect(pt))
		{
			//...Mouse is inside the window
			if(this->isScrollTimerActive)
			{
				this->KillTimer(CTEXTVIEW_TIMER_SCROLL_ID);
				this->isScrollTimerActive = false;
			}
		}
		else
		{
			//...Mouse is outside the window
			if(!this->isScrollTimerActive)
			{
				this->SetTimer(CTEXTVIEW_TIMER_SCROLL_ID, CTEXTVIEW_TIMER_SCROLL_TIMEOUT);
				this->isScrollTimerActive = true;
			}
		}
		
		this->caretOffset = this->CoordToOffset(x, y);
		this->InvalidateRange(this->selectionEnd, this->caretOffset);
		this->selectionEnd = this->caretOffset;
		
		this->RepositionCaret();
	}
}

void CTextView::OnMouseWheel(int16 zDelta, uint16 keys)
{
	uint32 nScrollLines;

	SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &nScrollLines, 0);
	
	if(nScrollLines <= 1)
		nScrollLines = 3;
	
	this->Scroll(0, (-zDelta / 120) * nScrollLines);
	this->RepositionCaret();
}

void CTextView::OnPaint()
{
	uint32 i, firstLine, lastLine;
	CRect rcClient, rcPaint;
	CPointer<CDeviceContext> pMemDC;
	CPointer<CBitmap> pBmp;
	CPaintDC dc(this);

	this->GetClientRect(rcClient);
	
	//Get the area we need to draw
	dc.GetPaintRect(rcPaint);

	//Create a backbuffer, which we can render to
	pMemDC = dc.CreateCompatibleDC();
	pBmp = dc.CreateCompatibleBitmap(rcClient.GetWidth(), rcClient.GetHeight()); //This must be created from paint dc!
	pMemDC->SelectObject(pBmp);
	
	//Check which lines we need to draw
	firstLine = this->GetScrollPos() + (rcPaint.top / this->lineHeight);
	lastLine = this->GetScrollPos() + (rcPaint.bottom / this->lineHeight);
	
	//Draw lines
	for(i = firstLine; i <= lastLine; i++)
	{
		this->DrawLine(i, *pMemDC);
	}

	//Present the backbuffer
	dc.BitBlt(rcPaint.left, rcPaint.top, rcPaint.GetWidth(), rcPaint.GetHeight(), pMemDC, rcPaint.left, rcPaint.top, SRCCOPY);
}

bool CTextView::OnRequestCursor(const CWindow *pContainingWindow, uint16 hitTest, uint16 msgId)
{
	if(hitTest == HTCLIENT)
	{
		CCursor cursor;
		
		cursor.LoadSystemCursor(IDC_IBEAM);
		cursor.SetAsActive();
		
		return true;
	}
	
	CWindow::RespondDefault();
	return false;
}

void CTextView::OnSize(WPARAM resizingType, uint32 newWidth, uint32 newHeight)
{
	this->linesToDisplay = MIN(newHeight / this->lineHeight, this->pDoc->GetNumberOfLines());
	this->maxColumnsToDisplay = MIN(newWidth / this->minAverageFontWidth, this->pDoc->GetLongestLineLength());

	this->UpdateScrollBars();
}

void CTextView::OnTimerExpires(uint32 timerId)
{
	switch(timerId)
	{
	case CTEXTVIEW_TIMER_SCROLL_ID:
		{
			int16 distX, distY, scrollX, scrollY;
			POINT pt;
			CRect rcClient;
			
			GetCursorPos(&pt);
			this->ScreenToClient(pt);
			this->GetClientRect(rcClient);

			if(!rcClient.PointInRect(pt))
			{
				if(pt.x > (int32)rcClient.GetWidth())
				{
					distX = (int16)(pt.x - rcClient.GetWidth());
					scrollX = 1;
				}
				else if(pt.x < 0)
				{
					distX = (int16)pt.x;
					scrollX = -1;
				}
				else
				{
					distX = 0;
					scrollX = 0;
				}
				
				if(pt.y > (int32)rcClient.GetHeight())
				{
					distY = (int16)(pt.y - rcClient.GetHeight());
					scrollY = 1;
				}
				else if(pt.y < 0)
				{
					distY = (int16)pt.y;
					scrollY = -1;
				}
				else
				{
					distY = 0;
					scrollY = 0;
				}
				
				if(ABS(distX) > 10)
					scrollX = distX / 10;
				if(ABS(distY) > 10)
					scrollY = distY / 10;
				
				this->Scroll(scrollX, scrollY);
				this->OnMouseMove(0, (int16)pt.x, (int16)pt.y);
			}
		}
		break;
	}
}

void CTextView::OnVerticalScroll(uint16 scrollCode)
{
	uint32 oldPos;

	oldPos = this->vertScrollPos;
	
	switch(scrollCode)
	{
	case SB_LINEUP:
		this->Scroll(0, -1);
		break;
	case SB_LINEDOWN:
		this->Scroll(0, 1);
		break;
	case SB_PAGEUP:
		this->Scroll(0, -this->linesToDisplay);
		break;
	case SB_PAGEDOWN:
		this->Scroll(0, this->linesToDisplay);
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		{
			this->vertScrollPos = this->GetScrollTrackPos();
			this->RefreshWindow();
		}
		break;
	case SB_TOP:
		{
			this->vertScrollPos = 0;
			this->RefreshWindow();
		}
		break;
	case SB_BOTTOM:
		{
			this->vertScrollPos = this->vertScrollMax;
			this->RefreshWindow();
		}
		break;
	}

	if(oldPos != this->vertScrollPos)
	{
		this->UpdateScrollBars();
		this->RepositionCaret();
	}
}

//Private Functions
uint16 CTextView::ApplyTextAttributes(uint32 docOffset, STextAttribute &refTextAttribute)
{
	uint16 textLength;
	uint32 selStart, selEnd;

	this->GetNormalizedSelection(selStart, selEnd);
	
	//Maximum length
	textLength = 0xFFFF;
	
	//Set default colors first
	refTextAttribute.background = this->backgroundColor;
	refTextAttribute.fontDataIndex = CTEXTVIEW_DEFAULTFONTINDEX;
	refTextAttribute.foreground = this->textColor;

	//debugging
	if(docOffset > 2000)
	{
		CFont font;
		font.LoadSystemFont(ANSI_FIXED_FONT);
		this->SetFont(1, font);
		refTextAttribute.fontDataIndex = 1;
	}
	
	//Ask user!
	
	//Selection is more important than everything else
	if(selStart != selEnd && docOffset < selStart && docOffset + textLength > selStart)
	{
		//Break earlier, because there is a selection
		textLength = selStart - docOffset;
	}
	else if(selStart != selEnd && INRANGE(docOffset, selStart, selEnd))
	{
		//We are at the start of a selection!
		textLength = selEnd - docOffset;
		refTextAttribute.background = this->selectionBackgroundColor;
		//refTextAttribute.foreground = this->selectionTextColor;
	}
	
	return textLength;
}

uint32 CTextView::CoordToOffset(uint32 x, uint32 y)
{
	uint16 length;
	uint32 lineNumber, textOffset, lineOffset, x1, x2, i, lastLineOffset, width;
	STextAttribute textAttribute;
	CString lineText, part;
	CPointer<CDeviceContext> pDC;

	x -= this->GetLeftMarginWidth();
	x += this->horzScrollPos * this->minAverageFontWidth;

	x1 = 0;
	x2 = 0;
	lastLineOffset = 0;

	pDC = this->GetDC();
	
	lineNumber = this->vertScrollPos + y / this->lineHeight;
	textOffset = this->pDoc->GetLine(lineNumber, lineText);

	for(lineOffset = 0; lineOffset < lineText.GetLength();)
	{
		length = this->ApplyTextAttributes(textOffset + lineOffset, textAttribute);
		
		//Check text attribute
		ASSERT(textAttribute.fontDataIndex < CTEXTVIEW_MAXFONTS);
		ASSERT(this->fonts[textAttribute.fontDataIndex].isValid);
		
		//Make sure length does not overflow
		if(length > lineText.GetLength() - lineOffset)
		{
			length = lineText.GetLength() - lineOffset;
		}
		
		//Set text attributes
		pDC->SelectObject(this->fonts[textAttribute.fontDataIndex].font);

		//Search until we got the right portion
		i = 0;
		while(x2 < x)
		{
			x1 = x2;
			lastLineOffset = lineOffset + i;
			i += this->GetTextPortionInfo(lineText, lineOffset + i, width, textAttribute, *pDC);
			x2 += width;
			if(width == 0)
				break;
		}
		if(x2 >= x)
			break;
		
		//Update
		lineOffset += length;
	}
	
	if(x2 < x)
	{
		//User has clicked beyond the text
		return this->pDoc->GetLineOffset(lineNumber+1) - 1;
	}

	//We have the correct portion... search for the correct char now
	x -= x1;

	if((uint8)lineText[lastLineOffset] < 32) //also includes tab
	{
		this->GetTextPortionInfo(lineText, lastLineOffset, x1, textAttribute, *pDC);

		if(x > x1 / 2)
			return this->pDoc->GetLineOffset(lineNumber) + lastLineOffset + 1;
		return this->pDoc->GetLineOffset(lineNumber) + lastLineOffset;
	}
	
	part = lineText.SubString(lastLineOffset, lineText.GetLength() - lastLineOffset);
	for(uint32 i = 1; i < part.GetLength(); i++)
	{
		x1 = x2;
		this->GetTextPortionInfo(part.SubString(0, i), 0, x2, textAttribute, *pDC);
		
		if(x2 >= x)
		{
			ASSERT(i);
			
			if(x - x1 <= x2 - x)
				return this->pDoc->GetLineOffset(lineNumber) + lastLineOffset + i - 1;
			return this->pDoc->GetLineOffset(lineNumber) + lastLineOffset + i;
		}
	}

	return 0;
}

void CTextView::CreateCaret(uint16 height)
{
	uint32 width;
	
	SystemParametersInfo(SPI_GETCARETWIDTH, 0, &width, 0);
	this->CreateSolidCaret(width, height);

	this->isCaretVisible = false;
}

void CTextView::DrawLeftMargin(uint32 lineNumber, CDeviceContext &refDC)
{
	SIZE size;
	CRect rcMargin;
	CString text;
	
	rcMargin.top = (lineNumber - this->GetScrollPos()) * this->lineHeight;
	rcMargin.bottom = rcMargin.top + this->lineHeight;
	rcMargin.right = this->GetLeftMarginWidth();
	
	//Paint the background
	refDC.FillSolidRect(rcMargin, this->backgroundColor);

	if(this->showLineNumbers && lineNumber < this->pDoc->GetNumberOfLines())
	{
		rcMargin.right -= this->leftMarginWidth;
		
		text = CString(lineNumber + 1);
		
		refDC.SelectObject(this->fonts[CTEXTVIEW_DEFAULTFONTINDEX].font);
		refDC.SetBackgroundColor(this->backgroundColor);
		refDC.SetTextColor(this->textColor);

		size = refDC.GetTextExtentPoint32A(text);
		
		refDC.TextOutA(rcMargin.right - size.cx, rcMargin.top + this->lineSpacingUpper + this->maxAscent - this->fonts[CTEXTVIEW_DEFAULTFONTINDEX].ascent, text);
	}
}

void CTextView::DrawLine(uint32 lineNumber, CDeviceContext &refDC)
{
	CRect rcLine;
	
	//Set the area where we want to draw
	this->GetClientRect(rcLine);

	rcLine.left = -(int32)this->GetScrollPos(false) * this->minAverageFontWidth + this->GetLeftMarginWidth();
	rcLine.top = (lineNumber - this->GetScrollPos()) * this->lineHeight;
	rcLine.bottom = rcLine.top + this->lineHeight;

	//Paint the background
	refDC.FillSolidRect(rcLine, this->backgroundColor);
	
	//Draw text only if there is some	
	if(lineNumber < this->pDoc->GetNumberOfLines())
	{
		this->DrawLineText(lineNumber, rcLine, refDC);
	}
	this->DrawLeftMargin(lineNumber, refDC);
}

void CTextView::DrawLineText(uint32 lineNumber, CRect &refRcLine, CDeviceContext &refDC)
{
	uint16 length;
	int32 x;
	uint32 offset, textOffset;
	STextAttribute textAttribute;
	CString lineText;
	
	//Set initial values
	x = refRcLine.left;
	offset = 0;
	
	//Get the line text
	textOffset = this->pDoc->GetLine(lineNumber, lineText);
	
	//Keep drawing until we reach the end of the window
	while(x < refRcLine.right && offset < lineText.GetLength())
	{
		length = this->ApplyTextAttributes(textOffset + offset, textAttribute);

		//Check text attribute
		ASSERT(textAttribute.fontDataIndex < CTEXTVIEW_MAXFONTS);
		ASSERT(this->fonts[textAttribute.fontDataIndex].isValid);
		
		//Make sure length does not overflow
		if(length > lineText.GetLength() - offset)
		{
			length = lineText.GetLength() - offset;
		}
		
		//Set text attributes
		refDC.SelectObject(this->fonts[textAttribute.fontDataIndex].font);
		refDC.SetBackgroundColor(textAttribute.background);
		refDC.SetTextColor(textAttribute.foreground);

		//Render the text
		x += this->DrawTextA(x, refRcLine.top, lineText.SubString(offset, length), textAttribute, refDC);
		
		//Update
		offset += length;
	}
	
	//Fill the rest of the line with the background color
	refRcLine.left = x;
	refDC.FillSolidRect(refRcLine, this->backgroundColor);
}

uint16 CTextView::DrawText(uint32 x, uint32 y, const CString &refText, const STextAttribute &refTextAttribute, CDeviceContext &refDC)
{
	uint16 nTabChars, fontHeight;
	uint32 xStart, spacingTop, offset;
	SIZE size;
	CRect rc;
	CString drawText;
	
	nTabChars = CTEXTVIEW_NTABCHARS;
	fontHeight = this->fonts[refTextAttribute.fontDataIndex].charHeight + this->fonts[refTextAttribute.fontDataIndex].externalLeading;
	xStart = x;
	spacingTop = this->maxAscent + this->lineSpacingUpper - this->fonts[refTextAttribute.fontDataIndex].ascent;
	offset = 0;
	rc.Init(x, y, 0, this->lineHeight);

	for(uint32 i = 0; i <= refText.GetLength(); i++)
	{
		//Flush text
		if(i == refText.GetLength() || refText[i] == '\t' || (byte)refText[i] < 32)
		{
			drawText = refText.SubString(offset, i - offset);
			size = refDC.GetTextExtentPoint32A(drawText);
			rc.left = x;
			rc.SetWidth(size.cx);
			
			refDC.TextOutA(x, y + spacingTop, ETO_OPAQUE, rc, drawText);
			
			x += size.cx;
			offset = i+1;
		}

		if(i < refText.GetLength())
		{
			if(refText[i] == '\t')
			{
				//draw a tab
				rc.left = x;
				rc.SetWidth(this->fonts[refTextAttribute.fontDataIndex].averageCharWidth * nTabChars);
				refDC.TextOutA(x, y + spacingTop, ETO_OPAQUE, rc, CString());
				
				x += rc.GetWidth();

				nTabChars = CTEXTVIEW_NTABCHARS + 1;
			}
			else if((byte)refText[i] < 32)
			{
				//draw a control char
				drawText = CString((uint32)refText[i], NS_DEC, false, 2);
				size = refDC.GetTextExtentPoint32A(drawText);
				
				//swap foreground and background color
				refDC.SetBackgroundMode(false);
				refDC.SetTextColor(refTextAttribute.background);
				
				//draw a "rounded" rectangle
				rc.Init(x + 1, y + spacingTop + 1, size.cx + 4, fontHeight - 2);
				refDC.FillSolidRect(rc, refTextAttribute.foreground);
				rc.Init(x + 2, y + spacingTop, size.cx + 2, fontHeight);
				refDC.FillSolidRect(rc, refTextAttribute.foreground);

				//draw the text
				rc.left = x + 3;
				rc.SetWidth(size.cx);
				
				refDC.TextOutA(x + 3, y + spacingTop, 0, rc, drawText);
				
				//reset colors
				refDC.SetBackgroundMode(true);
				refDC.SetTextColor(refTextAttribute.foreground);

				//one pixel background spacing, 2 inside the box
				x += 3 + size.cx + 3;
			}
		}

		nTabChars--;
		if(nTabChars == 1)
			nTabChars = CTEXTVIEW_NTABCHARS;
	}

	return x - xStart;
}

uint16 CTextView::GetLeftMarginWidth() const
{
	if(this->showLineNumbers)
	{
		return this->leftMarginWidth + (CString(this->pDoc->GetNumberOfLines()).GetLength() + 1) * this->fonts[CTEXTVIEW_DEFAULTFONTINDEX].averageCharWidth;
	}

	return this->leftMarginWidth;
}

uint32 CTextView::GetTextPortionInfo(const CString &refString, uint32 lineOffset, uint32 &refWidth, const STextAttribute &refTextAttribute, const CDeviceContext &refDC)
{
	SIZE size;
	CString drawText;

	refWidth = 0;
	
	if(refString.IsEmpty())
		return 0;
	
	if(refString[lineOffset] == '\t')
	{
		refWidth = this->fonts[refTextAttribute.fontDataIndex].averageCharWidth * (CTEXTVIEW_NTABCHARS - (lineOffset-1) % CTEXTVIEW_NTABCHARS);
		return 1;
	}

	if((byte)refString[lineOffset] < 32)
	{
		drawText = CString((uint32)refString[lineOffset], NS_DEC, false, 2);
		size = refDC.GetTextExtentPoint32A(drawText);
		
		refWidth = 3 + size.cx + 3;
		return 1;
	}
	
	for(uint32 i = lineOffset; i <= refString.GetLength(); i++)
	{
		if(i == refString.GetLength() || refString[i] == '\t' || (byte)refString[i] < 32)
		{
			drawText = refString.SubString(lineOffset, i - lineOffset);
			size = refDC.GetTextExtentPoint32A(drawText);

			refWidth = size.cx;

			return i - lineOffset;
		}
	}
	
	return 0;
}

void CTextView::InvalidateLine(uint32 lineNumber)
{
	CRect rcLine;

	this->GetClientRect(rcLine);
	
	rcLine.top = (lineNumber - this->GetScrollPos()) * this->lineHeight;
	rcLine.bottom = rcLine.top + this->lineHeight;

	this->InvalidateRect(rcLine, false);
}

void CTextView::InvalidateRange(uint32 startOffset, uint32 endOffset)
{
	uint32 tmp, startLine, endLine;

	if(endOffset < startOffset)
	{
		tmp = startOffset;
		startOffset = endOffset;
		endOffset = tmp;
	}
	
	ASSERT(startOffset <= endOffset);

	startLine = this->pDoc->GetLineNumber(startOffset);
	endLine = this->pDoc->GetLineNumber(endOffset);

	for(uint32 i = startLine; i <= endLine; i++)
	{
		this->InvalidateLine(i);
	}
}

void CTextView::MoveCaret()
{
	if(this->IsKeyPressed(VK_SHIFT)) //extend selection
	{
		this->InvalidateRange(this->selectionEnd, this->caretOffset);
		this->selectionEnd = this->caretOffset;
	}
	else //clear selection
	{
		this->InvalidateRange(this->selectionStart, this->selectionEnd);
		this->selectionStart = this->caretOffset;
		this->selectionEnd = this->caretOffset;
	}

	this->RepositionCaret();
}

bool CTextView::OffsetToCoord(uint32 offset, uint32 &refX, uint32 &refY)
{
	uint16 length;
	uint32 lineNumber, lineOffset, textOffset, i, j, width;
	STextAttribute textAttribute;
	CString lineText, cutText;
	CPointer<CDeviceContext> pDC;

	pDC = this->GetDC();
	
	lineNumber = this->pDoc->GetLineNumber(offset);
	
	if(lineNumber >= this->vertScrollPos && lineNumber <= this->vertScrollPos + this->linesToDisplay)
	{
		lineOffset = offset - this->pDoc->GetLineOffset(lineNumber);
		textOffset = this->pDoc->GetLine(lineNumber, lineText);
		cutText = lineText.SubString(0, lineOffset);

		refX = 0;
		refY = (lineNumber - this->vertScrollPos) * this->lineHeight;

		if(lineOffset == 0 && this->horzScrollPos == 0)
		{
			refX += this->GetLeftMarginWidth();
			return true;
		}
		
		i = 0;
		while(i < cutText.GetLength())
		{
			length = this->ApplyTextAttributes(textOffset + i, textAttribute);
			
			//Check text attribute
			ASSERT(textAttribute.fontDataIndex < CTEXTVIEW_MAXFONTS);
			ASSERT(this->fonts[textAttribute.fontDataIndex].isValid);

			//Make sure length does not overflow
			if(length > lineText.GetLength() - i)
			{
				length = lineText.GetLength() - i;
			}

			//Set text attributes
			pDC->SelectObject(this->fonts[textAttribute.fontDataIndex].font);

			j = i;
			while(j += this->GetTextPortionInfo(cutText, j, width, textAttribute, *pDC))
			{
				refX += width;
				
				if(j >= cutText.GetLength())
				{
					if(refX < this->horzScrollPos * this->minAverageFontWidth || refX > (this->horzScrollPos + this->maxColumnsToDisplay) * this->minAverageFontWidth)
						return false;
					refX -= this->horzScrollPos * this->minAverageFontWidth;
					refX += this->GetLeftMarginWidth();
					
					return true;
				}
			}

			//Update
			i += length;
		}
	}
	
	return false;
}

void CTextView::RecalcFontValues()
{
	uint16 nCheckedFonts, i, currentCharHeight;

	this->lineHeight = 0;
	this->maxAscent = 0;
	this->minAverageFontWidth = 0xFFFF;

	nCheckedFonts = 0;
	i = 0;
	while(nCheckedFonts < this->nFonts)
	{
		if(this->fonts[i].isValid)
		{
			currentCharHeight = this->fonts[i].charHeight + this->fonts[i].externalLeading;

			if(currentCharHeight > this->lineHeight)
				this->lineHeight = currentCharHeight;
			if(this->fonts[i].ascent > this->maxAscent)
				this->maxAscent = this->fonts[i].ascent;
			if(this->fonts[i].averageCharWidth < this->minAverageFontWidth)
				this->minAverageFontWidth = this->fonts[i].averageCharWidth;
			
			nCheckedFonts++;
		}
		i++;
	}

	//add the spacings to the line height
	this->lineHeight += this->lineSpacingLower + this->lineSpacingUpper;
}

void CTextView::RefreshWindow()
{
	this->Invalidate(false);
}

void CTextView::RepositionCaret()
{
	uint32 x, y;
	
	if(this->OffsetToCoord(this->caretOffset, x, y))
	{
		SetCaretPos(x, y);
		this->ShowCaret();
	}
	else
	{
		this->ShowCaret(false);
	}
}

void CTextView::Scroll(int32 dx, int32 dy)
{
	CRect rcClip, rcUpdate;

	this->GetClientRect(rcClip);
	rcUpdate = rcClip;
	
	//scroll up
	if(dy < 0)
	{
		dy = -(int32)MIN((uint32)-dy, this->vertScrollPos);
		rcClip.top = -dy * this->lineHeight;
	}
	//scroll down
	else if(dy > 0)
	{
		dy = MIN((uint32)dy, this->vertScrollMax - this->vertScrollPos);
		rcClip.bottom = (this->linesToDisplay - dy) * this->lineHeight;
	}
	
	//scroll left
	if(dx < 0)
	{
		dx = -(int32)MIN((uint32)-dx, this->horzScrollPos);
		rcClip.left = -dx * this->minAverageFontWidth * 4;
	}
	//scroll right
	else if(dx > 0)
	{
		dx = MIN((uint32)dx, this->horzScrollMax - this->horzScrollPos);
		rcClip.right = (this->maxColumnsToDisplay - dx - 4) * this->minAverageFontWidth;
	}

	rcClip.left += this->GetLeftMarginWidth();

	//Check if we need to scroll
	if(dx || dy)
	{
		//adjust the scrollbar positions
		this->horzScrollPos += dx;
		this->vertScrollPos += dy;

		CWindow::Scroll(-dx * this->minAverageFontWidth, -dy * this->lineHeight, NULL, &rcClip);

		rcUpdate.Subtract(rcClip);
		this->InvalidateRect(rcUpdate);
		
		this->UpdateScrollBars();
	}
}

void CTextView::ShowCaret(bool show)
{
	if(show && !this->isCaretVisible)
	{
		CWindow::ShowCaret(true);
		this->isCaretVisible = true;
	}
	else if(!show && this->isCaretVisible)
	{
		CWindow::ShowCaret(false);
		this->isCaretVisible = false;
	}
}

void CTextView::UpdateScrollBars()
{
	//Vertical bar
	this->SetScrollPos(this->vertScrollPos);
	this->SetScrollPageSize(this->linesToDisplay);
	this->SetScrollRange(this->pDoc->GetNumberOfLines() - 1);

	//Horizontal bar
	this->SetScrollPos(this->horzScrollPos, false);
	this->SetScrollPageSize(this->maxColumnsToDisplay, false);
	this->SetScrollRange(this->pDoc->GetLongestLineLength() - 1, false);

	//Recalculate max values
	this->vertScrollMax = this->pDoc->GetNumberOfLines() - this->linesToDisplay;
	this->horzScrollMax = this->pDoc->GetLongestLineLength() - this->maxColumnsToDisplay;
}

//Public Functions
void CTextView::Create(const CWindow &refParentWindow)
{
	CFont defFont;
	
	CWindow::Create(WS_HSCROLL | WS_VSCROLL, WS_EX_CLIENTEDGE, refParentWindow);
	
	//Set a default font
	defFont.LoadSystemFont(DEFAULT_GUI_FONT);
	this->SetFont(CTEXTVIEW_DEFAULTFONTINDEX, defFont);
}

void CTextView::SetFont(uint16 idx, const CFont &refFont)
{
	TEXTMETRIC tm;
	CPointer<CDeviceContext> pDC;
	
	ASSERT(idx < CTEXTVIEW_MAXFONTS);

	SFontData &refFontData = this->fonts[idx];

	if(!refFontData.isValid)
	{
		//if this index was not in use before, we add a new font
		this->nFonts++;
	}
	
	refFontData.isValid = true;
	refFontData.font = refFont;

	//Get metrics for the font
	pDC = this->GetDC();
	pDC->SelectObject(refFont);
	pDC->GetTextMetricsA(&tm);
	
	refFontData.ascent = (uint16)tm.tmAscent;
	refFontData.averageCharWidth = (uint16)tm.tmAveCharWidth;
	refFontData.charHeight = (uint16)tm.tmHeight;
	refFontData.externalLeading = (uint16)tm.tmExternalLeading;
	
	//this might be a larger font, so recalc the line height
	this->RecalcFontValues();
}

/*//Constructor
CTextView::CTextView(ITextDocument *pDoc)
{
	//Display Variables
	this->selectionStart = 0;
	this->selectionEnd = 0;

	//Font Variables
	this->maxAscent = 0;
	this->fontWidth = 0;
	
	//Scrollbar Variables
	this->horzScrollPos = 0;
	this->horzScrollMax = 0;
	this->vertScrollPos = 0;
	this->vertScrollMax = 0;

	//Layout Variables
	this->linesToDisplay = 0;
	this->columnsToDisplay = 0;
}

//Callbacks
void CTextView::OnLeftMouseButtonDown(uint16 keys, uint16 x, uint16 y)
{
	this->CoordToTextOffset(x, y);
}

uint8 CTextView::OnMouseActivate(CPopupWindow *pActivatedWnd, uint16 hitTest, uint16 mouseMessage)
{
	if(hitTest == HTCLIENT)
	{
		this->Focus();
		return MA_ACTIVATE;
	}
	return MA_NOACTIVATE;
}

void CTextView::OnSetFont(const CFont &refFont)
{
	this->SetFont(CTEXTVIEW_DEFAULTFONTINDEX, refFont);

	this->fontWidth = this->fonts[CTEXTVIEW_DEFAULTFONTINDEX].charWidth;
	this->UpdateMetrics();
}

void CTextView::OnSize(WPARAM resizingType, uint32 newWidth, uint32 newHeight)
{
	if(this->PinToBottomCorner())
	{
		this->RefreshWindow();
		this->RepositionCaret();
	}

	this->UpdateScrollBars();
}

//Private Functions
void CTextView::CoordToTextOffset(uint16 x, uint16 y) const
{
	uint32 lineNumber, offset;

	lineNumber = this->vertScrollPos + (y / this->lineHeight);

	if(lineNumber >= this->pDoc->GetNumberOfLines())
	{
		lineNumber = this->pDoc->GetNumberOfLines() ? this->pDoc->GetNumberOfLines() - 1 : 0;
		offset = this->pDoc->GetNumberOfChars();
	}

	x += this->horzScrollPos * this->fontWidth;
}

void CTextView::DrawLine(uint32 lineNumber, CDeviceContext &refDC)
{
	CRect rcClient, rcText;

	this->GetClientRect(rcClient);

	//Fill out text rectangle
	rcText.left = -(int32)this->horzScrollPos * this->fontWidth;
	rcText.top = (lineNumber - this->vertScrollPos) * this->lineHeight;
	rcText.right = rcClient.right;
	rcText.bottom = rcText.top + this->lineHeight;

	//Check if there is text in this line
	if(lineNumber >= this->pDoc->GetNumberOfLines())
	{
		refDC.FillSolidRect(rcText, this->backgroundColor);
		return;
	}
	
	//Draw the text
	this->DrawLineText(lineNumber, rcText, refDC);
}

bool CTextView::PinToBottomCorner()
{
	bool repos;
	
	repos = false;
	if(this->horzScrollPos + this->columnsToDisplay > this->pDoc->GetLongestLineLength())
	{
		this->horzScrollPos = this->pDoc->GetLongestLineLength() - this->columnsToDisplay;
		repos = true;
	}
	
	if(this->vertScrollPos + this->linesToDisplay > this->pDoc->GetNumberOfLines())
	{
		this->vertScrollPos = this->pDoc->GetNumberOfLines() - this->linesToDisplay;
		repos = true;
	}
	
	return repos;
}

void CTextView::RecalcLineHeight()
{
	uint16 currentFontHeight;
	
	this->lineHeight = 0;
	this->maxAscent = 0;

	repeat(this->nFonts, i)
	{
		currentFontHeight = this->fonts[i].charHeight + this->fonts[i].externalLeading;

		this->lineHeight = MAX(this->lineHeight, currentFontHeight);
		this->maxAscent = MAX(this->maxAscent, this->fonts[i].ascent);
	}

	//Add spacings
	this->lineHeight += this->lineSpacingLower + this->lineSpacingUpper;

	if(CWindow::GetFocusedWindow() == this)
	{
		this->Focus();
	}
}

void CTextView::UpdateMetrics()
{
	CRect rcClient;

	this->GetClientRect(rcClient);
	this->OnSize(0, rcClient.right, rcClient.bottom);
	this->RefreshWindow();
}
*/