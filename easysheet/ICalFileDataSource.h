#pragma once
#include "datasource.h"

struct Group { 
	size_t rowIndex; 
	const char *name; 
	Group *nextGroup; 
}; 

class ICalFileDataSource :
	public DataSource
{
public:
	ICalFileDataSource(const char *filename, time_t start, time_t end);
	~ICalFileDataSource(void);
	virtual int          getRowCount();
	virtual int          getColumnCount();
	virtual time_t       getValueAt(int rowIndex, int columnIndex);
	virtual const char * getRowLabel(int rowIndex);
	virtual const char * getColumnLabel(int columnIndex);
	static time_t parseDate(const char *s, bool isdate);
private:
	int       rowCount;
	int       columnCount;
	time_t ** sheet;
	FILE *    stream;
	time_t    start;
	time_t    end;
	int       groupCount;
	Group *   firstGroup;
	void      begin(const char *context);
	time_t    parseDuration(const char *s);
	void      parseRrule(const char *s, struct rrule *rrule);
	time_t    rrepeat(time_t, struct rrule *);
	char *    strsplt(char *src, const char *sep, char **rest);
	void      addRows(size_t delta);
	Group *	  addGroup(const char *s);
	Group *   findGroup(const char *summary);
	Group *   findGroupByRowIndex(int rowIndex);

};

