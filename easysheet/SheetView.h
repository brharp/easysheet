#pragma once
#include "Display.h"
#include "DataSource.h"

class SheetView
{
public:
	SheetView(DataSource &dataSource) : dataSource(dataSource) {};
	~SheetView(void);
	void	render(Display &display);
private:
	DataSource &dataSource;
};

