#include "StdAfx.h"
#include "GDIDisplay.h"


void GDIDisplay::startSheet()
{
	pt.x = 0;
	pt.y = 0;
}

void GDIDisplay::startRow()
{
}

void GDIDisplay::startCell()
{
}

void GDIDisplay::cellData(time_t data)
{
	RECT rc;
	WCHAR text[256];

	// Convert time to string.
	wsprintf(text, L"%d", data / 60);

	// Construct bounding rectangle.
	rc.left   = pt.x;
	rc.top    = pt.y;
	rc.right  = pt.x + cellWidth;
	rc.bottom = pt.y + cellHeight;

	// Draw text.
	DrawText(hdc, text, wcslen(text), &rc,
		DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

void GDIDisplay::endCell()
{
	pt.x += cellWidth;
}

void GDIDisplay::endRow()
{
	pt.x = 0;
	pt.y += cellHeight;
}

void GDIDisplay::endSheet()
{
}

void GDIDisplay::rowHeader(const char *label)
{
	RECT rc;
	WCHAR text[256];
	size_t wlen;

	// Construct bounding rectangle.
	rc.left   = pt.x;
	rc.top    = pt.y;
	rc.right  = pt.x + cellWidth;
	rc.bottom = pt.y + cellHeight;

	// Convert string.
	mbstowcs_s(&wlen, text, label, 255);

	// Draw text.
	DrawText(hdc, text, wcslen(text), &rc,
		DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	pt.x += cellWidth;
}
