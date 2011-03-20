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
#include <combobox.h>
#include <commdlg.h>
#include <private.h>
#include <checkbox.h>
#include <thwindow.h>
#include <help.h>
#include "util.h"
#include "radiobut.h"
#include "grncard.h"
/*
void SetMenuText(HWND, char*, int);
void SetAgent(char* ACode, TOpenBase& agents, PTComboBox AgName, int);
void SetSumma(char* dest, char* v);
double GetPremiumFromIni(char* entry, char Plan);
void GetFromIni(char* b, LPSTR entry, PTEdit dest);
unsigned char lenPRVT[] = {
                            3, 11, 66, 4, 66, 11, 61, 125, 25, 41, 11, 11, 125,
			    25, 11, 255, 61, 11, 125, 125, 4, 4, 4, 10, 4
                          };
char* GetRCStr(int strID);

char* PrvtFields[] = {
                        "Seria",
                        "Number",
                        "Insured",
                        "Address",
                        "Birthdate",
                        "Destination",
                        "Year_Plan",
                        "Summa",
                        "Period",
                        "Begin",
                        "End",
                        "Premium",
                        "Payment",
                        "Conditions",
                        "Place",
                        "Date",
                        "Count",
                        "Company",
			"AdrCompany",
			"Mans",
			"Old",
			"Child",
			"Agent_code",
			"Valuta",
                        "CurrCode"
                     };

int  PrvtCount = 25;
char* PrvtName = "\\bases\\private";

class Edit : public TEdit
{
     THWindow  *  help;
    public:
                  Edit(PTWindowsObject AParent, int ResourceId, WORD ATextLen) :
                  TEdit(AParent, ResourceId, ATextLen) { help = 0; }

             void WMRButtonDown(RTMessage) = [WM_FIRST + WM_RBUTTONDOWN];
             void WMRButtonUp(RTMessage) = [WM_FIRST + WM_RBUTTONUP];
};

void Edit::WMRButtonDown(RTMessage)
{
   char buff[50];
   char dest[180] = "";

   //SetFocus(HWindow);
   GetText(buff, sizeof buff);
   SetSumma(dest, buff);
   SetCapture(HWindow);
   POINT p = { 0, Attr.H / 2 };
   ClientToScreen(HWindow, &p);
   if(help) delete help;
   help = new THWindow(dest, p.x, p.y);
}

void Edit::WMRButtonUp(RTMessage)
{
   if(help)
    {
      delete help;
      help = 0;
    }
   ReleaseCapture();
}

class Mcombo : public TComboBox
{
    public:
                   Mcombo(PTWindowsObject, int, int);
      virtual WORD Transfer(Pvoid DataPtr, WORD TransferFlag);
              void SetupWindow();

              void WMCtlColor(RTMessage) = [WM_FIRST + WM_CTLCOLOR];
};

class Scombo : public Mcombo
{
               int Mask;
    public:
                   Scombo(PTWindowsObject p, int a, int b, int mask):
                    Mcombo(p, a, b), Mask(mask) {};
              void SetupWindow();
};

void Scombo::SetupWindow()
{
   TComboBox::SetupWindow();

   char buff[100];
   for(int i = 1;; i++)
    {
      wsprintf(buff, Mask == 6 ? "Destination%u" : "Period%u", i);
      if(!GetPrivateProfileString("PRIVATE", buff, "", buff, sizeof buff, ININame)) break;
      if(Mask == 9)
       {
         char* ch = strchr(buff, ',');
         if(!ch)
          {
	    MessageBox(0, "Ошибка в файле конфигурации (Period)", "Сообщаю", MB_OK | MB_ICONINFORMATION);
            break;
          }
         else *ch = 0;
       }
      AddString(buff);
    }
}

int Modify;

PrivateDlg::PrivateDlg(PTWindowsObject p)
    :Dlg_base(p, "LEBEN", PrvtName, PrvtFields, PrvtCount),
     agents(AgentDBName, AgentFields, 2)
{
   Modify = 0;
   for(int i = 0; i < 20; i++)
     if(i == 6 || i == 9) e[i] = (PTEdit)new Scombo(this, 200 + i, lenPRVT[i], i);
     else if(i == 7 || i == 12) e[i] = new Edit(this, 200 + i, lenPRVT[i]);
     else e[i] = new TEdit(this, 200 + i, lenPRVT[i]);

   for(i = 0; i < 5; i++)
    new TCheckBox(this, 300 + i, 0);

   for(i = 20; i < 23; i++)
     e[i] = new TEdit(this, 200 + i, lenPRVT[i]);

   e[23] = new NumberEdit(this, 571, 10, 1);

   CurrencyCode = new TComboBox(this, 572, 4);
   CurrencyCode->DisableTransfer();

   AgName = new TComboBox(this, 223, 100);
   AgName->DisableTransfer();
   YearCombo = new TComboBox(this, 224, 100);
   YearCombo->DisableTransfer();

   *OLDAGENT = 0;
}
*/
void SetLittleFont(HWND HWindow, short* Numb, int nNumb)
{
   LOGFONT lFont;
   HFONT hfontDlg = (HFONT) NULL;
   if ((hfontDlg = (HFONT) SendMessage(HWindow, WM_GETFONT, 0, 0L)))//
    if (GetObject(hfontDlg, sizeof(LOGFONT), (LPSTR) &lFont))
     {
       lFont.lfWeight = FW_THIN;
	   if (hfontDlg = CreateFontIndirect((LPLOGFONT) &lFont))
       for(int i = 0; i < nNumb; i++)
         SendDlgItemMessage(HWindow, Numb[i], WM_SETFONT, (WPARAM) hfontDlg, 0);
     }
}
/*
void PrivateDlg::PeriodYear(RTMessage msg)
{
   if((msg.LP.Hi == LBN_SELCHANGE) && Modify)
    PostMessage(HWindow, 1, 0, 0);
}

void PrivateDlg::SetupWindow()
{
int Numb[8] = { 101, 207, 208, 212, 213, 218, 219, 405};

   Dlg_base::SetupWindow();

   YearCombo->AddString("1 year");
   YearCombo->AddString("6 monthes");
   YearCombo->AddString("3 monthes");
   YearCombo->SetSelIndex(0);

   CurrencyCode->AddString("USD");
   CurrencyCode->AddString("DM");
   CurrencyCode->AddString("EUR");
   CurrencyCode->AddString("RUR");
   CurrencyCode->AddString("BRB");

   if(agents.pxErr)
      CloseWindow();
   else {
      agents.First();
      for(int i = 0; i < agents.GetRecords(); i++) {
	 char Tmp[100];
	 agents.Get();
	 agents(Tmp, 100, 1);
	 char AgCode[10];
	 agents(AgCode, 10, 0);
	 int Index = AgName->AddString(Tmp);
	 SendMessage(AgName->HWindow, CB_SETITEMDATA, Index, atoi(AgCode));
	 agents.Next();
       }
   }
   SetAgent(OLDAGENT, agents, AgName, 0);
   // menu SetMenuText(GetApplication()->MainWindow->HWindow, DB1::Agent_Name, 2);

   SetLittleFont(HWindow, Numb, 8);

   EnableMenuItem(GetSubMenu(GetMenu(Parent->HWindow), 2), 1, MF_BYPOSITION | MF_GRAYED);
   if(!base->GetRecords())
     PostMessage(HWindow, WM_COMMAND, 405, MAKELONG(405, BN_CLICKED));

   GetDataFromBase();
}

extern void DblToStr(char* str, double V, int Digit = 2);
extern DATE GD(char* str, int = 0);

void PrivateDlg::GetDataFromBase()
{
   memset(&buffer, 0, sizeof buffer);
   (*base)(buffer.Seria, sizeof buffer.Seria, 0);
   double value;
   (*base)(value, 1);
   sprintf(buffer.Number, "%lu", (long)value);
   (*base)(buffer.Insured, sizeof buffer.Insured, 2);
   (*base)(buffer.Address, sizeof buffer.Address, 3);

   DATE date;

   (*base)(date, 4);
   int m, d, y;
   *buffer.Birthdate = 0;
   if(!ISBLANKDATE(date))
    {
      PXDateDecode(date, &m, &d, &y);
      sprintf(buffer.Birthdate, "%02u.%02u.%04u", d, m, y);
    }


   (*base)(buffer.Destination, sizeof buffer.Destination, 5);

   ///
 //  TMessage msg;
   char YP[2];
   (*base)(YP, sizeof YP, 6);

   buffer.Plan[0] = 1;
   buffer.Plan[1] = 1;
   buffer.Plan[2] = 1;
   if(*YP == 'G' || *YP == 'S' || *YP == 'T' || *YP == 0)
    {
       ShowWindow(YearCombo->HWindow, TRUE);
       ShowWindow(e[9]->HWindow, FALSE);
       ((PTCheckBox)ChildWithId(300))->Check();
       buffer.Year[0] = 1;
       buffer.Year[1] = 0;
       YearCombo->SetSelIndex(*YP == 'S' ? 1 : *YP == 'T' ? 2 : 0);
    }
   else
    {
       ShowWindow(YearCombo->HWindow, FALSE);
       ShowWindow(e[9]->HWindow, TRUE);
       ((PTCheckBox)ChildWithId(300))->Uncheck();
       buffer.Year[0] = 0;
       buffer.Year[1] = 1;
       buffer.Plan[*YP - 'A'] = 0;
    }
   AuxDisen();

   ///

   (*base)(value, 7);
   DblToStr(buffer.Summa, value);
   if(strlen(buffer.Summa)) strcat(buffer.Summa, " USD");
   (*base)(buffer.Period, sizeof buffer.Period, 8);

   (*base)(date, 9);
   *buffer.Begin = 0;
   if(!ISBLANKDATE(date)) {
     PXDateDecode(date, &m, &d, &y);
     sprintf(buffer.Begin, "%02u.%02u.%04u", d, m, y);
   }

   (*base)(date, 10);
   *buffer.End = 0;
   if(!ISBLANKDATE(date)) {
     PXDateDecode(date, &m, &d, &y);
     sprintf(buffer.End, "%02u.%02u.%04u", d, m, y);
   }


   (*base)(value, 11);
   DblToStr(buffer.Premium, value);
   if(strlen(buffer.Premium)) strcat(buffer.Premium, " USD");

   (*base)(date, 12);
   *buffer.Payment = 0;
   if(!ISBLANKDATE(date)) {
     PXDateDecode(date, &m, &d, &y);
     sprintf(buffer.Payment, "%02u.%02u.%04u", d, m, y);
   }

   (*base)(buffer.Conditions, sizeof buffer.Conditions, 13);
   (*base)(buffer.Place, sizeof buffer.Place, 14);

   (*base)(date, 15);
   *buffer.Date = 0;
   if(!ISBLANKDATE(date)) {
      PXDateDecode(date, &m, &d, &y);
      sprintf(buffer.Date, "%02u.%02u.%04u", d, m, y);
   }

   (*base)(value, 16);
   char bbb[25];
   DblToStr(bbb, value);
   strcpy(buffer.Count, value > 0 ? bbb : "");

   (*base)(buffer.Company, sizeof buffer.Company, 17);
   (*base)(buffer.AdrCompany, sizeof buffer.AdrCompany, 18);

   (*base)(value, 19);
   DblToStr(bbb, value);
   strcpy(buffer.Mans, value > 0 ? bbb : "");

   (*base)(value, 20);
   DblToStr(bbb, value);
   strcpy(buffer.Old, value > 0 ? bbb : "");

   (*base)(value, 21);
   DblToStr(bbb, value);
   strcpy(buffer.Child, value > 0 ? bbb : "");

   (*base)(OLDAGENT, sizeof OLDAGENT, 22);
   SetAgent(OLDAGENT, agents, AgName, 0);

   (*base)(value, 23);
   DblToStr(buffer.Valuta, value);

   char CurrCode[4];
   (*base)(CurrCode, sizeof CurrCode, 24);
   CurrencyCode->SetSelIndex(-1); 
   CurrencyCode->SetText(CurrCode);

   Modify = 0;
   Dlg_base::GetDataFromBase();
   EnableWindow(GetDlgItem(HWindow, 406), FALSE);
}

void PrivateDlg::SetButtons()
{
   Dlg_base::SetButtons();
}

int PrivateDlg::SetDataToBase()
{
   Dlg_base::SetDataToBase();

   (*base)(0, buffer.Seria);
   if(!buffer.Seria[0])
    {
       SetFocus(e[0]->HWindow);
       return 0;
    }
   (*base)(1, atof(buffer.Number));
   if(!buffer.Number[0])
    {
       SetFocus(e[1]->HWindow);
       return 0;
    }

   BOOL Bad = (strlen(buffer.Insured) == 0) || (!strcmp("ИСПОРЧЕН", buffer.Insured));
   if(Bad) {
      if(MessageBox(HWindow, "Сохранить полис как испорченный ?", "Отвечай поскорей", MB_YESNO | MB_ICONINFORMATION | MB_SYSTEMMODAL) == IDYES)
      {
        strcpy(buffer.Insured, "ИСПОРЧЕН");
	buffer.Birthdate[0] = 0;
	*buffer.Destination = 0;
        *buffer.Summa = 0;
	*buffer.Count = 0;
	*buffer.Mans = 0;
	*buffer.Old = 0;
	*buffer.Child = 0;
	*buffer.Premium = 0;
        *buffer.Begin = 0;
	*buffer.Period = 0;
	*buffer.End = 0;
	*buffer.Address = 0;
      }
      else return 0;
   }

   (*base)(2, buffer.Insured);
   (*base)(3, buffer.Address);

   DATE date;
   if(buffer.Birthdate[0])
    {
      date = GD(buffer.Birthdate);
      if(!date)
       {
          SetFocus(e[5]->HWindow);
          return 0;
       }
     }
   else date = BLANKDATE;
   (*base)(4, date);
   (*base)(5, buffer.Destination);
   if(!buffer.Destination[0] && !Bad)
    {
       SetFocus(e[6]->HWindow);
       return 0;
    }

   char YP[2] = "G";
   if(buffer.Year[1])
    {
     for(int i = 0; i < 3; i++)
      if(!buffer.Plan[i]) break;
     YP[0] = 'A' + i;
    }
   else {
	if(YearCombo->GetSelIndex() == 1) YP[0] = 'S';
	if(YearCombo->GetSelIndex() == 2) YP[0] = 'T';
	YearCombo->GetText(buffer.Period, sizeof buffer.Period);
   }

   if(Bad) *YP = 0;

   (*base)(6, YP);

   (*base)(7, atof(buffer.Summa));
   (*base)(8, buffer.Period);
   if(*buffer.Begin && !Bad) {
      date = GD(buffer.Begin);
      if(!date)
       {
          SetFocus(e[10]->HWindow);
          return 0;
       }
   }
   else date = BLANKDATE;

   (*base)(9, date);
   if(*buffer.End && !Bad) {
     date = GD(buffer.End);
     if(!date)
      {
         SetFocus(e[11]->HWindow);
         return 0;
      }
   }
   else date = BLANKDATE;
   (*base)(10, date);
   (*base)(11, atof(buffer.Premium));
   if(*buffer.Payment) {
      date = GD(buffer.Payment);
      if(!date)
       {
          SetFocus(e[14]->HWindow);
          return 0;
       }
   }
   else date = BLANKDATE;
   (*base)(12, date);
   (*base)(13, buffer.Conditions);
   (*base)(14, buffer.Place);
   date = GD(buffer.Date);
   if(!date)
    {
       SetFocus(e[17]->HWindow);
       return 0;
    }
   (*base)(15, date);
   (*base)(16, atof(buffer.Count));
   if(atoi(buffer.Count) > 0 && atoi(buffer.Mans) == 0)
	 strcpy(buffer.Mans, buffer.Count);
   (*base)(17, buffer.Company);
   (*base)(18, buffer.AdrCompany);
   (*base)(19, atof(buffer.Mans));
   (*base)(20, atof(buffer.Old));
   (*base)(21, atof(buffer.Child));

   int Index = AgName->GetSelIndex();
   if(Index == -1) {
      MessageBox(HWindow, "Введи агента", "Ошибка", MB_OK | MB_ICONSTOP);
      return 0;	
   }
   char AgCode[10];
   wsprintf(AgCode, "%04d", SendMessage(AgName->HWindow, CB_GETITEMDATA, Index, 9));
   (*base)(22, AgCode);

   (*base)(23, atof(buffer.Valuta));
   char CurrCode[4];
   CurrencyCode->GetText(CurrCode, sizeof CurrCode);
   (*base)(24, CurrCode);

   return 1;
}

PrivateDlg::~PrivateDlg()
{
   EnableMenuItem(GetSubMenu(GetMenu(Parent->HWindow), 2), 1, MF_BYPOSITION | MF_ENABLED);
}

void PrivateDlg::Find()
{
   base->Refresh();
   char b[100];
   if(e[2]->GetText(b, sizeof b)) {
       Array Strings(10, 0, 10);
       //(*base)(3, b);
       base->First();
       char str[100], seria[15], owner[100];
       //double number;
       Process*	findWnd = base->GetRecords() > 50 ? (Process*)GetModule()->MakeWindow(new Process(this)) : 0;
       SetCursor(LoadCursor(0, IDC_WAIT));
       for(long i = 0; i < base->GetRecords(); i++) {
	   base->Get();
	   double number;
	   (*base)(seria, sizeof seria, 0);
	   (*base)(owner, sizeof owner, 2);
	   if((strlen(owner) >= strlen(b) &&
	      !strncmpi(owner, b, strlen(b))) || (strstr(owner, b))) {
	       (*base)(number, 1);
	       wsprintf(str, "%s/%lu %s", seria, (long)number, owner);
	       Strings.add(*new String(str));
	       //(*base)(3, b);
	       if(findWnd) findWnd->SetAll(Strings.getItemsInContainer());
	   }
	   base->Next();
	   if(findWnd) findWnd->SetPercent(i * 100 / base->GetRecords());
       }
       if(findWnd) delete findWnd;
       int nCount = Strings.getItemsInContainer();
       if(nCount == 0) {
	 MessageBox(HWindow, "Такого страхователя в базе нет.", "Сообщаю", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
	 return;
       }
       else {
	  strcpy(str, (LPSTR)(LPCSTR)(String&)(Strings[0]));
	  if(nCount > 1)
	   if(GetModule()->ExecDialog(new SelectStr(this, Strings, str)) != IDOK) return;
       }
       (*base)(0, strtok(str, "/"));
       //char* sss =strtok(0, " ");MessageBox(0, sss, sss, 0);
       (*base)(1, atof(strtok(0, " ")));
       if(!PXSrchKey(base->getTblHandle(), base->getRecHandle(), 2, SEARCHFIRST)) {
	   base->Get();
	   GetDataFromBase();
       }
   }
   else
   if(e[0]->GetText(b, sizeof b)) {
	(*base)(0, b);
	e[1]->GetText(b, sizeof b);
	(*base)(1, atof(b));

   	if(!PXSrchKey(base->getTblHandle(), base->getRecHandle(), 2, SEARCHFIRST))
    	{
      		base->Get();
      		GetDataFromBase();
    	}
	else MessageBox(GetApplication()->MainWindow->HWindow, GetRCStr(523), "Сообщаю", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
   }
   else
   MessageBox(GetApplication()->MainWindow->HWindow, "Не указано поле для поиска данных.", "Сообщаю", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
}
*/
char* ArrStr[19] = {
                     "one",
                     "two",
                     "three",
                     "four",
                     "five",
                     "six",
                     "seven",
                     "eight",
                     "nine",
                     "ten",
                     "eleven",
                     "twelve",
                     "thirteen",
                     "fourteen",
                     "fivteen",
                     "sixteen",
                     "seventeen",
                     "eighteen",
                     "nineteen"
                  };

char* Ten[8] = {
                 "twenty",
                 "thirty",
                 "fourty",
                 "fivty",
                 "sixty",
                 "seventy",
                 "eighty",
                 "ninety"
               };

void _1000(char* dest, int value, char* what)
{
   int tmp;
   if(value)
    {
       tmp = value / 100;
       if(tmp)
        {
          strcat(dest, ArrStr[tmp - 1]);
          strcat(dest, " hundred ");
        }
       tmp = (value % 100);
       if(tmp)
        {
          if(tmp < 20)
           {
             strcat(dest, ArrStr[tmp - 1]);
             strcat(dest, " ");
           }
          else
           {
             strcat(dest, Ten[tmp / 10 - 2]);
             strcat(dest, " ");
             if(tmp % 10)
              {
                strcat(dest, ArrStr[tmp % 10 - 1]);
                strcat(dest, " ");
              }
           }
        }

       strcat(dest, what);
       strcat(dest, " ");
    }
}

void SetSumma(char* dest, char* val, char* curr)
{
   long value = atol(val);
//   strcat(dest, val);
//   strcat(dest, " ( ");
   _1000(dest, value / 1000000L, "million");
   _1000(dest, (value % 1000000L) / 1000, "thousand");
   _1000(dest, value % 1000L, "");

   strcat(dest, curr);
//   if((value % 10) != 1) strcat(dest, "s");
   strcat(dest, " ");
   if(atof(val) - atol(val))
    {
      int cent = int((atof(val) * 100 - atol(val) * 100));
      wsprintf(dest + strlen(dest), "%u", cent);
      strcat(dest, " cent");
      if((cent % 10) != 1) strcat(dest, "s");
    }
//   strcat(dest, " )");
}
/*
void PrivateDlg::Print1()
{
   if(!SetDataToBase()) return;
   Dlg_base::SetDataToBase();

   char* str = new char [3000];
   char* dest = str;

   memset(str, 0, 2800);

   strcpy(dest, buffer.Company);
   dest += strlen(dest) + 1;
   strcpy(dest, buffer.AdrCompany);
   dest += strlen(dest) + 1;

   strcpy(dest, buffer.Seria);
   dest += strlen(dest) + 1;
   strcpy(dest, buffer.Number);
   dest += strlen(dest) + 1;

   strcpy(dest, buffer.Insured);
   dest += strlen(dest) + 1;
   strcpy(dest, buffer.Address);
   dest += strlen(dest) + 1;
   strcpy(dest, buffer.Birthdate);
   dest += strlen(dest) + 1;
   strcpy(dest, buffer.Destination);
   if(!buffer.Destination[0])
    {
       SetFocus(e[6]->HWindow);
       return;
    }
   dest += strlen(dest) + 1;
   strcpy(dest, (buffer.Year[0] && !YearCombo->GetSelIndex()) ? "X" : " ");
   if(buffer.Year[0]) 
       YearCombo->GetText(buffer.Period, sizeof buffer.Period);
   dest += strlen(dest) + 1;
   strcpy(dest, buffer.Year[1] ? "X" : " ");
   dest += strlen(dest) + 1;
   for(int i = 0; i < 3; i++)
    {
      strcpy(dest, buffer.Plan[i] ? "X" : " ");
      dest += strlen(dest) + 1;
    }

   SetSumma(dest, buffer.Summa);
   dest += strlen(dest) + 1;

   strcpy(dest, strtok(buffer.Period, " "));
   dest += strlen(dest) + 1;
   strcpy(dest, strtok(0, " "));
   dest += strlen(dest) + 1;
   strcpy(dest, buffer.Begin);
   dest += strlen(dest) + 1;
   strcpy(dest, buffer.End);
   dest += strlen(dest) + 1;

   SetSumma(dest, buffer.Premium);
   dest += strlen(dest) + 1;

   //Разбить дату
   strcpy(dest, strtok(buffer.Payment, "./"));
   dest += strlen(dest) + 1;

   strcpy(dest, strtok(0, "./"));
   dest += strlen(dest) + 1;

   strcpy(dest, strtok(0, "./"));
   memmove(dest, dest + strlen(dest) - 1, strlen(dest));
   dest += strlen(dest) + 1;

   strcpy(dest, buffer.Conditions);
   dest += strlen(dest) + 1;

   strcpy(dest, buffer.Place);
   dest += strlen(dest) + 1;

   strcpy(dest, strtok(buffer.Date, "./"));
   dest += strlen(dest) + 1;
   strcpy(dest, strtok(0, "./"));
   dest += strlen(dest) + 1;
   strcpy(dest, strtok(0, "./"));
   memmove(dest, dest + strlen(dest) - 1, strlen(dest));

   ((TRulerWin*)Parent)->SetValues(str);
   Dlg_base::Print1();
   delete str;
}

void PrivateDlg::Calculate(RTMessage msg)
{
   char buff[100];
   int  d, m, y;
   DATE date;

   e[3]->GetText(buff, sizeof buff);
   int count = atoi(buff);
   if(count < 1) count = 1;

   BOOL Enable = count > 1;
   //EnableWindow(GetDlgItem(HWindow, 1001), Enable);
   EnableWindow(GetDlgItem(HWindow, 1000), Enable);
   EnableWindow(GetDlgItem(HWindow, 220), Enable);
   //EnableWindow(GetDlgItem(HWindow, 222), Enable);


   e[20]->GetText(buff, sizeof buff);
   int Mans = atoi(buff);
   if(Mans<1)Mans=count;
   e[21]->GetText(buff, sizeof buff);
   int Old = atoi(buff);
   e[22]->GetText(buff, sizeof buff);
   int Child = atoi(buff);

   if(Mans > count) {
       MessageBox(HWindow, "Количество человек, на которых выписывается полис\nне может превышать размер группы!", "Ошибка", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
       e[20]->GetText(buff, sizeof buff);
       if(strlen(buff)) buff[strlen(buff) - 1] = 0;
       e[20]->SetText(buff);
       SetFocus(e[20]->HWindow);
       return;
   }  
   if(Old + Child > Mans) {
       MessageBox(HWindow, "Количество детей и стариков не может превышать количество\nчеловек, на которых выписывается полис!", "Ошибка", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
       SetFocus(e[21]->HWindow);
       return;
   }  

   //Сумма
   int Year = ((PTCheckBox)ChildWithId(300))->GetCheck() == BF_CHECKED;

   char cond[100];
   char val[100];

   char Plan = 'A';
   for(int i = 302; i < 305; i++)
     {
        PTCheckBox cb = (PTCheckBox)ChildWithId(i);
        if(cb->GetCheck() == BF_UNCHECKED) break;
        else Plan++;
     }

   //Для испорченных
   //if(Plan > 'C' && !Year) return;
  
   char _103[100];

   if(Year)
    {
      SendDlgItemMessage(HWindow, 103, WM_SETTEXT, 0, (long)"");
//      int Index = YearCombo->GetSelIndex();
  //    char* Sect = Index == 0 ? "Year_summ" : Index == 1 ? "6Month_premium" : "3Month_premium";
      GetPrivateProfileString("PRIVATE", "Year_summ", "", cond, sizeof cond, ININame);
    }
   else
    {
      wsprintf(_103, "Выриант %c, Сумма ", Plan);
      wsprintf(cond, "Plan_%c", Plan);
      GetPrivateProfileString("PRIVATE", cond, "", cond, sizeof cond, ININame);
      strcat(_103, cond);
      strcat(_103, " USD");
    }
   //Вычисление суммы страхования и формулы
   makeCond(cond, val, Mans, -1, -1, atof(cond));
   e[8]->SetText(cond);
   e[7]->SetText(val);
   
   if(Year) {
      int Index = YearCombo->GetSelIndex();
      char* Sect = Index == 0 ? "Year_premium" : Index == 1 ? "6Month_premium" : "3Month_premium";
      GetPrivateProfileString("PRIVATE", Sect, "", cond, sizeof cond, ININame);
    }
   else
    {
      if(((PTComboBox)e[9])->GetString(buff, ((PTComboBox)e[9])->GetSelIndex()) < 1) return;
      sprintf(cond, "%2.2f", GetPremiumFromIni(buff, Plan));
      strcat(_103, ", Премия ");
      strcat(_103, cond);
      SendDlgItemMessage(HWindow, 103, WM_SETTEXT, 0, (long)_103);
    }
   //Вычисление суммы  премии и формулы
   makeCond(cond, val, Mans, Old, Child, atof(cond));
   e[12]->SetText(val);
   e[13]->SetText(cond);
   SetSummaCurr(msg);

   //конец страховки
   e[10]->GetText(buff, sizeof buff);
   date = GD(buff, 1);
   if(!date) return;
   PXDateDecode(date, &m, &d, &y);
   if(!Year) {
      if(((PTComboBox)e[9])->GetString(buff, ((PTComboBox)e[9])->GetSelIndex()) < 1) return;
   }
   else
      if(YearCombo->GetText(buff, 15) < 1) return;

   int inc = 0;
   for(i = 0; (i < strlen(buff)) && (!inc); i++)
    {
      if(!strncmpi(buff + i, "day", 3) || !strncmpi(buff + i, "дн", 2) || !strncmpi(buff + i, "де", 2)) inc = 1;
      if(!strncmpi(buff + i, "mon", 3) || !strncmpi(buff + i, "мес", 3)) inc = 2;
      if(!strncmpi(buff + i, "year", 4) || !strncmpi(buff + i, "год", 3)) inc = 3;
    }

   if(!inc)
    { err:
      MessageBox(0, "Ошибка в файле конфигурации (Period)", "Cообщение", MB_OK | MB_SYSTEMMODAL | MB_ICONINFORMATION);
      return;
    }

   int How_many = atoi(buff);
   if(!How_many) goto err;

   if(inc == 3) PXDateEncode(m, d, y + How_many, &date); //Year
   if(inc == 2) //month
    {
      m += How_many;
      if(m > 12)
       {
          m -= 12;
          y++;
       }
      while((PXDateEncode(m, d, y, &date) != PXSUCCESS) && (d > 1)) d--;
    }
   if(inc == 1) date += How_many;

   PXDateDecode(--date, &m, &d, &y);

   wsprintf(buff, "%02u.%02u.%04u", d, m, y);
   e[11]->SetText(buff);
}

void PrivateDlg::Period(RTMessage msg)
{
   if((msg.LP.Hi == LBN_SELCHANGE) && Modify)
    PostMessage(HWindow, 1, 0, 0);
}

void PrivateDlg::Vozr(RTMessage msg)
{
//   if((msg.LP.Hi == EN_CHANGE) && Modify)
//    PostMessage(HWindow, 1, 0, 0);
}

void PrivateDlg::How(RTMessage msg)
{
   if((msg.LP.Hi == EN_CHANGE) && Modify)
    PostMessage(HWindow, 1, 0, 0);
}

void PrivateDlg::Number1(RTMessage msg)
{
//   msg.LP.Hi = EN_CHANGE;
   How(msg);
}

void PrivateDlg::Number2(RTMessage msg)
{
//   msg.LP.Hi = EN_CHANGE;
   How(msg);
}

void PrivateDlg::Number3(RTMessage msg)
{
//   msg.LP.Hi = EN_CHANGE;
   How(msg);
}

void PrivateDlg::Begin(RTMessage msg)
{
   if((msg.LP.Hi == EN_CHANGE) && Modify)
    PostMessage(HWindow, 1, 0, 0);
}

double GetPremiumFromIni(char* entry, char Plan)
{
   char   buff[100];
   double ret = 0;

   for(int i = 1;; i++)
    {
       wsprintf(buff, "Period%u", i);
       if(!GetPrivateProfileString("PRIVATE", buff, "", buff, sizeof buff, ININame)) break;
       if(strstr(buff, entry))
        {
	   char* ch = buff;
           for(int i = 0; i <= Plan - 'A'; i++)
            {
              ch = strchr(ch, ',');
              if(!ch) break;
              ch++;
            }

           if(ch) ret = atof(ch + 1);
           break;
        }
    }
   if(!ret) MessageBox(0, "Ошибка в файле конфигурации (Period)", entry, MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
   return ret;
}

void  PrivateDlg::Number(RTMessage)
{
    char buff[100];
    e[1]->GetText(buff, sizeof buff);
    for(int i = 0; buff[i]; i++)
     if(buff[i] < '0' || buff[i] > '9')
      {
         SetFocus(e[1]->HWindow);
         MessageBeep(0);
      }
}

/////////////////////////////////////

void  PrivateDlg::Clear()
{
   for(int i = 0; i < 18; i++)
     if(i == 9) ((PTComboBox)e[i])->SetSelIndex(((PTComboBox)e[i])->GetCount() - 1);
     else e[i]->SetText("");

   for(i = 20; i < 23; i++)
     e[i]->SetText("");

   date d;
   getdate(&d);
   char buff[100];
   wsprintf(buff, "%02u.%02u.%04u", d.da_day, d.da_mon, d.da_year);
   e[10]->SetText(buff);
   e[14]->SetText(buff);
   e[17]->SetText(buff);
   GetPrivateProfileString("PRIVATE", "Place_of_issue", "", buff, sizeof buff, ININame);
   e[16]->SetText(buff);

   GetPrivateProfileString("PRIVATE", "Insurant", "", buff, sizeof buff, ININame);
   e[18]->SetText(buff);
   GetPrivateProfileString("PRIVATE", "Ins_address", "", buff, sizeof buff, ININame);
   e[19]->SetText(buff);

   e[3]->SetText("1");
   e[20]->SetText("1");
   GetPrivateProfileString("PRIVATE", "Seria", "", buff, sizeof buff, ININame);
   e[0]->SetText(buff);


   SetFocus(e[0]->HWindow);

   TMessage msg;
   Change(msg);
}

void  PrivateDlg::NoAutoCheckYN1(RTMessage)
{
   if(((PTCheckBox)ChildWithId(300))->GetCheck() != BF_CHECKED)
    {
      ((PTCheckBox)ChildWithId(301))->Uncheck();
      ((PTCheckBox)ChildWithId(300))->Check();
      PostMessage(HWindow, WM_COMMAND, 199, 0); //Calc
      ShowWindow(YearCombo->HWindow, TRUE);
      ShowWindow(e[9]->HWindow, FALSE);
    }
}

void  PrivateDlg::NoAutoCheckYN2(RTMessage)
{
   if(((PTCheckBox)ChildWithId(301))->GetCheck() != BF_CHECKED)
    {
      ((PTCheckBox)ChildWithId(300))->Uncheck();
      ((PTCheckBox)ChildWithId(301))->Check();
      PostMessage(HWindow, WM_COMMAND, 199, 0); //Calc
      ShowWindow(YearCombo->HWindow, 0);
      ShowWindow(e[9]->HWindow, 1);
   }
}

void  PrivateDlg::NoAutoCheck1(RTMessage)
{
   ((PTCheckBox)ChildWithId(302))->Uncheck();
   ((PTCheckBox)ChildWithId(303))->Check();
   ((PTCheckBox)ChildWithId(304))->Check();
   PostMessage(HWindow, 1, 0, 0);
}

void  PrivateDlg::NoAutoCheck2(RTMessage)
{
   ((PTCheckBox)ChildWithId(302))->Check();
   ((PTCheckBox)ChildWithId(303))->Uncheck();
   ((PTCheckBox)ChildWithId(304))->Check();
   PostMessage(HWindow, 1, 0, 0);
}

void  PrivateDlg::NoAutoCheck3(RTMessage)
{
   ((PTCheckBox)ChildWithId(302))->Check();
   ((PTCheckBox)ChildWithId(303))->Check();
   ((PTCheckBox)ChildWithId(304))->Uncheck();
   PostMessage(HWindow, 1, 0, 0);
}

void  PrivateDlg::AuxDisen()
{
   SendDlgItemMessage(HWindow, 103, WM_SETTEXT, 0, (long)"");
   int Year = ((PTCheckBox)ChildWithId(300))->GetCheck() == BF_CHECKED;

   for(int i = 302; i <= 304; i++)
    {
      PTCheckBox c = (PTCheckBox)ChildWithId(i);
      if(Year) c->Check();
      EnableWindow(c->HWindow, Year ? FALSE : TRUE);
    }
   EnableWindow(GetDlgItem(HWindow, 102), Year ? FALSE : TRUE);
   EnableWindow(GetDlgItem(HWindow, 209), Year ? FALSE : TRUE);
}

void  PrivateDlg::Disen(RTMessage msg)
{
   int Year = ((PTCheckBox)ChildWithId(300))->GetCheck() == BF_CHECKED;
   AuxDisen();
   if(!Year) NoAutoCheck1(msg);
   else
    {
       //PTComboBox cb = (PTComboBox)ChildWithId(209);
       //cb->SetSelIndex(cb->GetCount() - 1);
    }
   PostMessage(HWindow, 1, 0, 0);
}

void  PrivateDlg::makeCond(char* cond, char* val, int Mans, int Old, int Child, double res)
{
   char count_buf[40];

   e[3]->GetText(count_buf, sizeof count_buf);
   int count = atoi(count_buf);
   if(!count) count = 1;


   if(Old == -1) {
   	double sum = res * Mans;
   	if(sum - long(sum))
	   sprintf(val, "%2.2f USD", sum);
	else
	   sprintf(val, "%lu USD", (long)sum);

	sprintf(cond, "(%2.2f$)*%uчел", res, Mans);
	return;
   }

   //Skidka
   double K = 1;
	  // MessageBox(0, "111", "", 0);

   if(count > 2) {

   //Read from ini file
	  // MessageBox(0, "222", "", 0);
      for(int i = 1;1;i++) {
	   char Key[25];
	   wsprintf(Key, "Reduction%u", i);
	   char buff[20];
	   //MessageBox(0, Key, "", 0);
	   if(!GetPrivateProfileString("PRIVATE", Key, "", buff, sizeof buff, ININame))
	      break;
	   if(count > atoi(buff)) {
		char* Div = strchr(buff, ',');
		if(!Div)
		    MessageBox(0, "Ошибка в файле blank.ini", Key, MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
                else 
		    K = atof(Div + 1);

		break;
           }
      }

//    old code
//    if(count > 50) K = 0.9;
//    else if(count > 20) K = 0.925;
//    else if(count >= 10) K = 0.95;
   }
	
   double sum = res * Mans;

   ////////////
   char buff[20];
   GetPrivateProfileString("PRIVATE", "OldManK", "", buff, sizeof buff, ININame);
   if(Old) {
       sum += res * Old * (atof(buff) - 1.);
   }
   GetPrivateProfileString("PRIVATE", "ChildK", "", buff, sizeof buff, ININame);
   if(Child) {
       sum -= (res * Child) * atof(buff);
   }

   ///////////


   sum *= K;

   if(sum - long(sum))
      sprintf(val, "%2.2f USD", sum);
   else
      sprintf(val, "%lu USD", (long)sum);

   sprintf(cond, "(%2.2f$)*%uчел", res, Mans);
   if(Old) sprintf(cond + strlen(cond), " + (%2.2f$)*%u 61год", res, Old);
   if(Child) sprintf(cond + strlen(cond), " - (%2.2f$)*%u до16лет", res, Child);
   if(K<1) {
      char buffer[255];
      sprintf(buffer, "(%s)*%2.1f%", cond, K*100);
      strcpy(cond, buffer);
   }
}

void  PrivateDlg::Help(RTMessage msg)
{
   if(msg.WParam == 1000)
      WinHelp(GetApplication()->MainWindow->HWindow, "blank.hlp", HELP_CONTEXT, HELP_PRIVATE);

   Dlg_base::Help(msg);
}

void PrivateDlg::WMCtlColor(RTMessage msg)
{
   Dlg_base::WMCtlColor(msg);
   if(msg.LP.Hi == CTLCOLOR_STATIC && (HWND)msg.LP.Lo == GetDlgItem(HWindow, 103)) 
	SetTextColor((HDC)msg.WParam, RGB(255, 0, 0));
   if(msg.LP.Hi == CTLCOLOR_STATIC && (HWND)msg.LP.Lo == GetDlgItem(HWindow, 1002))
	SetTextColor((HDC)msg.WParam, RGB(0, 0, 255));
}

void PrivateDlg::Change(RTMessage msg)
{
   Modify = 1;
   EnableWindow(GetDlgItem(HWindow, 406), TRUE);
   Calculate(msg);
   //How(msg);
}

int PrivateDlg:: Update()
{
   // Если операция обновления, то перезаписать агента старого
//   (*base)(22, OLDAGENT);
//   SetAgent(OLDAGENT, agents, AgName); //MessageBox(0, "Save", "update", 0);
}

void  PrivateDlg::SetSummaCurr(RTMessage)
{
    char buffer [100];
    GetDlgItemText(HWindow, 212, buffer, sizeof buffer);
    char* Pos = strchr(buffer, ' ');
    if(Pos == 0) return;
    *Pos = 0;
    SetDlgItemText(HWindow, 571, buffer);
    //MessageBox(0, Pos, Pos, 0);
    Pos++;
    while(*Pos && *Pos == ' ') Pos++;
    CurrencyCode->SetText(Pos);
    //MessageBox(0, Pos, Pos, 0);
}
*/