#ifndef __PXTABLE_H
#define __PXTABLE_H

#include <pxengine.h>

struct PolisRange {
PolisRange(){*Seria=0;*Agent=0;To=From=0;}
	char Seria[7];
	char Agent[5];
	double From, To;
};

class DynStr {
	char* m_data;
    int m_size;
public:
    DynStr(int Size = 512);
    DynStr(const DynStr& o)
    {
    	m_size = o.m_size;
    	m_data = new char[m_size];
		strcpy(m_data, o.m_data);
    }
    ~DynStr();
    operator char*() { return m_data; }
	char operator [](int i) { return m_data[i]; }
	void set(char ch, int pos) {m_data[pos]=ch;}
    int _sizeof() { return m_size; }
};

class PXTable {

   protected:

     RECORDHANDLE recHandle;
     TABLEHANDLE  tblHandle;
	 char* m_TableName;

   public:
	 PXTable(const char* tName, int IsCorrectPath = 1);
     virtual ~PXTable();

     TABLEHANDLE getTblHandle()
      { return tblHandle; }

     RECORDHANDLE getRecHandle()
      { return recHandle; }

     int pxErr;

	 const char* GetTableName() { return m_TableName; }

   public:
     int Close();
     int Open();
};

#endif

