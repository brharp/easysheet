#pragma once
class Display
{
public:
	Display(void);
	~Display(void);
	virtual void startSheet() = 0;
	virtual void endSheet() = 0;
	virtual void startRow() = 0;
	virtual void endRow() = 0;
	virtual void startCell() = 0;
	virtual void endCell() = 0;
	virtual void cellData(time_t data) = 0;
	virtual void rowHeader(const char *label) = 0;
	virtual void columnHeader(const char *label) = 0;
};

