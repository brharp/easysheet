#pragma once
#include "display.h"
class GDIDisplay :
	public Display
{
public:
	GDIDisplay(HDC hdc) 
		: hdc(hdc), cellWidth(50), cellHeight(50) {
			pt.x = 0;
			pt.y = 0;
	};
	~GDIDisplay(void) {};
	virtual void startSheet();
	virtual void endSheet();
	virtual void startRow();
	virtual void endRow();
	virtual void startCell();
	virtual void endCell();
	virtual void cellData(time_t data);
	virtual void rowHeader(const char *label);
private:
	HDC   hdc;
	POINT pt;
	int   cellWidth;
	int   cellHeight;
};

