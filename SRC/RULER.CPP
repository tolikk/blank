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
#include <static.h>
#include <dialog.h>
#include <string.h>
#include <edit.h>
#include <stdio.h>
#pragma hdrfile

#include <process.h>
#include <io.h>
#include "printer.h"
#include "scroller.h"
#include "ids.h"
#include "array.h"
#include "filedial.h"
#include "k.h"
#include "o.h"
#include "rulerout.h"
#include "ruler.h"
#include "grncard.h"
#include "bmpsize.h"
#include "page.h"
#include "date.h"
#include "warta.h"
//#include "strax.h"
#include "univers.h"
#include "private.h"
#include "private2.h"
#include "priv2005.h"
#include "mandator.h"
#include "bulstrad.h"
#include "dialog.h"
#include "shellapi.h"
#include "commdlg.h"
//#include "sovag.h"
#include "checkbox.h"
#include "belgreen.h"
#include <priv_frn.h>
#include "litva.h"
//#include "russ.h"
#include <sys/stat.h>
#include <time.h>
#include <setrange.h>

char ValidateInternalImport = 0;
//extern char bPrintOld;
char ActivateParadox = 0;
void DblToStr(char* str, double V, int Digit = 2);
char* GetRCStr(int strID);
void SetMenuText(HWND, char*, int);
extern HBITMAP ReadBitMap(char * fileName);
extern void ShowRuler(HDC);
extern void (*printFunc)(HDC);
int PageOffX;
int PageOffY;
int BadAgent;
int AgFieldsCount = 14;
char* AgentFields[] = {"Agent_code",
		       "Name",
		       "DivisionCode",
		       "Type",
		       "MandPcnt",
		       "ForeignPcnt",
		       "PolandPcnt",
		       "Leben2Pcnt",
		       "bulstradpcnt",
		       "sovag",
			   "litva",  //10
			   "belgreen",
			   "russ",
			    "type" };
char LastInsType[2] = "\0\0";
char* AgentDBName = "\\bases\\Agent";
void CreateBackup();
void ExpStr(_str s, int& IsRussian, int& IsEnglish, int& IsDigit, int& IsBad, int CheckMinus);
extern char* Avaria2Fields[];
extern TDialog* ActiveForm;


///////////////////////////////////////////////////////////////////////////////

struct SettingsBuffer {
	int  Is2000;
//	char DefAddress[60];
	int  bIsValidateImport;
	char Round[5];
	int  IsBackup;
	char BkPeriod[2];
//	char DefPlace[60];
};

class SettingsDlg : public TDialog {
	TComboBox*       DefCity;
	TComboBox*       Language;
	TComboBox*       m_MonthRegister;
	TCheckBox*       Is2000;
	TEdit*           DefAddress;
	TEdit*           Addresses;
	TCheckBox*       IsValidateImport;
	NumberEdit*      RoundNal;
	TCheckBox*       IsBK;
	NumberEdit*      BkPeriod;
	TEdit*           Company;
	TEdit*           CompanyKod;
public:
		SettingsDlg(PTWindowsObject p, void* buffer);

	void SetupWindow();
	void Ok(RTMessage Msg);

	 int IndexLanguage;
};

SettingsDlg::SettingsDlg(PTWindowsObject p, void* buffer)
    	   : TDialog(p, "SETTINGS")
{
    TransferBuffer = buffer;
	Language = new TComboBox(this, 101, 30);
    Language->DisableTransfer();
	DefCity = new TComboBox(this, 106, 60);
	DefCity->DisableTransfer();
	m_MonthRegister = new TComboBox(this, 105, 30);
    m_MonthRegister->DisableTransfer();
    Is2000 = new TCheckBox(this, 102, 0);
	DefAddress = new TEdit(this, 103, 60);
	DefAddress->DisableTransfer();
	Addresses = new TEdit(this, 112, 512);
	Addresses->DisableTransfer();
	IsValidateImport = new TCheckBox(this, 104, 0);
	RoundNal = new NumberEdit(this, 107, 5, 0);

	IsBK = new TCheckBox(this, 108, 0);
	BkPeriod = new NumberEdit(this, 109, 2, 0);

	(Company = new TEdit(this, 110, 128))->DisableTransfer();
	(CompanyKod = new TEdit(this, 111, 3))->DisableTransfer();

	IndexLanguage = -1;
}

extern const char* MANDATORY_SECT;

void SettingsDlg::SetupWindow()
{
    TDialog::SetupWindow();

    Language->AddString("РУССКИЙ");
    Language->AddString("АНГЛИЙСКИЙ");
    Language->SetSelIndex(IndexLanguage);

    m_MonthRegister->AddString("Маленькими");
    m_MonthRegister->AddString("Большими");
    m_MonthRegister->AddString("Только 1й символ большой");
	m_MonthRegister->SetSelIndex(((TRulerWin*)GetApplicationObject()->MainWindow)->MonthRegister);

	DynStr s, s2;
	GetPrivateProfileString(MANDATORY_SECT, "Company", "", s, s._sizeof(), ::ININame);
	Company->SetText(s);
	GetPrivateProfileString("DIVISION", "ID", "", s, s._sizeof(), ::ININame);
	CompanyKod->SetText(s);
	GetPrivateProfileString(MANDATORY_SECT, "DefAddress", "", s, s._sizeof(), ::ININame);
	DefAddress->SetText(s);

	for(int i = 0;; i++)
	{
		sprintf(s, "OwnerPlace%d", i);
		if(!GetPrivateProfileString(MANDATORY_SECT, s, "", s, s._sizeof(), ::ININame)) break;
		char* ch = strchr(s, ',');
		while(ch && *ch && (*ch == ',' || *ch == ' ')) ch++;
		DefCity->AddString(ch);
	}
	GetPrivateProfileString(MANDATORY_SECT, "DefPlacement", "", s, s._sizeof(), ::ININame);
	DefCity->SetText(s);

	for(i = 0;; i++)
	{
		sprintf(s, "Street%d", i);
		if(!GetPrivateProfileString(MANDATORY_SECT, s, "", s, s._sizeof(), ::ININame)) break;
        strcat(s2, s);
		strcat(s2, "\r\n");
	}
	Addresses->SetText(s2);
}

void SettingsDlg::Ok(RTMessage Msg)
{
    IndexLanguage = Language->GetSelIndex();
    if(IndexLanguage == -1) return;
    ((TRulerWin*)GetApplicationObject()->MainWindow)->MonthRegister = m_MonthRegister->GetSelIndex();
	DynStr s, s2;
	Company->GetText(s, s._sizeof());
	WritePrivateProfileString(MANDATORY_SECT, "Company", s, ::ININame);
	CompanyKod->GetText(s, s._sizeof());
	WritePrivateProfileString("DIVISION", "ID", s, ::ININame);
	DefAddress->GetText(s, s._sizeof());
	WritePrivateProfileString(MANDATORY_SECT, "DefAddress", s, ::ININame);
	DefCity->GetText(s, s._sizeof());
	WritePrivateProfileString(MANDATORY_SECT, "DefPlacement", s, ::ININame);

	Addresses->GetText(s, s._sizeof());
	char* ch = strtok(s, "\r\n");
    int i = 0;
	while(ch)
	{
		while(*ch && *ch == ' ') ch++;
		if(*ch)
        {
			sprintf(s2, "Street%d", i++);
			WritePrivateProfileString(MANDATORY_SECT, s2, ch, ::ININame);
		}
		ch = strtok(NULL, "\r\n");
    }

	TDialog::Ok(Msg);
}

extern char* Cross;
extern char* NoPrint;
extern char* SimpleText;

class NewAgent : public TDialog {
	TEdit*       CtrlCode;
	TCheckBox*   CtrlPeopleType;
	TEdit*       CtrlName;
	TEdit*       CtrlDiVCode;

	TEdit*       CtrlP1;
	TEdit*       CtrlP2;
	//TEdit*       CtrlP3;
	TEdit*       CtrlP4;
	TEdit*       CtrlP5;
	//TEdit*       CtrlP6;
	TEdit*       CtrlP7;
	TEdit*       CtrlP8;
	TEdit*       PrivNmb;
public:
		NewAgent(PTWindowsObject p, int*, char*, char*, char*, char*, char*, char*, char*, char*, char*, char*);
		char Code[5];
		char PrivNumber[17];
		int  Type;
		char Name[60];
		char DivCode[5];
		char PrMand[7];
		char PrForn[7];
		//char PrPoland[7];
		char PrLeben[7];
		char PrBulstrad[7];
		//char PrSovag[7];
		char PrBelGreen[7];
		char PrRuss[7];

		char* ACode;
        int*  AType;
		char* AName;
		char* ADivCode;
		char* Pr_Mand;
		char* Pr_Forn;
		//char* Pr_Poland;
		char* Pr_Leben;
		char* Pr_BulStrad;
		//char* Pr_Sovag;
		char* Pr_BelGreen;
		char* Pr_Russ;
		char* Pr_Number;

	void SetupWindow();
	void Ok(RTMessage Msg);
};

NewAgent::NewAgent(PTWindowsObject p, int* paramType, char* paramCode, char* paramName, char* paramDivCode, char* p1, char* p2, char* p4, char* p5, char* p7, char* p8, char* privNumber)
	: TDialog(p, "NEWAGENT"),
	  ACode(paramCode),
      AType(paramType),
	  AName(paramName),
	  ADivCode(paramDivCode),
	  Pr_Mand(p1),
	  Pr_Forn(p2),
	  //Pr_Poland(p3),
	  Pr_Leben(p4),
	  Pr_BulStrad(p5),
	  //Pr_Sovag(p6),
	  Pr_BelGreen(p7),
	  Pr_Russ(p8),
	  Pr_Number(privNumber)
{
	strcpy(Code, paramCode);
    Type = *paramType;
    strcpy(Name, paramName);
    strcpy(DivCode, paramDivCode);
    strcpy(PrMand, p1);
    strcpy(PrForn, p2);
    //strcpy(PrPoland, p3);
    strcpy(PrLeben, p4);
    strcpy(PrBulstrad, p5);
    //strcpy(PrSovag, p6);
	strcpy(PrBelGreen, p7);
	strcpy(PrRuss, p8);
	strcpy(PrivNumber, privNumber);

    TransferBuffer = Code;
	CtrlCode = new TEdit(this, 101, 5);
	PrivNmb = new TEdit(this, 151, 17);
	CtrlPeopleType = new TCheckBox(this, 112, 0);
    CtrlName = new TEdit(this, 102, 60);
    CtrlDiVCode = new NumberEdit(this, 103, 5, 0);

    CtrlP1 = new NumberEdit(this, 104, 7, 1, 1);
    CtrlP2 = new NumberEdit(this, 105, 7, 1, 1);
    //CtrlP3 = new NumberEdit(this, 106, 7, 1, 1);
    CtrlP4 = new NumberEdit(this, 107, 7, 1, 1);
    CtrlP5 = new NumberEdit(this, 108, 7, 1, 1);
    //CtrlP6 = new NumberEdit(this, 109, 7, 1, 1);
	CtrlP7 = new NumberEdit(this, 110, 7, 1, 1);
	CtrlP8 = new NumberEdit(this, 111, 7, 1, 1);
}

void NewAgent::SetupWindow()
{
    TDialog::SetupWindow();
    if(!*Code) SendMessage(CtrlCode->HWindow, EM_SETREADONLY, 0, 0);
}

void NewAgent::Ok(RTMessage Msg)
{
    TransferData(TF_GETDATA);
    if(strlen(Code) != 4 || !*Name) return;
	strcpy(ACode, Code);
    *AType = Type;
    strcpy(AName, Name);
    strcpy(ADivCode, DivCode);

	strcpy(Pr_Mand, PrMand);
    strcpy(Pr_Forn, PrForn);
    //strcpy(Pr_Poland, PrPoland);
    strcpy(Pr_Leben, PrLeben);
    strcpy(Pr_BulStrad, PrBulstrad);
    //strcpy(Pr_Sovag, PrSovag);
	strcpy(Pr_BelGreen, PrBelGreen);
	strcpy(Pr_Russ, PrRuss);
	strcpy(Pr_Number, PrivNumber);

    TDialog::Ok(Msg);
}

class TAgentSetup : public TDialog {
        TEdit*       Code;
	TListBox*   AgName;
	TEdit*       AgDivCode;
	TOpenBase    agents;
public:
		TAgentSetup(PTWindowsObject p);

	void SetupWindow();
	void Cancel(RTMessage Msg) = [ID_FIRST + IDCANCEL];
	void Ok(RTMessage Msg);
	void ChangeIndex(RTMessage Msg) = [ID_FIRST + 102];
	void CmNewAgent(RTMessage Msg) = [ID_FIRST + 103];
	void CmEditAgent(RTMessage Msg) = [ID_FIRST + 105];
	void SetButtons();
         void Fill();
};

void TAgentSetup::CmNewAgent(RTMessage)
{
	 char mCode[5] = "";
     int  mType = 1;
	 char mName[71] = "";
     char mDivCode[10] = "0";
     char P1[10];
     char P2[10];
     //char P3[10];
     char P4[10];
     char P5[10];
     //char P6[10];
	 char P7[10];
	 char P8[10];
	 char PNMB[17];

     *P1 = 0;
     *P2 = 0;
     //*P3 = 0;
     *P4 = 0;
     *P5 = 0;
     //*P6 = 0;
	 *P7 = 0;
	 *P8 = 0;
	 *PNMB = 0;

	 NewAgent* dlg = new NewAgent(this, &mType, mCode, mName, mDivCode, P1, P2, P4, P5, P7, P8, PNMB);
	 if(!agents.GetRecords())
	 {
	    MessageBox(HWindow, GetRCStr(521), "Сообщаю", MB_OK | MB_ICONINFORMATION);
	    *dlg->Code = 0;
     }
	 else
	 {
		agents.Last();
		do {
     	    agents.Get();
			agents(mCode, 5, 0);
			agents.Prev();
		}
		while(mCode[0] < '0' || mCode[0] > '9');

	    sprintf(dlg->Code, "%04u", atoi(mCode) + 1);
     }

	 if(GetModule()->ExecDialog(dlg) == IDOK)
	 {
		int idx = AgName->FindExactString(mName, -1);
		if(idx != -1)
		{
			 MessageBox(HWindow, GetRCStr(543), "Очень важно", MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL);
			 AgName->GetString(mName, idx);
			 MessageBox(HWindow, mName, "Очень важно", MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL);
			 AgName->SetSelIndex(idx);
		}
		else
		{
			if(atoi(mCode) < 100)
			{
				MessageBox(HWindow, "Ошибка < 0", "Очень важно", MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL);
				return;
			}
			idx = AgName->FindString(mName, -1);
			if(idx != -1)
			{
				MessageBox(HWindow, GetRCStr(543), mName, MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL);
			 	AgName->GetString(mName, idx);
			 	MessageBox(HWindow, mName, "Очень важно", MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL);
				AgName->SetSelIndex(idx);
				return;
            }
			agents(0, mCode);
			agents(1, mName);
			agents(2, atof(mDivCode));
			agents(4, atof(P1));
			agents(5, atof(P2));
			agents(6, *(double*)(PNMB+0));
			agents(7, atof(P4));
			agents(8, atof(P5));
			agents(9, *(double*)(PNMB+8));
			agents(11, atof(P7));
			agents(12, atof(P8));
			agents(13, mType ? "F" : "U");
			if(agents.AddRecord() == PXSUCCESS)
			{
			   Code->SetText(mCode);
			   strcat(mName, ", ");
			   strcat(mName, PNMB);
			   AgName->SetSelIndex(AgName->AddString(mName));
			   SendMessage(AgName->HWindow, LB_SETITEMDATA, AgName->GetSelIndex(), atol(mCode));
			   EnableWindow(GetDlgItem(HWindow, IDOK), TRUE);
			}
			SetButtons();
	     }
     }
}
void TAgentSetup::CmEditAgent(RTMessage)
{
	char mCode[5] = "";
	char mType[2];
    char mName[71] = "";
    char mDivCode[10] = "0";
    char P1[10];
    char P2[10];
    //char P3[10];
    char P4[10];
    char P5[10];
    //char P6[10];
	char P7[10];
	char P8[10];
	char PNMB[17];

    int Index = AgName->GetSelIndex();
    if(Index == -1) return;
    Code->GetText(mCode, sizeof mCode);
    agents(0, mCode);
    if(PXSrchKey(agents.getTblHandle(), agents.getRecHandle(), 1, SEARCHFIRST) != PXSUCCESS) return;
    agents.Get();

    agents(mName, sizeof mName, 1);
    double N;
    agents(N, 2);
    DblToStr(mDivCode, N);
    agents(N, 4);
    DblToStr(P1, N);
    agents(N, 5);
    DblToStr(P2, N);
	agents(*(double*)PNMB, 6);
	agents(*(double*)(PNMB+8), 9);
	PNMB[sizeof(PNMB)-1] = 0;
	//DblToStr(P3, N);
    agents(N, 7);
    DblToStr(P4, N);
    agents(N, 8);
    DblToStr(P5, N);
    //agents(N, 9);
    //DblToStr(P6, N);
	agents(N, 11);
    DblToStr(P7, N);
	agents(N, 12);
	DblToStr(P8, N);
	agents(mType, sizeof mType, 13);
	int iType = *mType == 'F';

	NewAgent* dlg = new NewAgent(this, &iType, mCode, mName, mDivCode, P1, P2, P4, P5, P7, P8, PNMB);

	if(GetModule()->ExecDialog(dlg) == IDOK)
	{
		agents.Get();
		agents(1, mName);
	    agents(2, atof(mDivCode));
	    agents(4, atof(P1));
	    agents(5, atof(P2));
		agents(6, *(double*)PNMB);
		agents(7, atof(P4));
	    agents(8, atof(P5));
		agents(9, *(double*)(PNMB+8));
		agents(11, atof(P7));
		agents(12, atof(P8));
		agents(13, iType ? "F" : "U");
		if(PXRecUpdate(agents.getTblHandle(), agents.getRecHandle()) != PXSUCCESS)
		{
		    MessageBox(HWindow, "Ошибка сохранения данных", "Сообщаю", MB_OK | MB_ICONINFORMATION);
		}
		else
		{
			AgDivCode->SetText(mDivCode);
			AgName->DeleteString(AgName->GetSelIndex());
			strcat(mName, ", ");
			strcat(mName, PNMB);
			int Index = AgName->AddString(mName);
			//SendMessage(AgName->HWindow, CB_SETITEMDATA, 0, atol(mCode));
			AgName->SetSelIndex(Index);
		    SendMessage(AgName->HWindow, LB_SETITEMDATA, AgName->GetSelIndex(), atol(mCode));
		}
    }
}

TAgentSetup::TAgentSetup(PTWindowsObject p)
	   : TDialog(p, "AGENT"),
	     agents(AgentDBName, AgentFields, AgFieldsCount)
{
    if(agents.pxErr) exit(0);
    Code = new TEdit(this, 101, 5);
    AgName = new TListBox(this, 102);
    AgDivCode = new TEdit(this, 104, 5);
}

void TAgentSetup::SetButtons()
{
    int Index = AgName->GetSelIndex();
	if(Index != -1) {
      char Tmp[100];
	  sprintf(Tmp, "%04ld", SendMessage(AgName->HWindow, LB_GETITEMDATA, Index, 0));
	  agents(0, Tmp);
	  if(!PXSrchKey(agents.getTblHandle(), agents.getRecHandle(), 1, SEARCHFIRST)) {
	   agents.Get();
	   agents(Tmp, 100, 0);
	   Code->SetText(Tmp);
	   double N;
	   agents(N, 2);
	   sprintf(Tmp, "%g", N);
	   AgDivCode->SetText(Tmp);
	}
      }
   EnableWindow(GetDlgItem(HWindow, IDOK), Index != -1);
}

void TAgentSetup::ChangeIndex(RTMessage)
{
      SetButtons();
}

void TAgentSetup::Fill()
{
   for(int i = 0; i < agents.GetRecords(); i++)
   {
	  agents.Get();

	  DynStr Tmp;
	  agents(Tmp, 100, 0);
	  if(Tmp[0] < '0' || Tmp[0] > '9')
	  {
		  if(Tmp[0] == 0)
		  {
			  agents(Tmp, 100, 1);
			  MessageBox(HWindow, Tmp, "Код пуст", MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL);
		  }
	      agents.Next();
		  continue;
      }

	  memset(Tmp, 0, Tmp._sizeof());
	  agents(Tmp, 100, 1);

	  _str s;
	  strcpy(s.s, Tmp);
	  int IsRussian, IsEnglish, IsDigit, IsBad_;
	  ExpStr(s, IsRussian, IsEnglish, IsDigit, IsBad_, 1);
	  if(IsEnglish)
      {
		  MessageBox(HWindow, GetRCStr(626), Tmp, MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL);
      }

	  if(AgName->FindString(Tmp, -1) != -1)
	  {
		  MessageBox(HWindow, GetRCStr(543), Tmp, MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL);
		  strcat(Tmp, " УДАЛИ");
		  agents(1, Tmp);
		  if(PXRecUpdate(agents.getTblHandle(), agents.getRecHandle()) != PXSUCCESS)
          {
			  MessageBox(HWindow, "Ошибка", Tmp, MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL);
          }
	  }

	  strcat(Tmp, ", ");
	  char* off = Tmp+strlen(Tmp);
	  agents(*(double*)off, 6);
	  agents(*(double*)(off+8), 9);

	  int idx = AgName->AddString(Tmp);
	  agents(Tmp, 100, 0);
	  SendMessage(AgName->HWindow, LB_SETITEMDATA, idx, atol(Tmp));
	  agents.Next();
	}

   HMENU m = GetSystemMenu(HWindow, FALSE);
   DeleteMenu(m, 1, MF_BYPOSITION);
   agents(0, Dlg_base::Agent_Code);

   if(!PXSrchFld(agents.getTblHandle(), agents.getRecHandle(), agents.getField(0), SEARCHFIRST))
   {
	   char Tmp[100];
       agents.Get();
       agents(Tmp, 100, 1);
       AgName->SetSelString(Tmp, -1);
   }
   else
   {
       AgName->SetSelIndex(0);
   }
   SetButtons();
}

void TAgentSetup::SetupWindow()
{
   TDialog::SetupWindow();
   Fill();
}

void TAgentSetup::Cancel(RTMessage Msg)
{
     if(BadAgent)
      if(MessageBox(HWindow, GetRCStr(522), "Выбирай", MB_YESNO | MB_ICONQUESTION) == IDYES)
	return;
      else exit(0);
     TDialog::Cancel(Msg);
}

void TAgentSetup::Ok(RTMessage Msg)
{
    Code->GetText(Dlg_base::Agent_Code, sizeof Dlg_base::Agent_Code);
	AgName->GetString(Dlg_base::Agent_Name, AgName->GetSelIndex());
    SetMenuText(GetApplication()->MainWindow->HWindow, Dlg_base::Agent_Name, 2);

    //DefAgent
    //WritePrivateProfileString("FOREIGN", "DefAgent", Dlg_base::Agent_Code, ININame);
    WritePrivateProfileString("PRIVATE2", "DefAgent", Dlg_base::Agent_Code, ININame);
	WritePrivateProfileString("PRIV2005", "DefAgent", Dlg_base::Agent_Code, ININame);
	WritePrivateProfileString("MANDATORY", "DefAgent", Dlg_base::Agent_Code, ININame);
    //WritePrivateProfileString("POLAND", "DefAgent", Dlg_base::Agent_Code, ININame);
	//WritePrivateProfileString("SOVAG", "DefAgent", Dlg_base::Agent_Code, ININame);
	WritePrivateProfileString("BELGREEN", "DefAgent", Dlg_base::Agent_Code, ININame);

	DynStr s;
	if(!GetPrivateProfileString("RUSSIAN", "INI", "", s, s._sizeof(), ININame))
		strcpy(s, ININame);
	WritePrivateProfileString("RUSSIAN", "DefAgent", Dlg_base::Agent_Code, s);

	if(!GetPrivateProfileString("UNIVERSAL", "INI", "", s, s._sizeof(), ININame))
		strcpy(s, ININame);
	WritePrivateProfileString("UNIVERSAL", "DefAgent", Dlg_base::Agent_Code, s);

	if(!GetPrivateProfileString("KOMPLEX", "INI", "", s, s._sizeof(), ININame))
		strcpy(s, ININame);
	WritePrivateProfileString("MANDATORY", "DefAgent", Dlg_base::Agent_Code, s);

	TDialog::Ok(Msg);
}

///////////////////////////////////////////////////////////////////////////

// Construct a TRulerWin object
extern int R, G, B;
int IsBackup = 1;
//HWND glbHWindow;

TRulerWin::TRulerWin(PTWindowsObject AParent, LPSTR ATitle, PTModule AModule)
        : TWindow( AParent, ATitle, AModule ),
          array(10, 0, 10)
{
	AssignMenu(ActivateParadox ? ID_MENU : 101);
    msg_reciever = 0;
    Attr.Style |= WS_HSCROLL | WS_VSCROLL;
    Attr.X = GetSystemMetrics(SM_CXSCREEN) / 8;
    Attr.Y = GetSystemMetrics(SM_CYSCREEN) / 8;
    Attr.H = Attr.Y * 6;
    Attr.W = Attr.X * 6;
    m_Printer = new TPrinter;
    press = 0;
    Scroller = new TScroller(this, 6, 6, 20, 100);
    Scroller->TrackMode = 0;
    m_bmp = 0;
    BmpDX = 210;
    BmpDY = 297;
    printFunc = ShowRuler;
    Is2000Full = 1;
	Language = 0;
	*CurrentBlkFile = 0;
}

void TRulerWin::AgentSetup(RTMessage)
{
    if(GetModule()->ExecDialog(new TAgentSetup(this)) == IDOK) {
	if(BadAgent)
	  WritePrivateProfileString("GREEN", "Agent", Dlg_base::Agent_Code, ININame),
	  BadAgent = 0;
    }
}
/*
void TRulerWin::WMClose(RTMessage Msg)
{
	if(ActiveForm) DestroyWindow(ActiveForm->HWindow);
	TWindow::WMClose(Msg);
}
*/
typedef BOOL (FAR PASCAL *SLWA)(
  HWND hwnd,           // handle to the layered window
  COLORREF crKey,      // specifies the color key
  BYTE bAlpha,         // value for the blend function
  DWORD dwFlags        // action
);

void TRulerWin::SetupWindow()
{
   TWindow::SetupWindow();

   Caption("noname.blk");

   GetPrivateProfileString("MANDATORY", "DefAgent", "", Dlg_base::Agent_Code, sizeof Dlg_base::Agent_Code, ININame);
//   GetPrivateProfileString("2РП", "Date2IsEmpty", "", TextTo2RP, sizeof TextTo2RP, ININame);
   Is2000Full = GetPrivateProfileInt("2000 Год", "IsFull", 1, ININame);
   IsBackup = GetPrivateProfileInt("BACKUP", "Period", 5, ININame);
   CreateBackup();

//   bIsMandMonth = GetPrivateProfileInt("MANDATORY", "Месяцы", 0, ININame);
   Language = GetPrivateProfileInt("Язык", "Номер", 0, ININame);
   MonthRegister = GetPrivateProfileInt("Месяцы", "Регистр", 0, ININame);
   if(MonthRegister > 2 || MonthRegister < 0) MonthRegister = 2;

   char fullName[100];

   if(ActivateParadox)
   {
	   TOpenBase agents(AgentDBName, AgentFields, 5);
	   if(agents.pxErr) exit(0);

	   agents(0, "АЛВН");
	   if(PXSrchKey(agents.getTblHandle(), agents.getRecHandle(), 1, SEARCHFIRST))
	   {
		   agents(0, "АЛВН");
		   agents(1, "АЛЬВЕНА");
		   agents(2, 1000.);
		   agents(3, "U");
		   agents(4, 7.);
		   agents.AddRecord();
	   }

	   agents(0, Dlg_base::Agent_Code);
	   if(PXSrchKey(agents.getTblHandle(), agents.getRecHandle(), 1, SEARCHFIRST))
	   {
			BadAgent = 1;
			PostMessage(HWindow, WM_COMMAND, 113, 0);
	   }
	   else
       {
			PostMessage(HWindow, WM_COMMAND, 200, MAKELONG(0, 55));
       }
	   agents.Get();
	   agents(Dlg_base::Agent_Name, sizeof Dlg_base::Agent_Name, 1);
   }

   GetModuleFileName(GetModule()->hInstance, fullName, sizeof fullName);
   *strrchr(fullName, '.') = 0;
   strcat(fullName, ".cnf");
   FILE* f = fopen(fullName, "rt");
   if(f)
    {
       fscanf(f, "%d %d", &PageOffX, &PageOffY);
       fclose(f);
    }
   else
   if(ActivateParadox)
   	   PostMessage(HWindow, WM_COMMAND, 109, 0);

   GetPrivateProfileString("OPTIONS", "RGB", "", fullName, sizeof fullName, ININame);
   char *ch = strtok(fullName, ",");
   if(ch) if(!atoi(ch)) R = 0;
   ch = strtok(0, ",");
   if(ch) if(!atoi(ch)) G = 0;
   ch = strtok(0, ",");
   if(ch) if(!atoi(ch)) B = 0;
   Gray = CreateSolidBrush(RGB(192, 192, 192));
   White = CreateSolidBrush(RGB(R, G, B));

   HMENU menu = GetMenu(HWindow);
   //menu = GetSubMenu(menu, 4);
   char buff[32];
   //GetPrivateProfileString("MANDATORY", "FindMode", "", buff, sizeof buff, ININame);
   //CheckMenuItem(menu, 970, MF_BYCOMMAND | (strcmp(buff, "ONSERVER") ? MF_UNCHECKED : MF_CHECKED));
   //GetPrivateProfileString("MANDATORY", "FindModeParam", "", buff, sizeof buff, ININame);
   //CheckMenuItem(menu, 971, MF_BYCOMMAND | (strcmp(buff, "FULLSEARCH") ? MF_CHECKED : MF_UNCHECKED));
   menu = GetSubMenu(GetMenu(HWindow), 3);
//   CheckMenuItem(menu, 137, MF_BYCOMMAND | (GetPrivateProfileInt("MANDATORY", "NewCalculate", 1, ININame) ? MF_CHECKED : MF_UNCHECKED));
   CheckMenuItem(menu, 144, MF_BYCOMMAND | (GetPrivateProfileInt("MANDATORY", "FillAddr", 0, ININame) ? MF_CHECKED : MF_UNCHECKED));
//   CheckMenuItem(menu, 184, MF_BYCOMMAND | (GetPrivateProfileInt("PRIV2005", "Print", 2006, ININame) == 2006 ? MF_CHECKED : MF_UNCHECKED));
}

void TRulerWin::Zoom()
{
   ShowWindow(HWindow, IsZoomed(HWindow) ? SW_SHOWNORMAL : SW_SHOWMAXIMIZED);
}

void TRulerWin::Left()
{
   Scroller->ScrollBy(-1, 0);
}

void TRulerWin::Right()
{
   Scroller->ScrollBy(1, 0);
}

void TRulerWin::Up()
{
   Scroller->ScrollBy(0, -1);
}

void TRulerWin::Down()
{
   Scroller->ScrollBy(0, 1);
}

int moffx, moffy;

RECT from;

void TRulerWin::OffsetBlank(RTMessage)
{
   char buff[20];
   memset(buff, 0, sizeof buff);

   if(GetModule()->ExecDialog(new OffBlank(this, buff)) == IDCANCEL) return;
   int offX = atoi(buff) * 0.3;
   int offY = atoi(buff + 5) * 0.3;

   for(int i = 0; i < array.getItemsInContainer(); i++)
    if((O&)array[i] != NOOBJECT)
     {
       ((O&)array[i]).lu.x += offX;
       ((O&)array[i]).rd.x += offX;
       ((O&)array[i]).lu.y += offY;
       ((O&)array[i]).rd.y += offY;
     }

   if(offX || offY) InvalidateRect(HWindow, 0, 0);
}

void TRulerWin::WMLButtonDown(RTMessage Msg)
{
   press = 1;
   lu.x = LOWORD(Msg.LParam);
   lu.y = HIWORD(Msg.LParam);
   rd = lu;
   SetRect(&from, lu.x, lu.y, lu.x, lu.y);
   SetCapture(HWindow);
   for(int i = 0; i < array.getItemsInContainer(); i++)
    {
       if((O&)array[i] != NOOBJECT)
        {
         int ret = ((O&)array[i]).A(lu.x, lu.y);
         if(ret)
           {
              press = 2;
              prtO = (O*)&array[i];

              if(ret == -1)
               {
                 lu.x = rd.x - (prtO->rd.x - prtO->lu.x);
                 lu.y = rd.y - (prtO->rd.y - prtO->lu.y);
                 press = 3;
               }

              SetRect(&from, prtO->lu.x, prtO->lu.y, prtO->rd.x + 10, prtO->rd.y + 10);
              /*TScroller& s = *Scroller;
              from.left -= s.XUnit * s.XPos + 10;
              from.top -= s.YUnit * s.YPos + 10;
              from.right -= s.XUnit * s.XPos + 10;
              from.bottom -= s.YUnit * s.YPos + 10;*/

              moffx = -(prtO->lu.x - ((lu.x - 10) + (Scroller->XUnit*Scroller->XPos)));
              moffy = -(prtO->lu.y - ((lu.y - 10) + (Scroller->YUnit*Scroller->YPos)));

              WMMouseMove(Msg);
              break;
           }
        }
    }
}

POINT ptCurrent;

void TRulerWin::WMRButtonDown(RTMessage Msg)
{
   if(press) return;

   ptCurrent = MAKEPOINT(Msg.LParam);

   prtO = 0;

   for(int i = 0; i < array.getItemsInContainer(); i++)
    if((O&)array[i] != NOOBJECT)
     if(((O&)array[i]).A(ptCurrent.x, ptCurrent.y))
      {
	prtO = &(O&)array[i];
      }

   if(prtO)
    {
      HMENU hmenu = CreatePopupMenu();
      int isCross = !strcmp(prtO->MainFontRec.lfFaceName, Cross);
      int isNoPrint = !strcmp(prtO->MainFontRec.lfFaceName, NoPrint);
      int isStaticText = !strcmp(prtO->MainFontRec.lfFaceName, SimpleText);
      AppendMenu(hmenu, MF_ENABLED | ((!isNoPrint && !isCross && !isStaticText) ? MF_CHECKED : 0), 501, "Шрифт...");
      AppendMenu(hmenu, MF_ENABLED, 502, "Параметры...");
      AppendMenu(hmenu, MF_SEPARATOR, 0, "");
      AppendMenu(hmenu, MF_ENABLED | (isNoPrint ? MF_CHECKED : 0), 503, "Не печатать");
      AppendMenu(hmenu, MF_ENABLED | (isCross ? MF_CHECKED : 0), 504, "Крестик");
      AppendMenu(hmenu, MF_ENABLED | (isStaticText ? MF_CHECKED : 0), 505, "Произвольный текст");
      AppendMenu(hmenu, MF_SEPARATOR, 0, "");
      AppendMenu(hmenu, MF_ENABLED, 500, "Удалить");
      POINT ptMenu = ptCurrent;
      ClientToScreen(HWindow, &ptMenu);
      TrackPopupMenu(hmenu, TPM_LEFTALIGN, ptMenu.x, ptMenu.y, 0, HWindow, NULL);
    }
}

void TRulerWin::CMDelete(RTMessage)
{
   RECT r = { prtO->lu.x, prtO->lu.y, prtO->rd.x + 10, prtO->rd.y + 10 };

   TScroller& s = *Scroller;
   r.left -= s.XUnit * s.XPos;
   r.top -= s.YUnit * s.YPos;
   r.right -= s.XUnit * s.XPos;
   r.bottom -= s.YUnit * s.YPos;

   InvalidateRect(HWindow, &r, 0);

   for(int i = 0; i < array.getItemsInContainer(); i++)
    if(prtO == &array[i])
     {
       array.destroy(i);
       break;
     }
}

void TRulerWin::CMSimpleText(RTMessage)
{
   prtO->SimpleText(ptCurrent.x, ptCurrent.y);
}

void TRulerWin::CMParam(RTMessage)
{
   prtO->ChangeParam(ptCurrent.x, ptCurrent.y);
}

void TRulerWin::CMFonts(RTMessage)
{
   prtO->Fonts(ptCurrent.x, ptCurrent.y);//ChangeParam(ptCurrent.x, ptCurrent.y);
}

void TRulerWin::CMCross(RTMessage)
{
   prtO->Cross(ptCurrent.x, ptCurrent.y);//ChangeParam(ptCurrent.x, ptCurrent.y);
}

void TRulerWin::CMNoPrint(RTMessage)
{
   prtO->NoPrint(ptCurrent.x, ptCurrent.y);//ChangeParam(ptCurrent.x, ptCurrent.y);
}

void TRulerWin::WMLButtonUp(RTMessage)
{
   if(press)
    {
      MessageBeep(0);

      HDC DC = GetDC(HWindow);
      //RECT r = { lu.x, lu.y, rd.x, rd.y };
      //if(press != 2) DrawFocusRect(DC, &r);
      ReleaseDC(HWindow, DC);
      ReleaseCapture();
    }

   int Add = 1;

   if(press == 1)
    {

      if((abs(lu.y - rd.y) > 5) && (abs(lu.x - rd.x) > 5))
        {
          array.add((Object&)*(prtO = new O(lu.x, lu.y, rd.x, rd.y)));
        }
      else Add = 0;
    }
   else if(press == 2)
    {
       int ox = ((prtO->rd.x - prtO->lu.x) - moffx);
       int oy = ((prtO->rd.y - prtO->lu.y) - moffy);

       prtO->lu.x = (lu.x - 10) + (Scroller->XUnit*Scroller->XPos) + ox;
       prtO->rd.x = (rd.x - 10) + (Scroller->XUnit*Scroller->XPos) + ox;

       prtO->lu.y = (lu.y - 10) + (Scroller->YUnit*Scroller->YPos) + oy;
       prtO->rd.y = (rd.y - 10) + (Scroller->YUnit*Scroller->YPos) + oy;
    }
   else if(press == 3)
    {
       prtO->rd.x = (rd.x - 10) + (Scroller->XUnit*Scroller->XPos);
       prtO->rd.y = (rd.y - 10) + (Scroller->YUnit*Scroller->YPos);
    }

   if(press > 1)
    {
      RECT to, res;
      SetRect(&to, prtO->lu.x, prtO->lu.y, prtO->rd.x + 10, prtO->rd.y + 10);
      UnionRect(&res, &from, &to);
      TScroller&s = *Scroller;
      res.left -= s.XUnit * s.XPos;
      res.top -= s.YUnit * s.YPos;
      res.right -= s.XUnit * s.XPos;
      res.bottom -= s.YUnit * s.YPos;
      if(Add) InvalidateRect(HWindow, &res, 0);
    }

   press = 0;
}

int KoordShow = 0;

class TAboutDialog : public TDialog
{
	HFONT f;
public:
	TAboutDialog(PTWindowsObject o) : TDialog(o, "ABOUT") {};
	~TAboutDialog()
	{
		if(f)
		{
        	DeleteObject(f);
        }
    }
	void WMLSetCursor(RTMessage m) = [WM_SETCURSOR]
	{
		POINT pt;
		RECT r;
		GetWindowRect(GetDlgItem(HWindow, 125), &r);
		GetCursorPos(&pt);
		if(PtInRect(&r, pt))
		{
			SetCursor(LoadCursor(0, IDC_UPARROW));
			SetWindowLong(HWindow, DWL_MSGRESULT, MAKELONG(TRUE, 0));
			return;
		}
		DefWndProc(m);
	}
	void WMLButtonDown(RTMessage m) = [WM_LBUTTONDOWN]
	{
		POINT pt;
		RECT r;
		GetWindowRect(GetDlgItem(HWindow, 125), &r);
		GetCursorPos(&pt);
		if(PtInRect(&r, pt))
		{
			ShellExecute(0, "open", "mailto:kaacpp@mail.ru?subject=Программа%20БЛАНК&body=Здравствуйте%20Анатолий!", "", "", SW_SHOWNA);
        }
		EndDialog(HWindow, 0);
	}
	void SetupWindow()
	{
		TDialog::SetupWindow();
		char buff[64];
		GetDlgItemText(HWindow, 1257, buff, sizeof buff);
		if(strstr(buff, "тол") == 0 || strstr(buff, "ост") == 0)
		{
			for(int i = 0;; i++) MessageBeep(0);
                }
		struct stat statbuf;
		GetDlgItemText(HWindow, 100, buff, sizeof buff);
		strcat(buff, " (от ");
		DynStr name;
		GetModuleFileName(GetApplicationObject()->hInstance, name, name._sizeof());
		stat(name, &statbuf);
		struct tm* t = gmtime(&statbuf.st_ctime);
		sprintf(buff + strlen(buff), "%02d.%02d.%d)", t->tm_mday, t->tm_mon+1, 1900 + t->tm_year);
		SetDlgItemText(HWindow, 100, buff);
		f = (HFONT)SendMessage(HWindow, WM_GETFONT, 0, 0);
		if(f)
		{
			LOGFONT lf;
			GetObject(f, sizeof lf, &lf);
			lf.lfWeight = FW_BOLD;
            		lf.lfUnderline = 1;
			f = CreateFontIndirect(&lf);
			SendMessage(GetDlgItem(HWindow, 125), WM_SETFONT, (WPARAM)f, 0);
		}

		int limit = 25;
		if(strlen(name) > 2*limit)
		{
			strcpy(name + limit, name + strlen(name) - limit);
			memcpy(name + limit, "...", 3);
		}
        	strcat(name, "\n");
		strcat(name, (strchr(::ININame, '\\') == 0 ? "ЛОКАЛЬНЫЙ" : (::ININame)));
		GetProfileString("Paradox Engine", "NetNamePath", "ПУТЬ ДЛЯ PARADOX НЕ УКАЗАН", buff, sizeof buff);
		strcat(name, "\n");
		strcat(name, buff);
		strcat(name, "\nПодразделение: ");
		GetPrivateProfileString("DIVISION", "ID", "", name + strlen(name), 10, ::ININame);

		SetDlgItemText(HWindow, 5021, name);
	}
};

int _IsValidPolisNumberFull(const char* SECTION, _str s, long n, char* INI, char* Agent, int AskRange);

void TRulerWin::About(RTMessage msg)
{
	TDialog* About = (TDialog*)GetApplication()->ExecDialog(new TAboutDialog(this));
	KoordShow++;
	if(!koord && KoordShow > 1) GetModule()->MakeWindow(new K(this));

	if(msg.LP.Hi == 55)
	{
		_str s;
		_IsValidPolisNumberFull("A", s, 0, 0, "", 1);
    }
}

void TRulerWin::WMMouseMove(RTMessage Msg)
{
   static RECT r;

   if(press)
     {
        HDC DC = GetDC(HWindow);

        DrawFocusRect(DC, &r);

        rd.x = LOWORD(Msg.LParam);
        rd.y = HIWORD(Msg.LParam);

        SetRect(&r,  lu.x, lu.y, rd.x, rd.y );

        if(press == 2)
          {
             lu.x = rd.x - (prtO->rd.x - prtO->lu.x);
             lu.y = rd.y - (prtO->rd.y - prtO->lu.y);
             OffsetRect(&r, (prtO->rd.x - prtO->lu.x) - moffx, (prtO->rd.y - prtO->lu.y) - moffy);
          }

        DrawFocusRect(DC, &r);
        ReleaseDC(HWindow, DC);
     }
   else
    {
       SetRect(&r, 0, 0, 0, 0);
       lu.x = LOWORD(Msg.LParam);
       lu.y = HIWORD(Msg.LParam);
       rd = lu;
    }

   if(koord)
    if(press != 2) koord->SetK( lu.x, lu.y, rd.x, rd.y );
    else koord->SetK( r.left, r.top, r.left, r.top );

   if(press == 2) SetCursor(LoadCursor(0, IDC_SIZE));
   if(press == 3) SetCursor(LoadCursor(0, IDC_SIZENWSE));
}

// Destroy a TRulerWin object

TRulerWin::~TRulerWin( void )
{
    if(m_Printer) delete m_Printer;
    if(m_bmp) DeleteObject(m_bmp);
}

void TRulerWin::Help(RTMessage)
{
    WinHelp(HWindow, "blank.hlp", HELP_CONTENTS, 0);
}

void TRulerWin::GetWindowClass(WNDCLASS & AWndClass)
{
   TWindow::GetWindowClass(AWndClass);
   AWndClass.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
   AWndClass.hCursor = (HCURSOR)LoadCursor(GetApplication()->hInstance, "HAND");
   AWndClass.hIcon = (HCURSOR)LoadIcon(GetApplication()->hInstance, "PRN");
}
// Execute File:Print command

void TRulerWin::WMMinMax(RTMessage Msg)
{
   ((POINT far *)Msg.LParam)[3].x = 620;
   ((POINT far *)Msg.LParam)[3].y = 460;
}

void TRulerWin::CMFilePrint(RTMessage)
{
  PTRulerOut Printout = 0;

  if ( m_Printer )
  {
    Printout = new TRulerOut("Бланк");
    if ( Printout )
    {
      Printout->SetBanding( TRUE );
      m_Printer->Print(this, Printout);
      delete Printout;
    }
  }
}


// Execute File:Printer-setup command

void TRulerWin::CMFilePrinterSetup(RTMessage)
{
  if ( m_Printer )
    m_Printer->Setup(this);
}


// Execute File:Exit command

void TRulerWin::CMFileExit(RTMessage)
{
  CloseWindow();
}


// Paint window's contents on screen

void TRulerWin::Paint(HDC DC, PAINTSTRUCT& ps)
{
   RECT rc;

   TScroller& s = *Scroller;
   GetClientRect(HWindow, &rc);
   SetViewportOrg(DC, 10 - s.XUnit * s.XPos, 10 - s.YUnit * s.YPos);
   Rectangle(DC, 0, 0, 420 * 3 / 2, 297 * 3);

   if(m_bmp)
    {
      SetCursor(LoadCursor(0, IDC_WAIT));
	  HGDIOBJ line = SelectObject(DC, CreatePen(PS_SOLID, 3, RGB(0, 0, 255)));
      Rectangle(DC, 0, 0, BmpDX * 3 + 3, BmpDY * 3 + 3);
      DeleteObject(SelectObject(DC, line));

      BITMAP b;
      GetObject(m_bmp, sizeof(BITMAP), &b);
      HDC hMemDC = CreateCompatibleDC(DC);
      HGDIOBJ oldBmp = SelectObject(hMemDC, m_bmp);
      StretchBlt(DC, 1, 1, BmpDX * 3 + 1, BmpDY * 3 + 1, hMemDC, 0, 0, b.bmWidth, b.bmHeight, SRCCOPY);
      SelectObject(hMemDC, oldBmp);
      DeleteDC(hMemDC);
      SetCursor((HCURSOR)GetClassWord(HWindow, GCW_HCURSOR));
	}

   for(int i = 0; i < array.getItemsInContainer(); i++)
    {
       if((O&)array[i] != NOOBJECT)
         ((O&)array[i]).Draw(DC, 1);

    }
}

BOOL MyOpenFile(HWND HWindow, OPENFILENAME& ofn, char* szFilter, char* szDirName, char* szFile, char* Title, char* ext, int Load = 1 )
{
    DynStr strfileTitle;

    for (int i = 0; szFilter[i] != '\0'; i++)
      if (szFilter[i] == '|') szFilter[i] = '\0';

	GetModuleFileName(GetApplicationObject()->hInstance, szDirName, 128);
    *strrchr(szDirName, '\\') = 0;
    strcat(szDirName, ext);
    chdir(szDirName);

    memset(&ofn, 0, sizeof(OPENFILENAME));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = HWindow;
    ofn.lpstrFilter = szFilter;
    ofn.nFilterIndex = 0;
    ofn.lpstrFile= szFile;
    ofn.nMaxFile = 255;
    ofn.lpstrTitle = Title;
    ofn.lpstrFileTitle = strfileTitle;
    ofn.nMaxFileTitle = strfileTitle._sizeof();
    ofn.lpstrInitialDir = szDirName;
	ofn.Flags = OFN_HIDEREADONLY | OFN_SHOWHELP | OFN_OVERWRITEPROMPT;
    if(Load) ofn.Flags |= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    return Load ? GetOpenFileName(&ofn) : GetSaveFileName(&ofn);
}


void TRulerWin::Open(RTMessage)
{
    OPENFILENAME ofn;
	DynStr szDirName;
	DynStr szFile;
    DynStr szFilter;
    strcpy(szFilter, "Файлы бланков(*.blk)|*.blk||");

	if(MyOpenFile(HWindow, ofn, szFilter, szDirName, szFile, "Открыть бланк", "\\blk")) {
	  char Key[10];
	  sprintf(Key, "Name%c", *LastInsType);
	  WritePrivateProfileString("LastTmpl", Key, ofn.lpstrFile, ::ININame);
	  LoadLastFile(*LastInsType);
	}
}

void TRulerWin::LoadBlank(LPSTR name)
{
	DynStr szDirName;
	if(*CurrentBlkFile)
	 {
		if(name==0) {
		if(!((GetAsyncKeyState(VK_SHIFT) & 0x8000) && (GetAsyncKeyState(VK_CONTROL) & 0x8000)))
		  Read(CurrentBlkFile);
          }
		else
        {
			strcpy(szDirName,CurrentBlkFile);
			//MessageBox(0, CurrentBlkFile, CurrentBlkFile, 0);
			strcpy(strrchr(szDirName, '\\')+1, name);
			Read(szDirName);
		}
	 }
	 else {
	  TMessage msg;
		Open(msg);
        }
}

void TRulerWin::SaveAs(RTMessage)
{
    OPENFILENAME ofn;
    DynStr szDirName;
       DynStr szFile;
    DynStr szFilter;
    strcpy(szFilter, "Файлы бланков(*.blk)|*.blk||");

	if(MyOpenFile(HWindow, ofn, szFilter, szDirName, szFile, "Сохранить бланк как", "\\blk", 0)) {
      Save(ofn.lpstrFile);
      Caption(ofn.lpstrFile);
      EnableMenuItem(GetSubMenu(GetMenu(HWindow), 0), 107, MF_BYCOMMAND | MF_ENABLED);
    }
}

void TRulerWin::Save(RTMessage)
{
    Save(BlankName);
}

void TRulerWin::NewFile(RTMessage)
{
    array.flush();
    Caption("noname.blk");
    InvalidateRect(HWindow, 0, 0);
    EnableMenuItem(GetSubMenu(GetMenu(HWindow), 0), 107, MF_BYCOMMAND | MF_GRAYED);
}

void TRulerWin::Caption(LPSTR n)
{
    DynStr c;
    strcpy(BlankName, AnsiUpper(n));
	//if(strrchr(n, '\\')) strcpy(n, strrchr(n, '\\') + 1);
    wsprintf(c, "Впечатывалка в бланки - %s", n);
    SetCaption(c);
}

void TRulerWin::Read(LPSTR name)
{
   array.flush();
   FILE* f = fopen(name, "rt");
   if(f)
    {
       int notread = 0;

       while(!feof(f))
        {
           O& o = *new O(0, 0, 0, 0);
           notread = o.Read(f);
           if(notread)
            {
               delete &o;
               break;
            }
	   array.add((Object&)o);
	}

       fclose(f);
       Caption(name);
       Numbers();
       InvalidateRect(HWindow, 0, 0);
    }
   else MessageBox(HWindow, name, "Файл не найден", MB_OK | MB_ICONSTOP);
}

void TRulerWin::Save(LPSTR name)
{
   FILE* f = fopen(name, "wt");
   if(f)
    {
       for(int i = 0; i < array.getItemsInContainer(); i++)
         if((O&)array[i] != NOOBJECT) ((O&)array[i]).Print(f);
       fclose(f);
    }
   else MessageBox(HWindow, "Ошибка записи", "Ошибка", MB_OK | MB_ICONSTOP);
}

void TRulerWin::Printing(HDC DC)
{
   for(int i = 0; i < array.getItemsInContainer(); i++)
     if((O&)array[i] != NOOBJECT) ((O&)array[i]).Draw(DC, 0);
}

#define _OPENBMP_

void TRulerWin::OpenBmp(RTMessage)
{
#ifdef _OPENBMP_
    OPENFILENAME ofn;
    DynStr szDirName;
    DynStr szFile;
    char  szFilter[128] = "Windows файлы(*.bmp)|*.bmp||";

    if(m_bmp) DeleteObject(m_bmp);
    m_bmp = 0;

    if(MyOpenFile(HWindow, ofn, szFilter, szDirName, szFile, "Открыть рисунок бланка", "\\bitmap")) {
       GetModule()->ExecDialog(new GetSizeBMP(this, ofn.lpstrFile, &BmpDX, &BmpDY));
       SetCursor(LoadCursor(0, IDC_WAIT));
       m_bmp = ReadBitMap(ofn.lpstrFile);
       if(!m_bmp) MessageBox(HWindow, "Не могу прочитать изображение", "Ошибка", MB_OK | MB_ICONSTOP);
    }
  InvalidateRect(HWindow, 0, 0);
#endif
}

void TRulerWin::PageSetup(RTMessage)
{
    GetModule()->ExecDialog(new Page(this));
}

void TRulerWin::GreenCard(RTMessage)
{
    SetCursor(LoadCursor(0, IDC_WAIT));
//    GetApplication()->MakeWindow(new DB1(this));
	GetApplication()->MakeWindow(new BELGREEN(this));
	SetCursor((HCURSOR)GetClassWord(HWindow, GCW_HCURSOR));
	LoadLastFile('G');
}

/*void TRulerWin::SOVAG(RTMessage)
{
//    SetCursor(LoadCursor(0, IDC_WAIT));
//    GetApplication()->MakeWindow(new ::SOVAG(this));
//    SetCursor((HCURSOR)GetClassWord(HWindow, GCW_HCURSOR));
}

void TRulerWin::BulStrad(RTMessage)
{
//    SetCursor(LoadCursor(0, IDC_WAIT));
//    GetApplication()->MakeWindow(new Bulstrad(this));
//    SetCursor((HCURSOR)GetClassWord(HWindow, GCW_HCURSOR));
}
  */
/*void TRulerWin::Warta(RTMessage)
{
    SetCursor(LoadCursor(0, IDC_WAIT));
    GetApplication()->MakeWindow(new DB2(this));
    SetCursor((HCURSOR)GetClassWord(HWindow, GCW_HCURSOR));
} */


void TRulerWin::Strax(RTMessage)
{
    SetCursor(LoadCursor(0, IDC_WAIT));
	GetApplication()->MakeWindow(new UNIVERSAL(this));
    SetCursor((HCURSOR)GetClassWord(HWindow, GCW_HCURSOR));
	LoadLastFile('U');
}

//void TRulerWin::Private(RTMessage)
//{
//    SetCursor(LoadCursor(0, IDC_WAIT));
//    GetApplication()->MakeWindow(new PrivateDlg(this));
//    SetCursor((HCURSOR)GetClassWord(HWindow, GCW_HCURSOR));
//}

/*void TRulerWin::Foreign(RTMessage)
{
//    SetCursor(LoadCursor(0, IDC_WAIT));
//    GetApplication()->MakeWindow(new Private3Dlg(this));
//    SetCursor((HCURSOR)GetClassWord(HWindow, GCW_HCURSOR));
}
  */
/*void TRulerWin::Russian(RTMessage)
{
    SetCursor(LoadCursor(0, IDC_WAIT));
    GetApplication()->MakeWindow(new ::Russian(this));
    SetCursor((HCURSOR)GetClassWord(HWindow, GCW_HCURSOR));
	LoadLastFile('R');
} */
 /*
void TRulerWin::Litva(RTMessage)
{
//    SetCursor(LoadCursor(0, IDC_WAIT));
//    GetApplication()->MakeWindow(new LITVA(this));
//    SetCursor((HCURSOR)GetClassWord(HWindow, GCW_HCURSOR));
}

void TRulerWin::Private2(RTMessage)
{
//    SetCursor(LoadCursor(0, IDC_WAIT));
//    GetApplication()->MakeWindow(new Private2Dlg(this));
//    SetCursor((HCURSOR)GetClassWord(HWindow, GCW_HCURSOR));
}
   */
void TRulerWin::Private2005(RTMessage)
{
    SetCursor(LoadCursor(0, IDC_WAIT));
	GetApplication()->MakeWindow(new Private2005Dlg(this));
	SetCursor((HCURSOR)GetClassWord(HWindow, GCW_HCURSOR));
	LoadLastFile('P');
}

void TRulerWin::MandatoryInsur(RTMessage)
{
    SetCursor(LoadCursor(0, IDC_WAIT));
    GetApplication()->MakeWindow(new MandatoryDlg(this));
    SetCursor((HCURSOR)GetClassWord(HWindow, GCW_HCURSOR));
    LoadLastFile('M');
}

void TRulerWin::Komplex(RTMessage)
{
    SetCursor(LoadCursor(0, IDC_WAIT));
    GetApplication()->MakeWindow(new MandatoryDlg(this, 1));
    SetCursor((HCURSOR)GetClassWord(HWindow, GCW_HCURSOR));
    LoadLastFile('K');
}

void TRulerWin::LoadLastFile(char Ltr)
{
	LastInsType[0] = Ltr;
	DynStr fullName;
	char Key[10];
	sprintf(Key, "Name%s", Ltr != '\0' ? LastInsType : "");
	if((GetPrivateProfileString("LastTmpl", Key, "", fullName, fullName._sizeof(), ::ININame) /*||
		GetPrivateProfileString("LastTmpl", "Name", "", fullName, fullName._sizeof(), ::ININame)*/)
		&& access(fullName, 0) == 0)
    {
		Read(fullName);
		strcpy(CurrentBlkFile, fullName);
		EnableMenuItem(GetSubMenu(GetMenu(HWindow), 0), 107, MF_BYCOMMAND | MF_ENABLED);
		if(Ltr != '\0')
        {
			//WritePrivateProfileString("LastTmpl", "Name", fullName, ::ININame);
			WritePrivateProfileString("LastTmpl", Key, fullName, ::ININame);
		}
	}
}

void TRulerWin::Clear()
{
    for(int i = 0; i < array.getItemsInContainer(); i++)
      if((O&)array[i] != NOOBJECT) ((O&)array[i]).SetText("");
}

void TRulerWin::RepRuss(RTMessage)
{
	RunReporter32("RUSSMAND");
}

void TRulerWin::Numbers()
{
    char Number[4];

    for(int i = 0; i < array.getItemsInContainer(); i++)
      if((O&)array[i] != NOOBJECT)
        {
          sprintf(Number, "%d", i+1);
          ((O&)array[i]).SetText(Number);
        }
}
/*
void TRulerWin::CMLanguage(RTMessage Msg)
{
    HMENU menu = GetMenu(GetApplicationObject()->MainWindow->HWindow);
    int Language = GetMenuState(menu, 112, MF_BYCOMMAND) == MF_UNCHECKED ? MF_CHECKED : MF_UNCHECKED;
    char menuText[100];
    LoadString(GetApplicationObject()->hInstance, Language == MF_UNCHECKED ? 200 : 201, menuText, sizeof menuText);
    ModifyMenu(menu, 112, MF_BYCOMMAND | MF_STRING | Language, 112, menuText);
}
*/
void TRulerWin::SetValues(char* buff)
{
    char* ch = buff;

    for(int i = 0; i < array.getItemsInContainer(); i++)
      if((O&)array[i] != NOOBJECT)
        {
          ((O&)array[i]).SetText(ch);
          ch += strlen(ch) + 1;
        }
   InvalidateRect(HWindow, 0, 0);
}
// Print page (or pages)


class TRulerApp : public TApplication
{
public:
        TRulerApp(LPSTR AName, HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) :
        TApplication(AName, hInstance, hPrevInstance, lpCmdLine, nCmdShow) {}
        ~TRulerApp();

  virtual void InitMainWindow(void);
  virtual void InitInstance();
  virtual BOOL CanClose();
};

BOOL TRulerApp::CanClose()
 {
   WinHelp(MainWindow->HWindow, "blank.hlp", HELP_QUIT, 0);
   return TRUE;
 }


void TRulerApp::InitInstance()
{
   TApplication::InitInstance();
   HAccTable = LoadAccelerators(hInstance, "ACCEL");
}

void TRulerApp::InitMainWindow(void)
{
    MainWindow = new TRulerWin(NULL, "");
}


PTModule Ctl3dModule;

TRulerApp::~TRulerApp()
{
//   if(Ctl3dModule)
//     {
//	FreeLibrary(Ctl3dModule->hInstance);
//	delete Ctl3dModule;
//	Ctl3dModule = 0;
//     }
}

void CreateBackup()
{
	if(IsBackup<=0) return;
	DynStr DbPath;
	DynStr ArcPath;
	DynStr DbPath2;
	DynStr AppPath;
	struct date dt;
	long lastdays;
	getdate(&dt);
	GetProfileString("Paradox Engine", "BlankBackup", "", DbPath, DbPath._sizeof());
	lastdays = atol(DbPath);
	if(dt.da_day + dt.da_mon * 30L + dt.da_year * 365L - lastdays >= IsBackup) {
		if(!GetProfileString("Paradox Engine", "NetNamePath", "", DbPath, DbPath._sizeof())) return;
		if(DbPath[strlen(DbPath) - 1] != '\\') strcat(DbPath, "\\");
		strcpy(AppPath, DbPath);
		strcpy(ArcPath, DbPath);
		sprintf(AppPath + strlen(AppPath), "zip.exe", (int)dt.da_day, (int)dt.da_mon, (int)dt.da_year);
		strcat(ArcPath, "DBCopy");
		mkdir(ArcPath);
		sprintf(ArcPath + strlen(ArcPath), "\\%02d%02d%d.zip", (int)dt.da_day, (int)dt.da_mon, (int)dt.da_year);

		if(access(ArcPath, 0) != 0) { //Файла архива нет
			//strcat(DbPath2, " -r -p -ex ");
			//strcat(DbPath2, " -max -add -rec -dir=current ");
			strcat(DbPath2, " -max -add -dir=current ");
			strcat(DbPath2, ArcPath);
			strcat(DbPath2, " ");
			strcat(DbPath2, DbPath);
			strcat(DbPath2, "*.db ");
			strcat(DbPath2, DbPath);
			strcat(DbPath2, "*.px ");

			HRSRC resId = FindResource(GetApplicationObject()->hInstance, MAKEINTRESOURCE(101), "Packer");
			if(!resId) return;
			HGLOBAL memId = LoadResource(GetApplicationObject()->hInstance, resId);
			void FAR* resPtr = LockResource(memId);
			FILE* f = fopen(AppPath, "wb");

			fwrite(resPtr, 6, GlobalSize(memId) / 6, f);
			//(char*)resPtr += int(GlobalSize(memId) / 32000) * 32000L;
			//fwrite(resPtr, 1, GlobalSize(memId) % 32000, f);

			fclose(f);
			UnlockResource(resId);
			FreeResource(memId);
			//MessageBox(0, AppPath, "Внимание", MB_ICONINFORMATION);
			//MessageBox(0, DbPath, "Внимание", MB_ICONINFORMATION);
			//MessageBox(0, DbPath2, "Внимание", MB_ICONINFORMATION);
			ShellExecute(0, "open", AppPath, DbPath2, DbPath, SW_MINIMIZE);
			MessageBox(0, "Было запущено резервное копирование", "Внимание", MB_ICONINFORMATION);
		}
		sprintf(DbPath, "%lu", dt.da_day + dt.da_mon * 30L + dt.da_year * 365L);
		WriteProfileString("Paradox Engine", "BlankBackup", DbPath);
	}

}

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int)
{
   ActivateParadox = strcmpi(lpCmdLine, "CALC");

  ::ININame = new char[64];

  if(GetProfileInt("Paradox Engine", "SwapSize", 0) < 48)
  {
	WriteProfileString("Paradox Engine", "SwapSize", "64");
  }

  if(GetProfileString("Paradox Engine", "NetNamePath", "", ::ININame, 64))
  {
	if(::ININame[strlen(::ININame) - 1] != '\\') strcat(::ININame, "\\");
	strcat(::ININame, "blank.ini");
	if(access(::ININame, 0) != 0) strcpy(::ININame, "blank.ini");
  }

  DynStr s;
  int PassOk = TRUE;
  if(GetPrivateProfileString("APP", "PASS", "", s, s._sizeof(), ::ININame)) {
	if(!(GetAsyncKeyState(VK_CONTROL) & 0x8000))
		PassOk = FALSE;

	for(int i = 0; s[i]; i++)
		if(!(GetAsyncKeyState(s[i]) & 0x8000))
			PassOk = FALSE;
  }
  if(!PassOk) {
	MessageBox(0, "Нет доступа к программе", "STOP", MB_OK | MB_ICONSTOP);
	return -1;
  }


  int Ctl3dOn = 0;

  TApplication* RulerApp = new TRulerApp("Ruler", hInstance, hPrevInstance, lpCmdLine, SW_SHOWMAXIMIZED);

  SetErrorMode(SEM_NOOPENFILEERRORBOX);
  Ctl3dModule = new TModule("CTL3DV2.DLL", LoadLibrary("CTL3DV2.DLL"), "");

  BOOL FAR PASCAL(*ctl3dRegister)(HANDLE);
  (FARPROC)ctl3dRegister = GetProcAddress(Ctl3dModule->hInstance, "Ctl3dRegister");

  if (ctl3dRegister)
    {
       ctl3dRegister(hInstance);
       BOOL FAR PASCAL(*ctl3dAutoSubclass)(HANDLE);

      (FARPROC)ctl3dAutoSubclass = GetProcAddress(Ctl3dModule->hInstance, "Ctl3dAutoSubclass");
      if (ctl3dAutoSubclass)
	{
	  ctl3dAutoSubclass(hInstance);
	  Ctl3dOn = 1;
	}
    }

  if(!Ctl3dOn)
    {
      FreeLibrary(Ctl3dModule->hInstance);
      delete Ctl3dModule;
      Ctl3dModule = 0;
    }


  int Err = PXWinInit("Blank", PXSHARED);
  if(Err == PXSUCCESS)
   {
	 RulerApp->Run();
	 //if(ActivateParadox)
		 if(Err = PXExit())
			 MessageBox(0, PXErrMsg(Err), "Ошибка Paradox Engine (PxExit)", MB_OK | MB_SYSTEMMODAL);
   }
  else MessageBox(0, PXErrMsg(Err), "Ошибка Paradox Engine (PXWinInit)", MB_OK | MB_SYSTEMMODAL);

  DeleteObject(White);
  DeleteObject(Gray);

  if(Ctl3dModule)
    {
      BOOL FAR PASCAL(*ctl3dUnRegister)(HANDLE);
     (FARPROC)ctl3dUnRegister = GetProcAddress(Ctl3dModule->hInstance, "Ctl3dUnregister");
      if(ctl3dUnRegister) ctl3dUnRegister(hInstance);
      FreeLibrary(Ctl3dModule->hInstance);
      delete Ctl3dModule;
      Ctl3dModule = 0;
    }

  delete RulerApp;
  return 1;
}

HBRUSH Gray, White;
////////////////////////////////////////////////////////////////////////////

void TRulerWin::ReportGreen(RTMessage)
{
	RunReporter32("BELGREEN");
}

void TRulerWin::ReportKomplex(RTMessage)
{
	RunReporter32("KOMPLEX");
}
 
int CreatePkIndex(char* TableName, int Count)
{
	FIELDHANDLE fld[10];
	for(int i = 0; i < Count; i++)
		fld[i] = i;

	PXKeyDrop(TableName, 0);
	int Result = PXKeyAdd(TableName, Count, fld, PRIMARY);
	if(Result != PXSUCCESS)
		MessageBox(0, TableName, "Первичный Индекс не создан!!!", MB_OK | MB_ICONSTOP);
	return 1;
}

void TRulerWin::RebuildIndexMand(RTMessage)
{
	if (MessageBox(HWindow, "Вы сделали копии данных и запомнили количество полисов?", "Ответь", MB_YESNO | MB_ICONQUESTION) == IDNO)
        return;

	SetCursor(LoadCursor(0, IDC_WAIT));

	DynStr TableName;
	GetProfileString("Paradox Engine", "NetNamePath", "", TableName, TableName._sizeof());
	if(TableName[strlen(TableName) - 1] != '\\')
		strcat(TableName, "\\");
	strcat(TableName, "MANDATOR");
	for(int i = 1; i < 30; i++)	PXKeyDrop(TableName, 1);
	CreatePkIndex(TableName, 2);
	strcpy(strrchr(TableName, '\\') + 1, "MANDAV2");
	CreatePkIndex(TableName, 3);
	strcpy(strrchr(TableName, '\\') + 1, "MANDOWN");
	CreatePkIndex(TableName, 3);
	strcpy(strrchr(TableName, '\\') + 1, "MAND_TXT");
	CreatePkIndex(TableName, 3);
 	strcpy(strrchr(TableName, '\\') + 1, "AVPAYS");
	CreatePkIndex(TableName, 7);

	MessageBox(HWindow, "Программа поработала", "Всё", MB_OK | MB_ICONINFORMATION);
}

int Executor(char* Module, char* What, int AddLockalPath)
{
	char szModuleName[128];

	SetCursor(LoadCursor(0, IDC_WAIT));
	if(AddLockalPath) {
			GetModuleFileName(GetApplicationObject()->hInstance, szModuleName, sizeof(szModuleName)-15);
        	*strrchr(szModuleName, '\\') = 0;
		strcat(szModuleName, "\\");
	}
	else {
		*szModuleName = 0;
        }
	strcat(szModuleName, Module);
	int runResult = (int)ShellExecute(GetApplicationObject()->MainWindow->HWindow, 0, szModuleName, What, 0, SW_SHOWNORMAL);
	   if(runResult < 32) {
		if(strlen(Module) > 8 + 4)
		{
			strcpy(szModuleName, Module);
			memmove(szModuleName + 4, szModuleName + (strlen(Module) - 4 - 4), strlen(Module));
			return Executor(szModuleName, What, AddLockalPath);
		}
		else
		{
			DynStr buff;
			sprintf(buff, GetRCStr(544), szModuleName, runResult);
			MessageBox(GetApplicationObject()->MainWindow->HWindow, buff, "Ошибка", MB_OK | MB_ICONINFORMATION);
		}
	    }
	return runResult;
}

void TRulerWin::RunReporter32(char* What)
{
    Executor("REPORTER32.EXE", What, 1);
}

void TRulerWin::StartAutoMarka(RTMessage)
{
	Executor("INIEditorPrj.exe", "16", 1);
}
 /*
void TRulerWin::RunReporter(char* What)
{
	Executor("REPORTER.EXE", What);
}

void TRulerWin::Poland(RTMessage)
{
    RunReporter32("POLAND");
}

void TRulerWin::BulstradRpt(RTMessage)
{
    RunReporter32("BULSTRAD");
}
   */
void TRulerWin::Leben2005(RTMessage)
{
	RunReporter32("PRIV2005");
}
 /*
void TRulerWin::Leben2(RTMessage)
{
    RunReporter32("LEBEN2");
}
     /*
void TRulerWin::ReportSovag(RTMessage)
{
	RunReporter32("SOVAG");
}

void TRulerWin::ForeignRpt(RTMessage)
{
    RunReporter32("FOREIGN");
}
       */
void TRulerWin::ReportMandatory(RTMessage)
{
    RunReporter32("");
}

void TRulerWin::ExportUnivers(RTMessage)
{
	 UNIVERSAL::Export(this);
}

void TRulerWin::ImportUnivers(RTMessage)
{
	 UNIVERSAL::Import();
}

void TRulerWin::Settings(RTMessage)
{
     char Value[5];
     SettingsBuffer buffer;
     SettingsDlg* dlg = new SettingsDlg(this, &buffer);
     dlg->IndexLanguage = Language;
     buffer.Is2000 = Is2000Full;
	 buffer.bIsValidateImport = GetPrivateProfileInt("DIVISION", "ValidateImport", 0, ININame);
//	 strcpy(buffer.Text, TextTo2RP);
	 GetPrivateProfileString("MANDATORY", "RublMin", "10", buffer.Round, sizeof buffer.Round, ININame);
	 buffer.IsBackup = IsBackup;
	 wsprintf(buffer.BkPeriod, "%d", IsBackup);

	 if(GetModule()->ExecDialog(dlg) == IDOK) {
	  Language = dlg->IndexLanguage;
	  Is2000Full = buffer.Is2000;
	  IsBackup = buffer.IsBackup;
//	  strcpy(TextTo2RP, buffer.Text);
//	  WritePrivateProfileString("2РП", "Date2IsEmpty", TextTo2RP, ININame);
	  wsprintf(Value, "%d", Is2000Full);
	  WritePrivateProfileString("2000 Год", "IsFull", Value, ININame);
	  wsprintf(Value, "%d", buffer.bIsValidateImport);
	  WritePrivateProfileString("DIVISION", "ValidateImport", Value, ININame);
	  //wsprintf(Value, "%d", bIsMandMonth);
	  //WritePrivateProfileString("MANDATORY", "Месяцы", Value, ININame);
	  wsprintf(Value, "%d", Language);
	  WritePrivateProfileString("Язык", "Номер", Value, ININame);
	  wsprintf(Value, "%d", MonthRegister);
	  WritePrivateProfileString("Месяцы", "Регистр", Value, ININame);
	  WritePrivateProfileString("MANDATORY", "RublMin", buffer.Round, ININame);
	  WritePrivateProfileString("BACKUP", "Period", buffer.IsBackup ? buffer.BkPeriod : "0", ININame);
	 }
}

void TRulerWin::RestoreMandator(RTMessage)
{
     MandatoryDlg::Restore(0);
}

void TRulerWin::RestoreMandatorFromBk(RTMessage)
{
//     MandatoryDlg::Restore2(0);
}

void TRulerWin::EndFind(RTMessage msg)
{
     if(msg_reciever)
	PostMessage(msg_reciever, msg.Message, msg.WParam, msg.LParam);
}


void TRulerWin::ExportKomplex(RTMessage Msg)
{
	 MandatoryDlg::Export(1);
}

void TRulerWin::ImportKomplex(RTMessage Msg)
{
	 MandatoryDlg::Import(1);
}

void TRulerWin::ExportMand(RTMessage)
{
     MandatoryDlg::Export(0);
}

void TRulerWin::ImportMand(RTMessage)
{
     MandatoryDlg::Import(0);
}

/*void TRulerWin::PrintLeben2006(RTMessage)
{
	 HMENU menu = GetMenu(HWindow);
	 menu = GetSubMenu(menu, 1);
     int State = GetMenuState(menu, 184, MF_BYCOMMAND);
     CheckMenuItem(menu, 184, MF_BYCOMMAND | ((State & MF_CHECKED) ? MF_UNCHECKED : MF_CHECKED));
	 WritePrivateProfileString("PRIV2005", "Print", (State & MF_CHECKED) ? "2005" : "2006", ININame);
}

/*
void TRulerWin::OnServer(RTMessage Msg)
{
     HMENU menu = GetMenu(HWindow);
     menu = GetSubMenu(menu, 4);
     int State = GetMenuState(menu, 970, MF_BYCOMMAND);
     CheckMenuItem(menu, 970, MF_BYCOMMAND | ((State & MF_CHECKED) ? MF_UNCHECKED : MF_CHECKED));
     WritePrivateProfileString("MANDATORY", "FindMode", (State & MF_CHECKED) ? "SELF" : "ONSERVER", ININame);
}

void TRulerWin::FromFirst(RTMessage Msg)
{
     HMENU menu = GetMenu(HWindow);
     menu = GetSubMenu(menu, 4);
     int State = GetMenuState(menu, 971, MF_BYCOMMAND);
     CheckMenuItem(menu, 971, MF_BYCOMMAND | ((State & MF_CHECKED) ? MF_UNCHECKED : MF_CHECKED));
     WritePrivateProfileString("MANDATORY", "FindModeParam", (State & MF_CHECKED) ? "FULLSEARCH" : 0, ININame);
}
*/
void TRulerWin::FIZFACE(RTMessage)
{
	DynStr szModuleName;
	GetModuleFileName(GetApplicationObject()->hInstance, szModuleName, szModuleName._sizeof()-32);
	strcpy(strrchr(szModuleName, '\\') + 1, "FizFace.txt");
	MessageBox(HWindow, GetRCStr(617), "Сообщаю", MB_ICONINFORMATION);
	Executor("notepad.exe", szModuleName, 0);
}

void TRulerWin::URFACE(RTMessage)
{
	DynStr szModuleName;
	GetModuleFileName(GetApplicationObject()->hInstance, szModuleName, szModuleName._sizeof()-32);
	strcpy(strrchr(szModuleName, '\\') + 1, "UrFace.txt");
	MessageBox(HWindow, GetRCStr(617), "Сообщаю", MB_ICONINFORMATION);
	Executor("notepad.exe", szModuleName, 0);
}

void TRulerWin::ImportAlvena(RTMessage)
{
	RunReporter32("ALVENA");
}

void TRulerWin::ReportUnivers(RTMessage)
{
	RunReporter32("UNIVERS");
}

void TRulerWin::Agents(RTMessage)
{
	RunReporter32("AGENTS");
}

void TRulerWin::TOOL(RTMessage)
{
	RunReporter32("TOOL");
}

/*extern char* MandatoryFields[];
extern char* MandatoryName;
extern unsigned char MandatoryCnt;

void TRulerWin::ExportAllNmbrs(RTMessage)
{
	OPENFILENAME ofn;
    DynStr szDirName;
    DynStr szFile;
    DynStr szFilter;
	strcpy(szFilter, "Файлы номеров(*.nmb)|*.nmb||");
	DATE RegDt;

	if(MyOpenFile(HWindow, ofn, szFilter, szDirName, szFile, "Экспорт", "", 0)) {
	  FILE* f = fopen(ofn.lpstrFile, "wb");
	  if(f) {
		  TOpenBase agents(AgentDBName, AgentFields, AgFieldsCount);
		  if(agents.pxErr != 0) {
			  fclose(f);
              return;
		  }
          agents.Get();
		  fprintf(f, "%s %s\n", agents.GetString(0).s, agents.GetString(1).s);

		  TOpenBase base(MandatoryName, MandatoryFields, MandatoryCount);
		  if(base.pxErr == 0) {
			  long AllRecords = base.GetRecords();
			  SetCursor(LoadCursor(0, IDC_WAIT));
			  for(long i = 0; i < AllRecords; i++) {
				  base.Get();
				  base(RegDt, 2);


				  int m, d, y;
				  PXDateDecode(RegDt, &m, &d, &y);
				  if(y >= 2004) {
					  fprintf(f, "%s,%ld\n", base.GetString(0).s, (long)base.GetDouble(1));
                  }

				  base.Next();
              }
		  }
          {
		  TOpenBase base("\\mandav2", Avaria2Fields, 40);

		  if(base.pxErr == 0) {
			  long AllRecords = base.GetRecords();
			  SetCursor(LoadCursor(0, IDC_WAIT));
			  for(long i = 0; i < AllRecords; i++) {
				  base.Get();
				  base(RegDt, 3); //avdate


				  int m, d, y;
				  PXDateDecode(RegDt, &m, &d, &y);
				  if(y >= 2004) {
					  fprintf(f, "%s,%ld\n", base.GetString(0).s, (long)base.GetDouble(1));
                  }
                  else {
				  	base(RegDt, 37); //update
 				  	PXDateDecode(RegDt, &m, &d, &y);
				  	if(y >= 2004) {
					  fprintf(f, "%s,%ld\n", base.GetString(0).s, (long)base.GetDouble(1));
					}
                  }
				  base.Next();
              }
		  }
          }
		  fclose(f);
      }
	}   
}
*/
void TRulerWin::ExportBelGreen(RTMessage)
{
	BELGREEN::Export(this);
}

void TRulerWin::ImportBelGreen(RTMessage)
{
	BELGREEN::Import();
}
 /*
void TRulerWin::ExportRuss(RTMessage)
{
	Russian::Export(this);
}

void TRulerWin::ImportRuss(RTMessage)
{
	Russian::Import();
}

/*void TRulerWin::ExportPrivate2(RTMessage)
{
	Private2Dlg::Export(this);
}

void TRulerWin::ImportPrivate2(RTMessage)
{
	Private2Dlg::Import();
}
*/
void TRulerWin::ExportPrivate2005(RTMessage)
{
	Private2005Dlg::Export(this);
}
void TRulerWin::ImportPrivate2005(RTMessage)
{
	Private2005Dlg::Import();
}
/*
void TRulerWin::MandNewCalc(RTMessage)
{
   int NewCalc = GetPrivateProfileInt("MANDATORY", "NewCalculate", 1, ININame);
   NewCalc = !NewCalc;
   HMENU menu = GetMenu(HWindow);
   menu = GetSubMenu(menu, 1);
   CheckMenuItem(menu, 137, MF_BYCOMMAND | (NewCalc ? MF_CHECKED : MF_UNCHECKED));
   char s[3];
   wsprintf(s, "%d", NewCalc);
   WritePrivateProfileString("MANDATORY", "NewCalculate", s, ININame);
}
*/
void TRulerWin::Rates(RTMessage Msg)
{
	RunReporter32("RATES");
}

void TRulerWin::PrintAddr(RTMessage)
{
   int NewCalc = GetPrivateProfileInt("MANDATORY", "FillAddr", 0, ININame);
   NewCalc = !NewCalc;
   HMENU menu = GetMenu(HWindow);
   menu = GetSubMenu(menu, 3);
   CheckMenuItem(menu, 144, MF_BYCOMMAND | (NewCalc ? MF_CHECKED : MF_UNCHECKED));
   char s[3];
   wsprintf(s, "%d", NewCalc);
   WritePrivateProfileString("MANDATORY", "FillAddr", s, ININame);
}

/*
void TRulerWin::MandOldPrint(RTMessage)
{
   bPrintOld = !bPrintOld;
   HMENU menu = GetMenu(HWindow);
   menu = GetSubMenu(menu, 1);
   CheckMenuItem(menu, 143, MF_BYCOMMAND | (bPrintOld ? MF_CHECKED : MF_UNCHECKED));
}
*/
void TRulerWin::BlankIni(RTMessage Msg)
{
	//Executor("NOTEPAD.EXE", ::ININame, 0);
}

void TRulerWin::CMRange(RTMessage Msg)
{
	RunReporter32("RANGE");
}

void TRulerWin::SysCommand(RTMessage msg)
{
	if(msg.WParam == SC_CLOSE)
	{
		if(ActiveForm)
		{
			EndDialog(ActiveForm->HWindow, 1);
			PostQuitMessage(0);
            return;
        }
	}
	DefWndProc(msg);
}

void TRulerWin::WMSetXRange(RTMessage Msg)
{
	GetApplicationObject()->MainWindow->GetModule()->ExecDialog(new TSetRange(GetApplicationObject()->MainWindow));
}
