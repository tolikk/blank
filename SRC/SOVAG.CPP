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
#include <sovag.h>
#include "help.h"

/*

void FillPlacement(TComboBox* PlacementCombo, char* Key, char* Sect, int);
void SetAgent(char* ACode, TOpenBase& agents, PTComboBox AgName, int);

char* SOVAGFields[] = {
"Seria",   //0
"Number",  //1
"PSeria",  //2
"PNumber", //3
"Place",   //4
"RegDate", //5
"RegTime", //6
"Owner",   //7
"Addr", //8
"DtFrom",  //9
"Period",  //10
"DtTO",    //11
"AutoType",//12
"Letter",  //13
"AutoNmb", //14
"Model",   //15
"Tarif",   //16
"Pay1",    //17
"Pay1C",   //18
"Pay2",    //19
"Pay2C",   //20
"PayDate", //21
"AgCode",  //22
"Pcnt",    //23
"State",   //24
"Seria2",  //25
"Number2", //26
"AutoNmb2",//27
"Model2",  //28
};

char* SOVAGName = "\\bases\\sovag";
#define CNT_SOVAG_FLDS (sizeof(SOVAGFields) / sizeof(SOVAGFields[0]))

//DATE

int SAVAModifyTarif = 1;

class ChangeChecker
{
    int* Val;

    public:
    ChangeChecker(int* V); 
    ~ChangeChecker(); 
};



SOVAG::SOVAG(PTWindowsObject p)
	: Dlg_base(p, "SOVAG", SOVAGName, SOVAGFields, CNT_SOVAG_FLDS),
	  agents(AgentDBName, AgentFields, AgFieldsCount)
{
	m_Seria = new TEdit(this, 100, 11);
	m_Number = new NumberEdit(this, 101, 11, 0);
	m_Seria2 = new TEdit(this, 126, 11);
	m_Number2 = new NumberEdit(this, 127, 11, 0);
	m_pSeria = new TEdit(this, 122, 11);
	m_pNumber = new NumberEdit(this, 123, 11, 0);
	m_Place = new TEdit(this, 102, 32);
	m_RegDate = new TEdit(this, 103, 11);
	m_Time = new TEdit(this, 104, 6);
	m_Insurer = new TEdit(this, 105, 48);
	m_Address = new TEdit(this, 106, 48);

	m_dtFrom = new TEdit(this, 107, 11);
	(m_Period = new TComboBox(this, 108, 1))->DisableTransfer();
	m_dtTo = new TEdit(this, 109, 11);

	(m_AutoType = new TComboBox(this, 110, 1))->DisableTransfer();
	m_Letter = new TEdit(this, 111, 4);

	m_autoNmb = new TEdit(this, 112, 24);
	m_Model = new TEdit(this, 113, 24);
	m_autoNmb2 = new TEdit(this, 128, 24);
	m_Model2 = new TEdit(this, 129, 24);

	m_Tarif = new NumberEdit(this, 114, 10, 1);
	m_PayDate = new TEdit(this, 125, 11);
	m_Pay1 = new NumberEdit(this, 116, 10, 1);
	(m_Curr1 = new TComboBox(this, 117, 1))->DisableTransfer();
	m_Pay2 = new NumberEdit(this, 118, 10, 1);
	(m_Curr2 = new TComboBox(this, 119, 1))->DisableTransfer();

	(m_Agent = new TComboBox(this, 120, 1))->DisableTransfer();
	m_AgPercent = new NumberEdit(this, 121, 5, 1);
	(m_State = new TComboBox(this, 124, 1))->DisableTransfer();
}

void InitVal(TComboBox* cb);
void InitVal2(TComboBox* cb);

void _FillAgents(TOpenBase* agents, TComboBox* AgName, int index);
void AlignCombo(TComboBox*);

void SOVAG::SetupWindow()
{
   Dlg_base::SetupWindow();

   m_State->AddString("НОРМАЛЬНЫЙ");
   m_State->AddString("УТЕРЯН");
   m_State->AddString("ИСПОРЧЕН");

   InitVal(m_Curr1);
   InitVal2(m_Curr2);
   m_Curr1->DeleteString(m_Curr1->FindExactString("DM", -1));
   m_Curr2->DeleteString(m_Curr2->FindExactString("DM", -1));

   _FillAgents(&agents, m_Agent, SOVAG_TBL);
   FillPlacement(m_Period, "Period%d", "SOVAG", 0);
   FillPlacement(m_AutoType, "AutoType%d", "SOVAG", 0);
   //FillPlacement(m_Country, "Country%d", "SOVAG", 0);

   AlignCombo(m_Period);
   AlignCombo(m_Curr1);
   AlignCombo(m_Curr2);
   AlignCombo(m_AutoType);
   AlignCombo(m_Agent);

   GetDataFromBase();
}

void DblToStr(char* str, double V, int Digit = 2);
extern DATE GD(char* str, int = 0);
void DateFBToStr(TOpenBase* base, int field, char* buffer);

void SOVAG::GetDataFromBase()
{  
   double N;
   char strBuffer[64];

   ChangeChecker obj(&SAVAModifyTarif);

   int PolisState;

	(*base)(N, 24);
	SetState(PolisState = N);

	(*base)(buffer.Seria, sizeof buffer.Seria, 0);
	(*base)(N, 1);
	DblToStr(buffer.Number, N, 0);
	(*base)(buffer.prSeria, sizeof buffer.prSeria, 2);
	(*base)(N, 3);
	DblToStr(buffer.prNumber, N, 0);

	(*base)(buffer.Seria2, sizeof buffer.Seria, 25);
	(*base)(N, 26);
	DblToStr(buffer.Number2, N, 0);


	(*base)(buffer.Place, sizeof buffer.Place, 4);
	DateFBToStr(base, 5, buffer.RegDate);
	(*base)(buffer.Time, sizeof buffer.Time, 6);

	(*base)(buffer.Insurer, sizeof buffer.Insurer, 7);
	(*base)(buffer.Address, sizeof buffer.Address, 8);

	DateFBToStr(base, 9, buffer.dtFrom);
	DateFBToStr(base, 11, buffer.dtTo);

	m_Period->SetSelIndex(-1);
	(*base)(strBuffer, sizeof strBuffer, 10);
	m_Period->SetText(strBuffer);
	if(m_Period->GetCount() && m_Period->GetSelIndex() == -1 && PolisState != SOVAG_BAD)
	    MessageBox(HWindow, "Не найден период", strBuffer, MB_OK | MB_ICONSTOP);

	m_AutoType->SetSelIndex(-1);
	(*base)(N, 12);
	if(PolisState != SOVAG_BAD)
  	    m_AutoType->SetSelIndex(N);

	(*base)(buffer.Letter, sizeof buffer.Letter, 13);

	(*base)(buffer.autoNmb, sizeof buffer.autoNmb, 14);
	(*base)(buffer.Model, sizeof buffer.Model, 15);
	(*base)(buffer.autoNmb2, sizeof buffer.autoNmb, 27);
	(*base)(buffer.Model2, sizeof buffer.Model, 28);

	(*base)(N, 16);
	DblToStr(buffer.Tarif, N, 0);

	(*base)(N, 17);
	DblToStr(buffer.Pay1, N, 2);
	m_Curr1->SetSelIndex(-1);
	(*base)(strBuffer, sizeof strBuffer, 18);
	m_Curr1->SetText(strBuffer);

	(*base)(N, 19);
	DblToStr(buffer.Pay2, N, 2);
	m_Curr2->SetSelIndex(-1);
	(*base)(strBuffer, sizeof strBuffer, 20);
	m_Curr2->SetText(strBuffer);
	if(!*strBuffer) m_Curr2->SetSelIndex(0);

	DateFBToStr(base, 21, buffer.PayDate);

        m_Agent->SetSelIndex(-1);
        char AGENT[10];
	(*base)(AGENT, sizeof AGENT, 22);
        SetAgent(AGENT, agents, m_Agent, 0);

	(*base)(N, 23);
	DblToStr(buffer.AgPercent, N, 2);
     InitDZK();
     Dlg_base::GetDataFromBase();
}

void SOVAG::SetButtons()
{
   Dlg_base::SetButtons();
}

void SOVAG::SetState(int State)
{
   m_State->SetSelIndex(State);
   //buffer.State = State;
}

char* GetRCStr(int strID);

int SOVAG::SetDataToBase()
{
   Dlg_base::SetDataToBase();

   char strBuffer[64];
   int IsBad = 0;

   if(!strcmp(buffer.Insurer, "ИСПОРЧЕН") || !*buffer.Insurer) {
      if(MessageBox(HWindow, GetRCStr(506), "Отвечай поскорей", MB_YESNO | MB_ICONINFORMATION | MB_SYSTEMMODAL) == IDNO)
	  return 0;

      //if(!GetBadTarif(HWindow, GetCurrency(), buffer.Tarif))
      //    return 0;

      //SetDlgItemText(HWindow, 333, !*GetCurrency() ? "DM" : GetCurrency());
      m_Tarif->SetText("");

      SetState(SOVAG_BAD);
      m_Insurer->SetText("ИСПОРЧЕН");
      strcpy(buffer.Insurer, "ИСПОРЧЕН");
      IsBad = 1;
   }

   (*base)(0, buffer.Seria);
   (*base)(1, atof(buffer.Number));
   if(m_State->GetSelIndex() == -1)
       SetState(SOVAG_NORMAL);

   (*base)(7, buffer.Insurer);

    if(!IsBad) {
     (*base)(16, atof(buffer.Tarif));
     if(atof(buffer.Pay1) < 0.01) {
	SetFocus(m_Pay1->HWindow);
        return 0;
      }
      (*base)(17, atof(buffer.Pay1)); //SUMM1
      m_Curr1->GetText(strBuffer, sizeof strBuffer);
      if(strlen(strBuffer) == 0) {
	  SetFocus(m_Pay1->HWindow);
          return 0;
      }
      char Curr1[5];
      strcpy(Curr1, strBuffer);
      (*base)(18, strBuffer); //VAL CURR 1

      m_Curr2->GetText(strBuffer, sizeof strBuffer);
      (*base)(19, atof(buffer.Pay2)); //SUMM2
      (*base)(20, strBuffer); //VAL CURR 2

      if(*strBuffer && atof(buffer.Pay2) < 0.01 ||
	 !*strBuffer && atof(buffer.Pay2) >= 0.01) {
	  SetFocus(m_Pay2->HWindow);
          return 0;
      }

      if(!strcmp(strBuffer, Curr1)) {
	  return 0;
      }

      DATE date = GD(buffer.PayDate);
      if(!date) {
         SetFocus(m_PayDate->HWindow);
         return 0;
      }
      (*base)(21, date);

    }

    DATE date = GD(buffer.RegDate);
      if(!date) {
         SetFocus(m_RegDate->HWindow);
         return 0;
      }
      (*base)(5, date);
      (*base)(6, buffer.Time);


   int Index = m_Agent->GetSelIndex();
   if(Index == -1) {
       SetFocus(m_Agent->HWindow);
       return 0;
   }
   char AgCode_[10];
   wsprintf(AgCode_, "%04d", SendMessage(m_Agent->HWindow, CB_GETITEMDATA, Index, 9));
   (*base)(22, AgCode_);

   (*base)(24, (double)m_State->GetSelIndex());

   int IsDup = 0;

   if(!IsBad) {
      if(*buffer.prSeria || atol(buffer.prNumber) > 0) {
	char msgStr[128];
	sprintf(msgStr, "Вы уверены что полис %s/%s потерялся?", buffer.prSeria, buffer.prNumber);
	if(MessageBox(HWindow, msgStr, "Отвечай поскорей", MB_YESNO | MB_ICONINFORMATION | MB_SYSTEMMODAL) == IDNO)
	    return 0;

	char* ShortFields[] = { "Seria", "Number", "State" };
	TOpenBase I(SOVAGName, ShortFields, 3);
	 if(I.pxErr) {
	    Err:MessageBox(HWindow, GetRCStr(552), "Ошибка", MB_OK | MB_ICONSTOP);
	    return 0;
	 }
	 I(0, buffer.prSeria);
	 I(1, atof(buffer.prNumber));
	 if(PXSrchKey(I.getTblHandle(), I.getRecHandle(), 2, SEARCHFIRST) == PXSUCCESS) {
	     I.Get();

	     //Для дубликата тариф = BAD
	     //if(!GetBadTarif(HWindow, GetCurrency(), buffer.Tarif))
	     //	return 0;
	     m_Tarif->SetText("");
	     (*base)(14, atof("0"));

	     double N;
	     I(N, 2);
	     if(N == SOVAG_BAD) {
		 MessageBox(HWindow, GetRCStr(563), "Ошибка", MB_OK | MB_ICONSTOP);
                 return 0;
             }
	     I(2, (double)SOVAG_LOST);
	     if(PXRecUpdate(I.getTblHandle(), I.getRecHandle()) != PXSUCCESS)
		goto Err;

	     IsDup = 1;
	 }
	 else {
	    MessageBox(HWindow, GetRCStr(551), "Ошибка", MB_OK | MB_ICONSTOP);
	    return 0;
	 }
      }

      (*base)(2, buffer.prSeria);
      (*base)(3, atof(buffer.prNumber));

      (*base)(25, buffer.Seria2);
      (*base)(26, atof(buffer.Number2));

      (*base)(4, buffer.Place);

      DATE date = GD(buffer.dtFrom);
      if(!date) {
         SetFocus(m_dtFrom->HWindow);
         return 0;
      }
      (*base)(9, date);
      date = GD(buffer.dtTo);
      if(!date) {
         SetFocus(m_dtTo->HWindow);
         return 0;
      }
      (*base)(11, date);

      if(m_Period->GetSelIndex() == -1) {
	  SetFocus(m_Period->HWindow);
          return 0;
      }
      m_Period->GetText(strBuffer, sizeof strBuffer);
      (*base)(10, strBuffer);

      if(m_AutoType->GetSelIndex() == -1) {
      	  SetFocus(m_AutoType->HWindow);
          return 0;
      }

      (*base)(12, (double)m_AutoType->GetSelIndex());

      (*base)(13, buffer.Letter);
      (*base)(14, buffer.autoNmb);
      (*base)(15, buffer.Model);
      (*base)(8, buffer.Address);
      (*base)(27, buffer.autoNmb2);
      (*base)(28, buffer.Model2);
      if(!*buffer.Letter || !*buffer.autoNmb || !*buffer.Model) {
	  SetFocus(m_Model->HWindow);
	  return 0;
      }
      if(IsWindowEnabled(m_Seria2->HWindow) && (!*buffer.autoNmb2 || !*buffer.Model2)) {
	  SetFocus(m_Model2->HWindow);
	  return 0;
      }

      if(atof(buffer.Tarif) < 0.01) {
	  SetFocus(m_Tarif->HWindow);
          return 0;
      }

      if(IsDup) {
	  (*base)(23, 0.);
          m_AgPercent->SetText("0");
      }
      else
	  (*base)(23, atof(buffer.AgPercent));

      if(atof(buffer.AgPercent) < 0) {
	  SetFocus(m_AgPercent->HWindow);
          return 0;
      }
   }

   return 1;
}

SOVAG::~SOVAG()
{
}

void SOVAG::auxFindSN(char* Seria, double Number)
{
    //char buff[32];
    //m_Seria->GetText(buff, sizeof buff);
    (*base)(0, Seria);
    //m_Number->GetText(buff, sizeof buff);
    (*base)(1, Number);
    if(!PXSrchKey(base->getTblHandle(), base->getRecHandle(), 2, SEARCHFIRST))
    {
       base->Get();
       GetDataFromBase();
    }
    else
       MessageBox(HWindow, GetRCStr(556), "Ошибка поиска", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
}

void SOVAG::FindSN()
{
    char buffs[16];
    char buffn[16];
    m_Seria->GetText(buffs, sizeof buffs);
    m_Number->GetText(buffn, sizeof buffn);
    auxFindSN(buffs, atof(buffn));
}

void SOVAG::FindPrSN()
{
    char buffs[16];
    char buffn[16];
    m_pSeria->GetText(buffs, sizeof buffs);
    m_pNumber->GetText(buffn, sizeof buffn);
    auxFindSN(buffs, atof(buffn));
}

void SOVAG::FindName()
{
}

void SOVAG::Clear()
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

void SOVAG::Default()
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
   m_Time->SetText(buff);

   TMessage msg;
   msg.LP.Hi = CBN_SELCHANGE;
   if(GetPrivateProfileString("SOVAG", "DefAgent", "", buff, sizeof buff, ININame)) {
      SetAgent(buff, agents, m_Agent, 0);
      ChAgent(msg);
   }

   m_Period->SetSelIndex(GetPrivateProfileInt("SOVAG", "DefPeriod", 0, ININame));
   m_AutoType->SetSelIndex(GetPrivateProfileInt("SOVAG", "DefAutoType", 0, ININame));
   ChAutoType(msg);
   GetPrivateProfileString("SOVAG", "DefPlace", "", buff, sizeof buff, ININame);
   m_Place->SetText(buff);

   CalcEndDate();
   CalcTarif();

   SetState(SOVAG_NORMAL);
}

void SOVAG::ChAgent(RTMessage msg)
{
   int IsUr;
   if(msg.LP.Hi == CBN_SELCHANGE)
       m_AgPercent->SetText(GetAgPercent(SendMessage(m_Agent->HWindow, CB_GETITEMDATA, m_Agent->GetSelIndex(), 0), SOVAG_TBL, IsUr));
}

void SOVAG::ChPeriod(RTMessage msg)
{
   if(msg.LP.Hi == CBN_SELCHANGE && SAVAModifyTarif) {
       CalcEndDate();
       CalcTarif();
   }
}

void SOVAG::ChAutoType(RTMessage msg)
{
   if(msg.LP.Hi == CBN_SELCHANGE) {
       CalcTarif();
       if(SAVAModifyTarif) {
          char s[48];
	  m_AutoType->GetText(s, sizeof s);
	  char* pos = strchr(s, ',');
	  if(pos) {
	      *pos = 0;
	      while(*--pos == ' ' && pos > s)
		  *pos = 0;
	      m_Letter->SetText(s);
	  }
	  char Key[10];
	  sprintf(Key, "SERIA%d", m_AutoType->GetSelIndex());
	  if(GetPrivateProfileString("SOVAG", Key, "", s, sizeof s, ININame))
	      m_Seria->SetText(s);

	  InitDZK();
       }
   }
}

void SOVAG::ChStartDate(RTMessage msg)
{
   if(msg.LP.Hi == EN_CHANGE && SAVAModifyTarif)
       CalcEndDate();
}

DATE aux_incDate(DATE startdate, int inc1, int value);
int getDurationType(char* str);

void SOVAG::CalcEndDate()
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
       
void SOVAG::CalcTarif()
{
     if(!SAVAModifyTarif)
	 return;

//     int IndexCountry = m_Country->GetSelIndex();
     int IndexAutoType = m_AutoType->GetSelIndex();
     int IndexPeriod = m_Period->GetSelIndex();

     if(IndexAutoType == -1 || IndexPeriod == -1)
	 return;

     char tarifStr[128];
     //char Key[32];
     //sprintf(Key, "Tarif%s_%s", "%d", "%d");
     sprintf(tarifStr, "Tarif%d", IndexAutoType);
     GetPrivateProfileString("SOVAG", tarifStr, "", tarifStr, sizeof tarifStr, ININame);

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

void SOVAG::SetVal()
{
     char buff[26];
     m_Tarif->GetText(buff, sizeof buff);
     m_Pay1->SetText(buff);
     m_Curr1->SetText("EUR");

     m_Pay2->SetText("");
     m_Curr2->SetSelIndex(0);
}

void SOVAG::Find()
{
   base->Refresh();
   HMENU Popup = CreatePopupMenu();
   AppendMenu(Popup, MF_ENABLED | MF_STRING, 720, "По Серии/Номеру");
   AppendMenu(Popup, MF_ENABLED | MF_STRING, 721, "Утерянный");
   //AppendMenu(Popup, MF_ENABLED | MF_STRING, 722, "По Серии/Номеру ДЗК");
   POINT ptCurrent;
   GetCursorPos(&ptCurrent);
   TrackPopupMenu(Popup, TPM_LEFTALIGN, ptCurrent.x, ptCurrent.y, 0, HWindow, NULL);
   DestroyMenu(Popup);
}

void SOVAG::WMCommand(RTMessage msg)
{
   if(msg.Message == WM_COMMAND) {
      if(msg.WParam == 720) FindSN();
      if(msg.WParam == 721) FindPrSN();
   }
   Dlg_base::WMCommand(msg);
}

void SOVAG::InitDZK()
{
   char s[16];
   m_Letter->GetText(s, sizeof s);
   int IsDZK = strchr(s, '+') != 0;
   EnableWindow(m_Seria2->HWindow, IsDZK);
   EnableWindow(m_Number2->HWindow, IsDZK);
   EnableWindow(m_autoNmb2->HWindow, IsDZK);
   EnableWindow(m_Model2->HWindow, IsDZK);
   if(IsDZK) {
       m_Seria->GetText(s, sizeof s);
       m_Seria2->SetText(s);
   }
   else {
       m_Seria2->SetText("");
       m_Number2->SetText("");
       m_autoNmb2->SetText("");
       m_Model2->SetText("");
   }
}
*/