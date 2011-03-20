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
#include <dialog.h>
#include <edit.h>
#include <pxengine.h>
#include <pxtable.h>
#include <openbase.h>
#include <array.h>
#include <grncard.h>
#include <stdio.h>
#include <string.h>
#include <ruler.h>
#include <window.h>
#include <commdlg.h>
#include <warta.h>
#include "help.h"

#ifdef NEED_WARTA

int lenDB2[22] = { 26, 16, 21, 8, 11, 61, 61, 11, 8, 71, 41, 71, 51, 31, 251, 21, 21, 26, 231, 21, 121, 181 };

void GetFromIni(char* b, LPSTR entry, PTEdit dest);
char* wartaFields[] = {
                           "Number",
                           "Branch",
                           "Phone",
                           "Telex",
                           "Fax",
                           "Iname",
                           "Iaddress",
                           "GetDate",
                           "Isum_n",
                           "Isum",
                           "Subject1",
                           "Subject2",
                           "Subject3",
                           "Subject4",
                           "Text_1",
                           "From",
                           "To",
                           "About",
                           "Clause",
                           "City",
                           "Agent",
                           "Payment"
                         };

int  Wr_cardCount = 22;
char* WRName = "\\bases\\warta";

DB2::DB2(PTWindowsObject p)
    :Dlg_base(p, "WARTA", WRName, wartaFields, Wr_cardCount)
{
   for(int i = 0; i < 22; i++)
     e[i] = new TEdit(this, 100 + i, lenDB2[i]);
}

void DB2::SetupWindow()
{
   Dlg_base::SetupWindow();
}

extern void DblToStr(char* str, double V);
extern DATE GD(char* str, int = 0);

void DB2::GetDataFromBase()
{
   double N;
   char*  ch = (char*)&buffer;
 //  int    m, d, y;

   for(int i = 0; i < 8; i++)
    {
      (*base)(ch, lenDB2[i], i);
      ch += lenDB2[i];
    }

   //дата выдачи
/*
   DATE date;
   (*base)(date, 7);
   PXDateDecode(date, &m, &d, &y);
   sprintf(buffer.GetDate, "%02u.%02u.%02u", d, m, y % 100);
*/
   (*base)(N, 8);
   sprintf(buffer.Isum_n, "%2.2f", N);

   ch = buffer.Isum;
   for(i = 9; i < 22; i++)
    {
      (*base)(ch, lenDB2[i], i);
      ch += lenDB2[i];
    }

   Dlg_base::GetDataFromBase();
}

void DB2::SetButtons()
{
   Dlg_base::SetButtons();
   /*
   int flag1 = TRUE;
   if(base->NumberCurrent() == 1) flag1 = FALSE;
   EnableWindow(GetDlgItem(HWindow, 401), flag1);
   EnableWindow(GetDlgItem(HWindow, 403), flag1);

   flag1 = TRUE;
   if(base->NumberCurrent() == base->GetRecords()) flag1= FALSE;
   EnableWindow(GetDlgItem(HWindow, 402), flag1);
   EnableWindow(GetDlgItem(HWindow, 404), flag1);*/
}

int DB2::SetDataToBase()
{
   Dlg_base::SetDataToBase();

   char* ch = (char*)&buffer;

   for(int i = 0; i < 8; i++)
    {
      (*base)(i, ch);
      ch += lenDB2[i];
    }

/*   DATE date = GD(buffer.GetDate);
   if(!date)
    {
       SetFocus(e[7]->HWindow);
       return 0;
    }

   (*base)(7, date);*/
   (*base)(8, atof(buffer.Isum_n));

   ch = buffer.Isum;
   for(i = 9; i < 22; i++)
    {
      (*base)(i, ch);
      ch += lenDB2[i];
    }

   return 1;
}

DB2::~DB2()
{
}

void DB2::Find()
{
   char b[100];
   if(e[0]->GetText(b, sizeof b))
    {
       (*base)(0, b);
       if(!PXSrchKey(base->getTblHandle(), base->getRecHandle(),1, SEARCHFIRST))
        {
           base->Get();
           GetDataFromBase();
        }
       else MessageBox(GetApplication()->MainWindow->HWindow, "Полис с таким номером не существует в базе.", "Сообщаю", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
    }
}

void DB2::Print1()
{
   char* str = new char [2000];
   char* dest = str;
   char* b = (char*)&buffer;
   int j = 0;//lenDB2[0];

   memset(str, 0, 2000/*sizeof str*/);
   if(!SetDataToBase()) return;
   Dlg_base::SetDataToBase();

   for(int i = 0; i < 22; i++)
     {
        strcpy(dest, &b[j]);
        dest += strlen(dest) + 1;
        j += lenDB2[i];
     }

   ((TRulerWin*)Parent)->SetValues(str);
   Dlg_base::Print1();
   delete str;
}

void DB2::Clear()
{
   for(int i = 0; i < 22; i++) e[i]->SetText("");
   Default();
}

void DB2::Default()
{
   char *buff = new char[2000];

   GetFromIni(buff, "brauch", e[1]);
   GetFromIni(buff, "phone", e[2]);
  GetFromIni(buff, "telex", e[3]);
   GetFromIni(buff, "fax", e[4]);

   GetFromIni(buff, "text%d", e[14]);
   GetFromIni(buff, "agent%d", e[20]);
   GetFromIni(buff, "payment%d", e[21]);

   delete buff;
}


void  DB2::Help(RTMessage msg)
{
   if(msg.WParam == 1000)
      WinHelp(GetApplication()->MainWindow->HWindow, "blank.hlp", HELP_CONTEXT, HELP_WARTA);

   Dlg_base::Help(msg);
}

void GetFromIni(char* b, LPSTR entry, PTEdit dest)
{
   char ini[200];
   char entr[20];
   *b = 0;

   for(int i = 1; i < 20; i++)
    {
      sprintf(entr, entry, i);
      if(!GetPrivateProfileString("WARTA", entr, "", ini, sizeof ini, ININame))
        break;

      if(strlen(b)) strcat(b, "\r\n");
      strcat(b, ini);
      if(!strcmp(entr, entry)) break;
    }

   dest->SetText(b);
}

#endif