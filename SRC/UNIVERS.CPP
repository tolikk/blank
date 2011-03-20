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
#include <radiobut.h>
#include <combobox.h>
#include <strax.h>
#include <help.h>
#include <univers.h>
#include <impexp.h>
#include <date.h>

#define STATE_NORMAL "0"
#define STATE_BAD "1"

double GetRateCurrency(DATE payDate, char* CurrencyField);
int auxFindLike(const char* TableName, char* FldName, char* Maska, Array& Strings, TDialog* Dlg, char* ResStr, int IsDigit, char** CheckFld);
const char* GetKopeyka(char* Curr, double Val);
const char* GetCurrName(char* Curr, long Val);
void AddBufDate(char*&dest, char* date, int flag = 0);
int IsCompany(const char* s);
int _IsValidPolisNumber(const char* SECTION, _str s, long n, char* INI, char* Agent);
char* GetSummaText(double Value, char* Valuta, char* Kop = NULL);
void AlignCombo(TComboBox*);
void SetAgent(char* ACode, TOpenBase& agents, PTComboBox AgName, int);
void _FillAgents(TOpenBase* agents, TComboBox* AgName, int fld_index);
DATE aux_incDate(DATE startdate, int inc1, int value);
int getDurationType(char* str, char** = 0);
void FillPlacement3(TComboBox* PlacementCombo, const char* key_mask, const char* Sect, int IsDiv, int Mul, int StrKey, const char* ININame);
void FillPlacement2(TComboBox* PlacementCombo, const char* key_mask, const char* Sect, int IsDiv, int Mul, const char* ININame);
void InitVal(TComboBox* cb);
void InitVal2(TComboBox* cb);
void DblToStr(char* str, double V, int Digit = 2);
extern DATE GD(char* str, int = 0);
void DateFBToStr(TOpenBase* base, int field, char* buffer);
void SetDateToBuffer(char* Buffer, DATE date);

char* dw_to_str(DWORD val)
{
	static char str[5];
	strncpy(str, (char*)&val, 4);
    return str;
}

DWORD str_to_dw(char* str)
{
	DWORD val = 0;
	strncpy((char*)&val, str, 4);
	return val;
}

char* UNIVERSAL_SECT = "UNIVERSAL";

char* FIELDS[] = {
"Seria",
"Number",
"PrevS",
"PrevN",
"Vid",
"FU",
"RegDate",
"StartDate",
"Another",
"From",
"To",
"Period",
"Insurer",
"InsAddr",
"InsOther",
"InsObj",
"InsPcnt",
"Zastr",
"Vigod",
"VigodAddr",
"LizoZast",
"Terr",
"PlaceIns",
"InsEvents",
"PropertyIns",
"Variant",
"RealSum",
"RealCurr",
"InsSum",
"InsCur",
"Fransh",
"Wait",
"PremSum",
"PremCur",
"Feer",
"FeeSum",
"FeeCur",
"FeeDate",
"RepDate",
"FeeDoc",
"FeeTyp",
"SrokPay",
"OtherCond",
"Agent",
"AgType",
"AgPcnt",
"State"
};

const char* GetRCStr(int strID);

enum {
_SERIA,
_NUMBER,
_PREVSERIA,
_PREVNUMBER,
_dwVid,
_FU,
_RegDate,
_StartDate,
_Another,
_From,
_To,
_Period,
_Insurer,
_InsAddr,
_InsOther,
_InsObj,
_InsPcnt,
_Zastr,
_Vigod,
_VigodAddr,
_LizoZast,
_Terr,
_PlaceIns,
_InsEvents,
_PropertyIns,
_Variant,
_RealSum,
_RealCurr,
_InsSum,
_InsCur,
_Fransh,
_Wait,
_PremSum,
_PremCur,
_Feer,
_FeeSum,
_FeeCur,
_FeeDate,
_RepDate,
_FeeDoc,
_FeeTyp,
_SrokPay,
_OtherCond,
_Agent,
_AgentFU,
_AgentPcnt,
_State
};

char* UNIVERSAL_INI = NULL;
char* TABLE = "\\univers";

UNIVERSAL::UNIVERSAL(PTWindowsObject p)
		 : Dlg_base(p, "UNIVERSAL", TABLE, FIELDS, sizeof(FIELDS) / sizeof(*FIELDS)),
       	   agents(AgentDBName, AgentFields, 2),
		   dlg1(this, &buffer1),
  		   dlg2(this, &buffer2)
{
	memset(&buffer, 0, sizeof buffer);
	memset(&buffer1, 0, sizeof buffer1);
	memset(&buffer2, 0, sizeof buffer2);
	TransferBuffer = &buffer;
   
	(Seria = new TComboBox(this, 103, 0))->DisableTransfer();
	Number = new NumberEdit (this, 104, sizeof(buffer.Number), 0);
	(Vid = new TComboBox(this, 100, 0))->DisableTransfer();

	Fiz = new TRadioButton(this, 101, 0);
	Ur = new TRadioButton(this, 102, 0);

	RegDate = new TDateEdit (this, 105, sizeof(buffer.RegDate));
	StartDate = new TDateEdit (this, 106, sizeof(buffer.StartDate));
	Another = new INIEdit (ININame, "UNI", this, 107, sizeof(buffer.Another), "Analog%d", "");

	(Agent = new TComboBox(this, 112, 0))->DisableTransfer();
	AgentFU = new UrFizich(this, 113, 5);
	AgentPcnt = new NumberEdit(this, 114, 7, 1);

    //INI FIle
	char strBuffer[48];
	GetPrivateProfileString(UNIVERSAL_SECT, "INI", ININame, strBuffer, sizeof strBuffer, ININame);
	UNIVERSAL_INI = strdup(strBuffer);

	dlg2.dlg1 = &dlg1;
}

UNIVERSAL::~UNIVERSAL()
{
}

void UNIVERSAL::SetupWindow()
{
	TDialog::SetupWindow();

    RECT r;
	GetWindowRect(GetDlgItem(HWindow, 1000), &r);
	ScreenToClient(HWindow, (POINT*)&r);

	dlg1.Create();
	dlg2.Create();
	dlg1.EnableKBHandler();
	dlg2.EnableKBHandler();
	SetWindowPos(dlg1.HWindow, GetDlgItem(HWindow, 110), 0, r.top, 0, 0, SWP_NOSIZE);
	SetWindowPos(dlg2.HWindow, GetDlgItem(HWindow, 110), 0, r.top, 0, 0, SWP_NOSIZE | SWP_HIDEWINDOW);

	FillPlacement3(Vid, "Vid%d", UNIVERSAL_SECT, 1, 1, 1, UNIVERSAL_INI);
	Vid->InsertString("ИСПОРЧЕН", 0);
	SendMessage(Vid->HWindow, CB_SETITEMDATA, 0, str_to_dw("0000"));

	FillPlacement2(Seria, "Seria%d", UNIVERSAL_SECT, 0, 0, UNIVERSAL_INI);
	_FillAgents(&agents, Agent, -1);
	AlignCombo(Agent);

	Dlg_base::SetupWindow();
}

void UNIVERSAL::GetDataFromBase()
{
   double N;
   short Shrt;
   DynStr buff;

   memset(&buffer, 0, sizeof buffer);
   (*base)(buffer.Seria, sizeof buffer.Seria, _SERIA);
   Seria->SetSelIndex(-1);
   Seria->SetText(buffer.Seria);

   (*base)(N, _NUMBER);
   ::DblToStr(buffer.Number, N, 0);

   (*base)(buff, buff._sizeof(), _FU);
   buffer.Fiz = strcmp("F", buff) == 0;
   buffer.Ur = !buffer.Fiz;

   (*base)(buff, buff._sizeof(), _dwVid);
   Vid->SetSelIndex(-1);
   for(int i = 0; i < Vid->GetCount(); i++)
   {
		if(SendMessage(Vid->HWindow, CB_GETITEMDATA, i, 0) == str_to_dw(buff)) Vid->SetSelIndex(i);
   }

   ::DateFBToStr(base, _RegDate, buffer.RegDate);
   ::DateFBToStr(base, _StartDate, buffer.StartDate);

   (*base)(buffer.Another, sizeof buffer.Another, _Another);

   ::DateFBToStr(base, _From, buffer1.From);
   ::DateFBToStr(base, _To, buffer1.To);

   (*base)(buffer.Agent, sizeof buffer.Agent, _Agent);
   (*base)(buffer.AgentFU, sizeof buffer.AgentFU, _AgentFU);
   strcpy(buffer.AgentFU, *buffer.AgentFU == 'F' ? "Физ" : "Юр");
   (*base)(N, _AgentPcnt);
   ::DblToStr(buffer.AgentPcnt, N, 3);

   (*base)(buffer1.Period, sizeof buffer1.Period, _Period);
   dlg1.Period->SetSelIndex(-1);
   dlg1.Period->SetText(buffer1.Period);
   (*base)(buffer1.Insurer, sizeof buffer1.Insurer, _Insurer);
   (*base)(buffer1.InsAddr, sizeof buffer1.InsAddr, _InsAddr);
   (*base)(buffer1.InsOther, sizeof buffer1.InsOther, _InsOther);
   (*base)(buffer1.InsObj, sizeof buffer1.InsObj, _InsObj);
   (*base)(N, _InsPcnt);
   ::DblToStr(buffer1.InsPcnt, N, 5);
   (*base)(buffer1.Zastr, sizeof buffer1.Zastr, _Zastr);
   (*base)(buffer1.Vigod, sizeof buffer1.Vigod, _Vigod);
   (*base)(buffer1.VigodAddr, sizeof buffer1.VigodAddr, _VigodAddr);
   (*base)(buffer1.LizoZast, sizeof buffer1.LizoZast, _LizoZast);
   (*base)(Shrt, _Terr);
   dlg1.Territory->SetSelIndex(-1);
   for(i = 0; i < dlg1.Territory->GetCount(); i++)
   {
		if(SendMessage(dlg1.Territory->HWindow, CB_GETITEMDATA, i, 0) == Shrt) dlg1.Territory->SetSelIndex(i);
   }
   (*base)(buffer1.PlaceIns, sizeof buffer1.PlaceIns, _PlaceIns);
   (*base)(buffer1.InsEvents, sizeof buffer1.InsEvents, _InsEvents);
   (*base)(buffer1.PropertyIns, sizeof buffer1.PropertyIns, _PropertyIns);
   (*base)(buffer1.Variant, sizeof buffer1.Variant, _Variant);

   (*base)(N, _RealSum);
   ::DblToStr(buffer2.RealSum, N, 2);
   (*base)(buffer2.RealCurr, sizeof buffer2.RealCurr, _RealCurr);
   (*base)(N, _InsSum);
   ::DblToStr(buffer2.InsSum, N, 2);
   (*base)(buffer2.InsCur, sizeof buffer2.InsCur, _InsCur);
   (*base)(buffer2.Fransh, sizeof buffer2.Fransh, _Fransh);
   (*base)(buffer2.Wait, sizeof buffer2.Wait, _Wait);
   (*base)(N, _PremSum);
   ::DblToStr(buffer2.PremSum, N, 2);
   (*base)(buffer2.PremCur, sizeof buffer2.PremCur, _PremCur);
   (*base)(buffer2.Feer, sizeof buffer2.Feer, _Feer);
   (*base)(N, _FeeSum);
   ::DblToStr(buffer2.FeeSum, N, 2);
   (*base)(buffer2.FeeCur, sizeof buffer2.FeeCur, _FeeCur);
   ::DateFBToStr(base, _FeeDate, buffer2.FeeDate);
   ::DateFBToStr(base, _RepDate, buffer2.RepDate);
   if(!strcmp(buffer2.RepDate, buffer2.FeeDate)) *buffer2.RepDate = 0;
   (*base)(buffer2.FeeDoc, sizeof buffer2.FeeDoc, _FeeDoc);
   (*base)(buff, buff._sizeof(), _FeeTyp);
   buffer2.FeeTypBeznal = buff[0] == 'B';
   (*base)(buffer2.SrokPay, sizeof buffer2.SrokPay, _SrokPay);
   (*base)(buffer2.OtherCond, sizeof buffer2.OtherCond, _OtherCond);

   dlg2.RealCurr->SetSelIndex(-1);
   dlg2.InsCurr->SetSelIndex(-1);
   dlg2.PremCur->SetSelIndex(-1);
   dlg2.FeeCur->SetSelIndex(-1);
   dlg2.RealCurr->SetText(buffer2.RealCurr);
   dlg2.InsCurr->SetText(buffer2.InsCur);
   dlg2.PremCur->SetText(buffer2.PremCur);
   dlg2.FeeCur->SetText(buffer2.FeeCur);
   SetAgent(buffer.Agent, agents, Agent, 0);

   dlg1.TransferData(TF_SETDATA);
   dlg2.TransferData(TF_SETDATA);
   dlg2.UpdateSummTexts();

   InitGUI();

   Dlg_base::GetDataFromBase();
}

int validField(TWindow* wnd, char type, int canBeEmpty)
{
	DynStr data;
	SendMessage(wnd->HWindow, WM_GETTEXT, data._sizeof(), (DWORD)(char*)data);
	if(!canBeEmpty && !*data) {
		SetFocus(wnd->HWindow);
		return 0;
	}
	if(*data) {
		if(type == 'D') {
			if(!GD(data))
			{
				SetFocus(wnd->HWindow);
				return 0;
            }
		}
	}
	return 1;
}

int UNIVERSAL::SetDataToBase()
{
	Dlg_base::SetDataToBase();
	dlg1.TransferData(TF_GETDATA);
	dlg2.TransferData(TF_GETDATA);

	if(!validField(Vid, 'S', 0)) return 0;
	if(!validField(Seria, 'S', 0)) return 0;
	if(!validField(Number, 'N', 0)) return 0;
	if(!validField(RegDate, 'D', 0)) return 0;
	if(!validField(Agent, 'S', 0)) return 0;
	if(!validField(AgentFU, 'S', 0)) return 0;

	Seria->GetText(buffer.Seria, sizeof buffer.Seria);
	buffer.dwVid = SendMessage(Vid->HWindow, CB_GETITEMDATA, Vid->GetSelIndex(), 0);
	long acode = SendMessage(Agent->HWindow, CB_GETITEMDATA, Agent->GetSelIndex(), 0);
	memmove(buffer.Agent, &acode, 4);
	buffer.Agent[4] = 0;

    //Проверка диапазона номеров
	_str s;
	strcpy(s.s, buffer.Seria);
	if(!_IsValidPolisNumber(UNIVERSAL_SECT, s, atol(buffer.Number), UNIVERSAL_INI, buffer.Agent))
	{
		MessageBox(HWindow, GetRCStr(711), "!", MB_ICONSTOP);
		return 0;        
	}

	int IsBad = Vid->GetSelIndex() < 1;
	if(IsBad && MessageBox(HWindow, GetRCStr(506), "Отвечай поскорей", MB_YESNO | MB_ICONINFORMATION | MB_SYSTEMMODAL) == IDNO) return 0;

	if(!IsBad)
    {
		dlg1.Period->GetText(buffer1.Period, sizeof buffer1.Period);
		buffer1.Terr = SendMessage(dlg1.Territory->HWindow, CB_GETITEMDATA, dlg1.Territory->GetSelIndex(), 0);

		dlg2.RealCurr->GetText(buffer2.RealCurr, sizeof buffer2.RealCurr);
		dlg2.InsCurr->GetText(buffer2.InsCur, sizeof buffer2.InsCur);
		dlg2.PremCur->GetText(buffer2.PremCur, sizeof buffer2.PremCur);
		dlg2.FeeCur->GetText(buffer2.FeeCur, sizeof buffer2.FeeCur);

		//VALIDATE
		//////////////////////////////////////////////////////////////////////////////////////
		if(!validField(StartDate, 'D', 0)) return 0;
		if(!buffer.Fiz && !buffer.Ur)
		{
			SetFocus(Fiz->HWindow);
			return 0;
		}
		if(!validField(dlg1.DtFrom, 'D', 0)) return 0;
		if(!validField(dlg1.DtTo, 'D', 0)) return 0;
		if(!validField(dlg1.Period, 'S', 0)) return 0;
		if(!validField(dlg1.Insurer, 'S', 0)) return 0;
		if(!validField(dlg1.InsurerAddr, 'S', 0)) return 0;
		//InsurerOther;
		if(!validField(dlg1.InsObject, 'S', 0)) return 0;
		if(!validField(dlg1.InsPercent, 'N', 0)) return 0;
		if(!validField(dlg1.ZastraxLizo, 'S', 1)) return 0;
		if(!validField(dlg1.Vigoder, 'S', 0)) return 0;
		//VigoderAddr;
		if(!validField(dlg1.LizoWhosOtfvet, 'S', 1)) return 0;
		if(!validField(dlg1.Territory, 'S', 0)) return 0;
		if(!validField(dlg1.PlaceInsur, 'S', 1)) return 0;
		if(!validField(dlg1.SetInsEvents, 'S', 0)) return 0;
		if(!validField(dlg1.PropertyIns, 'S', 1)) return 0;
		if(!validField(dlg1.Variant, 'S', 0)) return 0;

		if(!validField(dlg2.RealSum, 'N', 0)) return 0;
		if(!validField(dlg2.RealCurr, 'S', 0)) return 0;
		if(!validField(dlg2.InsSum, 'N', 0)) return 0;
		if(!validField(dlg2.InsCurr, 'S', 0)) return 0;
		if(!validField(dlg2.Fransh, 'S', 0)) return 0;
		if(!validField(dlg2.Wait, 'S', 1)) return 0;
		if(!validField(dlg2.PremSum, 'N', 0)) return 0;
		if(!validField(dlg2.PremCur, 'S', 0)) return 0;
		if(!validField(dlg2.Feer, 'S', 0)) return 0;
		if(!validField(dlg2.FeeSum, 'N', 0)) return 0;
		if(!validField(dlg2.FeeCur, 'S', 0)) return 0;
		if(!validField(dlg2.FeeDate, 'D', 0)) return 0;
		if(!validField(dlg2.RepDate, 'D', 1)) return 0;
		if(!validField(dlg2.FeeDoc, 'S', buffer2.FeeTypBeznal)) return 0;
		//   PTCheckBox	FeeTypBeznal;
		//   PTEdit 		SrokPay;
		//   PTEdit 		OtherCond;
		//////////////////////////////////////////////////////////////////////////////////////
		//Проверка лица
		if(IsCompany(buffer1.Insurer) && buffer.Fiz) {
		if(MessageBox(HWindow, GetRCStr(608), buffer1.Insurer, MB_YESNO | MB_ICONSTOP) == IDNO)
			return 0;
		}
		if(!IsCompany(buffer1.Insurer) && !buffer.Fiz) {
		if(MessageBox(HWindow, GetRCStr(609), buffer1.Insurer, MB_YESNO | MB_ICONSTOP) == IDNO)
			return 0;
		}

		//STORE
		(*base)(_FU, buffer.Fiz ? "F" : "U");
		(*base)(_StartDate, GD(buffer.StartDate));
		(*base)(_Another, buffer.Another);

		(*base)(_From, GD(buffer1.From));
		(*base)(_To, GD(buffer1.To));
		(*base)(_Period, buffer1.Period);
		(*base)(_Insurer, buffer1.Insurer);
		(*base)(_InsAddr, buffer1.InsAddr);
		(*base)(_InsOther, buffer1.InsOther);
		(*base)(_InsObj, buffer1.InsObj);
		(*base)(_InsPcnt, atof(buffer1.InsPcnt));
		(*base)(_Zastr, buffer1.Zastr);
		(*base)(_Vigod, buffer1.Vigod);
		(*base)(_VigodAddr, buffer1.VigodAddr);
		(*base)(_LizoZast, buffer1.LizoZast);
		(*base)(_Terr, (short)buffer1.Terr);
		(*base)(_PlaceIns, buffer1.PlaceIns);
		(*base)(_InsEvents, buffer1.InsEvents);
		(*base)(_PropertyIns, buffer1.PropertyIns);
		(*base)(_Variant, buffer1.Variant);

		(*base)(_RealSum, atof(buffer2.RealSum));
		(*base)(_RealCurr, buffer2.RealCurr);
		(*base)(_InsSum, atof(buffer2.InsSum));
		(*base)(_InsCur, buffer2.InsCur);
		(*base)(_Fransh, buffer2.Fransh);
		(*base)(_Wait, buffer2.Wait);
		(*base)(_PremSum, atof(buffer2.PremSum));
		(*base)(_PremCur, buffer2.PremCur);
		(*base)(_Feer, buffer2.Feer);
		(*base)(_FeeSum, atof(buffer2.FeeSum));
		(*base)(_FeeCur, buffer2.FeeCur);
		DATE FeeDate = GD(buffer2.FeeDate);
		DATE RepDate = FeeDate;
		if(*buffer2.RepDate)
        {
			RepDate = GD(buffer2.RepDate);
        }
		(*base)(_FeeDate, FeeDate);
		(*base)(_RepDate, RepDate);
		(*base)(_FeeDoc, buffer2.FeeDoc);
		(*base)(_FeeTyp, buffer2.FeeTypBeznal ? "B" : "N");
		(*base)(_SrokPay, buffer2.SrokPay);
		(*base)(_OtherCond, buffer2.OtherCond);
		(*base)(_AgentPcnt, atof(buffer.AgentPcnt));
		(*base)(_State, STATE_NORMAL);
	}
	else
	{
		(*base)(_State, STATE_BAD);
	}
	(*base)(_dwVid, dw_to_str(buffer.dwVid));
	(*base)(_SERIA, buffer.Seria);
	(*base)(_NUMBER, atof(buffer.Number));
	(*base)(_RegDate, GD(buffer.RegDate));
	(*base)(_Agent, buffer.Agent);
	(*base)(_AgentFU, buffer.AgentFU[0] == 'Ю' ? "U" : "F");
	return 1;
}

void UNIVERSAL::ChAgent(RTMessage msg)
{
   int IsUr;
   if(msg.LP.Hi == CBN_SELCHANGE) {
	   AgentPcnt->SetText(GetAgPercent(SendMessage(Agent->HWindow, CB_GETITEMDATA, Agent->GetSelIndex(), 0), UNIVERSAL_TBL, IsUr));
	   AgentFU->SetText(IsUr ? "Юр" : "Физ");
   }
}

void UNIVERSAL::Clear()
{
	ClearFields();

	Fiz->SetCheck(1);
	Ur->SetCheck(0);
	Seria->SetSelIndex(0);
	Number->SetText("");
	date d;
	getdate(&d);

	DynStr _buff;
	wsprintf(_buff, "%02u.%02u.%04u", d.da_day, d.da_mon, d.da_year);
	RegDate->SetText(_buff);

	if(GetPrivateProfileString(UNIVERSAL_SECT, "DefAgent", "", buffer.Agent, sizeof buffer.Agent, UNIVERSAL_INI))
	{
	  SetAgent(buffer.Agent, agents, Agent, 0);
      TMessage msg;
      msg.LP.Hi = CBN_SELCHANGE;
	  ChAgent(msg);
   }
}

void UNIVERSAL::ClearFields()
{
	Another->SetText("");
	StartDate->SetText("");

	char class_name[32];
	HWND Child = GetWindow(dlg1.HWindow, GW_CHILD);
	for(;Child;) {
		::GetClassName(Child, class_name, sizeof class_name);
		if(!strcmpi(class_name, "EDIT"))
		SendMessage(Child, WM_SETTEXT, 0, (long)"");
		if(!strcmpi(class_name, "COMBOBOX"))
		SendMessage(Child, CB_SETCURSEL, -1, 0);
		Child = GetWindow(Child, GW_HWNDNEXT);
	}
	Child = GetWindow(dlg2.HWindow, GW_CHILD);
	for(;Child;) {
		::GetClassName(Child, class_name, sizeof class_name);
		if(!strcmpi(class_name, "EDIT"))
		SendMessage(Child, WM_SETTEXT, 0, (long)"");
		if(!strcmpi(class_name, "COMBOBOX"))
		SendMessage(Child, CB_SETCURSEL, -1, 0);
		Child = GetWindow(Child, GW_HWNDNEXT);
	}
}

void UNIVERSAL::ShowInfo(RTMessage)
{
	ShowWindow(dlg2.HWindow, SW_HIDE);
	ShowWindow(dlg1.HWindow, SW_SHOW);
}

void UNIVERSAL::SetStartDate(RTMessage)
{
	DynStr buff;
	RegDate->GetText(buff, buff._sizeof());
	StartDate->SetText(buff);
}

void UNIVERSAL::ShowPays(RTMessage)
{
	ShowWindow(dlg1.HWindow, SW_HIDE);
	ShowWindow(dlg2.HWindow, SW_SHOW);
}

void UNIVERSAL::Invalid(RTMessage)
{
	Vid->SetSelIndex(0);
	ClearFields();
}

void UNIVERSAL::ChangeVid(RTMessage)
{
	InitGUI();
}

void UNIVERSAL::InitGUI()
{
	long VidId = SendMessage(Vid->HWindow, CB_GETITEMDATA, Vid->GetSelIndex(), 0);
	Another->SetININame(UNIVERSAL_INI);
	Another->SetSectionName(UNIVERSAL_SECT);
	
	dlg1.InitGUI(VidId);
	dlg2.InitGUI(VidId);
}

/////////////////////////

UNIVERSAL1::UNIVERSAL1(PTWindowsObject parent, UNIVERSAL1_DATA* data)
		  : TDialog(parent, "UNIVERSAL1"),
			buffer(data)
{
	TransferBuffer = buffer;

	DtFrom = new TDateEdit(this, 112, sizeof buffer->From);
	DtTo = new TDateEdit(this, 113, sizeof buffer->To);
	Insurer = new INIEdit(UNIVERSAL_INI, UNIVERSAL_SECT, this, 100, sizeof buffer->Insurer, "Owner%d", "", 1);
	strcpy(Insurer->DivString, " ");
	InsurerAddr = new INIEdit(UNIVERSAL_INI, UNIVERSAL_SECT, this, 101, sizeof buffer->InsAddr, "Addr%d", "");
	InsurerOther = new TEdit(this, 102, sizeof buffer->InsOther);
	InsObject = new TEdit(this, 104, sizeof buffer->InsObj);
	InsPercent = new NumberEdit(this, 103, sizeof buffer->InsPcnt, 1, "Percent%d");
	ZastraxLizo = new TEdit(this, 105, sizeof buffer->LizoZast);
	Vigoder = new INIEdit(UNIVERSAL_INI, UNIVERSAL_SECT, this, 106, sizeof buffer->Vigod, "Owner%d", "", 1);
	VigoderAddr = new TEdit(this, 107, sizeof buffer->VigodAddr);
	LizoWhosOtfvet = new TEdit(this, 108, sizeof buffer->LizoZast);
	(Territory = new TComboBox(this, 109, 0))->DisableTransfer();
	PlaceInsur = new TEdit(this, 110, sizeof buffer->PlaceIns);
	(Period = new TComboBox(this, 111, 0))->DisableTransfer();
	SetInsEvents = new INIEdit(UNIVERSAL_INI, UNIVERSAL_SECT, this, 114, sizeof buffer->InsEvents, "", "");
	PropertyIns = new TEdit(this, 115, sizeof buffer->PropertyIns);
	Variant = new INIEdit(UNIVERSAL_INI, UNIVERSAL_SECT, this, 116, sizeof buffer->Variant, "", "");
}

UNIVERSAL1::~UNIVERSAL1()
{
}

void UNIVERSAL1::SetupWindow()
{
	TDialog::SetupWindow();
	InsurerAddr->SetININame(UNIVERSAL_INI);
	FillPlacement2(Territory, "Territory%d", UNIVERSAL_SECT, 1, 1, UNIVERSAL_INI);
	FillPlacement2(Period, "Period%d", UNIVERSAL_SECT, 1, 1, UNIVERSAL_INI);
	SetWindowLong(HWindow, GWL_EXSTYLE, GetWindowLong(HWindow, GWL_EXSTYLE) | 0x00010000L);
}

void UNIVERSAL1::SetVigoder(RTMessage)
{
	DynStr buff;
	Insurer->GetText(buff, buff._sizeof());
	Vigoder->SetText(buff);
}

void UNIVERSAL1::InitGUI(long Id)
{
	char buff[32];
	SetInsEvents->SetININame(UNIVERSAL_INI);
	Variant->SetININame(UNIVERSAL_INI);
	InsurerAddr->SetSectionName(UNIVERSAL_SECT);
	InsPercent->SetININame(UNIVERSAL_INI);

	InsPercent->SetSectionName(UNIVERSAL_SECT);
	SetInsEvents->SetSectionName(UNIVERSAL_SECT);

	sprintf(buff, "InsEvent%s_%%d", dw_to_str(Id));
	SetInsEvents->SetMask(buff);

	Variant->SetSectionName(UNIVERSAL_SECT);
	sprintf(buff, "Variant%s_%%d", dw_to_str(Id));
	Variant->SetMask(buff);
}

void UNIVERSAL1::ChangePeriod(RTMessage)
{
	char datebuf[15], PeriodName[32];
    DtFrom->GetText(datebuf, sizeof datebuf);
	Period->GetText(PeriodName, sizeof PeriodName);
    if(!*PeriodName) return;
    DATE startdate = GD(datebuf, 1);

    if(startdate) {
       DATE enddate = aux_incDate(startdate, getDurationType(PeriodName), atoi(PeriodName));
       int d, m, y;
       PXDateDecode(--enddate, &m, &d, &y);
	   sprintf(datebuf, "%02u.%02u.%04u", d, m, y);
	   DtTo->SetText(datebuf);
    }
}

/////////////////////////

UNIVERSAL2::UNIVERSAL2(PTWindowsObject parent, UNIVERSAL2_DATA* data)
		  : TDialog(parent, "UNIVERSAL2"),
			buffer(data)
{
	RealSum = new NumberEdit(this, 100, sizeof buffer->RealSum, 1);
	(RealCurr = new TComboBox(this, 101, 0))->DisableTransfer();
	InsSum = new NumberEdit(this, 102, sizeof buffer->InsSum, 1);
	(InsCurr = new TComboBox(this, 103, 0))->DisableTransfer();
	Fransh = new INIEdit(UNIVERSAL_INI, UNIVERSAL_SECT, this, 104, sizeof buffer->Fransh, "", "");
	Wait = new TEdit(this, 105, sizeof buffer->Wait);
	PremSum = new NumberEdit(this, 106, sizeof buffer->PremSum, 1);
	(PremCur = new TComboBox(this, 107, 0))->DisableTransfer();
	Feer = new TEdit(this, 108, sizeof buffer->Feer);
	FeeSum = new NumberEdit(this, 109, sizeof buffer->FeeSum, 1);
	(FeeCur = new TComboBox(this, 110, 0))->DisableTransfer();
	FeeDate = new TDateEdit(this, 111, sizeof buffer->FeeDate);
	RepDate = new TDateEdit(this, 122, sizeof buffer->RepDate);
	FeeTypBeznal = new TCheckBox(this, 112, 0);
	FeeDoc = new TEdit(this, 113, sizeof buffer->FeeDoc);
	SrokPay = new INIEdit(UNIVERSAL_INI, UNIVERSAL_SECT, this, 114, sizeof buffer->SrokPay, "PayMode%d", "");
	OtherCond = new INIEdit(UNIVERSAL_INI, UNIVERSAL_SECT, this, 115, sizeof buffer->OtherCond, "", "");

	TransferBuffer = buffer;
}

UNIVERSAL2::~UNIVERSAL2()
{
}

void UNIVERSAL2::SetupWindow()
{
	TDialog::SetupWindow();

	InitVal2(RealCurr);
	InitVal2(InsCurr);
	InitVal2(PremCur);
	InitVal2(FeeCur);
	SetWindowLong(HWindow, GWL_EXSTYLE, GetWindowLong(HWindow, GWL_EXSTYLE) | 0x00010000L);
}

void UNIVERSAL2::InitGUI(long Id)
{
	char buff[32];
	Fransh->SetININame(UNIVERSAL_INI);
	SrokPay->SetININame(UNIVERSAL_INI);
	OtherCond->SetININame(UNIVERSAL_INI);
	Fransh->SetSectionName(UNIVERSAL_SECT);
	sprintf(buff, "Fransh%s_%%d", dw_to_str(Id));
	Fransh->SetMask(buff);
	SrokPay->SetSectionName(UNIVERSAL_SECT);
	OtherCond->SetSectionName(UNIVERSAL_SECT);
	sprintf(buff, "OtherCond%s_%%d", dw_to_str(Id));
	OtherCond->SetMask(buff);
}

void UNIVERSAL2::SetFeer(RTMessage)
{
	DynStr buff;
	dlg1->Insurer->GetText(buff, buff._sizeof());
	Feer->SetText(buff);
}

void UNIVERSAL2::ChangeSumm1(RTMessage)
{
	UpdateSummTexts();
}

void UNIVERSAL2::ChangeSumm2(RTMessage)
{
	UpdateSummTexts();
}

void UNIVERSAL2::ChangeSumm3(RTMessage)
{
	UpdateSummTexts();
}

void UNIVERSAL2::ChangeSummCurr1(RTMessage)
{
	UpdateSummTexts();
}

void UNIVERSAL2::ChangeSummCurr2(RTMessage)
{
	UpdateSummTexts();
}

void UNIVERSAL2::ChangeSummCurr3(RTMessage)
{
	UpdateSummTexts();
}

void UNIVERSAL2::NB(RTMessage)
{
}

void UNIVERSAL2::UpdateSummTexts()
{
	char s[15], curr[5];
	RealSum->GetText(s, sizeof s);
	RealCurr->GetText(curr, sizeof curr);
	SetDlgItemText(HWindow, 1000, GetSummaText(atof(s), curr));

	InsSum->GetText(s, sizeof s);
	InsCurr->GetText(curr, sizeof curr);
	SetDlgItemText(HWindow, 1001, GetSummaText(atof(s), curr));

	PremSum->GetText(s, sizeof s);
	PremCur->GetText(curr, sizeof curr);
	SetDlgItemText(HWindow, 1002, GetSummaText(atof(s), curr));
}

void append(char*& dest, char* str)
{
	if(str != 0) strcat(dest, str);
	dest += strlen(dest) + 1;
}

void UNIVERSAL::Print()
{
	DynStr buff;
	
	if(!SetDataToBase()) return;
    dlg2.TransferData(TF_GETDATA);

	char* dest = new char [2600];
   	char* strdata = dest;
   	memset(dest, 0, 2500);

    append(dest, buffer.Fiz ? "X" : "");
    append(dest, buffer.Ur ? "X" : "");
	GetPrivateProfileString(UNIVERSAL_SECT, "Title0", "", buff, buff._sizeof(), UNIVERSAL_INI);
	append(dest, buff);
	GetPrivateProfileString(UNIVERSAL_SECT, "Title1", "", buff, buff._sizeof(), UNIVERSAL_INI);
	append(dest, buff);

	append(dest, buffer1.Insurer);
	strcpy(buff, buffer1.InsAddr);
	if(*buffer1.InsOther)
	{
		strcat(buff, ",");
		strcat(buff, buffer1.InsOther);
    }
	append(dest, buff);

	Vid->GetText(buff, buff._sizeof());
	append(dest, buff);
	sprintf(buff, "VidInfo%s", dw_to_str(buffer.dwVid));
	GetPrivateProfileString(UNIVERSAL_SECT, buff, "", buff, buff._sizeof(), UNIVERSAL_INI);
	append(dest, strtok(buff, "|"));
	append(dest, strtok(NULL, "|"));
	append(dest, strtok(NULL, "|"));
	append(dest, strtok(NULL, "|"));
	append(dest, strtok(NULL, "|"));
	append(dest, strtok(NULL, "|"));
	append(dest, strtok(NULL, "|"));

	append(dest, buffer1.InsObj);
	append(dest, buffer1.InsPcnt);
	append(dest, buffer1.Zastr);

	strcpy(buff, buffer1.Vigod);
	if(*buffer1.VigodAddr)
	{
		strcat(buff, ",");
		strcat(buff, buffer1.VigodAddr);
    }
	append(dest, buff);
	append(dest, buffer1.LizoZast);

	dlg1.Territory->GetText(buff, buff._sizeof());
	append(dest, buff);
	append(dest, buffer1.InsEvents);
	dlg1.Period->GetText(buff, buff._sizeof());
	strcat(dest, buff); //next will move desc

	AddBufDate(dest, buffer1.From, 1);
	AddBufDate(dest, buffer1.To, 1);
	dest += (strlen(dest) + 1);

	append(dest, buffer1.InsEvents);
	append(dest, buffer1.PropertyIns);
	append(dest, buffer1.Variant);

	sprintf(buff, "%s (%s) %s %s", buffer2.RealSum, GetSummaText(atol(buffer2.RealSum), ""), GetCurrName(buffer2.RealCurr, atol(buffer2.RealSum)), GetKopeyka(buffer2.RealCurr, atof(buffer2.RealSum)));
	append(dest, buff);
	sprintf(buff, "%s (%s) %s %s", buffer2.InsSum, GetSummaText(atol(buffer2.InsSum), ""), GetCurrName(buffer2.InsCur, atol(buffer2.InsSum)), GetKopeyka(buffer2.InsCur, atof(buffer2.InsSum)));
	append(dest, buff);

	append(dest, buffer2.Fransh);
	append(dest, buffer2.Wait);

	sprintf(buff, "%s (%s) %s %s", buffer2.PremSum, GetSummaText(atol(buffer2.PremSum), ""), GetCurrName(buffer2.PremCur, atol(buffer2.PremSum)), GetKopeyka(buffer2.PremCur, atof(buffer2.PremSum)));
	append(dest, buff);
	append(dest, buffer2.Feer);

	sprintf(buff, "%s (%s) %s %s", buffer2.FeeSum, GetSummaText(atol(buffer2.FeeSum), ""), GetCurrName(buffer2.FeeCur, atol(buffer2.FeeSum)), GetKopeyka(buffer2.FeeCur, atof(buffer2.FeeSum)));
	append(dest, buff);
	append(dest, buffer2.FeeTypBeznal ? "" : "X");
	append(dest, buffer2.FeeTypBeznal ? "X" : "");

	strcpy(buff, buffer2.FeeDate);
	if(buffer2.FeeTypBeznal)
	{
    	strcat(buff, ", ");
		strcat(buff, buffer2.FeeDoc);
	}
	append(dest, buff);

	strcpy(dest, buffer2.SrokPay);
	AddBufDate(dest, buffer.StartDate, 1);
	dest += (strlen(dest) + 1);

	append(dest, buffer2.OtherCond);

	strcpy(dest, buffer.Another);
	AddBufDate(dest, buffer.RegDate, 1);
	dest += (strlen(dest) + 1);

    ((TRulerWin*)Parent)->SetValues(strdata);
    Dlg_base::Print1();
    delete dest;
}

void UNIVERSAL::FindSN(RTMessage)
{
	char buff[32];
    Seria->GetText(buff, sizeof buff);
    (*base)(0, buff);
    Number->GetText(buff, sizeof buff);
	(*base)(1, atof(buff));
    if(!PXSrchKey(base->getTblHandle(), base->getRecHandle(), 2, SEARCHFIRST))
    {
       base->Get();
       GetDataFromBase();
    }
    else
       MessageBox(HWindow, GetRCStr(556), "Ошибка поиска", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
}

void FindByField(UNIVERSAL* dlg, int fldNumber, TEdit* Ctrl)
{
	Array Strings(10, 0, 10);
    char str[70];
    char maska[20];
	Ctrl->GetText(maska, sizeof maska);
	int Result = auxFindLike(TABLE, FIELDS[fldNumber], maska, Strings, dlg, str, 0, 0);
    if(Result != 1) return;
	(*dlg->getBase())(0, strtok(str, "/"));
	(*dlg->getBase())(1, atof(strtok(0, " ")));
	if(!PXSrchKey(dlg->getBase()->getTblHandle(), dlg->getBase()->getRecHandle(), 2, SEARCHFIRST)) {
	   dlg->getBase()->Get();
       dlg->GetDataFromBase();
    }
    else
       MessageBox(dlg->HWindow, "Ошибка", "Ошибка поиска", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
}


void UNIVERSAL1::FindName(RTMessage)
{
	FindByField((UNIVERSAL*)Parent, _Insurer, Insurer);
}

void UNIVERSAL1::FindAddr(RTMessage)
{
	FindByField((UNIVERSAL*)Parent, _InsAddr, InsurerAddr);
}

void UNIVERSAL1::FindObj(RTMessage)
{
	FindByField((UNIVERSAL*)Parent, _InsObj, InsObject);
}

void UNIVERSAL2::FindOtherCond(RTMessage)
{
	FindByField((UNIVERSAL*)Parent, _OtherCond, OtherCond);
}

void UNIVERSAL2::Pay1(RTMessage)
{
	GeneratePayments(1, 0);
}

void UNIVERSAL2::Pay2(RTMessage)
{
	GeneratePayments(2, 0);
}

void UNIVERSAL2::Pay3(RTMessage)
{
	GeneratePayments(3, 0);
}

void UNIVERSAL2::Pay4(RTMessage)
{
	GeneratePayments(4, 0);
}

void UNIVERSAL2::PayAndSumm1(RTMessage)
{
	GeneratePayments(2, 1);
}

void UNIVERSAL2::PayAndSumm2(RTMessage)
{
	GeneratePayments(3, 1);
}

void UNIVERSAL2::PayAndSumm3(RTMessage)
{
	GeneratePayments(4, 1);
}

void UNIVERSAL2::GeneratePayments(int Count, int WithCurrency)
{
	DynStr buff;
	if(Count == 1)
    {
		if(GetPrivateProfileString(UNIVERSAL_SECT, "PayMode0", "", buff, buff._sizeof(), UNIVERSAL_INI)) {
			SrokPay->SetText(buff);
		}
		return;
	}

	PremSum->GetText(buff, buff._sizeof());
	double FullSumma = atof(buff);
	FeeSum->GetText(buff, buff._sizeof());
	double FirstSumma = atof(buff);

	char curr1[10], curr2[10];
	PremCur->GetText(curr1, sizeof curr1);
	FeeCur->GetText(curr2, sizeof curr2);
	strcat(curr1, "_NAL");
	strcat(curr2, "_NAL");
	if(PremCur->GetSelIndex() != FeeCur->GetSelIndex())
	{
		FeeDate->GetText(buff, buff._sizeof());
		DATE FeeDate = GD(buff, 1);
		double rate1 = GetRateCurrency(FeeDate, curr1);
		double rate2 = GetRateCurrency(FeeDate, curr2);
		FirstSumma = FirstSumma / rate1 * rate2;
	}

	FullSumma -= FirstSumma;

	((UNIVERSAL*)Parent)->dlg1.DtFrom->GetText(buff, buff._sizeof());
	DATE from = GD(buff, 1);
	if(!from) return;

	((UNIVERSAL*)Parent)->dlg1.DtTo->GetText(buff, buff._sizeof());
	DATE to = GD(buff, 1);
	if(!to) return;

	int deltaDays = (to - from) / (Count - 1);
	int deltaSumm = int(FullSumma / (Count - 1) + 0.5);

	PremCur->GetText(curr1, sizeof curr1);
	strcpy(buff, "");
	for(int i = 1; i < Count; i++)
	{
		from += deltaDays;
		if(strlen(buff)) strcat(buff, "; ");
		SetDateToBuffer(buff + strlen(buff), from);
		if(WithCurrency)
		{
			sprintf(buff + strlen(buff), " - %0.2f %s", i < (Count-1) ? deltaSumm : FullSumma, curr1);
			FullSumma -= deltaSumm;
		}
	}

	SrokPay->SetText(buff);
}

static int ImpFunc(TOpenBase* FileDb, TOpenBase* Db, void* Data)
{
	if(strcmp(FileDb->GetString(_dwVid).s, Db->GetString(_dwVid).s))
	{
		DynStr msg;
		sprintf(msg, "%s/%5.0f\nБыл ВИД %s\nСтал %s", Db->GetString(_SERIA).s, Db->GetDouble(_NUMBER),
		Db->GetString(_dwVid).s, FileDb->GetString(_dwVid).s);
		if(MessageBox(GetApplicationObject()->MainWindow->HWindow, msg, "!", MB_ICONQUESTION | MB_YESNO) != IDYES)
		{
			*strchr(msg, '$') = 0;
			((Array*)Data)->add(*new String(msg));
			return 0;
        }

    }
	if(FileDb->GetDouble(_InsSum) != Db->GetDouble(_InsSum))
	{
		DynStr msg;
		sprintf(msg, "%s/%5.0f\nБыло %s %5.0f\nСтало %s %5.0f\n$%s", Db->GetString(_SERIA).s, Db->GetDouble(_NUMBER),
		Db->GetString(_Insurer).s, Db->GetDouble(_InsSum), FileDb->GetString(_Insurer).s, FileDb->GetDouble(_InsSum), GetRCStr(637));
		if(MessageBox(GetApplicationObject()->MainWindow->HWindow, msg, "!", MB_ICONQUESTION | MB_YESNO) != IDYES)
		{
			*strchr(msg, '$') = 0;
			((Array*)Data)->add(*new String(msg));
			return 0;
        }

    }
	return 1;
}

void UNIVERSAL::Import()
{
	Array list(100);
	list.ownsElements(TRUE);
	long Result = ImportData(TABLE, FIELDS, sizeof(FIELDS)/sizeof(*FIELDS), ImpFunc, &list, _Agent, _State, atoi(STATE_BAD), 0, 0);
	if(Result) {
		MessageBox(GetApplicationObject()->MainWindow->HWindow, GetRCStr(700+Result), "!", MB_ICONEXCLAMATION);
	}
	ShowArray(list);
}

void UNIVERSAL::Export(TWindowsObject* Parent)
{
	Priod_Params* data = new Priod_Params;
	strcpy(data->filename, TABLE);
	if(GetApplicationObject()->ExecDialog(new getDate(Parent, &data->m, &data->y, &data->dm, data->filename)) == IDOK) {
		long Error = ExportData(TABLE, FIELDS, sizeof(FIELDS)/sizeof(*FIELDS), 0, 0, data->filename, data->m, data->y, data->dm, _Agent, -1);
		sprintf(data->filename, GetRCStr(Error > 0 ? 621: 622), labs(Error));
		MessageBox(Parent->HWindow, data->filename, "", 0);
	}
	delete data;
}