#ifndef RICHTEXTPAINTER_H
#define RICHTEXTPAINTER_H

#include <vector>

using namespace Gdiplus;

class RichTextPainter
{
public:
	//structures
	enum CustomRichTextFlags
	{
		FlagNone,
		FlagColor,
		FlagBackground,
		FlagAll
	};

	struct CustomRichText_t
	{
		CString text;
		Color textColor;
		Color textBackground;
		CustomRichTextFlags flags;
		bool highlight;
		Color highlightColor;
		int highlightWidth = 2;
		bool highlightConnectPrev = false;
	};

	typedef std::vector<CustomRichText_t> List;

	//functions
	static void paintRichText(CDC* painter, int x, int y, int w, int h, int xinc, const List & richText, LOGFONT* fontMetrics, bool bAlign = false);
	static void htmlRichText(const List & richText, CString & textHtml, CString & textPlain);
};

#endif // RICHTEXTPAINTER_H
