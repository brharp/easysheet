#include "StdAfx.h"
#include "GDIDisplay.h"


void GDIDisplay::startSheet()
{
	pt.x = top;
	pt.y = left;
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
	if (data > 0) {
		int hours = data / 3600;
		int minutes = (data % 3600) / 60;
		wsprintf(text, L"%d:%02d", hours, minutes);
	} else {
		wsprintf(text, L"  -  ");
	}

	// Construct bounding rectangle.
	rc.left   = pt.x;
	rc.top    = pt.y;
	rc.right  = pt.x + cellWidth;
	rc.bottom = pt.y + cellHeight;

	// Draw text.
	DrawText(hdc, text, wcslen(text), &rc,
		DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
}

void GDIDisplay::endCell()
{
	pt.x += cellWidth;
}

void GDIDisplay::endRow()
{
	pt.x = left;
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
	rc.right  = pt.x + headerWidth;
	rc.bottom = pt.y + cellHeight;

	// Convert string.
	mbstowcs_s(&wlen, text, label, 255);

	// Draw text.
	DrawText(hdc, text, wcslen(text), &rc,
		DT_SINGLELINE | DT_VCENTER);

	pt.x += headerWidth;
}

void GDIDisplay::columnHeader(const char *label)
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
		DT_RIGHT | DT_SINGLELINE | DT_VCENTER);

	pt.x += cellWidth;
}
