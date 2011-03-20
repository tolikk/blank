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
#include <priv_frn.h>
#include <checkbox.h>
#include <thwindow.h>
#include <help.h>
#include <stdlib.h>
#include <stdio.h>
#include "util.h"
#include "radiobut.h"
#include "grncard.h"

/*

#define max(a, b) ((a) > (b) ? (a) : (b))

extern DATE GD(char* str, int = 0);
void DateFBToStr(TOpenBase* base, int field, char* buffer);
static const char* FORMSECTION = "FOREIGN";
void AlignCombo(TComboBox*);

void SetAgent(char* ACode, TOpenBase& agents, PTComboBox AgName, int);

char* Prvt3Fields[] = {
"Seria",
"Number",
"InsComp",
"RegDate",
"RegTime",
"FIO",
"State",
"Passport",
"Srok",
"DateFrom",
"DateTo",
"Insurer",
"Summa",
"Fee",
"PayDate",
"IsNal",
"AgentCode",
"Curr",
"AgPercent",
"PaySumma",
"Uridich"
		     };

int  Prvt3Count = 21;
char* Prvt3Name = "\\bases\\foreign";

int CanModifyPrivate3;

Private3Dlg::Private3Dlg(PTWindowsObject p)
	   : Dlg_base(p, "FOREIGN", Prvt3Name, Prvt3Fields, Prvt3Count),
             agents(AgentDBName, AgentFields, 2)
{
     CanModifyPrivate3 = 0;

     m_Seria = new TEdit(this, 200, 10);
     m_Number = new NumberEdit(this, 201, 11, 0);
     m_InsComp = new TEdit(this, 203, 48);
     m_RegDate = new TEdit(this, 204, 11);
     m_RegTime = new TEdit(this, 205, 6);
     m_FIO = new TEdit(this, 206, 64);
     m_State = new TEdit(this, 208, 32);
     m_Passport = new TEdit(this, 207, 32);
     m_Srok = new NumberEdit(this, 212, 4, 0);
     m_DateFrom = new TEdit(this, 210, 11);
     m_DateTo = new TEdit(this, 211, 11);
     m_Insurer = new TEdit(this, 209, 64);
     m_Summa = new NumberEdit(this, 213, 11, 1);
     m_Fee = new NumberEdit(this, 214, 6, 1);
     m_PaySumma = new NumberEdit(this, 52, 11, 1);
     m_PayDate = new TEdit(this, 215, 11);
     m_AgPercent = new NumberEdit(this, 54, 4, 1);
     new TCheckBox(this, 55, 0);

     m_AgentCombo = new TComboBox(this, 202, 1);
     m_AgentCombo->DisableTransfer();
     m_NalBezNalCombo = new TComboBox(this, 216, 1);
     m_NalBezNalCombo->DisableTransfer();
     m_PayCurr = new TComboBox(this, 53, 1);
     m_PayCurr->DisableTransfer();
}

void SetLittleFont(HWND HWindow, int* Numb, int nNumb);

void Private3Dlg::SetupWindow()
{
int Numb[] = { 54, 405 };

   Dlg_base::SetupWindow();

   m_NalBezNalCombo->AddString("Нал");
   m_NalBezNalCombo->AddString("Безнал");

   m_PayCurr->AddString("");
   m_PayCurr->AddString("USD");
   m_PayCurr->AddString("DM");
   m_PayCurr->AddString("EUR");
   m_PayCurr->AddString("RUR");
   m_PayCurr->AddString("BRB");

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
	 int Index = m_AgentCombo->AddString(Tmp);
	 SendMessage(m_AgentCombo->HWindow, CB_SETITEMDATA, Index, atoi(AgCode));
	 agents.Next();
       }
   }

   SetLittleFont(HWindow, Numb, sizeof(Numb) / sizeof(Numb[0]));

   if(!base->GetRecords())
       PostMessage(HWindow, WM_COMMAND, 405, MAKELONG(405, BN_CLICKED));

   GetDataFromBase();
}

extern void DblToStr(char* str, double V, int Digit = 2);

void Private3Dlg::GetDataFromBase()
{
   memset(&buffer, 0, sizeof buffer);
   (*base)(buffer.Seria, sizeof buffer.Seria, 0);
   double value;
   (*base)(value, 1);
   sprintf(buffer.Number, "%lu", (long)value);

   (*base)(buffer.InsComp, sizeof buffer.InsComp, 2);
   ::DateFBToStr(base, 3, buffer.RegDate);
   (*base)(buffer.RegTime, sizeof buffer.RegTime, 4);
   (*base)(buffer.FIO, sizeof buffer.FIO, 5);
   (*base)(buffer.State, sizeof buffer.State, 6);
   (*base)(buffer.Passport, sizeof buffer.Passport, 7);
   (*base)(value, 8);
   if((long)value > 0) {
	sprintf(buffer.Srok, "%lu", (long)value);
   }

   ::DateFBToStr(base, 9, buffer.DateFrom);
   ::DateFBToStr(base, 10, buffer.DateTo);
   
   (*base)(buffer.Insurer, sizeof buffer.Insurer, 11);
   (*base)(value, 12);
   DblToStr(buffer.Summa, value);
   (*base)(value, 13);
   DblToStr(buffer.Fee, value);
   (*base)(value, 19);
   DblToStr(buffer.PaySumm, value);

   ::DateFBToStr(base, 14, buffer.PayDate);

   (*base)(value, 18);
   DblToStr(buffer.AgPercent, value);

   char AgentStr[5];
   (*base)(AgentStr, sizeof AgentStr, 16);
   SetAgent(AgentStr, agents, m_AgentCombo, 0);

   int IsBad = *buffer.DateFrom == 0;

   m_NalBezNalCombo->SetSelIndex(-1);
   if(!IsBad) {
       (*base)(value, 15);
       m_NalBezNalCombo->SetSelIndex(value > 0 ? 0 : 1);
   }

   m_PayCurr->SetSelIndex(-1);
   if(!IsBad) {
       char curr[5];
       (*base)(curr, sizeof curr, 17);
       m_PayCurr->SetText(curr);
   }

   *AgentStr = 0;
   (*base)(AgentStr, sizeof AgentStr, 20);
   buffer.IsUridich = !strcmp("Y", AgentStr);


   CanModifyPrivate3 = 0;
   Dlg_base::GetDataFromBase();
   EnableWindow(GetDlgItem(HWindow, 406), FALSE);
}

int Private3Dlg::SetDataToBase()
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

   BOOL IsBad = (strlen(buffer.FIO) == 0) || (!strcmp("ИСПОРЧЕН", buffer.FIO));
   if(IsBad) {
      if(MessageBox(HWindow, "Сохранить полис как испорченный ?", "Отвечай поскорей", MB_YESNO | MB_ICONINFORMATION | MB_SYSTEMMODAL) == IDYES)
      {
	strcpy(buffer.FIO, "ИСПОРЧЕН");
	*buffer.RegTime = 0;
	*buffer.State = 0;
	*buffer.Passport = 0;
	*buffer.Srok = 0;
	*buffer.DateFrom = 0;
	*buffer.DateTo = 0;
	*buffer.Insurer = 0;
	*buffer.Summa = 0;
	*buffer.Fee = 0;
	*buffer.PaySumm = 0;
	*buffer.PayDate = 0;
	*buffer.AgPercent = 0;
	m_NalBezNalCombo->SetSelIndex(-1);
	m_PayCurr->SetSelIndex(-1);
      }
      else return 0;
   }

   DATE regdate = GD(buffer.RegDate);
   //DATE regdate = date;
   if(!regdate) {
      SetFocus(m_RegDate->HWindow);
      return 0;
   }
   (*base)(3, regdate);

   (*base)(2, buffer.InsComp);
   if(strlen(buffer.InsComp) == 0) {
      SetFocus(m_InsComp->HWindow);
      return 0;
   }
   (*base)(5, buffer.FIO);

   if(!IsBad) {
       (*base)(4, buffer.RegTime);
       (*base)(6, buffer.State);
       if(strlen(buffer.State) == 0) {
	  SetFocus(m_State->HWindow);
          return 0;
       }
       (*base)(7, buffer.Passport);
       if(strlen(buffer.Passport) == 0) {
	  SetFocus(m_Passport->HWindow);
          return 0;
       }
       double Val = atof(buffer.Srok);
       if(Val < 1 || Val > 366) {
	  SetFocus(m_Srok->HWindow);
          return 0;
       }
       (*base)(8, Val);

       DATE date_from = GD(buffer.DateFrom);
       if(!date_from) {
	  SetFocus(m_DateFrom->HWindow);
	  return 0;
       }
       (*base)(9, date_from);

       DATE date_to = GD(buffer.DateTo);
       if(!date_to) {
	  SetFocus(m_DateTo->HWindow);
	  return 0;
       }
       (*base)(10, date_to);

       if(date_from > date_to || regdate > date_from) {
          MessageBox(HWindow, "Даты регистрации и страхование не правильно соотносятся", "Ошибка", MB_OK | MB_ICONSTOP);
	  return 0;
       }

       (*base)(11, buffer.Insurer);
       if(strlen(buffer.Insurer) == 0) {
	  SetFocus(m_Insurer->HWindow);
          return 0;
       }
       (*base)(12, atof(buffer.Summa));
       (*base)(13, atof(buffer.Fee));
       (*base)(18, atof(buffer.AgPercent));
       (*base)(19, atof(buffer.PaySumm));

       if(atof(buffer.Summa) <= atof(buffer.Fee) || atof(buffer.PaySumm) < 0.01 || atof(buffer.Fee) < 0.01) {
	  MessageBox(HWindow, "Суммы какие-то странные", "Ошибка", MB_OK | MB_ICONSTOP);
	  return 0;
       }
       if(atof(buffer.AgPercent) > 50 || atof(buffer.AgPercent) , 0) {
	  MessageBox(HWindow, "Процент агенту какой-то не такой", "Ошибка", MB_OK | MB_ICONSTOP);
	  return 0;
       }

       DATE paydate = GD(buffer.PayDate);
       if(!paydate) {
	  SetFocus(m_PayDate->HWindow);
          return 0;
       }
       (*base)(14, paydate);

     if(m_NalBezNalCombo->GetSelIndex() == -1) {
	SetFocus(m_NalBezNalCombo->HWindow);
	return 0;
      }
     (*base)(15, (double)(m_NalBezNalCombo->GetSelIndex() == 0));

      char CurrCode[4];
      m_PayCurr->GetText(CurrCode, sizeof CurrCode);
      if(strlen(CurrCode) == 0) {
	SetFocus(m_PayCurr->HWindow);
	return 0;
      }
      (*base)(17, CurrCode);

      if(buffer.IsUridich)
	  (*base)(20, "Y");
      else
	  (*base)(20, "N");
   }

   int Index = m_AgentCombo->GetSelIndex();
   if(Index == -1) {
      MessageBox(HWindow, "Введи агента", "Ошибка", MB_OK | MB_ICONSTOP);
      return 0;
   }
   char AgCode[10];
   wsprintf(AgCode, "%04d", SendMessage(m_AgentCombo->HWindow, CB_GETITEMDATA, Index, 9));
   (*base)(16, AgCode);

   return 1;
}

Private3Dlg::~Private3Dlg()
{
//   EnableMenuItem(GetSubMenu(GetMenu(Parent->HWindow), 2), 1, MF_BYPOSITION | MF_ENABLED);
}

void Private3Dlg::Find()
{
   HMENU Popup = CreatePopupMenu();
   AppendMenu(Popup, MF_ENABLED | MF_STRING, 700, "По Серии/Номеру");
   AppendMenu(Popup, MF_ENABLED | MF_STRING, 701, "По Застрахованному");
   POINT ptCurrent;
   GetCursorPos(&ptCurrent);
   TrackPopupMenu(Popup, TPM_LEFTALIGN, ptCurrent.x, ptCurrent.y, 0, HWindow, NULL);
   DestroyMenu(Popup);
}

void Private3Dlg::StatesMenu(RTMessage)
{
/*   HMENU Popup = CreatePopupMenu();
   POINT ptCurrent;
   GetCursorPos(&ptCurrent);
   TrackPopupMenu(Popup, TPM_LEFTALIGN, ptCurrent.x, ptCurrent.y, 0, HWindow, NULL);
   DestroyMenu(Popup);* /
}

void Private3Dlg::WMCommand(RTMessage msg)
{
   Dlg_base::WMCommand(msg);
   if(msg.WParam == 700) FindSN();
   if(msg.WParam == 701) FindName();
/*   if(msg.WParam > 900 && msg.WParam < 950) Clear(msg.WParam);
   if(msg.WParam > 800 && msg.WParam < 850) {
      char vid[3];
      m_Vid1Combo->GetText(vid, sizeof vid);
      if(vid[1] == ' ') vid[1] = 0;
      char key[32];
      char buff[128];
      sprintf(key, "States_%s_%d", vid, msg.WParam - 800);
      GetPrivateProfileString(FORMSECTION, key, "", buff, sizeof buff, ININame);
      m_State->SetText(buff);
   }* /
}

void Private3Dlg::FindSN()
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
   else MessageBox(GetApplication()->MainWindow->HWindow, "Полис с таким номером не существует в базе.", "Сообщаю", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
}

void Private3Dlg::FindName()
{
}

void Private3Dlg::Print1()
{
/*   ((TRulerWin*)Parent)->SetValues(str);
   Dlg_base::Print1();
   delete str;* /
}

void Private3Dlg::SetDates()
{
    if(!CanModifyPrivate3)
       return;

    char datebuf[15];
    char duration[15];
    m_DateFrom->GetText(datebuf, sizeof datebuf);
    m_Srok->GetText(duration, sizeof duration);
    DATE startdate = GD(datebuf, 1);
    if(startdate) {
       DATE enddate = startdate + atoi(duration);
       int d, m, y;
       PXDateDecode(--enddate, &m, &d, &y);
       sprintf(datebuf, "%02u.%02u.%04u", d, m, y);
       m_DateTo->SetText(datebuf);
    }


}

void Private3Dlg::ChangeSrok(RTMessage msg)
{
    if(msg.LP.Hi == EN_CHANGE)            {
        SetDates();
	if(CanModifyPrivate3)
	    PostMessage(HWindow, 1, 0, 0);
    }
}

void Private3Dlg::ChangeStartDate(RTMessage msg)
{
    if(msg.LP.Hi == EN_CHANGE)
	SetDates();

/*    if(CanModifyPrivate3 && msg.LP.Hi == EN_CHANGE) {
	char buff[32];
	m_FromDate->GetText(buff, sizeof buff);
	m_WriteDate->SetText(buff);
    }* /
}

void Private3Dlg::Calculate(RTMessage msg)
{
   SetDates();

   char key[32], str[64];
   m_Summa->SetText("");
   m_Fee->SetText("");

   m_Srok->GetText(str, sizeof str);
   int Duration = atoi(str);

   for(int i = 0;; i++) {
       sprintf(key, "Tarif%d", i);
       if(!GetPrivateProfileString(FORMSECTION, key, "", str, sizeof str, ININame))
	   return;


       char* token = strtok(str, "-");
       if(!token) return;

       int MinDays = atoi(str);
       char* token2 = strtok(0, ",");
       if(!token2) return;
       int MaxDays = atoi(token2);

       sprintf(str, "%d-%d", MinDays, MaxDays);
       //MessageBox(0, token+1, token2+1, 0);
       //MessageBox(0, str, str, 0);

       if(Duration >= MinDays && Duration <= MaxDays) {
	    //MessageBeep(0);
	    char* token3 = strtok(0, ",");
            if(!token3) return;
	    double Summa = atof(token3);
	    char* token4 = strtok(0, ",");
            if(!token4) return;
	    double Fee = atof(token4);

	    sprintf(str, "%g", Summa);
            m_Summa->SetText(str);
	    sprintf(str, "%g", Fee);
            m_Fee->SetText(str);

	    return;
       }
   }
}

/////////////////////////////////////

void  Private3Dlg::SetInsurer2(RTMessage)
{
   char buffer[128];
   m_FIO->GetText(buffer, sizeof buffer);
   m_Insurer->SetText(buffer);
}

void  Private3Dlg::Clear()
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
   char buff[128];
   wsprintf(buff, "%02u.%02u.%04u", d.da_day, d.da_mon, d.da_year);
   m_DateFrom->SetText(buff);
   m_PayDate->SetText(buff);
   m_RegDate->SetText(buff);
   struct time t;
   gettime(&t);
   wsprintf(buff, "%02u:%02u", t.ti_hour, t.ti_min);
   m_RegTime->SetText(buff);

   GetPrivateProfileString(FORMSECTION, "Seria", "", buff, sizeof buff, ININame);
   m_Seria->SetText(buff);

   GetPrivateProfileString(FORMSECTION, "Company", "", buff, sizeof buff, ININame);
   m_InsComp->SetText(buff);

   m_PayCurr->SetText("BRB");
   m_NalBezNalCombo->SetSelIndex(0);

   if(GetPrivateProfileString(FORMSECTION, "DefAgent", "", buff, sizeof buff, ININame)) {
      SetAgent(buff, agents, m_AgentCombo, 0);
      TMessage msg;
      msg.LP.Hi = CBN_SELCHANGE;
      ChAgent(msg);
   }

   TMessage msg;
   Change(msg);
}

void  Private3Dlg::Help(RTMessage msg)
{
   if(msg.WParam == 1000)
      WinHelp(GetApplication()->MainWindow->HWindow, "blank.hlp", HELP_CONTEXT, HELP_PRIVATE);

   Dlg_base::Help(msg);
}

void Private3Dlg::WMCtlColor(RTMessage msg)
{
   Dlg_base::WMCtlColor(msg);
   if(msg.LP.Hi == CTLCOLOR_STATIC && (HWND)msg.LP.Lo == GetDlgItem(HWindow, 103)) 
	SetTextColor((HDC)msg.WParam, RGB(255, 0, 0));
   if(msg.LP.Hi == CTLCOLOR_STATIC && (HWND)msg.LP.Lo == GetDlgItem(HWindow, 1002))
	SetTextColor((HDC)msg.WParam, RGB(0, 0, 255));
}

void Private3Dlg::Change(RTMessage msg)
{
   CanModifyPrivate3 = 1;
   EnableWindow(GetDlgItem(HWindow, 406), TRUE);
//   Calculate(msg);
}


void Private3Dlg::WMMove(RTMessage msg)
{
     AlignCombo(m_AgentCombo);
     AlignCombo(m_NalBezNalCombo);
}

void Private3Dlg::ChangeNumber(RTMessage msg)
{
     if(msg.LP.Hi == EN_CHANGE) {
	 if(!CanModifyPrivate3) 
	    return;

	 m_Insurer->SetText("");
     }
}

void Private3Dlg::ChAgent(RTMessage msg)
{
   int IsUr;
   if(msg.LP.Hi == CBN_SELCHANGE)
	   ((TEdit*)ChildWithId(54))->SetText(GetAgPercent(SendMessage(m_AgentCombo->HWindow, CB_GETITEMDATA, m_AgentCombo->GetSelIndex(), 0), FOREIGN_TBL, IsUr));
}
*/