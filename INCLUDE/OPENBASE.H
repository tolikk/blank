#ifndef __OPENBASE_H
#define __OPENBASE_H

#include <pxtable.h>

typedef struct _str {
	char s[64];
};

class TOpenBase : public PXTable {

	FIELDHANDLE* fh;
	int          m_FieldsCount;
	char**	 	m_fields;
	int 	 	m_lastErrCode;
	int 		m_ShowFieldMessage;

  public:
	TOpenBase(const char*, char**, int, int CorrectPath = 1, int ShowFieldMessage = 1);
    virtual ~TOpenBase();

    FIELDHANDLE  getField(int n)  { return fh[n]; }

	int GetFields() { return m_FieldsCount; }
	_str GetLastErr();

	void  setShowFieldMessage(int ShowFieldMessage) { m_ShowFieldMessage = ShowFieldMessage; }

    int  Changed();
    int  Refresh();

    int  First();
    int  Last();
    long  GetRecords();
    int  Get();
    int  AddRecord();
	int  UpdateRecord();
	int  Delete();
    int  Next();
    int  Prev();
    long  NumberCurrent();
    int   IsNull(int field);

    int  Close();
	int  Open(int fieldCount = -1);
    int  ReOpen();

    int operator ()(int field, double value);
    int operator ()(int field, char* value);
    int operator ()(int field, DATE value);
	int operator ()(int field, short value);
	int operator ()(double& value, int field);
	int operator ()(char* value, int len, int field);
    int operator ()(DATE& value, int field);
	int operator ()(short& value, int field);

    double GetDouble(int field);
	_str GetString(int field);
    DATE GetDate(int field);
	short GetShort(int field);
};

#endif