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
#include <priv2005.h>
#include <checkbox.h>
#include <thwindow.h>
#include <help.h>
#include <stdlib.h>
#include <stdio.h>
#include "util.h"
#include "radiobut.h"
#include "grncard.h"
#include "date.h"
#include "dateedit.h"
#include "impexp.h"
#include "strng.h"
#include "math.h"
#include "editors.h"

#define max(a, b) ((a) > (b) ? (a) : (b))

extern char ActivateParadox;
static const char* FORMSECTION = "PRIV2014";
void AlignCombo(TComboBox*);

void FillPlacement(TComboBox* PlacementCombo, const char* Key, const char* = 0, int = 1);
void InitVal2(TComboBox* cb);
void SetAgent(char* ACode, TOpenBase& agents, PTComboBox AgName, int);
short TarifCol = -1;

char* Prvt2005Fields[] = {
                        "Seria",
                        "Number",
                        "Insurer",
                        "Address",
                        "Birthdate",
			"Passport",
                        "Country", //6 
			"Conditions",
                        "Insurer2",
						"From",
			"To", //10
                        "Duration", //11
			"InsSum",
                        "Tarif",
			"Pay",
			"PayDt",
			"InsCnt",         //16
			"RegDt",
			"Agent",
			"Cash",   
			"InsType", //20
			"UridSum", //21
			"InsSumCurr",
			"Ks",
			"Sum1",
			"Sum1Curr",
			"Sum2",
			"Sum2Curr",
			"AgPercent",   //28
			"AgType",      //29
			"Assistant", //30
			"FromTm",
			"StopDate",
			"RetSum",
			"RetCur",
			"State",  //35
			"Period",  //36
			"RepDt",  //37
		 	"PSeria", //38
			"PNumber", //39
			"UpDt", //40
			"Passport2" //41
			 };

#define STATE_NORMAL "0"
#define STATE_BAD    "1"                              
#define STATE_STOP   "2"
#define STATE_LOST   "3"

int  Prvt2005Count = 42;
char* Prvt2005Name = "\\bases\\PRIV2005";

static char CanModifyPrivate2005;
char MyState;

Private2005Dlg::Private2005Dlg(PTWindowsObject p)
	   : Dlg_base(p, !ActivateParadox ? "PRIVCALC" : GetSystemMetrics(SM_CXSCREEN) < 1024 ? "PRIV2005" : "PRIV2005_1024", Prvt2005Name, Prvt2005Fields, Prvt2005Count),
             agents(AgentDBName, AgentFields, AgFieldsCount)
{
	 CanModifyPrivate2005 = 0;

	 m_Seria = new TComboBox(this, 200, 3);
	 m_Seria->DisableTransfer();
	 m_Number = new NumberEdit(this, 201, 11, 0);
	 m_PSeria = new TEdit(this, 66, 3);
	 m_PNumber = new NumberEdit(this, 67, 11, 0);
	 m_RegDate = new TDateEdit(this, 223, 11);
	 m_Insurer = new TEdit(this, 203, 66);
	 m_InsCnt = new NumberEdit(this, 228, 3, 0);
	 m_InsurerAddress = new TEdit(this, 205, 66);
	 m_InsurerBirthDate = new TEdit(this, 204, 11);
	 m_Passport = new TEdit(this, 206, 41);
	 m_Conditions = new TEdit(this, 211, 129);
	 m_Country = new TEdit(this, 1000, 50);
	 m_Insurer2 = new TEdit(this, 212, 66);
	 m_Passport2 = new TEdit(this, 69, 41);
	 m_InsFiz = new TCheckBox(this, 56, 0);
	 m_FromDate = new TDateEdit(this, 213, 11);
	 m_FromTime = new TTimeEdit(this, 60, 6);
	 m_Period1 = new NumberEdit(this, 62, 5, 0);
	 m_ToDate = new TDateEdit(this, 214, 11);
	 m_Duration = new NumberEdit(this, 215, 5, 0);

	 m_UridSum = new NumberEdit(this, 61, 10, 1);
	 m_Tarif = new NumberEdit(this, 217, 10, 1);
	 m_Payment = new NumberEdit(this, 218, 10, 1);
	 m_PaymentDate = new TDateEdit(this, 222, 11);
	 m_PaymentNumber = new TEdit(this, 68, 11);
	 m_ReportDate = new TDateEdit(this, 65, 11);
	 m_PayNal = new TCheckBox(this, 51, 0);
	 m_Sum1 = new NumberEdit(this, 225, 10, 1);
	 m_Sum2 = new NumberEdit(this, 226, 10, 1);
	 m_AgPercent = new NumberEdit(this, 53, 5, 1);
	 m_AgUr = new TCheckBox(this, 54, 0);
	 m_StopDate = new TDateEdit(this, 57, 11);
	 m_RetSum = new NumberEdit(this, 59, 10, 1);

     m_AgentCombo = new TComboBox(this, 202, 1);
	 m_AgentCombo->DisableTransfer();
	 m_InsSum = new TComboBox(this, 207, 1);
	 m_InsSum->DisableTransfer();
	 m_InsSumCurr = new TComboBox(this, 208, 1);
	 m_InsSumCurr->DisableTransfer();
	 m_Countries = new TComboBox(this, 209, 1);
	 m_Countries->DisableTransfer();
	 m_Sum1Combo = new TComboBox(this, 219, 1);
     m_Sum1Combo->DisableTransfer();
	 m_Sum2Combo = new TComboBox(this, 227, 1);
     m_Sum2Combo->DisableTransfer();
	 m_RetSumCombo = new TComboBox(this, 58, 1);
	 m_RetSumCombo->DisableTransfer();

     m_Ks = new TListBox(this, 220, 0);
     m_Ks->DisableTransfer();

	 m_Assistant = new TComboBox(this, 55, 1);
	 m_Assistant->DisableTransfer();
}

void SetLittleFont(HWND, short*, int);
void _FillAgents(TOpenBase* agents, TComboBox* AgName, int index);
void DateFBToStr(TOpenBase* base, int field, char* buffer);

void Private2005Dlg::SetState(char* state)
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
	if(*state == *STATE_LOST) {
		sprintf(s, "%s [%s]", SrcDlgName, "УТЕРЯН");
		SetWindowText(HWindow, s);
	}
	MyState = *state;
}

void Private2005Dlg::SetupWindow()
{
   Dlg_base::SetupWindow();

   PTWindowsObject obj = GetFirstChild();
   while(obj != 0)
   {
	if(!GetDlgItem(HWindow, obj->GetId()))
	{
		char b[11];
		sprintf(b, "%d", obj->GetId());
   MessageBox(0, b, "Stop", HWindow ? MB_ICONQUESTION : 0);
	}
	obj = obj->Next();
	if(obj == GetFirstChild())
	break;
   }


   FillPlacement(m_Seria, "Series%d", (char*)FORMSECTION, 0);
   AlignCombo(m_Seria);

	InitVal2(m_Sum1Combo);
	InitVal2(m_Sum2Combo);
	InitVal2(m_RetSumCombo);
	m_Sum1Combo->DeleteString(m_Sum1Combo->FindExactString("DM", -1));
	m_Sum2Combo->DeleteString(m_Sum2Combo->FindExactString("DM", -1));
	m_RetSumCombo->DeleteString(m_RetSumCombo->FindExactString("DM", -1));
	m_InsSumCurr->AddString("USD");
	m_InsSumCurr->AddString("EUR");

   FillPlacement(m_Assistant, "Assist%d", (char*)FORMSECTION, 0);
   FillPlacement(m_Countries, "Country%d", (char*)FORMSECTION, 0);

   _FillAgents(&agents, m_AgentCombo, LEBEN2_TBL);

   if(ActivateParadox)
	   if(!base->GetRecords())
		 PostMessage(HWindow, WM_COMMAND, 405, MAKELONG(405, BN_CLICKED));

   FillKs();

   if(ActivateParadox)
	   GetDataFromBase();
   else
	   CanModifyPrivate2005 = 1;
}

extern void DblToStr(char* str, double V, int Digit = 2);
extern DATE GD(char* str, int = 0);

void Private2005Dlg::GetDataFromBase()
{
   *BaseSumm = 0;
   *OldCurr = 0;
   CanModifyPrivate2005 = 0;

   memset(&buffer, 0, sizeof buffer);
   (*base)(buffer._Seria, sizeof buffer._Seria, 0);
   m_Seria->SetText(buffer._Seria);
   (*base)(buffer.PSeria, sizeof buffer.PSeria, 38);
   double value;
   (*base)(value, 1);
   sprintf(buffer.Number, "%lu", (long)value);
   (*base)(value, 39);
   DblToStr(buffer.PNumber, value, 0);
   (*base)(buffer.Insurer, sizeof buffer.Insurer, 2);
   (*base)(buffer.Address, sizeof buffer.Address, 3);
   (*base)(buffer.BirthDate, sizeof buffer.BirthDate, 4);
   (*base)(buffer.Passport, sizeof buffer.Passport, 5);
   (*base)(buffer.Passport2, sizeof buffer.Passport2, 41);
   short Val;
   (*base)(Val, 6);
   m_Countries->SetSelIndex(Val);

   TMessage msg;
   msg.LP.Hi = CBN_SELCHANGE;
   ChangeCountry(msg);
   (*base)(buffer.Conditions, sizeof buffer.Conditions, 7);
   char* ch = strchr(buffer.Conditions, '|');
   if(ch) {
	*ch = 0;
	strncpy(buffer.PaymentNumber, ch+1, sizeof buffer.PaymentNumber);
   }
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

   (*base)(date, 37);
   PXDateDecode(date, &m, &d, &y);
   sprintf(buffer.ReportDate, "%02u.%02u.%04u", d, m, y);

   (*base)(Val, 11);
   if((long)Val > 0)
	sprintf(buffer.Duration, "%lu", (long)Val);

   char str[32];

   (*base)(str, sizeof str, 22);
   m_InsSumCurr->SetText(str);

   (*base)(value, 12);
   m_InsSum->SetSelIndex(-1);
   for(int i = 0; i < m_InsSum->GetCount(); i++) {
		m_InsSum->GetString(str, i);
		if(atof(str) == value) m_InsSum->SetSelIndex(i);
   }

   (*base)(value, 21);
   DblToStr(buffer.UridSumm, value);
   (*base)(value, 13);
   DblToStr(buffer.Tarif, value);

   (*base)(value, 14);
   DblToStr(buffer.Payment, value);

   (*base)(date, 15);
   if(!ISBLANKDATE(date)) {
	   PXDateDecode(date, &m, &d, &y);
	   sprintf(buffer.PaymentDate, "%02u.%02u.%04u", d, m, y);
   }

   (*base)(Val, 16);
   DblToStr(buffer.InsCnt, Val);

   (*base)(date, 17);
   PXDateDecode(date, &m, &d, &y);
   sprintf(buffer.RegDate, "%02u.%02u.%04u", d, m, y);

   char AgentStr[5];
   (*base)(AgentStr, sizeof AgentStr, 18);
   SetAgent(AgentStr, agents, m_AgentCombo, 0);

   int IsBad = *buffer.FromDate == 0;

   (*base)(str, sizeof str, 19);
   buffer.PayNal = *str == 'N';

   m_Assistant->SetSelIndex(-1);
   (*base)(Val, 30);
    m_Assistant->SetSelIndex((int)Val);

   if(!IsBad) {
	  char strStates[129];
	  (*base)(Val, 36);
	  sprintf(buffer.Period, "%d", int(Val));

	  (*base)(strStates, sizeof strStates, 23);
	  SetStatesKs(strStates);

      *strStates = 0;
      (*base)(strStates, sizeof strStates, 29);
	  buffer.AgUrid = !strcmp("U", strStates);

      double value;
      (*base)(value, 28);
      DblToStr(buffer.AgPercent, value);

	  (*base)(strStates, sizeof strStates, 20);
	  buffer.InsType = *strStates == 'F';

	  (*base)(buffer.FromTime, sizeof buffer.FromTime, 31);
   }

   m_Sum1Combo->SetSelIndex(-1);
   m_Sum2Combo->SetSelIndex(-1);
   m_RetSumCombo->SetSelIndex(-1);

   if(!IsBad) {
       (*base)(value, 24);
	   DblToStr(buffer.Sum1, value);

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

   Dlg_base::GetDataFromBase();
   SetState(str);
   EnableWindow(GetDlgItem(HWindow, 406), FALSE);
}


int Private2005Dlg::SetDataToBase()
{
   date curr_date;
   getdate(&curr_date);
   DATE CURRENT_DATE;
   PXDateEncode(curr_date.da_mon, curr_date.da_day, curr_date.da_year, &CURRENT_DATE);

   Dlg_base::SetDataToBase();

   m_Seria->GetText(buffer._Seria, sizeof buffer._Seria);

   (*base)(40, CURRENT_DATE);
   (*base)(0, buffer._Seria);
   if(!buffer._Seria[0])
    {
       SetFocus(m_Seria->HWindow);
       return 0;
    }
   (*base)(1, atof(buffer.Number));
   if(atof(buffer.Number)<1)
    {
       SetFocus(m_Number->HWindow);
       return 0;
	}

   (*base)(38, buffer.PSeria);
   (*base)(39, atof(buffer.PNumber));

   if(!*buffer.PSeria && *buffer.PNumber || *buffer.PSeria && !*buffer.PNumber) {
	   SetFocus(m_PNumber->HWindow);
       return 0;
   }

   int IsDup = (*buffer.PSeria) != 0;

   BOOL IsBad = (strlen(buffer.Insurer) == 0) || (!strcmp("ИСПОРЧЕН", buffer.Insurer));
   if(IsBad) {
	  if(MessageBox(HWindow, GetRCStr(602), "Отвечай поскорей", MB_YESNO | MB_ICONINFORMATION | MB_SYSTEMMODAL) == IDYES)
      {
		 strcpy(buffer.Insurer, "ИСПОРЧЕН");
		 *buffer.PSeria = 0;
		 *buffer.PNumber = 0;
		 *buffer.BirthDate = 0;
		 *buffer.Address = 0;
		 *buffer.Passport = 0;
		 *buffer.Passport2 = 0;
		 *buffer.Conditions = 0;
		 *buffer.Insurer2 = 0;
		 *buffer.FromDate = 0;
		 *buffer.FromTime = 0;
		 *buffer.ToDate = 0;
		 *buffer.Duration = 0;
		 *buffer.UridSumm = 0;
		 *buffer.Tarif = 0;
		 *buffer.Payment = 0;
		 *buffer.PaymentDate = 0;
		 *buffer.Sum1 = 0;
		 *buffer.Sum2 = 0;
		 *buffer.StopDate = 0;
		 *buffer.RetSum = 0;
		 *buffer.InsCnt = 0;
		 m_Sum1Combo->SetSelIndex(-1);
		 m_Sum2Combo->SetSelIndex(-1);
		 m_RetSumCombo->SetSelIndex(-1);
		 m_Countries->SetSelIndex(-1);
		 m_InsSum->SetSelIndex(-1);
		 m_InsSumCurr->SetSelIndex(-1);
	  }
      else return 0;
   }

   (*base)(2, buffer.Insurer);
   (*base)(3, buffer.Address);
   (*base)(4, buffer.BirthDate);

   if(strlen(buffer.BirthDate) > 2 && !GD(buffer.BirthDate)) {
		SetFocus(m_InsurerBirthDate->HWindow);
   		return 0;
   }

   if(!buffer.Passport[0] && !IsBad)
    {
       SetFocus(m_Passport->HWindow);
       return 0;
    }
   (*base)(5, buffer.Passport);
   (*base)(41, buffer.Passport2);

   if(strcmp(buffer.Insurer, buffer.Insurer2)) {
	   if((!buffer.Passport2[0] || !strcmp(buffer.Passport, buffer.Passport2)) && !IsBad )
		{
		   SetFocus(m_Passport2->HWindow);
		   return 0;
		}
   }

   (*base)(6, (short)m_Countries->GetSelIndex());
   strcat(buffer.Conditions, "|");
   strcat(buffer.Conditions, buffer.PaymentNumber);
   (*base)(7, buffer.Conditions);
   if(!IsBad && *buffer.Insurer2 == 0) {
       SetFocus(m_Insurer2->HWindow);
       return 0;
   }
      
   (*base)(8, buffer.Insurer2);

   DATE date, fromdate, todate, regdate = 0, paydate = 0;
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

   if(atoi(buffer.Duration) < 1 && !IsBad)
   {
      SetFocus(m_Duration->HWindow);
      return 0;
   }

   if(atoi(buffer.Duration) > atoi(buffer.Period) && !IsBad)
   {
      SetFocus(m_Duration->HWindow);
      return 0;
   }

   if(!IsBad) {
	 todate = date = GD(buffer.ToDate);
     if(!date)
      {
         SetFocus(m_ToDate->HWindow);
	     return 0;
      }
      if(fromdate > todate) {
	 	SetFocus(m_ToDate->HWindow);
	 	return 0;
	  }

   }
   else date = BLANKDATE;
   (*base)(10, date);

   (*base)(11, (short)atoi(buffer.Duration));
   char buff[32];
   m_InsSum->GetText(buff, sizeof buff);
   (*base)(12, atof(buff));
   (*base)(21, atof(buffer.UridSumm));
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

   if(!IsBad) {
	   paydate = date = GD(buffer.PaymentDate);
	   if(!date || paydate > fromdate) {
		   SetFocus(m_PaymentDate->HWindow);
		   return 0;
	   }
	   (*base)(15, date);
   }

   (*base)(16, short(IsBad ? 0 : max(1, atoi(buffer.InsCnt))));

   //if(!IsBad) {
	regdate = date = GD(buffer.RegDate);
       if(!date)
	 {
	   SetFocus(m_RegDate->HWindow);
	   return 0;
	 }
      (*base)(17, date);
   //}
	DATE repdate = date = GD(buffer.ReportDate);
       if(!date)
	 {
	   SetFocus(m_ReportDate->HWindow);
	   return 0;
	 }
	  (*base)(37, date);

	  if(abs(repdate - paydate) > 100 && !IsBad && !*buffer.PSeria) {
		SetFocus(m_ReportDate->HWindow);
	 	return 0;
	  }


   DATE _2005;
   PXDateEncode(1, 1, 2005, &_2005);
   if((regdate > (paydate + 30)) && !IsBad && !IsDup || regdate < _2005) {
		SetFocus(m_RegDate->HWindow);
		return 0;
   }
   if(IsDup && regdate >= todate) {
		SetFocus(m_RegDate->HWindow);
		return 0;
   }

   if(IsDup && (regdate < fromdate)) {
		if(MessageBox(HWindow, "Вы уверены что дубликат до начала страхования?", "Ошибка", MB_YESNO | MB_ICONQUESTION) == IDNO) {
			SetFocus(m_RegDate->HWindow);
			return 0;
        }
   }

   if(regdate > fromdate && !IsBad && !IsDup) {
		SetFocus(m_RegDate->HWindow);
		return 0;
   }

   int Index = m_AgentCombo->GetSelIndex();
   if(Index == -1) {
      MessageBox(HWindow, "Введи агента", "Ошибка", MB_OK | MB_ICONSTOP);
	  return 0;
   }

   char AgCode[10];

   long ag_code = SendMessage(m_AgentCombo->HWindow, CB_GETITEMDATA, Index, 0);
   memmove(AgCode, &ag_code, 4);
   AgCode[4] = 0;
   (*base)(18, AgCode);

   if(!IsBad) 
	  (*base)(19, buffer.PayNal ? "N" : "B");

   if(!IsBad) {
	 (*base)(20, buffer.InsType ? "F" : "U");
   }

   if(!IsBad) { //Period 1, 2
      m_InsSumCurr->GetText(buff, sizeof buff);
 	  (*base)(22, buff);
   }

   if(!IsBad) {
      char strStates[129];
      GetStatesKs(strStates);
     (*base)(23, strStates);
   }

   /////////////////////
	if(*buffer.PSeria) { //Поиск сцепки!!! сумма 0
		 DynStr _buffer2;
		 sprintf(_buffer2, "Вы уверены, что полис %s/%s утерян?", buffer.PSeria, buffer.PNumber);

		 if(MessageBox(HWindow, _buffer2, "Отвечай", MB_YESNO | MB_ICONQUESTION) == IDNO)
			 return 0;

		  char* ShortFields[] = { "Seria", "Number", "STATE" };
		  TOpenBase& I = *new TOpenBase(Prvt2005Name, ShortFields, 3);
		  if(I.pxErr) {
		     Err:MessageBox(HWindow, GetRCStr(552), "Ошибка", MB_OK | MB_ICONSTOP);
			 return 0;
		  }
		  //Проверим предыдущий для скорости
		  I(0, buffer.PSeria);
		  I(1, atof(buffer.PNumber));
		  int IsFound = FALSE;
		  if(PXSrchKey(I.getTblHandle(), I.getRecHandle(), 2, SEARCHFIRST) == PXSUCCESS) {
			  I.Get();
			  IsFound = TRUE;
			  I(2, STATE_LOST);
			  if(PXRecUpdate(I.getTblHandle(), I.getRecHandle()) != PXSUCCESS)
				  goto Err;
		  }
		  else {
			  MessageBox(HWindow, GetRCStr(551), "Ошибка", MB_OK | MB_ICONSTOP);
			  return 0;
		  }
		  delete &I;
	}

   if(!IsBad) {
	   if(atof(buffer.Sum1) < 0.01 && !*buffer.PSeria) {
	  		SetFocus(m_Sum1->HWindow);
          return 0;
       }
	   (*base)(24, IsDup ? 0. : atof(buffer.Sum1));
       char CurrCode[4];
       m_Sum1Combo->GetText(CurrCode, sizeof CurrCode);
	   if(!*CurrCode && !*buffer.PSeria) {
			SetFocus(m_Sum1Combo->HWindow);
			return 0;
       }
	   (*base)(25, IsDup ? "" : CurrCode);

	   (*base)(26, IsDup ? 0 : atof(buffer.Sum2));
       m_Sum2Combo->GetText(CurrCode, sizeof CurrCode);
	   if(*CurrCode == 0 && atof(buffer.Sum2) >= 0.01 || *CurrCode && atof(buffer.Sum2) < 0.01)
       {
			SetFocus(m_Sum2Combo->HWindow);
			return 0;
       }
	   (*base)(27, IsDup ? "" : CurrCode);

       if(atof(buffer.AgPercent) > 50 || atof(buffer.AgPercent) < 0) {
		  MessageBox(HWindow, GetRCStr(603), "Ошибка", MB_OK | MB_ICONSTOP);
		  return 0;
	   }
	  (*base)(28, IsDup ? 0. : atof(buffer.AgPercent));
	  (*base)(29, buffer.AgUrid ? "U" : "F");

	  if(strchr(buffer.FromTime, ':') == 0 || atoi(buffer.FromTime)>23) {
		SetFocus(m_FromTime->HWindow);
		return 0;
	  }
	  if(atoi(strchr(buffer.FromTime, ':') +1)>59) {
		SetFocus(m_FromTime->HWindow);
		return 0;
	  }

	  (*base)(31, buffer.FromTime);

	  DATE stopdate = 0;
	  if(*buffer.StopDate) {
	   	stopdate = GD(buffer.StopDate, 1);
		if(!stopdate || (stopdate && (stopdate < fromdate && stopdate < regdate || stopdate >= todate))) {
			SetFocus(m_StopDate->HWindow);
			return 0;
		}
 	    (*base)(32, stopdate);
	  }

	  (*base)(33, atof(buffer.RetSum));

	   m_RetSumCombo->GetText(CurrCode, sizeof CurrCode);
	   if(*CurrCode == 0 && atof(buffer.RetSum) >= 0.01 || *CurrCode && atof(buffer.RetSum) < 0.01)
	   { 
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
		   (*base)(35, MyState == *STATE_LOST ? STATE_LOST : STATE_NORMAL);
		   SetState(MyState == *STATE_LOST ? STATE_LOST : STATE_NORMAL);
	   }
   }
   else {
		(*base)(28, 0.); //AgPercent
		(*base)(35, STATE_BAD);
		SetState(STATE_BAD);
   }

   if(!IsBad) {   
		(*base)(30, (double)m_Assistant->GetSelIndex());
		if(m_Assistant->GetSelIndex() == -1) {
	   		SetFocus(m_Assistant->HWindow);
	   		return 0;
		}
		GetWindowText(m_Period1->HWindow, buff, sizeof buff);
		if(atoi(buff) > 366 || atoi(buff) < 1) {
			SetFocus(m_Period1->HWindow);
        	return 0;
		}
		(*base)(36, (short)atoi(buff));
   }

   return 1;
}

Private2005Dlg::~Private2005Dlg()
{
//   EnableMenuItem(GetSubMenu(GetMenu(Parent->HWindow), 2), 1, MF_BYPOSITION | MF_ENABLED);
}

void Private2005Dlg::Find()
{
   HMENU Popup = CreatePopupMenu();
   AppendMenu(Popup, MF_ENABLED | MF_STRING, 700, "По Серии/Номеру");
   AppendMenu(Popup, MF_ENABLED | MF_STRING, 701, "По Застрахованному");
   AppendMenu(Popup, MF_ENABLED | MF_STRING, 702, "По Страхователю");
   POINT ptCurrent;
   GetCursorPos(&ptCurrent);
   TrackPopupMenu(Popup, TPM_LEFTALIGN, ptCurrent.x, ptCurrent.y, 0, HWindow, NULL);
   DestroyMenu(Popup);
}

void Private2005Dlg::WMCommand(RTMessage msg)
{
   Dlg_base::WMCommand(msg);
   if(msg.WParam == 700) FindSN();
   if(msg.WParam == 701) FindName(1);
   if(msg.WParam == 702) FindName(0);
}

void Private2005Dlg::FindSN()
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

void Private2005Dlg::FindName(int Insurer)
{
    char b[16];
	char str[64];
	if((Insurer ? m_Insurer : m_Insurer2)->GetText(b, sizeof b)) {
	    Array Strings(10, 0, 10);
		int Result = auxFindLike(Prvt2005Name, Insurer ? "Insurer" : "Insurer2", b, Strings, this, str, 0, 0);
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
}

char* GetSummaText(double Value, char* Valuta, char* Kop = NULL);
void SetSumma(char* dest, char* v, char*);

DynStr GetKopeykaText(double Val, char* Curr)
{
	DynStr result;
	//копейки
    Val = (int)((Val - (int)Val) * 100 + 1e-5);
    if(Val > 1)
    {
		strcat(result, "  ");
		strcat(result, GetSummaText((int)Val, !strcmp(Curr, "BRB") || !strcmp(Curr, "RUR") ? "копеек" :  "центов"));
	}
	return result;
}

int dayInMonth(int month, int year)
{
    switch(month) {
    	case 1 : return 31;
	case 3 : return 31;
	case 4 : return 30;
	case 5 : return 31;
	case 6 : return 30;
	case 7 : return 31;
	case 8 : return 31;
	case 9 : return 30;
	case 10 : return 31;
	case 11 : return 30;
	case 12 : return 31;
    }
    if ((year % 4==0) && (year % 100!=0)) return 29;
    else if ((year % 100==0) && (year % 400==0)) return 29;
    return 28;
}

_str FormatPeriod(char* sd1, char* sd2)
{
    _str res;
    int ys = 0, ms = 0, ds = 0;

    DynStr s;
    strcpy(s, sd1);
    DATE dt1 = GD(s); 
    strcpy(s, sd2);
    DATE dt2 = GD(s);

    /*
    int y1, m1, d1;
    int y2, m2, d2;
    int y3, m3, d3;

    PXDateDecode(dt1, &m1, &d1, &y1);
    PXDateDecode(dt2, &m2, &d2, &y2);

    //MessageBox(0, sd1, sd2, 0);

    DATE xd;
    for(ms = 1; ms <= 13; ms++) {
	int mm = m1 + ms, yy = y1;
	if(mm > 12) {
	   yy++;
	   mm = mm % 12;        	
        }

	PXDateEncode(mm, d1, yy, &xd);
        PXDateDecode(xd, &m3, &d3, &y3);
	while(d3 != d1) {
	    PXDateDecode(--xd, &m3, &d3, &y3);
	}

	if(xd - 1 == dt2) {
	    sprintf(res.s, "%d month%s", ms, ms > 1 ? "es" : "");
	    return res;
	}
	if(xd > dt2) { //1.1   -> 1.2
	    if(ms == 1) break;
            ms--;
	    mm = m1 + ms, yy = y1;
	    if(mm > 12) {
	       yy++;
	       mm = mm % 12;
	    }
	    PXDateEncode(mm, d1, yy, &xd);
	    ds = dt2 - xd + 1;
	    sprintf(res.s, "%d month%s %d day%s", ms, ms > 1 ? "es" : "", ds, ds > 1 ? "s" : "");
	    return res;
	}
    }*/


    sprintf(res.s, "%d days", dt2 - dt1 + 1);
    return res;
}

void Private2005Dlg::Print1()
{
   int Version2006 = 1; //GetPrivateProfileInt("PRIV2005", "Print", 0, ININame) != 5;

   if(!SetDataToBase()) return;
   Dlg_base::SetDataToBase();

   char* passPart2 = strpbrk(buffer.Passport, "-;=,");
   if(passPart2 != NULL)
   {
		*passPart2 = 0;
		passPart2 += 1;
   }
   else
   {
		passPart2 = buffer.Passport2;
   }
   char* str = new char [3000];
   char* dest = str;

   memset(str, 0, 2800);

   strcpy(dest, buffer.Insurer2);
   dest += strlen(dest) + 1;
   strcpy(dest, buffer.Address);
   dest += strlen(dest) + 1;
   strcpy(dest, buffer.Insurer);
   dest += strlen(dest) + 1;
   strcpy(dest, buffer.BirthDate);
   dest += strlen(dest) + 1;
   strcpy(dest, buffer.Passport);
   dest += strlen(dest) + 1;
   strcpy(dest, buffer.Country);

   dest += strlen(dest) + 1;
   strcpy(dest, FormatPeriod(buffer.FromDate, buffer.ToDate).s); //buffer.Period
   dest += strlen(dest) + 1;
   strcpy(dest, buffer.FromDate);
   dest += strlen(dest) + 1;
   strcpy(dest, buffer.ToDate);
   dest += strlen(dest) + 1;
   if(strcmp(buffer.Duration, buffer.Period))
   {
	sprintf(dest, "%s days", buffer.Duration);
   }
   else
   {
	strcpy(dest, FormatPeriod(buffer.FromDate, buffer.ToDate).s);
   }
   dest += strlen(dest) + 1;

   char* curr_name = "";
   if(Version2006 == 0)
   {
	   m_InsSum->GetText(dest, 25);
	   char summa[15];
	   strcpy(summa, dest);
	   strcat(dest, " ");
	   dest += strlen(dest);
	   curr_name = dest;
	   m_InsSumCurr->GetText(dest, 25);

	   char curr_[5];
	   m_InsSumCurr->GetText(curr_, 5);
	   dest += strlen(dest) + 1;
	   SetSumma(dest, summa, curr_);
	   dest += strlen(dest) + 1;
   }
   else
   {
	   //Сумма прописью попадает в сумму между числом и валютой а вместо пропислс будет ФИО плательщика
	   m_InsSum->GetText(dest, 25);
	   char summa[15];
	   strcpy(summa, dest);
	   strcat(dest, " ( ");
	   dest += strlen(dest);
	   SetSumma(dest, summa, "");
	   strcat(dest, ") ");
	   dest += strlen(dest);
	   curr_name = dest;
	   m_InsSumCurr->GetText(dest, 25);
	   dest += strlen(dest) + 1;
	   //strcpy(dest, buffer.Insurer);
	   dest += strlen(dest) + 1;
   }
  
   strcpy(dest, buffer.Payment);
   //strcat(dest, " (");
   //double Val = atof(buffer.Payment) + 1e-5;
   //strcat(dest, GetSummaText((int)Val, ""));
   //strcat(dest, ") ");
   strcat(dest, " ");
   dest += strlen(dest);
   m_InsSumCurr->GetText(dest, 25);
   if(strcmp(dest, "BRB") == 0) strcpy(dest, "рублей");
   char Curr[5];
   m_InsSumCurr->GetText(Curr, 5);

   //strcat(dest, GetKopeykaText(Val, Curr));

   dest += strlen(dest) + 1;


   if(Version2006 == 0)
   {
   	   strcpy(dest, buffer.Sum1);
       strcat(dest, " ");
       dest += strlen(dest);
       m_Sum1Combo->GetText(dest, 25);
	   curr_name = dest;
	   if(strcmp(dest, "BRB") == 0) curr_name = "рублей";

	   if(Version2006 == 0)
	   {
			dest += strlen(dest) + 1;
	   }
	   else
	   {
			strcat(dest, "      ");
	   }
	   strcat(dest, GetSummaText(atof(buffer.Sum1), curr_name));
	   dest += strlen(dest) + 1;
   }
   else
   {
   		
	   strcpy(dest, buffer.Sum1);
	   strcat(dest, " ( ");
	   double Val = atof(buffer.Sum1) + 1e-5;
	   strcat(dest, GetSummaText((long)Val, ""));
	   strcat(dest, ")  ");
	   dest += strlen(dest);
	   m_Sum1Combo->GetText(dest, 25);
	   m_Sum1Combo->GetText(Curr, 5);
	   if(strcmp(dest, "BRB") == 0) strcpy(dest, "рублей");
	   strcat(dest, GetKopeykaText(Val, Curr));
	   if(fabs(Val - (int)Val) < 0.01) strcat(dest, "\r\n");
	   strcat(dest, " Страхователем");
	   dest += strlen(dest) + 1;

/*    5 (пять) USD двадцать семь центов ИВАНОВ ИВАН ИВАНЫЧ
	   strcpy(dest, buffer.Sum1);
	   strcat(dest, " (");
	   double Val = atof(buffer.Sum1) + 1e-5;
	   strcat(dest, GetSummaText((int)Val, ""));
	   strcat(dest, ") ");
	   dest += strlen(dest);
	   m_Sum1Combo->GetText(dest, 25);
	   m_Sum1Combo->GetText(Curr, 5);
	   if(strcmp(dest, "BRB") == 0) strcpy(dest, "рублей");
       strcat(dest, GetKopeykaText(Val, Curr));
	   strcat(dest, " ");
	   strcat(dest, buffer.Insurer2);
	   dest += strlen(dest) + 1;*/
   }

   strcpy(dest, (m_PayNal->GetCheck() == 0) ? "X" : " ");
   dest += strlen(dest) + 1;
   strcpy(dest, (m_PayNal->GetCheck() == 1) ? "X" : " ");

   dest += strlen(dest) + 1;
   strcpy(dest, buffer.PaymentNumber); //N п.п.

   dest += strlen(dest) + 1;
   strcpy(dest, buffer.PaymentDate);

   dest += strlen(dest) + 1;
   strcpy(dest, buffer.Conditions);

   dest += strlen(dest) + 1;
   int m, d, y;
   DATE d1 = GD(buffer.PaymentDate);
   DATE d2 = GD(buffer.FromDate);
   PXDateDecode(max(d1, d2), &m, &d, &y);

   if(Version2006 == 0)
   {
		sprintf(dest, "%02d.%02d.%04d", d, m, y);
   }
   else
   {
	   strcpy(dest, buffer.RegDate);
   }

   dest += strlen(dest) + 1;
   strcpy(dest, buffer.RegDate);
   dest += strlen(dest) + 1;
   m_AgentCombo->GetText(dest, 32);
   dest += strlen(dest) + 1;

   if(Version2006 == 1)
   {
		//passport
		if(!strcmpi(buffer.Insurer, buffer.Insurer2))
		{
		   strcpy(dest, buffer.Passport);
		   dest += strlen(dest) + 1;
		}
		else
		{
			if(passPart2 == NULL || !*passPart2)
			{
				delete str;
				strcat(buffer.Passport, "-<Набери номер паспорта>");
				m_Passport->SetText(buffer.Passport);
				SetFocus(m_Passport->HWindow);
				MessageBox(HWindow, "Разные имена. Введите два паспорта через ;", "!", MB_OK | MB_ICONSTOP);
				return;
            }
            else
			{
			   strcpy(dest, passPart2);
			   dest += strlen(dest) + 1;
			}
		}
	   //Пропустить 2 пустых окна
	   dest += strlen(dest) + 1;
	   dest += strlen(dest) + 1;

	   //Дата выдачи в маленькое окошко
	   strcpy(dest, buffer.RegDate);
	   dest += strlen(dest) + 1;
   }

   ((TRulerWin*)Parent)->SetValues(str);
   Dlg_base::Print1();
   delete str;
}

int getDurationType(char* str, char** _pstr = 0);
DATE aux_incDate(DATE startdate, int inc1, int value);

void Private2005Dlg::SetDates()
{
	if(!CanModifyPrivate2005/* || m_Vid1Combo->GetSelIndex() > 3*/)
       return;

	char datebuf[15];
    m_FromDate->GetText(datebuf, sizeof datebuf);
	DATE startdate = GD(datebuf, 1);
	m_Period1->GetText(datebuf, sizeof datebuf);

	if(startdate) {
	   DATE enddate = startdate + atoi(datebuf);

	   int d, m, y;
	   while(PXDateDecode(--enddate, &m, &d, &y) != PXSUCCESS);
	   sprintf(datebuf, "%02u.%02u.%04u", d, m, y);
       m_ToDate->SetText(datebuf);
	}
}

void Private2005Dlg::ChangePeriod3(RTMessage msg)
{
	if(msg.LP.Hi == EN_CHANGE)
		SetDates();
}

void Private2005Dlg::ChangeDuration(RTMessage msg)
{
	if(CanModifyPrivate2005 && msg.LP.Hi == EN_CHANGE) {
		PostMessage(HWindow, 1000, 1, 0);
	}
}

void Private2005Dlg::ChangeStartDate(RTMessage msg)
{
	if(msg.LP.Hi == EN_CHANGE)
		SetDates();
}

void Private2005Dlg::Calculate(RTMessage msg)
{
   m_Tarif->SetText("");
   m_Payment->SetText("");

   int Index = m_InsSum->GetSelIndex();
   if(Index < 0) return;
   DynStr buff;
   m_InsCnt->GetText(buff, buff._sizeof());
   int Group = max(1, atoi(buff));
   m_Duration->GetText(buff, buff._sizeof());
   int Duration = atoi(buff);

   double Tarif = 0;
   for(int i = 0;; i++) {
	   sprintf(buff, "TarifPeriod%d", i);
	   if(!GetPrivateProfileString(FORMSECTION, buff, "", buff, buff._sizeof(), ININame))
		   return;
	   char* ch = strchr(buff, '-');
	   if(!ch) return;
	   if(Duration>=atoi(buff) && Duration<=atoi(ch+1)) {
		   char* ch = strchr(buff, ','); //после периода
		   if(!ch) return;

		   for(int n = 0; n < (TarifCol + Index); n++) {
			   ch = strchr(ch + 1, ',');
			   if(!ch) return;
		   }
		   Tarif = atof(ch + 1) * Group;
		   break;
       }
   }

   sprintf(buff, "%g", Tarif);
   m_Tarif->SetText(buff);

   int idxChecks[128];
   int Count = m_Ks->GetSelIndexes(idxChecks, 128);
   double K = 1;
   char formula[64] = "K = ";
   DynStr UridBuff;
   int UridFlag = 0;
   for(i = 0; i < Count; i++) {
	   double Koeff = int(SendMessage(m_Ks->HWindow, LB_GETITEMDATA, idxChecks[i], 0)) / 100.;
	   int Idx1 = SendMessage(m_Ks->HWindow, LB_GETITEMDATA, idxChecks[i], 0) >> 16;
	   if(i) strcat(formula, " * ");
       sprintf(formula + strlen(formula), "%g", Koeff);
	   K *= Koeff;

	   sprintf(UridBuff, "Urid%d_%d", (int)HIBYTE(Idx1), (int)LOBYTE(Idx1));
	   if(GetPrivateProfileInt(FORMSECTION, UridBuff, 0, ININame)) UridFlag = 1;
   }

   if(!UridFlag)
	   m_UridSum->SetText("");

   if(strlen(formula) > 4) {
      if(Count == 1) formula[4] = 0;
      else strcat(formula, " = ");
      sprintf(formula + strlen(formula), "%g", K);
      m_Conditions->SetText(formula);
   }

   Tarif *= K;
   Tarif = long((Tarif + 0.005) * 100) / 100.;
   DblToStr(buff, Tarif);
   m_Payment->SetText(buff);
   m_Sum1->SetText(buff);
   //strcpy(BaseSumm, buff);
   *BaseSumm = 0;
   m_InsSumCurr->GetText(buff, sizeof buff);
	m_Sum1Combo->SetText(buff);
   m_Sum2->SetText("");
   m_Sum2Combo->SetSelIndex(0);
}

/////////////////////////////////////

void  Private2005Dlg::SetInsurer2(RTMessage)
{
   char buffer[128];
   m_Insurer->GetText(buffer, sizeof buffer);
   m_Insurer2->SetText(buffer);
}

void Private2005Dlg::Clear()
{
	CanModifyPrivate2005 = 1;
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

   struct time t;
   gettime(&t);
   date d;
   getdate(&d);
   char buff[100];
   wsprintf(buff, "%02u:%02u", t.ti_hour, t.ti_min);
   m_FromTime->SetText(buff);
   wsprintf(buff, "%02u.%02u.%04u", d.da_day, d.da_mon, d.da_year);
   m_FromDate->SetText(buff);
   m_PaymentDate->SetText(buff);
   m_ReportDate->SetText(buff);
   m_RegDate->SetText(buff);

   GetPrivateProfileString(FORMSECTION, "Seria", "", buff, sizeof buff, ININame);
   m_Seria->SetText(buff);

   m_PayNal->SetCheck(1);
   m_InsFiz->SetCheck(1);

   if(*Dlg_base::Agent_Code/*GetPrivateProfileString(FORMSECTION, "DefAgent", "", buff, sizeof buff, ININame)*/) {
	  SetAgent(Dlg_base::Agent_Code, agents, m_AgentCombo, 0);
      TMessage msg;
      msg.LP.Hi = CBN_SELCHANGE;
      ChAgent(msg);
   }

   for(int i = 0; i < m_Ks->GetCount(); i++)
      m_Ks->SetSelIndexes(&i, 1, FALSE);

   SendDlgItemMessage(HWindow, 303, WM_SETTEXT, 0, (long)"Выбрано 0");

   m_Assistant->SetSelIndex(GetPrivateProfileInt(FORMSECTION, "DefAssist", 0, ININame));
   m_Countries->SetSelIndex(GetPrivateProfileInt(FORMSECTION, "DefCountry", 0, ININame));
   m_InsSumCurr->SetSelIndex(0);
   //m_InsSumCurr->GetText(OldCurr, sizeof OldCurr);
   *BaseSumm = 0;
   *OldCurr = 0;
   m_Duration->SetText("");

   TMessage msg;
   Change(msg);
   msg.LP.Hi = CBN_SELCHANGE;
   ChangeCountry(msg);
}

void Private2005Dlg::Help(RTMessage msg)
{
   if(msg.WParam == 1000)
      WinHelp(GetApplication()->MainWindow->HWindow, "blank.hlp", HELP_CONTEXT, HELP_PRIVATE);

   Dlg_base::Help(msg);
}

void Private2005Dlg::WMCtlColor(RTMessage msg)
{
   Dlg_base::WMCtlColor(msg);
   if(msg.LP.Hi == CTLCOLOR_STATIC && (HWND)msg.LP.Lo == GetDlgItem(HWindow, 103)) 
	SetTextColor((HDC)msg.WParam, RGB(255, 0, 0));
   if(msg.LP.Hi == CTLCOLOR_STATIC && (HWND)msg.LP.Lo == GetDlgItem(HWindow, 1002))
	SetTextColor((HDC)msg.WParam, RGB(0, 0, 255));
}

void Private2005Dlg::Change(RTMessage msg)
{
   CanModifyPrivate2005 = 1;
   EnableWindow(GetDlgItem(HWindow, 406), TRUE);
}


void Private2005Dlg::WMMove(RTMessage msg)
{
	 AlignCombo(m_AgentCombo);
	 AlignCombo(m_Countries);
	 AlignCombo(m_InsSum);
	 AlignCombo(m_InsSumCurr);
     AlignCombo(m_Sum1Combo);
     AlignCombo(m_Sum2Combo);
	 AlignCombo(m_Assistant);
}

void Private2005Dlg::ChangeKs(RTMessage msg)
{
     if(msg.LP.Hi == LBN_SELCHANGE) {
		 if(!CanModifyPrivate2005 && ActivateParadox) {
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
		 msg.LP.Hi = CBN_SELCHANGE;
		 ChangeInsSumma(msg);

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

void Private2005Dlg::SetStatesKs(char* States) //по 2 значения
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

void Private2005Dlg::GetStatesKs(char* strStates) //по 2 значения
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

void Private2005Dlg::FillKs()
{
   m_Ks->ClearList();

	char* mask = "K%d_%d";
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

void ToRubles(double& Value);

void Private2005Dlg::RoundSumma(RTMessage)
{
    char buff[32];
	//m_Sum1Combo->GetText(OldCurr, sizeof OldCurr);
    m_Payment->GetText(buff, sizeof buff);
	DblToStr(buff, long(atof(buff) + 0.5), 2);
	m_Payment->SetText(buff);
	m_Sum1->GetText(buff, sizeof buff);
	double summ = atof(buff);

			if(m_InsFiz->GetCheck() == 1 && !strcmp("BRB", OldCurr)) {
			    ToRubles(summ);
			}
			else {
				summ = long(summ + 0.5);
                        }


	DblToStr(buff, summ, 2);
	m_Sum1->SetText(buff);
	//strcpy(BaseSumm, buff);
	*BaseSumm = 0;
	*OldCurr = 0;

	m_Sum2->SetText("");
	m_Sum2Combo->SetSelIndex(0);
}

void Private2005Dlg::ChangeGroup(RTMessage msg)
{
    if(msg.LP.Hi == EN_CHANGE) {
		if(!CanModifyPrivate2005) return;
	 	PostMessage(HWindow, 1000, 0, 0);
	}
}

void Private2005Dlg::ChangeNumber(RTMessage msg)
{
     if(msg.LP.Hi == EN_CHANGE) {
		MyState = 0;
		if(!CanModifyPrivate2005) return;
		m_Insurer2->SetText("");
	 }
}

void Private2005Dlg::ChAgent(RTMessage msg)
{
   int IsUr;
   if(msg.LP.Hi == CBN_SELCHANGE) {
	   ((TEdit*)ChildWithId(53))->SetText(GetAgPercent(SendMessage(m_AgentCombo->HWindow, CB_GETITEMDATA, m_AgentCombo->GetSelIndex(), 0), LEBEN2_TBL, IsUr));
	   m_AgUr->SetCheck(IsUr);
   }
}

static int ImpFunc(TOpenBase* FileDb, TOpenBase* Db, void* Data)
{
	if(FileDb->GetDouble(13) != Db->GetDouble(13))
	{
		DynStr msg;
		sprintf(msg, "%s/%5.0f\nБыло %s %5.0f\nСтало %s %5.0f\n$%s", Db->GetString(0).s, Db->GetDouble(1),
		Db->GetString(2).s, Db->GetDouble(13), FileDb->GetString(2).s, FileDb->GetDouble(13), GetRCStr(637));
		if(MessageBox(GetApplicationObject()->MainWindow->HWindow, msg, "!", MB_ICONQUESTION | MB_YESNO) != IDYES)
		{
			*strchr(msg, '$') = 0;
			((Array*)Data)->add(*new String(msg));
			return 0;
        }

    }
	return 1;
}

void Private2005Dlg::Import()
{
	Array list(100);
	list.ownsElements(TRUE);
	long Result = ImportData(Prvt2005Name, Prvt2005Fields, Prvt2005Count, ImpFunc, &list, 18, 35, atoi(STATE_BAD), 0, 0);
	if(Result) {
		MessageBox(GetApplicationObject()->MainWindow->HWindow, GetRCStr(700+Result), "!", MB_ICONEXCLAMATION);
	}
	ShowArray(list);
}

void Private2005Dlg::Export(PTWindowsObject Parent)
{
	Priod_Params* data = new Priod_Params;
	strcpy(data->filename, Prvt2005Name);
	if(GetApplicationObject()->ExecDialog(new getDate(Parent, &data->m, &data->y, &data->dm, data->filename)) == IDOK) {
		long Error = ExportData(Prvt2005Name, Prvt2005Fields, Prvt2005Count, 0, 0, data->filename, data->m, data->y, data->dm, 18, 10);
		sprintf(data->filename, GetRCStr(Error > 0 ? 621: 622), labs(Error));
		MessageBox(Parent->HWindow, data->filename, "", 0);
	}
	delete data;
}

void Private2005Dlg::ChangeCountry(RTMessage msg)
{
	DynStr buff;
	if(msg.LP.Hi == CBN_SELCHANGE) {
		TarifCol = 0;
		m_InsSum->ClearList();
		for(int i = 0; i <= m_Countries->GetSelIndex(); i++) {
			sprintf(buff, "Country%dInsSum", i);
			if(!GetPrivateProfileString(FORMSECTION, buff, "", buff, buff._sizeof(), ININame)) {
			   MessageBox(HWindow, buff, FORMSECTION, 0);
			   break;
			}
			char* pbuff = buff;
			char* ch = strchr(buff, ',');

			while(ch) {
				if(i == m_Countries->GetSelIndex()) {
					*ch = 0;
					m_InsSum->AddString(pbuff);
				}
				if(i < m_Countries->GetSelIndex())
					TarifCol++;
				pbuff = ch + 1;
				ch = strchr(pbuff, ',');
			}
			if(i < m_Countries->GetSelIndex())
				TarifCol++;
			if(i == m_Countries->GetSelIndex())
				m_InsSum->AddString(pbuff);
		}
		m_InsSum->SetSelIndex(0);
		ChangeInsSumma(msg);

	   char key[32];
	   sprintf(key, "pcountry%d", m_Countries->GetSelIndex());
	   GetPrivateProfileString(FORMSECTION, key, "?", buffer.Country, sizeof buffer.Country, ::ININame);
	   m_Country->SetText(buffer.Country);
	}
}

void Private2005Dlg::ChangeInsSumma(RTMessage msg)
{
	if(msg.LP.Hi == CBN_SELCHANGE && CanModifyPrivate2005) {
		DynStr buff;
		sprintf(buff, "Country%dUrSum", m_Countries->GetSelIndex());
		if(!GetPrivateProfileString(FORMSECTION, buff, "", buff, buff._sizeof(), ININame))
			return;
		char* s = strtok(buff, ",");
		for(int i = 0; i < m_InsSum->GetSelIndex(); i++)
			s = strtok(0, ",");
		m_UridSum->SetText(s);
		if (CanModifyPrivate2005) {
			PostMessage(HWindow, 1000, 1, 0);
		}
	}
}

void ReplaceCurrs(TEdit* m_PayDate, char* OldCurr, TEdit* m_Pay1, TComboBox* m_Curr1, int);

void Private2005Dlg::ChPayCurr(RTMessage msg)
{
	if(msg.LP.Hi == CBN_SETFOCUS)
	{
	    m_InsSumCurr->GetText(OldCurr, sizeof OldCurr);
	    *BaseSumm = 0;
	}
	if(msg.LP.Hi == CBN_SELCHANGE)
	{
	    if(/**OldCurr && */GetWindowTextLength(m_Sum1Combo->HWindow) > 0)
            {
		if(!*BaseSumm)
		{
			//m_InsSumCurr->GetText(OldCurr, sizeof OldCurr);
			m_Sum1->GetText(BaseSumm, sizeof BaseSumm);
		}
		//MessageBox(0, OldCurr, BaseSumm, 0);
		char oc[5];
		strcpy(oc, OldCurr);
		ReplaceCurrs(m_PaymentDate, oc, m_Sum1, m_Sum1Combo, m_InsFiz->GetCheck() == 0);
		strcpy(OldCurr, oc);
		//MessageBox(0, OldCurr, BaseSumm, 0);
            }
	    //MessageBox(0, OldCurr, BaseSumm, 0);
	}
}

void  Private2005Dlg::ChMainCurr(RTMessage msg)
{
	if(msg.LP.Hi == CBN_SELCHANGE) {
		//m_InsSumCurr->GetText(OldCurr, sizeof OldCurr);
	}
}

int getDurationType(char* str, char** _pstr)
{
	AnsiLower(str);
	int result = 0;
	for(int i = 0; result == 0 && i < strlen(str); i++)
    {
	  if(!(strncmpi(str + i, "day", 3)) ||
		 !(strncmpi(str + i, "дн", 2))  ||
		 !(strncmpi(str + i, "сут", 3)) ||
		 !(strncmpi(str + i, "де", 2))) result = 1;
	  if(!(strncmpi(str + i, "mon", 3)) || !(strncmpi(str + i, "мес", 3))) result = 2;
	  if(!(strncmpi(str + i, "year", 4)) || !(strncmpi(str + i, "год", 3))) result = 3;
	}
	if(result && _pstr) {
		char* pstr = str + i;
		//MessageBox(0, pstr, "1", 0);
		while(pstr != str && (*pstr <= '0' || *pstr >= '9'))
			pstr--;
		//MessageBox(0, pstr, "2", 0);
		while(pstr != str && (*pstr >= '0' && *pstr <= '9'))
			pstr--;
		//MessageBox(0, pstr, "3", 0);
		while(pstr != str && (*pstr <= '0' || *pstr >= '9'))
			pstr++;
		//MessageBox(0, pstr, "4", 0);
		*_pstr = pstr;
	}
    return result;
}

DATE aux_incDate(DATE startdate, int inc1, int value)
{
   int d, m, y;
   PXDateDecode(startdate, &m, &d, &y);
   int startday = d;
   if(inc1) {
	if(inc1 == 1) startdate += value;
	if(inc1 == 2) {
		m += value;
		if(m > 12)
		{
        	m -= 12;
            y++;
        }
		while((PXDateEncode(m, d, y, &startdate) != PXSUCCESS) && (d > 1)) d--;
		if(startday > d) startdate++;
	}
	if(inc1 == 3)
	{
		if(PXDateEncode(m, d, y + value, &startdate) != PXSUCCESS) //leap year
		{
			PXDateEncode(m, d-1, y + value, &startdate);
			startdate++;
        }
    }
   }
   return startdate;
}

