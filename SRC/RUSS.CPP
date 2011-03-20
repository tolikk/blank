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
#include <russ.h>
#include "help.h"
#include "math.h"
#include "date.h"
#include "impexp.h"
#include "mand_av2.h"
#include "dateedit.h"

extern char ActivateParadox;
void FillPlacement(TComboBox* PlacementCombo, const char* Key, const char* Sect, int);
void SetAgent(char* ACode, TOpenBase& agents, PTComboBox AgName, int);
char* NumberToWords(double Value);

char* RussianFields[] = {
"Seria",    //0
"Number",   //1
"PSer",     //2
"PNmb",     //3
"RegDt",    //4
"InsStart", //5
"TmStart",  //6
"Period",   //7
"InsEnd",   //8
"Insurer",  //9
"InsurerT", //10
"Owner",    //11
"OwnerT",   //12
"Marka",    //13
"AutoId",   //14
"PassSer",  //15
"PassNmb",  //16
"AutoNmb",  //17
"Tarif",    //18
"Pay1",     //19
"Curr1",    //20
"Pay2",     //21
"Curr2",    //22
"PayDate",  //23
"StopDt",   //24
"State",    //25
"Agent",    //26
"AgPcnt",   //27
"SpecSr",    //28
"SpecNmb",    //29
"InsComp",  //30
"RetSum",   //31
"RetCurr",  //32
"TarifGrp", //33
"Charact",  //34
"RepDt",    //35
"AgType",   //36
"Resident",    //37
};

char* RussianName = "\\bases\\MANDRUSS";
char* RusIniName = 0;
#define CNT_RUSS_FLDS (sizeof(RussianFields) / sizeof(RussianFields[0]))

//DATE

int RUSSModifyTarif = 1;
double GetRateCurrency(DATE payDate, char* CurrencyField);

class ChangeChecker
{
    int* Val;

    public:
    ChangeChecker(int* V);
	~ChangeChecker();
};

Russian::Russian(PTWindowsObject p)
	: Dlg_base(p, ActivateParadox ? "Russ" : "RUSSCALC", RussianName, RussianFields, CNT_RUSS_FLDS),
	  agents(AgentDBName, AgentFields, AgFieldsCount)
{
	char strBuffer[48];
	if(!GetPrivateProfileString("RUSSIAN", "INI", "", strBuffer, sizeof strBuffer, ININame))
		RusIniName = strdup(ININame);
	else
		RusIniName = strdup(strBuffer);

	(m_Seria = new TComboBox(this, 100, 5))->DisableTransfer();
	m_Number = new NumberEdit(this, 101, 11, 0);
	m_prSeria = new TEdit(this, 102, 5);
	m_prNumber = new NumberEdit(this, 103, 11, 0);

	m_RegDate = new TDateEdit(this, 115, 11);

	m_dtFrom = new TDateEdit(this, 104, 11);
	m_tmFrom = new TTimeEdit(this, 108, 6);
	m_dtTo = new TDateEdit(this, 105, 11);

	m_Insurer = new TEdit(this, 122, 49);
	m_InsurerType = new UrFizich(this, 124, 10);
	m_Owner = new TEdit(this, 109, 49);
	m_OwnerType = new UrFizich(this, 125, 10);

	//m_Marka = new TEdit(this, 128, 25);
	DynStr buff;
	GetPrivateProfileString("RUSSIAN", "AutoMarkFile", RusIniName, buff, buff._sizeof(), RusIniName);
	//((INIEdit*)m_Marka)->SetININame(buff);
	m_Marka = new INIEdit(buff, "MANDATORY", this, 128, 25, "AutoType%d", "AutoType%d_%d");


	m_IdNumber = new TEdit(this, 107, 18);
	m_PassSer = new TEdit(this, 131, 5);
	m_PassNmb = new NumberEdit(this, 132, 7, 0);
	m_AutoNmb = new TEdit(this, 106, 13);
	m_Charact = new NumberEdit(this, 111, 4, 0);

	m_SpecSer = new TEdit(this, 116, 4);
	m_SpecNmb = new NumberEdit(this, 136, 11, 0);

	m_Tarif = new NumberEdit(this, 126, 10, 1);

	m_Sum1 = new NumberEdit(this, 112, 10, 1);
	m_Sum2 = new NumberEdit(this, 113, 10, 1 );
	m_PayDate = new TDateEdit(this, 114, 11);
	m_RepDate = new TDateEdit(this, 135, 11);
	m_Percent = new NumberEdit(this, 117, 6, 1);
	m_AgType = new UrFizich(this, 110, 10);

	m_StopDate = new TDateEdit(this, 130, 11);
    m_RetSum = new NumberEdit(this, 133, 10, 1);

	m_Resident = new TCheckBox(this, 137, 0);

	(m_Period = new TComboBox(this, 200, 1))->DisableTransfer();
	(m_Val1 = new TComboBox(this, 203, 1))->DisableTransfer();
	(m_Val2 = new TComboBox(this, 204, 1))->DisableTransfer();
	(m_RetVal = new TComboBox(this, 134, 1))->DisableTransfer();
	(m_Agent = new TComboBox(this, 205, 1))->DisableTransfer();
	(m_State = new TComboBox(this, 206, 1))->DisableTransfer();
	(m_Company = new TComboBox(this, 207, 1))->DisableTransfer();
	(m_AutoType = new TComboBox(this, 201, 1))->DisableTransfer();

}

void InitVal(TComboBox* cb);
void InitVal2(TComboBox* cb);

void _FillAgents(TOpenBase* agents, TComboBox* AgName, int index);
void AlignCombo(TComboBox*);

void Russian::SetupWindow()
{
   Dlg_base::SetupWindow();

   m_State->AddString("НОРМАЛЬНЫЙ");
   m_State->AddString("УТЕРЯН");
   m_State->AddString("ИСПОРЧЕН");
   m_State->AddString("РАСТОРГНУТ");

   InitVal(m_Val1);
   InitVal2(m_Val2);
   InitVal2(m_RetVal);
   m_Val1->DeleteString(m_Val1->FindExactString("DM", -1));
   m_Val2->DeleteString(m_Val2->FindExactString("DM", -1));
   m_RetVal->DeleteString(m_RetVal->FindExactString("DM", -1));

   _FillAgents(&agents, m_Agent, RUSSIAN_TBL);
   char* OldIniName = ININame;
   ININame = RusIniName;
   FillPlacement(m_Period, "Period%d", "Russian", 0);
   FillPlacement(m_Company, "Company%d", "Russian", 0);
   FillPlacement(m_Seria, "Seria%d", "Russian", 0);
   ININame = OldIniName;

   char strBuffer[64];
   for(int i = 0;; i++) {
	   wsprintf(strBuffer, "Vid%d", i);
	   if(!GetPrivateProfileString("RUSSIAN", strBuffer, "", strBuffer, sizeof strBuffer, RusIniName))
		   break;
	   char* ch = strchr(strBuffer, ',');
       if(ch) *ch = 0;
	   m_AutoType->AddString(strBuffer);
   }

   AlignCombo(m_Period);
   AlignCombo(m_Company);
   AlignCombo(m_Agent);
   AlignCombo(m_AutoType);
   AlignCombo(m_Seria);

   if(ActivateParadox && base->First())
	   GetDataFromBase();
   else {
     date d;
   	getdate(&d);
   	char buff[100];
   	wsprintf(buff, "%02u.%02u.%04u", d.da_day, d.da_mon, d.da_year);
   m_dtFrom->SetText(buff);
   }
}

void DblToStr(char* str, double V, int Digit = 2);
extern DATE GD(char* str, int = 0);
void DateFBToStr(TOpenBase* base, int field, char* buffer);

void Russian::GetDataFromBase()
{  
   *OldCurr = 0;

   double N;
   char strBuffer[64];

   ChangeChecker obj(&RUSSModifyTarif);

   int PolisState;

	(*base)(N, 25);
	SetState(PolisState = N);

	(*base)(buffer.Seria_, sizeof buffer.Seria_, 0);
	m_Seria->SetText(buffer.Seria_);
	(*base)(N, 1);
	DblToStr(buffer.Number, N, 0);
	(*base)(buffer.prSeria, sizeof buffer.prSeria, 2);
	(*base)(N, 3);
	DblToStr(buffer.prNumber, N, 0);
	DateFBToStr(base, 5, buffer.dtFrom);
	(*base)(buffer.tmFrom, sizeof buffer.tmFrom, 6);
	DateFBToStr(base, 8, buffer.dtTo);
	DateFBToStr(base, 35, buffer.RepDate);

	m_Period->SetSelIndex(-1);
	short Period;
	(*base)(Period, 7);
	for(int i = 0; i < m_Period->GetCount(); i++) {
		char sVal[10];
		wsprintf(sVal, "%d ", Period);
		m_Period->GetString(strBuffer, i);
		if(strstr(strBuffer, sVal) == strBuffer)
			m_Period->SetSelIndex(i);
		else {
			wsprintf(sVal, "%d,", Period);
			if(strstr(strBuffer, sVal) == strBuffer)
				m_Period->SetSelIndex(i);
		}
	}
	if(m_Period->GetCount() && m_Period->GetSelIndex() == -1 && PolisState != RUSS_BAD)
	    MessageBox(HWindow, "Не найден период", "Ошибка", MB_OK | MB_ICONSTOP);

	m_AutoType->SetSelIndex(-1);
	short Autotype;
	(*base)(Autotype, 33);
	m_AutoType->SetSelIndex(Autotype);
	short Charact;
	(*base)(Charact, 34);
	DblToStr(buffer.Charact, Charact+1e-5, 0);

	(*base)(buffer.Insurer, sizeof buffer.Insurer, 9);
	(*base)(buffer.Owner, sizeof buffer.Owner, 11);
	(*base)(buffer.InsurerType, sizeof buffer.InsurerType, 10);
	(*base)(buffer.OwnerType, sizeof buffer.OwnerType, 12);
	if(*buffer.InsurerType == 'F') strcpy(buffer.InsurerType, "Физ.лицо");
	if(*buffer.InsurerType == 'U') strcpy(buffer.InsurerType, "Юр.лицо");
	if(*buffer.OwnerType == 'F') strcpy(buffer.OwnerType, "Физ.лицо");
	if(*buffer.OwnerType == 'U') strcpy(buffer.OwnerType, "Юр.лицо");
	(*base)(buffer.AgType, sizeof buffer.AgType, 36);
	if(*buffer.AgType == 'U') strcpy(buffer.AgType, "Юр.лицо");
	if(*buffer.AgType == 'F') strcpy(buffer.AgType, "Физ.лицо");

	(*base)(buffer.Marka, sizeof buffer.Marka, 13);
	(*base)(buffer.IdNumber, sizeof buffer.IdNumber, 14);
	(*base)(buffer.PassSer, sizeof buffer.PassSer, 15);
	(*base)(N, 16);
	DblToStr(buffer.PassNmb, N, 0);

	(*base)(buffer.SpecSer, sizeof buffer.SpecSer, 28);
	(*base)(N, 29);
	DblToStr(buffer.SpecNumber, N, 0);

	(*base)(buffer.AutoNmb, sizeof buffer.AutoNmb, 17);
	(*base)(N, 18);
	DblToStr(buffer.Tarif, N, 2);

	(*base)(N, 31);
	DblToStr(buffer.RetSum, N, 2);

	(*base)(strBuffer, sizeof strBuffer, 32);
	m_RetVal->SetSelIndex(-1);
	m_RetVal->SetText(strBuffer);

	m_Company->SetSelIndex(-1);
    short S;
	(*base)(S, 30);
	m_Company->SetSelIndex(S);

	if(m_Company->GetCount() && m_Company->GetSelIndex() == -1)
		MessageBox(HWindow, "Компания не определена", "Ошибка", MB_OK | MB_ICONSTOP);

	(*base)(N, 19);
	DblToStr(buffer.Sum1, N, 2);
	m_Val1->SetSelIndex(-1);
	(*base)(strBuffer, sizeof strBuffer, 20);
	m_Val1->SetText(strBuffer);

	(*base)(N, 21);
	DblToStr(buffer.Sum2, N, 2);
	m_Val2->SetSelIndex(-1);
	(*base)(strBuffer, sizeof strBuffer, 22);
	m_Val2->SetText(strBuffer);
	if(!*strBuffer) m_Val2->SetSelIndex(0);

	DateFBToStr(base, 23, buffer.PayDate);
	DateFBToStr(base, 4, buffer.Regdate);

    m_Agent->SetSelIndex(-1);
    char AGENT[10];
    (*base)(AGENT, sizeof AGENT, 26);
    SetAgent(AGENT, agents, m_Agent, 0);

	(*base)(N, 27);
	DblToStr(buffer.Percent, N, 2);

	DateFBToStr(base, 24, buffer.StopDate);

	if(PolisState == RUSS_BAD)
        *buffer.RepDate = 0;
	else {
		if(!strcmp(buffer.RepDate, buffer.PayDate))
			*buffer.RepDate = 0;
	}

	(*base)(AGENT, sizeof AGENT, 37);
	buffer.Resident = (*AGENT == 'Y');

	Dlg_base::GetDataFromBase();
}

void Russian::SetButtons()
{
   Dlg_base::SetButtons();
}

void Russian::SetState(int State)
{
   m_State->SetSelIndex(State);
}

/*
int GetBadTarif(HWND HWindow, char* Curr, char* sBadTarif)
{
   char strBuffer[32];
   char Key[32];
   sprintf(Key, "%sBadTarif", Curr);
	  if(!GetPrivateProfileString("Russian", Key, "", strBuffer, sizeof strBuffer, ININame)) {
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
*/
void Russian::ChRegDate(RTMessage msg)
{
	if(msg.LP.Hi == EN_CHANGE && RUSSModifyTarif) {
//		CalcTarif();
		InitTime();
    }
}
/*
char* Russian::GetCurrency(char* TTN)
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
*/

int _IsValidPolisNumber(const char* SECTION, _str s, long n, char* INI, char* Agent);

int Russian::SetDataToBase()
{
   Dlg_base::SetDataToBase();

   int Index = m_Agent->GetSelIndex();
   if(Index == -1) {
       SetFocus(m_Agent->HWindow);
       return 0;
   }

   char AgCode_[10];
   long _AgValue = SendMessage(m_Agent->HWindow, CB_GETITEMDATA, Index, 9);
   memcpy(AgCode_, &_AgValue, 4);
   AgCode_[4] = 0;
   char strBuffer[64];
   int IsBad = 0;
   if(!strcmp(buffer.Owner, "ИСПОРЧЕН") || !*buffer.Owner) {
      if(MessageBox(HWindow, GetRCStr(506), "Отвечай поскорей", MB_YESNO | MB_ICONINFORMATION | MB_SYSTEMMODAL) == IDNO)
	  	return 0;

      m_Tarif->SetText("");

	  SetState(RUSS_BAD);
	  m_Insurer->SetText("ИСПОРЧЕН");
	  strcpy(buffer.Insurer, "ИСПОРЧЕН");
	  (*base)(9, buffer.Insurer);
	  (*base)(33, short(-1));
	  IsBad = 1;
   }

   m_Seria->GetText(buffer.Seria_, sizeof buffer.Seria_);
   (*base)(0, buffer.Seria_);
   (*base)(1, atof(buffer.Number));
   if(m_State->GetSelIndex() == -1 || (!IsBad && m_State->GetSelIndex() == RUSS_BAD))
	   SetState(RUSS_NORMAL);

   if(!_IsValidPolisNumber("RUSSIAN", base->GetString(0), atol(buffer.Number), RusIniName, AgCode_)) {
		SetFocus(m_Number->HWindow);
     	return 0;
   }

   DATE PayDate = 0;
   if(!IsBad) {
	   (*base)(9, buffer.Insurer);
	   (*base)(11, buffer.Owner);
	   (*base)(18, atof(buffer.Tarif));

	   if(atof(buffer.Sum1) < 0.01 && !*buffer.prNumber) {
			SetFocus(m_Sum1->HWindow);
        	return 0;
	   }
       (*base)(19, atof(buffer.Sum1)); //SUMM1
       m_Val1->GetText(strBuffer, sizeof strBuffer);
	   if(strlen(strBuffer) == 0 && !*buffer.prNumber) {
		  SetFocus(m_Sum1->HWindow);
          return 0;
       }
       char Curr1[5];
       strcpy(Curr1, strBuffer);
       (*base)(20, strBuffer); //VAL CURR 1

	   m_Val2->GetText(strBuffer, sizeof strBuffer);
	   (*base)(21, atof(buffer.Sum2)); //SUMM2
       (*base)(22, strBuffer); //VAL CURR 2

       if(*strBuffer && atof(buffer.Sum2) < 0.01 || !*strBuffer && atof(buffer.Sum2) >= 0.01) {
	      SetFocus(m_Sum2->HWindow);
		  return 0;
       }

	  if(!strcmp(strBuffer, Curr1) && !*buffer.prSeria) {
		  SetFocus(m_Val1->HWindow);
		  return 0;
      }

	  DATE date = GD(buffer.PayDate);
      if(!date) {
         SetFocus(m_PayDate->HWindow);
         return 0;
	  }
	  PayDate = date;
	  (*base)(23, date);

	  if(strlen(buffer.Insurer) < 5) {
          SetFocus(m_Insurer->HWindow);
          return 0;
      }
	  if(strlen(buffer.Owner) < 5) {
		  SetFocus(m_Owner->HWindow);
          return 0;
	  }
	  (*base)(9, buffer.Insurer);
	  (*base)(10, *buffer.InsurerType == 'Ф' ? "F" : "U");
	  if(!*buffer.InsurerType) {
		  SetFocus(m_InsurerType->HWindow);
          return 0;
	  }
	  (*base)(11, buffer.Owner);
	  (*base)(12, *buffer.OwnerType == 'Ф' ? "F" : "U");
	  if(!*buffer.OwnerType) {
		  SetFocus(m_OwnerType->HWindow);
          return 0;
	  }
	  if(strlen(buffer.Marka) < 3) {
		  SetFocus(m_Marka->HWindow);
          return 0;
      }
	  (*base)(13, buffer.Marka);

	  if(strlen(buffer.IdNumber) < 5) {
		  SetFocus(m_IdNumber->HWindow);
          return 0;
      }
	  (*base)(14, buffer.IdNumber);

	  if(strlen(buffer.PassSer) < 2) {
		  SetFocus(m_PassSer->HWindow);
          return 0;
      }
	  (*base)(15, buffer.PassSer);

	  if(strlen(buffer.PassNmb) < 5) {
		  SetFocus(m_PassNmb->HWindow);
          return 0;
      }
	  (*base)(16, atof(buffer.PassNmb));

	  //if(strlen(buffer.SpecSer) < 1) {
	  //	  SetFocus(m_SpecSer->HWindow);
      //    return 0;
      //}
	  (*base)(28, buffer.SpecSer);

	  //if(strlen(buffer.SpecNumber) < 5) {
	  //	  SetFocus(m_SpecNmb->HWindow);
      //    return 0;
	  //}
	  (*base)(29, atof(buffer.SpecNumber));

	  if(strlen(buffer.AutoNmb) < 5) {
		  SetFocus(m_AutoNmb->HWindow);
          return 0;
      }
	  (*base)(17, buffer.AutoNmb);
	}

	DATE regdate;
    DATE date = regdate = GD(buffer.Regdate);
    if(!date) {
        SetFocus(m_RegDate->HWindow);
        return 0;
    }
	(*base)(4, date);

	DATE repdate = *buffer.RepDate ? GD(buffer.RepDate) : -1;
	if(!repdate) {
        SetFocus(m_RepDate->HWindow);
        return 0;
	}
	if(IsBad/* && repdate == -1*/)
		repdate = regdate;
	if(!IsBad && repdate == -1)
		repdate = PayDate;

	(*base)(35, repdate);

	if(fabs(repdate - regdate) > 30) {
		SetFocus(m_RepDate->HWindow);
        return 0;
	}

   (*base)(26, AgCode_);

   if(!IsBad) {
	   (*base)(36, *buffer.AgType == 'Ф' ? "F" : "U");
	   if(!*buffer.AgType) {
		  SetFocus(m_AgType->HWindow);
		  return 0;
	   }
   }

   (*base)(25, (short)m_State->GetSelIndex());
   (*base)(30, (short)m_Company->GetSelIndex());
   if(m_Company->GetSelIndex() == -1) {
	   SetFocus(m_Company->HWindow);
       return 0;
   }

   int IsDup = 0;

   if(!IsBad) {
      if(*buffer.prSeria || atol(buffer.prNumber) > 0) {
	   char msgStr[128];
	   sprintf(msgStr, "Вы уверены что полис %s/%s потерялся?", buffer.prSeria, buffer.prNumber);
	   if(MessageBox(HWindow, msgStr, "Отвечай поскорей", MB_YESNO | MB_ICONINFORMATION | MB_SYSTEMMODAL) == IDNO)
	       return 0;

	   char* ShortFields[] = { "Seria", "Number", "State" };
	   TOpenBase I(RussianName, ShortFields, 3);
	   if(I.pxErr) {
	      Err:MessageBox(HWindow, GetRCStr(552), "Ошибка", MB_OK | MB_ICONSTOP);
	      return 0;
	   }
	   I(0, buffer.prSeria);
	   I(1, atof(buffer.prNumber));


	   if(PXSrchKey(I.getTblHandle(), I.getRecHandle(), 2, SEARCHFIRST) == PXSUCCESS) {
	      I.Get();
          short State;
	      I(State, 2);
		  if(State == RUSS_BAD) {
		     MessageBox(HWindow, GetRCStr(563), "Ошибка", MB_OK | MB_ICONSTOP);
             return 0;
	     }
		 I(2, (short)RUSS_LOST);
	     if(PXRecUpdate(I.getTblHandle(), I.getRecHandle()) != PXSUCCESS)
		     goto Err;

		 IsDup = 1;

		 //Обнуляем суммы
		 (*base)(19, 0.);
		 (*base)(20, "");
		 (*base)(21, 0.);
		 (*base)(22, "");
		 (*base)(18, 0.);
		 m_Sum1->SetText("0");
		 m_Sum2->SetText("0");
		 m_Tarif->SetText("0");
	   }
	   else {
		 MessageBox(HWindow, GetRCStr(551), "Ошибка", MB_OK | MB_ICONSTOP);
	     return 0;
	   }
	  }
	  else {
		 if(atof(buffer.Tarif) < 0.01) {
			 SetFocus(m_Tarif->HWindow);
			 return 0;
		 }
	  }

     (*base)(2, buffer.prSeria);
     (*base)(3, atof(buffer.prNumber));

      date = GD(buffer.dtFrom);
      if(!date) {
	     SetFocus(m_dtFrom->HWindow);
         return 0;
      }
	  (*base)(5, date);
      DATE dateFrom = date;

	  if(strlen(buffer.tmFrom) < 3 || !strchr(buffer.tmFrom, ':')) {
		 SetFocus(m_tmFrom->HWindow);
         return 0;
	  }
	  (*base)(6, buffer.tmFrom);

      date = GD(buffer.dtTo);
      if(!date) {
         SetFocus(m_dtTo->HWindow);
         return 0;
	  }
	  (*base)(8, date);
	  DATE dateTo = date;

	  if(dateFrom >= dateTo || (dateTo - dateFrom) > 366) {
		 SetFocus(m_dtTo->HWindow);
         return 0;
	  }

	  if(dateFrom < PayDate) {
		 SetFocus(m_PayDate->HWindow);
         return 0;
	  }

      if(m_Period->GetSelIndex() == -1) {
	      SetFocus(m_Period->HWindow);
          return 0;
      }
      m_Period->GetText(strBuffer, sizeof strBuffer);
	  (*base)(7, (short)atoi(strBuffer));

	  if(IsDup) {
	      (*base)(27, 0.);
          m_Percent->SetText("0");
      }
      else
	     (*base)(27, atof(buffer.Percent));

      if(atof(buffer.Percent) < 0) {
	      SetFocus(m_Percent->HWindow);
          return 0;
	  }
	  (*base)(33, (short)m_AutoType->GetSelIndex());
	  (*base)(34, (short)atoi(buffer.Charact));
	  (*base)(37, buffer.Resident ? "Y" : "N");

	  DATE stopdate = GD(buffer.StopDate, 1);
	  if(*buffer.StopDate && !stopdate) {
		  SetFocus(m_StopDate->HWindow);
          return 0;
	  }
	  if (stopdate) {
		  if(MessageBox(HWindow, "Полис расторгнут?", "?", MB_YESNO | MB_ICONQUESTION) == IDNO) return 0;
		  (*base)(24, stopdate);
		  (*base)(25, (short)3);
		  (*base)(31, atof(buffer.RetSum));
		  m_RetVal->GetText(strBuffer, sizeof strBuffer);
		  (*base)(32, strBuffer);
		  SetState(RUSS_STOP);
	  }
      else
	  if(m_State->GetSelIndex() == RUSS_STOP) {
		  SetState(RUSS_NORMAL);
		  m_RetVal->SetSelIndex(-1);
		  m_RetSum->SetText("");
	  }
   }

   return 1;
}

Russian::~Russian()
{
	delete RusIniName;
}

int auxFindLike(const char* TableName, char* FldName, char* Maska, Array& Strings, TDialog* Dlg, char* ResStr, int IsDigit = 0, char** CheckFld = 0);

void Russian::FindByField(int fldNumber, TEdit* Ctrl)
{
	Array Strings(10, 0, 10);
    char str[70];
    char maska[20];
	Ctrl->GetText(maska, sizeof maska);
	int Result = auxFindLike(RussianName/*"\\Mandruss"*/, RussianFields[fldNumber], maska, Strings, this, str);
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

/*void Russian::ChDNumber(RTMessage msg)
{
    if(RUSSModifyTarif && msg.LP.Hi == EN_CHANGE) 
	CalcTarif();
}
  */
void Russian::FindSN(RTMessage)
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

void Russian::FindAutoNumber(RTMessage)
{
	FindByField(17, m_AutoNmb);
	/*
    Array Strings(10, 0, 10);
    char str[70];
    char maska[20];
    m_AutoNmb->GetText(maska, sizeof maska);
	int Result = auxFindLike("\\Mandruss", "autonmb", maska, Strings, this, str);
    if(Result != 1) return;
    (*base)(0, strtok(str, "/"));
    (*base)(1, atof(strtok(0, " ")));
    if(!PXSrchKey(base->getTblHandle(), base->getRecHandle(), 2, SEARCHFIRST)) {
       base->Get();
       GetDataFromBase();
    }
    else
	   MessageBox(HWindow, "Ошибка", "Ошибка поиска", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
       */
}

void Russian::FindPrSN(RTMessage)
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

void Russian::FindName(RTMessage)
{
	FindByField(11, m_Owner);
}

void Russian::FindIns(RTMessage)
{
	FindByField(9, m_Insurer);
}

void Spell(char*& dest, char* ch)
{
     while(*ch) {
		 *dest++ = *ch++;
		 *dest++ = 0;
     }
}

void Russian::Print(RTMessage)
{
   if(!SetDataToBase()) return;

   char SpecNumber[16];
   char SpecSer[7];
   char AutoId[20];

   m_SpecSer->GetText(SpecSer, sizeof SpecSer);
   m_SpecNmb->GetText(SpecNumber, sizeof SpecNumber);
   m_IdNumber->GetText(AutoId, sizeof AutoId);

   GetDataFromBase();

   m_SpecSer->SetText(SpecSer);
   m_SpecNmb->SetText(SpecNumber);
   m_IdNumber->SetText(AutoId);
   strcpy(buffer.SpecSer, SpecSer);
   strcpy(buffer.SpecNumber, SpecNumber);
   strcpy(buffer.IdNumber, AutoId);

   char* dest = new char [2500];
   char* strdata = dest;
   memset(dest, 0, 2400);

	 m_tmFrom->GetText(buffer.tmFrom, sizeof buffer.tmFrom);

	 Spell(dest, strtok(buffer.tmFrom, ":;./"));
	 Spell(dest, strtok(0, ":;./"));


     char* start = dest;
	 m_dtFrom->GetText(buffer.dtFrom, sizeof buffer.dtFrom);
	 Spell(dest, strtok(buffer.dtFrom, ":;./"));
	 Spell(dest, strtok(0, ":;./"));
	 Spell(dest, strtok(0, ":;./") + 2);

	 m_dtTo->GetText(buffer.dtTo, sizeof buffer.dtTo);
	 Spell(dest, strtok(buffer.dtTo, ":;./"));
	 Spell(dest, strtok(0, ":;./"));
	 Spell(dest, strtok(0, ":;./") + 2);

	 memmove(dest, start, dest - start);
	 dest += dest - start;


	 strcpy(dest, buffer.Insurer);
	 dest += strlen(dest) + 1;

	 strcpy(dest, buffer.Owner);
	 dest += strlen(dest) + 1;

	 strcpy(dest, buffer.Marka);
	 dest += strlen(dest) + 1;

	 for(int i = 0; i < 17; i++) {
		 if(strlen(buffer.IdNumber) > i)
			 *dest++ = buffer.IdNumber[i];
		 *dest++ = 0;
	 }

	 for(i = 0; i < 4; i++) {
		 if(strlen(buffer.PassSer) > i)
			 *dest++ = buffer.PassSer[i];
		 *dest++ = 0;
	 }

	 for(i = 0; i < 6; i++) {
		 if(strlen(buffer.PassNmb) > i)
			 *dest++ = buffer.PassNmb[i];
		 *dest++ = 0;
	 }

	 strcpy(dest, buffer.AutoNmb);
	 dest += strlen(dest) + 1;

	 for(i = 0; i < 3; i++) {
		 if(strlen(buffer.SpecSer) > i)
			 *dest++ = buffer.SpecSer[i];
		 *dest++ = 0;
	 }

	 for(i = 0; i < 10; i++) {
		 if(strlen(buffer.SpecNumber) > i)
			 *dest++ = buffer.SpecNumber[i];
		 *dest++ = 0;
	 }


   sprintf(dest, "%0.2f (%s) рублей", atof(buffer.Tarif), NumberToWords(atof(buffer.Tarif)));

   int Kop = long(atof(buffer.Tarif) * 100 + 0.0001) % 100;
   if(Kop) sprintf(dest + strlen(dest), " %02d копеек", Kop);

   dest += strlen(dest) + 1;

	 m_RegDate->GetText(buffer.Regdate, sizeof buffer.Regdate);
   strcat(dest, strtok(buffer.Regdate, ":;./"));
   dest += strlen(dest) + 1;
   strcat(dest, strtok(0, ":;./"));

   strcpy(dest, GetRCStr(119 + atoi(dest)));

   dest += strlen(dest) + 1;
   strcat(dest, strtok(0, ":;./") + 2);
   dest += strlen(dest) + 1;

   ((TRulerWin*)Parent)->SetValues(strdata);
   Dlg_base::Print1();
   delete strdata;
}

void Russian::Clear()
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

void Russian::Default()
{
   date d;
   getdate(&d);
   char buff[100];
   wsprintf(buff, "%02u.%02u.%04u", d.da_day, d.da_mon, d.da_year);
   m_dtFrom->SetText(buff);
   m_RegDate->SetText(buff);
   m_PayDate->SetText(buff);
/*   struct time t;
   gettime(&t);
   wsprintf(buff, "%02u:%02u", t.ti_hour, t.ti_min);
   m_RegTime->SetText(buff);
*/
   GetPrivateProfileString("Russian", "DefSeria", "", buff, sizeof buff, RusIniName);
   m_Seria->SetText(buff);
   m_SpecSer->SetText(buff);

   if(GetPrivateProfileString("Russian", "DefAgent", "", buff, sizeof buff, RusIniName)) {
	  //MessageBox(0, buff, buff, 0);
      SetAgent(buff, agents, m_Agent, 0);
      TMessage msg;
      msg.LP.Hi = CBN_SELCHANGE;
      ChAgent(msg);
   }

   m_Company->SetSelIndex(GetPrivateProfileInt("Russian", "DefCompany", 0, RusIniName));
   m_Period->SetSelIndex(GetPrivateProfileInt("Russian", "DefPeriod", 0, RusIniName));
   m_AutoType->SetSelIndex(GetPrivateProfileInt("Russian", "DefTarif", 0, RusIniName));

   CalcEndDate();
   CalcTarif();

   SetState(RUSS_NORMAL);
}

void Russian::Help(RTMessage msg)
{
   if(msg.WParam == 1000)
      WinHelp(GetApplication()->MainWindow->HWindow, "blank.hlp", HELP_CONTEXT, HELP_WARTA);

   Dlg_base::Help(msg);
}

void Russian::ChAgent(RTMessage msg)
{
   int IsUr;
   if(msg.LP.Hi == CBN_SELCHANGE) {
	   m_Percent->SetText(GetAgPercent(SendMessage(m_Agent->HWindow, CB_GETITEMDATA, m_Agent->GetSelIndex(), 0), RUSSIAN_TBL, IsUr));
	   m_AgType->SetText(IsUr ? "Юр." : "Физ.");

   }
}

void Russian::ChPeriod(RTMessage msg)
{
   if(msg.LP.Hi == CBN_SELCHANGE && RUSSModifyTarif) {
       CalcEndDate();
       CalcTarif();
   }
}

void Russian::ChAutoType(RTMessage msg)
{
   if(msg.LP.Hi == CBN_SELCHANGE && RUSSModifyTarif) {
	   CalcTarif();
   }
}

void Russian::ChStartDate(RTMessage msg)
{
   if(msg.LP.Hi == EN_CHANGE && RUSSModifyTarif) {
       CalcEndDate();
	   InitTime();
   }
}

DATE aux_incDate(DATE startdate, int inc1, int value);
int getDurationType(char* str, char**);

void Russian::CalcEndDate()
{
    char datebuf[15], PeriodName[32];
    m_dtFrom->GetText(datebuf, sizeof datebuf);
    m_Period->GetText(PeriodName, sizeof PeriodName);
    if(!*PeriodName) return;
    DATE startdate = GD(datebuf, 1);

	if(startdate) {
	   char* sptr = PeriodName;
	   int dtype = getDurationType(PeriodName, &sptr);
       DATE enddate = aux_incDate(startdate, dtype, atoi(sptr));
       int d, m, y;
       PXDateDecode(--enddate, &m, &d, &y);
       sprintf(datebuf, "%02u.%02u.%04u", d, m, y);
       m_dtTo->SetText(datebuf);
    }
}

void Russian::CalcTarif()
{
	 if(!RUSSModifyTarif) return;

	 char tarifStr[128];
	 char Key[32];

	 int IndexAutoType = m_AutoType->GetSelIndex();
	 int IndexPeriod = m_Period->GetSelIndex();
	 m_Charact->GetText(tarifStr, sizeof tarifStr);
	 int Charact = atoi(tarifStr);
	 //m_InsurerType->GetText(tarifStr, sizeof tarifStr);
	 m_OwnerType->GetText(tarifStr, sizeof tarifStr);
	 char PeopleType = *tarifStr == 'Ю' ? 'U' : 'F';

	 if(IndexAutoType == -1 || IndexPeriod == -1) return;
	 sprintf(Key, "Vid%d", IndexAutoType);
	 GetPrivateProfileString("Russian", Key, "", tarifStr, sizeof tarifStr, RusIniName);

	 char* ch = strchr(tarifStr, ',');
	 if(ch) ch++;
	 int CharactGroup = 0;

	 //Поиск классификации по параметру
	 if(ch) {
		  for(;;) {
			  if(Charact <= atoi(ch)) {
				  CharactGroup = atoi(ch);
				  break;
			  }
              ch = strchr(ch, ',');
			  if(!ch) break;
  	          if(ch) ch++;
		  }
     }

	 sprintf(tarifStr, "Tarif%s_%c_%d_%d", m_Resident->GetCheck() ? "R" : "", PeopleType, IndexAutoType, CharactGroup);
	 GetPrivateProfileString("Russian", tarifStr, "", tarifStr, sizeof tarifStr, RusIniName);

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

     m_Tarif->SetText(OneTarifStr);
}

void Russian::SetVal(RTMessage)
{
     char buff[26];
     m_Tarif->GetText(buff, sizeof buff);
     m_Sum1->SetText(buff);
	 m_Val1->SetText("RUR");
	 strcpy(OldCurr, "RUR");

     m_Sum2->SetText("");
     m_Val2->SetSelIndex(0);
}

void Russian::CopyInsurer(RTMessage)
{
	 char strBuffer[64];
	 m_Insurer->GetText(strBuffer, sizeof strBuffer);
	 m_Owner->SetText(strBuffer);
	 m_InsurerType->GetText(strBuffer, sizeof strBuffer);
	 m_OwnerType->SetText(strBuffer);
}

void Russian::ChInsurerType(RTMessage msg)
{
   if(msg.LP.Hi == EN_CHANGE && RUSSModifyTarif) {
	   CalcTarif();
   }
}

void Russian::ChCharact(RTMessage msg)
{
   if(msg.LP.Hi == EN_CHANGE && RUSSModifyTarif) {
	   CalcTarif();
   }
}
/*
void Russian::ChRegDate(RTMessage msg)
{
	if(msg.LP.Hi == EN_CHANGE && RUSSModifyTarif) {
        InitTime();
    }
}


void Russian::ChStartDate(RTMessage msg)
{
	if(msg.LP.Hi == EN_CHANGE && RUSSModifyTarif) {
		InitTime();
    }
}
*/
void Russian::InitTime()
{
	char s1[16], s2[16];
    m_RegDate->GetText(s1, sizeof s1);
	m_dtFrom->GetText(s2, sizeof s2);

	DATE regdt = GD(s1, 1);
	DATE startdt = GD(s2, 1);

	if(regdt == startdt) {
       struct time t;
       gettime(&t);
	   wsprintf(s1, "%02u:%02u", t.ti_hour, t.ti_min);
	   m_tmFrom->SetText(s1);
   }
	else
		m_tmFrom->SetText("00:00");
}

void Russian::ChResident(RTMessage msg)
{
	if(msg.LP.Hi == BN_CLICKED) 
	   CalcTarif();
}

static int ImpFunc(TOpenBase* FileDb, TOpenBase* Db, void* Data)
{
	if(FileDb->GetDouble(18) != Db->GetDouble(18))
	{
		DynStr msg;
		sprintf(msg, "%s/%5.0f\nБыло %s %5.0f\nСтало %s %5.0f\n$%s", Db->GetString(0).s, Db->GetDouble(1),
		Db->GetString(9).s, Db->GetDouble(18), FileDb->GetString(9).s, FileDb->GetDouble(18), GetRCStr(637));
		if(MessageBox(GetApplicationObject()->MainWindow->HWindow, msg, "!", MB_ICONQUESTION | MB_YESNO) != IDYES)
		{
			*strchr(msg, '$') = 0;
			((Array*)Data)->add(*new String(msg));
			return 0;
        }

    }
	return 1;
}

void Russian::Import()
{
	Array list(100);
	list.ownsElements(TRUE);
	char list1[] = {25, 2, 3, 24, 0};
	char list2[] = {25, 2, 3, 24, 26, 5, 6, 7, 8, 18, 19, 20, 21, 22, 23, 31, 32, 35, 0};

	long Result = ImportData(RussianName, RussianFields, CNT_RUSS_FLDS, ImpFunc, &list, 26, 25, RUSS_BAD, list1, list2);
	if(Result) {
		MessageBox(GetApplicationObject()->MainWindow->HWindow, GetRCStr(700+Result), "!", MB_ICONEXCLAMATION);
	}
	ShowArray(list);
}

void Russian::Export(PTWindowsObject Parent)
{
	Priod_Params* data = new Priod_Params;
	strcpy(data->filename, RussianName);
	if(GetApplicationObject()->ExecDialog(new getDate(Parent, &data->m, &data->y, &data->dm, data->filename)) == IDOK) {
		long Error = ExportData(RussianName, RussianFields, CNT_RUSS_FLDS, 0, 0, data->filename, data->m, data->y, data->dm, 26, 8);
		sprintf(data->filename, GetRCStr(Error > 0 ? 621: 622), labs(Error));
		//if(Error <= 0) unlink(data->filename);
		MessageBox(Parent->HWindow, data->filename, "", 0);
	}
	delete data;
}

void ReplaceCurrs(TEdit* m_PayDate, char* OldCurr, TEdit* m_Pay1, TComboBox* m_Curr1, int);
void ReplaceCurrs2(TEdit* m_PayDate, char* OldCurr, TEdit* m_Pay1, TComboBox* m_Curr1, int IsUr)
{
	char currency[5];
	m_Curr1->GetText(currency, sizeof currency);
	int round = !IsUr || strcmp("BRB", currency)==0;
	ReplaceCurrs(m_PayDate, OldCurr, m_Pay1, m_Curr1, !round);
}

void Russian::ChPayCurr(RTMessage msg)
{
	if(msg.LP.Hi == CBN_SELCHANGE && *OldCurr) {
		m_InsurerType->GetText(buffer.InsurerType, sizeof buffer.InsurerType);
		ReplaceCurrs2(m_PayDate, OldCurr, m_Sum1, m_Val1, *buffer.InsurerType == 'Ю');
    }
}

void Russian::ChNormal(RTMessage msg)
{
   SetState(RUSS_NORMAL);
}

