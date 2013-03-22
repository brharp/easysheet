#pragma once
#include "display.h"
class GDIDisplay :
	public Display
{
public:
	GDIDisplay(HDC hdc) 
		: hdc(hdc), cellWidth(50), cellHeight(25),
	      headerWidth(200), top(15), left(15) {};
	~GDIDisplay(void) {};
	virtual void startSheet();
	virtual void endSheet();
	virtual void startRow();
	virtual void endRow();
	virtual void startCell();
	virtual void endCell();
	virtual void cellData(time_t data);
	virtual void rowHeader(const char *label);
	virtual void columnHeader(const char *label);
private:
	HDC   hdc;
	POINT pt;
	int   cellWidth;
	int   cellHeight;
	int   headerWidth;
	int   left;
	int   top;
};

