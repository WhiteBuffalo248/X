#include "stdafx.h"
#include "RichTextPainter.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//TODO: fix performance (possibly use QTextLayout?)
void RichTextPainter::paintRichText(CDC* painter, int x, int y, int w, int h, int xinc, const List & richText, LOGFONT* fontMetrics, bool bAlign)
{
	
	CPen highlightPen;
	CRect corRect;
	corRect.left = x;
	corRect.top = y;
	corRect.right = corRect.left + w;
	corRect.bottom = corRect.top + h;
	
	
	TEXTMETRIC tm;
	painter->GetTextMetrics(&tm);
	int charWidth = tm.tmAveCharWidth;

	for (const CustomRichText_t & curRichText : richText)
	{
		int textWidth = painter->GetTextExtent(curRichText.text).cx;
		int textAlignWidth = charWidth * curRichText.text.GetLength();

		int backgroundWidth = textWidth;
		if (backgroundWidth + xinc > w)
			backgroundWidth = w - xinc;
		if (backgroundWidth <= 0) //stop drawing when going outside the specified width
			break;
		switch (curRichText.flags)
		{
		case FlagNone: //defaults
			break;
		case FlagColor: //color only
			/*
			pen.DeleteObject();
			pen.CreatePen(PS_SOLID, 2,curRichText.textColor.ToCOLORREF());
			painter->.SelectObject(&pen);
			*/
			painter->SetTextColor(curRichText.textColor.ToCOLORREF());
			break;
		case FlagBackground: //background only
			if (backgroundWidth > 0 && curRichText.textBackground.GetAlpha())
			{
				//brush.setColor(curRichText.textBackground);
				//painter->fillRect(QRect(x + xinc, y, backgroundWidth, h), brush);
				CRect backRect = corRect;
				backRect.left = x + xinc;				
				backRect.right = backRect.left + backgroundWidth;
				painter->FillSolidRect(&backRect, curRichText.textBackground.ToCOLORREF());
			}
			break;
		case FlagAll: //color+background
			if (backgroundWidth > 0 && curRichText.textBackground.GetAlpha())
			{
				//brush.setColor(curRichText.textBackground);
				//painter->fillRect(QRect(x + xinc, y, backgroundWidth, h), brush);
				CRect backRect = corRect;
				backRect.left = x + xinc;
				backRect.right = backRect.left + backgroundWidth;
				painter->FillSolidRect(&backRect, curRichText.textBackground.ToCOLORREF());
			}
			// pen对DrawText 字体颜色无效
			/*
			pen.DeleteObject();
			pen.CreatePen(PS_SOLID, 2, curRichText.textColor.ToCOLORREF());
			painter->SelectObject(&pen);
			*/
			painter->SetTextColor(curRichText.textColor.ToCOLORREF());
			break;
		}
		//painter->drawText(QRect(x + xinc, y, w - xinc, h), Qt::TextBypassShaping, curRichText.text)
		CRect textRect = corRect;
		textRect.left += xinc;
		textRect.right = textRect.left + textWidth;
		painter->DrawText(curRichText.text, &textRect, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
		if (curRichText.highlight && curRichText.highlightColor.GetAlpha())
		{
			//highlightPen.setColor(curRichText.highlightColor);
			//highlightPen.setWidth(curRichText.highlightWidth);
			if (!highlightPen.GetSafeHandle())
			{
				//painter->setPen(highlightPen);
				highlightPen.CreatePen(PS_SOLID, 1, curRichText.highlightColor.ToCOLORREF());				
				painter->SelectObject(&highlightPen);
			}
			
			int highlightOffsetX = curRichText.highlightConnectPrev ? -1 : 1;
			//painter->drawLine(x + xinc + highlightOffsetX, y + h - 1, x + xinc + backgroundWidth - 1, y + h - 1);
			painter->MoveTo(x + xinc + highlightOffsetX, y + h - 2);
			painter->LineTo(x + xinc + backgroundWidth, y + h - 2);
		}
		if (bAlign)
			xinc += textAlignWidth;
		else
			xinc += textWidth;
	}
}

/**
* @brief RichTextPainter::htmlRichText Convert rich text in x64dbg to HTML, for use by other applications
* @param richText The rich text to be converted to HTML format
* @param textHtml The HTML source. Any previous content will be preserved and new content will be appended at the end.
* @param textPlain The plain text. Any previous content will be preserved and new content will be appended at the end.
*/
void RichTextPainter::htmlRichText(const List & richText, CString & textHtml, CString & textPlain)
{
	for (const CustomRichText_t & curRichText : richText)
	{
		if (curRichText.text == " ") //blank
		{
			textHtml += " ";
			textPlain += " ";
			continue;
		}
		switch (curRichText.flags)
		{
		case FlagNone: //defaults
			textHtml += "<span>";
			break;
		case FlagColor: //color only
			textHtml.AppendFormat(_T("<span style=\"color:0x%08X\">"), curRichText.textColor);
			break;
		case FlagBackground: //background only
			if (!curRichText.textBackground.GetAlpha()) // QColor::name() returns "#000000" for transparent color. That's not desired. Leave it blank.
				textHtml.AppendFormat(_T("<span style=\"background-color:0x%08X\">"), curRichText.textBackground);
			else
				textHtml += "<span>";
			break;
		case FlagAll: //color+background
			if (curRichText.textBackground.GetAlpha()) // QColor::name() returns "#000000" for transparent color. That's not desired. Leave it blank.
				textHtml.AppendFormat(_T("<span style=\"color:0x%08X; background-color:0x%08X\">"), curRichText.textColor, curRichText.textBackground);
			else
				textHtml.AppendFormat(_T("<span style=\"color:0x%08X\">"), curRichText.textColor);			
			break;
		}
		if (curRichText.highlight) //Underline highlighted token
			textHtml += "<u>";
		//textHtml += curRichText.text.toHtmlEscaped();
		textHtml += curRichText.text;
		if (curRichText.highlight)
			textHtml += "</u>";
		textHtml += "</span>"; //Close the tag
		textPlain += curRichText.text;
	}
}