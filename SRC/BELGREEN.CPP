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
#include <belgreen.h>
#include "help.h"
#include "dlg_base.h"
#include "impexp.h"
#include "date.h"
#include "dateedit.h"

#define AUX_TEXT 547

extern char ActivateParadox;
void ExpStr(_str s, int& IsRussian, int& IsEnglish, int& IsDigit, int& IsBad, int CheckMinus);
int _IsValidPolisNumber(const char*, _str s, long n, char*, char* Agent);
int auxFindLike(const char* TableName, char* FldName, char* Maska, Array& Strings, TDialog* Dlg, char* ResStr, int IsDigit, char** CheckFld);
long _PrevNumber = -1;
double GetRateCurrency(DATE payDate, char* CurrencyField);
const char* _NextText = "Следующий";

char* NumberToWords(double Value);

int IsRussian(char* Str, char* ExcludeChrSect = "Q")
{
    DynStr _buff;
    GetPrivateProfileString("BELGREEN", ExcludeChrSect, "", _buff,  _buff._sizeof(), ININame);

	for(char* ch = Str; *ch; ch++) {
		if(*ch >= 'а' &&  *ch <= 'я' || *ch >= 'А' && *ch <= 'Я')
		    if(!strchr(_buff, *ch))
            return 1;
	}
    return 0;
}

int IsEnglish(char* Str)
{
	for(char* ch = Str; *ch; ch++) {
		if(*ch >= 'a' &&  *ch <= 'z' || *ch >= 'A' && *ch <= 'Z')
            return 1;
	}
    return 0;
}

int IsBadSymbol(char* Str)
{
	for(char* ch = Str; *ch; ch++) {
		if(strchr("~`_'\".,-+=/*!@#$%^&()[]", *ch))
            return 1;
	}

	return strstr(Str, "  ") || *Str == ' ' || Str[strlen(Str)] == ' ';
}

int IsRussOrBad(char* Str, char* ExcludeChrSect = "Q")
{
	return IsRussian(Str, ExcludeChrSect) || IsBadSymbol(Str);
}


void FillPlacement(TComboBox* PlacementCombo, const char* Key, const char* Sect, int);
void SetAgent(char* ACode, TOpenBase& agents, PTComboBox AgName, int);

char* FormTitle = "Зелёная карта";
char* StateNames[6] = { "Нормальный", "Заменён", "Испорчен", "Утерян агентом", "Расторгнут", "Утерян" };

char* BELGREENFields[] = {
"Seria",   //0
"Number",  //1
"PSeria",  //2
"PNumber", //3

"AutoNmb", //4
"Model",   //5

"Owner",   //6
"Org",     //7
"Addr",    //8
"DtFrom",  //9
"Period",  //10
"DtTO",    //11
"Letter",  //12

"Place",   //13
"RegDate", //14
"RegTime", //15

"Tarif",   //16
"Pay1",    //17
"Pay1C",   //18
"Pay2",    //19
"Pay2C",   //20
"PayDate", //21

"AgCode",  //22
"Pcnt",    //23
"State",   //24
"Type",    //25
"RepDate", //26
"Country",  //27
"Text",      //28
"AgType",   //29
"OwnType",   //30
"Body",    //31
"Motor",   //32
"Resident", //33
"RetDate",  //34
"Ret1",     //35
"Ret1Cur",  //36
"UPD",  //37
"AGNAME", //38
"PRCPSER", //39
"PRCPNMB",  //40
"DUPTYPE"  //41
};

char* BELGREENName = "\\bases\\BELGREEN";
#define CNT_BELGREEN_FLDS (sizeof(BELGREENFields) / sizeof(BELGREENFields[0]))

int BELGREENModifyTarif = 1;

LockEdit::LockEdit(PTWindowsObject A, int Id, int Len, int N)
        : TEdit(A, Id, Len)
{
    m_N = N;
}

void LockEdit::WMChar(RTMessage)
{
    PostMessage(GetParent(HWindow), WM_USER + 1, m_N, 0);
}

void LockEdit::WMLButtonDown(RTMessage)
{
	PostMessage(GetParent(HWindow), WM_USER + 1, m_N, 0);
}

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

BELGREEN::BELGREEN(PTWindowsObject p)
	: Dlg_base(p, ActivateParadox ? "BEL_GREEN" : "BGCALC", BELGREENName, BELGREENFields, CNT_BELGREEN_FLDS),
	  agents(AgentDBName, AgentFields, AgFieldsCount)
{
	BELGREEN_data* SIZE;
	if(!*Dlg_base::Agent_Code)
		GetPrivateProfileString("BELGREEN", "DefAgent", "", Dlg_base::Agent_Code, sizeof Dlg_base::Agent_Code, ININame);

	_PrevNumber = -1;

	m_Seria = new TEdit(this, 300, 10);
	m_Number = new NumberEdit(this, 323, 11, 0);
	m_cepNumber = new NumberEdit(this, 104, 11, 0);
	m_pSeria = new TEdit(this, 324, 10);
	m_pNumber = new NumberEdit(this, 325, 11, 0);

	m_autoNmb = new TEdit(this, 301, 11);
	DynStr buff;
	GetPrivateProfileString("BELGREEN", "AutoMarkFile", ::ININame, buff, buff._sizeof(), ::ININame);
//	m_Model->SetININame(buff);
	m_Model = new INIEdit(buff, "MANDATORY", this, 302, sizeof SIZE->Model, "AutoType%d", "AutoType%d_%d");

	m_Body = new TEdit(this, 334, 24);
	m_Motor = new TEdit(this, 332, 24);

	m_Insurer = new TEdit(this, 329, 48);
	m_Organ = new TEdit(this, 303, 48);
	m_Address = new TEdit(this, 304, sizeof SIZE->Address);

	m_dtFrom = new TDateEdit(this, 305, 11);
	(m_Period = new TComboBox(this, 307, 1))->DisableTransfer();
	m_dtTo = new TDateEdit(this, 306, 11);

	m_Place = new TEdit(this, 320, 32);
	m_RegDate = new TDateEdit(this, 321, 11);
	m_Time = new TTimeEdit(this, 322, 6);

	m_Tarif[0] = new LockEdit(this, 309, 10, 0);
	m_Tarif[1] = new LockEdit(this, 311, 10, 1);
	m_Tarif[2] = new LockEdit(this, 313, 10, 2);
	m_Tarif[3] = new LockEdit(this, 315, 10, 3);
	m_Tarif[4] = new LockEdit(this, 317, 10, 4);
	m_Tarif[5] = new LockEdit(this, 318, 10, 5);
	m_Tarif[6] = new LockEdit(this, 319, 10, 6);
	m_Tarif[7] = new LockEdit(this, 350, 10, 7); //Прочее!!!
 	m_Tarif[8] = new LockEdit(this, 351, 10, 8);

	m_PayDate = new TDateEdit(this, 312, 11);
	m_RepDate = new TDateEdit(this, 326, 11);
	m_Pay1 = new NumberEdit(this, 327, 10, 1);
	(m_Curr1 = new TComboBox(this, 501, 1))->DisableTransfer();
	m_Pay2 = new NumberEdit(this, 328, 10, 1);
	(m_Curr2 = new TComboBox(this, 502, 1))->DisableTransfer();

	(m_DupType1 = new TComboBox(this, 108, 1))->DisableTransfer();

	(m_Agent = new TComboBox(this, 500, 1))->DisableTransfer();
	m_AgPercent = new NumberEdit(this, 310, 5, 1);
	(m_Country = new TComboBox(this, 314, 1))->DisableTransfer();

	m_RetDate = new TDateEdit(this, 101, 11);
	m_RetSumma = new NumberEdit(this, 102, 10, 1);
	(m_RetCurr = new TComboBox(this, 103, 1))->DisableTransfer();

	m_IsPricep = new TCheckBox(this, 316, 0);
	m_IsPricep->DisableTransfer();
	m_AgUr = new TCheckBox(this, 331, 0);
	m_InsFiz = new TRadioButton(this, 700, 0);
	m_InsUr_ = new TRadioButton(this, 330, 0);
	m_InsIP = new TRadioButton(this, 701, 0);
	m_InsResid = new TCheckBox(this, 335, 0);
	m_Beznal = new TCheckBox(this, 112, 0);
}

void InitVal(TComboBox* cb);
void InitVal2(TComboBox* cb);

void _FillAgents(TOpenBase* agents, TComboBox* AgName, int index);
void AlignCombo(TComboBox*);

void BELGREEN::SetupWindow()
{
   Dlg_base::SetupWindow();

   static HFONT littleFont = CreateFont(12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Tahoma");

   SendMessage(m_DupType1->HWindow, WM_SETFONT, (WPARAM)littleFont, 0);

   m_DupType1->AddString("Взамен");
   m_DupType1->AddString("Дубликат");

   char Letter[10] = "";
   GetPrivateProfileString("BELGREEN", "Other", "", Letter, sizeof Letter, ININame);
   if(*Letter) SetWindowText(GetDlgItem(HWindow, 1007), Letter);

   InitVal(m_Curr1);
   InitVal2(m_Curr2);
   InitVal2(m_RetCurr);
   m_Curr1->DeleteString(m_Curr1->FindExactString("DM", -1));
   m_Curr2->DeleteString(m_Curr2->FindExactString("DM", -1));
   m_Curr1->DeleteString(m_Curr1->FindExactString("BRB", -1));
   m_Curr2->DeleteString(m_Curr2->FindExactString("BRB", -1));
   //m_Curr1->DeleteString(m_Curr1->FindExactString("RUR", -1));
   //m_Curr2->DeleteString(m_Curr2->FindExactString("RUR", -1));

   _FillAgents(&agents, m_Agent, BELGREEN_TBL);
   FillPlacement(m_Period, "Period%d", "BELGREEN", 0);
   FillPlacement(m_Country, "Country%d", "BELGREEN", 0);

   AlignCombo(m_Country);
   AlignCombo(m_Period);
   AlignCombo(m_Curr1);
   AlignCombo(m_Curr2);
   AlignCombo(m_Agent);
   AlignCombo(m_RetCurr);

   if(ActivateParadox) {
	  if(base->GetRecords())
		  GetDataFromBase();
	  else
		  Default();
   }
   else {
	  date d;
	  getdate(&d);
	  DynStr _buff;
	  wsprintf(_buff, "%02u.%02u.%04u", d.da_day, d.da_mon, d.da_year);
	  m_dtFrom->SetText(_buff);
   }
}

void DblToStr(char* str, double V, int Digit = 2);
extern DATE GD(char* str, int = 0);
void DateFBToStr(TOpenBase* base, int field, char* buffer);

void BELGREEN::GetDataFromBase()
{
   *OldCurr = 0;
   *BaseSumm = 0;

   double N;
   char strBuffer[50];
   memset(&buffer, 0, sizeof buffer);

   ChangeChecker obj(&BELGREENModifyTarif);

   int PolisState;

   m_DupType1->SetSelIndex(-1);
	(*base)(strBuffer, sizeof strBuffer, 24);
	SetState(PolisState = (strBuffer[0] - '0'));
	if(PolisState < BELGREEN_NORMAL || PolisState > BELGREEN_STATE_LAST) MessageBox(HWindow, "Ошибка состояния", strBuffer, MB_OK | MB_ICONSTOP);

	(*base)(buffer.Seria, sizeof buffer.Seria, 0);
	(*base)(N, 1);
	DblToStr(buffer.Number, N, 0);
	_PrevNumber = N+0.00001;
	(*base)(buffer.prSeria, sizeof buffer.prSeria, 2);
	(*base)(N, 3);
	DblToStr(buffer.prNumber, N, 0);

	if(*buffer.prNumber)
		SetState(PolisState, "Дубликат");

	(*base)(buffer.autoNmb, sizeof buffer.autoNmb, 4);
	(*base)(buffer.Model, sizeof buffer.Model, 5);
	(*base)(buffer.Body, sizeof buffer.Model, 31);
	(*base)(buffer.Motor, sizeof buffer.Model, 32);

	(*base)(buffer.Insurer, sizeof buffer.Insurer, 6);
	(*base)(buffer.Organ, sizeof buffer.Organ, 7);
	(*base)(buffer.Address, sizeof buffer.Address, 8);

	(*base)(strBuffer, sizeof strBuffer, 29);
	buffer.AgUr = strBuffer[0] == 'U';
	(*base)(strBuffer, sizeof strBuffer, 30);
	buffer.InsUr_ = strBuffer[0] == 'U';
	buffer.InsFiz = strBuffer[0] == 'F';
	buffer.InsIP = strBuffer[0] == 'I';

	(*base)(strBuffer, sizeof strBuffer, 33);
	buffer.InsResid = strBuffer[0] == 'Y';

	DateFBToStr(base, 9, buffer.dtFrom);
	DateFBToStr(base, 11, buffer.dtTo);

	m_Period->SetSelIndex(-1);
	(*base)(strBuffer, sizeof strBuffer, 10);
	m_Period->SetText(strBuffer);
	if(m_Period->GetCount() && m_Period->GetSelIndex() == -1 && PolisState != BELGREEN_BAD && m_State != BELGREEN_LOST_BY_AGENT)
	    MessageBox(HWindow, "Не найден период", strBuffer, MB_OK | MB_ICONSTOP);

	m_Country->SetSelIndex(-1);
	(*base)(N, 27);
	m_Country->SetSelIndex(N);
	if(m_Country->GetCount() && m_Country->GetSelIndex() == -1 && PolisState != BELGREEN_BAD && m_State != BELGREEN_LOST_BY_AGENT)
		MessageBox(HWindow, "Не найдена страна", strBuffer, MB_OK | MB_ICONSTOP);

	(*base)(buffer.Place, sizeof buffer.Place, 13);
	DateFBToStr(base, 14, buffer.RegDate);
	(*base)(buffer.Time, sizeof buffer.Time, 15);

	buffer.Beznal = buffer.InsUr_;
	(*base)(strBuffer, sizeof strBuffer, 25);

	int Type = strBuffer[0] - '0';
	if(strBuffer[0] >= 'A')
	{
		Type = strBuffer[0] - 'A';
		buffer.Beznal = 1;
    }
	if(strBuffer[0] >= 'I')
	{
		Type = strBuffer[0] - 'I';
		buffer.Beznal = 0;
	}

	if(m_State != BELGREEN_BAD && m_State != BELGREEN_LOST_BY_AGENT) {
	    if(Type < 0 || Type > 8) MessageBox(HWindow, "Ошибка типа", strBuffer, MB_OK | MB_ICONSTOP);

	    (*base)(N, 16);
		DblToStr(buffer.Tarif[Type], N, 0);
		if(!*buffer.Tarif[Type])
			strcpy(buffer.Tarif[Type], "0");
	}

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
	DateFBToStr(base, 26, buffer.RepDate);

    m_Agent->SetSelIndex(-1);
//    char AGENT[10];
    (*base)(strBuffer, sizeof strBuffer, 22);
    SetAgent(strBuffer, agents, m_Agent, 0);


	(*base)(N, 23);
	DblToStr(buffer.AgPercent, N, 2);


	DateFBToStr(base, 34, buffer.RetDate);
	(*base)(N, 35);
	DblToStr(buffer.RetSumma, N, 2);
	(*base)(strBuffer, sizeof strBuffer, 36);
	m_RetCurr->SetSelIndex(-1);
	if(*strBuffer)
	    m_RetCurr->SetText(strBuffer);

	(*base)(strBuffer, sizeof strBuffer, 28);
	SetDlgItemText(HWindow, AUX_TEXT, strBuffer);

	double pSumma = -1;
	if(*buffer.prSeria) {
	      char* ShortFields[] = { "Seria", "Number", "Tarif" };
	      TOpenBase& I = *new TOpenBase(BELGREENName, ShortFields, 3);
	      if(!I.pxErr) {
				I(0, buffer.prSeria);
				I(1, atof(buffer.prNumber));
		if(PXSrchKey(I.getTblHandle(), I.getRecHandle(), 2, SEARCHFIRST) == PXSUCCESS) {
		    I.Get();
		    I(pSumma, 2);
		}
                else
		    MessageBox(HWindow, "Не найден утерянный", buffer.prNumber, 0);
	      }
		  delete &I;

		  char* pbuf = strBuffer + 40;
		  *pbuf = 0;
		  (*base)(pbuf, sizeof(strBuffer)-40, 41);
		  m_DupType1->SetSelIndex(atoi(pbuf));
	}

	if(*strBuffer/* || (Type == 6 && (atof(buffer.Tarif[Type]) < 0.01 && pSumma < 0.01))*/) { //Поиск сцепки!!! сумма 0
		  //MessageBox(0, strBuffer, strBuffer, 0);
		  //Найти для прицепа полис основной
	      char Number[11];
	      strcpy(Number, buffer.Number);
		  char* ShortFields[] = { "Seria", "Number", "LETTER", "PRCPSER", "PRCPNMB", "DTTO"  };
		  TOpenBase& I = *new TOpenBase(BELGREENName, ShortFields, 6);
	      if(!I.pxErr) {
			//Проверим предыдущий для скорости
			I(0, buffer.Seria);
			I(1, atof(buffer.Number) - 1);
			int IsFound = FALSE;
			if(PXSrchKey(I.getTblHandle(), I.getRecHandle(), 2, SEARCHFIRST) == PXSUCCESS) {
				I.Get();
				char Letter[10];
                DATE dt1, dt2;
				I(Letter, sizeof Letter, 2);
                I(dt1, 5);
				(*base)(strBuffer, sizeof strBuffer, 12);
				(*base)(dt2, 11);
				IsFound = !strcmp(Letter, strBuffer) && dt1 == dt2;
			}

			if(!IsFound) {
				I(4, atof(buffer.Number));
				if(PXSrchFld(I.getTblHandle(), I.getRecHandle(), I.getField(4), SEARCHFIRST) == PXSUCCESS) {
				  I.Get();
				  I(N, 1);
				  DblToStr(Number, N + 1, 2); //CreateAddText отнимет
				  if(PXSrchFld(I.getTblHandle(), I.getRecHandle(), I.getField(4), SEARCHNEXT) == PXSUCCESS)
					   MessageBox(HWindow, "Дублирование сцепки", "Ошибка", 0);
				}
			}
			//else
			//   MessageBox(HWindow, "Fast Search", "Ошибка", 0);
		  }
          delete &I;
	      SetDlgItemText(HWindow, AUX_TEXT, CreateAddText(strBuffer, Number));
	      SetFlagPriced(1);
	      m_Curr1->SetSelIndex(-1);
	}
	else
	      SetFlagPriced(0);

	EnableWindow(m_cepNumber->HWindow, (Type == 6) && (atof(buffer.Tarif[Type]) >= 0.01 || *buffer.prSeria));

	(*base)(N, 40);
	DblToStr(buffer.prcpNumber, N, 2);
	if((Type == 6) && (!*buffer.prcpNumber) && !*strBuffer) {
	    strcpy(buffer.prcpNumber, _NextText);
	}

	Dlg_base::GetDataFromBase();
}

void BELGREEN::SetButtons()
{
   Dlg_base::SetButtons();
}

void BELGREEN::SetState(int State, char* AddText)
{
	char* _Title = new char[128];
	wsprintf(_Title, "%s [%s] ", FormTitle, StateNames[State]);
	if(AddText) strcat(_Title, AddText);
	SetWindowText(HWindow, _Title);
	delete _Title;
	m_State = State;
}

void BELGREEN::LOST(RTMessage msg)
{
	m_Insurer->SetText("УТЕРЯН");
	SavePolis();
}

void BELGREEN::BAD(RTMessage msg)
{
	m_Insurer->SetText("ИСПОРЧЕН");
	SavePolis();
}

int BELGREEN::SetDataToBase()
{              
   Dlg_base::SetDataToBase();

   int Index = m_Agent->GetSelIndex();
   if(Index == -1) {
       SetFocus(m_Agent->HWindow);
       return 0;
   }
   char AgCode_[32];
   long ag_code = SendMessage(m_Agent->HWindow, CB_GETITEMDATA, Index, 0);
   memmove(AgCode_, &ag_code, 4);
   AgCode_[4] = 0;


  int IsRussian, IsEnglish, IsDigit, IsBad_, IsBadPolis = 0;
  _str s;
   char strBuffer[64];

   _str* ps = (_str*)strBuffer;
   strcpy(ps->s, buffer.Seria);
   if(!_IsValidPolisNumber("BELGREEN", *ps, atol(buffer.Number), 0, AgCode_)) {
	   MessageBox(HWindow, "Номер полиса не правильный", "Сообщаю", MB_ICONINFORMATION | MB_SYSTEMMODAL);
	   return 0;
   }

   if(_PrevNumber != atol(buffer.Number))
     SetState(BELGREEN_NORMAL);

   _PrevNumber = atol(buffer.Number);

   double SecondNumber = -1;

   if(!strcmp(buffer.Insurer, "ИСПОРЧЕН") || !*buffer.Insurer)
   {
	  if(MessageBox(HWindow, GetRCStr(506), "Отвечай поскорей", MB_YESNO | MB_ICONINFORMATION | MB_SYSTEMMODAL) == IDNO)
		  return 0;
      
	  if(m_IsPricep->GetCheck())
		  SecondNumber = atof(buffer.Number) - 1;
      else
	  if(m_Tarif[6]->GetTextLen())
		  SecondNumber = atof(buffer.Number) + 1;

	  if(SecondNumber != -1)
		MessageBox(HWindow, GetRCStr(601), "Сообщаю", MB_ICONINFORMATION | MB_SYSTEMMODAL);

	  SetState(BELGREEN_BAD);
      m_Insurer->SetText("ИСПОРЧЕН");
	  strcpy(buffer.Insurer, "ИСПОРЧЕН");
	  SetDlgItemText(HWindow, AUX_TEXT, "");
	  SetFlagPriced(0);
	  (*base)(27, (double)-1);
	  IsBadPolis = 1;
   }

   if(!strcmp(buffer.Insurer, "УТЕРЯН"))
   {
	  if(MessageBox(HWindow, "Полис утерян агентом?", "Отвечай поскорей", MB_YESNO | MB_ICONINFORMATION | MB_SYSTEMMODAL) == IDNO)
	  return 0;

	  SetState(BELGREEN_LOST_BY_AGENT);
	  SetDlgItemText(HWindow, AUX_TEXT, "");
	  SetFlagPriced(0);
	  (*base)(27, (double)-1);
	  IsBadPolis = 1;
   }

   (*base)(0, buffer.Seria);
   (*base)(1, atof(buffer.Number));
   (*base)(6, buffer.Insurer);
   (*base)(30, buffer.InsUr_ ? "U" : buffer.InsIP ? "I" : "F");
   (*base)(29, buffer.AgUr ? "U" : "F");
   (*base)(33, buffer.InsResid ? "Y" : "N");


   strcpy(s.s, buffer.Insurer);
   ExpStr(s, IsRussian, IsEnglish, IsDigit, IsBad_, 1);
   if(!IsBadPolis && (IsRussian || IsBad_))
   {
		SetFocus(m_Insurer->HWindow);
   		return 0;
   }

   strcpy(s.s, buffer.Organ);
   ExpStr(s, IsRussian, IsEnglish, IsDigit, IsBad_, 1);
   if(!IsBadPolis && (IsRussian || IsBad_))
   {
		SetFocus(m_Organ->HWindow);
   		return 0;
   }

   DATE Rep_date = *buffer.RepDate ? GD(buffer.RepDate) : 0;
   if(*buffer.RepDate && !Rep_date)
   {
	   SetFocus(m_RepDate->HWindow);
       return 0;
   }

   DATE reg_date = GD(buffer.RegDate);
   if(!reg_date)
   {
       SetFocus(m_RegDate->HWindow);
       return 0;
   }
   if(IsBadPolis && Rep_date == 0) Rep_date = reg_date;
   (*base)(14, reg_date);
   (*base)(15, buffer.Time);
   if(strchr(buffer.Time, ';')) *strchr(buffer.Time, ';') = ':';
   if(strlen(buffer.Time) != 5 || strchr(buffer.Time, ':') == 0 || atoi(buffer.Time) > 23)
   {
       SetFocus(m_Time->HWindow);
       return 0;
   }
   (*base)(22, AgCode_);
   m_Agent->GetText(AgCode_, sizeof AgCode_);
   (*base)(38, AgCode_);

   if(!IsBadPolis)
   {
	  DATE from_date = GD(buffer.dtFrom);
	  if(!from_date) {
         SetFocus(m_dtFrom->HWindow);
         return 0;
      }
	  (*base)(9, from_date);
      DATE to_date = GD(buffer.dtTo);
      if(!to_date) {
         SetFocus(m_dtTo->HWindow);
		 return 0;
      }
	  (*base)(11, to_date);
	  int IsSetTarif = 0;
	  for(int Type = 0; Type < 9; Type++) {
		  if(*buffer.Tarif[Type]) {
			  (*base)(16, atof(buffer.Tarif[Type]));
			  IsSetTarif = 1;
			  break;
		  }
	  }

	  char FirstChar = buffer.Beznal ? 'A' : 'I';
	  strBuffer[0] = FirstChar + Type;
	  strBuffer[1] = 0;
	  (*base)(25, strBuffer);

	  GetDlgItemText(HWindow, 1000 + Type, strBuffer, sizeof strBuffer);
	  (*base)(12, strBuffer);

	  if(!IsSetTarif) {
		SetFocus(m_Tarif[0]->HWindow);
          	return 0;
	  }

	  //Запись номера прицепа
	  if(strchr(strBuffer, '+') && (atof(buffer.Tarif[Type]) >= 0.01 || *buffer.prSeria))
	  {
	       if(*buffer.prcpNumber && *buffer.prcpNumber != _NextText[0]) {
	           (*base)(39, buffer.Seria);
		   	   (*base)(40, atof(buffer.prcpNumber));
		   }
		   //Не забудь загеристрировать прицеп!
		   MessageBox(HWindow, GetRCStr(600), "Сообщаю", MB_ICONINFORMATION);
	  }

	  DATE Pay_date = GD(buffer.PayDate);
      if(!Pay_date) {
         SetFocus(m_PayDate->HWindow);
         return 0;
      }
	  if(strlen(buffer.Address) < 5) {
		SetFocus(m_Address->HWindow);
         return 0;
      }
      (*base)(21, Pay_date);
	  if(Rep_date == 0) Rep_date = Pay_date;

	  //
	  char msg[48];
	  if(IsRussOrBad(strcpy(msg, buffer.Model)) ||
		 IsRussOrBad(strcpy(msg, buffer.autoNmb), "AutoNmb_AllowChars") ||
		 IsBadSymbol(strcpy(msg, buffer.Body)) ||
		 IsBadSymbol(strcpy(msg, buffer.Motor)) ||
		 IsRussOrBad(strcpy(msg, buffer.Insurer)) ||
		 IsRussOrBad(strcpy(msg, buffer.Organ)) ||
		 IsRussOrBad(strcpy(msg, buffer.Address))) {
		 sprintf(strBuffer, GetRCStr(599), msg);
		 MessageBox(HWindow, strBuffer, "Сообщаю", MB_ICONINFORMATION);
		 return 0;
      }
	  //

      int IsDup = 0;

	  m_pRegDate = 0;
	  if(*buffer.prSeria || atol(buffer.prNumber) > 0)
	  {
		  DynStr msgStr;
		  //Вы уверены что полис %s/%s нуна заменить?
		  sprintf(msgStr, GetRCStr(598), buffer.prSeria, buffer.prNumber);
		  if(MessageBox(HWindow, msgStr, "Отвечай поскорей", MB_YESNO | MB_ICONINFORMATION | MB_SYSTEMMODAL) == IDNO)
			  return 0;

		  char* ShortFields[] = { "Seria", "Number", "State", "RegDate", "UPD", "DtFrom" };
		  TOpenBase I(BELGREENName, ShortFields, 6);
		  if(I.pxErr)
		  {
	          Err:MessageBox(HWindow, GetRCStr(552), "Ошибка", MB_OK | MB_ICONSTOP);
	          return 0;
	      }
	      I(0, buffer.prSeria);
	      I(1, atof(buffer.prNumber));
		  if(PXSrchKey(I.getTblHandle(), I.getRecHandle(), 2, SEARCHFIRST) == PXSUCCESS)
		  {
			 I.Get();
			 DATE StartDateDup;
			 I(StartDateDup, 5);
			 if(StartDateDup != from_date)
			 {
				 MessageBox(HWindow, GetRCStr(627), "Ошибка", MB_OK | MB_ICONSTOP);
                 return 0;
			 }
			 I(m_pRegDate, 3);
			 char State[2];
			 I(State, sizeof State, 2);
			 if((State[0] - '0') == BELGREEN_BAD || (State[0] - '0') == BELGREEN_STOPPED)
			 {
		         MessageBox(HWindow, GetRCStr(563), "Ошибка", MB_OK | MB_ICONSTOP);
                 return 0;
			 }

			 if(m_DupType1->GetSelIndex() == 0) strBuffer[0] = '0' + BELGREEN_REPLACED;
			 else strBuffer[0] = '0' + BELGREEN_LOST;

			 strBuffer[1] = 0;
			 I(2, strBuffer);

             struct date d;
             getdate(&d);
             DATE UPD;
             PXDateEncode(d.da_mon, d.da_day, d.da_year, &UPD);
			 I(4, UPD);

			 if(PXRecUpdate(I.getTblHandle(), I.getRecHandle()) != PXSUCCESS) goto Err;

			 IsDup = 1;

			 if(m_DupType1->GetSelIndex() < 0)
			 {
				SetFocus(m_DupType1->HWindow);
             	return 0;
             }

			 strcpy(strBuffer, "0");
			 strBuffer[0] += m_DupType1->GetSelIndex();
			 (*base)(41, strBuffer);
	      }
		  else
		  {
	         MessageBox(HWindow, GetRCStr(551), "Ошибка", MB_OK | MB_ICONSTOP);
	         return 0;
  	      }
	  } //Dublicate Block

      (*base)(2, buffer.prSeria);
      (*base)(3, atof(buffer.prNumber));

	  if(!m_IsPricep->GetCheck())
      {
		if((atof(buffer.Pay1) < 0.01 && !IsDup) || (atof(buffer.Pay1) > 0.01 && IsDup)) {
			SetFocus(m_Pay1->HWindow);
			return 0;
		}
      }

	  (*base)(17, atof(buffer.Pay1)); //SUMM1
	  m_Curr1->GetText(strBuffer, sizeof strBuffer);
	  if(strlen(strBuffer) == 0 && !m_IsPricep->GetCheck())
	  {
	      SetFocus(m_Pay1->HWindow);
          return 0;
	  }
	  char Curr1[5];
      strcpy(Curr1, strBuffer);
      (*base)(18, strBuffer); //VAL CURR 1

	  if(IsDup)
	  {
	      (*base)(19, atof("")); //SUMM2
          (*base)(20, ""); //VAL CURR 2
	  }
	  else
	  {
		  m_Curr2->GetText(strBuffer, sizeof strBuffer);
		  (*base)(19, atof(buffer.Pay2)); //SUMM2
		  (*base)(20, strBuffer); //VAL CURR 2

		  if(*strBuffer && atof(buffer.Pay2) < 0.01 || !*strBuffer && atof(buffer.Pay2) >= 0.01)
		  {
			  SetFocus(m_Pay2->HWindow);
			  return 0;
		  }
		  if(*strBuffer && !strcmp(strBuffer, Curr1))
		  {
			  SetFocus(m_Curr1->HWindow);
			  return 0;
		  }
      }

	  (*base)(13, buffer.Place);
	  strcpy(s.s, buffer.Place);
	  ExpStr(s, IsRussian, IsEnglish, IsDigit, IsBad_, 1);
	  if(IsEnglish || !*buffer.Place || *buffer.Place == ' ') {
			  SetFocus(m_Place->HWindow);
			  return 0;
	  }
	  (*base)(7, buffer.Organ);

	  if(to_date <= from_date || (to_date - from_date) > 370)
	  {
		  SetFocus(m_dtFrom->HWindow);
		  return 0;
	  }
	  if(!IsDup) //NORMAL POLIS
	  {
          int RegDateBeforehand = GetPrivateProfileInt("BELGREEN", "RegDateBeforehand", 92, ININame);
          if(RegDateBeforehand <= 0) RegDateBeforehand = 1;
		  if((reg_date + RegDateBeforehand) < from_date || reg_date > from_date) {
		 	  MessageBox(HWindow, GetRCStr(594), "Ошибка", MB_OK | MB_ICONSTOP);
			  SetFocus(m_RegDate->HWindow);
		      return 0;
		  }
	  }
	  else //DUBLICATE
	  {
		  if(reg_date < from_date)
		  {
		 	  MessageBox(HWindow, GetRCStr(595), "Ошибка", MB_OK | MB_ICONSTOP);
			  SetFocus(m_RegDate->HWindow);
		      return 0;
		  }
      }

	  if(m_Period->GetSelIndex() == -1)
	  {
  	    SetFocus(m_Period->HWindow);
        return 0;
      }
      m_Period->GetText(strBuffer, sizeof strBuffer);
      (*base)(10, strBuffer);

	  if(m_Country->GetSelIndex() == -1) {
		SetFocus(m_Country->HWindow);
        return 0;
      }
	  (*base)(27, (double)m_Country->GetSelIndex());

	  (*base)(4, buffer.autoNmb);
      (*base)(5, buffer.Model);
	  (*base)(31, buffer.Body);
	  (*base)(32, buffer.Motor);

	  (*base)(8, buffer.Address);
	  if(!*buffer.Model) {
  	     SetFocus(m_Model->HWindow);
	     return 0;
	  }

	  if(!*buffer.autoNmb && !*buffer.Body && !*buffer.Motor) {
		 SetFocus(m_autoNmb->HWindow);
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

	  //Проверка дат
	  if(!IsDup && Pay_date > from_date)
	  {
		  SetFocus(m_PayDate->HWindow);
		  return 0;
	  }

	  if(!IsDup)
	  {
		  int RegDateBeforehand = GetPrivateProfileInt("BELGREEN", "RegDateBeforehand", 92, ININame);
          if(RegDateBeforehand <= 0) RegDateBeforehand = 1;
		  if((Pay_date + (m_InsUr_->GetCheck() == 1 ? 365 : RegDateBeforehand)) < from_date || Pay_date > reg_date)
		  {
			  SetFocus(m_PayDate->HWindow);
              return 0;
		  }
	  }
	  else
	  {
		  //У дубликата дата выдачи в периоде страхования
		  if(reg_date < from_date || reg_date > to_date)
		  {
			  SetFocus(m_RegDate->HWindow);
              return 0;
		  }

		  if(Pay_date != reg_date && m_DupType1->GetSelIndex() == 0)
		  {
			  SetFocus(m_RegDate->HWindow);
              return 0;
		  }
	  }
	  GetDlgItemText(HWindow, AUX_TEXT, strBuffer, sizeof strBuffer);
	  strBuffer[7] = 0;
	  (*base)(28, strBuffer);

	  //RETURN MONEY
	  if(*buffer.RetDate && !*buffer.RetSumma || !*buffer.RetDate && *buffer.RetSumma) {
	      SetFocus(m_RetDate->HWindow);
	      return 0;
	  }

	  if(*buffer.RetDate) {
		  if(m_State == BELGREEN_LOST) {
		 MessageBox(HWindow, GetRCStr(593), "Ошибка", MB_OK | MB_ICONSTOP);
		  return 0;
              }

	      DATE RetDt = GD(buffer.RetDate);
	      if(!RetDt || RetDt < reg_date || RetDt >= to_date) {
  	          SetFocus(m_RetDate->HWindow);
		  return 0;
	      }
	      (*base)(34, RetDt);
	      if(atof(buffer.RetSumma) < 0) {
	      	  SetFocus(m_RetSumma->HWindow);
	      	  return 0;
	      }
	      (*base)(35, atof(buffer.RetSumma));
	      m_RetCurr->GetText(strBuffer, sizeof strBuffer);
	      if(!*strBuffer) {
		  SetFocus(m_RetCurr->HWindow);
		  return 0;
	      }
	      (*base)(36, strBuffer);
	      SetState(BELGREEN_STOPPED);
	  }
	  else
	  if(m_State == BELGREEN_STOPPED)
	      SetState(BELGREEN_NORMAL);
}

   strBuffer[0] = '0' + m_State;
   strBuffer[1] = 0;
   (*base)(24, strBuffer);
   (*base)(26, Rep_date);

   struct date d;
   getdate(&d);
   DATE UPD;
   PXDateEncode(d.da_mon, d.da_day, d.da_year, &UPD);
   (*base)(37, UPD);

   return 1;
}

BELGREEN::~BELGREEN()
{
}

int BELGREEN::auxFindSN(char* Seria, double Number, int Msg)
{
	(*base)(0, Seria);
	(*base)(1, Number);
    if(!PXSrchKey(base->getTblHandle(), base->getRecHandle(), 2, SEARCHFIRST))
    {
       base->Get();
	   GetDataFromBase();
       return 1;
    }
    else {
	   if(Msg) MessageBox(HWindow, GetRCStr(556), "Ошибка поиска", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
	   return 0;
    }
}

void BELGREEN::FindSN()
{
    char buffs[16];
    char buffn[16];
	m_Seria->GetText(buffs, sizeof buffs);
    m_Number->GetText(buffn, sizeof buffn);
    auxFindSN(buffs, atof(buffn));
}

void BELGREEN::FindPricep()
{
    char buffs[16];
    char buffn[16];
    m_Seria->GetText(buffs, sizeof buffs);
    m_cepNumber->GetText(buffn, sizeof buffn);
    if(atol(buffn) == 0) {
	m_Number->GetText(buffn, sizeof buffn);
	sprintf(buffn, "%ld", atol(buffn) + 1);
    }
    auxFindSN(buffs, atof(buffn));
}

void BELGREEN::FindPrSN()
{
    char buffs[16];
    char buffn[16];
    m_pSeria->GetText(buffs, sizeof buffs);
    m_pNumber->GetText(buffn, sizeof buffn);
    auxFindSN(buffs, atof(buffn));
}

void BELGREEN::FindAutoNmb()
{
    char Maska[32] = "";
    char SN[32] = "";
    m_autoNmb->GetText(Maska, sizeof Maska);
    if(!*Maska) return;
    char* CheckFields[3] = { "owner", 0, 0 };
    Array Strings(10, 0, 10);
    if (auxFindLike(BELGREENName, "AutoNmb", Maska, Strings, this, SN, 0, CheckFields) == 1) {
       char* ch = strchr(SN, '/');
       *ch = 0;
       auxFindSN(SN, atof(ch + 1));
   }
}

void BELGREEN::FindOrg()
{
    char Maska[32] = "";
    char SN[32] = "";
    m_Organ->GetText(Maska, sizeof Maska);
    if(!*Maska) return;
    char* CheckFields[3] = { "owner", 0, 0 };
    Array Strings(10, 0, 10);
    if (auxFindLike(BELGREENName, "Org", Maska, Strings, this, SN, 0, CheckFields) == 1) {
       char* ch = strchr(SN, '/');
       *ch = 0;
       auxFindSN(SN, atof(ch + 1));
   }
}

void BELGREEN::FindName()
{
    char Maska[32] = "";
    char SN[32] = "";
    m_Insurer->GetText(Maska, sizeof Maska);
    if(!*Maska) return;
    char* CheckFields[3] = { "Text", 0, 0 };
    Array Strings(10, 0, 10);
    if (auxFindLike(BELGREENName, "owner", Maska, Strings, this, SN, 0, CheckFields) == 1) {
       char* ch = strchr(SN, '/');
       *ch = 0;
       auxFindSN(SN, atof(ch + 1));
   }
}

void BELGREEN::Clear()
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
   SetFlagPriced(0);
//   m_IsPricep->SetCheck(0);
   SetDlgItemText(HWindow, AUX_TEXT, "");

   Default();
}

void BELGREEN::Default()
{
   date d;
   getdate(&d);
   DynStr _buff;
   wsprintf(_buff, "%02u.%02u.%04u", d.da_day, d.da_mon, d.da_year);
   m_dtFrom->SetText(_buff);
   m_RegDate->SetText(_buff);
   m_PayDate->SetText(_buff);
   struct time t;
   gettime(&t);
   wsprintf(_buff, "%02u:%02u", t.ti_hour, t.ti_min);
   m_Time->SetText(_buff);

   TMessage msg;
   msg.LP.Hi = CBN_SELCHANGE;
   if(*Dlg_base::Agent_Code) {
	  SetAgent(Dlg_base::Agent_Code, agents, m_Agent, 0);
      ChAgent(msg);
   }

   m_InsResid->SetCheck(1);
   m_Period->SetSelIndex(GetPrivateProfileInt("BELGREEN", "DefPeriod", 0, ININame));
   m_Country->SetSelIndex(GetPrivateProfileInt("BELGREEN", "DefCountry", 0, ININame));
   GetPrivateProfileString("BELGREEN", "DefSeria", "", _buff, _buff._sizeof(), ININame);
   AnsiUpper(_buff);
   m_Seria->SetText(_buff);
   GetPrivateProfileString("BELGREEN", "DefPlace", "", _buff, _buff._sizeof(), ININame);
   AnsiUpper(_buff);
   m_Place->SetText(_buff);
   m_Address->SetText("BELARUS ");

   CalcEndDate();
   CalcTarif(GetPrivateProfileInt("BELGREEN", "DefAutoType", 0, ININame));

   SetFlagPriced(0);
   SetState(BELGREEN_NORMAL);
}

void BELGREEN::ChAgent(RTMessage msg)
{
   int IsUr;
   if(msg.LP.Hi == CBN_SELCHANGE) {
	   m_AgPercent->SetText(GetAgPercent(SendMessage(m_Agent->HWindow, CB_GETITEMDATA, m_Agent->GetSelIndex(), 0), BELGREEN_TBL, IsUr));
       m_AgUr->SetCheck(IsUr);
   }
}

void BELGREEN::ChPeriod(RTMessage msg)
{
   if(msg.LP.Hi == CBN_SELCHANGE && BELGREENModifyTarif) {
       CalcEndDate();
       CalcTarif(-1);
   }
}

void BELGREEN::ChStartDate(RTMessage msg)
{
   if(msg.LP.Hi == EN_CHANGE && BELGREENModifyTarif)
       CalcEndDate();
}

DATE aux_incDate(DATE startdate, int inc1, int value);
int getDurationType(char* str, char** = 0);

void BELGREEN::CalcEndDate()
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

void BELGREEN::CalcTarif(int IndexAutoType)
{
	 if(!BELGREENModifyTarif) return;

	 int isDup = m_pSeria->GetTextLen() > 0 || m_pNumber->GetTextLen() > 0 || m_IsPricep->GetCheck();

	 int IndexCountry = m_Country->GetSelIndex();
	 int IndexPeriod = m_Period->GetSelIndex();
	 if(IndexAutoType == -1) {
		 for(int i = 0; i < 9; i++)
			 if(m_Tarif[i]->GetTextLen())
				 IndexAutoType = i;
     }
	 if(IndexCountry == -1 || IndexAutoType == -1 || IndexPeriod == -1) return;

	 EnableWindow(m_cepNumber->HWindow, IndexAutoType == 6);

	 DynStr _tarifStr;
	 sprintf(_tarifStr, "Tarif%d_%d", IndexCountry, IndexAutoType);
	 GetPrivateProfileString("BELGREEN", _tarifStr, "", _tarifStr, _tarifStr._sizeof(), ININame);

     char* firstdivider = _tarifStr;
     char* OneTarifStr;
     for(int i = 0; i <= IndexPeriod; i++) {
		 firstdivider = strchr(OneTarifStr = firstdivider, ',');
		 if(!firstdivider) break;
		 *firstdivider = 0;
		 firstdivider++;
	 }

     if((IndexPeriod - i) > 1) {
		 m_Tarif[IndexAutoType]->SetText("?");
		 SetVal();
		 return;
	 }

	 while(*OneTarifStr == ' ') OneTarifStr++;
	 
     m_Tarif[IndexAutoType]->SetText(isDup ? "0" : OneTarifStr);
	 SetVal();
}

void BELGREEN::SetVal()
{
	 char buff[26] = "";

	 for(int i = 0; i < 9; i++)
		 if(m_Tarif[i]->GetText(buff, sizeof buff))
             break;

	 m_Pay1->SetText(buff);
     strcpy(BaseSumm, buff);
     if(m_IsPricep->GetCheck())
	     m_Curr1->SetSelIndex(-1);
     else {
		 m_Curr1->SetText("EUR");
		 strcpy(OldCurr, "EUR");
     }

     m_Pay2->SetText("");
     m_Curr2->SetSelIndex(0);
}

void BELGREEN::Find()
{
   base->Refresh();
   HMENU Popup = CreatePopupMenu();
   AppendMenu(Popup, MF_ENABLED | MF_STRING, 720, "По Серии/Номеру");
   if(m_Tarif[6]->GetTextLen() > 1)
       AppendMenu(Popup, MF_ENABLED | MF_STRING, 725, "Сцепку");
   AppendMenu(Popup, MF_SEPARATOR, 0, "");
   AppendMenu(Popup, MF_ENABLED | MF_STRING, 721, "Утерянный");
   AppendMenu(Popup, MF_ENABLED | MF_STRING, 722, "По страхователю");
   AppendMenu(Popup, MF_ENABLED | MF_STRING, 723, "По гос. номеру");
   AppendMenu(Popup, MF_ENABLED | MF_STRING, 724, "По огранизации");
   POINT ptCurrent;
   GetCursorPos(&ptCurrent);
   TrackPopupMenu(Popup, TPM_LEFTALIGN, ptCurrent.x, ptCurrent.y, 0, HWindow, NULL);
   DestroyMenu(Popup);
}

void BELGREEN::WMCommand(RTMessage msg)
{
   if(msg.Message == WM_COMMAND) {
      if(msg.WParam == 720) FindSN();
	  if(msg.WParam == 721) FindPrSN();
	  if(msg.WParam == 722) FindName();
	  if(msg.WParam == 723) FindAutoNmb();
	  if(msg.WParam == 724) FindOrg();
	  if(msg.WParam == 725) FindPricep();
   }
   Dlg_base::WMCommand(msg);
}

void BELGREEN::Increment(RTMessage msg)
{
	char ser[10], s[10];
	m_Seria->GetText(ser, sizeof ser);
	m_Number->GetText(s, sizeof s);
	sprintf(s, "%lu", atol(s) + 1);
	m_Number->SetText(s);
	if(!auxFindSN(ser, atol(s), 0)) {
		Clear();
		m_Number->SetText(s);
	}
}

void BELGREEN::ChCountry(RTMessage msg)
{
   CalcTarif(-1);
}

void BELGREEN::ChangeAutoType(RTMessage msg)
{
   if(m_IsPricep->GetCheck()) return;

   for(int i = 0; i < 9; i++)
       m_Tarif[i]->SetText("");
   CalcTarif(msg.WParam);
}

void BELGREEN::ChPS(RTMessage msg)
{
    if(msg.LP.Hi == EN_CHANGE)
       CalcTarif(-1);
}

void BELGREEN::ChPN(RTMessage msg)
{
	if(msg.LP.Hi == EN_CHANGE)
       CalcTarif(-1);
}

void BELGREEN::Print1()
{
	PrintData(1);
}

void BELGREEN::Print2()
{
	PrintData(2);
}

void BELGREEN::PrintAll()
{
	PrintData(-1);
}

void BELGREEN::PrintData(int Flag)
{
   if(!SetDataToBase()) return;
   char* dest = new char [2500];
#define SIZE_PREMIUM 500
   char* _Premium = new char [SIZE_PREMIUM];
   char* strdata = dest;
   memset(dest, 0, 2400);

   if(Flag == 1 || Flag == -1) {

	 m_dtFrom->GetText(buffer.dtFrom, sizeof buffer.dtFrom);
	 strcpy(dest, strtok(buffer.dtFrom, "./"));
	 dest += strlen(dest) + 1;
	 strcpy(dest, strtok(0, "./"));
	 dest += strlen(dest) + 1;
	 strcpy(dest, strtok(0, "./"));
	 dest += strlen(dest) + 1;

	 m_dtTo->GetText(buffer.dtTo, sizeof buffer.dtTo);
	 strcpy(dest, strtok(buffer.dtTo, "./"));
	 dest += strlen(dest) + 1;
	 strcpy(dest, strtok(0, "./"));
	 dest += strlen(dest) + 1;
	 strcpy(dest, strtok(0, "./"));
	 dest += strlen(dest) + 1;

	 if(*buffer.autoNmb) strcpy(dest, buffer.autoNmb);
	 else if(*buffer.Body) strcpy(dest, buffer.Body);
	 else if(*buffer.Motor) strcpy(dest, buffer.Motor);
	 dest += strlen(dest) + 1;

	 char Letter[5];
	 (*base)(Letter, sizeof Letter, 12);

	 if(strstr(Letter, "+") || *buffer.Tarif[6])
		 {                                         

	     if(GetDlgItemText(HWindow, AUX_TEXT, dest, 5) > 0)
	     {
		GetPrivateProfileString("BELGREEN", "Hitch", Letter, dest, sizeof Letter, ININame);
		 }
	     else
	     {
		GetPrivateProfileString("BELGREEN", "Trailer", Letter, dest, sizeof Letter, ININame);
		 }
	 }
	 else if(*buffer.Tarif[7])
		GetPrivateProfileString("BELGREEN", "Other", Letter, dest, sizeof Letter, ININame);
	 else
	 {
		strcpy(dest, Letter);
         }

	 dest += strlen(dest) + 1;

	 strcpy(dest, buffer.Model);
	 dest += strlen(dest) + 1;

	 strcpy(dest, buffer.Insurer);
	 strcat(dest, "\r\n");
	 if(*buffer.Organ) {
  	   strcat(dest, buffer.Organ);
	   strcat(dest, "\r\n");
     }
	 strcat(dest, buffer.Address);
	 dest += strlen(dest) + 1;
   }
   else
	 dest += 10;

   if(Flag == 2 || Flag == -1) {
	 GetDlgItemText(HWindow, AUX_TEXT, _Premium, SIZE_PREMIUM);
	 if(*_Premium) {
		 CreateAddText(_Premium, 0);
	 }
	 else
	 if(m_pSeria->GetTextLen() > 0) {
		 int d, m, y;
		 PXDateDecode(m_pRegDate, &m, &d, &y);
		 sprintf(_Premium, "Выдан взамен з/к №%s от \"%02d\" %s %d г.", buffer.prNumber, d, GetRCStr(99 + m), y);
	 }
	 else {
		char* ch = _Premium;
		m_Pay1->GetText(_Premium, SIZE_PREMIUM);
		strcat(ch, " (");
		strcat(ch, NumberToWords(atof(ch)));
		strcat(ch, ") ");
		ch += strlen(ch);
		m_Curr1->GetText(ch, SIZE_PREMIUM);

		ch += strlen(ch) + 1;
		if(m_Pay2->GetTextLen() > 0) {
			strcpy(ch-1, "\r\n");
			ch += strlen(ch);
			m_Pay2->GetText(ch, SIZE_PREMIUM);
  			strcat(ch, " (");
			strcat(ch, NumberToWords(atof(ch)));
			strcat(ch, ") ");
			ch += strlen(ch);
			m_Curr2->GetText(ch, SIZE_PREMIUM);
			ch += strlen(ch) + 1;
		}
	 }

	 strcat(dest, _Premium);
	 dest += strlen(dest) + 1;

	 strcat(dest, buffer.Place);
	 strcat(dest, "\r\n");
	 dest += strlen(dest);
	 m_Agent->GetText(dest, 50);
	 dest += strlen(dest) + 1;

	 m_RegDate->GetText(buffer.RegDate, sizeof buffer.RegDate);
	 strcpy(dest, strtok(buffer.RegDate, "./"));
	 dest += strlen(dest) + 1;
	 strcpy(dest, strtok(0, "./"));
	 dest += strlen(dest) + 1;
	 strcpy(dest, strtok(0, "./"));
	 dest += strlen(dest) + 1;

	 strcpy(dest, strtok(buffer.Time, ":"));
	 dest += strlen(dest) + 1;
	 strcpy(dest, strtok(0, ":"));
	 dest += strlen(dest) + 1;
   }
   else
	 dest += 7;

   if(Flag == 1 || Flag == -1) {
	 char key[24];
#define SIZE_BUFF 250


	 char* buffer = new char[SIZE_BUFF];
	 wsprintf(key, "Country%d_Cross", m_Country->GetSelIndex());
	 GetPrivateProfileString("BELGREEN", key, "", buffer, SIZE_BUFF, ININame);
	 for(int i = 0; i < 30; i++) {
			sprintf(key, "|%d|", i+1);
			if(strstr(buffer, key)) {
			  strcpy(dest, "X");
			  dest += strlen(dest) + 1;
			}
			else {
			  strcpy(dest, " ");
			  dest += strlen(dest) + 1;
			}
	 }
	 if (m_Tarif[6]->GetTextLen() > 0) { //Тариф по сцепке
		 m_Tarif[6]->GetText(key, sizeof key);
		 if(atof(key) > 1) { //тока для основного полиса - основной - будет сумма
			 m_Seria->GetText(buffer, SIZE_BUFF);
		     m_Number->GetText(key, sizeof key);
		     sprintf(buffer + strlen(buffer), "/%lu", atol(key)+1);
			 strcpy(dest, buffer);
         }
	 }
	 delete buffer;
   }

   delete _Premium;
   ((TRulerWin*)Parent)->SetValues(strdata);
   Dlg_base::Print1();
   delete dest;

//   MessageBox(0, "", "", 0);
}

int BELGREEN::IsExistsNext(int RetToOldPos)
{
	char s[10], n[10];
	m_Seria->GetText(s, sizeof s);
	m_Number->GetText(n, sizeof n);
	long Pos = base->NumberCurrent();
	int Found = auxFindSN(s, atol(n) + 1, 0);
	if(RetToOldPos) { 
	   PXRecGoto(base->getTblHandle(), Pos);
	   base->Get();
	   GetDataFromBase();
	}
	return Found;
}

void BELGREEN::SetFlagPriced(int Flag)
{
	EnableWindow(m_Seria->HWindow, !Flag);
	EnableWindow(m_Number->HWindow, !Flag);
	EnableWindow(m_pSeria->HWindow, !Flag);
	EnableWindow(m_pNumber->HWindow, !Flag);
	m_IsPricep->SetCheck(Flag);
}

void BELGREEN::CreatePolisPricep()
{
	if(m_State != BELGREEN_NORMAL) return;
	if(m_IsPricep->GetCheck()) return;
	if(m_Tarif[6]->GetTextLen() == 0) return;
	if(m_Insurer->GetTextLen() == 0) return;
	if(!SavePolis()) return;

	char buff[50];
	m_cepNumber->GetText(buff, sizeof buff);

	if(IsExistsNext(1))
		MessageBox(HWindow, GetRCStr(596), "Сообщаю", MB_ICONINFORMATION);

	m_cepNumber->SetText(buff);
	if(m_State != BELGREEN_NORMAL) return;

    int Delta = 0;
	m_cepNumber->GetText(buff, sizeof buff);
	if(atol(buff) == 0) {
		m_Number->GetText(buff, sizeof buff);
        Delta = 1;
    }
	wsprintf(buff, GetRCStr(597), atol(buff) + Delta);
	if(MessageBox(HWindow, buff, "Ответь", MB_YESNO | MB_ICONQUESTION) == IDNO) return;
	CreatePricep();
}

char* BELGREEN::CreateAddText(char* smsg, char* nmb)
{
	strcpy(smsg, "Сцепка з/к по тягячу №");
	m_Seria->GetText(smsg + strlen(smsg), 7);
	strcat(smsg, "/");
	char n[10];
    if(!nmb)
		m_Number->GetText(n, sizeof n);
	else
		strcpy(n, nmb);
	sprintf(smsg + strlen(smsg), "%lu", atol(n) - 1);
	return smsg;
}

void BELGREEN::CreatePricep()
{
	SetFlagPriced(1);
	char n[10];
		*n = 0;
	char oldser[10] = "";
    double oldnn;

	char oldAutoNmb[25] = "";
	char oldModel[25] = "";
	char oldBody[25] = "";
	char oldMotor[25] = "";

	//Проверка на дубликаты
	if(m_pSeria->GetTextLen() > 0) {
		//Found pricep for lost polis
		m_pSeria->GetText(oldser, sizeof oldser);
		m_pNumber->GetText(n, sizeof n);
		char* ShortFields[] = { "Seria", "Number", "PrcpSer", "PrcpNmb", "autonmb", "model", "body", "motor" };
	    TOpenBase& I = *new TOpenBase(BELGREENName, ShortFields, 8);
	    if(!I.pxErr) {
  		   I(0, oldser);
		   I(1, atof(n));
		   if(PXSrchKey(I.getTblHandle(), I.getRecHandle(), 2, SEARCHFIRST) == PXSUCCESS) {
			  I.Get();
			  I(oldser, sizeof(oldser), 2);
			  I(oldnn, 3);
			  if(!*oldser) {
				m_pSeria->GetText(oldser, sizeof oldser);
				m_pNumber->GetText(n, sizeof n);
				oldnn = atof(n) + 1;
			  }
   		      I(0, oldser);
			  I(1, oldnn);
			  if(PXSrchKey(I.getTblHandle(), I.getRecHandle(), 2, SEARCHFIRST) == PXSUCCESS) {
 			      I.Get();
				  I(oldAutoNmb, sizeof oldAutoNmb, 4);
				  I(oldModel, sizeof oldModel, 5);
				  I(oldBody, sizeof oldBody, 6);
				  I(oldMotor, sizeof oldMotor, 7);
			  }
			  else {
				 DblToStr(n, oldnn, 0);
				 MessageBox(HWindow, "Не найдена сцепка", n, 0);
                 return;
			  }
		   }
           else {
			  MessageBox(HWindow, "Не найден полис", n, 0);
              return;
		   }
	    }
	    delete &I;
	}

	m_cepNumber->GetText(n, sizeof n);
	if(atol(n) == 0) {
		m_Number->GetText(n, sizeof n);
		wsprintf(n, "%lu", atol(n) + 1);
	}

	m_Number->SetText(n);

	char smsg[64];
	CreateAddText(smsg, n);

	if (*oldser) {
		m_pSeria->SetText(oldser);
        DblToStr(oldser, oldnn, 0);
		m_pNumber->SetText(oldser);
	}
    else {
		m_pSeria->SetText("");
		m_pNumber->SetText("");
    }

	m_cepNumber->SetText("");
	m_autoNmb->SetText(oldAutoNmb);
	m_Model->SetText(oldModel);
	m_Body->SetText(oldBody);
	m_Motor->SetText(oldMotor);

	m_Tarif[6]->SetText("0");
	SetVal();
	SetDlgItemText(HWindow, AUX_TEXT, smsg);
}

void BELGREEN::Avaria()
{
//    BELGREEN_AV* dlg = new BELGREEN_AV(this);
//    if(GetModule()->ExecDialog(dlg) == IDOK) {
//    }
}

void BELGREEN::FindByAutoNmb(RTMessage msg)
{
	FindAutoNmb();
}

void BELGREEN::FindByNmb(RTMessage msg)
{
	FindSN();
}

void BELGREEN::FindByFIO(RTMessage msg)
{
    FindName();
}

void BELGREEN::WMCtlColor(RTMessage msg)
{
	HDC hdcChild = (HDC)msg.WParam;            /* child-window display context */
	HWND hwndChild = (HWND) LOWORD(msg.LParam); /* handle of child window       */
	int nCtlType = (int) HIWORD(msg.LParam);    /* type of control              */

	if (GetWindowWord(hwndChild, GWW_ID) == 547) 
		SetTextColor(hdcChild, RGB(255, 0, 0));

	TDialog::WMCtlColor(msg);
}

static int ExpFunc(TOpenBase* FileDb, TOpenBase* Db, void* Data)
{
	return 1;
}

void BELGREEN::Export(PTWindowsObject Parent)
{
	Priod_Params* data = new Priod_Params;
	strcpy(data->filename, BELGREENName);
	if(GetApplicationObject()->ExecDialog(new getDate(Parent, &data->m, &data->y, &data->dm, data->filename)) == IDOK) {
		long Error = ExportData(BELGREENName, BELGREENFields, CNT_BELGREEN_FLDS, ExpFunc, 0, data->filename, data->m, data->y, data->dm, 22, 11);
		sprintf(data->filename, GetRCStr(Error > 0 ? 621: 622), labs(Error));
		//if(Error <= 0) unlink(data->filename);
		MessageBox(Parent->HWindow, data->filename, "", 0);
	}
	delete data;
}

static int ImpFunc(TOpenBase* FileDb, TOpenBase* Db, void* Data)
{
	if(FileDb->GetDouble(16) != Db->GetDouble(16))
	{
		DynStr msg;
		sprintf(msg, "%s/%5.0f\nБыло %s %5.0f\nСтало %s %5.0f\n$%s", Db->GetString(0).s, Db->GetDouble(1),
		Db->GetString(6).s, Db->GetDouble(16), FileDb->GetString(6).s, FileDb->GetDouble(16), GetRCStr(637));
		if(MessageBox(GetApplicationObject()->MainWindow->HWindow, msg, "!", MB_ICONQUESTION | MB_YESNO) != IDYES)
		{
			*strchr(msg, '$') = 0;
			((Array*)Data)->add(*new String(msg));
			return 0;
        }

    }
	return 1;
}


void BELGREEN::Import()
{
	Array list(100);
	list.ownsElements(TRUE);
	char list1[] = {2, 3, 24, 41, 0};
	char list2[] = {2, 3, 24, 41, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 34, 35, 36, 37, 0};
	long Result = ImportData(BELGREENName, BELGREENFields, CNT_BELGREEN_FLDS, ImpFunc, &list, 22, 24, BELGREEN_BAD, list1, list2);
	if(Result) {
		MessageBox(GetApplicationObject()->MainWindow->HWindow, GetRCStr(700+Result), "!", MB_ICONEXCLAMATION);
	}
	ShowArray(list);
}

void BELGREEN::CreateNextPolis(int State)
{
	if(!SavePolis()) return;
	char buff[32];
	m_Seria->GetText(buff, sizeof buff);
	m_pSeria->SetText(buff);
	m_Number->GetText(buff, sizeof buff);
	m_pNumber->SetText(buff);
	m_Number->SetText("");
	m_DupType1->SetSelIndex(State);
	if(State == 1)
	{
		EnableWindow(m_Seria->HWindow, TRUE);;
		EnableWindow(m_Number->HWindow, TRUE);;
	}
    date d;
    getdate(&d);
    wsprintf(buff, "%02u.%02u.%04u", d.da_day, d.da_mon, d.da_year);
    m_RegDate->SetText(buff);
}

void BELGREEN::ReplacePolis(RTMessage msg)
{
	CreateNextPolis(0);
}

void BELGREEN::DupPolis(RTMessage msg)
{
	CreateNextPolis(1);
}


void ToRubles(double& Value);

void ReplaceCurrsImpl(TEdit* m_PayDate, char* OldCurr, TEdit* m_Pay1, TComboBox* m_Curr1, int IsUr, int RoundMode)
{
		char dt[15];
		m_PayDate->GetText(dt, sizeof dt);
		DATE payDate = GD(dt);
		if(!payDate) return;
		wsprintf(dt, "%s_NAL", OldCurr);
		double Curr1 = GetRateCurrency(payDate, dt);
		m_Curr1->GetText(dt, sizeof dt);
		int isRub = strcmp(dt, "RUR")==0;
		strcat(dt, "_NAL");
		double Curr2 = GetRateCurrency(payDate, dt);
		m_Curr1->GetText(OldCurr, sizeof OldCurr);
		if(Curr1 > 0 && Curr2 > 0) {
			m_Pay1->GetText(dt, sizeof dt);
			double summ = atof(dt) * Curr1 / Curr2;
			if(!IsUr && !strcmp("BRB", OldCurr)) {
			    ToRubles(summ);
			}
			else {
				summ = long(summ * 100 + 0.005) / 100. + 1e-5;
			}
			sprintf(dt, RoundMode == 1 && isRub ? "%0.0f" : IsUr ? "%0.2f" : "%0.0f", summ);
			//sprintf(dt, "%0.5f", summ);
			m_Pay1->SetText(dt);
		}
}

void ReplaceCurrs(TEdit* m_PayDate, char* OldCurr, TEdit* m_Pay1, TComboBox* m_Curr1, int IsUr)
{
	ReplaceCurrsImpl(m_PayDate, OldCurr, m_Pay1, m_Curr1, IsUr, 0);
}

void BELGREEN::ChangeCurrency(RTMessage msg)
{
	if(msg.LP.Hi == CBN_SELCHANGE && *OldCurr) {
		if(*BaseSumm)
		{
			m_Pay1->SetText(BaseSumm);
			strcpy(OldCurr, "EUR");
        }
		ReplaceCurrsImpl(m_PayDate, OldCurr, m_Pay1, m_Curr1, m_InsUr_->GetCheck(), 1);
	}
}

void BELGREEN::Fiz(RTMessage msg)
{
	m_Beznal->SetCheck(0);
}

void BELGREEN::Ur(RTMessage msg)
{
	m_Beznal->SetCheck(1);
}

void BELGREEN::IP(RTMessage msg)
{
	m_Beznal->SetCheck(0);
}
