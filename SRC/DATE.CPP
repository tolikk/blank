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
#include <bmpsize.h>
#include <stdio.h>
#include <string.h>

#include <static.h>
#include <date.h>
#include <dos.h>
#include <commdlg.h>
#include <dlg_base.h>

BOOL MyOpenFile(HWND HWindow, OPENFILENAME& ofn, char* szFilter, char* szDirName, char* szFile, char* Title, char* ext, int Load = 1 );
char* GetRCStr(int);

getDate::getDate(PTWindowsObject p, int* mn, int* yr, int* dmn, char* filename)
           :TDialog(p, "DATE")
{
   m = mn;
   y = yr;
   dm = dmn;
   file = filename;
   e1 = new TComboBox(this, 101, 4);
   e1_1 = new TComboBox(this, 105, 4);
   e2 = new TEdit(this, 102, 5);
   e3 = new TEdit(this, 103, 256);
}

void getDate::SetupWindow()
{
   TDialog::SetupWindow();

   char buff[30];

   for(int i = 0; i < 12; i++)
    {
       LoadString(GetApplication()->hInstance, 100 + i, buff, sizeof buff);
	   e1->AddString(buff);
  	   sprintf(buff, "%d", i);
	   e1_1->AddString(buff);
	}

   struct date d;

   getdate(&d);

   if(d.da_mon == 1)
    {
      d.da_year--;
      d.da_mon = 12;
    }

   sprintf(buff, "%u", d.da_year);

   e1->SetSelIndex(d.da_mon - 1);
   e1_1->SetSelIndex(0);
   e2->SetText(buff);
}

void getDate::Ok(RTMessage msg)
{
   char buff[20];

   e2->GetText(buff, sizeof buff);
   if(strlen(buff) != 4) return;
   *y = atoi(buff);
   if(*y > 2100 || *y < 2000) return;
   *m = e1->GetSelIndex() + 1;
   *dm = e1_1->GetSelIndex();
   e3->GetText(file, 256);
   if(!*file) SelectFile();
   e3->GetText(file, 256);
   if(!*file) return;

   TDialog::Ok(msg);
}

void getDate::SelectFile()
{
	OPENFILENAME ofn;
	DynStr szDirName;
    DynStr szNumbers;
	DynStr szFile;
	e3->GetText(szFile, szFile._sizeof());
	char  l_szFilter[] = "װאיכû מבלוםא(*.bdt)|*.bdt||";
	strcpy(szDirName, szFile);
    char buff[20];
	e2->GetText(buff, sizeof buff);
	if(szFile[0] == 0) {
		strcpy(szDirName, "D:\\");
		char* ch = strrchr(file, '\\');
		strcpy(szFile, ch ? (ch + 1) : file);
		if(strlen(buff) > 2)
			strcpy(szFile + 4, buff + strlen(buff) - 2);
		wsprintf(szFile + strlen(szFile), "%02d.bdt", e1->GetSelIndex() + 1);
		//strcat(szFile, ".bdt");
	}

	if(!MyOpenFile(GetApplicationObject()->MainWindow->HWindow, ofn, l_szFilter, szDirName, szFile, (char*)GetRCStr(620), "", 0)) return;
	e3->SetText(szFile);
}

