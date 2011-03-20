#include <owl.h>
#include <static.h>
#include <dialog.h>
#include <string.h>
#include <edit.h>
#include <stdio.h>
#include <array.h>
#include <window.h>
#pragma hdrfile
#include <pxengine.h>
#include <owl.h>
#include <pxengine.h>
#include <stdio.h>

#include "pxtable.h"
#include "openbase.h"
#include "values.h"

#define BadField "База %s, Поле #%u (%s)\n%s"
extern char ActivateParadox;

TOpenBase::TOpenBase(const char* name, char** fields, int count, int CorrectPath, int ShowFieldMessage) :
	   PXTable(name, CorrectPath),
	   m_fields(fields)
{
   fh = 0;
   m_FieldsCount = count;
   m_ShowFieldMessage = ShowFieldMessage;
   Open();
}

int TOpenBase::Open(int fieldCount)
{
   if(!ActivateParadox) return 1;

   PXTable::Open();

   if(fieldCount > 0) m_FieldsCount = fieldCount;
   //pxErr = -1;
   if(m_FieldsCount > 0)
   {
       fh = new FIELDHANDLE [m_FieldsCount];
       memset(fh, m_FieldsCount * sizeof(FIELDHANDLE), 0);
       if(pxErr) return 0;
       for(int i = 0; (i < m_FieldsCount) && !pxErr; i++)
	pxErr = PXFldHandle(getTblHandle(), m_fields[i], &fh[i]);

       if(pxErr)
	    {
	      if(m_ShowFieldMessage)
	  {
	    DynStr StrErr;
	    sprintf(StrErr, BadField, m_TableName, i-1, m_fields[i-1], PXErrMsg(pxErr));
		    MessageBox(GetApplicationObject()->MainWindow->HWindow, StrErr,  "Ошибка", MB_OK | MB_ICONHAND);
	      }
	  return 0;
	}

       Refresh();
       return 1;
   }
   return 0;
}

int TOpenBase::ReOpen()
{
    Close();
    Open();
}
 
TOpenBase::~TOpenBase()
{
   Close();
}

int TOpenBase::Close()
{
   if(!ActivateParadox) return 1;

   PXTable::Close();
   if(fh)
   {
	delete fh;
	fh = 0;
   }
   return 1;
}

// ‡ ЇЁбм ў Ї®«п
int TOpenBase::operator ()(int field, double value)
{
   if(field > (m_FieldsCount - 1))
      MessageBox(GetApplicationObject()->MainWindow->HWindow, "Выход за пределы массива",  "Ошибка", MB_OK | MB_ICONHAND);
   return PXPutDoub(getRecHandle(), fh[field], value);
}

int TOpenBase::operator ()(int field, short value)
{
   if(field > (m_FieldsCount - 1))
      MessageBox(GetApplicationObject()->MainWindow->HWindow, "Выход за пределы массива",  "Ошибка", MB_OK | MB_ICONHAND);
   return PXPutShort(getRecHandle(), fh[field], value);
}

int TOpenBase::operator ()(int field, char* value)
{
   if(field > (m_FieldsCount - 1))
	  MessageBox(GetApplicationObject()->MainWindow->HWindow, "Выход за пределы массива",  "Ошибка", MB_OK | MB_ICONHAND);

   char fldType[5];
   PXFldType(getTblHandle(), getField(field), sizeof(fldType), fldType);
   if(*fldType != 'A')
   {
		BLOBHANDLE blbHandle;
		int err = PXBlobOpenWrite(getRecHandle(), fh[field], &blbHandle, strlen(value) + 1, 1);
		if(err != PXSUCCESS) MessageBox(GetApplicationObject()->MainWindow->HWindow, PXErrMsg(err),  "Ошибка", MB_OK | MB_ICONHAND);
		err = PXBlobPut(blbHandle, strlen(value) + 1, 0, value);
		if(err != PXSUCCESS) MessageBox(GetApplicationObject()->MainWindow->HWindow, PXErrMsg(err),  "Ошибка", MB_OK | MB_ICONHAND);
		err = PXBlobClose(blbHandle, 1);
		if(err != PXSUCCESS) MessageBox(GetApplicationObject()->MainWindow->HWindow, PXErrMsg(err),  "Ошибка", MB_OK | MB_ICONHAND);
   }
   else
   {
	   return PXPutAlpha(getRecHandle(), fh[field], value);
   }
}

int TOpenBase::operator ()(int field, DATE value)
{
   if(field > (m_FieldsCount - 1))
      MessageBox(GetApplicationObject()->MainWindow->HWindow, "Выход за пределы массива",  "Ошибка", MB_OK | MB_ICONHAND);
   return PXPutDate(getRecHandle(), fh[field], value);
}

// —вҐ­ЁҐ Ё§ Ї®«Ґ©
int TOpenBase::operator ()(double& value, int field)
{
   if(field > (m_FieldsCount - 1))
      MessageBox(GetApplicationObject()->MainWindow->HWindow, "Выход за пределы массива",  "Ошибка", MB_OK | MB_ICONHAND);
   int ret = PXGetDoub(getRecHandle(), fh[field], &value);
   if(ISBLANKDOUBLE(value)) value = 0;
   return ret;
}

int TOpenBase::operator ()(short& value, int field)
{
   if(field > (m_FieldsCount - 1))
      MessageBox(GetApplicationObject()->MainWindow->HWindow, "Выход за пределы массива",  "Ошибка", MB_OK | MB_ICONHAND);
   int ret = PXGetShort(getRecHandle(), fh[field], &value);
   if(ISBLANKSHORT(value)) value = 0;
   return ret;
}

double TOpenBase::GetDouble(int field)
{
   double Val;
   (*this)(Val, field);
   return Val;
}

_str TOpenBase::GetString(int field)
{
    _str s;
	(*this)(s.s, sizeof s.s, field);
	return s;
}

DATE TOpenBase::GetDate(int field)
{
	DATE d;
	(*this)(d, field);
    return d;
}

short TOpenBase::GetShort(int field)
{
	short v;
	(*this)(v, field);
    return v;
}

int TOpenBase::operator ()(char* value, int len, int field)
{
   unsigned long size = 0;
   *value = 0;
   if(field > (m_FieldsCount - 1))
      MessageBox(GetApplicationObject()->MainWindow->HWindow, "Выход за пределы массива",  "Ошибка", MB_OK | MB_ICONHAND);
   char fldType[5];
   PXFldType(getTblHandle(), getField(field), sizeof(fldType), fldType);
   if(*fldType != 'A')
   {
		BLOBHANDLE blbHandle;
		int err = PXBlobOpenRead(getRecHandle(), fh[field], &blbHandle);
		if(err != PXSUCCESS) MessageBox(GetApplicationObject()->MainWindow->HWindow, PXErrMsg(err),  "Ошибка1", MB_OK | MB_ICONHAND);
		err = PXBlobGetSize(blbHandle, &size);
		if(err != PXSUCCESS) MessageBox(GetApplicationObject()->MainWindow->HWindow, PXErrMsg(err),  "Ошибка1", MB_OK | MB_ICONHAND);
		err = PXBlobGet(blbHandle, len < size ? len : size, 0, value);
		if(err != PXSUCCESS) MessageBox(GetApplicationObject()->MainWindow->HWindow, PXErrMsg(err),  "Ошибка2", MB_OK | MB_ICONHAND);
		err = PXBlobClose(blbHandle, 0);
		if(err != PXSUCCESS) MessageBox(GetApplicationObject()->MainWindow->HWindow, PXErrMsg(err),  "Ошибка3", MB_OK | MB_ICONHAND);
   }
   else
   {
		return PXGetAlpha(getRecHandle(), fh[field], len, value);
   }
}

int TOpenBase::operator ()(DATE& value, int field)
{
   if(field > (m_FieldsCount - 1))
      MessageBox(GetApplicationObject()->MainWindow->HWindow, "Выход за пределы массива",  "Ошибка", MB_OK | MB_ICONHAND);
   return PXGetDate(getRecHandle(), fh[field], &value);
}

/*int TOpenBase::operator ()(short& value, int field)
{
   if(field > (m_FieldsCount - 1))
      MessageBox(GetApplicationObject()->MainWindow->HWindow, "Выход за пределы массива",  "Ошибка", MB_OK | MB_ICONHAND);
   return PXGetShort(getRecHandle(), fh[field], &value);
}
*/
int  TOpenBase::First()
{
     return !PXRecFirst(getTblHandle());
}

int  TOpenBase::Last()
{
     return !PXRecLast(getTblHandle());
}

long  TOpenBase::GetRecords()
{
     long n = 0;
     PXTblNRecs(getTblHandle(), &n);
     return n;
}

int  TOpenBase::Get()
{
     return !PXRecGet(getTblHandle(), getRecHandle());
}

int  TOpenBase::AddRecord()
{
	 return m_lastErrCode = PXRecAppend(getTblHandle(), getRecHandle());
}

int TOpenBase::UpdateRecord()
{
	 return m_lastErrCode = PXRecUpdate(getTblHandle(), getRecHandle());
}

int  TOpenBase::Delete()
{
	 return !(m_lastErrCode = PXRecDelete(getTblHandle()));
}

int  TOpenBase::Next()
{
     return !PXRecNext(getTblHandle());
}

int  TOpenBase::Prev()
{
     return !PXRecPrev(getTblHandle());
}

long TOpenBase::NumberCurrent()
{
   long n = 0;
   PXRecNum(getTblHandle(), &n);
   return n;
}

int TOpenBase::Changed()
{
   int Chng;
   PXNetTblChanged(getTblHandle(), &Chng);
   return Chng;
}

int TOpenBase::Refresh() 
{
   return !PXNetTblRefresh(getTblHandle());
}       

int TOpenBase::IsNull(int field)
{
    if(field > (m_FieldsCount - 1))
      MessageBox(GetApplicationObject()->MainWindow->HWindow, "Выход за пределы массива",  "Ошибка", MB_OK | MB_ICONHAND);
   int Null;
    if(PXFldBlank(getRecHandle(), fh[field], &Null) != PXSUCCESS)
      MessageBox(GetApplicationObject()->MainWindow->HWindow, "Error in PXFldBlank",  "Ошибка", MB_OK | MB_ICONHAND);
    return Null;
}

_str TOpenBase::GetLastErr()
{
	_str s;
	sprintf(s.s, "%s:%d %s", m_TableName, m_lastErrCode, PXErrMsg(m_lastErrCode));
	return s;
}
