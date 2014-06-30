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
#include <mandator.h>
#include <checkbox.h>
#include <thwindow.h>
#include <stdlib.h>
#include <help.h>
#include "util.h"
#include "math.h"
#include "stdlib.h"
#include "GRNCARD.h"
#include "io.h"
#include "sortarry.h"
#include "dateedit.h"
#include "editors.h"
#include "dir.h"

#define max(a, b) ((a) > (b) ? (a) : (b))

#undef NDEBUG
#include "assert.h"

//#define ONLY_SEC_PAY (-11111.)

extern char ActivateParadox;
int IsValidAvariaRecord(TOpenBase& base);
void AddBufDate(char*&dest, char* date, int flag = 0);
//char bPrintOld = 0;

class InputStringDlg : public TDialog {
      char m_Password[64];
      char m_Prompt[64];
      char m_Caption[64];
      char*m_RetBuffer;
public:
      InputStringDlg(PTWindowsObject Parent, const char* Caption, const char* Prompt, char* Password, char* RetBuffer);
      void SetupWindow();
      virtual void Ok(RTMessage Msg) = [ID_FIRST + IDOK];
};

char* GetCheckErrorText(int Err);
double GetRateCurrency(DATE payDate, char* CurrencyField);

InputStringDlg::InputStringDlg(PTWindowsObject Parent, const char* Caption, const char* Prompt, char* Password, char* RetBuffer)
              : TDialog(Parent, "GET_DEL_CODE")
{
     strcpy(m_Password, Password);
     strcpy(m_Prompt, Prompt);
     strcpy(m_Caption, Caption);
     m_RetBuffer = RetBuffer;
}

void InputStringDlg::SetupWindow()
{
    TDialog::SetupWindow();
    SetWindowText(HWindow, m_Caption);
    SendDlgItemMessage(HWindow, 101, EM_LIMITTEXT, 20, 0);
    SetDlgItemText(HWindow, 35, m_Prompt);
}

void InputStringDlg::Ok(RTMessage Msg)
{
     char buffer[32];
     GetDlgItemText(HWindow, 101, buffer, sizeof buffer);
     if(strlen(m_Password) && strcmp(buffer, m_Password)) return;
     if(m_RetBuffer) strcpy(m_RetBuffer, buffer);
     TDialog::Ok(Msg);
}


char UpdateFormData=0;
int IsCompany(const char* s);

char* GetRCStr(int strID)
{
    static char* str = 0;
    if(!str) str = new char[256];

    if(!LoadString(GetApplicationObject()->hInstance, strID, str, 256))
       strcpy(str, "<Не найдена>");

    return str; 
}


void DateFBToStr(TOpenBase* base, int field, char* buffer);
extern void DblToStr(char* str, double V, int Digit = 2);
extern DATE GD(char* str, int = 0);

void FillPlacement(TComboBox* PlacementCombo, const char* Key, const char*, int);
void FillPlacement2(TComboBox* PlacementCombo, const char* key_mask, const char* Sect, int IsDiv, int Mul, const char* ININame);
AddIntoListBox(TListBox* CB, Avaria2Data data, int Index = -1);
//AddIntoListBox2(TListBox* CB, OwnersData data, int Index = -1);
void InitTime(TEdit* e);
char* MandatoryName = "\\bases\\mandator";
const char* MANDATORY_SECT = "MANDATORY";
char* NumberToWords(double Value);
char CurrentRateEUR[10];
void SetLittleFont(HWND, short*, int);
void SetAgent(char* ACode, TOpenBase& agents, PTComboBox AgName, int);
void divDate(char* b);
void AddBufDate(char*&dest, char* date, int flag);
short _1SU_DEF = 2; //ГС

char CodeK3(double K3)
{
	return 33 + (unsigned char)(unsigned int)(K3 * 10 + 0.5);
}

double DecodeK3(char* K3)
{
	if((unsigned char)K3[0] < 33) return 0;
	double res = (((unsigned char)K3[0]) - 33) / 10. + 1e-10;
	return res;
}
/*
void AddLog(char* S, double N, char* Inf, char* Type, char* O)
{
	if(!GetPrivateProfileInt(MANDATORY_SECT, "Log", 0, ININame))
     return;

    char Usr[10];
    GetPrivateProfileString(MANDATORY_SECT, "User", "?", Usr, sizeof Usr, ININame);

    DATE currDate;
    struct date d;
    getdate(&d);
	PXDateEncode(d.da_mon, d.da_day, d.da_year, &currDate);

    struct  time t;
    gettime(&t);
    char Tm[7];
    sprintf(Tm, "%2d:%02d", t.ti_hour, t.ti_min);

    char* ShortFields[] = { "S", "N", "D", "T", "O", "USR", "TM", "Inf" };
    TOpenBase Log("\\bases\\Log", ShortFields, 8);
    if(Log.pxErr) return;
	Log(0, S);
    Log(1, N);
    Log(2, currDate);
    Log(3, Type);
    Log(4, O);
    Log(5, Usr);
    Log(6, Tm);
    Log(7, Inf);
    Log.AddRecord();
}
*/
void _FillAgents(TOpenBase* agents, TComboBox* AgName, int fld_index)
{
      double N;
      agents->First();
      for(int i = 0; i < agents->GetRecords(); i++) {
	 DynStr Tmp;
	 agents->Get();

	 if(fld_index != -1) {
	    (*agents)(N, 4 + fld_index);
	    if(N == -1 || agents->IsNull(4 + fld_index)) {
		agents->Next();
                continue;
	    }
	 }

	 (*agents)(Tmp, 100, 1);
	 char AgCode[10];
	 (*agents)(AgCode, 10, 0);
	 int Index = AgName->AddString(Tmp);
	 long AgCharCode;
	 memmove(&AgCharCode, AgCode, 4);
	 SendMessage(AgName->HWindow, CB_SETITEMDATA, Index, AgCharCode);
	 agents->Next();
       }
}

char* TrimStr(char* str)
{
   int Len = strlen(str);
   while(*str == ' ') {
      memmove(str, str + 1, Len);
      Len--;
   }

   while(Len && str[Len - 1] == ' ') {
      str[Len - 1] = 0;
      Len--;
   }

   while(strstr(str, "  ")) {
      char* pos = strstr(str, "  ");
      assert(pos);
      memmove(pos, pos + 1, strlen(pos));
   }

   return str;
}

unsigned char lenMandatory[] = {
			     5, //seria
			     13, //number
			     11, //regdate
			     5,
			     13,
			     70 + 1,//name
			     65 + 1,//address
			     11, //fromdate
			     6,  //fromtime
			     11, //todate
			     4,  //letter
			     31, //marka
			     11,
			     11,
			     11,
			     11,
			     11,
			     21,
			     21,
			     11,
			     11,
			     3,  //Discount - скидка
			     11,
                             129,
			     11,
                             129,
			     11,
			     11,
			     11,
			     16 + 1,
			     11,
			     96+1,
			     11,
			     11,
			     15,
			     11
			  };

/*
char* MandatoryAux_Fields[] = {
"Seria",
"Number",
"InsComp",
"Allfeetext",
"Pay1Text"
};
*/
char* MandatoryFields[] = {
"Seria",  //0
"Number",  //1
"RegDate",  //2
"Name",     //3
"Address",  //4
"FromDate", //5
"FromTime", //6
"ToDate",
"Letter",
"Marka",      //9
"AutoNumber", //10
"Volume",
"Tonnage",
"Quantity",
"Power",
"NumberBody", //15
"Chassis", //16
"Tarif",   //17
"Rate",
"Discount",
"AllFee", //20
"AllFeeText", //1СУ
"Pay1",
"Pay1Text",
"PayType",
"PayDate",   //25
"DraftNumber",
"Fee2",
"Fee2Text", //28  -- agent2code
"Fee2Date",
"AgentCode", //30
"Period",
"Super",     //32
"InsComp", //33 - Признак юр лица агента
"IsFee2", //34

"IsPay2",         //35
"Pay2Date",       //36
"Pay2Rate",       //37
"Pay2Summa",      //38
"Pay2Type",       //39
"Pay2DraftNumber", //40

"Resident",         //41

"Pay1Curr",      //42
"Pay2Curr",       //43
"PrevSeria",        //44
"PrevNumber",        //45

//Part2
"TarifGlass",    //46
"Placement",     //47  --K3 * 10
"K",             //48
"ClassOldAv",    //49
"CountAv",       //50
"ClassAvCurr",   //51
"K1",            //52
"K2",            //53
"StopDate",      //54
"Vladenie",      //55
"InsurerType",   //56
"Version",       //57
"State",         //58
"CityCode",      //59
"UpdateDate",    //60
"OwnerCode",     //61   --retdate
"BaseCarCode",   //62   --polis reports
"CarCode",       //63   --agent2percent
"RetBRB",        //64   
"RetEUR",         //65
"AgPercent",      //66
"ret1",           //67
"ret2",            //68
"country",            //69  --agent2type
"mobtel",          //70
"hometel",          //71
"contact",          //72
"sms",          //73

"S0",           //74
"Curr0",        //75
"Nal0",         //76
"SU0",          //77
"D0",           //78
"Doc0",         //79
"TYP0"          //80
};

void ToRubles(double& Value)
{
   char buffer[32];
   if(!GetPrivateProfileString(MANDATORY_SECT, "RublMin", "10", buffer, sizeof buffer, ::ININame))
     return;
   
   Value = floor(Value * 100.0) / 100.0;
   //Value = int((Value + atof(buffer) / 2) / atof(buffer)) * atof(buffer);
   Value = floor((Value + atof(buffer) / 2.0) / atof(buffer)) * atof(buffer);
}

int Type(unsigned long Data)
{
	return ((Data & 0x00FF0000) >> 16);
}

int Letter(unsigned long Data)
{
    return ((Data & 0xFF000000) >> 24);
}

int IsLetter(unsigned long Data)
{
    int Letter = ::Letter(Data);
    return Letter < '0' || Letter > '9';
}

unsigned char MandatoryCnt = 74;
unsigned char KomplexCnt = 81;

char* Avaria2Fields[] = {
"Seria",         //0
"Number",        //1
"N",             //2
"AvDate",        //3
"AvPlace",       //4
"NWork",         //5
"WrDate",        //6
"What",          //7
"Doc1Type",      //8
"SN",            //9
"FIO",           //10
"FIOType",       //11
"FIOAddress",    //12
"BadMan",        //13
"BMStaj",        //14
"BMVTR",         //15
"Sum1",          //16
"Sum1V",         //17
"V1",            //18
"Sum1_1",        //19
"Sum1_1v",       //20
"V1_1",          //21
"V1_Date",       //22
"Sum2",          //23
"SumV2",         //24
"V2",            //25
"Sum2_1",        //26
"Sum2_1V",       //27
"V2_1",          //28
"V2_Date",       //29
"Sum3",          //30
"SumV3",         //31
"V3",            //32
"Sum3_1",        //33
"Sum3_1V",       //34
"V3_1",          //35
"V3_Date",       //36
"UpdateDate",    //37
"CityCode",      //38
"CityCode_P",    //39
"DISCOUNT_P",    //40
"IsResid_P",     //41
"Vladen_P",      //42
"MARKA_P",       //43
"BASE_TYPE_P",   //44
"CHARVAL",       //45
"AUTONMB_P",     //46
"BODY_NO",       //47
"OwnerCode_P",   //48
"BaseCarCode_P", //49
"CarCode_P",     //50
"PaymentCode",    //51
"DeclTC",         //52
"DeclHL",
"DeclIM",
"LossCode",
"DtFreeRz",
"DtClLoss",
"SumPay",
"OcenTC",
"OcenHL",         //60
"OcenIm",
"OthTC",
"OthHL",
"OthIm",
"IsTC1",
"IsTC2",
"IsHL1",
"IsHL2",
"IsIM1",
"IsIM2"           //70
};


/*
char* OwnerFields[17] = {
"Seria",        //0
"Number",	//1
"Name",         //2
"UpdateDate",   //3
"Placement",    //4
"K",            //5
"Fizich",       //6
"Pravo",        //7
"Address",      //8
"PravoTxt",     //9
"WhenPrava",    //10
"CityCode",     //11
"OwnerCode",     //12
"RegDate",      //13
"Summa",        //14
"PayDoc",	//15
"PayDate"	//16
};
*/

char ModifyMandatory;

void UrFizich::WMRButtonDown(RTMessage)
{
    ChText();
}

void UrFizich::WMLButtonDown(RTMessage)
{
    ChText();
}

void UrFizich::ChText()
{
    char T[5];
    GetText(T, sizeof T);
    if(*T == 'Ю')
	SetText("Физич.");
    else
	SetText("Юрид.");
}

BOOL MyOpenFile(HWND HWindow, OPENFILENAME& ofn, char* szFilter, char* szDirName, char* szFile, char* Title, char* ext, int Load = 1 );

long MandatoryDlg::initPosition;

char* Taxi_Fields[] = {
"Ser",  //0
"N",    //1
"Summa",//2
"Curr", //3
"Nal",  //4
"SU",   //5
"D",    //6
"PayDoc",//7
"Type"
};


#define ASS() 
		//char qwqwqw[10]; \
		//sprintf(qwqwqw, "%d", (int)__LINE__); \
		//MessageBox(0, qwqwqw, qwqwqw, 0)

DATE fstSept;
char* MandatoryDlg::Rubl;

MandatoryDlg::MandatoryDlg(PTWindowsObject p, int Komplex)
	:Dlg_base(p, !ActivateParadox
			? (Komplex ? "KOMPLEXCALC" : "MANDCALC")
			: Komplex
				? "KOMPLEX"
				: "MANDATORY",
			Komplex
				? "\\bases\\komplex"
				: MandatoryName,
			MandatoryFields, Komplex ? KomplexCnt : MandatoryCnt),
     agents(AgentDBName, AgentFields, 2),
     avaria2(Komplex ? 0 : "\\mandav2", Avaria2Fields, Komplex ? 0 : 71),
     //owners_("\\mandown", OwnerFields, 17),
     TaxiDb(Komplex ? 0 : "\\TAXI", Taxi_Fields, Komplex ? 0 : sizeof(Taxi_Fields) / sizeof(*Taxi_Fields)),
     m_TableKeyNames(10, 0, 10),
     m_TarifKeyNames(10, 0, 10),
     m_CheckAuto(10, 0, 10),
     m_CheckShassi(10, 0, 10),
     m_CheckNumberBody(10, 0, 10),
     m_LastFind(10, 0, 10),
	 Version(2)
{

   this->Komplex = Komplex;
   PXDateEncode(9, 1, 2006, &fstSept);
   if(Komplex)
   {
	GetPrivateProfileString("KOMPLEX", "INI", ::ININame, ININame, sizeof ININame, ::ININame);
   }
   else
   {
	strcpy(ININame, ::ININame);
   }
   if(!*Dlg_base::Agent_Code)
   {
		GetPrivateProfileString(MANDATORY_SECT, "DefAgent", "", Dlg_base::Agent_Code, sizeof Dlg_base::Agent_Code, ININame);
   }
   memset(e, 0, sizeof e);
   ModifyMandatory = 0;
   IsOpenPart2 = 0;
   StartWidth = 0;

   memset(m_AVPayData, 0, sizeof m_AVPayData);

   m_UrFizich1 = new UrFizich(this, 64, 3);
   m_UrFizich1->DisableTransfer();

   m_UrFizich2 = new UrFizich(this, 73, 3);
   m_UrFizich2->DisableTransfer();

   memset(&e, 0, sizeof e);

   //ПИЗДЕЦ АЩЕ!!!!!!!!!
   e[0] = (TEdit*)new TComboBox(this, 50, lenMandatory[0]);
   e[3] = (TEdit*)new TComboBox(this, 53, lenMandatory[3]);
   ((TComboBox*)e[0])->DisableTransfer();
   ((TComboBox*)e[3])->DisableTransfer();
   for(int i = 1; i < 5; i++) // 0, 3 because seria is combo
   {
		if(i == 2)
		{
			e[i] = new TDateEdit(this, 50 + i, lenMandatory[i]);
		}
        else
		if(i != 3)
        {
			e[i] = new TEdit(this, 50 + i, lenMandatory[i]);
		}
   }

   for(i = 5; i < MANDATORY_ED_COUNT; i++)
   {
	 if(i == 8)
	 {
		e[i] = new TTimeEdit(this, 100 + i - 5, lenMandatory[i]);
	 }
     else
	 if(i == 5 || i == 6)
	 {
		e[i] = new UPSTREdit(::ININame, MANDATORY_SECT, this, 100 + i - 5, lenMandatory[i], i == 5 ? "Owner%d" : "Street%d", i == 5 ? "Owner%d_%d" : "Street%d_%d", 1);
		strcpy(((UPSTREdit*)e[i])->DivString, " ");
     }
     else
	 if(i == 11)
	 {
		DynStr buff;
		GetPrivateProfileString(MANDATORY_SECT, "AutoMarkFile", ININame, buff, buff._sizeof(), ININame);
		e[i] = new INIEdit(buff, MANDATORY_SECT, this, 100 + i - 5, lenMandatory[i], "AutoType%d", "AutoType%d_%d");
	 }
     else
     if(i == 12)
		e[i] = new INIEdit(::ININame, MANDATORY_SECT, this, 100 + i - 5, lenMandatory[i], "AutoNumber%d", "AutoNumber%d_%d", 1);
     else
     if(i == 20 || i == 21)
		e[i] = new NumberEdit(this, 100 + i - 5, lenMandatory[i], 1);
	 else
	 if((100 + i - 5 == 102) || (100 + i - 5 == 121) || (100 + i - 5 == 125) || (100 + i - 5 == 127))
		e[i] = new TDateEdit(this, 100 + i - 5, lenMandatory[i]);
	 else
		e[i] = new TEdit(this, 100 + i - 5, lenMandatory[i]);
   }

   m_OwnPolis = new TCheckBox(this, 75, 0);
   Nal = new TCheckBox(this, 201, 0);
   BezNal = new TCheckBox(this, 202, 0);
   Karta = new TCheckBox(this, 85, 0);
   IsFee2 = new TCheckBox(this, 203, 0);
   (m_1SU = new TComboBox(this, 66, 0))->DisableTransfer();

   Nal_2 = new TCheckBox(this, 206, 0);
   BezNal_2 = new TCheckBox(this, 207, 0);
   IsPay2 = new TCheckBox(this, 205, 0);
   m_ToReport = new TCheckBox(this, 1003, 0);
	
   //Part2
   m_Placement = new TComboBox(this, 990, 33);
   m_Placement->DisableTransfer();
   m_TarifGlass = new NumberEdit(this, 992, 11, 1);
   m_K1 = new NumberEdit(this, 993, 11, 1);
   (m_K3 = new TComboBox(this, 77, 0))->DisableTransfer();
   _m_OldClassAv = new TComboBox(this, 994, 5);
   _m_OldClassAv->DisableTransfer();
   _m_CountAv = new TEdit(this, 995, 3);
   _m_CurrClassAv = new TEdit(this, 996, 3);
   m_StopDate = new TDateEdit(this, 998, 11);
   m_RetDate = new TDateEdit(this, 76, 11);

   m_MobilePhone = new INIEdit(::ININame, MANDATORY_SECT, this, 79, 30, "MobilePhone%d", 0);
   m_HomePhone = new INIEdit(::ININame, MANDATORY_SECT, this, 80, 30, "HomePhone%d", 0);
   m_Contact = new UPSTREdit(::ININame, MANDATORY_SECT, this, 84, 30, "Owner%d", "Owner%d_%d", 1);
   m_SMS = new TCheckBox(this, 83, 0);

   m_Osnovanie = new TComboBox(this, 999, 50);
   m_Osnovanie->DisableTransfer();
   m_Fizich = new TRadioButton(this, 1001, 0);
   m_Uridich = new TRadioButton(this, 1002, 0);
   m_IP = new TRadioButton(this, 1005, 0);
   m_CityEdit = new CityEdit(this, 57, 4, ININame);
   m_RetBRB = new NumberEdit(this, 61, 12, 1);
   m_RetEUR = new NumberEdit(this, 62, 12, 1);
   m_AgPercent1 = new NumberEdit(this, 63, 6, 1);
   m_AgPercent2 = new NumberEdit(this, 72, 6, 1);
   //Part2 END

   m_RetDate1 = new TDateEdit(this, 69, 11);
   m_RetDate2 = new TDateEdit(this, 70, 11);


   ///TAXI
   if(Komplex)
   {
		m_TaxiSumma = new NumberEdit(this, 89, 10, 1); //summa
	m_TaxiPayDate=new TDateEdit(this, 87, 11);   //dateedit
	m_TaxiDoc=new NumberEdit(this, 94, 12, 0);   //pay number
	m_Taxi1SU=new TCheckBox(this, 91, 0);
	m_TaxiNal=new TCheckBox(this, 92, 0);

   m_TaxiPayCurr = new TComboBox(this, 90, 0);
   m_TaxiPayCurr->DisableTransfer();
   m_TaxiInOut = new TComboBox(this, 88, 0);
   m_TaxiInOut->DisableTransfer();
   m_TaxiVid = new TComboBox(this, 93, 0);
   m_TaxiVid->DisableTransfer();
   }
   /////////



   //Resident = new TCheckBox(this, 208, 0);
   m_Resident = new TComboBox(this, 208, 0);
   m_Resident->DisableTransfer();

   m_Vzamen = new TComboBox(this, 209, 0);
   m_Vzamen->DisableTransfer();

   AgName1 = new TComboBox(this, 200, 100);
   AgName1->DisableTransfer();

   AgName2 = new TComboBox(this, 71, 100);
   AgName2->DisableTransfer();

   Period = new TComboBox(this, 300, 100);
   Period->DisableTransfer();

   Super = new TComboBox(this, 1000, 100);
   Super->DisableTransfer();

   ListBAvaria = new TListBox(this, 470);
   ListBAvaria->DisableTransfer();
   ListBOwners = new TListBox(this, 471);
   ListBOwners->DisableTransfer();

   ListBPhoto = new TListBox(this, 474);
   ListBPhoto->DisableTransfer();

   Pay1Curr = new TComboBox(this, 301, 100);
   Pay1Curr->DisableTransfer();
   Pay2Curr = new TComboBox(this, 302, 100);
   Pay2Curr->DisableTransfer();

//   m_VersionCombo = new TComboBox(this, 1437, 100);
//   m_VersionCombo->DisableTransfer();

   Rubl = new char[128];
   GetPrivateProfileString(MANDATORY_SECT, "RublName", "", Rubl, 128, ::ININame);
}

void SetDroppedWidthFromList(TComboBox* cb)
{
	int nMaxLength = 0;
	int nMaxLengthStr = 0;

	DynStr str;

	HDC dc = GetDC(cb->HWindow);

	HGDIOBJ oldFont = SelectObject(dc, (HFONT)SendMessage(cb->HWindow, WM_GETFONT, 0, 0));
	for (int i = 0; i < cb->GetCount(); i++) {
			cb->GetString(str, i);
			nMaxLength = max(nMaxLength, LOWORD(GetTextExtent(dc, str, strlen(str))));
	}
	nMaxLength += (::GetSystemMetrics(SM_CXVSCROLL) + LOWORD(GetTextExtent(dc, " ", 1))*2);
	SelectObject(dc, oldFont);
	ReleaseDC(cb->HWindow, dc);
	RECT cbRect;
	GetWindowRect(cb->HWindow, &cbRect);
	//nMaxLength = min(nMaxLength, GetSystemMetrics(SM_CXSCREEN) - cbRect.left);
	SendMessage(cb->HWindow, 0x0160, nMaxLength, 0);
}

void AlignCombo(TComboBox* Combo)
{
	ASS();
	RECT rect;
    GetWindowRect(Combo->HWindow, &rect);
    int MaxH = max(rect.top, (GetSystemMetrics(SM_CYSCREEN) - rect.bottom));

    ScreenToClient(GetParent(Combo->HWindow), (POINT*)&rect);
    ScreenToClient(GetParent(Combo->HWindow), (POINT*)&rect.right);
	MoveWindow(Combo->HWindow, rect.left, rect.top, rect.right - rect.left, MaxH - 32, FALSE);
	SetDroppedWidthFromList(Combo);
}

void MandatoryDlg::Placement(RTMessage msg)
{
	ASS();
	 if(msg.LP.Hi == CBN_SELCHANGE)
	   if(ModifyMandatory)
	 Calculate();
}

/*void MandatoryDlg::ChangeK(RTMessage msg)
{
	ASS();
}
*/

void MandatoryDlg::FillSuperCombo(int IdxTable, int Notify)
{
	ASS();
   char Key[32];
   DynStr _buffer;

   if(IdxTable == -1) IdxTable = m_Resident->GetSelIndex();
   if(IdxTable == -1) return;
   IdxTable = SendMessage(m_Resident->HWindow, CB_GETITEMDATA, IdxTable, 0);

   const char* Key1Mask = (String&)m_TableKeyNames[m_Resident->GetSelIndex()];

   Super->ClearList();
   for(int Type = 0;;Type++) {
      for(int subType = 0;; subType++) {
	  wsprintf(Key, Key1Mask, Type, subType);
	  assert(strlen(Key) < sizeof Key);
	  int readed = GetPrivateProfileString(MANDATORY_SECT, Key, "", _buffer, _buffer._sizeof( ), ININame);
	  if(readed) {
	      int Index = 0;
	      unsigned short Letter = 0;
	      if(subType == 0) { //Описание типа и буква
		     char* divider = strchr(_buffer, ',');
			 if(!divider || *_buffer == 0) {
						 Error:
				 wsprintf(_buffer, GetRCStr(500), Type, subType);
				 //assert(strlen(buffer) < sizeof buffer);
				 MessageBox(HWindow, _buffer, "Ошибка", MB_OK | MB_ICONSTOP);
				 return;
			 }

		     divider++;
		     while(*divider && *divider == ' ')
		         divider++;
		     Index = Super->AddString(divider);
		     Letter = *_buffer;
	      }
	      else {
		     char* divider = strchr(_buffer, ',');
			 if(!divider || *_buffer == 0) {
				 wsprintf(_buffer, GetRCStr(500), Type, subType);
				 //assert(strlen(buffer) < sizeof buffer);
				 MessageBox(HWindow, _buffer, "Ошибка", MB_OK | MB_ICONSTOP);
				 return;//goto Next;
			 }
			 //char* str[256] = "     ";
			 DynStr str;
			 strcpy(str, "     ");
					 Letter = *_buffer;
			 divider++;
			 while(*divider && *divider == ' ')
			   divider++;
			 strcat(str, divider);
			 //assert(strlen(str) < sizeof str);
			 Index = Super->AddString(str);
		  }

	      //Combo box contain codes <Буква>, <Type>, 0, <SubType>,
	      short HiWord = (Letter << 8) | (Type & 0x00FF);
		  SendMessage(Super->HWindow, CB_SETITEMDATA, Index, MAKELONG(subType, HiWord));

		  
		  /*char q[70];
          Super->GetString(q, Index);
		  sprintf(q+strlen(q), " %lX",  MAKELONG(subType, HiWord));
		  MessageBox(0, q, q, 0);
		  /*31000001
		  */
	  }
	  else {
	      if(subType == 0) goto Next;
		  else break;
	  }
      }
   }

   Next:

   if(!Notify) return;

   char key[24];
   sprintf(key, "DefTarif%d", IdxTable);
   assert(strlen(key) < sizeof key);
   if(GetPrivateProfileString(MANDATORY_SECT, key, "", _buffer, _buffer._sizeof(), ININame)) {
	int Type = atoi(_buffer);
	if(strstr(_buffer, "_")) {
	   int subType = atoi(strstr(_buffer, "_") + 1);

	   for(int i = 0; i < Super->GetCount(); i++)
	      if((SendMessage(Super->HWindow, CB_GETITEMDATA, i, 0) & 0x00FFFFFF) == MAKELONG(subType, Type)) {
		   Super->SetSelIndex(i);
		   TMessage msg;
		   ChangeSuper(msg);
                   break;
              }
        }
   }

}

void MandatoryDlg::Help(RTMessage msg)
{
	ASS();
   if(msg.WParam == 1000)
      WinHelp(GetApplication()->MainWindow->HWindow, "blank.hlp", HELP_CONTEXT, HELP_PRIVATE);

   /*if(msg.WParam == 333) {
       EnableAllControls();
       buffer.OwnerCode = 0;
       buffer.BaseCarCode = 0;
       buffer._CarCode_ = 0;
   }
   if(msg.WParam == 334) {
       for(int i = 0; i < ownersCount; i++)
	   autoowners[i].OwnerCode = 0;
   }
   if(msg.WParam == 335) {
	   for(int i = 0; i < avariaCount; i++) {
	   avarias2[i].OwnerCode = 0;
	   avarias2[i].BaseCarCode = 0;
	   avarias2[i].CarCode = 0;
	   avarias2[i].PaymentCode = 0;
       }
   } */
   if(msg.WParam == 351) {
       //if(m_State != 5) {
       //	   MessageBox(HWindow, "Полису уже ничем не поможешь!", "Ошибка", MB_OK | MB_ICONSTOP);
       //	   return;
       //}
       SetState(STATE_POLIS_NORMAL);
   }
   if(msg.WParam == 352)
       ShowPolisAvInfo();

   if(msg.WParam == 350) {
       char Number[16];
       TDialog* dlg = new TDialog(this, "MAKE_DUP");
       dlg->SetTransferBuffer(Number);
       e[1]->GetText(Number, sizeof Number);
       new TEdit(dlg, 101, 16);
       if(GetModule()->ExecDialog(dlg) == IDOK) {
	   UpdateFormData++;
	   char buffer[16];
	   e[0]->GetText(buffer, sizeof buffer);
	   e[3]->SetText(buffer);

	   e[1]->GetText(buffer, sizeof buffer);
	   e[4]->SetText(buffer);

	   struct date d;
	   getdate(&d);
	   wsprintf(buffer, "%02d.%02d.%04d", d.da_day, d.da_mon, d.da_year);
	   e[2]->SetText(buffer);

	   e[1]->SetText(Number);

	   UpdateFormData--;
	   ClearAvarias(0);
	   ClearPhotoList();
	   m_Vzamen->SetSelIndex(0);
	   SetFocus(e[1]->HWindow);
           EnableWindow(e[3]->HWindow, 1);
	   EnableWindow(e[4]->HWindow, 1);
	   EnableWindow(GetDlgItem(HWindow, 406), 1);

      TMessage msg;
  
	  if(*Dlg_base::Agent_Code/*GetPrivateProfileString(MANDATORY_SECT, "DefAgent", "", buffer, sizeof buffer, ININame)*/) {
		  SetAgent(Dlg_base::Agent_Code, agents, AgName1, 0);
   	      msg.LP.Hi = CBN_SELCHANGE;
		  ChAgent1(msg);
	   }

/*	   for(int i = 0; i < ownersCount; i++) {
	     autoowners[i].IsSaved = 0;
	     autoowners[i].OwnerCode = 0;
	   }*/
       }
   }


   Dlg_base::Help(msg); //call defwndproc
}

HWND HookWnd = 0;
HHOOK PrevProc;
HOOKPROC key_proc;

LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam)
{
	ASS();
	if(wParam == VK_F2) {
		if(IsWindowEnabled(GetDlgItem(HookWnd, 406)))
		  if(!(lParam & 0x80000000L))
			if(GetParent(GetFocus()) == HookWnd)
			  PostMessage(HookWnd, WM_COMMAND, 406, 0);
        MessageBeep(0);
    	return 1;
	}
	return CallNextHookEx(PrevProc, code, wParam, lParam);
}

char SupportHook;

void MandatoryDlg::SetupWindow()
{
	ASS();
/*	SupportHook = GetPrivateProfileInt(MANDATORY_SECT, "Keyboard", 0, ININame);
	if (SupportHook) {
	    HookWnd = HWindow;
	    key_proc = (HOOKPROC)MakeProcInstance((FARPROC)KeyboardProc, (HINSTANCE)GetApplicationObject()->hInstance);
		PrevProc = SetWindowsHookEx(WH_KEYBOARD, key_proc, (HINSTANCE)GetApplicationObject()->hInstance, (HTASK)NULL);
	}
*/
short Numb[] = { 85, 66, 209, 200, 64, 63, 55, 56, 118, 120, 124, 201, 202, 126, 100, 101, 460, 461, 462, 205, 206, 207, 9000, 9125, 2000, 1437, 55, 56, 497, 1457, 7632, 971 };

   Dlg_base::SetupWindow();

   //EnableWindow(m_MobilePhone->HWindow, 0);
   //EnableWindow(m_HomePhone->HWindow, 0);

   {
         
for(int i = 0; i < 10000; i++)
{
	PTWindowsObject o = ChildWithId(i);
	if(o && !o->HWindow)
	{
			char s[12]; sprintf(s, "Ctrl? %d", i);
    		MessageBox(0, s, s, 0);
    }      
}

}

   FillPlacement2((TComboBox*)e[0], "Series%d", MANDATORY_SECT, 0, 0, ININame);
   FillPlacement2((TComboBox*)e[3], "Series%d", MANDATORY_SECT, 0, 0, ININame);
   AlignCombo((TComboBox*)e[0]);
   AlignCombo((TComboBox*)e[3]);

   if(ListBAvaria) SendMessage(ListBAvaria->HWindow, LB_SETHORIZONTALEXTENT, 400, 0);
   if(ListBOwners) SendMessage(ListBOwners->HWindow, LB_SETHORIZONTALEXTENT, 400, 0);

   AppendMenu(GetSystemMenu(HWindow, FALSE), MF_SEPARATOR, 0, "");
   AppendMenu(GetSystemMenu(HWindow, FALSE), MF_STRING, 350, "Выдать дубликат...");
   AppendMenu(GetSystemMenu(HWindow, FALSE), MF_STRING, 351, "Сделать полис нормальным");
   if(!Komplex)
   {
	   AppendMenu(GetSystemMenu(HWindow, FALSE), MF_SEPARATOR, 0, "");
	   AppendMenu(GetSystemMenu(HWindow, FALSE), MF_STRING, 352, "Информация по авариям...");
   }

   ((TRulerWin*)GetApplicationObject()->MainWindow)->msg_reciever = HWindow;

   SetState(STATE_POLIS_NORMAL);

   m_Vzamen->AddString("Дубликат для");
   m_Vzamen->AddString("Предыдущий был");

//   m_VersionCombo->AddString("1");
//   m_VersionCombo->AddString("2");

   SendMessage(m_1SU->HWindow, CB_SETITEMDATA, m_1SU->AddString("НЕ 1СУ"), 'N');
   SendMessage(m_1SU->HWindow, CB_SETITEMDATA, m_1SU->AddString("ГС"), 'Y');
   SendMessage(m_1SU->HWindow, CB_SETITEMDATA, m_1SU->AddString("КВ"), '0');


   char key[32];
   DynStr _buffer;

   for(int i = 0; i < 10; i++)
   {
	  sprintf(key, "1SU_%d", i);
	  int Readed = GetPrivateProfileString(MANDATORY_SECT, key, "", _buffer, _buffer._sizeof(), ::ININame);
      if(!Readed)
		  break;
	  char* div = strchr(_buffer, ',');
	  if(div == 0)
		  break;
	  if(strstr(_buffer, "DEF") != 0)
		  _1SU_DEF = i + 3;
	  *div = 0;	   
	  SendMessage(m_1SU->HWindow, CB_SETITEMDATA, m_1SU->AddString(_buffer), 'A' + i);
   }


   SendMessage(m_1SU->HWindow, LB_SETHORIZONTALEXTENT, 100, 0);
   AlignCombo(m_1SU);

   FillPlacement2(m_Placement, "OwnerPlace%d", MANDATORY_SECT, 1, 100, ::ININame);
   AlignCombo(m_Placement);

   FillPlacement2(m_Osnovanie, "Base_of_possession%d", MANDATORY_SECT, 1, 1, ::ININame);
   AlignCombo(m_Osnovanie);

   FillPlacement2(_m_OldClassAv, "ClassAV%d", MANDATORY_SECT, 1, 100, ::ININame);
   AlignCombo(_m_OldClassAv);

   for(i = 0; i < 10; i++)
   {
	  sprintf(key, "K3_%d", i);
	  int Readed = GetPrivateProfileString(MANDATORY_SECT, key, "", _buffer, _buffer._sizeof(), ::ININame);
      if(!Readed)
		  continue;
		m_K3->AddString(_buffer);
   }
   AlignCombo(m_K3);
   //fill tarif tables
   for(i = 0; i < 10; i++) {
	  sprintf(key, "TarifTable%d", i);
	  assert(strlen(key) < sizeof key);
//	  MessageBox(0, ::ININame, ININame, 0);
	  int Readed = GetPrivateProfileString(MANDATORY_SECT, key, "", _buffer, _buffer._sizeof(), ININame);
      if(!Readed)
		  continue;

	  if(Readed > (_buffer._sizeof()) - 3) {
	     MessageBox(HWindow, GetRCStr(501), "Ошибка", MB_ICONSTOP);
         return;
      }

      char* div1 = strchr(_buffer, '|');
      if(!div1) break;
      char* div2 = strchr(div1 + 1, '|');
      if(!div2) break;
      char* div3 = strchr(div2 + 1, '|');
      if(!div3) break;
      char* div4 = strchr(div3 + 1, '|');
      if(!div4) break;
      char* div5 = strchr(div4 + 1, '|');
      if(!div5) break;
      *div1 = *div2 = *div3 = *div4 = *div5 = 0;

      DynStr temp;
	  SendMessage(m_Resident->HWindow, CB_SETITEMDATA, m_Resident->AddString(_buffer), i);

	  strcpy(temp, div1 + 1);
      strcat(temp, "%02d_%d");
      //assert(strlen(temp) < sizeof temp);
      m_TableKeyNames.add(*new String(temp));
      strcpy(temp, div2 + 1);
      strcat(temp, "%02d_%d");
      //assert(strlen(temp) < sizeof temp);
      m_TarifKeyNames.add(*new String(temp));
      strcpy(temp, div3 + 1);
      strcat(temp, "%d");
      //assert(strlen(temp) < sizeof temp);
      m_CheckAuto.add(*new String(temp));
      strcpy(temp, div4 + 1);
      strcat(temp, "%d");
      //assert(strlen(temp) < sizeof temp);
      m_CheckShassi.add(*new String(temp));
      strcpy(temp, div5 + 1);
      strcat(temp, "%d");
      //assert(strlen(temp) < sizeof temp);
      m_CheckNumberBody.add(*new String(temp));
   }

   if(m_Resident->GetCount() == 0) {
       MessageBox(HWindow, GetRCStr(502), "Ошибка", MB_OK | MB_ICONSTOP);
	   //CloseWindow();
	   //return;
   }
   AlignCombo(m_Resident);

   Pay1Curr->AddString("BRB");
   Pay1Curr->AddString("EUR");
   Pay1Curr->AddString("USD");
   //Pay1Curr->AddString("DM");
   Pay1Curr->AddString("RUR");

   Pay2Curr->AddString("BRB");
   Pay2Curr->AddString("EUR");
   Pay2Curr->AddString("USD");
   //Pay2Curr->AddString("DM");
   Pay2Curr->AddString("RUR");

   if(ActivateParadox) {
	   if(agents.pxErr)
	      CloseWindow();
	   else {
	      _FillAgents(&agents, AgName1, -1);
		  _FillAgents(&agents, AgName2, -1);
	   }
   }

   for(int PeriodCode = 0;;PeriodCode++) {
	char buffer[32];
	char Key[32];
	wsprintf(Key, "Period%d", PeriodCode);
	assert(strlen(Key) < sizeof Key);
	if(!GetPrivateProfileString(MANDATORY_SECT, Key, "", buffer, sizeof buffer, ININame))
	   break;
	Period->AddString(buffer);
   }

   FillSuperCombo(-1, 0);

   //Next:
   SetLittleFont(HWindow, Numb, sizeof(Numb) / sizeof(Numb[0]));
//   EnableMenuItem(GetSubMenu(GetMenu(Parent->HWindow), 2), 3, MF_BYPOSITION | MF_GRAYED);

   if(ActivateParadox)
   {
	  if(!base->GetRecords())
      {
//		  PostMessage(HWindow, WM_COMMAND, 405, MAKELONG(405, BN_CLICKED));
      }
	  else
	  if(initPosition)
	  {
		  PXRecGoto(base->getTblHandle(), initPosition);
		  base->Get();
		  initPosition = 0;
	  }
   }

   if(Komplex)
   {
	   m_TaxiVid->AddString("Такси");
	   m_TaxiVid->AddString("Другое");
	   m_TaxiVid->AddString("Тип авто");
	   m_TaxiVid->AddString("Коэфф. аварийности");
	   m_TaxiInOut->AddString("");
	   m_TaxiInOut->AddString("Доплата");
	   m_TaxiInOut->AddString("Возврат");
	   m_TaxiPayCurr->AddString("BRB");
	   m_TaxiPayCurr->AddString("EUR");
	   m_TaxiPayCurr->AddString("USD");
	   m_TaxiPayCurr->AddString("RUR");
	   AlignCombo(m_TaxiPayCurr);
	   AlignCombo(m_TaxiInOut);
	   AlignCombo(m_TaxiVid);
   }

   GetDataFromBase();

   AlignCombo(Super);
   AlignCombo(AgName1);
   AlignCombo(AgName2);

//   RECT r;
//   GetWindowRect(GetDlgItem(HWindow, 408), &r);
//   SendMessage(m_VersionCombo->HWindow, CB_SETITEMHEIGHT, -1, r.bottom - r.top);
//   SendMessage(m_VersionCombo->HWindow, CB_SETITEMHEIGHT, 0, r.bottom - r.top);
}

int CanAlign = 1;

void MandatoryDlg::WMMove(RTMessage msg)
{
	ASS();
   if(CanAlign) {
	 AlignCombo(Period);
	 AlignCombo(Super);
	 AlignCombo(AgName1);
	 AlignCombo(AgName2);
   }
}

void MandatoryDlg::ChangeCountAv(RTMessage msg)
{
	ASS();
   if(msg.LP.Hi == EN_CHANGE)
	Calculate();
}

void DateFBToStr(TOpenBase* base, int field, char* buffer)
{
	ASS();
   DATE date;

   (*base)(date, field);
   int m, d, y;
   *buffer = 0;
   if(!ISBLANKDATE(date))
    {
      PXDateDecode(date, &m, &d, &y);
	  sprintf(buffer, "%02u.%02u.%04u", d, m, y);
	}
}

void IntFBToStr(TOpenBase* base, int field, char* buffer)
{
   double value;
   (*base)(value, field);
   buffer[0] = 0;
   if((long)value != 0)
   {
      sprintf(buffer, "%0.0f", value);
   }
}

long Ostatok(double Value)
{
	return long((Value - floor(Value) + 0.001) * 100);
}

const char* GetKopeykaName(char* Curr, int Val)
{
	if(strcmp(Curr, "BRB") == 0 || strcmp(Curr, "RUR") == 0)
	{
		if(Val%10 == 1 && Val < 10) return "копейка";
		if(Val%10 > 1 && Val%10 < 5 && Val < 10) return "копейки";
		return "копеек";
	}
	if(strcmp(Curr, "USD") == 0 || strcmp(Curr, "EUR") == 0)
	{
		if(Val%10 == 1 && Val < 10) return "цент";
		if(Val%10 > 1 && Val%10 < 5 && Val < 10) return "цента";
		return "центов";
	}
	return "AAA";
}

const char* GetCurrName(char* Curr, long Val)
{
	if(strcmp(Curr, "BRB") == 0)
	{
		if(Val%10 == 1 && Val < 10) return "белорусский рубль";
		if(Val%10 > 1 && Val%10 < 5 && Val < 10) return "белорусских рубля";
		return "белорусских рублей";
	}
	if(strcmp(Curr, "RUR") == 0)
	{
		if(Val%10 == 1 && Val < 10) return "российский рубль";
		if(Val%10 > 1 && Val%10 < 5 && Val < 10) return "российских рубля";
		return "российских рублей";
	}
	if(strcmp(Curr, "USD") == 0)
	{
		if(Val%10 == 1 && Val < 10) return "доллар США";
		if(Val%10 > 1 && Val%10 < 5 && Val < 10) return "доллара США";
		return "долларов США";
	}
	if(strcmp(Curr, "EUR") == 0)
	{
		return "ЕВРО";
	}
	return Curr;
}

const char* GetKopeyka(char* Curr, double Value)
{
	long Val = Ostatok(Value);
	if(Val == 0) return "";
	char* Buffer = NumberToWords(Val);
	strcat(Buffer, " ");
	strcat(Buffer, GetKopeykaName(Curr, Val));
    return Buffer;
}

char* GetSummaText(double Value, char* Valuta, char* Kop = NULL)
{
	ASS();
	char* Buffer = NumberToWords(Value);
	strcat(Buffer, " ");
	strcat(Buffer, GetCurrName(Valuta, Value));
	int Rest = Ostatok(Value);
	if(Rest > 0)
	{
		strcat(Buffer, " ");
		wsprintf(Buffer + strlen(Buffer), "%d %s", Rest, (Kop == NULL||1 ? GetKopeykaName(Valuta, Rest) : Kop));
	}
    return Buffer;
}
/*
void MandatoryDlg::SetVersion(int V)
{
    Version = V;
	EnableWindow(GetDlgItem(HWindow, 992), Version == 2);
    EnableWindow(GetDlgItem(HWindow, 993), Version == 2);
    EnableWindow(GetDlgItem(HWindow, 994), Version == 2);
    EnableWindow(GetDlgItem(HWindow, 995), Version == 2);
    EnableWindow(GetDlgItem(HWindow, 996), Version == 2);
    EnableWindow(GetDlgItem(HWindow, 997), Version == 2);

    m_VersionCombo->SetSelIndex(Version - 1);
    //EnableWindow(GetDlgItem(HWindow, 420), Version == 2);
}
*/
int TestTextValue(char* buffer, char* key, char* value, char* errCaption, HWND Wnd)
{
	ASS();
	AnsiLower(buffer);
    if(!strstr(buffer, key))
      return 1;
    if(atof(value) == 0) {
	  MessageBox(Wnd, "Неправильное значение.", errCaption, MB_OK | MB_TASKMODAL);
	  return 0;
	  //if(MessageBox(Wnd, "Неправильное значение. Продолжить?", errCaption, MB_YESNO | MB_TASKMODAL | MB_DEFBUTTON2) == IDNO) return 0;
    }

    double Min = -1;
    double Max = -1;

    char* Start = strstr(buffer, "свыше ");
    if(!Start) Start = strstr(buffer, "от ");
    if(Start) {
      while(*Start != ' ' && *Start) Start++;
      if(*Start) Min = atof(Start);
    }

    char* End = strstr(buffer, "до ");
    if(!End) End = strstr(buffer, "по ");
    if(End) {
      while(*End != ' ' && *End) End++;
      if(*End) Max = atof(End);
    }

    if((Min != -1 && atof(value) < Min) ||
       (Max != -1 && atof(value) > Max)) {
	  MessageBox(Wnd, "Неправильное значение.", errCaption, MB_OK | MB_TASKMODAL);
	  return 0;
//	  if(MessageBox(Wnd, "Неправильное значение. Продолжить?", errCaption, MB_YESNO | MB_TASKMODAL) == IDNO) return 0;
    }

    return 1;
}

int MandatoryDlg::CheckAvDups()
{
   for(int j = 0; j < avariaCount; j++)
	for(int i = j + 1; i < avariaCount; i++)
	  if(!strcmpi(avarias2[i].WorkNmb, avarias2[j].WorkNmb)) {
		  return 0;
	  }
   return 1;
}


void MandatoryDlg::IsFee2Check(RTMessage msg)
{
	ASS();
	int IsFee2State = (IsFee2)->GetCheck() == BF_CHECKED;
    if(!ModifyMandatory) return;
    CalculateSumma();
    if(!IsFee2State) {
       IsPay2->SetCheck(BF_UNCHECKED);
       IsPay2Check(msg);
    }
    EnableWindow(IsPay2->HWindow, IsFee2State == 1);
}

MandatoryDlg::~MandatoryDlg()
{
	ASS();
   HookWnd = 0;
   if(SupportHook) {
       UnhookWindowsHookEx(PrevProc);
	   FreeProcInstance((FARPROC)key_proc);
   }
   ((TRulerWin*)GetApplicationObject()->MainWindow)->msg_reciever = HWindow;
//   EnableMenuItem(GetSubMenu(GetMenu(Parent->HWindow), 2), 3, MF_BYPOSITION | MF_ENABLED);
   delete Rubl;
}

void MandatoryDlg::Print1CY_Pay2(RTMessage msg)
{
   if(m_1SU->GetSelIndex() < 1) return;
   if(!Dlg_base::SetDataToBase()) return;
	char* sss = strtok(buffer.Pay2RealDate, "./");
	if(!sss)
	{
		return;
	}
	Print1CY(buffer.Pay2RealDate, buffer.Pay2RubSumma, buffer.Fee2Date, buffer.ToDate, "Вторая часть взноса");
}

void MandatoryDlg::Print1CY(char* PayRealDate, char* PayRubSumma, char* from, char* to, char* what)
{
   if(!Dlg_base::SetDataToBase()) return;

   char* str = new char [3000];
   char* dest = str;

   memset(str, 0, 3000);

	char* sss = strtok(PayRealDate, "./");

	strcpy(dest, sss);
	dest += strlen(dest) + 1;

	char* monthNumber = strtok(0, "./");
	strcpy(dest, GetRCStr(99 + atoi(monthNumber)));
	dest += strlen(dest) + 1;

	char* year = strtok(0, "./");
	strcpy(dest, year + strlen(year) - 2);
	dest += strlen(dest) + 1;

	GetPrivateProfileString(MANDATORY_SECT, "Company", "", dest, 2500, ININame);
	dest += strlen(dest) + 1;

    sprintf(dest, "%s/%s от %s", buffer.Seria, buffer.Number, buffer.RegDate);
	dest += strlen(dest) + 1;

	strcpy(dest, Komplex ? "Комплексное" : "Обязательное");
	dest += strlen(dest);
	strcpy(dest, "страхование гражданской ответственности");
	dest += strlen(dest) + 1;

	sprintf(dest, "%s, %s", buffer.Name, buffer.Address);
	dest += strlen(dest) + 1;

	sprintf(dest, "%s за %s - %s", what, from, to);
	dest += strlen(dest) + 1;

	strcpy(dest, PayRubSumma);
	strcat(dest, "    (");
	strcat(dest, NumberToWords(atof(PayRubSumma)));
	strcat(dest, ")  ");
	strcat(dest, Rubl);
	dest += strlen(dest) + 1;

	AgName2->GetText(dest, 50);

   ((TRulerWin*)Parent)->LoadBlank("1CY.BLK");
   ((TRulerWin*)Parent)->SetValues(str);
   Dlg_base::Print1();
   ((TRulerWin*)Parent)->LoadBlank(0);

   delete str;
}

void MandatoryDlg::Print1CY_AddPay(RTMessage msg)
{
	char buff[20];
	if(!Dlg_base::SetDataToBase()) return;
	if(Komplex)
	{
		if(!*buffer.TaxiSumma || !buffer.Taxi1SU) return;
		strcpy(buff, buffer.TaxiPayDate);
		char Reason[20];
		m_TaxiVid->GetText(Reason, sizeof Reason);
		Print1CY(buffer.TaxiPayDate, buffer.TaxiSumma, buff, buffer.ToDate, Reason);
	}
	else
    {
		taxi_data data;
		if(!FillTaxiData(data) || !data._1SU) return;
		strcpy(buff, data.PayDate);
		Print1CY(data.PayDate, data.Summa, buff, buffer.ToDate, data._Taxi ? "Такси" : data._Avar ? "Авария " : data._AutoType ? "Тип авто" : "Доплата");
	}
}

struct AktDlgBuff {
	char Number[15];
	char PredstInsur[60];
	char Owner[60];
	char PlaceAddress[60];
	char Year[5];
	char Distance[7];
	char Document[60];
	char AddInfo[500];
	char Breaks[500];
	char Color[20];
};

class AktDlg : public TDialog {
	  TComboBox* Color;
	  TEdit* Place;
	  char* IniName;
public:
	  AktDlg(PTWindowsObject Parent, AktDlgBuff* buff, char* AIniName) : TDialog(Parent, "VIEWAKT")
	  {
      	new NumberEdit(this, 101, 15, 0);
		new TEdit(this, 102, 60);
		new TEdit(this, 109, 60);
		Place = new TEdit(this, 103, 60);
		new NumberEdit(this, 104, 5, 0);
		new NumberEdit(this, 106, 7, 0);
		new TEdit(this, 107, 60);
		new TEdit(this, 108, 500);
		new TEdit(this, 110, 500);
		Color = new TComboBox(this, 105, 20);
		Color->DisableTransfer();
		TransferBuffer = buff;
		this->IniName = AIniName;
	  }

	  void SetupWindow()
	  {
		TDialog::SetupWindow();
		char key[10];
		char Value[50];
		for(int i = 0, j = 0; i < 100; i++)
		{
			sprintf(key, "Color%d", i);
			if(GetPrivateProfileString(MANDATORY_SECT, key, "", Value, sizeof Value, IniName)) {
				AnsiUpper(Value);
				if(Color->FindExactString(Value, -1) < 0) {
					Color->AddString(Value);
				}
				WritePrivateProfileString(MANDATORY_SECT, key, 0, IniName);
				sprintf(key, "Color%d", j++);
				WritePrivateProfileString(MANDATORY_SECT, key, Value, IniName);
			}
		}
		 if(GetPrivateProfileString(MANDATORY_SECT, "AktPlace", "", Value, sizeof Value, IniName)) {
			Place->SetText(Value);
    	 }
	  }

	  void Ok(RTMessage msg)
	  {
		Color->GetText(((AktDlgBuff* )TransferBuffer)->Color, sizeof ((AktDlgBuff* )TransferBuffer)->Color);
		AnsiUpper(((AktDlgBuff* )TransferBuffer)->Color);

		if(Color->FindExactString(((AktDlgBuff* )TransferBuffer)->Color, -1) < 0) {
			char key[10];
			sprintf(key, "Color%d", Color->GetCount());
			WritePrivateProfileString(MANDATORY_SECT, key, ((AktDlgBuff* )TransferBuffer)->Color, IniName);
			Color->AddString(((AktDlgBuff* )TransferBuffer)->Color);
		}

		TDialog::Ok(msg);

		WritePrivateProfileString(MANDATORY_SECT, "AktPlace", ((AktDlgBuff* )TransferBuffer)->PlaceAddress, IniName);
	  }
};

void MandatoryDlg::PrintViewAct(RTMessage)
{
   if(!Dlg_base::SetDataToBase()) return;

   AktDlgBuff akt;
   memset(&akt, 0, sizeof akt);
   strcpy(akt.Number, buffer.Number);
	strcpy(akt.PredstInsur, buffer.Name);
	strcpy(akt.Owner, buffer.Name);
//	char PlaceAddress[60];
//	char Year[5];
//	char Distance[10];
//	char Document[15];
//	char AddInfo[500];
//	char Breaks[500];
//	char Color[20];
   AktDlg* dlg = new AktDlg(this, &akt, ININame);

   if(GetModule()->ExecDialog(dlg) != IDOK) return;

   char* str = new char [5000];
   char* dest = str;

   memset(str, 0, 5000);

   //Подразделение
	GetPrivateProfileString(MANDATORY_SECT, "Company", "", dest, 2500, ::ININame);
	dest += strlen(dest) + 1;

	char* sss = strtok(buffer.RegDate, "./");

    //День
	strcpy(dest, sss);
	dest += strlen(dest) + 1;

	//Месяц
	char* monthNumber = strtok(0, "./");
	strcpy(dest, GetRCStr(99 + atoi(monthNumber)));
	dest += strlen(dest) + 1;

	//Год (2 цифры)
	char* year = strtok(0, "./");
	strcpy(dest, year + strlen(year) - 2);
	dest += strlen(dest) + 1;

	//Номер
	strcpy(dest, akt.Number);
	dest += strlen(dest) + 1;

	//ФИО Адрес
	sprintf(dest, "%s, %s", buffer.Name, buffer.Address);
	dest += strlen(dest) + 1;

    //Дата заявления
	e[2]->GetText(buffer.RegDate, sizeof buffer.RegDate);
	strcpy(dest, buffer.RegDate);
	dest += strlen(dest) + 1;

	//Представитель страховщика
	strcpy(dest, akt.PredstInsur);
	dest += strlen(dest) + 1;

	//Дата время осмотра
	sss = strtok(buffer.RegDate, "./");

    //День
	strcpy(dest, sss);
	dest += strlen(dest) + 1;

	//Месяц
	monthNumber = strtok(0, "./");
	strcpy(dest, GetRCStr(99 + atoi(monthNumber)));
	dest += strlen(dest) + 1;

	//Год (2 цифры)
	year = strtok(0, "./");
	strcpy(dest, year + strlen(year) - 2);
	dest += strlen(dest) + 1;

    char* tm = strtok(buffer.FromTime, ":");
	strcpy(dest, tm == 0 ? "" : tm);
	dest += strlen(dest) + 1;

	tm = strtok(NULL, ":");
	strcpy(dest, tm == 0 ? "" : tm);
	dest += strlen(dest) + 1;

    //Адрес где осматривали!!!!!!!!
	strcpy(dest, akt.PlaceAddress);
	dest += strlen(dest) + 1;

	//Параметры авто
	strcat(dest, buffer.Marka);
	dest += strlen(dest) + 1;
	strcat(dest, *buffer.Chassis ? buffer.Chassis : buffer.NumberBody);
	strcat(dest, ", ");
	strcat(dest, buffer.AutoNumber);
	strcat(dest, ", ");
	strcat(dest, akt.Year);
	dest += strlen(dest) + 1;
	strcpy(dest, akt.Color); //ЦВЕТ
	strcat(dest, ", ");
	strcat(dest, akt.Distance); //СПИДОМЕТР
	strcat(dest, ", ");
	strcat(dest, akt.Document); //СЕРИЯ НОМЕР СВИДЕТЕЛЬСТВА и КОГДА И КЕМ ВЫДАНО
	dest += strlen(dest) + 1;

	//ДОП СВЕДЕНИЯ!!!!!!!!
	strcpy(dest, akt.AddInfo);
	dest += strlen(dest) + 1;

	//ВЛАДЕЛЕЦ!!!!!!!!
	strcpy(dest, buffer.Name);
	dest += strlen(dest) + 1;

	//ПОВРЕЖДЕНИЯ!!!!!!!!
	strcpy(dest, akt.Breaks);
	dest += strlen(dest) + 1;

	//АГЕНТ!!!!!!!!
	AgName1->GetText(dest, 50);
	dest += strlen(dest) + 1;

	//ВЛАДЕЛЕЦ!!!!!!!!
	strcpy(dest, buffer.Name);
	dest += strlen(dest) + 1;

   ((TRulerWin*)Parent)->LoadBlank("viewakt.BLK");
   ((TRulerWin*)Parent)->SetValues(str);
   Dlg_base::Print1();
   ((TRulerWin*)Parent)->LoadBlank(0);

   delete str;
}

void MandatoryDlg::Print1()
{
   int OwnPolis = m_OwnPolis->GetCheck() == 0;
   if(!OwnPolis) return;

   ASS();
   if(!SetDataToBase() && (GetAsyncKeyState(VK_CONTROL) & 0x8000) == 0) return;
   Dlg_base::SetDataToBase();

   char buff[16], key[32];
   sprintf(key, "TarifTable_Is_Resident%d", SendMessage(m_Resident->HWindow, CB_GETITEMDATA, m_Resident->GetSelIndex(), 0));
   GetPrivateProfileString(MANDATORY_SECT, key, "", buff, sizeof buff, ININame);
   int IsResident = strcmp(buff, "YES") == 0;

   strcpy(buff, buffer.FromDate);
   strcpy(key, buffer.ToDate);
   long IsYear = (GD(key) - GD(buff) >= 360);

   int PrintVersion = GetPrivateProfileInt(MANDATORY_SECT, "PrintVersion", 3, ININame);

   char* str = new char [3000];
   char* dest = str;

   e[2]->GetText(str, 20);
   DATE  regdate = GD(str, 0);
   memset(str, 0, 3000);

   double K3 = 1;
   char K3s[5]="";
   if((m_Fizich->GetCheck()||m_IP->GetCheck()) && regdate >= fstSept)
   {
	   m_K3->GetText(K3s, sizeof K3s);
	   K3 = atof(K3s);
   }

   double SuperK = 1. + atof(buffer.K1)-1.+K3-1.+(SendMessage(_m_OldClassAv->HWindow, CB_GETITEMDATA, _m_OldClassAv->GetSelIndex(), 0)/100.)-1.-atof(buffer.Discount)/100.;

   SuperK = int(SuperK*10000. + 0.0001) / 10000.;

   if(atof(buffer.Discount)>0.)
   	 SuperK = max(SuperK, 0.3);
   else
     SuperK = max(SuperK, 0.5);

     strcpy(dest, buffer.InsComp);
     dest += strlen(dest) + 1;
     strcpy(dest, buffer.Seria);
     dest += strlen(dest) + 1;
	 strcpy(dest, buffer.Number);
     dest += strlen(dest) + 1;

     strcpy(dest, buffer.Name);
     dest += strlen(dest) + 1;

	 if(GetPrivateProfileInt("MANDATORY", "FillAddr", 0, ::ININame)) {
		 DynStr bf;
		 for(int i = 0;; i++) {
			 for(int j = 0;; j++) {
				sprintf(key, "CityCode%d_%d", i, j);
				if(!GetPrivateProfileString(MANDATORY_SECT, key, "", bf, bf._sizeof(), ::ININame)) {
					if(j == 0) goto Next;
					break;
				}
				if(strstr(bf, buffer.CityCode)) {
					char* ptr = bf;
					if(strrchr(bf, ',')) {
						ptr = strrchr(bf, ',');
						*ptr++ = 0;
					}
					if(strchr(ptr, '(')) *strchr(ptr, '(') = 0;
					AnsiUpper(ptr);
					if(!strstr(ptr, "ГОРОД ")) {
						strcat(dest, ptr);
						strcat(dest, " ");
					}
					goto Next;
				}
			 }
		 }
	 }

	 Next:;

     strcat(dest, buffer.Address);
	 dest += strlen(dest) + 1;


   if(PrintVersion == 3) {
	 /* owners depricated
	 int PRINT_OWNERS = GetPrivateProfileInt("MANDATORY", "PrintOwners", 0, ININame);
	 if(PRINT_OWNERS && ownersCount > 0) {
       AddOwnerToBuffer(dest, 0, 2);
	 }
	 else
         strcat(dest, " ");
     dest += strlen(dest) + 1;

	 if(PRINT_OWNERS && ownersCount > 1) {
       AddOwnerToBuffer(dest, 1, 2);
     }
     else
	 strcat(dest, " ");
     dest += strlen(dest) + 1;

	 if(PRINT_OWNERS && ownersCount > 2) {
       AddOwnerToBuffer(dest, 2, 2);
     }
     else
	 strcat(dest, " ");
     dest += strlen(dest) + 1;
	 */
     PrintData(dest, 1, PrintVersion);
     PrintData(dest, 2, PrintVersion);

     if(strlen(buffer.NumberBody) > 3) 
		 strcpy(dest, buffer.NumberBody);
     else
     if(strlen(buffer.Chassis) > 3)
		 strcpy(dest, buffer.Chassis);
	 else
		 strcpy(dest, "");

	 if(*dest == 0)
	 {
		GetPrivateProfileString(MANDATORY_SECT, "БН_Кузов_Шасси", "", dest, 32, ININame);
	 }

     dest += strlen(dest) + 1;

     strcpy(dest, buffer.TarifGlass);
     dest += strlen(dest) + 1;

	 if(IsResident)
	 {
	 	if(!buffer.IsFee2)
	   		strcpy(dest, buffer.Rate);
	    else
		   GetPrivateProfileString(MANDATORY_SECT, "RateText", "<Курс>", dest, 25, ::ININame);
	 }
	 dest += strlen(dest) + 1;

	 if(IsResident) strcpy(dest, buffer.K1);
     dest += strlen(dest) + 1;

	 if(IsResident && (IsYear || GetPrivateProfileInt(MANDATORY_SECT, "K2Print", 1, ::ININame)))
	 {
		sprintf(dest, "%0.2f", SendMessage(_m_OldClassAv->HWindow, CB_GETITEMDATA, _m_OldClassAv->GetSelIndex(), 0)/100.);
		while(dest[strlen(dest)-1] == '0' || dest[strlen(dest)-1]=='.')
		dest[strlen(dest)-1]=0;
	 }

     dest += strlen(dest) + 1;

	 strcpy(dest, K3s);
	 dest += strlen(dest) + 1;

	 if(IsResident)
	 {
		strcpy(dest, buffer.Discount);

/*		if(bPrintOld) {
		    if(fabs(atof(buffer.Discount)) > 0.01)
				sprintf(dest + strlen(dest), "; ");
 		    sprintf(dest + strlen(dest), "%c%g", (SuperK-1.>0?'+':'-'),fabs(SuperK-1.));
		}
        else */{
			dest += strlen(dest) + 1;
			sprintf(dest, "%c%g", ((SuperK-1.)==0. ? ' ' : SuperK-1.>=0.00001?'+':'-'),fabs(SuperK-1.));
		}
	 }
	 else
	 {
	  /*if(!bPrintOld)*/ dest += strlen(dest) + 1;
     }
     dest += strlen(dest) + 1;


	 PrintData(dest, 3, PrintVersion);

	 AgName1->GetText(dest, 30);
	 char* p1 = strtok(dest, " ");
	 char* p2 = p1 ? strtok(NULL, ". ") : 0;
	 char* p3 = p2 ? strtok(NULL, ". ") : 0;
	 char* p4 = p3 ? strtok(NULL, ". ") : 0;

	 if(p4 == 0 && p3 != 0) {
	 	char s3[3];
	 	if(p3 != 0)
	 	{
			p3[1] = 0;
			strcpy(s3, p3);
	 	}
	 	if(p1 != 0 && p2 != 0)
	 	{
			p2[1] = 0;
			char s[3]; strcpy(s, p2);
			strcat(p1, " ");
			strcat(p1, s);
			strcat(p1, ".");
	 	}
	 	if(p1 != 0 && p3 != 0)
	 	{
			strcat(p1, " ");
			strcat(p1, s3);
			strcat(p1, ".");
		}
	 }
	 else {
		 AgName1->GetText(dest, 30);
	 }
   }

   if(m_Vzamen->GetSelIndex() == 0) {
     //((TComboBox*)e[3])->GetText(buffer.PrevSeria, sizeof buffer.PrevSeria);
     *buffer.PrevSeria = 0;
     *buffer.PrevNumber = 0;
   }
   dest += strlen(dest) + 1;
   strcpy(dest, buffer.PrevSeria);
   dest += strlen(dest) + 1;
   strcpy(dest, buffer.PrevNumber);

   ((TRulerWin*)Parent)->SetValues(str);
   Dlg_base::Print1();
   delete str;
}

void MandatoryDlg::ChangeStartDate(RTMessage msg)
{
	ASS();
   	if(ModifyMandatory)
     	if(msg.LP.Hi == EN_CHANGE) {
			CalculatePeriod();
			CalculateSumma();
	     }
}

DATE aux_incDate(DATE startdate, int inc1, int value);
int getDurationType(char* str, char** = 0);

void MandatoryDlg::CalculatePeriod()
{
	ASS();
   if(UpdateFormData) return;
   if(m_OwnPolis->GetCheck() == 1) return;

   date d;
   getdate(&d);
   DATE CurrDate;
   PXDateEncode(d.da_mon, d.da_day, d.da_year, &CurrDate);

   char buffer[64];
   char buffer2[64];
   e[5+2]->GetText(buffer, sizeof buffer);
   Period->GetText(buffer2, sizeof buffer2);
   DATE date = GD(buffer, 1);
   if(date && strlen(buffer2)) { //Период изменилс

	  if(ModifyMandatory == 1)
	  {
	  	//Время меняется только при вводе нового полиса
		InitTime(e[6+2]);
	  }

      //Не надо сбрасывать время, если безнал и даты совпадают

      //int d, m, y;
      //PXDateDecode(date, &m, &d, &y);
	  //int Value = atoi(buffer2);
	  AnsiLower(buffer2);


	   DATE enddate = aux_incDate(date, getDurationType(buffer2), atoi(buffer2));
       int d, m, y;
       PXDateDecode(--enddate, &m, &d, &y);
	   sprintf(buffer, "%02u.%02u.%04u", d, m, y);
	  // m_dtTo->SetText(datebuf);

          /*
      if(strstr(buffer2, "го") || strstr(buffer2, "ле"))  y += Value;
      if(strstr(buffer2, "мес")) {
			m += Value;
			if(m > 12) {
	  			m -= 12;
           		y++;
			}
      }

      int IsDec = 0;
	  while(PXDateEncode(m, d, y, &date) != PXSUCCESS)
	  {
		IsDec = 1;
		d--;
      }

      if(strstr(buffer2, "дн") || strstr(buffer2, "сут"))
		date += Value;

      if(IsDec == 0)
	 	date -= 1;

      PXDateDecode(date, &m, &d, &y);
	  wsprintf(buffer, "%02d.%02d.%04d", d, m, y);
	  assert(strlen(buffer) < sizeof buffer);*/
      e[7+2]->SetText(buffer);
   }
}


//Извлекает тариф из БД и расчитывает K1(местность) K2(аварийность) и tarif на основе аварийности
void MandatoryDlg::Calculate()
{
	ASS();
   if(UpdateFormData) return;
   if(m_OwnPolis->GetCheck() == 1) return;

   CalculatePeriod();

   double tarifSumma = 0;
   int Index = Super->GetSelIndex();
   if(Index != -1) {
     long superData = SendMessage(Super->HWindow, CB_GETITEMDATA, Index, 0);
     int periodData = Period->GetSelIndex();
     if(periodData != -1) {
	DynStr _listTarif;
	char Key[32];

	const char* Key1Tarif = "";
	if(m_Resident->GetSelIndex() >= 0)
	   Key1Tarif = (String&)m_TarifKeyNames[m_Resident->GetSelIndex()];
	wsprintf(Key, Key1Tarif, (HIWORD(superData) & 0x00FF), (short)superData);
	assert(strlen(Key) < sizeof Key);
	if(!GetPrivateProfileString(MANDATORY_SECT, Key, "", _listTarif, _listTarif._sizeof(), ININame)) {
	     MessageBox(HWindow, GetRCStr(588), "Ошибка 1", MB_OK | MB_ICONSTOP | MB_TASKMODAL);
	     return;
	}

	for(int i = 0; i <= periodData; i++) {
	   char* tarif = strtok(i ? NULL : (char*)_listTarif, ",");
	   if(!tarif) {
	     MessageBox(HWindow, GetRCStr(589), "Ошибка 2", MB_OK | MB_ICONSTOP | MB_TASKMODAL);
	   }
           else
	     tarifSumma = atof(tarif);
	}
      }
   }
   //ТУТ ИМЕЕМ ТАРИФ ИЗ БАЗЫ ДАННЫХ
   char Value[32];

   if(Version == 1) {
      DblToStr(Value, tarifSumma, 2);
      e[17+2]->SetText(Value);
      m_TarifGlass->SetText(Value); //SET Tarif glass
      m_K1->SetText("");
      //m_K2->SetText("");
   }
   if(Version == 2) {
   // Calculate K
      int Counter = 1;
      double sumKPlace = SendMessage(m_Placement->HWindow, CB_GETITEMDATA, m_Placement->GetSelIndex(), 0) / 100.;

	  char str[12], strDate[12];
	  GetDlgItemText(HWindow, 52, str, sizeof str);

	  DATE RegDate = GD(str);

/*      for(int i = 0; i < ownersCount; i++) {
		  strcpy(strDate, autoowners[i].RegDate);
		  if(GD(strDate) == RegDate) {
		     sumKPlace += autoowners[i].K;
			 Counter++;
          }
      }*/
      double K1 = sumKPlace / Counter;
  
      long Rounder = GetPrivateProfileInt(MANDATORY_SECT, "K1_Round", -1, ::ININame);
      if(Rounder > 0)
		K1 = long(K1 * Rounder + 0.501) / (double)Rounder;

      DblToStr(Value, tarifSumma, 2);
      m_TarifGlass->SetText(Value); //SET Tarif glass

      sprintf(Value, "%g", K1);
      assert(strlen(Value) < sizeof Value);
      m_K1->SetText(Value);

	  double K2 = SendMessage(_m_OldClassAv->HWindow, CB_GETITEMDATA, _m_OldClassAv->GetSelIndex(), 0)/100.;

	  char buff[16], key[32];
	  sprintf(key, "TarifTable_Is_Resident%d", SendMessage(m_Resident->HWindow, CB_GETITEMDATA, m_Resident->GetSelIndex(), 0));
      GetPrivateProfileString(MANDATORY_SECT, key, "", buff, sizeof buff, ININame);
	  int IsResident = strcmp(buff, "YES") == 0;
	  if(IsResident) {
		//if(GetPrivateProfileInt("MANDATORY", "NewCalculate", 1, ININame)) {
			double K3 = 1;
			if((m_Fizich->GetCheck()||m_IP->GetCheck()) && RegDate>=fstSept)
			{
				m_K3->GetText(str, sizeof str);
				K3 = atof(str);
            }
		   e[19+2]->GetText(buff, sizeof buff); //discount
		   double DiscK = max(0., atof(buff)/100.);
		   if(DiscK > 1.) DiscK = 1.;
		   double SuperK = max(0.5, (1. + (K1 - 1.) + (K2 - 1.) + (K3 - 1.) - DiscK));

		   if(DiscK > 0.)  {
			   SuperK = max(0.3, (1. + (K1 - 1.) + (K2 - 1.) + (K3 - 1.) - DiscK));
		   }

		   tarifSumma = tarifSumma * SuperK;
		//}
        //else {
		//   tarifSumma = tarifSumma * K1 * K2;
		//}
	  }
      Rounder = GetPrivateProfileInt(MANDATORY_SECT, "TarifWithK", -1, ::ININame);
      if(Rounder > 0)
	      tarifSumma = long(tarifSumma * Rounder + 0.501) / (double)Rounder;

	  DblToStr(Value, tarifSumma, 2);
      e[17+2]->SetText(Value); //SET Tarif
   }

   CalculateSumma();
}

void MandatoryDlg::ChangeSumma1(RTMessage msg) //Rate
{
     if(msg.LP.Hi != EN_CHANGE)
        return;
     CalculateSumma();
}

void MandatoryDlg::ChangeSumma2(RTMessage msg) //Discount
{
	ASS();
	 if(msg.LP.Hi != EN_CHANGE)
      return;
	 Calculate();
}

void MandatoryDlg::K3Change(RTMessage msg)
{
	if(msg.LP.Hi == CBN_SELCHANGE && ModifyMandatory)
		Calculate();
}

void MandatoryDlg::CalculateSumma()
{
   ASS();
   char buffer[32];
   char buffer2[32];
   char buffer3[32];
   char glasstarif[32];
   char K3s[5];
   e[2]->GetText(buffer3, sizeof buffer3);
   DATE regdate = GD(buffer3, 1);

   if(UpdateFormData) return;
   if(m_OwnPolis->GetCheck() == 1) return;

   char buff[16], key[32];
   sprintf(key, "TarifTable_Is_Resident%d", SendMessage(m_Resident->HWindow, CB_GETITEMDATA, m_Resident->GetSelIndex(), 0));
   GetPrivateProfileString(MANDATORY_SECT, key, "", buff, sizeof buff, ININame);
   int IsResident = strcmp(buff, "YES") == 0;


   int IsFee2State = (IsFee2)->GetCheck() == BF_CHECKED;
   ShowWindow(m_OwnPolis->HWindow, IsFee2State ? SW_SHOW : SW_HIDE);
   if(!IsFee2State)
   {
   	m_OwnPolis->SetCheck(0);
	TMessage msg;
	CMOwnPolis(msg);
   }
   int IsNal = Nal->GetCheck() == BF_CHECKED;   //Если оплачено по безналу, то сумма округляется до 0.1 копеек

   m_TarifGlass->GetText(glasstarif, sizeof glasstarif);
   e[17+2]->GetText(buffer, sizeof buffer);  //tarif
   e[18+2]->GetText(buffer2, sizeof buffer2); //rate
   e[19+2]->GetText(buffer3, sizeof buffer3); //discount
   m_K3->GetText(K3s, sizeof K3s); 


   double AllSumma;
   double Rate = atof(buffer2);
   double Discount = atof(buffer3);
   double Tarif = atof(buffer);
   double GlassTarif = atof(glasstarif);
   double K3 = atof(K3s);

   char KText[10];
   m_K1->GetText(KText, sizeof KText);
   double K1 = atof(KText);
   double K2 = SendMessage(_m_OldClassAv->HWindow, CB_GETITEMDATA, _m_OldClassAv->GetSelIndex(), 0)/100.;//atof(K2Text);


//QQQ
   int IsFiz_ = (m_Fizich->GetCheck()||m_IP->GetCheck());

   if(!IsFiz_ || regdate <= fstSept)
   {
       K3 = -1;
   }

   //int NewCalc = GetPrivateProfileInt("MANDATORY", "NewCalculate", 1, ININame);
   {

	   if(Discount > 100.) {
		  e[19+2]->SetText("100");
		  Discount = 100;
	   }
	   if(Discount < 0) {
		  e[19+2]->SetText("0");
		  Discount = 0;
	   }
                          
	   //Подлежит уплате
	   //if(NewCalc)
	   {
		   AllSumma = Tarif;// GlassTarif * (1 + (K1 - 1) + (K2 - 1) - Discount / 100.);
	   }
	   //else
	   //{	
	   //	   AllSumma = Tarif * (1 - Discount / 100.);
       //    }
   }

   int Percent = GetPrivateProfileInt(MANDATORY_SECT, "Fee1Is", 50, ININame);

   int IsRounded = 1;

   if(!IsFee2State) { //1 Pay => rubli
       AllSumma *= Rate;

       //Наличку округляет по сумме из реестра
       if(IsNal) {
		 if(IsRounded) ToRubles(AllSumma);
       }
       else { //Без нал
		 if(IsRounded)
			AllSumma = floor(AllSumma + 0.5); //Надо бы применить ф-ию из реестра
		 else
			AllSumma = long(AllSumma * 10 + 0.00001) / 10.;	  //AllSumma = long(AllSumma + 0.5);
	   }
   }

   DblToStr(buffer, AllSumma, 2);
//   sprintf(buffer, "%f", AllSumma);

   //MessageBox(0,"1",buffer,0);

   //QQQQQQQQ
  double SuperK = 0;//K1 * K2 * (1.-Discount / 100.); --old calc
/*   if(NewCalc) */SuperK = 1 + K1-1. + K2-1. + fabs(K3)-1. - Discount / 100.;
   if(Discount == 0) {
		//if(!NewCalc && SuperK >= 0.5 && GlassTarif > 0) {
		//	SuperK = Tarif * (1. - Discount / 100.) / GlassTarif;
		//}
		//else
		   SuperK = max(SuperK, 0.5);
   }
   else {
		//if(!NewCalc && SuperK >= 0.3 && GlassTarif > 0) {
		//	SuperK = Tarif * (1. - Discount / 100.) / GlassTarif;
		//}
		//else
		SuperK = max(SuperK, 0.3);
   }
                      
   //QQQQQQQQQQQQ
   if(!IsResident) SuperK = 1.;
   SuperK = long(SuperK*10000. +0.0001)/10000.;

   int OwnPolis = m_OwnPolis->GetCheck() == 0;

   if(Pay1Curr->GetSelIndex() > 1)
   { //ANY CURRENCY
	   e[24+2]->GetText(buffer, sizeof buffer);
	   //MessageBox(0, buffer, buffer, 0);
	   DATE PayDate = GD(buffer);
	   Pay1Curr->GetText(buffer, sizeof buffer);
	   strcat(buffer, "_NAL");
	   double CurrRate = GetRateCurrency(PayDate, buffer);
	   if(CurrRate == 0) CurrRate = -1000000;
	   ((PTEdit)ChildWithId(114))->GetText(buffer, sizeof buffer);
	   ((PTEdit)ChildWithId(117))->SetText(buffer); //All fee - подлежит уплдате страховой взнос
	   DblToStr(buffer, atof(buffer)*Rate/CurrRate);
	   if(IsFee2State) DblToStr(buffer, long(atof(buffer)/2. * 100 + 0.501) / 100.0);
	   ((PTEdit)ChildWithId(119))->SetText(buffer); //All fee -Упалчен в сумме

	  e[21+2]->SetText("");
	  e[23+2]->SetText("");
	  TMessage msg;msg.LP.Hi=EN_CHANGE;
	  SetFocus( e[22+2]->HWindow);
	  ChangePay1(msg);
   }

   if(Pay1Curr->GetSelIndex() == 1)
   { //EUR
	 ((PTEdit)ChildWithId(114))->GetText(buffer, sizeof buffer);
	 ((PTEdit)ChildWithId(117))->SetText(buffer); //All fee - подлежит уплдате страховой взнос
	 if(IsFee2State) DblToStr(buffer, long(atof(buffer)/2. * 100 + 0.501) / 100.0);
	 ((PTEdit)ChildWithId(119))->SetText(buffer); //All fee -Упалчен в сумме

	e[21+2]->SetText("");
	e[23+2]->SetText("");
	TMessage msg;msg.LP.Hi=EN_CHANGE;
	SetFocus( e[22+2]->HWindow);
	ChangePay1(msg);
   }
  
   if(Pay1Curr->GetSelIndex() == 0)
   {
   	//Платим рублями
//strcat(buffer, "~");
	 e[20+2]->SetText(buffer); //All fee - подлежит уплдате страховой взнос

	 if(IsFee2State)
		e[21+2]->SetText("евро"); //ДВА ЭТАПА
	 else {
		//if(m_VersionCombo->GetSelIndex() == 0) {
		//   e[21+2]->SetText(GetSummaText(AllSumma, Rubl)); //- сумма в рублях
		//}
		//else { //ОДИН ЭТАП
		   char textbuff[64];
		   m_TarifGlass->GetText(textbuff, sizeof textbuff);
		   if(!IsResident)
		   {
			   DblToStr(textbuff, AllSumma, 2);
			   sprintf(textbuff + strlen(textbuff),  " = %g x %g", GlassTarif, Rate);
		   }
		   else
           //if(NewCalc)
		   {
			   DblToStr(textbuff, AllSumma, 2);
			   sprintf(textbuff + strlen(textbuff),  " = %g x (1 %c %g) x %g", GlassTarif, (SuperK-1) < 0 ? '-' : '+', fabs(SuperK-1.), Rate);
		   }
		   /*else
		   {
  		       strcat(textbuff + strlen(textbuff), " x ");
		       m_K1->GetText(textbuff + strlen(textbuff), sizeof textbuff);
		       strcat(textbuff + strlen(textbuff), " x ");
			  double K2 = SendMessage(_m_OldClassAv->HWindow, CB_GETITEMDATA, _m_OldClassAv->GetSelIndex(), 0)/100.;//atof(K2Text);
			   sprintf(textbuff + strlen(textbuff), "%0.2f", K2);
		       if(Discount > 0)
			       sprintf(textbuff + strlen(textbuff),  " x %g%c", Discount, '%');
		       double _Tarif_ = Tarif;
		       if(Discount > 0) {
			       _Tarif_ = Tarif * (1 - Discount / 100);
			       _Tarif_ = long(_Tarif_ * 100 + 0.5) / 100.;
               }
		       sprintf(textbuff + strlen(textbuff),  " = %g Евро", _Tarif_);
		   }*/
		   e[21+2]->SetText(textbuff); // формула
		//}
	 }
   }

   AllSumma = floor(GlassTarif * SuperK * 1000.0) / 1000.0;

   if(!IsFee2State)  //1 Pay => rubli
       AllSumma *= Rate;
   /////////////////

   //Уплачен в сумме
   if(IsFee2State)
   { // 2 Pay
      //translate to rubles
      //Процент от первой части

      AllSumma *= (Percent / 100.);
      AllSumma = long(AllSumma * 100 + 0.501) / 100.0;
      AllSumma *= Rate;

      if(IsNal) {
		 if(IsRounded) ToRubles(AllSumma);
      }
      else {
	 if(IsRounded)
		 AllSumma = floor(AllSumma + 0.501); //Надо бы применить ф-ию из реестра
	   else
		 AllSumma = long(AllSumma * 10 + 0.00001) / 10.;	  //AllSumma = long(AllSumma + 0.5);
      }

      DblToStr(buffer, AllSumma, 2);
      if(Pay1Curr->GetSelIndex() == 0)
      {
		e[22+2]->SetText(OwnPolis ? buffer : "0");
        char buff2[32];
        DynStr buffer;
        strcpy(buffer, GetSummaText(AllSumma, Rubl));
		strcat(buffer, ", (");
		DblToStr(buff2, GlassTarif * SuperK * (Percent / 100.), 2);
		strcat(buffer, buff2);
		strcat(buffer, " евро х ");
		e[18+2]->GetText(buff2, sizeof buff2);
		strcat(buffer, buff2);
		strcat(buffer, ")");
		e[23+2]->SetText(buffer);
      }
   }
   else { //Один этап
	  if(Pay1Curr->GetSelIndex() == 0) {

	   if(IsNal) {
		 if(IsRounded) ToRubles(AllSumma);
       }
	   else { //Без нал
		 if(IsRounded)
			AllSumma = floor(AllSumma + 0.5); //Надо бы применить ф-ию из реестра
		 else
			AllSumma = long(AllSumma * 10 + 0.00001) / 10.;	  //AllSumma = long(AllSumma + 0.5);
	   }

		DblToStr(buffer, AllSumma, 2);
   //sprintf(buffer, "%f", AllSumma);
//		strcat(buffer, "!");
		e[22+2]->SetText(buffer);
	    e[23+2]->SetText(GetSummaText(atof(buffer), Rubl));
	  }
   }

   //=====================================
   //Pay2 Date and summa (all in euro)

   if(!IsFee2State) {
       //Pay 2 summa text date
       e[26+2]->SetText("");
       e[27+2]->SetText("");
       e[28+2]->SetText("");
   }
   else {
	   e[28+2]->SetText(""); //pay2 date

	   double Fee1Summa = GlassTarif * SuperK * (Percent / 100.);
	   Fee1Summa = long(Fee1Summa * 100 + 0.501) / 100.0;
	   //double Fee2Summa = GlassTarif * SuperK * ((100 - Percent) / 100.);
	   double Fee2Summa = GlassTarif * SuperK - Fee1Summa;
	   DblToStr(buffer, Fee2Summa, 2);
	   e[26+2]->SetText(buffer);
	   e[27+2]->SetText("евро");

       Period->GetText(buffer, sizeof buffer);
       AnsiLower(buffer);
	   if(strstr(buffer, "го") || strstr(buffer, "ле"))
	   {
           	DynStr _buffdate;
	   		e[5+2]->GetText(_buffdate, _buffdate._sizeof());
		   	DATE date = GD(_buffdate, 1);
			if(!date)
			{
	       		SetFocus(e[5+2]->HWindow);
               	return;
	   		};

           int d, m, y;
		   PXDateDecode(date, &m, &d, &y);
           m += 6;
		   if(m > 12)
		   {
               m -= 12;
               y++;
	       }
           int IsDec = 0;
		   while(PXDateEncode(m, d, y, &date) != PXSUCCESS)
		   {
	           d--;
			   IsDec = 1;
	       }
		   PXDateDecode(date - (IsDec == 0 ? 1 : 0), &m, &d, &y); //Если не уменьшали то уменьшить на 1 день
	       wsprintf(_buffdate, "%02d.%02d.%04d", d, m, y);
		   e[28+2]->SetText(_buffdate); //pay2 date
	   }
   }
}

/////////////////////////////////////

void MandatoryDlg::InitTime(TEdit* edt)
{
   //if(GetWindowTextLength(e->HWindow) > 0) return;

   char buff[32];
   e[2]->GetText(buff, sizeof buff);
   DATE regdate = GD(buff, 1);
   ((TEdit*)ChildWithId(102))->GetText(buff, sizeof buff);
   DATE fromdate = GD(buff, 1);
   struct date d;
   getdate(&d);
   DATE today;
	PXDateEncode(d.da_mon, d.da_day, d.da_year, &today);

   //if(BezNal->GetCheck()) {
   //		edt->SetText("00:00");
   //		return;
   //}

   if(regdate + 7 < today) //too old polis
   		return;

   if(regdate != fromdate) {
   		edt->SetText("00:00");
		return;
   }
   struct  time t;
   gettime(&t);
   sprintf(buff, "%2d:%02d", t.ti_hour, t.ti_min);
   assert(strlen(buff) < sizeof buff);

   edt->SetText(buff);
}

void MandatoryDlg::InitDates()
{
	ASS();
   date d;
   getdate(&d);
   DynStr buff;
   wsprintf(buff, "%02u.%02u.%04u", d.da_day, d.da_mon, d.da_year);
   e[2]->SetText(buff); //Дата регистрации
   e[5+2]->SetText(buff); //Дата С
   e[24+2]->SetText(buff); //Дата 1 pay
}

void MandatoryDlg::Clear()
{
	ASS();

   UpdateFormData++;
//   EnableAllControls();

   ClearAvarias(0);
   ClearPhotoList();
//   ClearOwners(0);

   //SetVersion(2);
   SetState(STATE_POLIS_NORMAL);

   for(int i = 0; i < MANDATORY_ED_COUNT; i++)
     e[i]->SetText("");

   e[6+2]->SetText("");
   m_StopDate->SetText("");
   m_RetBRB->SetText("");
   m_RetEUR->SetText("");

   m_MobilePhone->SetText("");
   m_HomePhone->SetText("");
   m_Contact->SetText("");
   m_SMS->SetCheck(0);

   //m_K2->SetText("1");
   for(i = 0; i < _m_OldClassAv->GetCount(); i++)
	if(SendMessage(_m_OldClassAv->HWindow, CB_GETITEMDATA, i, 0) == 100) _m_OldClassAv->SetSelIndex(i);

   m_Vzamen->SetSelIndex(-1);
   VzamenFuncX();

   Pay1Curr->SetSelIndex(0);  //BRB
   Pay2Curr->SetSelIndex(-1);
              
   InitDates();

   if(Komplex)
   {
	m_TaxiInOut->SetSelIndex(0);
    TMessage msg;
	ChangeInOut(msg);
   }

   char buff[64];

   m_K3->SetSelIndex(-1);
   if(GetPrivateProfileString(MANDATORY_SECT, "K3DefIndex", "", buff, sizeof buff, ::ININame)) {
	m_K3->SetSelIndex(atoi(buff));
   }
   
   if(m_K3->GetSelIndex() < 0) {
	for(int i = 0; i < m_K3->GetCount(); i++) {
	 SendMessage(m_K3->HWindow, CB_GETLBTEXT, i, (LPARAM)(char*)buff);
	    if(strcmp(buff, "1") == 0) {
		m_K3->SetSelIndex(i);
            	break;
            }
        }
   }

   if(m_K3->GetSelIndex() < 0) {
	MessageBox(0, "K3", "!", 0);
   }

   if(GetPrivateProfileString(MANDATORY_SECT, "Seria", "", buff, sizeof buff, ININame))
      e[0]->SetText(buff);

   if(GetPrivateProfileString(MANDATORY_SECT, "CityCodeDef", "", buff, sizeof buff, ::ININame))
      m_CityEdit->SetText(buff);

   if(!strlen(CurrentRateEUR))
      GetPrivateProfileString(MANDATORY_SECT, "LastRate", "", CurrentRateEUR, sizeof CurrentRateEUR, ::ININame);

   e[18+2]->SetText(CurrentRateEUR); //Rate

   SetFocus(e[1]->HWindow);

   int DefPeriod = GetPrivateProfileInt(MANDATORY_SECT, "DefPeriod", -1, ININame);
   Period->SetSelIndex(DefPeriod);
   CalculatePeriod();

   GetPrivateProfileString(MANDATORY_SECT, "Company", "", buff, sizeof buff, ::ININame);
   e[29+2]->SetText(buff);

   GetPrivateProfileString(MANDATORY_SECT, "DefAddress", "", buff, sizeof buff, ::ININame);
   e[4+2]->SetText(buff);

   GetPrivateProfileString(MANDATORY_SECT, "DefAVClass", "B2", buff, sizeof buff, ::ININame);
   _m_OldClassAv->SetText(buff);
   GetPrivateProfileString(MANDATORY_SECT, "DefAVCount", "99", buff, sizeof buff, ININame);
   _m_CountAv->SetText(buff);
   GetPrivateProfileString(MANDATORY_SECT, "DefPlacement", "", buff, sizeof buff, ::ININame);
   m_Placement->SetSelIndex(m_Placement->FindExactString(buff, -1));
   //m_1SU->SetSelIndex(GetPrivateProfileInt(MANDATORY_SECT, "Def1SU", -1, ININame));

   TMessage msg;

   if(ActivateParadox && GetPrivateProfileString(MANDATORY_SECT, "DefAgent", "", buff, sizeof buff, ININame)) {
	  SetAgent(buff, agents, AgName1, 0);
	  msg.LP.Hi = CBN_SELCHANGE;
	  ChAgent1(msg);
   }
   m_UrFizich1->SetText("Физич.");

   //EnableWindow(m_VersionCombo->HWindow, 1);

   Change(msg);
   ModifyMandatory = 1; //Новый бланк

   for(i = 0; i < m_Resident->GetCount(); i++)
	   if(GetPrivateProfileInt(MANDATORY_SECT, "DefTarifTable", 0, ININame) == SendMessage(m_Resident->HWindow, CB_GETITEMDATA, i, 0)) {
			m_Resident->SetSelIndex(i);
            break;
       }
   FillSuperCombo(-1, 1);

   UINT DefVladen = GetPrivateProfileInt(MANDATORY_SECT, "DefBase_of_possession", -1, ::ININame);
   if(DefVladen >= 0) {
      for(int i = 0; i < m_Osnovanie->GetCount(); i++)
	if(SendMessage(m_Osnovanie->HWindow, CB_GETITEMDATA, i, 0) == DefVladen * 100)
	   m_Osnovanie->SetSelIndex(i);
   }

   m_OwnPolis->SetCheck(0);
   CMOwnPolis(msg);

   UpdateFormData--;

   NalFunc(msg);
   IsFee2->SetCheck(BF_UNCHECKED);
   m_ToReport->SetCheck(BF_CHECKED);
   IsFee2Check(msg);
   Fizich(msg);

   msg.LP.Hi = CBN_SELCHANGE;
   Placement(msg);
}

void MandatoryDlg::WMCtlColor(RTMessage msg)
{
   Dlg_base::WMCtlColor(msg);
   if(msg.LP.Hi == CTLCOLOR_STATIC && (HWND)msg.LP.Lo == GetDlgItem(HWindow, 2000))
	SetTextColor((HDC)msg.WParam, RGB(255, 0, 0));
   if(msg.LP.Hi == CTLCOLOR_BTN && ((HWND)msg.LP.Lo == GetDlgItem(HWindow, 201) || (HWND)msg.LP.Lo == GetDlgItem(HWindow, 202) || (HWND)msg.LP.Lo == GetDlgItem(HWindow, 85)))
	SetTextColor((HDC)msg.WParam, RGB(0, 0, 255));
   if(msg.LP.Hi == CTLCOLOR_BTN && (HWND)msg.LP.Lo == GetDlgItem(HWindow, 208))
	SetTextColor((HDC)msg.WParam, RGB(0, 128, 128));
   if(msg.LP.Hi == CTLCOLOR_STATIC && ((HWND)msg.LP.Lo == GetDlgItem(HWindow, 9000) || (HWND)msg.LP.Lo == GetDlgItem(HWindow, 9125)))
	SetTextColor((HDC)msg.WParam, RGB(0, 0, 128));
}

void MandatoryDlg::Continue(RTMessage msg)
{
	UpdateFormData++;
	struct date d;
	getdate(&d);
    char buffer[32];

	m_Vzamen->SetSelIndex(1);
	VzamenFuncX();

	//Number
	e[1]->GetText(buffer, sizeof buffer);
 	e[4]->SetText(buffer);

    //Seria
	e[0]->GetText(buffer, sizeof buffer);
 	e[3]->SetText(buffer);

	wsprintf(buffer, "%02d.%02d.%04d", d.da_day, d.da_mon, d.da_year);
	e[1]->SetText("");
	e[2]->SetText(buffer);

	if(IsFee2->GetCheck() == 0 || IsPay2->GetCheck() == 1)
	{
	  ((TEdit*)ChildWithId(104))->GetText(buffer, sizeof buffer);
	  DATE todate = GD(buffer, 1) + 1;

	  int m, d, y;
	  PXDateDecode(todate, &m, &d, &y);
	  sprintf(buffer, "%02u.%02u.%04u", d, m, y);

	}
	((TEdit*)ChildWithId(102))->SetText(buffer);

	if(ListBAvaria->GetCount() == 0)
	{
		if(IsFee2->GetCheck() == 0 || IsPay2->GetCheck() == 1) //one pay or 2pays complete
			if(_m_OldClassAv->GetSelIndex()>0)
				_m_OldClassAv->SetSelIndex(_m_OldClassAv->GetSelIndex()-1);
	}
	else
	{
			_m_OldClassAv->SetSelIndex(_m_OldClassAv->GetSelIndex()+1);
	}
	UpdateFormData--;
	ClearAvarias(0);
	ClearPhotoList();
	m_Vzamen->SetSelIndex(-1);
	IsPay2->SetCheck(0);
	Change(msg);
	Change(msg);

/*	for(int i = 0; i < ownersCount; i++)
	{
		autoowners[i].IsSaved = 0;
	  	autoowners[i].OwnerCode = 0;
	}
*/
	m_Vzamen->SetSelIndex(1);
}

void MandatoryDlg::Change(RTMessage msg)
{
	ASS();
   ModifyMandatory = 1;
   int State = IsWindowEnabled(GetDlgItem(HWindow, 406));
   EnableWindow(GetDlgItem(HWindow, 58), FALSE);
   EnableWindow(GetDlgItem(HWindow, 406), TRUE);
   if(State) Calculate();
}

void MandatoryDlg::ChangePeriod(RTMessage msg)
{
	ASS();
   if(ModifyMandatory) Calculate();
}

void MandatoryDlg::ChangeSuper(RTMessage msg)
{
	ASS();

//	if(msg.LP.Hi != CBN_SELCHANGE) return;
//   if(UpdateFormData) return;
   //Combo box contain codes <Буква>, <Type>, 0, <SubType>,
   int Index = Super->GetSelIndex();
   if(Index == -1 || !ModifyMandatory) return;

   if(::IsLetter(SendMessage(Super->HWindow, CB_GETITEMDATA, Index, 0)) != 0)
     if(Index < (Super->GetCount() - 1)) //Exist next items(strings)
       if(::Type(SendMessage(Super->HWindow, CB_GETITEMDATA, Index, 0)) == ::Type(SendMessage(Super->HWindow, CB_GETITEMDATA, Index + 1, 0)))
	 Index++;

   Super->SetSelIndex(Index);

   char Letter[4];
   Letter[0] = 0;
   Letter[1] = 0;
   Letter[2] = 0;
   Letter[3] = 0;

   if(!::IsLetter(SendMessage(Super->HWindow, CB_GETITEMDATA, Index, 0)))
      Letter[1] = ::Letter(SendMessage(Super->HWindow, CB_GETITEMDATA, Index, 0));

   while(::IsLetter(SendMessage(Super->HWindow, CB_GETITEMDATA, Index, 0)) == 0)
     Index--;

   Letter[0] = ::Letter(SendMessage(Super->HWindow, CB_GETITEMDATA, Index, 0));
   e[8+2]->SetText(Letter);

   DynStr buffer;
   SendMessage(Super->HWindow, CB_GETLBTEXT, Index, (LPARAM)(char*)buffer);
   SetDlgItemText(HWindow, 2000, buffer);

   e[11+2]->SetText("");
   e[12+2]->SetText("");
   e[13+2]->SetText("");
   e[14+2]->SetText("");
   e[17+2]->SetText("");

   m_TarifGlass->SetText("");

   Calculate();
}

void MandatoryDlg::ChangePay1(RTMessage msg)
{
	ASS();
   if(UpdateFormData) return;

   if(!ModifyMandatory)
      return;

   if(GetFocus() != e[22+2]->HWindow)
      return;

   if(msg.LP.Hi != EN_CHANGE)
      return;


   static int isinside = 0;
   if(isinside) return;
   isinside = 1;

   char buffer[32], curr[5];
   e[22+2]->GetText(buffer, sizeof buffer); //Pay1
   Pay1Curr->GetText(curr, sizeof curr);
   e[23+2]->SetText(GetSummaText(atof(buffer), Pay1Curr->GetSelIndex() == 0 ? Rubl : curr)); //Pay1

   End:
   isinside = 0;
}


/////////////////////////////////////////

void MandatoryDlg::IsAvaria(RTMessage msg)
{
	//Print Data for now
	ShowWindow(GetDlgItem(HWindow, 10000), SW_HIDE);
	ShowWindow(GetDlgItem(HWindow, 474), SW_HIDE);
	ShowWindow(GetDlgItem(HWindow, 460), SW_HIDE);
	ShowWindow(GetDlgItem(HWindow, 461), SW_HIDE);
	ShowWindow(GetDlgItem(HWindow, 462), SW_HIDE);

	ShowWindow(GetDlgItem(HWindow, 10020), SW_SHOW);
	ShowWindow(GetDlgItem(HWindow, 408), SW_SHOW);
	ShowWindow(GetDlgItem(HWindow, 4081), SW_SHOW);
	ShowWindow(GetDlgItem(HWindow, 56), SW_SHOW);
	ShowWindow(GetDlgItem(HWindow, 78), SW_SHOW);
	ShowWindow(GetDlgItem(HWindow, 95), SW_SHOW);

	if(IsOpenPart2 == 1)
    {
		OpenClose(); //Close
	}
    else
	if(IsOpenPart2 == 0) {
		OpenClose(); //Open
		IsOpenPart2 = 1;
        return;
	}
	else
		IsOpenPart2 = 1;

	/* for avaria code - depricated
	ASS();
	ShowWindow(GetDlgItem(HWindow, 409), SW_HIDE);
	ShowWindow(GetDlgItem(HWindow, 497), SW_HIDE);

    //Установить состояние кнопок (на открытие)
    SendDlgItemMessage(HWindow, 450, WM_SETTEXT, 0, (LPARAM)"<<<");
    SendDlgItemMessage(HWindow, 451, WM_SETTEXT, 0, (LPARAM)">>>");

    //Состояние контролов
    SendDlgItemMessage(HWindow, 10000, WM_SETTEXT, 0, (LPARAM)"Страховые случаи");
    ShowWindow(ListBAvaria->HWindow, SW_SHOW);
    ShowWindow(ListBOwners->HWindow, SW_HIDE);

    // -1 - undestand mode
	if(IsOpenPart2 == 1) { //Avaria mode
	SendDlgItemMessage(HWindow, 450, WM_SETTEXT, 0, (LPARAM)">>>");
	OpenClose(); //Close
	//IsOpenPart2 = 0;
	return;
    }
    if(IsOpenPart2 == 0) {
	OpenClose(); //Open
	IsOpenPart2 = 1;
        return;
    }
	if(IsOpenPart2 == 2) {
	IsOpenPart2 = 1;
    }
    if(IsOpenPart2 == -1)
	IsOpenPart2 = 1;*/
}

void MandatoryDlg::IsPhoto(RTMessage)
{
	ShowWindow(GetDlgItem(HWindow, 10000), SW_SHOW);
	ShowWindow(GetDlgItem(HWindow, 474), SW_SHOW);
	ShowWindow(GetDlgItem(HWindow, 460), SW_SHOW);
	ShowWindow(GetDlgItem(HWindow, 461), SW_SHOW);
	ShowWindow(GetDlgItem(HWindow, 462), SW_SHOW);

	ShowWindow(GetDlgItem(HWindow, 10020), SW_HIDE);
	ShowWindow(GetDlgItem(HWindow, 408), SW_HIDE);
	ShowWindow(GetDlgItem(HWindow, 4081), SW_HIDE);
	ShowWindow(GetDlgItem(HWindow, 56), SW_HIDE);
	ShowWindow(GetDlgItem(HWindow, 78), SW_HIDE);
	ShowWindow(GetDlgItem(HWindow, 95), SW_HIDE);

	if(IsOpenPart2 == 3)
    {
		OpenClose(); //Close
	}
    else
	if(IsOpenPart2 == 0) {
		OpenClose(); //Open
		IsOpenPart2 = 3;
        return;
	}
	else
		IsOpenPart2 = 3;
}

/*void MandatoryDlg::IsOwners(RTMessage)
{
	ASS();
	ShowWindow(GetDlgItem(HWindow, 409), SW_SHOW);
    ShowWindow(GetDlgItem(HWindow, 497), SW_SHOW);

    //Установить состояние кнопок (на открытие)
    SendDlgItemMessage(HWindow, 450, WM_SETTEXT, 0, (LPARAM)">>>");
    SendDlgItemMessage(HWindow, 451, WM_SETTEXT, 0, (LPARAM)"<<<");

    //Состояние контролов
    SendDlgItemMessage(HWindow, 10000, WM_SETTEXT, 0, (LPARAM)"Владельцы");
    ShowWindow(ListBAvaria->HWindow, SW_HIDE);
    ShowWindow(ListBOwners->HWindow, SW_SHOW);

    // -1 - undestand mode
    if(IsOpenPart2 == 2) { //Owners mode
	SendDlgItemMessage(HWindow, 451, WM_SETTEXT, 0, (LPARAM)">>>");
	OpenClose(); //Close
	//IsOpenPart2 = 0;
	return;
    }
    if(IsOpenPart2 == 0) {
	OpenClose(); //Open
	IsOpenPart2 = 2;
        return;
    }
    if(IsOpenPart2 == 1) {
	IsOpenPart2 = 2;
    }
    if(IsOpenPart2 == -1)
	IsOpenPart2 = 2;
}
*/
void MandatoryDlg::OpenClose()
{
	ASS();
	 RECT r;
     GetWindowRect(HWindow, &r);
	 int WndWidth = r.right - r.left;
     if(StartWidth == 0)
       StartWidth = WndWidth;

	 int delta = 1;
     RECT rrr;
	 GetWindowRect(GetDlgItem(HWindow, 471), &rrr);
	 int DeltaSize = rrr.right - rrr.left + 10;
//	 int DeltaSize = 185;
       //	     delta=DeltaSize;
     if(StartWidth >= WndWidth) { // to width
       //SendDlgItemMessage(HWindow, 450, WM_SETTEXT, 0, (LPARAM)"<<<");
       IsOpenPart2 = -1;
     }
     else { // to normal
       delta = -delta;
       DeltaSize = WndWidth - StartWidth + 1;
       //SendDlgItemMessage(HWindow, 450, WM_SETTEXT, 0, (LPARAM)">>>");
       IsOpenPart2 = 0;
     }

     CanAlign = 0;
     for(int i = 0, Left = 0; i < DeltaSize; i++, Left += delta) {
	if(r.left + WndWidth + Left > GetSystemMetrics(SM_CXSCREEN))
	   r.left = GetSystemMetrics(SM_CXSCREEN) - (WndWidth + Left);
	SetWindowPos(HWindow, 0, r.left, r.top, WndWidth + Left, r.bottom - r.top, 0);
	if(delta > 0) UpdateWindow(HWindow);
	long time = GetCurrentTime();
	//if(delta < 0) while(GetCurrentTime() - time > 100);
     }
     CanAlign = 1;
}

int MandatoryDlg::BeforeDelete()
{
	ASS();
	for(int i = 0; i < avariaCount; i++)
       if(!DeleteAvaria2(avarias2[i])) {
          MessageBox(0, "Ошибка удаления аварий.", "Сообщаю", MB_OK | MB_ICONSTOP | MB_TASKMODAL);
          return 0;
       }
//       else
  //	  AddLog(GetSeria(), GetNumber(), avarias2[i].N, "A", "d");
    /*
    for(i = 0; i < ownersCount; i++)
	if(!DeleteOwner(autoowners[i])) {
	    MessageBox(0, "Ошибка удаления владельцев.", "Сообщаю", MB_OK | MB_ICONSTOP | MB_TASKMODAL);
	    return 0;
	}
        else
	    AddLog(GetSeria(), GetNumber(), autoowners[i].Name, "O", "d");
    */
    char buff1[16];
    double Nmb;

    (*base)(buff1, sizeof buff1, 0);
    (*base)(Nmb, 1);

    //DeleteFromAux(buff1, Nmb);

//    _SavePolisToFile("D", buff1, Nmb);

    return 1;
}
      /*
void MandatoryDlg::DeleteFromAux(char* Seria, double Number)
{
//	ASS();
//	mandaux(0, Seria);
//    mandaux(1, Number);
//    if(PXSrchKey(mandaux.getTblHandle(), mandaux.getRecHandle(), 2, SEARCHFIRST) == PXSUCCESS)
//       mandaux.Delete();
}
	*/
/*AddIntoListBox2(TListBox* CB, OwnersData data, int Index)
{
	ASS();
	if(Index != -1)
       CB->DeleteString(Index);
    else
       Index = CB->GetCount();

    char* str = new char[512];
    sprintf(str, "%s\n%s\nНаходится %s\nK=%0.1f\n%s", data.Name, data.RegDate, data.Placement, (double)data.K, data.Fizich ? "Физическое" : "Юридическое");
    CB->InsertString(str, Index);
    CB->SetSelIndex(Index);
    delete str;
}
  */
AddIntoListBox(TListBox* CB, Avaria2Data data, int Index)
{
	ASS();
	if(Index != -1)
       CB->DeleteString(Index);
    else
       Index = CB->GetCount();

    char* str = new char[512];
	wsprintf(str, GetRCStr(508), data.N, data.WorkNmb, data.BadMan, data.WrDate, data.AVDate);
    CB->InsertString(str, Index);
    CB->SetSelIndex(Index);
    delete str;
}

char* MandatoryDlg::GetSeria()
{
	ASS();
   e[0]->GetText(buffer.Seria, sizeof buffer.Seria);
   return buffer.Seria;
}

double MandatoryDlg::GetNumber()
{
	ASS();
   char nmb[10];
   e[1]->GetText(nmb, sizeof nmb);
   return atof(nmb);
}

extern char* AVPayTableName;
extern char* AVPayFields[];

int MandatoryDlg::DeleteAvaria2(Avaria2Data data)
{
	ASS();
   char b[10];
   char nmb[10];

   avaria2.Refresh();
   if(e[0]->GetText(b, sizeof b) && e[1]->GetText(nmb, sizeof nmb))
    {
       double Number = atof(nmb);
       (avaria2)(0, b);
       (avaria2)(1, Number);
       (avaria2)(2, atof(data.N));
	   if(PXSrchKey(avaria2.getTblHandle(), avaria2.getRecHandle(), 3, SEARCHFIRST) == PXSUCCESS)
	   if(!avaria2.Delete())
		  return 0;

	   //Удаление выплат
	   TOpenBase base(AVPayTableName, AVPayFields, 10);
	   (base)(0, b);
	   (base)(1, Number);
	   (base)(2, atof(data.N));
	   while(PXSrchKey(base.getTblHandle(), base.getRecHandle(), 3, SEARCHFIRST) == PXSUCCESS)
	   base.Delete();

       PXRecBufEmpty(avaria2.getRecHandle());
       (avaria2)(0, b);
       (avaria2)(1, Number);
       (avaria2)(2, atof(data.N));
       return 1;
    }
    return 0;
}

int MandatoryDlg::SaveAvaria2(Avaria2Data data)
{
	ASS();
	if(DeleteAvaria2(data)) {
	   (avaria2)(3, GD(data.AVDate));
       (avaria2)(4, data.AVPlace);
       (avaria2)(5, data.WorkNmb);
	   (avaria2)(6, GD(data.WrDate));
       (avaria2)(7, (double)data.Decision);
       (avaria2)(8, (double)data.AVDocument);
       (avaria2)(9, data.SN);
       (avaria2)(10, data.FIO);
       (avaria2)(11, (double)data.IsBMFizich);
       (avaria2)(12, data.FIOAddress);
       (avaria2)(13, data.BadMan);
       (avaria2)(14, atof(data.Staj));
       (avaria2)(15, (double)data.VTR);

       (avaria2)(37, GD(data.UpdateDate));
       (avaria2)(38, data.CityCode);

       (avaria2)(39, data.BMCityCode);
       (avaria2)(40, data.BMDiscount);
       (avaria2)(41, (double)data.IsBMResident);
       (avaria2)(42, (double)data.BMOsnovan); //Число

       (avaria2)(43, data.Marka);
       (avaria2)(44, data.BaseType);
       (avaria2)(45, atof(data.AutoCharact));
       (avaria2)(46, data.AutoNmb);
       (avaria2)(47, data.BodyNo);

       (avaria2)(55, data.LossCode);

       if(*data.DtFreeRz)
		 (avaria2)(56, GD(data.DtFreeRz));
       if(*data.DtClLoss)
		 (avaria2)(57, GD(data.DtClLoss));

	   int nSave = IsValidAvariaRecord(avaria2);
  	   if(nSave) {
		   MessageBox(HWindow, GetCheckErrorText(nSave), "Авария", MB_ICONSTOP);
		   //return 0;
	   }

	   int Result = (avaria2.AddRecord() == PXSUCCESS) ? (nSave ? 1 : -1) : 0 ;

	   //Сохранение выплат
	   char ser[5];
	   char nmb[11];

	   e[0]->GetText(ser, sizeof ser);
	   e[1]->GetText(nmb, sizeof nmb);

	   double Number = atof(nmb);
	   TOpenBase base(AVPayTableName, AVPayFields, 14);
	   for(int i = 0; *m_AVPayData[i].Date; i++) {
		   (base)(0, ser);
		   (base)(1, Number);
		   (base)(2, atof(data.N));
		   (base)(3, GD(strcpy(nmb, m_AVPayData[i].Date)));
		   (base)(4, atof(m_AVPayData[i].Sum));
		   (base)(5, m_AVPayData[i].Curr);
		   (base)(6, m_AVPayData[i].Nal ? "1" : "0");
		   (base)(7, m_AVPayData[i].Doc);
		   (base)(8, m_AVPayData[i].Type1 ? "0" : "1");
		   (base)(9, short(m_AVPayData[i].Zatr));
		   (base)(10, m_AVPayData[i].Risk1 ? "0" : m_AVPayData[i].Risk2 ? "1" : m_AVPayData[i].Risk3 ? "2" : "3");
		   (base)(11, m_AVPayData[i].Rcvr1 ? "0" : "1");
		   (base)(12, m_AVPayData[i].SubType1 ? "1" : "2");
		   (base)(13, m_AVPayData[i].Name);
		   if(base.AddRecord() != PXSUCCESS) {
				return 0;
           }
	   }

	   return Result;
    }
    return 0;
}

////////
/*
int MandatoryDlg::DeleteOwner(OwnersData data)
{
	ASS();
   char b[10];
   char nmb[10];

   owners.Refresh();
   if(e[0]->GetText(b, sizeof b) && e[1]->GetText(nmb, sizeof nmb))
    {
       double Number = atof(nmb);
       (owners)(0, b);
       (owners)(1, Number);
       (owners)(2, data.Name);
       if(PXSrchKey(owners.getTblHandle(), owners.getRecHandle(), 3, SEARCHFIRST) == PXSUCCESS)
	   if(!owners.Delete())
	      return 0;
       PXRecBufEmpty(owners.getRecHandle());
       (owners)(0, b);
       (owners)(1, Number);
       (owners)(2, data.Name);
       return 1;
    }
    return 0;
 }

int MandatoryDlg::SaveOwner(OwnersData data)
{
	ASS();
	if(DeleteOwner(data)) {
	DATE CurrDate;
        struct date d;
        getdate(&d);
	PXDateEncode(d.da_mon, d.da_day, d.da_year, &CurrDate);
       (owners)(3, CurrDate); //UpdateDate

       (owners)(4, data.Placement);
       (owners)(5, data.K);
       (owners)(6, (double)data.Fizich);
       (owners)(7, (double)data.Pravo);
       (owners)(8, data.Address);
       (owners)(9, data.PravoTxt);
       double WhenPrava = atoi(data.WhenPrava);
       if(WhenPrava > 1930) {
	 (owners)(10, WhenPrava);
       }
       (owners)(11, data.CityCode);
       //if(data.OwnerCode > 0)
	  //(owners)(12, (double)data.OwnerCode);

	   (owners)(13, GD(data.RegDate));
       (owners)(14, atof(data.Summa));
       (owners)(15, data.PayDocNumber);
       if(*data.PayDate)
	   (owners)(16, GD(data.PayDate));

       return owners.AddRecord() == PXSUCCESS;
    }
    return 0;
}
*/

int Executor(char* Module, char* What, int AddLockalPath = 1);

void MandatoryDlg::AddAvaria(RTMessage)
{
	ASS();
	if(!SetDataToBase())
		return;

    int SaveIsEnable = IsWindowEnabled(GetDlgItem(HWindow, 406));
    if(IsOpenPart2 == 1 && SaveIsEnable) { //Save button
	MessageBox(HWindow, GetRCStr(509), "Сообщаю", MB_OK | MB_SYSTEMMODAL);
    	return;
    }
    EnableWindow(HWindow, 0);

	if(IsOpenPart2 == 3) {
		DynStr dbpath;
		GetProfileString("Paradox Engine", "NetNamePath", "", dbpath, dbpath._sizeof());
		char imageImportWidth[10] = "";
		GetPrivateProfileString(MANDATORY_SECT, "ImageWidth", "1000", imageImportWidth, sizeof imageImportWidth, ININame);
		DynStr buff(1000);
		sprintf(buff, "ADD %d %s %s %s\\IMG %s", HWindow, buffer.Seria, buffer.Number, (const char*)dbpath, imageImportWidth);
		Executor("Imager.exe", buff, 1);
    }
	if(IsOpenPart2 == 1) {
	  int maxNumberAvaria = 1;

          char buffer[24];
          ((TEdit*)ChildWithId(102))->GetText(buffer, sizeof buffer);
	  DATE MinDate = GD(buffer);
          ((TEdit*)ChildWithId(104))->GetText(buffer, sizeof buffer);
	  DATE MaxDate = GD(buffer);

	  if(!MinDate || !MaxDate) {
            EnableWindow(HWindow, 1);
	    return;
	  }

	  if(1) {
	     Avaria2Data data;
		 memset(&data, 0, sizeof data);

		 GetPrivateProfileString(MANDATORY_SECT, "Seria", "АБ", data.SN, sizeof data.SN, ::ININame);
		 strcat(data.SN, "/");
	     data.IsBMResident = 1;
		 data.IsBMFizich = 1;
		 for(int i = 0; i < avariaCount; i++)
		if(atoi(avarias2[i].N) >= maxNumberAvaria)
		   maxNumberAvaria = atoi(avarias2[i].N) + 1;
	     sprintf(data.N, "%d", maxNumberAvaria);
		 MandatoryDlgAvaria2* dlg = new MandatoryDlgAvaria2(this, &data, this->buffer.Seria, atof(this->buffer.Number), m_AVPayData, ININame);
	     char Name[64];
	     ((TEdit*)ChildWithId(100))->GetText(Name, sizeof Name);
	     dlg->m_FIOList.add(*new String(Name));
/*	     for(i = 0; i < ownersCount; i++)
		 dlg->m_FIOList.add(*new String(autoowners[i].Name));
  */
	     dlg->MinDate = MinDate;
	     dlg->MaxDate = MaxDate;
             RECT r;
             GetWindowRect(GetDlgItem(HWindow, 126), &r);
             dlg->ru.y = r.top;
             GetWindowRect(GetDlgItem(HWindow, 470), &r);
             dlg->ru.x = r.right;
	     if(GetModule()->ExecDialog(dlg) == IDOK) {
		 if(SaveAvaria2(data)) {
//		    AddLog(GetSeria(), GetNumber(), data.N, "A", "N");
		    memmove(&avarias2[avariaCount], &data, sizeof data);
			avariaCount++;
			AddIntoListBox(ListBAvaria, data);
			SetFocus(ListBAvaria->HWindow);
		 }
		 else
			MessageBox(HWindow, "Ошибка сохранения аварии", "Stop", MB_OK | MB_TASKMODAL | MB_ICONSTOP);
			MessageBox(HWindow, base->GetLastErr().s, "Stop", MB_OK | MB_TASKMODAL | MB_ICONSTOP);
		 }
	  }
    }
/*	if(IsOpenPart2 == 2) {
	  OwnersData data;
	  memset(&data, 0, sizeof data);
	  strcpy(data.WhenPrava, "19");
	  GetDlgItemText(HWindow, 52, data.RegDate, sizeof data.RegDate);

	  if(ownersCount == 0)
	      e[6]->GetText(data.Address, sizeof data.Address);
	  else
	      strcpy(data.Address, autoowners[ownersCount - 1].Address);

	  data.Fizich = 1;

	  m_Placement->GetText(data.Placement, sizeof data.Placement);

	  char str[15];
	  GetDlgItemText(HWindow, 52, str, sizeof str);
	  DATE MIND = GD(str);
	  if(!MIND) {
	  	EnableWindow(HWindow, 1);
		return;
          }
	  GetDlgItemText(HWindow, 104, str, sizeof str);
	  DATE MAXD = GD(str);
	  if(!MAXD) {
		EnableWindow(HWindow, 1);
		return;
          }

	  MandatoryDlgOwners* dlg = new MandatoryDlgOwners(this, &data, MIND, MAXD);
	  GetDlgItemText(HWindow, 993, str, sizeof str);
	  dlg->PlaceK_Form = atof(str);
	  if(GetModule()->ExecDialog(dlg) == IDOK) {
	      if(!SaveIsEnable) {
		 if(!SaveOwner(data)) {
		    MessageBox(HWindow, "Ошибка сохранения владельца", "Stop", MB_OK | MB_TASKMODAL | MB_ICONSTOP);
                    return;
		 }
		 AddLog(GetSeria(), GetNumber(), data.Name, "O", "N");
	      }
	      else {
		 AddLog(GetSeria(), GetNumber(), data.Name, "O", "R");
	      }
 	      memmove(&autoowners[ownersCount], &data, sizeof data);
	      ownersCount++;
  	      AddIntoListBox2(ListBOwners, data);
	      SetFocus(ListBOwners->HWindow);
	      autoowners[ownersCount].IsSaved = !SaveIsEnable;
	      if(SaveIsEnable) {
	          Calculate();
		  SavePolis();
	      }
	  }
    }*/

   if(!CheckAvDups()) {
		  MessageBox(HWindow, GetRCStr(633), "Ошибка", MB_OK | MB_ICONSTOP);
   }
	EnableWindow(HWindow, 1);
    SetActiveWindow(HWindow);
    SetFocus(IsOpenPart2 == 1 ? ListBAvaria->HWindow : ListBOwners->HWindow);
}

void  MandatoryDlg::MeasureItem(RTMessage msg)
{
	ASS();
	MEASUREITEMSTRUCT* data = (MEASUREITEMSTRUCT FAR*)msg.LParam;
    if(data->itemID != -1) {
       DynStr Text;
	   if(data->CtlID == 470)
  	      ListBAvaria->GetString(Text, data->itemID);
       if(data->CtlID == 471)
	      ListBOwners->GetString(Text, data->itemID);
	   if(data->CtlID == 474)
		  ListBPhoto->GetString(Text, data->itemID);
	   data->CtlType = ODT_LISTBOX;
       RECT r;
       HDC dc = GetDC(0);
       HGDIOBJ oldFont = SelectObject(dc, (HGDIOBJ)SendMessage(HWindow, WM_GETFONT, 0, 0));
       DrawText(dc, Text, strlen(Text), &r, DT_LEFT | DT_CALCRECT);
       ReleaseDC(0, dc);
       data->itemHeight = r.bottom - r.top;
       SelectObject(dc, oldFont);
    }
}

void  MandatoryDlg::WMDrawItem(RTMessage msg)
{
	ASS();
	DRAWITEMSTRUCT* lpdis = (DRAWITEMSTRUCT FAR*)msg.LParam;
    if(lpdis->itemID == -1) return;

    DynStr Text;

	if(lpdis->hwndItem == ListBPhoto->HWindow)
	ListBPhoto->GetString(Text, lpdis->itemID);
	else
	if(lpdis->hwndItem == ListBAvaria->HWindow)
	ListBAvaria->GetString(Text, lpdis->itemID);
    else
	ListBOwners->GetString(Text, lpdis->itemID);

    COLORREF BkColor = 0x00FFFFFF;
    int Focused = 0;

    if((lpdis->itemState & ODS_SELECTED) == ODS_SELECTED) {
	    BkColor = RGB(0, 0, 128);
    }
    if((lpdis->itemState & ODS_FOCUS) == ODS_FOCUS) {
	    BkColor = RGB(0, 0, 128);
	    Focused = 1;
    }

     HBRUSH Brush = CreateSolidBrush(BkColor);
     HGDIOBJ oldBrush = SelectObject(lpdis->hDC, Brush);
     Rectangle(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top, lpdis->rcItem.right, lpdis->rcItem.bottom);
     SetBkMode(lpdis->hDC, TRANSPARENT);
     SetTextColor(lpdis->hDC, RGB(255, 0, 0));
     DrawText(lpdis->hDC, Text, strlen(Text), &lpdis->rcItem, DT_LEFT/* | DT_WORDBREAK*/);
     if(Focused) DrawFocusRect(lpdis->hDC, &lpdis->rcItem);
     SelectObject(lpdis->hDC, oldBrush);
     msg.Result = TRUE;
}

void  MandatoryDlg::EditAvaria(RTMessage)
{
	ASS();
	int Index = ListBAvaria->GetSelIndex();
    if(IsOpenPart2 == 2) Index = ListBOwners->GetSelIndex();
	if(IsOpenPart2 == 3) Index = 0;

    if(Index == -1)
       return;

    int IsSaveEnable = IsWindowEnabled(GetDlgItem(HWindow, 406));
    if(IsOpenPart2 == 1 && IsSaveEnable) { //Save button
	MessageBox(HWindow, GetRCStr(509), "Сообщаю", MB_OK | MB_SYSTEMMODAL);
    	return;
    }
    EnableWindow(HWindow, 0);
	if(IsOpenPart2 == 3) {
		DynStr dbpath;
		GetProfileString("Paradox Engine", "NetNamePath", "", dbpath, dbpath._sizeof());
		DynStr buff;
		sprintf(buff, "VIEW %s %s %s\\IMG", buffer.Seria, buffer.Number, dbpath);
		Executor("Imager.exe", buff, 1);
		//MessageBox(0, buff, 0, 0);
    }
	if(IsOpenPart2 == 1) {
	if(1/*Version == 2*/) {
	    Avaria2Data data;
	    data = avarias2[Index];

		MandatoryDlgAvaria2* dlg = new MandatoryDlgAvaria2(this, &data, buffer.Seria, atof(buffer.Number), m_AVPayData, ININame);

	    char buffer[24];
	    ((TEdit*)ChildWithId(102))->GetText(buffer, sizeof buffer);
		dlg->MinDate = GD(buffer);
	    ((TEdit*)ChildWithId(104))->GetText(buffer, sizeof buffer);
		dlg->MaxDate = GD(buffer);

	    RECT r;
            GetWindowRect(GetDlgItem(HWindow, 126), &r);
            dlg->ru.y = r.top;
            GetWindowRect(GetDlgItem(HWindow, 470), &r);
	    dlg->ru.x = r.right;

	     char Name[64];
	     ((TEdit*)ChildWithId(100))->GetText(Name, sizeof Name);
             TrimStr(Name);
	     dlg->m_FIOList.add(*new String(Name));
//	     for(int i = 0; i < ownersCount; i++)
//		 dlg->m_FIOList.add(*new String(TrimStr(autoowners[i].Name)));

	    if(GetModule()->ExecDialog(dlg) == IDOK) {
		if(SaveAvaria2(data)) {
		   //memmove(&avarias2[Index], &data, sizeof data);
		   avarias2[Index] = data;
		   AddIntoListBox(ListBAvaria, data, Index);
		   SetFocus(ListBAvaria->HWindow);
		   ListBAvaria->SetSelIndex(Index);
//		   AddLog(GetSeria(), GetNumber(), data.N, "A", "E");
		}
		else
		     MessageBox(HWindow, "Ошибка сохранения аварии", "Stop", MB_OK | MB_TASKMODAL | MB_ICONSTOP);
	    }
        }
    }
    /*
    if(IsOpenPart2 == 2) {
	  OwnersData data;
	  data = autoowners[Index];
	  data.IsSaved = 1;

	  char str[15];
	  GetDlgItemText(HWindow, 52, str, sizeof str);
	  DATE MIND = GD(str);
	  if(!MIND) {
	  	EnableWindow(HWindow, 1);
		return;
          }
	  GetDlgItemText(HWindow, 104, str, sizeof str);
	  DATE MAXD = GD(str);
	  if(!MAXD) {
		EnableWindow(HWindow, 1);
		return;
          }
	  MandatoryDlgOwners* dlg = new MandatoryDlgOwners(this, &data, MIND, MAXD);
	  GetDlgItemText(HWindow, 993, str, sizeof str);
	  dlg->PlaceK_Form = atof(str);

	  if(GetModule()->ExecDialog(dlg) == IDOK) {
     	         if(!DeleteOwner(autoowners[Index])) {
		     MessageBox(HWindow, "Ошибка сохранения владельца", "Stop", MB_OK | MB_TASKMODAL | MB_ICONSTOP);
		     return;
		 }

		 if(!IsSaveEnable) {
		    if(!SaveOwner(data)) {
		      MessageBox(HWindow, "Ошибка сохранения владельца", "Stop", MB_OK | MB_TASKMODAL | MB_ICONSTOP);
		      return;
		    }
		    AddLog(GetSeria(), GetNumber(), data.Name, "O", "E");
		 }
		 else {
		    AddLog(GetSeria(), GetNumber(), data.Name, "O", "r");
		 }
		 autoowners[Index] = data;
		 AddIntoListBox2(ListBOwners, data, Index);
		 ListBOwners->SetSelIndex(Index);
		 SetFocus(ListBOwners->HWindow);
		 if(IsSaveEnable) {
		     Calculate();//PostMessage(HWindow, 1, 2, 0);
			 SavePolis();
		 }
		 autoowners[Index].IsSaved = !IsSaveEnable;
	  }
    } */
   if(!CheckAvDups()) {
		  MessageBox(HWindow, GetRCStr(633), "Ошибка", MB_OK | MB_ICONSTOP);
   }
	EnableWindow(HWindow, 1);
    SetActiveWindow(HWindow);
    SetFocus(IsOpenPart2 == 1 ? ListBAvaria->HWindow : ListBOwners->HWindow);
}

void  MandatoryDlg::StopData(RTMessage msg)
{
	ASS();
	if(m_State == 0)
     if(msg.LP.Hi == EN_CHANGE) {
        EnableWindow(GetDlgItem(HWindow, 58), FALSE);
        EnableWindow(GetDlgItem(HWindow,406), TRUE);
     }
}

void MandatoryDlg::DeleteAvaria(RTMessage)
{
	ASS();
	int Index = ListBAvaria->GetSelIndex();
	if(IsOpenPart2 == 2) Index = ListBOwners->GetSelIndex();
	if(IsOpenPart2 == 3) Index = ListBPhoto->GetSelIndex();

	if(Index == -1)
       return;

    DynStr _Msg;
    if(IsOpenPart2 == 1) {
	wsprintf(_Msg, GetRCStr(614), /*Version == 1 ? avarias[Index].RegDate :*/ avarias2[Index].WrDate);
	//if(Version == 2) {
	   if(avarias2[Index].OwnerCode > 0 || avarias2[Index].PaymentCode > 0 ||
	      avarias2[Index].CarCode > 0 || avarias2[Index].BaseCarCode > 0) {
	   	MessageBox(HWindow, GetRCStr(526), "Сообщаю",  MB_ICONINFORMATION | MB_SYSTEMMODAL);
                return;
	   }
	//}
	}
	if(IsOpenPart2 == 3) {
		strcpy(_Msg, "Удалить?");
    }
/*    if(IsOpenPart2 == 2) {
	if(!GetPrivateProfileInt(MANDATORY_SECT, "CanDelOwner", 0, ININame)) {
	    MessageBox(HWindow, "Низя", "Понял, да?", MB_ICONINFORMATION | MB_SYSTEMMODAL);
	    return;
        }
	if(autoowners[Index].OwnerCode > 0) {
	    MessageBox(HWindow, GetRCStr(525), "Сообщаю",  MB_ICONINFORMATION | MB_SYSTEMMODAL);
            return;
	}
	wsprintf(_Msg, GetRCStr(615), autoowners[Index].Name);
    }*/
	if(MessageBox(HWindow, _Msg, "Ответь", MB_YESNO | MB_ICONQUESTION | MB_SYSTEMMODAL) != IDYES)
       return;

	if(IsOpenPart2 == 3) {
		DynStr Text, fn;
		ListBPhoto->GetString(Text, Index);
		strtok(Text, "\n");
		strcpy(fn, strtok(NULL, "\n"));
		strcat(fn, "\\");
		strcat(fn, strtok(NULL, "\n"));
		if(unlink(fn))
		{
			MessageBox(HWindow, "Ошибка!", "!", MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL);
			LoadImagesList(buffer.Seria, atol(buffer.Number));
		}
		else
        {
			ListBPhoto->DeleteString(Index);
        }
	}
	if(IsOpenPart2 == 1) {
/*	if(Version == 1 && DeleteAvaria(avarias[Index])) {
	    avariaCount--;
	    ListBAvaria->DeleteString(ListBAvaria->GetSelIndex());
	    memmove(&avarias[Index], &avarias[Index + 1], sizeof(AvariaData) * (avariaCount + 1));
	}*/
	if(/*Version == 2 && */DeleteAvaria2(avarias2[Index])) {
//	    AddLog(GetSeria(), GetNumber(), avarias2[Index].N, "A", "D");
	    avariaCount--;
	    ListBAvaria->DeleteString(ListBAvaria->GetSelIndex());
		memmove(&avarias2[Index], &avarias2[Index + 1], sizeof(Avaria2Data) * (avariaCount + 1));
	}
    }
    /*if(IsOpenPart2 == 2) {
	if(DeleteOwner(autoowners[Index])) {
	    AddLog(GetSeria(), GetNumber(), autoowners[Index].Name, "O", "D");
	    ownersCount--;
	    ListBOwners->DeleteString(ListBOwners->GetSelIndex());
	    memmove(&autoowners[Index], &autoowners[Index + 1], sizeof(OwnersData) * (ownersCount + 1));
	    Calculate();//Message(HWindow, 1, 2, 0);
	}
    }*/
}

void MandatoryDlg::ChSer(RTMessage msg)
{
	ASS();
   if(!UpdateFormData) //Сообщение приходит, когда читаем из БД
	 if((msg.LP.Hi == CBN_SELCHANGE || msg.LP.Hi == EN_CHANGE) && ModifyMandatory == 0) {
        int IsSer;//, IsAv;
	ClearAvarias(&IsSer);
//	ClearOwners(&IsAv);
	ClearPhotoList();

	if(IsSer/* || IsAv*/)
	    MessageBox(HWindow, GetRCStr(524), "Сообщаю", MB_OK | MB_ICONINFORMATION);

	SetState(STATE_POLIS_NORMAL);
//	EnableAllControls();
	Change(msg);
//	EnableWindow(m_VersionCombo->HWindow, 1);
	ModifyMandatory = 1; //Новый полис
	if(ModifyMandatory) Calculate();//PostMessage(HWindow, 1, 0, 0);
     }
}

void MandatoryDlg::ChNmb(RTMessage msg)
{
	ASS();
   if(!UpdateFormData) //Сообщение приходит, когда читаем из БД
     if(msg.LP.Hi == EN_CHANGE && ModifyMandatory == 0) {
		int IsSer, IsAv;
		ClearAvarias(&IsSer);
//		ClearOwners(&IsAv);
		ClearPhotoList();
		if(IsSer/* || IsAv*/)
		    MessageBox(HWindow, GetRCStr(524), "Сообщаю", MB_OK | MB_ICONINFORMATION);

		SetState(STATE_POLIS_NORMAL);
//		EnableAllControls();
		Change(msg);
//	    EnableWindow(m_VersionCombo->HWindow, 1);
		ModifyMandatory = 1;
		InitDates();
		//e[6+2]->SetText("");
		InitTime(e[6+2]);
	    m_StopDate->SetText("");
	    m_RetBRB->SetText("");
		m_RetEUR->SetText("");
		if(ModifyMandatory) Calculate();//PostMessage(HWindow, 1, 0, 0);
     }
}

void MandatoryDlg::ClearAvarias(int* Flag)
{
	ASS();
   if(Flag)
	   *Flag = ListBAvaria->GetCount() > 0;
  // buffer.OwnerCode = 0;
//   buffer.BaseCarCode = 0;
//   buffer.CarCode = 0;
   avariaCount = 0;
   ListBAvaria->ClearList();
//   memset(&avarias, 0, sizeof avarias);
   memset(&avarias2, 0, sizeof avarias2);
}
/*
void MandatoryDlg::ClearOwners(int* Flag)
{
	ASS();
   if(Flag)
       *Flag = ListBOwners->GetCount() > 0;
//   ownersCount = 0;
   ListBOwners->ClearList();
   memset(&autoowners, 0, sizeof autoowners);
}
*/
void MandatoryDlg::IsPay2Check(RTMessage msg)
{
	ASS();
   if(m_State) {
//      MessageBeep(0);
      return;
   }

   EnableWindow(GetDlgItem(HWindow, 406), TRUE);

   int IsPay2OK = IsPay2->GetCheck() == BF_CHECKED;

   EnableWindow(GetDlgItem(HWindow, 58), IsFee2->GetCheck() == BF_CHECKED);

   EnableWindow(e[30+2]->HWindow, IsPay2OK);
   EnableWindow(e[31+2]->HWindow, IsPay2OK);
   EnableWindow(e[32+2]->HWindow, IsPay2OK);
   EnableWindow(e[33+2]->HWindow, IsPay2OK);
   EnableWindow(Pay2Curr->HWindow, IsPay2OK);

   if(!IsPay2OK) {
       e[30+2]->SetText("");
       e[31+2]->SetText("");
       e[32+2]->SetText("");
       e[33+2]->SetText("");
       Pay2Curr->SetSelIndex(-1);
   }
   else
   if(ModifyMandatory) {
	   Pay2Curr->SetSelIndex(0);
       SetFocus(e[30+2]->HWindow);

       //Set Nal BezNal
       //Pay1Nal
	   Nal_2->SetCheck(Nal->GetCheck());
       BezNal_2->SetCheck(BezNal->GetCheck());
   }
}

void MandatoryDlg::Nal_2Check(RTMessage msg)
{
	ASS();
   if(ModifyMandatory || IsWindowEnabled(GetDlgItem(HWindow, 406))) {
      Nal_2->SetCheck(BF_CHECKED);
      BezNal_2->SetCheck(BF_UNCHECKED);
	  //e[33+2]->SetText("");
	  ChangePay2Rate(msg);
	  m_1SU->SetSelIndex(_1SU_DEF);
	  PostMessage(m_1SU->HWindow, CB_SHOWDROPDOWN, 1, 0);
   }
}

void MandatoryDlg::BezNal_2Check(RTMessage msg)
{
	ASS();
   if(ModifyMandatory || IsWindowEnabled(GetDlgItem(HWindow, 406))) {
	   Nal_2->SetCheck(BF_UNCHECKED);
       BezNal_2->SetCheck(BF_CHECKED);
       ChangePay2Rate(msg);
	   m_1SU->SetSelIndex(0);
	   PostMessage(m_1SU->HWindow, CB_SHOWDROPDOWN, 1, 0);
   }
}

void MandatoryDlg::ChangePay2Rate(RTMessage)
{
	ASS();
   if(UpdateFormData) return;

   char Summa[32];
   char Rate[32];

   e[26+2]->GetText(Summa, sizeof Summa);
   e[31+2]->GetText(Rate, sizeof Rate);
   double AllSumma = atof(Summa) * atof(Rate);

   //HWND W2 = GetDlgItem(HWindow, 67);
   //int IsRounded2 = IsWindowVisible(W2);
   int IsRounded2 = 1;

   if(Nal_2->GetCheck() == BF_CHECKED) {//Если по безналу, то округлить до копеек
	  if(IsRounded2) ToRubles(AllSumma);
   }
   else {
	  if(IsRounded2)
	     AllSumma = floor(AllSumma + 0.5); //Надо бы применить ф-ию из реестра
	  else
	    	AllSumma = long(AllSumma * 10 + 0.00001) / 10.;	  //AllSumma = long(AllSumma + 0.5);

	  //AllSumma = long(AllSumma + 0.5);
   }	            

   DblToStr(Summa, AllSumma);
   e[32+2]->SetText(Summa);

   Pay2Curr->SetSelIndex(0);
}

void MandatoryDlg::ChangeResident(RTMessage msg)
{
	ASS();
   if(msg.LP.Hi == CBN_SELCHANGE)
	FillSuperCombo(-1, 1);
}

void MandatoryDlg::ChCurr1(RTMessage msg)
{
	if(msg.LP.Hi != CBN_SELCHANGE) return;
	ASS();
   if(UpdateFormData) return;
   char buffer[16];
   Pay1Curr->GetText(buffer, sizeof buffer);
   //if(!strcmp(buffer, "BRB") || !strcmp(buffer, "EUR")) {
	  if(ModifyMandatory)
		CalculateSumma();
		if(!strcmp(buffer, "BRB") || !strcmp(buffer, "EUR"))
		   strcpy(buffer, "Евро");
   //}
   //else {
   //    if(ModifyMandatory) {
   //	  	e[20+2]->SetText("");
   //	  	e[21+2]->SetText("");
//	  	e[22+2]->SetText("");
  //	  	e[23+2]->SetText("");
   //	   }
   //}
   strcat(buffer, "=");
   SendDlgItemMessage(HWindow, 10001, WM_SETTEXT, 0, (long)buffer);
}

void MandatoryDlg::ChCurr2(RTMessage)
{
	ASS();
   char buffer[16];
   Pay2Curr->GetText(buffer, sizeof buffer);
   if(!strcmp(buffer, "BRB"))
      strcpy(buffer, "Евро");
   strcat(buffer, "=");
   SendDlgItemMessage(HWindow, 10002, WM_SETTEXT, 0, (long)buffer);
}

void MandatoryDlg::ChangeInOut(RTMessage)
{
   if(!Komplex ) return;

	int active = m_TaxiInOut->GetSelIndex()>0;
	EnableWindow(m_TaxiPayCurr->HWindow, active);
	EnableWindow(m_TaxiVid->HWindow, active);
	EnableWindow(m_TaxiSumma->HWindow, active);;
	EnableWindow(m_TaxiPayDate->HWindow, active);
	EnableWindow(m_TaxiDoc->HWindow, active);
	EnableWindow(m_TaxiNal->HWindow, active);
	EnableWindow(m_Taxi1SU->HWindow, active);
	if(!active)
	{
		 m_TaxiPayCurr->SetSelIndex(-1);
		 m_TaxiVid->SetSelIndex(-1);
		 m_TaxiSumma->SetText(0);
		 m_TaxiPayDate->SetText(0);
		 m_TaxiDoc->SetText(0);
		 m_TaxiNal->SetCheck(0);
		 m_Taxi1SU->SetCheck(0);
	}
}

void MandatoryDlg::ChangePay2Date(RTMessage msg)
{
	ASS();
   if(UpdateFormData) return;
   //if(!ModifyMandatory)
   //	return;
   if(msg.LP.Hi != EN_CHANGE) return;

   SetCurrencyForPay(2);
   SetDlgItemText(HWindow, 70, "");
}

void MandatoryDlg::ChangePay1Date(RTMessage msg)
{
	ASS();
   if(UpdateFormData) return;

   int ToStartNal = GetPrivateProfileInt(MANDATORY_SECT, "PayDateToStartNal", 0, ::ININame);
   int ToStartNoNal = GetPrivateProfileInt(MANDATORY_SECT, "PayDateToStartNoNal", 0, ::ININame);

   if(ModifyMandatory)
     if(msg.LP.Hi == EN_CHANGE) {
	   SetDlgItemText(HWindow, 69, "");
	   if(((BezNal->GetCheck() == 1 || Karta->GetCheck() == 1) && ToStartNoNal != 0) || (Nal->GetCheck() == 1 && ToStartNal != 0)) {
		  int Delta = BezNal->GetCheck() == 1  || Karta->GetCheck() == 1 ? ToStartNoNal : ToStartNal;
		  if(Delta > 0) Delta--;

		  char buffer[32];

		  e[24+2]->GetText(buffer, sizeof buffer);
		  DATE paydate = GD(buffer, 1) + Delta;

		  int m, d, y;
		  PXDateDecode(paydate, &m, &d, &y); //Если безнал, то начало cтрахования в тот же день
		  sprintf(buffer, "%02u.%02u.%04u", d, m, y);
		  e[5+2]->SetText(buffer);

		  CalculatePeriod();

		  SetCurrency(); //Установка курса валюты самомтоятельно
	   }
     }
}

void MandatoryDlg::DispatchAMessage(WORD AMsg, RTMessage Msg, void (TWindowsObject::* _FAR func) (RTMessage))
{
	TDialog::DispatchAMessage(AMsg, Msg, func);
}

void MandatoryDlg::DefWndProc(RTMessage Msg)
{
	if(Msg.Message == WM_COMMAND && UpdateFormData) return;
    TDialog::DefWndProc(Msg);
}

void  MandatoryDlg::VzamenFunc(RTMessage)
{
	ASS();
	VzamenFuncX();
}

void  MandatoryDlg::VzamenFuncX()
{
	ASS();
	int IsCheck = m_Vzamen->GetSelIndex() != -1;
	if(!IsCheck)
	{
        e[3]->SetText("");
		e[4]->SetText("");
    }
    EnableWindow(e[3]->HWindow, IsCheck);
    EnableWindow(e[4]->HWindow, IsCheck);
}

int MandatoryDlg::AfterSave()
{
/*    for(int i = 0; i < ownersCount; i++)
       if(!autoowners[i].IsSaved) {
 	  if(!SaveOwner(autoowners[i])) {
	     MessageBox(HWindow, "Ошибка сохранения владельцев", "Ошибка", MB_ICONINFORMATION);
	     return 0;
          }
	  autoowners[i].IsSaved = TRUE;
	  AddLog(GetSeria(), GetNumber(), autoowners[i].Name, "O", "N");
       }
  */
}

void MandatoryDlg::Fizich(RTMessage msg)
{
	m_Fizich->SetCheck(1);
    m_Uridich->SetCheck(0);
	m_IP->SetCheck(0);
	if(ModifyMandatory) {
		Calculate();
		NalFunc(msg);
    }
}

void MandatoryDlg::Uridich(RTMessage msg)
{
//    m_Uridich->SetCheck(1);
//    m_Fizich->SetCheck(0);
    if(ModifyMandatory) {
        //Nal->SetCheck(0);
	//BezNal->SetCheck(1);
		Calculate();
		BezNalFunc(msg);
	}
}

void MandatoryDlg::IP(RTMessage msg)
{
//    m_Uridich->SetCheck(1);
//    m_Fizich->SetCheck(0);
    if(ModifyMandatory) {
        //Nal->SetCheck(0);
	//BezNal->SetCheck(1);
		Calculate();
		NalFunc(msg);
	}
}

void MandatoryDlg::ClearPhotoList()
{
	ListBPhoto->ClearList();
}                          

const char* RU = "ЦУКЕНГШЗХФЫВАПРОЛДЖЭЯЧСМИТБЮ";
const char* EN = "CUKENGSZXFIWAPROLDZEJQCMITBU";

void ReplaceRUtoEN(char* buff)
{
	for(int i = 0; buff[i]; i++)
	{
		for(int idx = 0; RU[idx]; idx++)
		{
			if(buff[i] == RU[idx])
			{
				buff[i] = EN[idx];
                break;
            }
		}
	}
}

void MandatoryDlg::LoadImagesList(const char* pSeria, long Number)
{
	ClearPhotoList();
	//Photos
		DynStr dbpath;
		GetProfileString("Paradox Engine", "NetNamePath", "", dbpath, dbpath._sizeof());
  strcat(dbpath, "\\IMG\\");
  char Seria[10];
  strcpy(Seria, pSeria);
  ReplaceRUtoEN(Seria);
  strcat(dbpath, Seria);
  strcat(dbpath, "\\");
  ltoa(Number, dbpath + strlen(dbpath), 33);
  DynStr filemask;
  strcat(dbpath, "\\");
  strcpy(filemask, dbpath);
  strcat(filemask, "*.*");
//MessageBox(0, dbpath, 0, 0);
   struct ffblk ffblk;
   int done;
   done = findfirst(filemask,&ffblk,0);
   int i = 1;
   while (!done)
   {
	  DynStr str;
	  sprintf(str, "%d.\n", i++);
	  strcat(str, dbpath);
	  strcat(str, "\n");
	  strcat(str, ffblk.ff_name);
	  ListBPhoto->AddString(str);
	  done = findnext(&ffblk);
   }
}

void MandatoryDlg::RefreshPhotos(RTMessage msg)
{
	char Seria[3];
	memcpy(Seria, &msg.WParam, 2);
	Seria[2] = 0;
	int result = 0;
	if(!strcmp(Seria, buffer.Seria) && atol(buffer.Number) == msg.LParam)
	{
		LoadImagesList(Seria, msg.LParam);
		result = 1;
	}
	if(InSendMessage())
	{
		ReplyMessage(result);
    }
}
