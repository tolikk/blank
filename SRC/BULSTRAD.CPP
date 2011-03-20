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
#include <bulstrad.h>
#include "help.h"

/*

void FillPlacement(TComboBox* PlacementCombo, char* Key, char* Sect, int);
void SetAgent(char* ACode, TOpenBase& agents, PTComboBox AgName, int);

char* BulstradFields[] = {
"Seria", //0
"Number", //1
"PSeria", //2
"PNumber", //3
"DtFrom",  //4
"DtTO",    //5
"Period",  //6
"Katg",    //7
"AutoNmb",  //8
"ShassiNmb", //9
"YearMade",  //10
"Terr",     //11
"Owner",    //12
"Address",    //13
"Tarif",     //14
"TarifC", // 15
"Pay1",     // 16
"Pay1C",      //17
"Pay2",        //18
"Pay2C",    // 19
"PayDate",  // 20
"RegDate",  // 21
"RegTime",  // 22
"AgCode",   // 23
"Pcnt",     // 24
"State",     // 25
"DS", //26
"DN", //27
"DTRF", //28
"PTRF",  //29
"Marka", //30
"Model", //31
"Country"   //32
};

char* BulstradName = "\\bases\\bulstrad";
#define CNT_BULSTRAD_FLDS (sizeof(BulstradFields) / sizeof(BulstradFields[0]))

//DATE

int BULModifyTarif = 1;

class ChangeChecker
{
    int* Val;

    public:
    ChangeChecker(int* V) {
	Val = V;
        *Val = 0;
    }
    ~ChangeChecker() {
	*Val = 1;
    }
};

Bulstrad::Bulstrad(PTWindowsObject p)
	: Dlg_base(p, "BULSTRAD", BulstradName, BulstradFields, CNT_BULSTRAD_FLDS),
	  agents(AgentDBName, AgentFields, AgFieldsCount)
{
	m_Seria = new TEdit(this, 100, 5);
	m_Number = new NumberEdit(this, 101, 11, 0);
	m_prSeria = new TEdit(this, 102, 5);
	m_prNumber = new NumberEdit(this, 103, 11, 0);

	m_dSeria = new TEdit(this, 122, 5);
	m_dNumber = new NumberEdit(this, 123, 11, 0);
	m_dTarif = new NumberEdit(this, 124, 4, 0);
	m_pTarif = new NumberEdit(this, 126, 5, 0);

	m_dtFrom = new TEdit(this, 104, 11);
	m_dtTo = new TEdit(this, 105, 11);
	m_autoNmb = new TEdit(this, 106, 15);
	m_shassiNmb = new TEdit(this, 107, 15);
	m_Marka = new TEdit(this, 128, 21);
	m_Model = new TEdit(this, 129, 21);
	m_YearMade = new NumberEdit(this, 108, 5, 0);
	m_Owner = new TEdit(this, 109, 48);
	m_Address = new TEdit(this, 110, 48);
	m_Tarif = new NumberEdit(this, 111, 10, 1);
	m_Sum1 = new NumberEdit(this, 112, 10, 1);
	m_Sum2 = new NumberEdit(this, 113, 10, 1 );
	m_PayDate = new TEdit(this, 114, 11);
	m_RegDate = new TEdit(this, 115, 11);
	m_RegTime = new TEdit(this, 116, 6);
	m_Percent = new NumberEdit(this, 117, 6, 1);
	m_CountryGet = new TEdit(this, 130, 15);
	(m_Period = new TComboBox(this, 200, 1))->DisableTransfer();
	(m_Kategor = new TComboBox(this, 201, 1))->DisableTransfer();
	(m_Country = new TComboBox(this, 202, 1))->DisableTransfer();
	(m_Val1 = new TComboBox(this, 203, 1))->DisableTransfer();
	(m_Val2 = new TComboBox(this, 204, 1))->DisableTransfer();
	(m_Agent = new TComboBox(this, 205, 1))->DisableTransfer();
	(m_State = new TComboBox(this, 206, 1))->DisableTransfer();
}
*/
void InitVal(TComboBox* cb);


void InitVal2(TComboBox* cb)
{
   cb->AddString("");
   InitVal(cb);
}

/*
void _FillAgents(TOpenBase* agents, TComboBox* AgName, int index);
void AlignCombo(TComboBox*);

void Bulstrad::SetupWindow()
{
   Dlg_base::SetupWindow();

   m_State->AddString("НОРМАЛЬНЫЙ");
   m_State->AddString("УТЕРЯН");
   m_State->AddString("ИСПОРЧЕН");

   InitVal(m_Val1);
   InitVal2(m_Val2);

   _FillAgents(&agents, m_Agent, BULSTRAD_TBL);
   FillPlacement(m_Period, "Period%d", "BULSTRAD", 0);
   FillPlacement(m_Kategor, "AutoType%d", "BULSTRAD", 0);
   FillPlacement(m_Country, "Country%d", "BULSTRAD", 0);

   AlignCombo(m_Period);
   AlignCombo(m_Kategor);
   AlignCombo(m_Country);
   AlignCombo(m_Agent);

   GetDataFromBase();
}

void DblToStr(char* str, double V, int Digit = 2);
extern DATE GD(char* str, int = 0);
void DateFBToStr(TOpenBase* base, int field, char* buffer);

void Bulstrad::GetDataFromBase()
{  
   double N;
   char strBuffer[64];

   ChangeChecker obj(&BULModifyTarif);

   int PolisState;

	(*base)(N, 25);
	SetState(PolisState = N);

	(*base)(buffer.Seria, sizeof buffer.Seria, 0);
	(*base)(N, 1);
	DblToStr(buffer.Number, N, 0);
	(*base)(buffer.prSeria, sizeof buffer.prSeria, 2);
	(*base)(N, 3);
	DblToStr(buffer.prNumber, N, 0);
	DateFBToStr(base, 4, buffer.dtFrom);
	DateFBToStr(base, 5, buffer.dtTo);


	(*base)(buffer.dSeria, sizeof buffer.dSeria, 26);
	(*base)(N, 27);
	DblToStr(buffer.dNumber, N, 2);
	(*base)(N, 28);
	DblToStr(buffer.dTarif, N, 2);
	(*base)(N, 29);
	DblToStr(buffer.pTarif, N, 2);


	m_Period->SetSelIndex(-1);
	(*base)(strBuffer, sizeof strBuffer, 6);
	m_Period->SetText(strBuffer);
	if(m_Period->GetCount() && m_Period->GetSelIndex() == -1 && PolisState != BULSTRAD_BAD)
	    MessageBox(HWindow, "Не найден период", "Ошибка", MB_OK | MB_ICONSTOP);

	m_Kategor->SetSelIndex(-1);
	(*base)(N, 7);
	if(PolisState != BULSTRAD_BAD)
	    m_Kategor->SetSelIndex(N);

	(*base)(buffer.autoNmb, sizeof buffer.autoNmb, 8);
	(*base)(buffer.shassiNmb, sizeof buffer.shassiNmb, 9);
	(*base)(buffer.Marka, sizeof buffer.Marka, 30);
	(*base)(buffer.Model, sizeof buffer.Model, 31);
	(*base)(buffer.Country, sizeof buffer.Country, 32);
	if(!*buffer.Country)
	    if(PolisState != BULSTRAD_BAD)
		strcpy(buffer.Country, "BEL");
	(*base)(N, 10);
	DblToStr(buffer.YearMade, N, 0);

	m_Country->SetSelIndex(-1);
	(*base)(N, 11);
	if(PolisState != BULSTRAD_BAD)
	    m_Country->SetSelIndex(N);
	if(m_Country->GetCount() && m_Country->GetSelIndex() == -1 && PolisState != BULSTRAD_BAD)
	    MessageBox(HWindow, "Не найдена страна", "Ошибка", MB_OK | MB_ICONSTOP);

	(*base)(buffer.Owner, sizeof buffer.Owner, 12);
	(*base)(buffer.Address, sizeof buffer.Address, 13);

	(*base)(N, 14);
	DblToStr(buffer.Tarif, N, 2);

	(*base)(strBuffer, sizeof strBuffer, 15);
        SetDlgItemText(HWindow, 333, strBuffer);

	(*base)(N, 16);
	DblToStr(buffer.Sum1, N, 2);
	m_Val1->SetSelIndex(-1);
	(*base)(strBuffer, sizeof strBuffer, 17);
	m_Val1->SetText(strBuffer);

	(*base)(N, 18);
	DblToStr(buffer.Sum2, N, 2);
	m_Val2->SetSelIndex(-1);
	(*base)(strBuffer, sizeof strBuffer, 19);
	m_Val2->SetText(strBuffer);
	if(!*strBuffer) m_Val2->SetSelIndex(0);

	DateFBToStr(base, 20, buffer.PayDate);
	DateFBToStr(base, 21, buffer.Regdate);

	(*base)(buffer.RegTime, sizeof buffer.RegTime, 22);

        m_Agent->SetSelIndex(-1);
        char AGENT[10];
        (*base)(AGENT, sizeof AGENT, 23);
        SetAgent(AGENT, agents, m_Agent, 0);

	(*base)(N, 24);
	DblToStr(buffer.Percent, N, 2);
	//MessageBox(0, buffer.Percent, buffer.Percent, 0);

     Dlg_base::GetDataFromBase();
}

void Bulstrad::SetButtons()
{
   Dlg_base::SetButtons();
}

void Bulstrad::SetState(int State)
{
   m_State->SetSelIndex(State);
   //buffer.State = State;
}

char* GetRCStr(int strID);

int GetBadTarif(HWND HWindow, char* Curr, char* sBadTarif)
{
   char strBuffer[32];
   char Key[32];
   sprintf(Key, "%sBadTarif", Curr);
      if(!GetPrivateProfileString("BULSTRAD", Key, "", strBuffer, sizeof strBuffer, ININame)) {
	  badTraifLbl:MessageBox(HWindow, "Тариф не найден или не правильный", strBuffer, MB_OK | MB_ICONSTOP);
	  return 0;
      }

      char* TransStart = strBuffer;
      while((*TransStart == ' ' || *TransStart == 8) && *TransStart) TransStart++;
      if(!*TransStart) goto badTraifLbl;
      char* bad;
      double BadTarif = strtod(TransStart, &bad);
      if(!*bad) goto badTraifLbl;

      if(strlen(Curr) == 0 && !strcmp(bad, "DM")) goto badTraifLbl;
      if(strcmp(Curr, "EUR") == 0 && !strcmp(bad, "EUR")) goto badTraifLbl;

      DblToStr(sBadTarif, BadTarif);

      return 1;
}

void Bulstrad::ChRegDate(RTMessage msg)
{
    if(msg.LP.Hi == EN_CHANGE)
	CalcTarif();
}

char* Bulstrad::GetCurrency(char* TTN)
{
    static char Curr[5];

    if(TTN) *TTN = 0;
    DATE divDate, div2Date;
    PXDateEncode(1, 1 , 2002, &divDate);
    PXDateEncode(8, 1 , 2002, &div2Date);

    char sDate[32];
    m_RegDate->GetText(sDate, sizeof sDate);
    DATE rDate = GD(sDate, 1);
    if (!rDate)
	strcpy(Curr, "?");
    else if(rDate >= div2Date) {
	strcpy(Curr, "EUR");
	if(TTN) strcpy(TTN, "EUR2");
    }
    else if(rDate >= divDate) {
	strcpy(Curr, "EUR");
	if(TTN) strcpy(TTN, "EUR");
    }
    else strcpy(Curr, "");

    return Curr;
}

int Bulstrad::SetDataToBase()
{
   Dlg_base::SetDataToBase();

   char strBuffer[64];
   int IsBad = 0;
   if(!strcmp(buffer.Owner, "ИСПОРЧЕН") || !*buffer.Owner) {
      if(MessageBox(HWindow, GetRCStr(506), "Отвечай поскорей", MB_YESNO | MB_ICONINFORMATION | MB_SYSTEMMODAL) == IDNO)
	  return 0;

      if(!GetBadTarif(HWindow, GetCurrency(0), buffer.Tarif))
          return 0;

      SetDlgItemText(HWindow, 333, !*GetCurrency(0) ? "DM" : GetCurrency(0));

      m_Tarif->SetText(buffer.Tarif);
      m_pTarif->SetText(buffer.Tarif);

      SetState(BULSTRAD_BAD);
      m_Owner->SetText("ИСПОРЧЕН");
      strcpy(buffer.Owner, "ИСПОРЧЕН");
      IsBad = 1;
   }

   (*base)(0, buffer.Seria);
   (*base)(1, atof(buffer.Number));
   if(m_State->GetSelIndex() == -1)
       SetState(BULSTRAD_NORMAL);

   (*base)(12, buffer.Owner);

    (*base)(14, atof(buffer.Tarif));


    GetDlgItemText(HWindow, 333, strBuffer, sizeof strBuffer);
    if (*strBuffer == 0) {
	SetFocus(m_Sum1->HWindow);
        return 0;
    }
    (*base)(15, strBuffer);


    if(atof(buffer.Sum1) < 0.01) {
	SetFocus(m_Sum1->HWindow);
        return 0;
    }
      (*base)(16, atof(buffer.Sum1)); //SUMM1
      m_Val1->GetText(strBuffer, sizeof strBuffer);
      if(strlen(strBuffer) == 0) {
	  SetFocus(m_Sum1->HWindow);
          return 0;
      }
      char Curr1[5];
      strcpy(Curr1, strBuffer);
      (*base)(17, strBuffer); //VAL CURR 1

      m_Val2->GetText(strBuffer, sizeof strBuffer);
      (*base)(18, atof(buffer.Sum2)); //SUMM2
      (*base)(19, strBuffer); //VAL CURR 2

      if(*strBuffer && atof(buffer.Sum2) < 0.01 ||
	 !*strBuffer && atof(buffer.Sum2) >= 0.01) {
	  SetFocus(m_Sum2->HWindow);
          return 0;
      }

      if(!strcmp(strBuffer, Curr1)) {
	  return 0;
      }

    DATE regdate;
    DATE date = regdate = GD(buffer.Regdate);
      if(!date) {
         SetFocus(m_RegDate->HWindow);
         return 0;
      }
      (*base)(21, date);
      (*base)(22, buffer.RegTime);

      date = GD(buffer.PayDate);
      if(!date) {
         SetFocus(m_PayDate->HWindow);
         return 0;
      }
      (*base)(20, date);



   int Index = m_Agent->GetSelIndex();
   if(Index == -1) {
       SetFocus(m_Agent->HWindow);
       return 0;
   }
   char AgCode_[10];
   wsprintf(AgCode_, "%04d", SendMessage(m_Agent->HWindow, CB_GETITEMDATA, Index, 9));
   (*base)(23, AgCode_);

   (*base)(25, (double)m_State->GetSelIndex());

   int IsDup = 0;

   if(!IsBad) {
      if(*buffer.prSeria || atol(buffer.prNumber) > 0) {
	char msgStr[128];
	sprintf(msgStr, "Вы уверены что полис %s/%s потерялся?", buffer.prSeria, buffer.prNumber);
	if(MessageBox(HWindow, msgStr, "Отвечай поскорей", MB_YESNO | MB_ICONINFORMATION | MB_SYSTEMMODAL) == IDNO)
	    return 0;

	char* ShortFields[] = { "Seria", "Number", "State" };
	TOpenBase I(BulstradName, ShortFields, 3);
	 if(I.pxErr) {
	    Err:MessageBox(HWindow, GetRCStr(552), "Ошибка", MB_OK | MB_ICONSTOP);
	    return 0;
	 }
	 I(0, buffer.prSeria);
	 I(1, atof(buffer.prNumber));
	 if(PXSrchKey(I.getTblHandle(), I.getRecHandle(), 2, SEARCHFIRST) == PXSUCCESS) {
	     I.Get();

	     DATE _1_8_2002;
	     PXDateEncode(8, 1, 2002,& _1_8_2002);
	     if(regdate >= _1_8_2002) {
		 strcpy(buffer.Tarif, "3");
	     }
             else {
	        //Для дубликата тариф = BAD
	        if(!GetBadTarif(HWindow, GetCurrency(0), buffer.Tarif))
		   return 0;
	     }
	     strcpy(buffer.pTarif, buffer.Tarif);
	     m_Tarif->SetText(buffer.Tarif);
	     m_pTarif->SetText(buffer.Tarif);
	     (*base)(14, atof(buffer.Tarif));
	     (*base)(29, atof(buffer.pTarif));

	     double N;
	     I(N, 2);
	     if(N == BULSTRAD_BAD) {
		 MessageBox(HWindow, GetRCStr(563), "Ошибка", MB_OK | MB_ICONSTOP);
                 return 0;
	     }
	     I(2, (double)BULSTRAD_LOST);
	     if(PXRecUpdate(I.getTblHandle(), I.getRecHandle()) != PXSUCCESS)
		goto Err;

	     IsDup = 1;
	 }
	 else {
	    MessageBox(HWindow, GetRCStr(551), "Ошибка", MB_OK | MB_ICONSTOP);
	    return 0;
	 }

      }

      if(GetWindowTextLength(m_dNumber->HWindow) > 0) {
         (*base)(26, buffer.dSeria);
         (*base)(27, atof(buffer.dNumber));
         (*base)(28, atof(buffer.dTarif));
      }
      (*base)(29, atof(buffer.pTarif));
      (*base)(2, buffer.prSeria);
      (*base)(3, atof(buffer.prNumber));

      date = GD(buffer.dtFrom);
      if(!date) {
	 SetFocus(m_dtFrom->HWindow);
         return 0;
      }
      (*base)(4, date);
      date = GD(buffer.dtTo);
      if(!date) {
         SetFocus(m_dtTo->HWindow);
         return 0;
      }
      (*base)(5, date);

      if(m_Period->GetSelIndex() == -1) {
	  SetFocus(m_Period->HWindow);
          return 0;
      }
      m_Period->GetText(strBuffer, sizeof strBuffer);
      (*base)(6, strBuffer);
      if(m_Kategor->GetSelIndex() == -1) {
	  SetFocus(m_Kategor->HWindow);
          return 0;
      }
      (*base)(7, double(m_Kategor->GetSelIndex()));

      (*base)(8, buffer.autoNmb);
      (*base)(9, buffer.shassiNmb);
      if(!*buffer.Marka) {
	  SetFocus(m_Marka->HWindow);
          return 0;
      }
      (*base)(30, buffer.Marka);
      if(!*buffer.Model) {
	  SetFocus(m_Model->HWindow);
          return 0;
      }
      (*base)(31, buffer.Model);
      if(!*buffer.Country) {
	  SetFocus(m_CountryGet->HWindow);
          return 0;
      }
      (*base)(32, buffer.Country);
      (*base)(10, atof(buffer.YearMade));

      if(m_Country->GetSelIndex() == -1) {
	  SetFocus(m_Country->HWindow);
          return 0;
      }
      (*base)(11, double(m_Country->GetSelIndex()));

      // seep upper (*base)(12, buffer.Owner);
      (*base)(13, buffer.Address);

      if(atof(buffer.Tarif) < 0.01) {
	  SetFocus(m_Tarif->HWindow);
          return 0;
      }
      if(IsDup) {
	  (*base)(24, 0.);
          m_Percent->SetText("0");
      }
      else
	  (*base)(24, atof(buffer.Percent));

      if(atof(buffer.Percent) < 0) {
	  SetFocus(m_Percent->HWindow);
          return 0;
      }
   }

   return 1;
}

Bulstrad::~Bulstrad()
{
}

void Bulstrad::ChDNumber(RTMessage msg)
{
    if(BULModifyTarif && msg.LP.Hi == EN_CHANGE) 
	CalcTarif();
}

void Bulstrad::FindSN()
{
    char buff[32];
    m_Seria->GetText(buff, sizeof buff);
    (*base)(0, buff);
    m_Number->GetText(buff, sizeof buff);
    (*base)(1, atof(buff));
    if(!PXSrchKey(base->getTblHandle(), base->getRecHandle(), 2, SEARCHFIRST))
    {
       base->Get();
       GetDataFromBase();
    }
    else
       MessageBox(HWindow, GetRCStr(556), "Ошибка поиска", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
}

//int auxFindLike(const char* TableName, char* FldName, char* Maska, Array& Strings, TDialog* Dlg, char* str);
int auxFindLike(const char* TableName, char* FldName, char* Maska, Array& Strings, TDialog* Dlg, char* ResStr, int IsDigit = 0, char** CheckFld = 0);

void Bulstrad::FindAutoNumber(RTMessage msg)
{
    Array Strings(10, 0, 10);
    char str[70];
    char maska[20];
    m_autoNmb->GetText(maska, sizeof maska);
    int Result = auxFindLike("\\bulstrad", "autonmb", maska, Strings, this, str);
    if(Result != 1) return;
    (*base)(0, strtok(str, "/"));
    (*base)(1, atof(strtok(0, " ")));
    if(!PXSrchKey(base->getTblHandle(), base->getRecHandle(), 2, SEARCHFIRST)) {
       base->Get();
       GetDataFromBase();
    }
    else
       MessageBox(HWindow, "Ошибка", "Ошибка поиска", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
}

void Bulstrad::FindPrSN()
{
    char buff[32];
    m_prSeria->GetText(buff, sizeof buff);
    (*base)(0, buff);
    m_prNumber->GetText(buff, sizeof buff);
    (*base)(1, atof(buff));
    if(!PXSrchKey(base->getTblHandle(), base->getRecHandle(), 2, SEARCHFIRST))
    {
       base->Get();
       GetDataFromBase();
    }
    else
       MessageBox(HWindow, GetRCStr(556), "Ошибка поиска", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
}

void Bulstrad::FindName()
{
}

void Bulstrad::Print1()
{
}

void Bulstrad::Clear()
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

   Default();
}

void Bulstrad::Default()
{
   date d;
   getdate(&d);
   char buff[100];
   wsprintf(buff, "%02u.%02u.%04u", d.da_day, d.da_mon, d.da_year);
   m_dtFrom->SetText(buff);
   m_RegDate->SetText(buff);
   m_PayDate->SetText(buff);
   struct time t;
   gettime(&t);
   wsprintf(buff, "%02u:%02u", t.ti_hour, t.ti_min);
   m_RegTime->SetText(buff);

   GetPrivateProfileString("BULSTRAD", "DefSeria", "", buff, sizeof buff, ININame);
   m_Seria->SetText(buff);

   if(GetPrivateProfileString("BULSTRAD", "DefAgent", "", buff, sizeof buff, ININame)) {
      SetAgent(buff, agents, m_Agent, 0);
      TMessage msg;
      msg.LP.Hi = CBN_SELCHANGE;
      ChAgent(msg);
   }

   m_Country->SetSelIndex(GetPrivateProfileInt("BULSTRAD", "DefCountry", 0, ININame));
   m_Period->SetSelIndex(GetPrivateProfileInt("BULSTRAD", "DefPeriod", 0, ININame));

   CalcEndDate();
   CalcTarif();

   SetState(BULSTRAD_NORMAL);
}


void Bulstrad::Help(RTMessage msg)
{
   if(msg.WParam == 1000)
      WinHelp(GetApplication()->MainWindow->HWindow, "blank.hlp", HELP_CONTEXT, HELP_WARTA);

   Dlg_base::Help(msg);
}

void Bulstrad::ChAgent(RTMessage msg)
{
   int IsUr;
   if(msg.LP.Hi == CBN_SELCHANGE)
       m_Percent->SetText(GetAgPercent(SendMessage(m_Agent->HWindow, CB_GETITEMDATA, m_Agent->GetSelIndex(), 0), BULSTRAD_TBL, IsUr));
}

void Bulstrad::ChPeriod(RTMessage msg)
{
   if(msg.LP.Hi == CBN_SELCHANGE && BULModifyTarif) {
       CalcEndDate();
       CalcTarif();
   }
}

void Bulstrad::ChAutoType(RTMessage msg)
{
   if(msg.LP.Hi == CBN_SELCHANGE) {
       CalcTarif();
   }
}

void Bulstrad::ChCountry(RTMessage msg)
{
   if(msg.LP.Hi == CBN_SELCHANGE) {
       CalcTarif();
   }
}

void Bulstrad::ChStartDate(RTMessage msg)
{
   if(msg.LP.Hi == EN_CHANGE && BULModifyTarif)
       CalcEndDate();
}

DATE aux_incDate(DATE startdate, int inc1, int value);
int getDurationType(char* str);

void Bulstrad::CalcEndDate()
{
    char datebuf[15], PeriodName[32];
    m_dtFrom->GetText(datebuf, sizeof datebuf);
    m_Period->GetText(PeriodName, sizeof PeriodName);
    if(!*PeriodName) return;
    DATE startdate = GD(datebuf, 1);

    if(startdate) {
       DATE enddate = aux_incDate(startdate, getDurationType(PeriodName), atoi(PeriodName));
       int d, m, y;
       PXDateDecode(--enddate, &m, &d, &y);
       sprintf(datebuf, "%02u.%02u.%04u", d, m, y);
       m_dtTo->SetText(datebuf);
    }
}

void Bulstrad::CalcTarif()
{
     if(!BULModifyTarif)
	 return;

     int IndexCountry = m_Country->GetSelIndex();
     int IndexAutoType = m_Kategor->GetSelIndex();
     int IndexPeriod = m_Period->GetSelIndex();

     if(IndexCountry == -1 || IndexAutoType == -1 || IndexPeriod == -1)
	 return;

     char tarifStr[128];
     char Key[32];
     GetCurrency(tarifStr);
     sprintf(Key, "%sTarif%s_%s", tarifStr, "%d", "%d");
     sprintf(tarifStr, Key, IndexCountry, IndexAutoType);
     GetPrivateProfileString("BULSTRAD", tarifStr, "", tarifStr, sizeof tarifStr, ININame);

     char* firstdivider = tarifStr;//strchr(tarifStr, ',');
     char* OneTarifStr;// = tarifStr;//strchr(tarifStr, ',');
     for(int i = 0; i <= IndexPeriod; i++) {
	 firstdivider = strchr(OneTarifStr = firstdivider, ',');
	 if(!firstdivider) break;
	 *firstdivider = 0;
	 firstdivider++;
     }

     if((IndexPeriod - i) > 1) {
	 m_Tarif->SetText("<не найден>");
	 return;
     }

     while(*OneTarifStr == ' ') OneTarifStr++;

     m_pTarif->SetText(OneTarifStr);

     char DopTarif[5];
     GetPrivateProfileString("BULSTRAD", "Tarif2", "1", DopTarif, sizeof DopTarif, ININame);
     if(GetWindowTextLength(m_dNumber->HWindow) > 0) {
         m_dTarif->SetText(DopTarif);
	 sprintf(OneTarifStr, "%g", atof(OneTarifStr) + atof(DopTarif));
	 GetPrivateProfileString("BULSTRAD", "Seria2", "", DopTarif, sizeof DopTarif, ININame);
	 m_dSeria->SetText(DopTarif);
     }
     else {
	 m_dTarif->SetText("");
	 m_dSeria->SetText("");
     }

     m_Tarif->SetText(OneTarifStr);
     SetDlgItemText(HWindow, 333, !*GetCurrency(0) ? "DM" : GetCurrency(0));
}

void Bulstrad::SetVal()
{
     char buff[26];
     m_Tarif->GetText(buff, sizeof buff);
     m_Sum1->SetText(buff);
     m_Val1->SetText(GetCurrency(0));

     m_Sum2->SetText("");
     m_Val2->SetSelIndex(0);
}
*/