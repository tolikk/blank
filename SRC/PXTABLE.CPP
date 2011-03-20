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
#include <process.h>
#include <string.h>

#include "pxtable.h"

extern char ActivateParadox;

PXTable::PXTable(const char* tName, int IsCorrectPath)
{
   DynStr FullName;
   m_TableName = 0;

    recHandle = -1;
	tblHandle = -1;

	if(tName && strlen(tName) > 0)
    {
	 //MessageBox(GetApplicationObject()->MainWindow->HWindow, tName, "Ошибка", MB_OK | MB_ICONSTOP);
		if(IsCorrectPath)
		{
		   if(!GetProfileString("Paradox Engine", "NetNamePath", "", FullName, FullName._sizeof()))
			{
				MessageBox(0, "Не указан путь к базе данных", "NetNamePath ???", MB_OK | MB_SYSTEMMODAL);
				exit(0);
			}

		   if(FullName[lstrlen(FullName) - 1] != '\\') lstrcat(FullName, "\\");
		   lstrcat(FullName, strrchr(tName, '\\') ? (strrchr(tName, '\\') + 1) : tName);
		}
		else
		   strcpy(FullName, tName);

		m_TableName = strdup(FullName);
	}
	//else
	  //	 MessageBox(GetApplicationObject()->MainWindow->HWindow, "aaaaaaaaaaaaaaaaaaaaaa", "Ошибка", MB_OK | MB_ICONSTOP);

}

int PXTable::Open()
{
   pxErr = PXSUCCESS;
   if(!ActivateParadox) return 1;
   pxErr = -1;
   if(m_TableName)
   {
       pxErr = PXTblOpen(m_TableName, &tblHandle, 0, 0);
       if(pxErr != PXSUCCESS) {
	     DynStr errMsg;
	 sprintf(errMsg, "Таблица [%s]\nОшибка : ", m_TableName);
	 strcat(errMsg, PXErrMsg(pxErr));
	 MessageBox(GetApplicationObject()->MainWindow->HWindow, errMsg, "Ошибка", MB_OK | MB_ICONSTOP);
	 tblHandle = -1;
       }
       else
       {
		    pxErr = PXRecBufOpen(tblHandle, &recHandle);
       }
   }
   /*else
   {
	 MessageBox(GetApplicationObject()->MainWindow->HWindow, m_TableName, "Ошибка", MB_OK | MB_ICONSTOP);
   } */
   return pxErr == PXSUCCESS;
}

PXTable::~PXTable()
{
//   Close();
   if(m_TableName)
      delete m_TableName;
   m_TableName = 0;
}


int PXTable::Close()
{
   if(recHandle != -1) {
       PXRecBufClose(recHandle);
       recHandle = -1;
   }
   if(tblHandle != -1) {
       int Code = PXTblClose(tblHandle);
       if(Code != PXSUCCESS)
	   MessageBox(0, PXErrMsg(Code), "Can't Close", 0);
   }
   //if(m_TableName)
   //    delete m_TableName;
   //m_TableName = 0;
   recHandle = -1;
   tblHandle = -1;
   return pxErr = -1;
}