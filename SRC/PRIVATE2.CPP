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
#include <private2.h>
#include <checkbox.h>
#include <thwindow.h>
#include <help.h>
#include <stdlib.h>
#include <stdio.h>
#include "util.h"
#include "radiobut.h"
#include "grncard.h"
#include "date.h"
#include "impexp.h"

#define max(a, b) ((a) > (b) ? (a) : (b))

static const char* FORMSECTION = "PRIVATE2";
void AlignCombo(TComboBox*);

void InitVal2(TComboBox* cb);
void SetAgent(char* ACode, TOpenBase& agents, PTComboBox AgName, int);

char* Prvt2Fields[] = {
                        "Seria",
                        "Number",
                        "Insurer",
                        "Address",
                        "Birthdate",
			"Passport",
                        "State", 
			"Conditions",
                        "Insurer2",
                        "Begin",
			"End", //10
                        "Duration",
			"AllSumma",
                        "Tarif",
			"Payment",
			"PaymentDate",
			"Group",
                        "WriteDate",
			"Agent_code",
			"Cash",
			"InsType", //20
			"Period1",
			"Period2",
			"Ks",
			"Sum1",
			"Sum1Curr",
			"Sum2",
			"Sum2Curr",
			"AgPercent",   //28
			"Uridich",      //29
			"Restrax", //30
			"Insurtype",
			"StopDate",
			"RetSum",
			"RetCur",
			"St"  //35
		     };

#define STATE_NORMAL "0"
#define STATE_BAD    "1"
#define STATE_STOP   "2"

int  Prvt2Count = 36;
char* Prvt2Name = "\\bases\\private2";

/*class Edit : public TEdit
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
*/
int CanModifyPrivate2;

Private2Dlg::Private2Dlg(PTWindowsObject p)
	   : Dlg_base(p, GetSystemMetrics(SM_CXSCREEN) == 640 ? "LEBEN2_640" : "LEBEN2_800", Prvt2Name, Prvt2Fields, Prvt2Count),
             agents(AgentDBName, AgentFields, AgFieldsCount)
{
     CanModifyPrivate2 = 0;

     m_Seria = new TEdit(this, 200, 3);
     m_Number = new NumberEdit(this, 201, 11, 0);
     m_Insurer = new TEdit(this, 203, 66);
     m_InsurerBirthDate = new TEdit(this, 204, 11);
     m_InsurerAddress = new TEdit(this, 205, 66);
     m_Passport = new TEdit(this, 206, 41);
     m_State = new TEdit(this, 210, 81);
     m_Conditions = new TEdit(this, 211, 251);
     m_Insurer2 = new TEdit(this, 212, 66);
     m_FromDate = new TEdit(this, 213, 11);
     m_ToDate = new TEdit(this, 214, 11);
     m_Duration = new NumberEdit(this, 215, 31, 0);
     m_AllSumma = new NumberEdit(this, 216, 31, 1);
     m_Tarif = new NumberEdit(this, 217, 31, 1);
     m_Payment = new NumberEdit(this, 218, 31, 1);
     m_PaymentDate = new TEdit(this, 222, 11);
	 m_WriteDate = new TEdit(this, 223, 11);
     m_Sum1 = new NumberEdit(this, 225, 20, 1);
	 m_Sum2 = new NumberEdit(this, 226, 20, 1);
     m_Group = new NumberEdit(this, 228, 3, 0);

     new NumberEdit(this, 53, 5, 1);
     new TCheckBox(this, 54, 0);
	 new TCheckBox(this, 56, 0);

	 m_StopDate = new TEdit(this, 57, 11);
	 m_RetSum = new NumberEdit(this, 59, 10, 1);

     m_AgentCombo = new TComboBox(this, 202, 1);
     m_AgentCombo->DisableTransfer();
     m_Vid1Combo = new TComboBox(this, 207, 1);
     m_Vid1Combo->DisableTransfer();
     m_Vid2Combo = new TComboBox(this, 208, 1);
     m_Vid2Combo->DisableTransfer();
     m_Vid3Combo = new TComboBox(this, 209, 1);
     m_Vid3Combo->DisableTransfer();
     m_NalBezNalCombo = new TComboBox(this, 224, 1);
     m_NalBezNalCombo->DisableTransfer();
     m_Sum1Combo = new TComboBox(this, 219, 1);
     m_Sum1Combo->DisableTransfer();
     m_Sum2Combo = new TComboBox(this, 227, 1);
     m_Sum2Combo->DisableTransfer();
	 m_RetSumCombo = new TComboBox(this, 58, 1);
	 m_RetSumCombo->DisableTransfer();

     m_Ks = new TListBox(this, 220, 0);
     m_Ks->DisableTransfer();

     m_Restrax = new TComboBox(this, 55, 1);
     m_Restrax->DisableTransfer();
}

void SetLittleFont(HWND, short*, int);
void _FillAgents(TOpenBase* agents, TComboBox* AgName, int index);
void DateFBToStr(TOpenBase* base, int field, char* buffer);

void Private2Dlg::SetState(char* state)
{
	static char* SrcDlgName = 0;
	if(!SrcDlgName) {
		SrcDlgName = new char[32];
		GetWindowText(HWindow, SrcDlgName, 32);
	}

    DynStr s;
    if(*state == *STATE_NORMAL)
		SetWindowText(HWindow, SrcDlgName);
	if(*state == *STATE_BAD) {
		sprintf(s, "%s [%s]", SrcDlgName, "ИСПОРЧЕН");
		SetWindowText(HWindow, s);
	}
	if(*state == *STATE_STOP) {
		sprintf(s, "%s [%s]", SrcDlgName, "РАСТОРГНУТ");
		SetWindowText(HWindow, s);
	}
}

void Private2Dlg::SetupWindow()
{
short Numb[] = { 300, 301, 405, 202, 302 };

   Dlg_base::SetupWindow();

	InitVal2(m_Sum1Combo);
	InitVal2(m_Sum2Combo);
	InitVal2(m_RetSumCombo);
/*   m_Sum1Combo->AddString("USD");
   m_Sum1Combo->AddString("DM");
   m_Sum1Combo->AddString("EUR");
   m_Sum1Combo->AddString("RUR");
   m_Sum1Combo->AddString("BRB");

   m_Sum2Combo->AddString("");
   m_Sum2Combo->AddString("USD");
   m_Sum2Combo->AddString("DM");
   m_Sum2Combo->AddString("EUR");
   m_Sum2Combo->AddString("RUR");
   m_Sum2Combo->AddString("BRB");

   m_RetSumCombo->AddString("");
   m_RetSumCombo->AddString("USD");
   m_RetSumCombo->AddString("EUR");
   m_RetSumCombo->AddString("RUR");
   m_RetSumCombo->AddString("BRB");
*/
   m_Vid1Combo->AddString("A1");
   m_Vid1Combo->AddString("A2");
   m_Vid1Combo->AddString("B");
   m_Vid1Combo->AddString("C");
   m_Vid1Combo->AddString("Y  1 Год");
   m_Vid1Combo->AddString("S  6 Месяцев");
   m_Vid1Combo->AddString("Q  3 Месяца");

   m_NalBezNalCombo->AddString("Наличные");
   m_NalBezNalCombo->AddString("Безналичные");

   char buff[48];
   for(int i = 0;; i++) {
       sprintf(buff, "Assist%d", i);
       if(!GetPrivateProfileString(FORMSECTION, buff, "", buff, sizeof buff, ININame)) break;
       m_Restrax->AddString(buff);
   }

   _FillAgents(&agents, m_AgentCombo, LEBEN2_TBL);
   SetLittleFont(HWindow, Numb, sizeof(Numb) / sizeof(Numb[0]));

   if(!base->GetRecords())
     PostMessage(HWindow, WM_COMMAND, 405, MAKELONG(405, BN_CLICKED));

   FillKs();

   GetDataFromBase();
}

extern void DblToStr(char* str, double V, int Digit = 2);
extern DATE GD(char* str, int = 0);

void Private2Dlg::GetDataFromBase()
{
   memset(&buffer, 0, sizeof buffer);
   (*base)(buffer.Seria, sizeof buffer.Seria, 0);
   double value;
   (*base)(value, 1);
   sprintf(buffer.Number, "%lu", (long)value);
   (*base)(buffer.Insurer, sizeof buffer.Insurer, 2);
   (*base)(buffer.Address, sizeof buffer.Address, 3);
   (*base)(buffer.BirthDate, sizeof buffer.BirthDate, 4);
   (*base)(buffer.Passport, sizeof buffer.Passport, 5);
   (*base)(buffer.State, sizeof buffer.State, 6);
   (*base)(buffer.Conditions, sizeof buffer.Conditions, 7);
   (*base)(buffer.Insurer2, sizeof buffer.Insurer2, 8);

   DATE date;
   int m, d, y;
   (*base)(date, 9);
   if(!ISBLANKDATE(date)) {
     PXDateDecode(date, &m, &d, &y);
     sprintf(buffer.FromDate, "%02u.%02u.%04u", d, m, y);
   }

   (*base)(date, 10);
   if(!ISBLANKDATE(date)) {
     PXDateDecode(date, &m, &d, &y);
     sprintf(buffer.ToDate, "%02u.%02u.%04u", d, m, y);
   }

   (*base)(value, 11);
   if((long)value > 0)
   	sprintf(buffer.Duration, "%lu", (long)value);

   (*base)(value, 12);
   DblToStr(buffer.AllSumma, value);

   (*base)(value, 13);
   DblToStr(buffer.Tarif, value);

   (*base)(value, 14);
   DblToStr(buffer.Payment, value);

   (*base)(date, 15);
   PXDateDecode(date, &m, &d, &y);
   sprintf(buffer.PaymentDate, "%02u.%02u.%04u", d, m, y);

   (*base)(value, 16);
   DblToStr(buffer.Group, value);

   (*base)(date, 17);
   if(!ISBLANKDATE(date)) {
      PXDateDecode(date, &m, &d, &y);
      sprintf(buffer.WriteDate, "%02u.%02u.%04u", d, m, y);
   }

   char AgentStr[5];
   (*base)(AgentStr, sizeof AgentStr, 18);
   SetAgent(AgentStr, agents, m_AgentCombo, 0);

   int IsBad = *buffer.FromDate == 0;

   m_NalBezNalCombo->SetSelIndex(-1);
   if(!IsBad) {
       (*base)(value, 19);
       m_NalBezNalCombo->SetSelIndex(value > 0 ? 0 : 1);
   }

   m_Restrax->SetSelIndex(-1);
//   if(!IsBad) {
       (*base)(value, 30);
       m_Restrax->SetSelIndex((int)value);
//   }

   m_Vid1Combo->SetSelIndex(-1);
   m_Ks->ClearList();
   if(!IsBad) {
       char InsType[3];
       (*base)(InsType, sizeof InsType, 20);
       m_Vid1Combo->SetText(InsType);
       if(InsType[0] == 'Y')
          m_Vid1Combo->SetSelIndex(4);
       if(InsType[0] == 'S')
          m_Vid1Combo->SetSelIndex(5);
       if(InsType[0] == 'Q')
	  m_Vid1Combo->SetSelIndex(6);

       FillKs();

       SetStatesControls(); //Enable/Disable

       FillCombo2();
       char buff[16];
       (*base)(buff, sizeof buff, 21);
       m_Vid2Combo->SetText(buff);

       FillCombo3();
       (*base)(buff, sizeof buff, 22);
       m_Vid3Combo->SetText(buff);
   }

   if(!IsBad) {
      char strStates[129];
      (*base)(strStates, sizeof strStates, 23);
      SetStatesKs(strStates);

      *strStates = 0;
      (*base)(strStates, sizeof strStates, 29);
      buffer.IsUridich = !strcmp("Y", strStates);

      double value;
      (*base)(value, 28);
      DblToStr(buffer.AgPercent, value);

	  (*base)(strStates, sizeof strStates, 31);
	  buffer.InsurType = *strStates == 'F';
   }

   m_Sum1Combo->SetSelIndex(-1);
   m_Sum2Combo->SetSelIndex(-1);
   m_RetSumCombo->SetSelIndex(-1);

   char str[5];
   if(!IsBad) {
       (*base)(value, 24);
	   DblToStr(buffer.Sum1, value);

	   //MessageBox(0, buffer.Sum1, buffer.Sum1, 0);

	   (*base)(str, sizeof str, 25);
       m_Sum1Combo->SetText(str);
       (*base)(value, 26);
       DblToStr(buffer.Sum2, value);
       (*base)(str, sizeof str, 27);
       m_Sum2Combo->SetText(str);

	   DateFBToStr(base, 32, buffer.StopDate);
	   (*base)(value, 33);
       DblToStr(buffer.RetSum, value);
	   (*base)(str, sizeof str, 34);
	   m_RetSumCombo->SetText(str);
   }

   (*base)(str, sizeof str, 35);
   SetState(str);

   CanModifyPrivate2 = 0;
   Dlg_base::GetDataFromBase();
   EnableWindow(GetDlgItem(HWindow, 406), FALSE);
}


int Private2Dlg::SetDataToBase()
{
   Dlg_base::SetDataToBase();

   (*base)(0, buffer.Seria);
   if(!buffer.Seria[0])
    {
       SetFocus(m_Seria->HWindow);
       return 0;
    }
   (*base)(1, atof(buffer.Number));
   if(!buffer.Number[0])
    {
       SetFocus(m_Number->HWindow);
       return 0;
    }

   BOOL IsBad = (strlen(buffer.Insurer) == 0) || (!strcmp("ИСПОРЧЕН", buffer.Insurer));
   if(IsBad) {
	  if(MessageBox(HWindow, GetRCStr(602), "Отвечай поскорей", MB_YESNO | MB_ICONINFORMATION | MB_SYSTEMMODAL) == IDYES)
      {
	strcpy(buffer.Insurer, "ИСПОРЧЕН");
	*buffer.BirthDate = 0;
	*buffer.Address = 0;
	*buffer.Passport = 0;
	*buffer.State = 0;
	*buffer.Conditions = 0;
	*buffer.Insurer2 = 0;
	*buffer.FromDate = 0;
	*buffer.ToDate = 0;
	*buffer.Duration = 0;
	*buffer.AllSumma = 0;
	*buffer.Tarif = 0;
	*buffer.Payment = 0;
	*buffer.WriteDate = 0;
	*buffer.Sum1 = 0;
	*buffer.Sum2 = 0;
	*buffer.StopDate = 0;
	*buffer.RetSum = 0;
		*buffer.Group = 0;
	m_Sum1Combo->SetSelIndex(-1);
	m_Sum2Combo->SetSelIndex(-1);
	m_RetSumCombo->SetSelIndex(-1);
	m_NalBezNalCombo->SetSelIndex(-1);
        m_Vid1Combo->SetSelIndex(-1);
      }
      else return 0;
   }

   (*base)(2, buffer.Insurer);
   (*base)(3, buffer.Address);
   (*base)(4, buffer.BirthDate);

   if(!buffer.Passport[0] && !IsBad)
    {
       SetFocus(m_Passport->HWindow);
       return 0;
    }
   (*base)(5, buffer.Passport);

   (*base)(6, buffer.State);
   (*base)(7, buffer.Conditions);
   if(!IsBad && *buffer.Insurer2 == 0) {
       SetFocus(m_Insurer2->HWindow);
       return 0;
   }
      
   (*base)(8, buffer.Insurer2);

   DATE date, fromdate, todate;
   if(!IsBad) {
      fromdate = date = GD(buffer.FromDate);
      if(!date)
       {
          SetFocus(m_FromDate->HWindow);
          return 0;
       }
   }
   else date = BLANKDATE;
   (*base)(9, date);

   if(!IsBad) {
	 todate = date = GD(buffer.ToDate);
     if(!date)
      {
         SetFocus(m_ToDate->HWindow);
	 return 0;
      }
      if(fromdate > date) {
	 SetFocus(m_ToDate->HWindow);
	 return 0;
      }
   }
   else date = BLANKDATE;
   (*base)(10, date);

   if(atof(buffer.Duration) < 1 && !IsBad)
   {
      SetFocus(m_Duration->HWindow);
      return 0;
   }
   (*base)(11, atof(buffer.Duration));
   (*base)(12, atof(buffer.AllSumma));
   if(atof(buffer.Tarif) < 1 && !IsBad)
   {
      SetFocus(m_Tarif->HWindow);
      return 0;
   }
   (*base)(13, atof(buffer.Tarif));
   if(atof(buffer.Payment) < 0.01 && !IsBad)
   {
      SetFocus(m_Payment->HWindow);
      return 0;
   }
   (*base)(14, atof(buffer.Payment));

   // if(!IsBad) {
       date = GD(buffer.PaymentDate);
	   if(!date)
	 {
	   SetFocus(m_PaymentDate->HWindow);
	   return 0;
	 }
      (*base)(15, date);
   //}

   (*base)(16, (IsBad ? 0 : max(1, atof(buffer.Group))));

   DATE _2005;
   PXDateEncode(1, 1, 2005, &_2005);

   if(!IsBad) {
	   date = GD(buffer.WriteDate);
	   if(!date || date >= _2005)
	 {
	   SetFocus(m_WriteDate->HWindow);
	   return 0;
	 }
      (*base)(17, date);
   }

   int Index = m_AgentCombo->GetSelIndex();
   if(Index == -1) {
      MessageBox(HWindow, "Введи агента", "Ошибка", MB_OK | MB_ICONSTOP);
      return 0;
   }

   char AgCode[10];

   long ag_code = SendMessage(m_AgentCombo->HWindow, CB_GETITEMDATA, Index, 0);
   //wsprintf(AgCode_, "%04d", SendMessage(m_Agent->HWindow, CB_GETITEMDATA, Index, 9));
   memmove(AgCode, &ag_code, 4);
   AgCode[4] = 0;
   (*base)(18, AgCode);

   if(!IsBad) {
     if(m_NalBezNalCombo->GetSelIndex() == -1) {
	SetFocus(m_NalBezNalCombo->HWindow);
	return 0;
      }
     (*base)(19, (double)(m_NalBezNalCombo->GetSelIndex() == 0));
   }

   if(!IsBad) {
     if(m_Vid1Combo->GetSelIndex() == -1) {
	SetFocus(m_Vid1Combo->HWindow);
	return 0;
      }
      char InsType[3];
      m_Vid1Combo->GetText(InsType, sizeof InsType);
      if(InsType[1] == ' ') InsType[1] = 0;
     (*base)(20, InsType);
   }

   if(!IsBad) { //Period 1, 2
      if(IsWindowEnabled(m_Vid2Combo->HWindow)) {
	  if(m_Vid2Combo->GetSelIndex() == -1) {
	     SetFocus(m_Vid2Combo->HWindow);
	     return 0;
	  }
	  char buff[16];
	  m_Vid2Combo->GetText(buff, sizeof buff);
	  (*base)(21, buff);
      }
      char buff[16];
      m_Vid3Combo->GetText(buff, sizeof buff);
     (*base)(22, buff);
   }

   if(!IsBad) {
      char strStates[129];
      GetStatesKs(strStates);
     (*base)(23, strStates);
   }

   if(!IsBad) {
       if(atof(buffer.Sum1) < 0.01) {
	  		SetFocus(m_Sum1->HWindow);
          return 0;
       }
       (*base)(24, atof(buffer.Sum1));
       char CurrCode[4];
       m_Sum1Combo->GetText(CurrCode, sizeof CurrCode);
       if(!*CurrCode) {
			SetFocus(m_Sum1Combo->HWindow);
			return 0;
       }
       (*base)(25, CurrCode);

       (*base)(26, atof(buffer.Sum2));
       m_Sum2Combo->GetText(CurrCode, sizeof CurrCode);
	   if(*CurrCode == 0 && atof(buffer.Sum2) >= 0.01 || *CurrCode && atof(buffer.Sum2) < 0.01)
       {
			SetFocus(m_Sum2Combo->HWindow);
			return 0;
       }
       (*base)(27, CurrCode);

       if(atof(buffer.AgPercent) > 50 || atof(buffer.AgPercent) < 0) {
		  MessageBox(HWindow, GetRCStr(603), "Ошибка", MB_OK | MB_ICONSTOP);
		  return 0;
       }
       (*base)(28, atof(buffer.AgPercent));
	  (*base)(29, buffer.IsUridich ? "Y" : "N");

	  (*base)(31, buffer.InsurType ? "F" : "U");

	  DATE stopdate = 0;
	  if(*buffer.StopDate) {
	   	stopdate = GD(buffer.StopDate, 1);
	  	if(!stopdate || (stopdate && (stopdate < fromdate || stopdate >= todate))) {
			SetFocus(m_StopDate->HWindow);
			return 0;
		}
 	    (*base)(32, stopdate);
	  }

	  (*base)(33, atof(buffer.RetSum));

	   m_RetSumCombo->GetText(CurrCode, sizeof CurrCode);
	   if(*CurrCode == 0 && atof(buffer.RetSum) >= 0.01 || *CurrCode && atof(buffer.RetSum) < 0.01)
	   {      //MessageBox(0, CurrCode, buffer.RetSum, 0);
			SetFocus(m_RetSumCombo->HWindow);
			return 0;
	   }
	   (*base)(34, CurrCode);

	   if(*CurrCode && !stopdate) {
			SetFocus(m_RetSum->HWindow);
			return 0;
	   }

	   if(stopdate) {
		   (*base)(35, STATE_STOP);
		   SetState(STATE_STOP);
	   }
       else {
		   (*base)(35, STATE_NORMAL);
		   SetState(STATE_NORMAL);
		}
   }
   else {
	(*base)(35, STATE_BAD);
		   SetState(STATE_BAD);
	  }
   
  	(*base)(30, (double)m_Restrax->GetSelIndex());
	if(m_Restrax->GetSelIndex() == -1) {
       SetFocus(m_Restrax->HWindow);
	   return 0;
	  }

   return 1;
}

Private2Dlg::~Private2Dlg()
{
//   EnableMenuItem(GetSubMenu(GetMenu(Parent->HWindow), 2), 1, MF_BYPOSITION | MF_ENABLED);
}

void Private2Dlg::Find()
{
   HMENU Popup = CreatePopupMenu();
   AppendMenu(Popup, MF_ENABLED | MF_STRING, 700, "По Серии/Номеру");
   AppendMenu(Popup, MF_ENABLED | MF_STRING, 701, "По Застрахованному");
   POINT ptCurrent;
   GetCursorPos(&ptCurrent);
   TrackPopupMenu(Popup, TPM_LEFTALIGN, ptCurrent.x, ptCurrent.y, 0, HWindow, NULL);
   DestroyMenu(Popup);
}

void Private2Dlg::StatesMenu(RTMessage)
{
   char vid[3];
   if(!m_Vid1Combo->GetText(vid, sizeof vid)) {
      m_Vid1Combo->ShowList();
      return;
   }

   if(vid[1] == ' ') vid[1] = 0;
   HMENU Popup = CreatePopupMenu();
   for(int i = 1;; i++) {
       char key[32];
       char buff[128];
       sprintf(key, "States_%s_%d", vid, i);
       if(!GetPrivateProfileString(FORMSECTION, key, "", buff, sizeof buff, ININame))
	   break;
       AppendMenu(Popup, MF_ENABLED | MF_STRING, 800 + i, buff);
   }
   for(i = 1;; i++) {
       char key[32];
       char buff[128];
       sprintf(key, "No_States_%s_%d", vid, i);
       if(!GetPrivateProfileString(FORMSECTION, key, "", buff, sizeof buff - 2, ININame))
	   break;

       if(i == 1)
	  AppendMenu(Popup, MF_SEPARATOR, 0, 0);

       AppendMenu(Popup, MF_GRAYED | MF_STRING, 1, buff);
   }
   POINT ptCurrent;
   GetCursorPos(&ptCurrent);
   TrackPopupMenu(Popup, TPM_LEFTALIGN, ptCurrent.x, ptCurrent.y, 0, HWindow, NULL);
   DestroyMenu(Popup);
}

void Private2Dlg::WMCommand(RTMessage msg)
{
   Dlg_base::WMCommand(msg);
   if(msg.WParam == 700) FindSN();
   if(msg.WParam == 701) FindName();
   if(msg.WParam > 900 && msg.WParam < 950) Clear(msg.WParam);
   if(msg.WParam > 800 && msg.WParam < 850) {
      char vid[3];
      m_Vid1Combo->GetText(vid, sizeof vid);
      if(vid[1] == ' ') vid[1] = 0;
      char key[32];
      char buff[128];
      sprintf(key, "States_%s_%d", vid, msg.WParam - 800);
      GetPrivateProfileString(FORMSECTION, key, "", buff, sizeof buff, ININame);
      m_State->SetText(buff);
   }
}

void Private2Dlg::FindSN()
{
   base->Refresh();
   char b[32];
   m_Seria->GetText(b, sizeof b);
  (*base)(0, b);
   m_Number->GetText(b, sizeof b);
  (*base)(1, atof(b));
   if(!PXSrchKey(base->getTblHandle(), base->getRecHandle(), 2, SEARCHFIRST)) {
	base->Get();
	GetDataFromBase();
   }
   else MessageBox(GetApplication()->MainWindow->HWindow, GetRCStr(604), "Сообщаю", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
}

int auxFindLike(const char* TableName, char* FldName, char* Maska, Array& Strings, TDialog* Dlg, char* ResStr, int IsDigit, char** CheckFld);

void Private2Dlg::FindName()
{
    char b[16];
	char str[64];
	if(m_Insurer->GetText(b, sizeof b)) {
	    Array Strings(10, 0, 10);
		int Result = auxFindLike(Prvt2Name, "Insurer", b, Strings, this, str, 0, 0);
		if(Result != 1) return;
 	    int nCount = Strings.getItemsInContainer();
        if(nCount == 0) {
  	       return;
        }
	    (*base)(0, strtok(str, "/"));
        (*base)(1, atof(strtok(0, " ")));
        if(!PXSrchKey(base->getTblHandle(), base->getRecHandle(), 2, SEARCHFIRST)) {
	        base->Get();
	        GetDataFromBase();
            return;
		}
    }
/*   base->Refresh();
   char b[128];
   if(m_Insurer->GetText(b, sizeof b)) {
       Array Strings(10, 0, 10);
       base->First();
       char str[100], seria[15], owner[100];
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
	       if(findWnd) findWnd->SetAll(Strings.getItemsInContainer());
	   }
	   base->Next();
	   if(findWnd) findWnd->SetPercent(i * 100 / base->GetRecords());
       }
       if(findWnd) delete findWnd;
       int nCount = Strings.getItemsInContainer();
       if(nCount == 0) {
	 MessageBox(HWindow, GetRCStr(605), "Сообщаю", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
	 return;
       }
       else {
	  strcpy(str, (LPSTR)(LPCSTR)(String&)(Strings[0]));
	  if(nCount > 1)
	   if(GetModule()->ExecDialog(new SelectStr(this, Strings, str)) != IDOK) return;
       }
       (*base)(0, strtok(str, "/"));
       (*base)(1, atof(strtok(0, " ")));
       if(!PXSrchKey(base->getTblHandle(), base->getRecHandle(), 2, SEARCHFIRST)) {
	   base->Get();
	   GetDataFromBase();
       }
   }*/
}

void Private2Dlg::Print1()
{
   if(!SetDataToBase()) return;
   Dlg_base::SetDataToBase();

   char* str = new char [3000];
   char* dest = str;

   memset(str, 0, 2800);

   strcpy(dest, buffer.Insurer);
   dest += strlen(dest) + 1;
   strcpy(dest, buffer.Address);
   dest += strlen(dest) + 1;
   strcpy(dest, buffer.BirthDate);
   dest += strlen(dest) + 1;
   strcpy(dest, buffer.Passport);
   dest += strlen(dest) + 1;
   strcpy(dest, buffer.State);
   dest += strlen(dest) + 1;
   strcpy(dest, buffer.Conditions);
   dest += strlen(dest) + 1;
   strcpy(dest, buffer.Insurer2);
   dest += strlen(dest) + 1;
   strcpy(dest, buffer.FromDate);
   dest += strlen(dest) + 1;
   strcpy(dest, buffer.ToDate);
   dest += strlen(dest) + 1;
   strcpy(dest, buffer.Duration);
   strcat(dest, " days");
   dest += strlen(dest) + 1;
   strcpy(dest, buffer.AllSumma);
   dest += strlen(dest) + 1;
   strcpy(dest, buffer.Tarif);
   dest += strlen(dest) + 1;
   strcpy(dest, buffer.Payment);
   strcat(dest, " USD");
   //dest += strlen(dest);
   //m_ValutaCombo->GetText(dest, 4);
   dest += strlen(dest) + 1;
   strcpy(dest, buffer.WriteDate);
   dest += strlen(dest) + 1;
   strcpy(dest, buffer.PaymentDate);
   dest += strlen(dest) + 1;
   strcpy(dest, (m_NalBezNalCombo->GetSelIndex() == 1) ? "X" : " ");
   dest += strlen(dest) + 1;
   strcpy(dest, (m_NalBezNalCombo->GetSelIndex() == 0) ? "X" : " ");
   dest += strlen(dest) + 1;
   m_Vid1Combo->GetText(dest, 3);

   ((TRulerWin*)Parent)->SetValues(str);
   Dlg_base::Print1();
   delete str;
}

/*   if(inc == 3) PXDateEncode(m, d, y + How_many, &date); //Year
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
*/

void Private2Dlg::SetDates()
{
    if(!CanModifyPrivate2/* || m_Vid1Combo->GetSelIndex() > 3*/)
       return;

    char str0[64], str1[64], str2[64];
    memset(str0, 0, sizeof str0);

    m_Vid1Combo->GetText(str0, sizeof str0);
    m_Vid2Combo->GetText(str1, sizeof str1);
    m_Vid3Combo->GetText(str2, sizeof str2);

    int inc0 = getDurationType(str0);
    int inc1 = getDurationType(str1);
    int inc2 = getDurationType(str2);

    char datebuf[15];
    m_FromDate->GetText(datebuf, sizeof datebuf);
    DATE startdate = GD(datebuf, 1);

    if(startdate) {
       DATE enddate = aux_incDate(startdate, inc1, atoi(str1));
       enddate = aux_incDate(enddate, inc2, atoi(str2));
       enddate = aux_incDate(enddate, inc0, atoi(str0 + 2));

       int d, m, y;
       PXDateDecode(--enddate, &m, &d, &y);
       sprintf(datebuf, "%02u.%02u.%04u", d, m, y);
       m_ToDate->SetText(datebuf);

       if(m_Vid1Combo->GetSelIndex() <= 3) { //Only for A1, A2, B, C
          DblToStr(datebuf, enddate - startdate + 1);
	  m_Duration->SetText(datebuf);
       }
    }
}

void Private2Dlg::ChangeStartDate(RTMessage msg)
{
    SetDates();

    if(CanModifyPrivate2 && msg.LP.Hi == EN_CHANGE) {
	char buff[32];
	m_FromDate->GetText(buff, sizeof buff);
	m_WriteDate->SetText(buff);
    }
}

void Private2Dlg::Calculate(RTMessage msg)
{
   m_AllSumma->SetText("");
   m_Duration->SetText("");
   m_Tarif->SetText("");
   m_ToDate->SetText("");
   m_Conditions->SetText("");

   int Index = m_Vid1Combo->GetSelIndex();
   char buff[32];
   m_Group->GetText(buff, sizeof buff);
   int Group = max(1, atoi(buff));

//   MessageBox(0, "key", "", 0);

   char* key = 0;
   switch(Index) {
	case 4 :
	case 5 :
	case 6 : key = "Year_summ";
		 break;
	case 0 : key = "Plan_A1";
                 break;
	case 1 : key = "Plan_A2";
                 break;
	case 2 : key = "Plan_B";
                 break;
	case 3 : key = "Plan_C";
		 break;
	default: return;
   };



   if(!GetPrivateProfileString(FORMSECTION, key, "", buff, sizeof buff, ININame))
       return;

   sprintf(buff, "%g", atof(buff) * Group);
   m_AllSumma->SetText(buff);

   if(Index > 3) {
      switch(Index) {
	 case 4 : key = "Year_premium";
                  break;
	 case 5 : key = "6Month_premium";
                  break;
	 case 6 : key = "3Month_premium";
		  break;
      }
      if(!GetPrivateProfileString(FORMSECTION, key, "", buff, sizeof buff, ININame))
	  return;

      char* ch = strchr(buff, ',');
      if(ch) *ch = 0;
      m_Tarif->SetText(buff);
      if(ch) m_Duration->SetText(ch + 1);
   }
   else {
       int Index2 = m_Vid2Combo->GetSelIndex();
       if(Index2 < 0) return;
       double Tarif = SendMessage(m_Vid2Combo->HWindow, CB_GETITEMDATA, Index2, 0);
       Tarif += SendMessage(m_Vid3Combo->HWindow, CB_GETITEMDATA, m_Vid3Combo->GetSelIndex(), 0);
       Tarif /= 100.;
       //Tarif += 0.5;
       //DblToStr(buff, Tarif);
       sprintf(buff, "%g", Tarif);
       m_Tarif->SetText(buff);
   }

   m_Tarif->GetText(buff, sizeof buff);
   double TARIF = atof(buff) * Group; //////// Тариф не меняется в форме

   int idxChecks[128];
   int Count = m_Ks->GetSelIndexes(idxChecks, 128);
   double K = 1;
   char formula[64] = "K = ";
   for(int i = 0; i < Count; i++) {
       double Koeff = int(SendMessage(m_Ks->HWindow, LB_GETITEMDATA, idxChecks[i], 0)) / 100.;
       if(i) strcat(formula, " * ");
       sprintf(formula + strlen(formula), "%g", Koeff);
       K *= Koeff;
   }

   if(strlen(formula) > 4) {
      if(Count == 1) formula[4] = 0;
      else strcat(formula, " = ");
      sprintf(formula + strlen(formula), "%g", K);
      m_Conditions->SetText(formula);
   }

   TARIF *= K;
   TARIF = long((TARIF + 0.005) * 100) / 100.;
   DblToStr(buff, TARIF);
   m_Payment->SetText(buff);
   m_Sum1->SetText(buff);
   m_Sum1Combo->SetSelIndex(0);
   m_Sum2->SetText("");
   m_Sum2Combo->SetSelIndex(0);

   SetDates();
}

/////////////////////////////////////

void  Private2Dlg::SetInsurer2(RTMessage)
{
   char buffer[128];
   m_Insurer->GetText(buffer, sizeof buffer);
   m_Insurer2->SetText(buffer);
}

void  Private2Dlg::Clear()
{
   SetFocus(m_Number->HWindow);

   char class_name[40];
   HWND Child = GetWindow(HWindow, GW_CHILD);
   for(;Child;) {
       ::GetClassName(Child, class_name, sizeof class_name);
       if(!strcmpi(class_name, "EDIT"))
	   SendMessage(Child, WM_SETTEXT, 0, (long)"");
       if(!strcmpi(class_name, "COMBOBOX"))
	   SendMessage(Child, CB_SETCURSEL, -1, 0);
       Child = GetWindow(Child, GW_HWNDNEXT);
   }

   date d;
   getdate(&d);
   char buff[100];
   wsprintf(buff, "%02u.%02u.%04u", d.da_day, d.da_mon, d.da_year);
   m_FromDate->SetText(buff);
   m_PaymentDate->SetText(buff);
   m_WriteDate->SetText(buff);

   GetPrivateProfileString(FORMSECTION, "Seria", "", buff, sizeof buff, ININame);
   m_Seria->SetText(buff);

   //m_ValutaCombo->SetSelIndex(0);
   m_NalBezNalCombo->SetSelIndex(0);

   if(GetPrivateProfileString(FORMSECTION, "DefAgent", "", buff, sizeof buff, ININame)) {
      SetAgent(buff, agents, m_AgentCombo, 0);
      TMessage msg;
      msg.LP.Hi = CBN_SELCHANGE;
      ChAgent(msg);
   }

   for(int i = 0; i < m_Ks->GetCount(); i++)
      m_Ks->SetSelIndexes(&i, 1, FALSE);

   SendDlgItemMessage(HWindow, 303, WM_SETTEXT, 0, (long)"Выбрано 0");

   m_Restrax->SetSelIndex(GetPrivateProfileInt(FORMSECTION, "DefAssist", 0, ININame));

   TMessage msg;
   Change(msg);

   if(GetPrivateProfileString(FORMSECTION, "DefVid", "", buff, sizeof buff, ININame)) {
      m_Vid1Combo->SetText(buff);
      if(m_Vid1Combo->GetSelIndex() >= 0) {
	  msg.LP.Hi = CBN_SELCHANGE;
	  ChangeCombo1(msg);
      }
	}

  ((TCheckBox*)ChildWithId(56))->SetCheck(1);

   HMENU Popup = CreatePopupMenu();
   for(i = 1;; i++) {
       char key[32];
       char buff[128];
       sprintf(key, "DefPeriod_%d", i);
       if(!GetPrivateProfileString(FORMSECTION, key, "", buff, sizeof buff, ININame))
	   break;
       AppendMenu(Popup, MF_ENABLED | MF_STRING, 900 + i, buff);
   }
   POINT ptCurrent;
   GetCursorPos(&ptCurrent);
   TrackPopupMenu(Popup, TPM_LEFTALIGN, ptCurrent.x, ptCurrent.y, 0, HWindow, NULL);
   DestroyMenu(Popup);
}

void  Private2Dlg::Clear(int Command)
{
   char buff[64];
   if(!GetPrivateProfileString(FORMSECTION, "DefBigPeriod", "", buff, sizeof buff, ININame))
       return;

   m_Vid2Combo->SetText(buff);
   if(m_Vid2Combo->GetSelIndex() < 0)
       return;

   if(m_Vid2Combo->GetSelIndex() >= 0) {
      TMessage msg;
      msg.LP.Hi = CBN_SELCHANGE;
      ChangeCombo2(msg);
   }

   sprintf(buff, "DefPeriod_%d", Command - 900);
   if(!GetPrivateProfileString(FORMSECTION, buff, "", buff, sizeof buff, ININame))
       return;

   m_Vid3Combo->SetText(buff);

//   MessageBox(0, "", "will send", 0);
//   PostMessage(HWindow, WM_FIRST+1, 0, 0);
//   PostMessage(HWindow, WM_FIRST, 1, 0);
   PostMessage(HWindow, 1000, 1, 0);
   //TMessage q;
   //Calculate(q);
}


void Private2Dlg::Help(RTMessage msg)
{
   if(msg.WParam == 1000)
      WinHelp(GetApplication()->MainWindow->HWindow, "blank.hlp", HELP_CONTEXT, HELP_PRIVATE);

   Dlg_base::Help(msg);
}

void Private2Dlg::WMCtlColor(RTMessage msg)
{
   Dlg_base::WMCtlColor(msg);
   if(msg.LP.Hi == CTLCOLOR_STATIC && (HWND)msg.LP.Lo == GetDlgItem(HWindow, 103)) 
	SetTextColor((HDC)msg.WParam, RGB(255, 0, 0));
   if(msg.LP.Hi == CTLCOLOR_STATIC && (HWND)msg.LP.Lo == GetDlgItem(HWindow, 1002))
	SetTextColor((HDC)msg.WParam, RGB(0, 0, 255));
}

void Private2Dlg::Change(RTMessage msg)
{
   CanModifyPrivate2 = 1;
   EnableWindow(GetDlgItem(HWindow, 406), TRUE);
//   Calculate(msg);
}


void Private2Dlg::WMMove(RTMessage msg)
{
     AlignCombo(m_AgentCombo);
     AlignCombo(m_Vid1Combo);
     AlignCombo(m_Vid2Combo);
     AlignCombo(m_Vid3Combo);
     AlignCombo(m_Sum1Combo);
     AlignCombo(m_Sum2Combo);
     AlignCombo(m_NalBezNalCombo);
}

void Private2Dlg::ChangeCombo1(RTMessage msg)
{
     if(msg.LP.Hi != CBN_SELCHANGE)
  	    return;

     if(!CanModifyPrivate2) {
	    GetDataFromBase();
	    return;
     }

     char P1[64], P2[64];
     m_Vid2Combo->GetText(P1, sizeof P1);
     m_Vid3Combo->GetText(P2, sizeof P2);

     SetStatesControls();
     SendMessage(m_Vid3Combo->HWindow, CB_RESETCONTENT, 0, 0);

     FillCombo2();
     FillKs();

     m_Vid2Combo->SetText(P1);
     if(m_Vid2Combo->GetSelIndex() >= 0) {
	     FillCombo3();
	     m_Vid3Combo->SetText(P2);
     }

	 PostMessage(HWindow, 1000, 0, 0);
}

void Private2Dlg::ChangeCombo2(RTMessage msg)
{
     if(msg.LP.Hi != CBN_SELCHANGE)
	return;

     if(!CanModifyPrivate2) {
	GetDataFromBase();
	return;
     }
     FillCombo3();
	 PostMessage(HWindow, 1000, 0, 0);
}

void Private2Dlg::ChangeCombo3(RTMessage)
{
	 PostMessage(HWindow, 1000, 0, 0);
}

double GetTarif(char* str, int n)
{
     char* pos = strchr(str, ','); //после текста

     for(int i = 1; i <= n; i++) {
	str = pos + 1;
	pos = strchr(str, ','); //i-Й
	if(i == n) {
	    while(*str && (*str != ',') && (*str < '0' || *str > '9'))
	       str++;
	    if(*str >= '0' && *str <= '9') {
	       if(atof(str) == 0)
		  return -2;
            }
      	    return atof(str);
	}
	if(!pos) return -1;
     }

     return -1;
}

void Private2Dlg::FillCombo2()
{
     SendMessage(m_Vid2Combo->HWindow, CB_RESETCONTENT, 0, 0);
     char key[32], buff[128];
     if(m_Vid1Combo->GetSelIndex() <= 3 && m_Vid1Combo->GetSelIndex() >= 0) {
          for(int i = 1;; i++) {
	    sprintf(key, "BigPeriod%d", i);
	    if(!GetPrivateProfileString(FORMSECTION, key, "", buff, sizeof buff, ININame))
	       break;
	    double Tarif = GetTarif(buff, m_Vid1Combo->GetSelIndex() + 1);
	    if(Tarif == -1) {
		MessageBox(0, GetRCStr(606), key/*"Сообщаю"*/, MB_OK | MB_ICONINFORMATION);
		break;
	    }
	    if(Tarif == 0) break;
	    if(Tarif == -2) Tarif = 0;
	    *strchr(buff, ',') = 0;
	    int Index = m_Vid2Combo->AddString(buff);
	    SendMessage(m_Vid2Combo->HWindow, CB_SETITEMDATA, Index, long(Tarif * 100. + 0.001)); //Погрешность
	  }
     }
}

void Private2Dlg::FillCombo3()
{
     SendMessage(m_Vid3Combo->HWindow, CB_RESETCONTENT, 0, 0);
     m_Vid3Combo->AddString("");
     m_Vid3Combo->SetSelIndex(0);
     SendMessage(m_Vid3Combo->HWindow, CB_SETITEMDATA, 0, 0);
     char key[32], buff[128];
     if(m_Vid2Combo->GetSelIndex() >= 0 && m_Vid2Combo->GetSelIndex() < (m_Vid2Combo->GetCount()-1)) {
          for(int i = 1;; i++) {
	    sprintf(key, "Period%d", i);
	    if(!GetPrivateProfileString(FORMSECTION, key, "", buff, sizeof buff, ININame))
	       break;
	    double Tarif = GetTarif(buff, m_Vid1Combo->GetSelIndex() + 1);
	    if(Tarif < 0) {
		MessageBox(0, GetRCStr(607), "Сообщаю", MB_OK | MB_ICONINFORMATION);
		break;
	    }
	    if(Tarif == 0) break;
	    *strchr(buff, ',') = 0;
	    int Index = m_Vid3Combo->AddString(buff);
	    SendMessage(m_Vid3Combo->HWindow, CB_SETITEMDATA, Index, long(Tarif * 100. + 0.001)); //Погрешность
	  }
     }
}

void Private2Dlg::SetStatesControls()
{
     EnableWindow(m_Vid2Combo->HWindow, m_Vid1Combo->GetSelIndex() <= 3);
     EnableWindow(m_Vid3Combo->HWindow, m_Vid1Combo->GetSelIndex() <= 3);
}

void Private2Dlg::ChangeKs(RTMessage msg)
{
     if(msg.LP.Hi == LBN_SELCHANGE) {
	 if(!CanModifyPrivate2) {
	    MessageBeep(0);
	    GetDataFromBase();
            return;
	 }

	 int idxStates[128];
	 int Count = m_Ks->GetSelIndexes(idxStates, 128);
	 int GroupNumber = -1;
	 int IsCheck = 0;
	 for(int i = 0; i < m_Ks->GetCount(); i++) {
	    long Value = SendMessage(m_Ks->HWindow, LB_GETITEMDATA, i, 0);
	    if(Value == -1) { //Separator
	       m_Ks->SetSelIndexes(&i, 1, FALSE);
	    }
	    else {
	       Value = HIWORD(Value);
		   int Grp = (Value & 0x0000FF00) >> 8;
	       if(Grp != GroupNumber) {
		  GroupNumber = Grp;
		  IsCheck = 0;
	       }
	       for(int x = 0; x < Count; x++) //Find in Selection
		 if(idxStates[x] == i) {
		    if(IsCheck) { //Сброс
		       if(GetFocus() == m_Ks->HWindow && //Active List box
			  SendMessage(m_Ks->HWindow, LB_GETCARETINDEX, 0, 0) == i) //Current item is focused
			 m_Ks->SetSelIndexes(&idxStates[x - 1], 1, FALSE);
                       else
		         m_Ks->SetSelIndexes(&i, 1, FALSE);
		    }
		    IsCheck = 1;
                 }
	    }
	 }

         RECT r = { 0, 0, 1000, 1000 };
	 InvalidateRect(m_Ks->HWindow, &r, FALSE);
	 PostMessage(HWindow, 1000, 0, 0);

	 char Msg[32];
	 sprintf(Msg, "Выбрано %d", m_Ks->GetSelCount());
	 SendDlgItemMessage(HWindow, 303, WM_SETTEXT, 0, (long)Msg);
     }
}

static void CheckLBItem(TListBox* lb, int Hi, int Low)
{
    for(int i = 0; i < lb->GetCount(); i++) {
	long Value = SendMessage(lb->HWindow, LB_GETITEMDATA, i, 0);
	Value = HIWORD(Value);
	if((Value & 0x000000FF) == Low)
	   if(((Value & 0x0000FFFF) >> 8) == Hi)
	       lb->SetSelIndexes(&i, 1, TRUE);
    }
}

void Private2Dlg::SetStatesKs(char* States) //по 2 значения
{
    for(int i = 0; i < m_Ks->GetCount(); i++) 
	m_Ks->SetSelIndexes(&i, 1, FALSE);

    char* ch = States;
    char Hi[3], Lo[3];
    Hi[2] = Lo[2] = 0;

    for(;*ch;) {
       Hi[0] = *ch;
       ch++;
       Hi[1] = *ch;
       ch++;
       Lo[0] = *ch;
       ch++;
       Lo[1] = *ch;
       ch++;
       CheckLBItem(m_Ks, atoi(Hi), atoi(Lo));
    }
    char Msg[32];
    sprintf(Msg, "Выбрано %d", m_Ks->GetSelCount());
    SendDlgItemMessage(HWindow, 303, WM_SETTEXT, 0, (long)Msg);
}

void Private2Dlg::GetStatesKs(char* strStates) //по 2 значения
{
    int States[128];
    int Count = m_Ks->GetSelIndexes(States, 128);
    *strStates = 0;
    char State[3];

    for(int i = 0; i < Count; i++) {
       long Value = SendMessage(m_Ks->HWindow, LB_GETITEMDATA, States[i], 0);
       Value = HIWORD(Value);
       sprintf(State, "%02d", int(Value & 0x0000FF00) >> 8);
       strcat(strStates, State);
       sprintf(State, "%02d", int(Value & 0x000000FF));
	   strcat(strStates, State);
    }
}

void Private2Dlg::FillKs()
{
 //  LockWindowUpdate(m_Ks->HWindow);
   m_Ks->ClearList();

   if(m_Vid1Combo->GetSelIndex() != -1) {
	char* mask = "K%d_%d";
	if(m_Vid1Combo->GetSelIndex() > 3)
	    mask = "K_YSQ_%d_%d";
	char key[32];
	char buff[128];
	for(int i = 1; i < 50; i++) {
	  for(int j = 1;; j++) {
	     sprintf(key, mask, i, j);
	     if(!GetPrivateProfileString(FORMSECTION, key, "", buff, sizeof buff, ININame))
		 break;
	     char* ch = strchr(buff, ',');
	     if(!ch) return;
	     int Index = m_Ks->AddString(buff);
	     SendMessage(m_Ks->HWindow, LB_SETITEMDATA, Index, MAKELONG(atof(buff) * 100, (i << 8) + j));
	  }
	  memset(buff, '—', sizeof(buff) - 1);
	  buff[sizeof(buff) - 1] = 0;
	  int Index = m_Ks->AddString(buff);
	  SendMessage(m_Ks->HWindow, LB_SETITEMDATA, Index, -1);
	}

	for(;;) {
	   if(SendMessage(m_Ks->HWindow, LB_GETITEMDATA, m_Ks->GetCount() - 1, 0) == -1)
	      m_Ks->DeleteString(m_Ks->GetCount() - 1);
	   else
	      break;
	}
   }

//   LockWindowUpdate(0);
}

void Private2Dlg::RoundSumma(RTMessage)
{
    char buff[32];
    m_Payment->GetText(buff, sizeof buff);
    DblToStr(buff, long(atof(buff) + 0.5), 2);
    m_Payment->SetText(buff);
    m_Sum1->SetText(buff);
    m_Sum1Combo->SetSelIndex(0);
    m_Sum2->SetText("");
    m_Sum2Combo->SetSelIndex(0);
}

void Private2Dlg::ChangeGroup(RTMessage msg)
{
     if(msg.LP.Hi == EN_CHANGE) {
	 if(!CanModifyPrivate2) {
	 //   GetDataFromBase();
	    return;
	 }
	 PostMessage(HWindow, 1000, 0, 0);
	 //MessageBeep(0);
     }
}

void  Private2Dlg::ChangeNumber(RTMessage msg)
{
     if(msg.LP.Hi == EN_CHANGE) {
	 if(!CanModifyPrivate2) 
	    return;

	 m_Insurer2->SetText("");
     }
}

void Private2Dlg::ChAgent(RTMessage msg)
{
   int IsUr;
   if(msg.LP.Hi == CBN_SELCHANGE)
	   ((TEdit*)ChildWithId(53))->SetText(GetAgPercent(SendMessage(m_AgentCombo->HWindow, CB_GETITEMDATA, m_AgentCombo->GetSelIndex(), 0), LEBEN2_TBL, IsUr));
}

/*
void Private2Dlg::Import()
{
	long Result = ImportData(Prvt2Name, Prvt2Fields, Prvt2Count, 0, 0, 18, 35, atoi(STATE_BAD), 0, 0);
	if(Result) {
		MessageBox(GetApplicationObject()->MainWindow->HWindow, GetRCStr(700+Result), "!", MB_ICONEXCLAMATION);
	}
}

void Private2Dlg::Export(PTWindowsObject Parent)
{
	Priod_Params* data = new Priod_Params;
	strcpy(data->filename, Prvt2Name);
	if(GetApplicationObject()->ExecDialog(new getDate(Parent, &data->m, &data->y, &data->dm, data->filename)) == IDOK) {
		long Error = ExportData(Prvt2Name, Prvt2Fields, Prvt2Count, 0, 0, data->filename, data->m, data->y, data->dm, 18, 10);
		sprintf(data->filename, GetRCStr(Error > 0 ? 621: 622), labs(Error));
		//if(Error <= 0) unlink(data->filename);
		MessageBox(Parent->HWindow, data->filename, "", 0);
	}
	delete data;
}
*/