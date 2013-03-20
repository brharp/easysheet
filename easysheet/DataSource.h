#pragma once

class DataSource
{
public:
	DataSource(void);
	~DataSource(void);
	virtual int	         getRowCount() = 0;
	virtual int	         getColumnCount() = 0;
	virtual time_t	     getValueAt(int rowIndex, int columnIndex) = 0;
	virtual const char * getRowLabel(int rowIndex) = 0;
	virtual const char * getColumnLabel(int columnIndex) = 0;
};

