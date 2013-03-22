#include "StdAfx.h"
#include "ICalFileDataSource.h"

struct rrule {
	const char *freq;
	int max_count;
	int count;
	time_t until;
	int interval;
	int *by_day;
	int *day;
};


static const char *BEGIN = "BEGIN", *END = "END", *SUMMARY = "SUMMARY",
	*DTSTART = "DTSTART", *DTEND = "DTEND", *DURATION = "DURATION",
	*DATE = "DATE", *VALUE = "VALUE", *VEVENT = "VEVENT",
	*RRULE = "RRULE", *FREQ = "FREQ", *YEARLY = "YEARLY",
	*MONTHLY = "MONTHLY", *WEEKLY = "WEEKLY", *DAILY = "DAILY",
	*INTERVAL = "INTERVAL", *COUNT = "COUNT", *UNTIL = "UNTIL",
	*BYDAY = "BYDAY";


ICalFileDataSource::ICalFileDataSource(const char *filename, time_t start, time_t end)
	: start(start), end(end)
{
	sheet       = NULL;
	rowCount    = 0;
	columnCount = (int)((end - start) / 86400);	// number of days in report range
	firstGroup  = NULL;
	groupCount  = 0;

	if (fopen_s(&stream, filename, "r") == 0) {
		addRows(256);
		begin(filename);	// start parse
		fclose(stream);
	}
}


ICalFileDataSource::~ICalFileDataSource(void)
{
}

int	ICalFileDataSource::getRowCount(void)
{
	if (firstGroup != NULL) {
		return firstGroup->rowIndex + 1;
	} else {
		return 0;
	}
}

int ICalFileDataSource::getColumnCount(void)
{
	return columnCount;
}

const char *ICalFileDataSource::getRowLabel(int rowIndex)
{
	Group *group;
	group = findGroupByRowIndex(rowIndex);
	if (group != NULL) {
		return group->name;
	} else {
		return NULL;
	}
}

const char *ICalFileDataSource::getColumnLabel(int columnIndex)
{
	static char label[256];
	time_t time;
	struct tm tm;

	time = start + columnIndex * 60 * 60 * 24;
	localtime_s(&tm, &time);
	sprintf(label, "%d", tm.tm_mday);
	return label;
}

time_t ICalFileDataSource::getValueAt(int rowIndex, int columnIndex)
{
	return sheet[rowIndex][columnIndex];
}

void	ICalFileDataSource::begin(const char *context) 
{ 
	char *summary = 0; 
	time_t dtstart = 0; 
	time_t dtend = 0; 
	time_t duration = 0;
	struct rrule rrule = {0};
	bool isdate = false; 
	char *line = 0;
	char *value, *name, *params, *pname, *pvalue;
	int ch;

	line = (char *)malloc(80);
	while (!feof(stream)) { 
		fgets(line, 80, stream); 
		while ((ch = fgetc(stream)) == ' ') { 
			line = (char *)realloc(line, strlen(line) + 80); 
			fgets(line + strlen(line), 80, stream); 
		}
		ungetc(ch, stream);

		strsplt(line, ":", &value); 
		name = strsplt(line, ";", &params);
		value = strsplt(value, "\r\n", 0);

		/* BEGIN */
		if (strcmp(name, BEGIN) == 0) { 
			begin(value); 
		/* SUMMARY */
		} else if (strcmp(name, SUMMARY) == 0){
			size_t sz = strlen(value) + 1;
			summary = (char *)realloc(summary, sz);
			strcpy_s(summary, sz, value);
		/* DTSTART */
		} else if (strcmp(name, DTSTART) == 0) { 
			while ((pname = strsplt(0, "=", &params)) != 0) { 
				pvalue = strsplt(0, ";", &params); 
				if (strcmp(pname, VALUE) == 0) { 
					isdate = !(strcmp(pvalue, DATE)); 
				} 
			} 
			dtstart = parseDate(value, isdate); 
		/* DTEND */
		} else if (strcmp(name, DTEND) == 0) { 
			while ((pname = strsplt(0, "=", &params)) != 0) { 
				pvalue = strsplt(0, ";", &params); 
				if (strcmp(pname, VALUE) == 0) { 
					isdate = !(strcmp(pvalue, DATE)); 
				} 
			} 
			dtend = parseDate(value, isdate); 
		} else if (strcmp(name, DURATION) == 0) {
			/* DURATION */
			duration = parseDuration(value);
		} else if (strcmp(name, RRULE) == 0) {
			/* RRULE */
			parseRrule(value, &rrule);
		} else if (strcmp(name, END) == 0) { 
			/* END */
			break; 
		} 
	} 

	if (strcmp(context, VEVENT) == 0) { 
		if (duration == 0) { 
			duration = dtend - dtstart; 
		} 
		do {
			dtend = dtstart + duration; 
			if (start <= dtstart && dtend <= end) {
				Group *group; 
				size_t day; 
				group = findGroup(summary); 
				day = (size_t)((dtstart - start) / 86400); 
				sheet[group->rowIndex][day] += duration; 
			}
		} while ((dtstart = rrepeat(dtstart, &rrule)) != -1);
	}
} 


/* strsplt - splits a string on sep */

char *	ICalFileDataSource::strsplt(char *src, const char *sep, char **rest)
{
    char *start = src != 0 ? src : *rest;
    char *end;

    /*
     * Skip over leading delimiters.
     */
    start += strspn(start, sep);
    if (*start == 0) {
		if (rest != 0) {
			*rest = start;
		}
		return 0;
    }

    /*
     * Separate off one token.
     */
    end = start + strcspn(start, sep);
    if (*end != 0) {
		*end++ = 0;
	}
	if (rest != 0) {
		*rest = end;
	}
    return start;
}

void	ICalFileDataSource::addRows(size_t delta) 
{ 
	int rowIndex, columnIndex; 
	sheet = (time_t **)realloc(sheet, (rowCount + delta) * sizeof(time_t *)); 
	for (rowIndex = 0; rowIndex < (int)delta; rowIndex++) { 
		sheet[rowCount + rowIndex] = (time_t *)malloc(columnCount * sizeof(time_t)); 
		for (columnIndex = 0; columnIndex < columnCount; columnIndex++) { 
			sheet[rowCount + rowIndex][columnIndex] = 0;
		}
	}
	rowCount += delta; 
} 




Group *	ICalFileDataSource::addGroup(const char *s) 
{ 
	Group *grp;
	if (groupCount == rowCount) { 
		addRows(256); 
	} 
	grp = new Group();
	grp->name = _strdup(s); 
	grp->rowIndex = groupCount++; 
	grp->nextGroup = firstGroup; 
	firstGroup = grp; 
	return grp; 
} 


Group * ICalFileDataSource::findGroup(const char *s) 
{ 
	Group *grp; 
	for (grp = firstGroup; grp != NULL; grp = grp->nextGroup) { 
		if (strcmp(s, grp->name) == 0) { 
			return grp; 
		} 
	} 
	return addGroup(s); 
} 


Group * ICalFileDataSource::findGroupByRowIndex(int rowIndex) 
{ 
	Group *grp; 
	for (grp = firstGroup; grp != NULL; grp = grp->nextGroup) { 
		if (rowIndex == grp->rowIndex) { 
			return grp; 
		} 
	}
	return NULL;
} 


time_t rrepeat(time_t dtstart, struct rrule *rrule)
{
	static int *by_day;
	struct tm broken_down_time;
	static int count;

	if (dtstart > (*rrule).until || (*rrule).count > (*rrule).max_count) {
		return -1;
	}

	if (rrule->freq == YEARLY) {
		localtime_s(&broken_down_time, &dtstart);
		broken_down_time.tm_year += rrule->interval;
		dtstart = mktime(&broken_down_time);
	}

	else if (rrule->freq == MONTHLY) {
		localtime_s(&broken_down_time, &dtstart);
		broken_down_time.tm_mon += rrule->interval;
		dtstart = mktime(&broken_down_time);
	}

	else if (rrule->freq == WEEKLY) {
		/* if there are no by_day expansions, or we have
		 * past the end of the by_day array, add interval
		 * to the start date */
		if (rrule->by_day == 0 || *rrule->day == -1) {
			dtstart += rrule->interval * 7 * 24 * 60 * 60;
		}
		/* any by_day expansions? */
		if (rrule->by_day != 0) {
			/* if past the end, reset pointer */
			if (rrule->day == 0 || *rrule->day == -1) {
				rrule->day = rrule->by_day;
			}
			/* set date from by_day array and increment pointer */
			localtime_s(&broken_down_time, &dtstart);
			broken_down_time.tm_mday += (*rrule->day++ - broken_down_time.tm_wday);
			dtstart = mktime(&broken_down_time);
		}
	}

	else {
		return -1;
	}

	++rrule->count;

	return dtstart;	
}


void ICalFileDataSource::parseRrule(const char *string, struct rrule *rrule)
{
	char *first, *rest, *name, *value, *s;
	const char **f, **d;

	rrule->max_count = 200; //default
	
	s = _strdup(string);
	rest = s;
	while ((first = strsplt(0, ";", &rest)) != 0) {
		name = strsplt(first, "=", &value);
		if (strcmp(name, FREQ) == 0) {
			const char *frq[] = { YEARLY, MONTHLY, WEEKLY, DAILY, 0 };
			for (f = frq; *f != 0; f++) {
				if (strcmp(value, *f) == 0) {
					rrule->freq = *f;
					break;
				}
			}
		} else if (strcmp(name, INTERVAL) == 0) {
			rrule->interval = atoi(value);
		} else if (strcmp(name, COUNT) == 0) {
			rrule->max_count = atoi(value);
		} else if (strcmp(name, UNTIL) == 0) {
			rrule->until = parseDate(value, true);
		} else if (strcmp(name, BYDAY) == 0) {
			const char *days[] = { "SU", "MO", "TU", "WE", "TH", "FR", "SA", 0 };
			char *first_day, *rest_days = value;
			int count_days = 0;
			while ((first_day = strsplt(0, ",", &rest_days)) != 0) {
				rrule->by_day = (int *)realloc(rrule->by_day, (count_days + 2) * sizeof(int));
				for (d = days; *d != 0; d++) {
					if (strcmp(first_day, *d) == 0) {
						rrule->by_day[count_days++] = (int)(d - days);
						break;
					}
				}
			}
			rrule->by_day[count_days] = -1;
			rrule->day = rrule->by_day;
		}
	}

	free(s);
}


time_t ICalFileDataSource::parseDate(const char *s, bool isdate) 
{ 
	struct tm tm = {0}; 
	int n; 

	n = sscanf_s(s, "%4d%2d%2dT%2d%2d%2d", &tm.tm_year, &tm.tm_mon, 
		&tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec); 

	tm.tm_year -= 1900;
	tm.tm_mon  -= 1;

	if (n == 6 || n == 3 && isdate) { 
		return mktime(&tm); 
	} else { 
		return -1; 
	} 
} 





time_t ICalFileDataSource::parseDuration(const char *s) 
{ 
	return 0; 
} 




time_t ICalFileDataSource::rrepeat(time_t dtstart, struct rrule *rrule)
{
	static int *by_day;
	struct tm broken_down_time;
	static int count;

	if (dtstart > (*rrule).until || (*rrule).count > (*rrule).max_count) {
		return -1;
	}

	if (rrule->freq == YEARLY) {
		localtime_s(&broken_down_time, &dtstart);
		broken_down_time.tm_year += rrule->interval;
		dtstart = mktime(&broken_down_time);
	}

	else if (rrule->freq == MONTHLY) {
		localtime_s(&broken_down_time, &dtstart);
		broken_down_time.tm_mon += rrule->interval;
		dtstart = mktime(&broken_down_time);
	}

	else if (rrule->freq == WEEKLY) {
		/* if there are no by_day expansions, or we have
		 * past the end of the by_day array, add interval
		 * to the start date */
		if (rrule->by_day == 0 || *rrule->day == -1) {
			dtstart += rrule->interval * 7 * 24 * 60 * 60;
		}
		/* any by_day expansions? */
		if (rrule->by_day != 0) {
			/* if past the end, reset pointer */
			if (rrule->day == 0 || *rrule->day == -1) {
				rrule->day = rrule->by_day;
			}
			/* set date from by_day array and increment pointer */
			localtime_s(&broken_down_time, &dtstart);
			broken_down_time.tm_mday += (*rrule->day++ - broken_down_time.tm_wday);
			dtstart = mktime(&broken_down_time);
		}
	}

	else {
		return -1;
	}

	++rrule->count;

	return dtstart;	
}

