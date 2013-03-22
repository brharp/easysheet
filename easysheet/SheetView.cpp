#include "StdAfx.h"
#include "SheetView.h"



SheetView::~SheetView(void)
{
}

void SheetView::render(Display &display)
{
	int rowCount;
	int columnCount;
	int rowIndex;
	int columnIndex;

	display.startSheet();
	rowCount = dataSource.getRowCount();
	columnCount = dataSource.getColumnCount();
	display.startRow();
	display.rowHeader("");
	for (columnIndex = 0; columnIndex < columnCount; columnIndex++) {
		display.columnHeader(dataSource.getColumnLabel(columnIndex));
	}
	display.endRow();
	for (rowIndex = 0; rowIndex < rowCount; rowIndex++) {
		display.startRow();
		display.rowHeader(dataSource.getRowLabel(rowIndex));
		for (columnIndex = 0; columnIndex < columnCount; columnIndex++) {
			display.startCell();
			display.cellData(dataSource.getValueAt(rowIndex, columnIndex));
			display.endCell();
		}
		display.endRow();
	}
	display.endSheet();
}