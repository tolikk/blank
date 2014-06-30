#include <owl.h>
#include <static.h>
#include <dialog.h>
#include <string.h>
#include <edit.h>
#include <stdio.h>
#include <array.h>
#include <window.h>
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
#include "util.h"
#include "math.h"
#include "GRNCARD.h"
#include "combobox.h"
#include "io.h"
#include "impexp.h"
#include "setrange.h"
#include "dateedit.h"
#include "checkbox.h"
#include "RADIOBUT.h"
#include "editors.h"

#undef NDEBUG
#include "assert.h"

#define max(a, b) ((a)>(b)?(a):(b))

int _IsValidPolisNumber(const char* SECTION, _str s, long n, char* INI, char* Agent);
void SetAgent(char* ACode, TOpenBase& agents, PTComboBox AgName, int);
extern char* AVPayFields[];
extern char* AVPayTableName;
extern const char* MANDATORY_SECT;
void DateFBToStr(TOpenBase* base, int field, char* buffer);
void divDate(char* b);
void AddBufDate(char*&dest, char* date, int flag = 0);
extern char* Taxi_Fields[];
void DblToStr(char* str, double V, int Digit = 2);
extern char ValidateInternalImport;
char* GetCheckErrorText(int Err)
{
	static char* errBuffer = new char[128];
	sprintf(errBuffer, "%d", Err);
	GetPrivateProfileString(MANDATORY_SECT, errBuffer, errBuffer, errBuffer, 1020, "checkerr.ini");
	return errBuffer;
}

void AlignCombo(TComboBox* Combo);
BOOL MyOpenFile(HWND HWindow, OPENFILENAME& ofn, char* szFilter, char* szDirName, char* szFile, char* Title, char* ext, int Load = 1);
extern int ModifyMandatory;
extern DATE GD(char* str, int = 0);
const char* GetRCStr(int strID);
int IsValidAvariaRecord(TOpenBase& base);
//int IsValidOwnerRecord(TOpenBase& base);
int SaveAgents(Array& AgentsCodes, FILE* f, TOpenBase& AGTable, Array* list_errors, TOpenBase& base, int AgField);

class ShowNumbersDlg : public TDialog {
	Array* listNumbers;
	long countPolises;

public:
	ShowNumbersDlg(PTWindowsObject A, Array*, long countPolises);
    void SetupWindow();
    void Save(RTMessage) = [ID_FIRST + 102];
};

struct PetitionData
{
	PetitionData()
    {
		memset(this, 0, sizeof(*this));
		PrintNumbers = 1;
    }
	char OwnDoc[128];
	char Discount[128];

	int PrintNumbers;

	char SN[32];
	char From[11], To[11], Avar[11];
	char Company[64];

	int before25;
	int after25;

	int before2;
	int after2;
	int noDriveLic;
};

class PetitionDlg : public TDialog {
	
	PetitionData* data;
	TComboBox* Company;
	TRadioButton* r1;
	TRadioButton* r2;
	TRadioButton* r3;
	TRadioButton* r4;
	TRadioButton* r5;
public:
	PetitionDlg(PTWindowsObject A, PetitionData* data);
	void SetupWindow();
    void Ok(RTMessage);
};

PetitionDlg::PetitionDlg(PTWindowsObject A, PetitionData* adata)
		  : TDialog(A, "PETITION"), data(adata)
{
    new TEdit(this, 100, 128);
	new TEdit(this, 101, 128);
	(Company = new TComboBox(this, 102, 20))->DisableTransfer();
	new TCheckBox(this, 107, 0);
	new TEdit(this, 103, 32);
	new TDateEdit(this, 104, 11);
	new TDateEdit(this, 105, 11);
	new TDateEdit(this, 106, 64);

	r1 = new TRadioButton(this, 108, 0);
	r2 = new TRadioButton(this, 109, 0);
	r3 = new TRadioButton(this, 110, 0);
	r4 = new TRadioButton(this, 111, 0);
	r5 = new TRadioButton(this, 112, 0);

	TransferBuffer = data;

	GetPrivateProfileString(MANDATORY_SECT, "OwnDoc", "", data->OwnDoc, sizeof data->OwnDoc, ININame);
	GetPrivateProfileString(MANDATORY_SECT, "Discount", "", data->Discount, sizeof data->Discount, ININame);
}

void PetitionDlg::SetupWindow()
{
	TDialog::SetupWindow();
	for(int i = 1;; i++)
	{
		char buff[48];
		sprintf(buff, "BuroCompanies%d", i);
		if(!GetPrivateProfileString(MANDATORY_SECT, buff, "", buff, sizeof buff, ::ININame)) break;
        char* bad = 0;
		strtol(buff, &bad, 10);
		Company->AddString(bad ? bad : buff);
	}
}

void PetitionDlg::Ok(RTMessage msg)
{
	TransferData(TF_GETDATA);

	if(*data->From && !GD(data->From))
    {
        SetFocus(ChildWithId(104)->HWindow);	
		return;
	}

	if(*data->To && !GD(data->To))
    {
		SetFocus(ChildWithId(105)->HWindow);
		return;
    }

	if(*data->Avar && !GD(data->Avar))
    {
		SetFocus(ChildWithId(106)->HWindow);
		return;
    }

	if(*data->SN && strlen(data->SN) < 7 && !strchr(data->SN, '/'))
    {
		SetFocus(ChildWithId(103)->HWindow);
		return;
	}

	Company->GetText(data->Company, sizeof data->Company);

	data->before25 = r1->GetCheck();
	data->after25 = r2->GetCheck();
	data->before2 = r3->GetCheck();
	data->after2 = r4->GetCheck();
	data->noDriveLic = r5->GetCheck();

	WritePrivateProfileString(MANDATORY_SECT, "OwnDoc", data->OwnDoc, ININame);
	WritePrivateProfileString(MANDATORY_SECT, "Discount", data->Discount, ININame);

	TDialog::Ok(msg);
}

#ifdef _BLANK_TRACE
void TRACE(char* text)
{
    DynStr szModuleName;
	GetModuleFileName(GetApplicationObject()->hInstance, szModuleName, szModuleName._sizeof()-32);
	*strrchr(szModuleName, '\\') = 0;
    strcat(szModuleName, "\\IMPTRACE.TXT");
	FILE* f = fopen(szModuleName, text ? "a+" : "wt");
	if(f) {
	   fputs(text, f);
	   fputs("\n", f);
	   flushall();
	   fclose(f);
    }
}

void TRACE(long Val)
{
    char ValStr[16];
    sprintf(ValStr, "%X", Val);
    TRACE(ValStr);
}
#else
#define TRACE(Params) ;
#endif

ShowNumbersDlg::ShowNumbersDlg(PTWindowsObject A, Array* ln, long acountPolises)
		  : TDialog(A, "POLIS_NMBS"),
			countPolises(acountPolises)
{
    listNumbers = ln;
    new TListBox(this, 101);
}

void ShowNumbersDlg::Save(RTMessage)
{
	DynStr szDirName;
	strcpy(szDirName, "C:\\");
    char  l_szFilter[] = "Текcт(*.txt)|*.txt||";
    DynStr filename;
    OPENFILENAME ofn;
    if(!MyOpenFile(HWindow, ofn, l_szFilter, szDirName, filename, "Сохранить номера", "", 0)) return;
    FILE* f = fopen(filename, "wt");
    if(!f) {
	MessageBox(HWindow, GetRCStr(529), "Сообщаю", MB_OK);
	return;
    }
    for(int i = 0; i < listNumbers->getItemsInContainer(); i++) {
	fprintf(f, "%s\n", (const char*)(String&)(*listNumbers)[i]);
    }
    fclose(f);
}

void ShowNumbersDlg::SetupWindow()
{
    TDialog::SetupWindow();
    TListBox* lb = (TListBox*)ChildWithId(101);

    char s[48];
    sprintf(s, "Количество полисов %lu", countPolises);
    lb->AddString(s);
    for(int i = 0; i < listNumbers->getItemsInContainer(); i++) {
	if(lb->AddString((char*)(const char*)(String&)(*listNumbers)[i]) == -1) {
	    //MessageBeep(0);
            for(int n = 0; n < 100; n++)
		lb->DeleteString(lb->GetCount() - 1);
	    lb->InsertString("В списке не всё", 0);
	    break;
        }
    }
}

LinkAgDlg::LinkAgDlg(PTWindowsObject A)
	 : TDialog(A, "LINK_AGENTS")
{
	m_lb = new TListBox(this, 101);
	m_combo = new TListBox(this, 105);
	m_cb = new TCheckBox(this, 106, 0);
	pIgnoreNames = 0;
	Msg = 0;
}

void LinkAgDlg::ChLb(RTMessage msg)
{
	if(msg.LP.Hi == CBN_SELCHANGE)
	{
		ChLbIndex();
    }
}

void LinkAgDlg::SetByCode(RTMessage msg)
{
	NewAgCodesPtr->flush();
	for(int i = 0; i < AgCodesPtr->getItemsInContainer(); i++)
	{
		NewAgCodesPtr->add(*new String((const char*)(String&)(*AgCodesPtr)[i]));
		if(i == SendMessage(m_lb->HWindow, CB_GETCURSEL, 0, 0))
		{
			SetComboCode((char*)(const char*)(String&)(*AgCodesPtr)[i]);
    	}
	}
}

void LinkAgDlg::SetComboCode(char*)
{
	for(int i = 0; i < m_combo->GetCount(); i++)
	{
		long CodeData = SendMessage(m_combo->HWindow, LB_GETITEMDATA, i, 0);
		char buff[10];
		memcpy(buff, &CodeData, 4);
		buff[4] = 0;
		if(strcmp(buff, (String&)(*NewAgCodesPtr)[PrevIndex]) == 0)
		{
			SendMessage(m_combo->HWindow, LB_SETCURSEL, i, 0);
		}
    }
}

void LinkAgDlg::ChLbIndex()
{
    //Change old Code
	char NewCode[10] = "";
	int myAgentIdx = (int)SendMessage(m_combo->HWindow, LB_GETCURSEL, 0, 0);
	if(myAgentIdx != -1)
	{
		long CodeData = SendMessage(m_combo->HWindow, LB_GETITEMDATA, myAgentIdx, 0);
		memcpy(NewCode, &CodeData, 4);
		NewCode[4] = 0;
	}

	NewAgCodesPtr->addAt(*new String(NewCode), PrevIndex);

	PrevIndex = m_lb->GetSelIndex();
    
	SendMessage(m_combo->HWindow, LB_SETCURSEL, -1, 0);

	SetComboCode(NewCode);

}

void LinkAgDlg::SetupWindow()
{
    TDialog::SetupWindow();
    m_combo->SetSelIndex(3);

	if(pIgnoreNames == 0) EnableWindow(m_cb->HWindow, 0);

    SetDlgItemText(HWindow, 555, Msg);

	char Tmp[80];

	for(int i = 0; i < AgNamesPtr->getItemsInContainer(); i++) {
		sprintf(Tmp, "%s %s", (char*)(const char*)((String&)(*AgCodesPtr)[i]), (char*)(const char*)((String&)(*AgNamesPtr)[i]));
		m_lb->AddString(Tmp);
    }

    TOpenBase agents(AgentDBName, AgentFields, 2);
    if(agents.pxErr) {
	   EnableWindow(GetDlgItem(HWindow, 1), FALSE);
       return;
    }
    for(i = 0; i < agents.GetRecords(); i++) {
	  char Code[20];
      agents.Get();
      agents(Tmp, sizeof(Tmp) - 1, 1);
	  agents(Code, 20, 0);
      int idx = m_combo->AddString(Tmp);
	  long CodeData;
	  memcpy(&CodeData, Code, 4);
	  SendMessage(m_combo->HWindow, LB_SETITEMDATA, idx, CodeData);
      agents.Next();
    }

    m_lb->SetSelIndex(0);
    PrevIndex = 0;

    ChLbIndex();

	//AlignCombo(m_combo);
}

int LinkAgDlg::IsValid()
{
	for(int i = 0; i < NewAgCodesPtr->getItemsInContainer(); i++)
    {
		if(strlen((const char*)((String&)(*NewAgCodesPtr)[i])) == 0)
		{
			m_lb->SetSelIndex(i);
			ChLbIndex();
			MessageBeep(0);
			return 0;
		}
	}
	return 1;
}

void LinkAgDlg::Ok(RTMessage msg)
{
    ChLbIndex();
    if(!IsValid())
        return;

	if(pIgnoreNames)
		*pIgnoreNames = m_cb->GetCheck() == 1;

    TDialog::Ok(msg);
}

void LinkAgDlg::SaveLinks(RTMessage)
{
    ChLbIndex();
    if(!IsValid()) {
	//MessageBox(HWindow, "Не все связи установлены", "Сообщаю", MB_OK);
	return;
    }

	DynStr szDirName;
	strcpy(szDirName, "C:\\");
    char  l_szFilter[64];
    strcpy(l_szFilter, GetRCStr(528));
	DynStr szFile;
    OPENFILENAME ofn;
    if(!MyOpenFile(HWindow, ofn, l_szFilter, szDirName, szFile, "Сохранить агентов", "", 0)) return;

	FILE* f = fopen(szFile, "wt");
    if(!f) {
	    MessageBox(HWindow, GetRCStr(529), "Сообщаю", MB_OK);
	    return;
    }

    //стр-ра файла агент из файла=агент из БД
    for(int i = 0; i < NewAgCodesPtr->getItemsInContainer(); i++) 
	fprintf(f, "%s=%s\n", (const char*)((String&)(*AgCodesPtr)[i]), (const char*)((String&)(*NewAgCodesPtr)[i]));

    fclose(f);
}

void LinkAgDlg::LoadLinks(RTMessage)
{
    char l_szDirName[256] = "C:\\";
    char  l_szFilter[] = "Файлы связей агентов(*.agn)|*.agn||";
    char l_szFile[256] = "";
    OPENFILENAME ofn;
    if(!MyOpenFile(HWindow, ofn, l_szFilter, l_szDirName, l_szFile, "Прочитать агентов", "", 1)) return;

    FILE* f = fopen(l_szFile, "rt");
    if(!f) {
	MessageBox(HWindow, "Не могу открыть файл", "Сообщаю", MB_OK);
	return;
    }

    while(!feof(f)) {
	   memset(l_szDirName, 0, sizeof l_szDirName);
	   fgets(l_szDirName, sizeof l_szDirName, f);

	   if(strlen(l_szDirName) == 0) break;
	   char* divider = strchr(l_szDirName, '=');
	   if(divider == 0) break;
	   *divider = 0;
	   divider++;
	   if(*divider == 0) break;
	   divider[4] = 0;
	   if(strlen(divider) != 4 || strlen(l_szDirName) != 4) break;

	   //MessageBox(0, l_szDirName, divider, 0);

	   for(int i = 0; i < NewAgCodesPtr->getItemsInContainer(); i++)
		   if(!strcmp((const char*)(String&)(*AgCodesPtr)[i], l_szDirName)) {
			   NewAgCodesPtr->addAt(*new String(divider), i);
			   if(i == SendMessage(m_lb->HWindow, LB_GETCURSEL, 0, 0)) {
				   for(int idx = 0; idx < m_combo->GetCount(); idx++)
					   if(SendMessage(m_combo->HWindow, LB_GETITEMDATA, idx, 0) == atol(divider))
						   SendMessage(m_combo->HWindow, LB_SETCURSEL, idx, 0);
			   }
		   }
	}
    //стр-ра файла агент из файла=агент из БД
//    for(int i = 0; i < NewAgCodesPtr->getItemsInContainer(); i++) 
//	fprintf(f, "%s=%s\n", (const char*)((String&)(*AgCodesPtr)[i]), (const char*)((String&)(*NewAgCodesPtr)[i]));

    fclose(f);

    MessageBox(HWindow, GetRCStr(513), "Сообщаю", MB_OK);
}

void LinkAgDlg::AllEq(RTMessage)
{
	char curr_code[10];
    int Idx = (int)SendMessage(m_combo->HWindow, LB_GETCURSEL, 0, 0);
    if(Idx == -1) SendMessage(m_combo->HWindow, LB_SETCURSEL, Idx = 0, 0);
	long Id = (long)SendMessage(m_combo->HWindow, LB_GETITEMDATA, Idx, 0);
	memcpy(curr_code, &Id, 4);
	curr_code[4] = 0;
	NewAgCodesPtr->flush();
	for(int i = 0; i < AgCodesPtr->getItemsInContainer(); i++)
    {
		NewAgCodesPtr->add(*new String(curr_code));
    }
}

void CopyFieldValue(TOpenBase& From, TOpenBase& To, int field);

int RestoreFieldValue(FILE* f, TOpenBase& To, int field)
{
    static row = 0;
    if(field == 60) row++;

    double Value;
    DynStr  buff(1025);
    DATE   date_val;

	PXFldType(
		To.getTblHandle(),      // table to get field type from
		To.getField(field),      // field number of field in table
        256,                // size of return buffer
		buff);         // field type of field as string


	if(buff[0] == 'A'||buff[0] == 'M') {
		int Size;
		fread(&Size, sizeof Size, 1, f);
		int maxLen = buff[0] == 'M' ? 1024 : 255;
		if(Size < maxLen && Size >= 0)
		{
		   	fread(buff, Size, 1, f);
	       	((char*)(const char*)buff)[Size] = 0;
		   	To(field, buff);
		}
		else {
	   		sprintf(buff, GetRCStr(531), Size, field);
	   		MessageBox(0, buff, "Ошибка", MB_OK | MB_SYSTEMMODAL);
			return 0;
		}
	}
    else
	if(buff[0] == 'D') {
		fread(&date_val, sizeof date_val, 1, f);
		if(date_val != -1234)
		    To(field, date_val);
    }
	else
	if(buff[0] == 'N') {
		Value = -1.;
		fread(&Value, sizeof Value, 1, f);

		if(Value != -1234.)
		    To(field, Value);

		if(Value > 1000000000000. || Value < -1000000000000.) {
		   sprintf(buff, GetRCStr(530), Value, field);
		   MessageBox(0, buff, "Ошибка", MB_OK | MB_SYSTEMMODAL);
		   return 0;
		}
    }
	else
	if(buff[0] == 'S') {
		short shortValue = 0;
		fread(&shortValue, sizeof shortValue, 1, f);
		if(shortValue != -1234)
			To(field, shortValue);
	}

    return 1;
}
/*
void aux_read_owner_from_file(FILE* f, TOpenBase& emptyTable, char Version)
{
    PXRecBufEmpty(emptyTable.getRecHandle());

    for(int i = 0; i <= 12; i++)
		RestoreFieldValue(f, emptyTable,  i);

    if(Version >= '4') {
        RestoreFieldValue(f, emptyTable,  13);  //RegDate
	    RestoreFieldValue(f, emptyTable,  14);  //Summa
	    if(Version == '4') {
	       RestoreFieldValue(f, emptyTable,  15);  //PayNmb
	       if(!emptyTable.IsNull(15)) {
	           double V;
	           emptyTable(V, 15);
	           char str[16];
	           sprintf(str, "%lu", (long)V);
	           emptyTable(15, str);
		   }
	    }
	    else
	       RestoreFieldValue(f, emptyTable,  15);  //PayNmb
		RestoreFieldValue(f, emptyTable,  16);  //PayDate
    }
}
*/
void aux_read_avaria_from_file(FILE* f, TOpenBase& Av2, TOpenBase* AvPaysTable, int Ignore, int Version)
{
	PXRecBufEmpty(Av2.getRecHandle());
    for(int i = 0; i <= 51; i++)
		if(!RestoreFieldValue(f, Av2,  i)) return;
	if(Version >= 4) {
		for(int i = 52; i <= 70; i++)
			if(!RestoreFieldValue(f, Av2,  i)) return;
	}
	if(Version >= ('B'-'0')) {
		//Чтение выплат
		if(!Ignore) {
			CopyFieldValue(Av2, *AvPaysTable, 0);
			CopyFieldValue(Av2, *AvPaysTable, 1);
			CopyFieldValue(Av2, *AvPaysTable, 2);
			while(PXSrchKey(AvPaysTable->getTblHandle(), AvPaysTable->getRecHandle(), 3, SEARCHFIRST) == PXSUCCESS)
				AvPaysTable->Delete();
        }

		char priznak;
		for(;;) {
			fread(&priznak, 1, 1, f);
			if(priznak == '!') break;
			for(int x = 0; x < 14; x++)
				RestoreFieldValue(f, *AvPaysTable, x);
			if(!Ignore) {
				AvPaysTable->AddRecord();
				//MessageBox(0, "", "", 0);
            }
		}
	}
}

//for flags see aux_save_fields_to_file

int aux_read_fields_from_file(FILE* f, TOpenBase& emptyTable, char Op, int Komplex)
{
    TRACE("READ PL");
	PXRecBufEmpty(emptyTable.getRecHandle());

    for(int i = 0; i <= 63; i++)
		RestoreFieldValue(f, emptyTable, i);   //Seria

	//if(Op == 'Б' || Op == 'P' || Op == '8' || Op == 'A' || Op == 'B' || Op == 'C' || Op == 'D' || Op == 'E') {
	    RestoreFieldValue(f, emptyTable, 64); //RetBRB
		RestoreFieldValue(f, emptyTable, 65); //RetEUR
    //}

	//if(Op == 'P' || Op == '8' || Op == 'A' || Op == 'B' || Op == 'C' || Op == 'D' || Op == 'E') {
		RestoreFieldValue(f, emptyTable, 66); //AgPercent
    //}

	//if(Op == '8' || Op == 'A' || Op == 'B' || Op == 'C' || Op == 'D' || Op == 'E') {
		RestoreFieldValue(f, emptyTable, 67); //Ret1
		RestoreFieldValue(f, emptyTable, 68); //Ret2
	/*}
    else {
		DATE d;
		emptyTable(d, 25);
		emptyTable(67, d);
		emptyTable(d, 36);
		emptyTable(68, d);
	}*/

	//if(Op == 'A' || Op == 'B' || Op == 'C' || Op == 'D' || Op == 'E')
		RestoreFieldValue(f, emptyTable, 69); 

	//if(Op == 'D' || Op == 'E')
    {
		RestoreFieldValue(f, emptyTable, 70);
		RestoreFieldValue(f, emptyTable, 71);
	}

	//if(Op == 'E')
    {
		RestoreFieldValue(f, emptyTable, 72);
		RestoreFieldValue(f, emptyTable, 73);
	}
	//ignore depricated fields

	if(Komplex)
	{
		for(int x = 74; x <= 80; x++)
			RestoreFieldValue(f, emptyTable, x);
	}

	return 1;
}

extern char* MandatoryFields[];
extern char MandatoryCnt;
extern char KomplexCnt;

/*void MandatoryDlg::Restore2(int Komplex)     
{
    OPENFILENAME ofn;
    DynStr l_szDirName;
    strcpy(l_szDirName, "D:\\");

    DynStr l_szFile;
    DynStr MsgBuff;
	*l_szFile = 0;
	*MsgBuff = 0;

    char  l_szFilter[] = "Файлы Backup(*.bkp)|*.bkp||";
    double Value;
    int Size;

    if(MyOpenFile(GetApplicationObject()->MainWindow->HWindow, ofn, l_szFilter, l_szDirName, l_szFile, (char*)GetRCStr(535), "")) {
	 strcpy(l_szDirName, l_szFile);
	 strcpy(strrchr(l_szFile, '\\'), "\\mandrest.db");

	 TOpenBase emptyTable(l_szFile, MandatoryFields, Komplex ? KomplexCnt : MandatoryCnt, 0);
	 if(emptyTable.pxErr)
	    goto FreeMemory;

	 if(emptyTable.GetRecords()) {
	    int Ret = MessageBox(GetApplicationObject()->MainWindow->HWindow, GetRCStr(536), "Ошибка", MB_YESNOCANCEL | MB_ICONQUESTION);
        if(Ret == IDCANCEL)
		    goto FreeMemory;
	    if(Ret == IDYES) {
	       SetCursor(LoadCursor(0, IDC_WAIT));
	       while(emptyTable.GetRecords())
		      emptyTable.Delete();
        }
	 }

	 SetCursor(LoadCursor(0, IDC_WAIT));

	 FILE* f = fopen(l_szDirName, "rb");
	 if(!f) {
	      MessageBox(GetApplicationObject()->MainWindow->HWindow, GetRCStr(537), "Ошибка", MB_OK | MB_ICONSTOP);
	      goto FreeMemory;
	 }

	 while(!feof(f)) {
	    char Op;
	    fread(&Op, sizeof Op, 1, f);

	    if(feof(f))
  		    break;

	    if(Op == 'D') {
		    fread(&Size, sizeof Size, 1, f);
			fread(l_szFile, Size, 1, f);
			((char*)l_szFile)[Size] = 0;
			fread(&Value, sizeof Value, 1, f);
			emptyTable(1, Value);
			int found = 0;
			if(PXSrchFld(emptyTable.getTblHandle(), emptyTable.getRecHandle(), emptyTable.getField(1), SEARCHFIRST) == PXSUCCESS) {
              for(;;) {
				char ser[10];
				emptyTable.Get();
				emptyTable(ser, sizeof ser, 0);
				if(!strcmp(ser, l_szFile)) {
					found = 1;
                    break;
				}
				if(PXSrchFld(emptyTable.getTblHandle(), emptyTable.getRecHandle(), emptyTable.getField(1), SEARCHNEXT) != PXSUCCESS)
					break;
			  }
            }

			if(found == 0) {
				wsprintf(MsgBuff + strlen(MsgBuff), "%s %ld\n", l_szFile, (long)(Value + 0.001));
				//MessageBox(GetApplicationObject()->MainWindow->HWindow, l_szFile, "Запомни", MB_OK | MB_ICONINFORMATION);
			}
            else
				emptyTable.Delete();
	    }
	    else {
			aux_read_fields_from_file(f, emptyTable, Op);
			int IsOK = 0;
			if(PXSrchKey(emptyTable.getTblHandle(), emptyTable.getRecHandle(), 2, SEARCHFIRST) != PXSUCCESS)
				IsOK = emptyTable.AddRecord();
			else
				IsOK = PXRecUpdate(emptyTable.getTblHandle(), emptyTable.getRecHandle());

			if(IsOK != PXSUCCESS)
				MessageBox(GetApplicationObject()->MainWindow->HWindow, "Ошибка", "Капцы", MB_OK | MB_ICONINFORMATION);
		}
	 }

     fclose(f);
		 MessageBox(GetApplicationObject()->MainWindow->HWindow, MsgBuff, "Запомни", MB_OK | MB_ICONINFORMATION);
		 MessageBox(GetApplicationObject()->MainWindow->HWindow, GetRCStr(538), "Сообщаю", MB_OK | MB_ICONINFORMATION);
	}

	FreeMemory:
//    delete l_szDirName;
//	delete l_szFile;
//	delete MsgBuff;
}
*/
int CheckMessageQueue(HWND wnd)
{
   MSG msg;

   while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      if(msg.hwnd == wnd) continue;
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

   return 1;
}
     /*
int MandatoryDlg::FindOnServer(char* FldName, char* Maska, Array& Strings)
{
   char ask_file[64];
   int TimeOut;
   UINT fileMode;
   GetProfileString("Paradox Engine", "NetNamePath", "", ask_file, sizeof ask_file);
   if(ask_file[strlen(ask_file) - 1] != '\\')
       strcat(ask_file, "\\");
   GetProfileString("Paradox Engine", "UserName", "", ask_file + strlen(ask_file), sizeof ask_file);
   strcat(ask_file, ".ask");

   strcpy(strrchr(ask_file, '.'), ".otv");
   if(!access(ask_file, 0) && unlink(ask_file)) {
	MessageBox(HWindow, GetRCStr(571), "Сообщаю", MB_OK | MB_ICONSTOP); //Server busy
	return -1;
   }

   //Файл запроса
   strcpy(strrchr(ask_file, '.'), ".ask");
   FILE* f = fopen(ask_file, "wt");
   if(f) {
       char buff[32];
       GetPrivateProfileString(MANDATORY_SECT, "FindModeParam", "", buff, sizeof buff, ININame);
	   fprintf(f, "%s '%s' %s", FldName, Maska, buff);
       fclose(f);
   }
   else {
       MessageBox(HWindow, "Ошибка создания файла.", "Ошибка", MB_OK | MB_ICONSTOP);
       return -1;
   }

   //Сотрём файл ответа (м.б. предыжущий)
   strcpy(strrchr(ask_file, '.'), ".stp");
   unlink(ask_file);
   strcpy(strrchr(ask_file, '.'), ".ask");

   TDialog* dlg = new TDialog(this, "MSG_DLG");
   GetModule()->MakeWindow(dlg);
   SetDlgItemText(dlg->HWindow, 100, "Передаю запрос...");
   EnableWindow(dlg->HWindow, FALSE);

   //Ждём начала работы сервера (сотрёт запрос)
   long time = GetCurrentTime();
   while(GetCurrentTime() - time < 7000) {
       CheckMessageQueue(HWindow);
       Yield();
       if(access(ask_file, 0)) break; //erase file -> server start exec before erase need create otv-file
   }

   //Не стёр - ошибка - занят
   if(access(ask_file, 0) == 0) { //File exists it is error
       unlink(ask_file);
       delete dlg;
       MessageBox(HWindow, GetRCStr(514), "Сообщаю", MB_OK | MB_ICONSTOP); //Server busy
       return -1;
   }

   //Ждём stp файл - конец поиска
   strcpy(strrchr(ask_file, '.'), ".stp");

   TimeOut = GetPrivateProfileInt(MANDATORY_SECT, "FindTimeOut", 10, ININame);
   if(TimeOut > 100) TimeOut = 100;

   fileMode = SetErrorMode(SEM_FAILCRITICALERRORS);
   long starttime = time = GetCurrentTime();
   long LastTimePrint = -1;
   char* msg_buf = new char[128];
   while((time = GetCurrentTime()) - starttime < (TimeOut * 1000)) {
       if((GetCurrentTime() - LastTimePrint) > 1000) {
	   sprintf(msg_buf, GetRCStr(572), TimeOut + (starttime - time) / 1000);
	   SetDlgItemText(dlg->HWindow, 100, msg_buf);
	   ::UpdateWindow(dlg->HWindow);
	   LastTimePrint = GetCurrentTime();
       }
       if(access(ask_file, 0) == 0) break; //Exists
       CheckMessageQueue(HWindow);
       Yield();
   }
   delete msg_buf;
   SetErrorMode(fileMode);

   delete dlg;

   //Если файл не появился
   //strcpy(strrchr(ask_file, '.'), ".stp");
   if(access(ask_file, 0) != 0) {
       //MessageBox(0, "delete dlg", "", 0);
       //delete dlg;
       MessageBox(HWindow, GetRCStr(514), "Сообщаю", MB_OK | MB_ICONSTOP); //Server busy
       return -1;
   }

   unlink(ask_file); //Удаляем признак выполнения запроса
   strcpy(strrchr(ask_file, '.'), ".otv");
   f = fopen(ask_file, "rt");
   if(f) {
       char* str = new char [128];

       while(!feof(f)) {
	   memset(str, 0, 128);
	   fgets(str, 100, f);
           if(str[0])
	     str[strlen(str) - 1] = 0;
	   if(!str[0]) continue;
	   Strings.add(*new String(str));
       }
       delete str;
       fclose(f);
       unlink(ask_file);
       return 1;//Strings.getItemsInContainer() > 0 ? 1 : 0;
   }
   else {
       MessageBox(HWindow, "Ошибка чтения файла.", "Ошибка", MB_OK | MB_ICONSTOP);
       return -1;
   }
}
	   */
//extern char* MandatoryAux_Fields[];
//extern char* OwnerFields[];
extern char* Avaria2Fields[];

class GetPeriodF : public TDialog {
public:
      int* pMonth;
	  int* pMonthPlus;
	  int* pYear;
      int* Mode;
      DynStr* FileName;

      static int LastMode1, LastMode2;

      GetPeriodF(PTWindowsObject Parent);

      void SetupWindow();
      void Ok(RTMessage);

      void ChangeIdx(RTMessage) = [ID_FIRST + 101];
      void ChangeCond(RTMessage) = [ID_FIRST + 103];
      void SelectFile(RTMessage) = [ID_FIRST + 105];
};

int GetPeriodF::LastMode1, GetPeriodF::LastMode2 = -1;

GetPeriodF::GetPeriodF(PTWindowsObject Parent)
	  : TDialog(Parent, "EXP_MAND")
{
      pMonth = 0;
      pYear = 0;
	  Mode = 0;
	  pMonthPlus = 0;
      (new TComboBox(this, 101, 1))->DisableTransfer();
      (new TComboBox(this, 102, 1))->DisableTransfer();
      (new TComboBox(this, 103, 1))->DisableTransfer();
	  (new TComboBox(this, 106, 1))->DisableTransfer();
	  (new TEdit(this, 104, 256))->DisableTransfer();
      (new TButton(this, 105))->DisableTransfer();
      (new TStatic(this, 201, 1))->DisableTransfer();
      (new TStatic(this, 202, 1))->DisableTransfer();
}

void GetPeriodF::SetupWindow()
{
    TDialog::SetupWindow();

	struct date dt;
    getdate(&dt);

	TComboBox* cb = (TComboBox*)ChildWithId(101);
	cb->AddString((char*)GetRCStr(539));

	if(dt.da_mon == 1) {
		dt.da_mon = 12;
		dt.da_year--;
    }

    for(int i = 100; i <= 111; i++) {
		cb->AddString((char*)GetRCStr(i));
		if(dt.da_mon == (i - 100 + 1) && LastMode1 == 0) {
			LastMode1 = dt.da_mon;
            //MessageBox(0, "","1",0);
		}
    }
	cb->SetSelIndex(LastMode1);

    cb = (TComboBox*)ChildWithId(102);
	for(i = 2007; i <= 2015; i++) {
		char s[10];
		sprintf(s, "%d", i);
		cb->AddString(s);
		if(dt.da_year == i && LastMode2 == -1) {
			LastMode2 = i - 2007;
			//MessageBox(0, "","2",0);
		}
    }
	cb->SetSelIndex(LastMode2);

    cb = (TComboBox*)ChildWithId(103);
	cb->AddString((char*)GetRCStr(630));
	cb->AddString((char*)GetRCStr(631));
	cb->AddString((char*)GetRCStr(632));
	cb->SetSelIndex(0);

	cb = (TComboBox*)ChildWithId(106);
	for(i = 0; i <= 12; i++)
	{
		char s[10]="";
        if (i > 0)
			sprintf(s, "+%d мес", i);
		cb->AddString(s);
	}
	cb->SetSelIndex(0);

	TMessage m;
	ChangeIdx(m);
	ChangeCond(m);
}

void GetPeriodF::ChangeCond(RTMessage)
{
	TComboBox* cb = (TComboBox*)ChildWithId(103);
	EnableWindow(ChildWithId(101)->HWindow, cb->GetSelIndex() != 2);
	EnableWindow(ChildWithId(201)->HWindow, cb->GetSelIndex() != 2);
	EnableWindow(ChildWithId(102)->HWindow, cb->GetSelIndex() != 2);
	EnableWindow(ChildWithId(104)->HWindow, cb->GetSelIndex() == 2);
	EnableWindow(ChildWithId(105)->HWindow, cb->GetSelIndex() == 2);
	EnableWindow(ChildWithId(202)->HWindow, cb->GetSelIndex() == 2);
	EnableWindow(ChildWithId(106)->HWindow, cb->GetSelIndex() != 2);
}

void GetPeriodF::SelectFile(RTMessage)
{
    char  l_szFilter[] = "Текcт(*.nmb)|*.nmb||";
    DynStr szDirName;
    OPENFILENAME ofn;
    strcpy(szDirName, "C:\\");
    if(!MyOpenFile(HWindow, ofn, l_szFilter, szDirName, *FileName, "Файл номеров", "", 1)) return;
    ((TEdit*)ChildWithId(104))->SetText(*FileName);
}

void GetPeriodF::ChangeIdx(RTMessage)
{
    TComboBox* cb = (TComboBox*)ChildWithId(101);
	TComboBox* cb2 = (TComboBox*)ChildWithId(102);
	EnableWindow(cb2->HWindow, cb->GetSelIndex() > 0);
	if(LastMode2 == -1)
		LastMode2 = 0;
	cb2->SetSelIndex(cb->GetSelIndex() > 0 ? LastMode2 : -1);
}

void GetPeriodF::Ok(RTMessage msg)
{
    TComboBox* cb = (TComboBox*)ChildWithId(101);
	*pMonth = cb->GetSelIndex();
    LastMode1 = *pMonth;
    cb = (TComboBox*)ChildWithId(102);
    *pYear = cb->GetSelIndex() + 2007;
    LastMode2 = cb->GetSelIndex();
    cb = (TComboBox*)ChildWithId(103);
	*Mode = cb->GetSelIndex();
	cb = (TComboBox*)ChildWithId(106);
	*pMonthPlus = cb->GetSelIndex();

    if(*Mode == 2 && (*FileName)[0] == 0)
		return;


    TDialog::Ok(msg);
}

void auxAddAgentCode(Array& AgentsCodes, char* Code)
{
    for(int i = 0; i < AgentsCodes.getItemsInContainer(); i++)
	if(!strcmp((const char*)((String&)AgentsCodes[i]), Code))
	    return;

    AgentsCodes.add(* new String(Code));
}

class DblList
{
   double* Array;
   long Count;
public:
   DblList() { Count = 0; Array = 0; }
   void SetSize(long Size) { Array = new double[Size]; memset(Array, 0, sizeof(double) * Size); }
   void Add(double Val) { Array[Count++] = Val; }
   int Find(double Val);
   ~DblList() { if(Array) delete Array; }
};

int DblList::Find(double Val)
{
    long Start = 0;
    if(Count > 10)
       Start = (Array[Count / 2] < Val) ? (Count / 2 - 2) : 0;
    for(long i = Start; i < Count; i++)
	if(Array[i] == Val)
            return 1;

	return 0;
}

char** LoadData(const char* Name)
{
	int Position = 0;
	int CurrSizeArray = 50;
	char** Data = (char**)malloc(CurrSizeArray * sizeof(char*));
	Data[0] = 0;

	DynStr _szModuleName;
	GetModuleFileName(GetApplicationObject()->hInstance, _szModuleName, _szModuleName._sizeof()-32);
	strcpy(strrchr(_szModuleName, '\\') + 1, Name);
	FILE* f = fopen(_szModuleName, "rt");
	if(f) {
		while(!feof(f)) {
			*((char*)_szModuleName) = 0;
			fgets(_szModuleName, _szModuleName._sizeof(), f);
			char* ch;
			if(_szModuleName[0]) {
				while(ch = strchr(_szModuleName, 10)) *ch = 0;
				while(ch = strchr(_szModuleName, 13)) *ch = 0;
				ch = _szModuleName;
				while(*ch && *ch == ' ') ch++;
				if(strlen(ch) > 2) {
					if (Position == (CurrSizeArray - 1))
						Data = (char**)realloc(Data, (CurrSizeArray += 20) * sizeof(char*));
					Data[Position++] = strdup(ch);
					Data[Position] = 0;
				}
			}
        }
		fclose(f);
    }

	return Data;
}

int IsFiz(const char* s)
{
	static char** sokr = LoadData("FizFace.txt");

	DynStr Key1, Key2;

	for(int i = 0; sokr[i]; i++) {
		strcpy(Key1, sokr[i]);  //"AAA "
		strcat(Key1, " ");
		char* ch = strstr(s, Key1);
		if(ch && (ch == s || *(ch-1) == ' '))
			return 1;

		strcpy(Key2, " ");      //" AAA"
		strcat(Key2, sokr[i]);
		ch = strstr(s, Key2);

		if(ch && (*(ch + strlen(Key2)) == ' ' || *(ch + strlen(Key2)) == 0))
			return 1;
	}
    return 0;
}

int IsCompany(const char* s)
{
	char* sokr[] = { "ГОО", "УНИВЕРСИТЕТ", "РТПУП", "ГВСП", "ВГУ", "ГОСПРЕДПРИЯТИЕ", "ЧПТУП",
					 "УВД", "ТТУ","БПСПООО", "ПФАФФ", "УЧАСТОК", "АВТОШКОЛА", "КАФЕ", "ТАМОЖНЯ", "СТО",
					 "ОБЛ", "БОЛЬНИЦА", "РЕСТОРАН", "ЧУПП", "ПО", 
					 "ВГСП", "ДИСТАНЦИЯ", "ОТДЕЛ", "ОУП", "ОРС", "ОООПП", "ОООСП", 
			         "ОБЪЕДИНЕНИЕ", "ЦЕНТР", "МУГП", "МКПБ", "КБ", "ИНСПЕКЦИЯ", 
					 "ОРТП", "ПСП", "МОДО", "КАУП", "ИЧУТП", "ДП", "ГТП", "УТЧП", 
					 "УПТК", "УКПП", "РУ", "ПТАО", "ПООО", "АВТОБАЗА", "ТПП",
					 "БЖД", "ХОЗЯЙСТВО", "УЧТП", "ФИЛИАЛ", "ВРУПП", "УЧТПП", "ПК", 
					 "УЧПП", "ТЭУП", "УПУП", "УПОРС", "МУЧП", "СТАНЦИЯ", "ПМК", 
					 "КОНЦЕРН", "ЧАСТЬ", "НВЦ", "ООВВР", "ПТП", "ВГТ", "ПМП", "КЧТТУП", 
					 "УРСП", "ЧУТП", "УКПМ", "РУПП", "РУСП", "ВГП", "НПФ", "НПП", "ЖРЭТ", 
					 "МКП", "ПКФ", "ЖКХ", "ПКХ", "ПС", "ДСК", "ЗАВОД", "ОДО", "ОАП", 
					 "РУП", "ДЕПО", "ГАП", "ВНП", "КФХ", "РМУП", "МП", "АП", "АО", "НПО", 
					 "КОЛХОЗ", "ОАО", "ООО", "ЗАО", "СП", "ПКП", "ЗАСО", "ЧП", "ИП", 
					 "СУ", "ОП", "УП", "ГТК", "ГПО", "ГП", "СМП", "УМ", "УЗЕЛ", "УПРАВЛЕНИЕ", "АДМИНИСТРАЦИЯ",
					 "СПООО", "ЧУТТП", "БАЗА", "ДОМ", "УЧП", "УЧНПП", "МВД", "УСК", "УПК", "ТЭЦ",
					 "РДУП", "СОЮЗ", "ТД", "ГАЛЕРЕЯ", "СООО", "СОВХОЗ", "КОМБИНАТ", "СМУ", "МПО", "РСУ",
					 "РН", "ТРЕЙДИНГ", "КОЛЛЕДЖ", "ПРЕДПРИЯТИЕ", "СЕРВИС", "МГП", "УКК", "МГКУ", "КВП",
					 "КУП", "ЖРЭУ", "РАЙПО", "ИЧУТЭП", "РАЙОНА", "РАЙОН", "ДУП", "ГПП", "АВТОКОМБИНАТ",
					 "БУТП", "СПМК", "КГБ", "ОБЩЕСТВО", "ГУВД", "ДЕПО", 0 };

	static char** sokr2 = 0;
	if(!sokr2) sokr2 = LoadData("UrFace.txt");

	DynStr Key1, Key2;

	int Count = 0;
	char* ch = strchr(s, ' ');
	if(!ch /*|| (ch - s) <= 2*/) return 1; //Нет пробелов или слово короче 3 букв - не чел
	Count++;
	char* ch2 = strchr(ch + 1, ' ');
	if(!ch2 /*|| (ch2 - ch) <= 2*/) return 1; //Нет 2х пробелов или слово короче 3 букв - не чел
	Count++;
	char* ch3 = strchr(ch2 + 1, ' ');
	if(ch3) return 1; //3 пробела - это много!!!!

	if(strpbrk(s, "0123456789"))
		return 1;

    for(int i = 0; sokr[i]; i++) {
		strcpy(Key1, sokr[i]);
		strcat(Key1, " ");
		char* ch = strstr(s, Key1);
		if(ch && (ch == s || *(ch-1) == ' '))
			return 1;

		strcpy(Key2, " ");
		strcat(Key2, sokr[i]);
		ch = strstr(s, Key2);
		if(ch && (*(ch + strlen(Key2)) == ' ' || *(ch + strlen(Key2)) == 0))
				return 1;
    }

	for(i = 0; sokr2[i]; i++) {
		strcpy(Key1, sokr2[i]);
		strcat(Key1, " ");
		strcpy(Key2, " ");
		strcat(Key2, sokr2[i]);
		char* ch = strstr(s, Key1);
		if(ch && (ch == s || *(ch-1) == ' '))
			return 1;
		ch = strstr(s, Key2);
		if(ch && (*(ch + strlen(Key2)) == ' ' || *(ch + strlen(Key2)) == 0))
				return 1;
    }
	return 0;
}
/*
int IsDigit(char* s, int Len = -1)
{
    if(Len == -1) Len = strlen(s);
    for(int i = 0; i < Len; i++)
	if(s[i] >= '0' && s[i] <= '9') return 1;
    return 0;
}

int IsEnglish(char* s, int Len = -1)
{
    if(Len == -1) Len = strlen(s);
    for(int i = 0; i < Len; i++) {
	if(s[i] >= 'A' && s[i] <= 'Z') return i + 1;
	if(s[i] >= 'a' && s[i] <= 'z') return i + 1;
    }
    return 0;
}

int IsRussian(char* s, int Len = -1)
{
    if(Len == -1) Len = strlen(s);
    for(int i = 0; i < Len; i++) {
	if(s[i] >= 'А' && s[i] <= 'Я') return i + 1;
	if(s[i] >= 'а' && s[i] <= 'я') return i + 1;
    }
    return 0;
}
*/
//V4 - Current

int IsValidMandRecord(TOpenBase&, int, int, const char*, int);

#define IS_FIZ_FLAG 1
#define IS_UR_FLAG 0
#define IS_IP_FLAG 2

void MandatoryDlg::Export(int Komplex)
{
	char ININame[128];
   if(Komplex)
   {
	GetPrivateProfileString("KOMPLEX", "INI", ::ININame, ININame, sizeof ININame, ::ININame);
   }
   else
   {
	strcpy(ININame, ::ININame);
   }


	Array list_asked(10, 0, 10);
	list_asked.ownsElements(1);
	Array list_errors(10, 0, 10);
     
    DblList list_numbers;
    DblList list_numbers2;

    OPENFILENAME ofn;
    DynStr szDirName;
    DynStr szNumbers;
#define errMsg szDirName

    strcpy(szDirName, "D:\\");

    Array AgentsCodes(10, 0, 10);
    AgentsCodes.ownsElements(1);

	DynStr szFile;

	int ExpMnth_ = 9, ExpY_ = 2000, ExpMnthPlus = 0; //Месяц и год экспорта
    int FilterMode = 0;


	GetPeriodF* dlg = new GetPeriodF(GetApplicationObject()->MainWindow);
	dlg->pMonth = &ExpMnth_;
    dlg->pYear = &ExpY_;
	dlg->Mode = &FilterMode;
	dlg->pMonthPlus = &ExpMnthPlus;
    dlg->FileName = &szNumbers;

    if(GetApplicationObject()->MainWindow->GetModule()->ExecDialog(dlg) != IDOK)
		return;

	ExpMnthPlus++;
	DATE MinDate=0, MaxDate=0;
	PXDateEncode(ExpMnth_, 1, ExpY_, &MinDate);
	PXDateEncode((ExpMnth_+ExpMnthPlus-1)%12+1, 1, (ExpMnth_+ExpMnthPlus)>12?(ExpY_+1):ExpY_, &MaxDate);

	if(FilterMode == 2) {
		sprintf(szFile, "epNumber.mnf");
	}
   	else
    if(ExpMnth_ == 0)
	    sprintf(szFile, "e%cAllDta.mnf", Komplex ? 'x' : 'p');
	else
		sprintf(szFile, "e%c%02u%04u.mnf", Komplex ? 'x' : 'p', ExpMnth_, ExpY_);
    char  l_szFilter[] = "Файлы ОСТС(*.mnf)|*.mnf||";

	if(!MyOpenFile(GetApplicationObject()->MainWindow->HWindow, ofn, l_szFilter, szDirName, szFile, (char*)GetRCStr(540), "", 0)) return;

	TOpenBase I(Komplex ? "komplex" : "mandator", MandatoryFields, Komplex ? KomplexCnt : MandatoryCnt);
    if(I.pxErr) {
	   MessageBeep(0);
       return;
    }

/*    TOpenBase Own("\\bases\\mandown", OwnerFields, 17);
    if(Own.pxErr) {
       MessageBeep(0);
       return;
    }*/
	TOpenBase Av2(Komplex ? 0 : "\\bases\\mandav2", Avaria2Fields, 71);
	if(!Komplex && Av2.pxErr) {
       MessageBeep(0);
       return;
    }
	TOpenBase TAXI(Komplex ? 0 : "\\bases\\Taxi", Taxi_Fields, 9);
    if(!Komplex && TAXI.pxErr) {
       MessageBeep(0);
       return;
    }
	TOpenBase AGTable(AgentDBName, AgentFields, 2);
    if(AGTable.pxErr) {
       MessageBeep(0);
       return;
    }

	TOpenBase AvPaysTable(Komplex ? 0 : AVPayTableName, AVPayFields, 14);
	if(!Komplex && AvPaysTable.pxErr) {
       MessageBeep(0);
       return;
    }

	FILE* log = fopen("c:\\explog.log", "w");

	FILE* f = fopen(szFile, "wb");
	if(!f) {
       MessageBox(GetApplicationObject()->MainWindow->HWindow, "Ошибка создания файла", "Ошибка", MB_OK | MB_ICONSTOP);
       return;
    }

    int d, m, y;

	fprintf(f, "ExpFile %cF%02u%04u", Komplex ? 'v' : 'V', (FilterMode == 2 ? 20 : ExpMnth_), ExpY_);

    if(FilterMode == 2) {
	FILE* nf = fopen(szNumbers, "rt");
	if(nf) {
	    fseek(nf, 0, SEEK_END);
	    long Size = ftell(nf) / 10;
	    long LIMIT = 8150;
	    if(Size > LIMIT)
	    	Size = LIMIT;
	    list_numbers.SetSize(LIMIT);
	    fseek(nf, 0, SEEK_SET);

	    for(long Counter = 0;; Counter++) {
		strcpy(szNumbers, "");
		fgets(szNumbers, szNumbers._sizeof(), nf);
		if(szNumbers[0] == 0) break;
		char* divider = strchr(szNumbers, ',');
		if(divider) {
			list_numbers.Add(atol(divider + 1));
		    if(Counter > (LIMIT-2)) {
				MessageBox(GetApplicationObject()->MainWindow->HWindow, "Слишком много полисов.", "Сообщаю", 0);
            	fclose(log);
				break;
            }
		}
	    }
	    fclose(nf);
	}
	else
            return;
    }
    else
	if(!Komplex && ExpMnth_ > 0 || FilterMode == 1) //Если Не все данные то проверить что бы даты аварий
    {
	list_numbers.SetSize(Av2.GetRecords() + 5);

	//Дата обновления аварий
	long AllRecords = Av2.GetRecords();
    for(long i = 0; i < AllRecords; i++) {
		fseek(log, 0, SEEK_SET);
		fprintf(log, "AVRL %lu.", i);
        fflush(log);
		Av2.Get();
	    DATE date = 0;
	    Av2(date, 37); //UpdateData
	    //PXDateDecode(date, &m, &d, &y);
		int IsAdd = date >= MinDate && date < MaxDate;//(ExpMnth == 0) || (m == ExpMnth && y == ExpY);
	    if(!IsAdd) {
		for(int NN = 22; (IsAdd == 0) && (NN <= 36); NN += 7) { //Даты выплат
                  if(!Av2.IsNull(NN)) {	
		    Av2(date, NN);
		    PXDateDecode(date, &m, &d, &y);
			IsAdd = (ExpMnth_ == 0) ||  date >= MinDate && date < MaxDate;//(m == ExpMnth && y == ExpY);
		  }
                }
	    }

	    if(IsAdd)
	      list_numbers.Add(Av2.GetDouble(1));
	    Av2.Next();
	}
	Av2.First();

	//Дата обновления доп владельцев
/*	list_numbers2.SetSize(Own.GetRecords() + 5);
	AllRecords = Own.GetRecords();
		for(i = 0; i < AllRecords; i++)
		{
		fseek(log, 0, SEEK_SET);
		fprintf(log, "OWNL %lu.", i);
        fflush(log);
		Own.Get();

	    DATE date = 0;
	    Own(date, 3); //UpdateData
	    PXDateDecode(date, &m, &d, &y);
	    if(ExpMnth_ > 0) //Не все данные
		 if(date >= MinDate && date < MaxDate)
		 {
			double Number;
			Own(Number, 1);
			list_numbers2.Add(Number);
	     }

	    Own.Next();
        }

	Own.First();
*/
    }

    int ContunueExp = 1;

    Process* findWnd = (Process*)GetApplicationObject()->MainWindow->GetModule()->MakeWindow(new Process(GetApplicationObject()->MainWindow));
    SetCursor(LoadCursor(0, IDC_WAIT));
	long AllAllRecords = I.GetRecords() + /*Own.GetRecords() + */ (Komplex ? 0 : Av2.GetRecords());
	long AllRecords = I.GetRecords();

	long CntPolises = 0, CntAvarias = 0;

	for(long Pos = 0, ExpCount = 0, Processed = 0; ContunueExp && Pos < AllRecords; Pos++, Processed++) {

		fseek(log, 0, SEEK_SET);
		fprintf(log, "POL %lu.", Pos);
		fflush(log);

	I.Get();

	if(list_errors.getItemsInContainer() > 1000)
		break;

	char Seria[10];
	char FIO[64];
	double FIOTypePr;
	double Number;
	I(Seria, sizeof Seria, 0);
	I(FIO, sizeof FIO, 3);
	I(FIOTypePr, 56);
	I(Number, 1);



	DATE date;
	DATE regdate;
	DATE repdate;
	I(date, 60); //UpdateData
	I(regdate, 2); 
	I(repdate, 67);

	if(ISBLANKDATE(date) || date == 0) {
	    if(ExpMnth_ > 0) { //Не весь период
			PXDateEncode(ExpMnth_, 1, ExpY_, &date);
			I(60, date); //UpdateData
        }
	    if(ExpMnth_ == 0 || PXRecUpdate(I.getTblHandle(), I.getRecHandle()) != PXSUCCESS) {
	        ContunueExp = 0;
			sprintf(errMsg, GetRCStr(533), Seria, (long)Number);
			MessageBox(GetApplicationObject()->MainWindow->HWindow, errMsg, "Ошибка", MB_OK | MB_ICONSTOP);
			break;
	    }
	}

	if((((ExpMnth_ == 0) ||
			  date >= MinDate && date < MaxDate ||
			  (regdate >= MinDate && regdate < MaxDate) || (repdate >= MinDate && repdate < MaxDate)) && (FilterMode == 0)) ||
			   list_numbers.Find(Number) ||
			   list_numbers2.Find(Number)) {

		   //fseek(log, 0, SEEK_SET); fprintf(log, "A"); fflush(log); //////!!!!!!!!!!!!!!

		   double Ver;
		   I(Ver, 57);

		   int CheckErr = Ver < 2 ? 0 : IsValidMandRecord(I, 0, 0, ININame, Komplex);
		   if(CheckErr) {
			   sprintf(errMsg, "%s/%ld Ошибка %s", I.GetString(0).s, (long)I.GetDouble(1), GetCheckErrorText(CheckErr));
			   list_errors.add(*new String(errMsg));
		   }

		   //fseek(log, 0, SEEK_SET); fprintf(log, "B"); fflush(log);; //////!!!!!!!!!!!!!!

		   //Контроль данных
		   double State;
		   I(State, 58);
		   char ag[5];
		   I(ag, sizeof ag, 30);

		   //fseek(log, 0, SEEK_SET); fprintf(log, "C"); fflush(log);; //////!!!!!!!!!!!!!!

		  if(State != 1 && *ag)
		  { //Не испорчен
		  //fseek(log, 0, SEEK_SET); fprintf(log, "G"); fflush(log); //////!!!!!!!!!!!!!!
		 //Конироль типа юр физ
			if(Ver > 1 && fabs(FIOTypePr - IS_IP_FLAG) > 0.001)
			{
//			   fseek(log, 0, SEEK_SET); fprintf(log, "H"); fflush(log); //////!!!!!!!!!!!!!!
				if(!(IsFiz(FIO) && FIOTypePr == IS_FIZ_FLAG))
				{ //сли нет физическое то проверить
//				   fseek(log, 0, SEEK_SET); fprintf(log, "I"); fflush(log); //////!!!!!!!!!!!!!!
					if(IsCompany(FIO) && FIOTypePr != IS_UR_FLAG)
					{ //Если точно компания то стоп!!!!!
						initPosition = I.NumberCurrent();
						sprintf(errMsg, GetRCStr(586), Seria, (long)Number, I.NumberCurrent());
//						fseek(log, 0, SEEK_SET); fprintf(log, "D"); fflush(log); //////!!!!!!!!!!!!!!
						list_errors.add(*new String(errMsg));
					}
				}
				 //fseek(log, 0, SEEK_SET); fprintf(log, "K"); fflush(log); //////!!!!!!!!!!!!!!
				if(!IsCompany(FIO) && FIOTypePr != IS_FIZ_FLAG)
				{ //Если не компания (так решила прога) то
				   //ИСКАТЬ В ПРОПУСКАХ	 
					//fseek(log, 0, SEEK_SET); fprintf(log, "J"); fflush(log); //////!!!!!!!!!!!!!!
				   for(int isfound = 0, i = 0; i < list_asked.getItemsInContainer(); i++)
	               {
						if(!strcmp(FIO, (const char*)(String&)list_asked[i]))
						{
				       		isfound = 1;
	                        break;
						}
	               }

//				   fseek(log, 0, SEEK_SET); fprintf(log, "E"); fflush(log); //////!!!!!!!!!!!!!!
				   if(!isfound)
				   {
					   sprintf(errMsg, GetRCStr(587), Seria, (long)Number, FIO);  //Уверены что юр лицо
					   if(MessageBox(GetApplicationObject()->MainWindow->HWindow, errMsg, "Ответь", MB_YESNO | MB_ICONQUESTION) == IDNO)
					   {
					      list_errors.add(*new String(errMsg));
					   }
					   else
	                   {
						  list_asked.add(*new String(FIO)); //в пропущенный добавить
	                   }
	               }
			   }
			} //Ver
		  } //Не испорчен!!!

//		  fseek(log, 0, SEEK_SET); fprintf(log, "F"); fflush(log); //////!!!!!!!!!!!!!!
		  fprintf(f, "+");
		  if	(!aux_save_fields_to_file2(&I, f, 0, Komplex))
		  {
			 sprintf(errMsg, "%s/%ld было выдано сообщение об ошибке", I.GetString(0).s, (long)I.GetDouble(1));
		     list_errors.add(*new String(errMsg));
		  }
		     
/*			int Len = GetPrivateProfileString(MANDATORY_SECT, "Company", "", errMsg, errMsg._sizeof(), ::ININame);
   		    fwrite(&Len, sizeof Len, 1, f);
			fwrite(errMsg, Len, 1, f);
			Len = 1;
			fwrite(&Len, sizeof Len, 1, f);
			fwrite("?", Len, 1, f);
			fwrite(&Len, sizeof Len, 1, f);
			fwrite("?", Len, 1, f);
  */
		ExpCount++;
		CntPolises++;
	    findWnd->SetAll(ExpCount);

	    char Agent_Code[10];
		I(Agent_Code, sizeof Agent_Code, 30);
		if(*Agent_Code)
        {
			auxAddAgentCode(AgentsCodes, Agent_Code);
        }
		double ispay2;
		I(ispay2, 35);
		if(ispay2 > 0)
		{
			I(Agent_Code, sizeof Agent_Code, 28);
			if(*Agent_Code)
            {
				auxAddAgentCode(AgentsCodes, Agent_Code);
            }
        }
	}

		I.Next();
		findWnd->SetPercent(Processed * 100 / AllAllRecords);
    }
    fprintf(f, "."); //Признак конца секции Полисов

	//----------------------------------------------------------------------------------------------------------------

/*    AllRecords = Own.GetRecords();
    for(Pos = 0; ContunueExp && Pos < AllRecords; Pos++, Processed++) {
		fseek(log, 0, SEEK_SET);
		fprintf(log, "OWN %lu.", Pos);
        fflush(log);
	Own.Get();

	DATE date;
	Own(date, 3); //UpdateData
        double Number;
	Own(Number, 1);

	if(ISBLANKDATE(date) || date == 0) {
	    if(ExpMnth_ > 0) {
			PXDateEncode(ExpMnth_, 1, ExpY_, &date);
			Own(3, date); //UpdateData
	    }
		if((ExpMnth_ == 0) || PXRecUpdate(Own.getTblHandle(), Own.getRecHandle()) != PXSUCCESS)
		{
			ContunueExp = 0;
			MessageBox(GetApplicationObject()->MainWindow->HWindow, GetRCStr(576), "Ошибка", MB_OK | MB_ICONSTOP);
			break;
        }
	}

	PXDateDecode(date, &m, &d, &y);
	if( (((ExpMnth_ == 0) || (date >= MinDate && date < MaxDate)) && (FilterMode == 0)) || list_numbers.Find(Number) || list_numbers.Find(Number)) {
	    ExpCount++;
	    findWnd->SetAll(ExpCount);

		int CheckErr = IsValidOwnerRecord(Own);
	   	if(CheckErr) {
			sprintf(errMsg, "%s/%ld (Доп. владелец) Ошибка %s", Own.GetString(0).s, (long)Own.GetDouble(1), GetCheckErrorText(CheckErr));
		   	list_errors.add(*new String(errMsg));
		}

		fprintf(f, "+");
        for(int x = 0; x <= 16; x++)
			CopyFieldValue(Own, f,  x);   
	}

	Own.Next();
	findWnd->SetPercent(Processed * 100 / AllAllRecords);
    }*/
    fprintf(f, "."); 

	//----------------------------------------------------------------------------------------------------------------

	//АВАРИИ
	AllRecords = Komplex ? 0 : Av2.GetRecords();
    for(Pos = 0; ContunueExp && Pos < AllRecords; Pos++, Processed++) {
		fseek(log, 0, SEEK_SET);
		fprintf(log, "AVR %lu.", Pos);
        fflush(log);
		Av2.Get();

        char Seria[10];
        double Number;
        Av2(Seria, sizeof Seria, 0);
	Av2(Number, 1);
	DATE date;
	Av2(date, 37); //UpdateData

	if(ISBLANKDATE(date) || date == 0) {
		if(ExpMnth_ > 0)
		{
			PXDateEncode(ExpMnth_, 1, ExpY_, &date);
			Av2(37, date); //UpdateData
        }
		if((ExpMnth_ == 0) || PXRecUpdate(Av2.getTblHandle(), Av2.getRecHandle()) != PXSUCCESS)
		{
			ContunueExp = 0;
			MessageBox(GetApplicationObject()->MainWindow->HWindow, GetRCStr(577), "Ошибка", MB_OK | MB_ICONSTOP);
			break;
        }
	}

	PXDateDecode(date, &m, &d, &y);
	if( (((ExpMnth_ == 0) || (date >= MinDate && date < MaxDate)) && (FilterMode == 0)) || list_numbers.Find(Number) || list_numbers.Find(Number)) {
		ExpCount++;
		CntAvarias++;
	    findWnd->SetAll(ExpCount);

		int CheckErr = IsValidAvariaRecord(Av2);
	   	if(CheckErr) {
			sprintf(errMsg, "%s/%ld (Авария N%ld) Ошибка %s", Av2.GetString(0).s, (long)Av2.GetDouble(1), (long)Av2.GetDouble(2), GetCheckErrorText(CheckErr));
		   	list_errors.add(*new String(errMsg));
		}

	    fprintf(f, "+");

		for(int x = 0; x <= 70; x++)
			CopyFieldValue(Av2, f,  x);

		//Запись выплат
		CopyFieldValue(Av2, AvPaysTable, 0);
		CopyFieldValue(Av2, AvPaysTable, 1);
		CopyFieldValue(Av2, AvPaysTable, 2);
		if(PXSrchKey(AvPaysTable.getTblHandle(), AvPaysTable.getRecHandle(), 3, SEARCHFIRST) == PXSUCCESS) {
            do {
				fprintf(f, "@");
				AvPaysTable.Get();
				for(int x = 0; x < 14; x++)
					CopyFieldValue(AvPaysTable, f,  x);
			}
			while(PXSrchKey(AvPaysTable.getTblHandle(), AvPaysTable.getRecHandle(), 3, SEARCHNEXT) == PXSUCCESS);
        }
		fprintf(f, "!");
	}

	Av2.Next();
	findWnd->SetPercent(Processed * 100 / AllAllRecords);
    }
    fprintf(f, "."); 


	//TAXI

	long TaxiCount = 0;
	AllRecords = Komplex ? 0 : TAXI.GetRecords();
    for(Pos = 0; ContunueExp && Pos < AllRecords; Pos++, Processed++) {
		TAXI.Get();

		fprintf(f, "+");
		for(int i = 0; i < 9; i++)
			CopyFieldValue(TAXI, f,  i);

		TAXI.Next();
		TaxiCount++;
	}
	fprintf(f, ".");
      
	//----------------------------------------------------------------------------------------------------------------

	//Запись Агентов

		fseek(log, 0, SEEK_SET);
		fprintf(log, "AGENTS.", Pos);
        fflush(log);


	if(ContunueExp)
		ContunueExp = SaveAgents(AgentsCodes, f, AGTable, &list_errors, I, 30);

	long BadCount = 0;

    //Запись испорченных
	fprintf(f, "+");
	AGTable.First();
	AGTable.Get();
	CopyFieldValue(AGTable, f,  0); 
	CopyFieldValue(AGTable, f,  1);
	AllRecords = I.GetRecords();
    long sPos = ftell(f);
	fwrite(&AllRecords, 4, 1, f);
	I.First();

	fwrite("End of file", 11, 1, f);

	fseek(f, sPos, SEEK_SET);
	fwrite(&BadCount, 4, 1, f);

		fseek(log, 0, SEEK_SET);
		fprintf(log, "STOP.", Pos);
		fflush(log);

	fclose(log);
	delete findWnd;

	if(ContunueExp == 0 || list_errors.getItemsInContainer() > 0) {
		fclose(f);
		unlink(szFile);
		MessageBox(GetApplicationObject()->MainWindow->HWindow, "Исправляйте ошибки", "", MB_OK | MB_ICONSTOP);
		ShowArray(list_errors);
	}
	else {
		char msg[128];
		sprintf(msg, "В файл записано полисов %lu, аварий %lu, испорченных %lu, доплат %lu", CntPolises, CntAvarias, BadCount, TaxiCount);
		MessageBox(GetApplicationObject()->MainWindow->HWindow, msg, "Ура", MB_OK | MB_ICONSTOP);
	}

    fclose(f);
}

const char* GetMyAgCode(char* AgentCode, Array& file, Array& base, int Number)
{
	if(!*AgentCode)
		return "";
	if(!strcmp(AgentCode, "АЛВН"))
		return "АЛВН";
	for(int i = 0; i < file.getItemsInContainer(); i++)
	if(!strcmp( (const char*)((String&)file[i]), AgentCode ))
	    return (const char*)((String&)base[i]);

    char s[64];
	sprintf(s, GetRCStr(578), Number, AgentCode, file.getItemsInContainer(), base.getItemsInContainer());
    MessageBox(GetApplicationObject()->MainWindow->HWindow, s, "Ошибка", MB_OK | MB_ICONSTOP);
    exit(0);
}

/////////////////////??????????
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
/////////////

void MandatoryDlg::Import(int Komplex)
{
//MessageBox(0, "qqqqqqqqqqqqqqqqqqqqqqqqqqqq0", 0, 0);
	ValidateInternalImport = GetPrivateProfileInt("DIVISION", "ValidateImport", 0, ::ININame);

	char ignoreFieldsList[] = {3, 4, 15, 16, 10};  //name address
	struct date d;
    getdate(&d);

    DATE CurrDate;
    PXDateEncode(d.da_mon, d.da_day, d.da_year, &CurrDate);

	TOpenBase I(Komplex ? "\\komplex" : "\\mandator", MandatoryFields, Komplex ? KomplexCnt : MandatoryCnt);
    if(I.pxErr) {
//MessageBox(0, "qqqqqqqqqqqqqqqqqqqqqqqqqqqq0", 0, 0);
       MessageBeep(0);
       return;
    }
//MessageBox(0, 0, 0, 0);
/*	TOpenBase Own("\\bases\\mandown", OwnerFields, 17);
    if(Own.pxErr) {
       MessageBeep(0);
       return;
    }
  */
	TOpenBase Av2(Komplex ? 0 : "mandav2", Avaria2Fields, 71);
    if(!Komplex && Av2.pxErr) {
       MessageBeep(0);
       return;
    }
//MessageBox(0, 0, 0, 0);

	TOpenBase TAXI0(Komplex ? 0 : "Taxi", Taxi_Fields, Komplex ? 0 : 9);
	if(!Komplex && TAXI0.pxErr) {
       MessageBeep(0);
       return;
    }
//MessageBox(0, 0, 0, 0);
	OPENFILENAME ofn;
	DynStr szDirName;
	strcpy(szDirName, "D:\\");

	DynStr szFile;

    char  l_szFilter[] = "Файлы ОСТС(*.mnf)|*.mnf||";

	if(!MyOpenFile(GetApplicationObject()->MainWindow->HWindow, ofn, l_szFilter, szDirName, szFile, "Импорт данных по ОСТС", "", 1)) return;

    FILE* f = fopen(szFile, "rb");
    if(!f) {
       MessageBox(GetApplicationObject()->MainWindow->HWindow, "Ошибка открытия файла", "Ошибка", MB_OK | MB_ICONSTOP);
       return;
    }

    fseek(f, 0, SEEK_END);
    long FSize = ftell(f);


    fseek(f, 0, SEEK_SET);

#define DATA_BUFFER szDirName

    fread(DATA_BUFFER, 16, 1, f);
    ((char*)DATA_BUFFER)[17] = 0;

    if(!(!strncmp(DATA_BUFFER, "ExpFile V", 9) && !Komplex || !strncmp(DATA_BUFFER, "ExpFile v", 9) && Komplex)) {
       fclose(f);
       MessageBox(GetApplicationObject()->MainWindow->HWindow, "Формат файла неправильный", "Ошибка", MB_OK | MB_ICONSTOP);
       return;
    }

    char Version = DATA_BUFFER[9];
    int m = atol(DATA_BUFFER + 10) / 10000;
    int y = atol(DATA_BUFFER + 10) - m * 10000;

	if(Version < 'D'/*Version != '7' && Version != '8'*/) {
       fclose(f);
       MessageBox(GetApplicationObject()->MainWindow->HWindow, GetRCStr(515), "Ошибка", MB_OK | MB_ICONSTOP);
       return;
    }

    if(m == 0)
		sprintf(DATA_BUFFER, GetRCStr(527), (char*)szFile, Version);
    else
    if(m == 20)
		sprintf(DATA_BUFFER, GetRCStr(618), (char*)szFile, Version);
    else
		sprintf(DATA_BUFFER, GetRCStr(516), (char*)szFile, Version, m, y);
//    assert(strlen(DATA_BUFFER) < sizeof(DATA_BUFFER));

    if(MessageBox(GetApplicationObject()->MainWindow->HWindow, DATA_BUFFER, "Импорт ОСТС", MB_YESNO | MB_ICONQUESTION) == IDNO) {
       fclose(f);
       return;
    }

    SetCursor(LoadCursor(0, IDC_WAIT));

    int DataIsOk = 1;

    long AllPolises = 0;

    int WhatBad = 0;
    int NumberBad = 0;

    //Проверка данных

    int CountPol = 0, CountOwn = 0, CountAv = 0;

	Array listNumbers(1000, 0, 100);
	listNumbers.ownsElements(TRUE);
	int countPolises = 0;

    Process* findWnd = (Process*)GetApplicationObject()->MainWindow->GetModule()->MakeWindow(new Process(GetApplicationObject()->MainWindow));
    SetDlgItemText(findWnd->HWindow, 377, "Проверка");

	char VerLetter = 0;
		VerLetter = //Version == '2' ? 'Б' :
					//						(Version == '3' || Version == '4' || Version == '5' || Version == '6' || Version == '7') ? 'P' :
					//						(Version == '8' || Version == '9') ? '8' :  Version == 'A' ? 'A' : Version == 'B' ? 'B' : Version == 'C' ? 'C' :
											Version >= 'D' && Version <= 'F' ? Version : '?';
	int istaxi=(VerLetter>='C');
	for(NumberBad = 0;;NumberBad++)
	{
		char Priznak;
		fread(&Priznak, 1, 1, f);

		if(Priznak == '.') {
			break;
		}

		if(Priznak != '+') {
			DataIsOk = 0;
			break;
		}
		CountPol++;

		if(!aux_read_fields_from_file(f, I, VerLetter, Komplex)) {
			DataIsOk = 0;
			break;
		}

		double N;
		char s[32] = "";
		I(s, sizeof s, 0);
		I(N, 1);

		sprintf(s + strlen(s), "%lu", (long)N);
		if(countPolises < 1000)
			listNumbers.add(*new String(s));
		else
			listNumbers.flush();
		countPolises++;

		//ignore mand_txt fields (3 text fields)
		if (VerLetter < 'F')
		{
			RestoreFieldValue(f, I, 0);
			RestoreFieldValue(f, I, 0);
			RestoreFieldValue(f, I, 0);
		}

		AllPolises++;

		if(feof(f)) {
			DataIsOk = 0;
			break;
		}

		findWnd->SetPercent((double)ftell(f) * 100 / FSize);
	}

    if(DataIsOk == 0)
		WhatBad = 1;
    else
	    NumberBad = 0;


    for(;DataIsOk;NumberBad++) {
		char Priznak;
		fread(&Priznak, 1, 1, f);
		if(Priznak == '.') break;

			DataIsOk = 0;
			break;
/*		if(Priznak != '+') {
			DataIsOk = 0;
			break;
		}
		CountOwn++;
		aux_read_owner_from_file(f, Own, Version);
		AllPolises++;
		if(feof(f)) {
			DataIsOk = 0;
			break;
		}
		findWnd->SetPercent(ftell(f) * 100 / FSize);*/
	}

    if(DataIsOk == 0 && WhatBad == 0)
	    WhatBad = 2;

    if(DataIsOk) NumberBad = 0;

	TOpenBase AvPaysTable(Komplex ? 0 : AVPayTableName, AVPayFields, 14);
	if(!Komplex && AvPaysTable.pxErr) {
       MessageBeep(0);
       return;
    }

	if(!Komplex)
    {

    for(;DataIsOk;NumberBad++) {
		char Priznak;
		fread(&Priznak, 1, 1, f);
		if(Priznak == '.') break;
		if(Priznak != '+') {
			DataIsOk = 0;
			break;
		}

		CountAv++;
		aux_read_avaria_from_file(f, Av2, &AvPaysTable, 1, Version - '0');

		AllPolises++;
		if(feof(f)) {
			DataIsOk = 0;
			break;
		}
		findWnd->SetPercent(ftell(f) * 100 / FSize);
	}
	}
	else
	{
		char Priznak;
		fread(&Priznak, 1, 1, f);
		DataIsOk = Priznak == '.';
	}

	if(DataIsOk == 0 && WhatBad == 0)
  	    WhatBad = 3;

	long restoredTaxi=0;
	if (istaxi)
	{
	   for(;DataIsOk;NumberBad++)
	   {
		   char Priznak;
		   fread(&Priznak, 1, 1, f);
		   if(Priznak == '.') break;
		   if(Priznak != '+') {
			   DataIsOk = 0;
			   break;
		   }

		   for(int i = 0; i < 9; i++)
				RestoreFieldValue(f, TAXI0, i);

		   if(feof(f)) {
			   DataIsOk = 0;
			   break;
		   }
		   restoredTaxi++;
	   }
	}

	if(DataIsOk == 0 && WhatBad == 0)
  	    WhatBad = 4;

    delete findWnd;


	if(DataIsOk) NumberBad = 0;

    Array AgentsCodes(10, 0, 10);
	Array AgentsNames(10, 0, 10);
	Array NewAgentsCodes(10, 0, 10);
    AgentsCodes.ownsElements(1);
    AgentsNames.ownsElements(1);
    NewAgentsCodes.ownsElements(1);

	if(DataIsOk)
		DataIsOk = ReadAgents(f, AgentsCodes, AgentsNames, NewAgentsCodes);

	int IgnoreNames = 0;
	if (DataIsOk && AgentsNames.getItemsInContainer()) {
		GetApplicationObject()->ExecDialog(new ShowNumbersDlg(GetApplicationObject()->MainWindow, &listNumbers, countPolises));
		sprintf(DATA_BUFFER, GetRCStr(547), CountPol, CountOwn, CountAv);
		if(istaxi)
		{
			sprintf(DATA_BUFFER+strlen(DATA_BUFFER), "\n Доплат %lu", restoredTaxi);
		}
		LinkAgDlg* dlg = new LinkAgDlg(GetApplicationObject()->MainWindow);
		dlg->Msg = DATA_BUFFER;
	    dlg->AgCodesPtr = &AgentsCodes;
	    dlg->NewAgCodesPtr = &NewAgentsCodes;
	    dlg->AgNamesPtr = &AgentsNames;
		dlg->pIgnoreNames = &IgnoreNames;
		if(GetApplicationObject()->ExecDialog(dlg) != IDOK)
		{
	       fclose(f);
	       return;
		}
	}
	else
	if(DataIsOk && CountPol == 0)
	{
	   sprintf(DATA_BUFFER, GetRCStr(546), CountOwn, CountAv);
	   MessageBox(GetApplicationObject()->MainWindow->HWindow, DATA_BUFFER, "Сообщаю", MB_OK | MB_ICONINFORMATION);
	   return;
	}

	if(DataIsOk == 0 && WhatBad == 0)
		WhatBad = 4;

    if(DataIsOk) NumberBad = 0;

    if(!DataIsOk) {
       fclose(f);
       char* sss = WhatBad == 1 ? "Полисы" : WhatBad == 2 ? "Владельцы" : WhatBad == 3 ? "Аварии" : "?";
       sprintf(DATA_BUFFER, GetRCStr(545), sss, NumberBad);
	   MessageBox(GetApplicationObject()->MainWindow->HWindow, DATA_BUFFER, "Ошибка", MB_OK | MB_ICONSTOP);
       return;
    }

    //sprintf(DATA_BUFFER, "В файле содержиться\n  Полисов %d\n  Владельцев %d\n  Аварий %d", CountPol, CountOwn, CountAv);
    //MessageBox(GetApplicationObject()->MainWindow->HWindow, DATA_BUFFER, "Сообщаю", MB_OK | MB_ICONINFORMATION);

    //Алгоритм обновления данных
    //Прочитать в буфер таб2
    //Скопировать в таб
    //Найти
    //Если не найден - без вопросов
    //Если дата обновления в таб больше (более старый полис передали), то сообщение и пропускаем
    //Если нет отличающихся полей - пропускаем (сравнить буфера) и синхронизировать коды в буферах
    //Если отличаются то обновить с проверкой кодов
    //У испрченных дата платежа сбрасывается???

    long Pos = 0;
    long IgnoredDate = 0;
    long Ignored = 0;
    long AddedPolises = 0;
    long AddedOwners = 0;
    long UpdatedPolises = 0;
    long UpdatedOwners = 0;
    long AddedAvarias = 0;
    long UpdatedAvarias = 0;

    fseek(f, 16, SEEK_SET); //Пропуск заголовка и версии
    findWnd = (Process*)GetApplicationObject()->MainWindow->GetModule()->MakeWindow(new Process(GetApplicationObject()->MainWindow));
    SetDlgItemText(findWnd->HWindow, 377, "Импорт");

    FILE* ImportLog = 0;
    {
        DynStr szModuleName;
		GetModuleFileName(GetApplicationObject()->hInstance, szModuleName, szModuleName._sizeof()-32);
        *strrchr(szModuleName, '\\') = 0;
	    strcat(szModuleName, "\\IMPORT\\implog.txt");
		ImportLog = fopen(szModuleName, "a+");
    }

	if(ImportLog == 0)
	{
	    MessageBox(GetApplicationObject()->MainWindow->HWindow, GetRCStr(579), "Ошибка", MB_OK | MB_ICONSTOP);
        fclose(f);
	    delete findWnd;
	    return;
    }
	else
	{
	    fprintf(ImportLog, "ИМПОРТ ------\n\n\n");
    }        

    TRACE((char*)0);
    //long CountPolises = 0;


    //DEBUG DEBUG DEBUG

    //char Maska[32] = "";
    ///if(GetApplicationObject()->ExecDialog(new InputStringDlg(GetApplicationObject()->MainWindow, "До какого номера работать", "Номер полиса", "", Maska)) != IDOK) return;
    ///long IgnoreNumber = atol(Maska);
    ///int AskRes = MessageBox(0, "Что делать?", "До (Yes) Кроме (No) Только его (Cancel)?", MB_YESNOCANCEL);
    //int WasPolis = 0;
    //DEBUG DEBUG DEBUG


	{ //start block
    PXSave();
	//Import polises (IN BLOCK)
	TRACE("TRY OPEN TABLES");

	TOpenBase temp_I(Komplex ? "komplex" : "mandator", MandatoryFields, Komplex ? KomplexCnt : MandatoryCnt);
	if(I.pxErr) {
	   fclose(f);
	   fclose(ImportLog);
	   delete findWnd;
	   return;
	}


	for(int nnn = 0;; nnn++)
	{
		findWnd->SetPercent(Pos++ * 100 / AllPolises);

		PXRecBufEmpty(I.getRecHandle());
	    PXRecBufEmpty(temp_I.getRecHandle());

	    char Priznak;
	    fread(&Priznak, 1, 1, f);
	    if(Priznak == '.') break;

		aux_read_fields_from_file(f, temp_I, VerLetter, Komplex);

		//ignore mand_txt fields
		if(VerLetter < 'F')
		{
			RestoreFieldValue(f, I, 0);
  			RestoreFieldValue(f, I, 0);
			RestoreFieldValue(f, I, 0);
		}
		PXRecBufCopy(temp_I.getRecHandle(), I.getRecHandle()); //Копируем буфер данных

		char* NewAgCode = (char*)GetMyAgCode(temp_I.GetString(30).s, AgentsCodes, NewAgentsCodes, 100);
		if(ValidateInternalImport && !_IsValidPolisNumber(MANDATORY_SECT, temp_I.GetString(0), temp_I.GetDouble(1), 0, NewAgCode))
		{
			DynStr errMsg;
			sprintf(errMsg, GetRCStr(709), temp_I.GetString(0).s, (long)temp_I.GetDouble(1), NewAgCode);
			MessageBox(GetApplicationObject()->MainWindow->HWindow, errMsg, "!", MB_OK | MB_ICONSTOP);
			DataIsOk = 0;
			break;
		}

		int ExistRec = PXSrchKey(I.getTblHandle(), I.getRecHandle(), 2, SEARCHFIRST) == PXSUCCESS;
		if(ExistRec)
		{
			TRACE("GET POLIS ");
			I.Get();

			char S[10];
			double N;
			I(S, sizeof S, 0);
			I(N, 1);
			char Data1_1[48];
			char Data2_1[48];
			I(Data1_1, sizeof Data1_1, 3);
			temp_I(Data2_1, sizeof Data1_1, 3);

			char agCodeFile[10];
			temp_I(agCodeFile, sizeof agCodeFile, 30); //Взяли агента из временной таблицы, куда закачен полис из файла

			double TarifBD, TarifFL,  SuperBD, SuperFL;
			char IsBadBD;
			char IsBadFL;
			char agCodeBase[10];
			char* ErrMsg;
			char Data1_2[48];
			char Data1_3[48];
			char Data2_2[48];
			char Data2_3[48];
			int CodeForITxt;
			int Result;
			DATE updDateTab, updDateFile;

			//Перекодируем 2го агента
			temp_I(agCodeBase, sizeof agCodeBase, 28);
			if(temp_I.GetDouble(35) == 0)
				temp_I(28, "");
			else
				temp_I(28, (char*)GetMyAgCode(agCodeBase, AgentsCodes, NewAgentsCodes, 4));

			if(!*agCodeFile) //Чужой!!!
			{
				CopyFieldValue(temp_I, I, 21);
				CopyFieldValue(temp_I, I, 28);
				CopyFieldValue(temp_I, I, 35);
				CopyFieldValue(temp_I, I, 36);
				CopyFieldValue(temp_I, I, 37);
				CopyFieldValue(temp_I, I, 38);
				CopyFieldValue(temp_I, I, 39);
				CopyFieldValue(temp_I, I, 40);
				CopyFieldValue(temp_I, I, 43);
				CopyFieldValue(temp_I, I, 63);
				CopyFieldValue(temp_I, I, 68);
				CopyFieldValue(temp_I, I, 69);
			}
			else
			{
				//Испорчен - нет
				I(TarifBD, 17);
				temp_I(TarifFL, 17);
				I(SuperBD, 32);
				temp_I(SuperFL, 32);

				IsBadBD = fabs(SuperBD - (-1)) <= 0.001;
				IsBadFL = fabs(SuperFL - (-1)) <= 0.001;

				if((IsBadBD + IsBadFL) == 1)
				{
					TRACE("ASK1");
					sprintf(DATA_BUFFER,
						GetRCStr(580),
						S, (long)N, Data1_1, Data2_1);
					fprintf(ImportLog, DATA_BUFFER);
					fprintf(ImportLog, "\n\n");
					if(MessageBox(GetApplicationObject()->MainWindow->HWindow, DATA_BUFFER, "Ошибка", MB_YESNO | MB_ICONSTOP) == IDNO) {
						DataIsOk = 0;
						break;
					}
					else {
						continue; //Продолжаем
					}
				}

				if(fabs(TarifBD - TarifFL) > 0.0009)
				{
					TRACE("ASK2");
					sprintf(DATA_BUFFER,
					//"Полис %s/%lu в Базе Данных тариф(%g, %g), а в файле (%g, %g). Пропустить?",
							GetRCStr(581),
							S, (long)N, TarifBD, TarifFL);
					fprintf(ImportLog, DATA_BUFFER);
					fprintf(ImportLog, "\n\n");
					if(MessageBox(GetApplicationObject()->MainWindow->HWindow, DATA_BUFFER, "Ошибка", MB_YESNO | MB_ICONSTOP) == IDNO) {
						DataIsOk = 0;
						break;
					}
					else {
						continue; //Продолжаем
					}
				}

				//check agent code
				//проверить совпадает ли перекодированный агент из файла с агентом из БД
				I(agCodeBase, sizeof agCodeBase, 30); //Взяли агента из текущей БД

				if(*agCodeFile && *agCodeBase && strcmp(agCodeBase, GetMyAgCode(agCodeFile, AgentsCodes, NewAgentsCodes, 1)))
				{
					I(Data1_2, sizeof Data1_1, 4);
					I(Data1_3, sizeof Data1_1, 9);
					temp_I(Data2_2, sizeof Data1_1, 4);
					temp_I(Data2_3, sizeof Data1_1, 9);

					ErrMsg = new char[1024];

					TRACE("ASK3");
					sprintf(ErrMsg,
						GetRCStr(582),
						S, (long)N, Data1_1, Data1_2, Data1_3, agCodeBase,
									Data2_1, Data2_2, Data2_3, GetMyAgCode(agCodeFile, AgentsCodes, NewAgentsCodes, 2));

					fprintf(ImportLog, ErrMsg);
					fprintf(ImportLog, "\n\n");
					Result = MessageBox(GetApplicationObject()->MainWindow->HWindow, ErrMsg, "Ошибка", MB_YESNOCANCEL | MB_ICONSTOP);

					delete ErrMsg;
					if(Result == IDYES) //Стоп
					{
					   DataIsOk = 0;
					   break;
					}
					else
					if(Result == IDNO) //Old Agent
					{
						temp_I(30, agCodeBase);
					}
					else
					if(Result == IDCANCEL) //Owerwrite
					{
						temp_I(30, (char*)GetMyAgCode(agCodeFile, AgentsCodes, NewAgentsCodes, 3));
					}
				}
				else
                {
					CopyFieldValue(I, temp_I, 30); //первый агент в базе пуст
				}
				//end check

				I(updDateTab, 60);
				temp_I(updDateFile, 60);
				//Синхронизация кодов

				//Сброс даты
				I(60, CurrDate);
				temp_I(60, CurrDate);

				DynStr buffer_tab(1024 * 5);
				DynStr buffer_file(1024 * 5);
				memset(buffer_tab, 0, 1024 * 5);
				memset(buffer_file, 0, 1024 * 5);

				assert(PXRawGet(I.getRecHandle(), buffer_tab, 1024 * 5) == PXSUCCESS);
				assert(PXRawGet(temp_I.getRecHandle(), buffer_file, 1024 * 5) == PXSUCCESS);

				TRACE("COMPARE");

				if(IgnoreNames)
				{
					for(int i1 = 0; i1 < sizeof(ignoreFieldsList)/sizeof(*ignoreFieldsList); i1++)
						CopyFieldValue(I, temp_I, ignoreFieldsList[i1]);
				}

				if(memcmp(buffer_tab, buffer_file, 1024 * 5))
				{
					PXRecBufCopy(temp_I.getRecHandle(), I.getRecHandle()); //Копируем буфер данных
					TRACE("UPD TBL");
				}
				else
				{
					Ignored++;
					TRACE("PL EQ");
                    continue;
				}
			} //full polis data - not external

			UpdatedPolises++;
			//Обновим данные
			if((CodeForITxt=PXRecUpdate(I.getTblHandle(), I.getRecHandle())) != PXSUCCESS)
			{
				MessageBox(GetApplicationObject()->MainWindow->HWindow, GetRCStr(549), "Ошибка", MB_OK | MB_ICONSTOP);
				MessageBox(GetApplicationObject()->MainWindow->HWindow, PXErrMsg(CodeForITxt), "Ошибка", MB_OK | MB_ICONSTOP);
				DataIsOk = 0;
				break;
			}
		}
 	    else {
			TRACE("ADD POLIS ");
			TRACE(_SS);
			//Не найден - добавить (с очиткой кодов на всякий случай)
			//PXPutBlank(I.getRecHandle(), 61);
			//PXPutBlank(I.getRecHandle(), 62);
			//PXPutBlank(I.getRecHandle(), 63);
					I(61, 0.);
	//		I(62, 0.);
	//		I(63, 0.);

			char agCode[10] = "";
			I(agCode, sizeof agCode, 30);
			I(30, (char*)GetMyAgCode(agCode, AgentsCodes, NewAgentsCodes, 5));

			////////////////////////////////////////////////////////////////////////////////

			I(agCode, sizeof agCode, 28);

			if(I.GetDouble(35) == 0) //ispay2
				I(28, "");
			else
				I(28, (char*)GetMyAgCode(agCode, AgentsCodes, NewAgentsCodes, 6));

			I(60, CurrDate);
			int AddCode1 = I.AddRecord();
			TRACE("ADD POLIS TXT");
			TRACE(_SS);
			//int AddCode2 = temp_I_txt.AddRecord();
			if(AddCode1 != PXSUCCESS/* || AddCode2 != PXSUCCESS*/) {
				TRACE(PXErrMsg(AddCode1));
				//TRACE(PXErrMsg(AddCode2));
				MessageBox(GetApplicationObject()->MainWindow->HWindow, GetRCStr(550), "Ошибка", MB_OK | MB_ICONSTOP);
				MessageBox(GetApplicationObject()->MainWindow->HWindow, PXErrMsg(AddCode1), "Ошибка", MB_OK | MB_ICONSTOP);
				//MessageBox(GetApplicationObject()->MainWindow->HWindow, PXErrMsg(AddCode2), "Ошибка", MB_OK | MB_ICONSTOP);
				DataIsOk = 0;
						break;
			}
			TRACE("ADDED");
			AddedPolises++;
			findWnd->SetAll(AddedPolises);
		}
	} //end for
    } //end block


    if(DataIsOk)
    {
		char Priznak;
		fread(&Priznak, 1, 1, f); //read point
/*
		TRACE("_OWNERS_");
	//Import Owners (IN BLOCK)
	//Проблемы при импорте владельцев Если поменяли ФИО (в Reporter32) или добавили-удалили то капцы
/	TOpenBase temp_Own("\\bases\\mandown", OwnerFields, 17);
	if(temp_Own.pxErr) {
	   fclose(ImportLog);
	   fclose(f);
	   delete findWnd;
	   return;
	}

        char szModuleName[128];
		GetModuleFileName(GetApplicationObject()->hInstance, szModuleName, sizeof(szModuleName)-32);
        *strrchr(szModuleName, '\\') = 0;
     	strcat(szModuleName, "\\IMPORT\\imp_owns.txt");

        date d;
        getdate(&d);

	FILE* f_new_own = fopen(szModuleName, "at");
	if(!f_new_own) {
	    MessageBox(GetApplicationObject()->MainWindow->HWindow, GetRCStr(579), "Ошибка", MB_OK | MB_ICONSTOP);
	    fclose(f);
 	    fclose(ImportLog);
	    delete findWnd;
	    return;
	}
	fprintf(f_new_own, GetRCStr(583), d.da_day, d.da_mon, d.da_year);

	for(;;) {
	    findWnd->SetPercent(Pos++ * 100 / AllPolises);

	    PXRecBufEmpty(Own.getRecHandle());
	    PXRecBufEmpty(temp_Own.getRecHandle());

	    char Priznak;
	    fread(&Priznak, 1, 1, f);
		if(Priznak == '.') break;
	    aux_read_owner_from_file(f, temp_Own, Version);

	    PXRecBufCopy(temp_Own.getRecHandle(), Own.getRecHandle()); //Копируем буфер данных
	    int ExistRec = PXSrchKey(Own.getTblHandle(), Own.getRecHandle(), 3, SEARCHFIRST) == PXSUCCESS; //PK 3 field
	    if(ExistRec) {
		Own.Get();

		DATE updDateTab, updDateFile;
		Own(updDateTab, 3);
		temp_Own(updDateFile, 3);
		if(0) {
                    IgnoredDate++;
		}
		else {
		    //ОБНОВЛЕНИЕ ДАННЫХ
                    //"OwnerCode"     //12

		    //Синхронизация кодов
		    CopyFieldValue(Own, temp_Own, 12);
		    //double Val;
		    //I(Val, 61);
			//temp_I(61, Val);
		    //I(Val, 62);
			//temp_I(62, Val);
		    //I(Val, 63);
		    //temp_I(63, Val);

		    Own(3, CurrDate);
		    temp_Own(3, CurrDate);

			DynStr buffer_tab(1024 * 5);
			DynStr buffer_file(1024 * 5);
			//char* buffer_tab = new char[1024 * 5];
		    //char* buffer_file = new char[1024 * 5];
		    memset(buffer_tab, 0, 1024 * 5);
		    memset(buffer_file, 0, 1024 * 5);

		    assert(PXRawGet(Own.getRecHandle(), buffer_tab, 1024 * 5) == PXSUCCESS);
		    assert(PXRawGet(temp_Own.getRecHandle(), buffer_file, 1024 * 5) == PXSUCCESS);

		    if(memcmp(buffer_tab, buffer_file, 1024 * 5)) {
			//Буфера отличаются
			//СБРОС ВСЕХ КОДОВ

			PXRecBufCopy(temp_Own.getRecHandle(), Own.getRecHandle()); //Копируем буфер данных
			Own(12, 0.);

			int ErrCode = PXRecUpdate(Own.getTblHandle(), Own.getRecHandle());
			if(ErrCode != PXSUCCESS) {
				MessageBox(GetApplicationObject()->MainWindow->HWindow, GetRCStr(517), PXErrMsg(ErrCode), MB_OK | MB_ICONSTOP);
			    DataIsOk = 0;
                            break;
			}

			UpdatedOwners++;
		    }
		    else
			Ignored++;

		    //delete buffer_tab;
		    //delete buffer_file;
		}
	    }
	    else {
		//Не найден - добавить (с очиткой кодов на всякий случай)
		//Проверить есть ли полис
		char Seria[10], Name[50];
		double Number;
        Own(Seria, sizeof Seria, 0);
		Own(Number, 1);
		Own(Name, sizeof Name, 2);
		I(0, Seria);
		I(1, Number);
                int IsIgnore = 0;
		if(PXSrchKey(I.getTblHandle(), I.getRecHandle(), 2, SEARCHFIRST) != PXSUCCESS) {
		    sprintf(DATA_BUFFER, GetRCStr(567), Seria, (long)Number);
		    fprintf(ImportLog, DATA_BUFFER);
		    fprintf(ImportLog, "\n\n");
		    if(MessageBox(GetApplicationObject()->MainWindow->HWindow, DATA_BUFFER, "Ошибка", MB_YESNO | MB_ICONQUESTION) != IDYES) {
		        DataIsOk = 0;
			break;
		    }
		    else 
			IsIgnore = 1;
		}

		if(!IsIgnore) {
		    //Если несколько владельцев, то записать для проверки
		    if(PXSrchKey(Own.getTblHandle(), Own.getRecHandle(), 2, SEARCHFIRST) == PXSUCCESS) {
			char strNmb[24];
			DblToStr(strNmb, Number, 0);
			fprintf(f_new_own, "%s/%s %s\n", Seria, strNmb, Name);
		    }

		    Own(12, 0.); //Сброс оwner_code
    		    Own(3, CurrDate);

		    if(Own.AddRecord() != PXSUCCESS) {
			MessageBox(GetApplicationObject()->MainWindow->HWindow, "Не могу вставить данные в таблицу владельцев", "Ошибка", MB_OK | MB_ICONSTOP);
			DataIsOk = 0;
			break;
		    }
		    AddedOwners++;
		}
                else
		    fprintf(f_new_own, "Пропущен владелец %s/%lu %s\n", Seria, (long)Number, Name);
		findWnd->SetAll(AddedOwners + AddedPolises);
	    }
	 } //end for

	 fprintf(f_new_own, "Конец блока\n\n\n");
	 fclose(f_new_own);*/
    } //end block

    if(DataIsOk)
	{
	if(!Komplex)
    {
	TRACE("AVARIAS_");
	//Import Avarias (IN BLOCK)
	//Проблемы при импорте аварий - добавили-удалили то капцы - потеря номера
	TOpenBase temp_Av2("\\bases\\mandav2", Avaria2Fields, 71);
	if(temp_Av2.pxErr) {
	   fclose(f);
	   fclose(ImportLog);
	   delete findWnd;
	   return;
	}

        char szModuleName[128];
        GetModuleFileName(GetApplicationObject()->hInstance, szModuleName, sizeof(szModuleName)-32);
        *strrchr(szModuleName, '\\') = 0;
	strcat(szModuleName, "\\IMPORT\\imp_avar.txt");

        date d;
        getdate(&d);

	FILE* f_new_av = fopen(szModuleName, "at");
	if(!f_new_av) {
	    MessageBox(GetApplicationObject()->MainWindow->HWindow, GetRCStr(579), "Ошибка", MB_OK | MB_ICONSTOP);
	    fclose(f);
	    fclose(ImportLog);
	    delete findWnd;
	    return;
	}
	fprintf(f_new_av, GetRCStr(584), d.da_day, d.da_mon, d.da_year);

	for(;;) {
	    findWnd->SetPercent(Pos++ * 100 / AllPolises);

	    PXRecBufEmpty(Av2.getRecHandle());
	    PXRecBufEmpty(temp_Av2.getRecHandle());

	    char Priznak;
	    fread(&Priznak, 1, 1, f);
		if(Priznak == '.') break;
		aux_read_avaria_from_file(f, temp_Av2, &AvPaysTable, 0, Version - '0');

	    PXRecBufCopy(temp_Av2.getRecHandle(), Av2.getRecHandle()); //Копируем буфер данных
	    int ExistRec = PXSrchKey(Av2.getTblHandle(), Av2.getRecHandle(), 3, SEARCHFIRST) == PXSUCCESS; //PK 3 field
	    if(ExistRec) {
		Av2.Get();

		DATE updDateTab, updDateFile;
		Av2(updDateTab, 37);
		temp_Av2(updDateFile, 37);
		if(0) {
					IgnoredDate++;
		}
		else {
		    //ОБНОВЛЕНИЕ ДАННЫХ
		    //"OwnerCode_P",   //48
		    //"BaseCarCode_P", //49
		    //"CarCode_P",     //50
		    //"PaymentCode"    //51

		    //Синхронизация кодов

		    CopyFieldValue(Av2, temp_Av2, 48);
		    CopyFieldValue(Av2, temp_Av2, 49);
		    CopyFieldValue(Av2, temp_Av2, 50);
		    CopyFieldValue(Av2, temp_Av2, 51);

		    Av2(37, CurrDate);
		    temp_Av2(37, CurrDate);

			DynStr buffer_tab(1024 * 5);
			DynStr buffer_file(1024 * 5);
			//char* buffer_tab = new char[1024 * 5];
		    //char* buffer_file = new char[1024 * 5];
		    memset(buffer_tab, 0, 1024 * 5);
		    memset(buffer_file, 0, 1024 * 5);

		    assert(PXRawGet(Av2.getRecHandle(), buffer_tab, 1024 * 5) == PXSUCCESS);
		    assert(PXRawGet(temp_Av2.getRecHandle(), buffer_file, 1024 * 5) == PXSUCCESS);

		    if(memcmp(buffer_tab, buffer_file, 1024 * 5)) {
			//Буфера отличаются
			//СБРОС ВСЕХ КОДОВ

			PXRecBufCopy(temp_Av2.getRecHandle(), Av2.getRecHandle()); //Копируем буфер данных
			Av2(48, 0.);
			Av2(49, 0.);
			Av2(50, 0.);
			Av2(51, 0.);

			int ErrCode = PXRecUpdate(Av2.getTblHandle(), Av2.getRecHandle());
			if(ErrCode != PXSUCCESS) {
			    MessageBox(GetApplicationObject()->MainWindow->HWindow, "Не могу обновить данные в таблице аварий", PXErrMsg(ErrCode), MB_OK | MB_ICONSTOP);
			    DataIsOk = 0;
                            break;
			}

			UpdatedAvarias++;
		    }
		    else
			Ignored++;

		    //delete buffer_tab;
			//delete buffer_file;
		}
	    }
	    else {
		//Не найден - добавить (с очиткой кодов на всякий случай)
		//Проверить есть ли полис
		char Seria[10];//, Name[50];
		double Number;
		Av2(Seria, sizeof Seria, 0);
		Av2(Number, 1);
		//Own(Name, sizeof Name, 2);
		I(0, Seria);
		I(1, Number);
		if(PXSrchKey(I.getTblHandle(), I.getRecHandle(), 2, SEARCHFIRST) != PXSUCCESS) {
		    sprintf(DATA_BUFFER, GetRCStr(566), Seria, (long)Number);
		    fprintf(ImportLog, DATA_BUFFER);
		    fprintf(ImportLog, "\n\n");
		    MessageBox(GetApplicationObject()->MainWindow->HWindow, DATA_BUFFER, "Ошибка", MB_OK | MB_ICONSTOP);
		    DataIsOk = 0;
                    break;
		}

		//Если несколько владельцев, то записать для проверки
		if(PXSrchKey(Av2.getTblHandle(), Av2.getRecHandle(), 2, SEARCHFIRST) == PXSUCCESS) {
		    char strNmb[24];
		    DblToStr(strNmb, Number, 0);
		    fprintf(f_new_av, "%s/%s\n", Seria, strNmb);
                }

		Av2(48, 0.);
		Av2(49, 0.);
		Av2(50, 0.);
		Av2(51, 0.);
 	        Av2(37, CurrDate);

		if(Av2.AddRecord() != PXSUCCESS) {
		    MessageBox(GetApplicationObject()->MainWindow->HWindow, GetRCStr(590), "Ошибка", MB_OK | MB_ICONSTOP);
		    DataIsOk = 0;
                    break;
		}
		AddedAvarias++;
		findWnd->SetAll(AddedAvarias + AddedPolises + AddedOwners);
	    }
	 } //end for

	 fprintf(f_new_av, "Конец блока\n\n\n");
	 fclose(f_new_av);
	 }
	 else
	 {
		char Priznak;
	    fread(&Priznak, 1, 1, f);
	}
    } //end block



	restoredTaxi = 0;
	if (istaxi) {
	if(Komplex)
	{
		char Priznak;
	    fread(&Priznak, 1, 1, f);
	}
	else
    {
	   for(;DataIsOk;NumberBad++) {
		   char Priznak;
		   fread(&Priznak, 1, 1, f);
		   if(Priznak == '.') break;
		   if(Priznak != '+') {
			   DataIsOk = 0;
			   break;
		   }

		   for(int i = 0; i < 9; i++)
				RestoreFieldValue(f, TAXI0, i);

			int ExistRec = PXSrchKey(TAXI0.getTblHandle(), TAXI0.getRecHandle(), 2, SEARCHFIRST) == PXSUCCESS; //PK 3 field
		if(ExistRec) {
			TAXI0.UpdateRecord();
		}
		else {
			TAXI0.AddRecord();
		}
		restoredTaxi++;

		   if(feof(f)) {
			   DataIsOk = 0;
			   break;
		   }
	   }
	   }
	}

    fclose(ImportLog);
    fclose(f);
    if(!DataIsOk) {
       //MessageBox(GetApplicationObject()->MainWindow->HWindow, "", "Ошибка", MB_OK | MB_ICONSTOP);
	   delete findWnd;
       return;
    }

    delete findWnd;

    TRACE("FINISH");

    sprintf(DATA_BUFFER,  GetRCStr(565),
	    //"Импорт файла прошёл успешно\n"
	    //"Добавлено: полисов (%ld), владельцев(%ld), аварий (%ld)\n"
	    //"Обновлено: полисов (%ld), владельцев(%ld), аварий (%ld)\n"
	    //"Записей  : пропущено (%ld), пропущено старых (%ld)",
	    AddedPolises, AddedOwners, AddedAvarias,
	    UpdatedPolises, UpdatedOwners, UpdatedAvarias,
		Ignored, IgnoredDate);
	if(istaxi)
	{
		sprintf(DATA_BUFFER+strlen(DATA_BUFFER), "\nДопалат %lu", restoredTaxi);
    }

	//assert(strlen(DATA_BUFFER) < sizeof(DATA_BUFFER));

    MessageBox(GetApplicationObject()->MainWindow->HWindow, DATA_BUFFER, "Мои поздравления", MB_OK | MB_ICONINFORMATION);
}

double GetRateCurrencyFull(DATE payDate, char* CurrencyField, DATE& foundDate)
{
    if(!strncmp(CurrencyField, "BRB", 3))
		return 1;

   char* ShortFields[] = { "RateDate", CurrencyField };
    TOpenBase I("\\bases\\rates", ShortFields, 2);
	if(I.pxErr) {
		return -I.pxErr;
    }
    I(0, payDate);
    if(PXSrchFld(I.getTblHandle(), I.getRecHandle(), I.getField(0), SEARCHFIRST) != PXSUCCESS) {
		int Result = PXSrchFld(I.getTblHandle(), I.getRecHandle(), I.getField(0), CLOSESTRECORD);
		if(Result == PXERR_ENDOFTABLE)
		I.Last();
		if(Result != PXSUCCESS)
            I.Prev();
    }

    I.Get();

    double Currency;
    I(Currency, 1);
    I(foundDate, 0);

    return Currency;
}

double GetRateCurrency(DATE payDate, char* CurrencyField)
{
	return GetRateCurrencyFull(payDate, CurrencyField, payDate);
}

void MandatoryDlg::SetCurrencyForPay(int PayNmb)
{
    static IsError = 0;
    if(IsError) return;

    char strPayDate[32];
    //Дата платежа
    ((TEdit*)ChildWithId(PayNmb == 1 ? 121 : 127))->GetText(strPayDate, sizeof strPayDate);
    DATE payDate = GD(strPayDate, 1);
    if(!payDate) return;

	char* CurrencyField = Nal->GetCheck() == 1 ? "EUR_NAL" : "EUR_BNL";

	double Currency = GetRateCurrency(payDate, CurrencyField);
	if(Currency <= 0) {
		IsError = -Currency;
		EnableWindow(GetDlgItem(HWindow, 60), FALSE);
		return;
    }

/*    char* ShortFields[] = { "RateDate", CurrencyField };
    TOpenBase I("\\bases\\rates", ShortFields, 2);
    if(IsError = I.pxErr) {
		EnableWindow(GetDlgItem(HWindow, 60), FALSE);
		return;
    }
    I(0, payDate);
    if(PXSrchFld(I.getTblHandle(), I.getRecHandle(), I.getField(0), SEARCHFIRST) != PXSUCCESS) {
	int Result = PXSrchFld(I.getTblHandle(), I.getRecHandle(), I.getField(0), CLOSESTRECORD);
	if(Result == PXERR_ENDOFTABLE)
	    I.Last();
	if(Result != PXSUCCESS)
            I.Prev();
    }

    I.Get();

	double Currency;
    I(Currency, 1);
*/
    if(Currency > 0.01) {
	 sprintf(strPayDate, "%g", Currency);
	((TEdit*)ChildWithId(PayNmb == 1 ? 115 : 128))->SetText(strPayDate);
    }
    else
	 MessageBeep(0);
}

void MandatoryDlg::StopPay(RTMessage)
{
    if (IsWindowEnabled(GetDlgItem(HWindow, 406))) {
	MessageBox(HWindow, GetRCStr(512), "Ошибка", MB_ICONINFORMATION);
	return;
    }
    if(MessageBox(HWindow, GetRCStr(511), "Отвечай", MB_ICONQUESTION | MB_YESNO) == IDNO) return;

     char* ShortFields[] = { "Seria", "Number", "State", "UpdateDate", "StopDate" };
     TOpenBase I(base->GetTableName(), ShortFields, 5);
     if(I.pxErr) {
	Err:MessageBox(HWindow, "Ошибка обновления полиса!!!", "Ошибка", MB_OK | MB_ICONSTOP);
	return;
     }
     I(0, buffer.Seria);
     I(1, atof(buffer.Number));
     if(PXSrchKey(I.getTblHandle(), I.getRecHandle(), 2, SEARCHFIRST) == PXSUCCESS) {
	 I.Get();
	 I(2, (double)STATE_POLIS_STOPPAY);
	 DATE lost_date;
	 struct date d;
	 getdate(&d);
	 PXDateEncode(d.da_mon, d.da_day, d.da_year, &lost_date);
	 I(3, lost_date);
	 I(4, lost_date);
	 if(PXRecUpdate(I.getTblHandle(), I.getRecHandle()) != PXSUCCESS)
	    goto Err;
     }
     else {
	MessageBox(HWindow, GetRCStr(541), "Ошибка", MB_OK | MB_ICONSTOP);
	return;
     }

    EnableWindow(GetDlgItem(HWindow, 58), 0);
    SetState(STATE_POLIS_STOPPAY);
}

void MandatoryDlg::GetDublicat(RTMessage)
{
   if(m_State == 3) {
         SetCursor(LoadCursor(0, IDC_WAIT));
	 char* ShortFields[] = { "Seria", "Number", "PrevSeria", "PrevNumber" };
	 TOpenBase I(base->GetTableName(), ShortFields, 4);
	 if(I.pxErr) {
	    //MessageBox(HWindow, "Ошибка обновления утерянного полиса!!!", "Ошибка", MB_OK | MB_ICONSTOP);
	    return;
	 }
	 char Seria[5];
	 char DbSeria[5];
	 char Number[16];
	 GetDlgItemText(HWindow, 50, Seria, sizeof Seria);
	 GetDlgItemText(HWindow, 51, Number, sizeof Number);
	 //	 MessageBox(HWindow, "start", Number, MB_OK | MB_ICONSTOP);
	 I(3, atof(Number));
         int Find = 0;
	 if(PXSrchFld(I.getTblHandle(), I.getRecHandle(), I.getField(3), SEARCHFIRST) == PXSUCCESS)
	 {
	     do {
		 I.Get();
		 I(DbSeria, sizeof DbSeria, 2);
	         //MessageBox(HWindow, "First", DbSeria, MB_OK | MB_ICONSTOP);
		 if(!strcmp(Seria, DbSeria)) {
                     Find = 1;
		     break;
		 }
		 if(PXSrchFld(I.getTblHandle(), I.getRecHandle(), I.getField(3), SEARCHNEXT) != PXSUCCESS)
                     break;
	     }
	     while(1);
	 }

	 if(Find)
	 {
	    char Title[96];
	    GetWindowText(HWindow, Title, sizeof Title);
	//MessageBox(HWindow, Title, "fnd", MB_OK | MB_ICONSTOP);
	    if(Title[strlen(Title) - 1] != ' ') {
		double N;
		I(N, 1);
		sprintf(Title + strlen(Title), " Дубликат %s/%lu ", DbSeria, (long)N);
		SetWindowText(HWindow, Title);
	    }
	 }
   }
}


void MandatoryDlg::Print2()
{
/*   if(!SetDataToBase()) return;
   Dlg_base::SetDataToBase();

   char* str = new char [3000];
   memset(str, 0, 3000);

   char* dest = str + 38;

   for(int i = 0; i < ownersCount; i++) 
       AddOwnerToBuffer(dest, i, 1);

   ((TRulerWin*)Parent)->SetValues(str);
   Dlg_base::Print2();
   delete str;*/
}

int MandatoryDlg::CanDeleteRecord()
{
    char buff[32];
    GetPrivateProfileString(MANDATORY_SECT, "CanDeletePolises", "0", buff, sizeof buff, ININame);
    if(!strcmp(buff, "0")) {
	MessageBox(HWindow, GetRCStr(510), "Сообщаю", MB_ICONINFORMATION);
	return 0;
    }
    else
    if(!strcmp(buff, "1"))
	return 1;
    else {
	if(GetModule()->ExecDialog(new InputStringDlg(this, "Пароль", GetRCStr(564), buff, 0)) == IDOK) {
	   return 1;
	}
    }
    return 0;
}

int MandatoryDlg::FindTransport()
{
   char b[100];
   if(e[10+2]->GetText(b, sizeof b)) {
       (*base)(10, b);
       if(!PXSrchFld(base->getTblHandle(), base->getRecHandle(), base->getField(10), SEARCHFIRST))
	{
           base->Get();
	   GetDataFromBase();
           return 1;
	}
   }
   return 0;
}

int MandatoryDlg::FindTransportLike()
{
   char b[100];
   if(e[10+2]->GetText(b, sizeof b))
       return FindLike(0, "AutoNumber", b, 0);
   return 0;
}

void MandatoryDlg::FindMobTel(RTMessage)
{
   char b[100];
   if(m_MobilePhone->GetText(b, sizeof b))
       FindLike(0, "MobTel", b, 0);
}

void MandatoryDlg::FindHomeTel(RTMessage)
{
   char b[100];
   if(m_HomePhone->GetText(b, sizeof b))
       FindLike(0, "HomeTel", b, 0);
}

/*int Is_FindMode_Srv()
{
   char buffer[16];
   GetPrivateProfileString(MANDATORY_SECT, "FindMode", "", buffer, sizeof buffer, ININame);
   return !strcmpi(buffer, "ONSERVER");
}
  */
int auxFindLike(const char* TableName, char* FldName, char* Maska, Array& Strings, TDialog* Dlg, char* ResStr, int IsDigit, char** CheckFld)
{
   int Len = strlen(Maska);
   while(*Maska == ' ') {
      memmove(Maska, Maska + 1, Len);
      Len--;
   }

   while(Len && Maska[Len - 1] == ' ') {
      Maska[Len - 1] = 0;
      Len--;
   }

    {
	   double Number;
	   double Value;
	   double sValue = atof(Maska);

	   char seria[24], part[24];
       DynStr buff, sfield;
       DATE RegDate;
	   { //Find Block
	    AnsiUpper(Maska);
		//int maskaLen = strlen(Maska);

		char* DtFld = strstr(TableName, "AV") != 0 ? "WrDate" : (strstr(TableName, "RUSS") != 0 || strstr(TableName, "PRIV") != 0) ? "RegDt" : "RegDate";
		char* ShortFields[] = { "Seria", "Number", DtFld, FldName, CheckFld ? CheckFld[0] : 0 };
		TOpenBase find_tab(TableName, ShortFields, CheckFld ? 5 : 4);
	    if(find_tab.pxErr) return 0;
	    find_tab.Refresh();

		//double number;
	    Process* findWnd = find_tab.GetRecords() > 50 ? (Process*)GetApplicationObject()->MakeWindow(new Process(Dlg)) : 0;
	    SetCursor(LoadCursor(0, IDC_WAIT));
	    long AllRecords = find_tab.GetRecords();
	    find_tab.First();

		char sDt[15];
	    for(long Pos = 0; Pos < AllRecords; Pos++) {
		    if(!find_tab.Get()) {
		        MessageBox(Dlg->HWindow, "Поиск остановлен по ошибке (Get)", "Сообщаю", MB_OK | MB_ICONINFORMATION);
                break;
		    }

			*part = 0;
			if(CheckFld) {
				find_tab(part, sizeof part, 4);
				if(!CheckFld[1]) {
					//strcat(str, " ");
					//strcpy(part, seria);
				}
				else
				if(!strcmp(part, CheckFld[1]))
					strcpy(part, CheckFld[2]);
			}


			DateFBToStr(&find_tab, 2, sDt);

			if(IsDigit) {
				find_tab(Value, 3);
				if(fabs(Value - sValue) < 0.0001)
				{
					find_tab(seria, sizeof seria, 0);
					find_tab(Number, 1);
					if(Value == long(Value))
						wsprintf(buff, "%s/%-8lu   %s   %lu", seria, (long)Number, sDt, (long)Value);
					else
						sprintf(buff, "%s/%-8lu   %s   %lf", seria, (long)Number, sDt, Value);

					if(*part) {
						strcat(buff, " ");
						strcat(buff, part);
					}

					Strings.add(*new String(buff));
					if(findWnd) findWnd->SetAll(Strings.getItemsInContainer());
				}
			}
            else {
				find_tab(sfield, sfield._sizeof(), 3);
				AnsiUpper(sfield);
				if(strstr(sfield, Maska) != NULL)
				{
					find_tab(seria, sizeof seria, 0);
					find_tab(Number, 1);
					find_tab(sfield, sfield._sizeof(), 3); //reread for char case
					wsprintf(buff, "%s/%-8lu   %s   %s", seria, (long)Number, sDt, sfield);
					if(*part) {
					    strcat(buff, " ");
						strcat(buff, part);
					}
					Strings.add(*new String(buff));
					if(findWnd) findWnd->SetAll(Strings.getItemsInContainer());
				}
			}

			find_tab.Next();
		    if(findWnd) findWnd->SetPercent(Pos * 100 / AllRecords);

			if(Strings.getItemsInContainer() > 10000) {
				 MessageBox(Dlg->HWindow, "Слишком много найдено.", "Сообщаю", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
	    		 return -1;
			}

		    if(GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
		        while(GetAsyncKeyState(VK_ESCAPE) & 0x8000);
		        break;
            }
	    }
	    if(findWnd) delete findWnd;
       } //End Find Block

       int nCount = Strings.getItemsInContainer();
       if(nCount == 0) {
	     MessageBox(Dlg->HWindow, "Такого в базе нет.", "Сообщаю", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
	     return -1;
	   }
       else {
		  strcpy(buff, (LPSTR)(LPCSTR)(String&)(Strings[0]));
		  if(nCount > 1) {
			 if(GetApplicationObject()->ExecDialog(new SelectStr(Dlg, Strings, buff)) != IDOK) return -1;
		  }
		 strncpy(ResStr, buff, 30);
		 ResStr[30] = 0;
	   }
	   return 1;
	}
}

int MandatoryDlg::FindLike(char* TableName, char* FldName, char* Maska, int Collect, int IsDigit, char** CheckFields)
{
   if(Collect) {
      while(m_LastFind.getItemsInContainer())
		m_LastFind.detach(0, TShouldDelete::Delete);
   }

   int Len = strlen(Maska);
   while(*Maska == ' ') {
      memmove(Maska, Maska + 1, Len);
      Len--;
   }
   while(Len && Maska[Len - 1] == ' ') {
      Maska[Len - 1] = 0;
      Len--;
   }

   char str[32];
	{
       Array Strings(10, 0, 10);

/*	   if(Is_FindMode_Srv() && !TableName && !IsDigit) {
	       int Result = FindOnServer(FldName, Maska, Strings);
	       if(Result != 1) return Result;
       }
	   else*/
       {
		   int Result = auxFindLike(!TableName ? base->GetTableName() : TableName, FldName, Maska, Strings, this, str, IsDigit, CheckFields);
	       if(Result != 1) return Result;
       } //End Find Block

       int nCount = Strings.getItemsInContainer();
       if(nCount == 0) {
  	       return -1;
       }
       else {
	      if(nCount > 1) {
		      if(Collect) {
	              Strings.ownsElements(0);
 		          ArrayIterator iter(Strings);
		          while(iter)  {
		              if(iter.current() != NOOBJECT)
		                   m_LastFind.add(iter.current());
		              iter++;
		          }
	          }
	      }
       }
       (*base)(0, strtok(str, "/"));
       (*base)(1, atof(strtok(0, " ")));
       if(!PXSrchKey(base->getTblHandle(), base->getRecHandle(), 2, SEARCHFIRST)) {
	       base->Get();
	       GetDataFromBase();
           return 1;
       }
    }
    return 0;
}

void MandatoryDlg::ShowPolisAvInfo()
{
   char b[32];
   char nmb[32];

   e[0]->GetText(b, sizeof b);
   e[1]->GetText(nmb, sizeof nmb);
   double Number = atof(nmb);

   char* str = new char[512];
   *str = 0;

   int Stop = 0;
   while(1) { //85347
       (*base)(0, b);
       (*base)(1, Number);
       if(PXSrchKey(base->getTblHandle(), base->getRecHandle(), 2, SEARCHFIRST) == PXSUCCESS) {
	   (avaria2)(0, b);
	   (avaria2)(1, Number);
	   int Counter = 0;
	   if(PXSrchKey(avaria2.getTblHandle(), avaria2.getRecHandle(), 2, SEARCHFIRST) == PXSUCCESS) {
	       Counter++;
	       while(PXSrchKey(avaria2.getTblHandle(), avaria2.getRecHandle(), 2, SEARCHNEXT) == PXSUCCESS)
		   Counter++;
	   }

	   sprintf(str + strlen(str), "Полис %s/%ld ", b, (long)Number);
	   base->Get();
           (*base)(b, sizeof b, 44);
	   (*base)(Number, 45);
	   if(*b)
	      sprintf(str + strlen(str), "Взамен %s/%ld ", b, (long)Number);
           sprintf(str + strlen(str), "%d аварий\n", Counter);
	   if(!*b)
	       break;
       }
       else
	   break;

       if(Stop++ > 10)
	   break;
   }

   MessageBox(HWindow, str, GetRCStr(585), MB_OK | MB_ICONINFORMATION);
   delete str;
}

void ExpStr(_str s, int& IsRussian, int& IsEnglish, int& IsDigit, int& IsBad, int CheckMinus)
{
	IsRussian = IsEnglish = IsDigit = IsBad = 0;

	if(s.s[0])
	{
		if(s.s[0] == ' ' || s.s[strlen(s.s) - 1] == ' ')
			IsBad = 1;
	}

	char* BadSymb = ";:.,\"''!@#$%^&*()_=+<>?~`/[]{}\\|";
	for(int i = 0; i < strlen(BadSymb); i++)
		if(strchr(s.s, BadSymb[i])) {
            IsBad = 1;
			break;
		}

    if(CheckMinus)
      if(strchr(s.s, '-'))
          IsBad = 1;

	if(strstr(s.s, "  "))
        IsBad = 1;

	for(i = 0; i < strlen(s.s); i++)
		if(s.s[i] >= '0' && s.s[i] <= '9') {
			IsDigit = 1;
			break;
		}

	for(i = 0; i < strlen(s.s); i++)
		if(s.s[i] >= 'а' && s.s[i] <= 'я' ||
		   s.s[i] >= 'a' && s.s[i] <= 'z') {
			IsBad = 1;
			break;
		}

	for(i = 0; i < strlen(s.s); i++)
		if(s.s[i] >= 'А' && s.s[i] <= 'Я') {
			IsRussian = 1;
			break;
		}

	for(i = 0; i < strlen(s.s); i++)
		if(s.s[i] >= 'A' && s.s[i] <= 'Z') {
			IsEnglish = 1;
			break;
		}
}

int _IsValidPolisNumberFull(const char* SECTION, _str s, long n, char* INI, char* Agent, int AskRange)
{
	if(!*Agent) return 1; //Чужой

	static PolisRange* _pr = 0;
	static PolisRangeCnt = 0;
	static long lastTimeAccess = 0;

	if((GetCurrentTime() - lastTimeAccess) > 5000)
	{
		PolisRangeCnt = 0;
		delete _pr;
    }

	lastTimeAccess = GetCurrentTime();

	if(PolisRangeCnt == 0)
	{
		char* FldNames[] = { "S", "START", "END", "AGENT" };
		DynStr buff;
		GetPrivateProfileString(SECTION, "NumbersFilePath", "", buff, 128, INI ? INI : ININame);
		int IsValidPath = 0;
		if(*buff == 0)
			strcpy(buff, "\\bases\\polisnmb");
		else {
			IsValidPath = 1;
			if(buff[strlen(buff) - 1] != '\\')
				strcat(buff, "\\");
			strcat(buff, "polisnmb");
		}
		TOpenBase base(buff, FldNames, 4, !IsValidPath, 0);

		PolisRangeCnt = -1;
		int FieldCount = 4;
		if(base.pxErr)
		{
			base.Close();
			base.setShowFieldMessage(1);
			FieldCount = 3;
			if(!base.Open(3))
				return 0;
		}
		_pr = new PolisRange[PolisRangeCnt = base.GetRecords()];
		for(int i = 0; i < PolisRangeCnt; i++)
		{
			base.Get();
			base(_pr[i].Seria, sizeof _pr[i].Seria, 0);
			_pr[i].From = base.GetDouble(1);
			_pr[i].To = base.GetDouble(2);
			_pr[i].Agent[0] = 0;
			if(FieldCount > 3)
            {
				base(_pr[i].Agent, sizeof _pr[i].Agent, 3);
			}
			base.Next();
		}
   }

   if(AskRange && PolisRangeCnt == 0)
   {
	   GetApplicationObject()->MainWindow->GetModule()->ExecDialog(new TSetRange(GetApplicationObject()->MainWindow));
   }

   for(int i = 0; i < PolisRangeCnt; i++)
	   if(!strcmp(s.s, _pr[i].Seria) && (_pr[i].Agent[0] == 0 || !*Agent || !strcmp(Agent, _pr[i].Agent))) {
		   if(n >= _pr[i].From && n <= _pr[i].To)
			   return 1;
	   }

   if(PolisRangeCnt==0)
   {
	   if(!strcmp(s.s, tempRange.Seria) && (tempRange.Agent[0] == 0 || !*Agent || !strcmp(Agent, tempRange.Agent))) {
		   if(n >= tempRange.From && n <= tempRange.To)
			   return 1;
	   }
   }
   
	return 0;
}

int _IsValidPolisNumber(const char* SECTION, _str s, long n, char* INI, char* Agent)
{
	return _IsValidPolisNumberFull(SECTION, s, n, INI, Agent, 0);
}

int IsImportMarka(_str marka, _str letter, const char* ININame)
{
static long lastAccess = 0;
static char* lININame = 0;
static char* NoCheckAutoType = 0;
static char* AutoType = 0;
static BYTE* AutoTypeImp = 0;
#define AUTO_TYPE_IMP_SIZE 250

	char Key[32];
	char buff[32];

	if(GetCurrentTime() - lastAccess > 1000L * 60) {

		if(lININame) delete lININame;
		if(NoCheckAutoType) delete NoCheckAutoType;
		if(AutoType) delete AutoType;
		if(AutoTypeImp) delete AutoTypeImp;
		lININame = new char[256];
		NoCheckAutoType = new char[256];
		memset(NoCheckAutoType, 0, 256);

		GetPrivateProfileString(MANDATORY_SECT, "AutoMarkFile", ININame, lININame, 256, ININame);
		for(int i = 0; i < 50; i++) {
			sprintf(Key, "NoCheckAutoType%d", i);
			if(!GetPrivateProfileString(MANDATORY_SECT, Key, "", NoCheckAutoType + i * 5, 5, lININame))
				break;
		}



		AutoType = new char[1024];
		AutoTypeImp = new BYTE[AUTO_TYPE_IMP_SIZE];
		memset(AutoType, 0, 1024);
		memset(AutoTypeImp, 0, AUTO_TYPE_IMP_SIZE);
		char* ch = AutoType;

		long Len = 0;
		for(i = 0; i < AUTO_TYPE_IMP_SIZE; i++) {
			if(Len > 1000) {
				*ch = 0;
			//	//MessageBox(0, "STOP", "", 0);
				break;
            }

			sprintf(Key, "AutoType%d", i);
			if(!GetPrivateProfileString(MANDATORY_SECT, Key, "", buff, sizeof buff, lININame)) {
				*ch = 0;
				break;
            }

			strcpy(ch, buff);

			sprintf(Key, "AutoType%dIMP", i);
			AutoTypeImp[i] = GetPrivateProfileInt(MANDATORY_SECT, Key, 1, lININame);

			ch += strlen(buff) + 1;
			Len += strlen(buff) + 1;
		}
	}
	lastAccess = GetCurrentTime();

	for(int i = 0;; i++) {
		if(i < 50 && (*(NoCheckAutoType + i * 5)) == 0) break;
		if(i < 50 && !strcmp(NoCheckAutoType + i * 5, letter.s)) return -1;
        if(i >= 50) {
			sprintf(Key, "NoCheckAutoType%d", i);
			if(!GetPrivateProfileString(MANDATORY_SECT, Key, "", buff, sizeof buff, lININame))
				break;
			if(!strcmp(buff, letter.s))
				return -1; //Не контролировать
		}
	}

	char* ch = AutoType;
	for(long n = 0; *ch; ch += strlen(ch) + 1, n++)
		if(strstr(marka.s, ch)) return AutoTypeImp[n];

	for(i = 0;; i++) {
		sprintf(Key, "AutoType%d", i);
		if(!GetPrivateProfileString(MANDATORY_SECT, Key, "", buff, sizeof buff, lININame))
			break;
		if(strstr(marka.s, buff)) {
			sprintf(Key, "AutoType%dIMP", i);
			return GetPrivateProfileInt(MANDATORY_SECT, Key, 1, lININame);
        }
	}

	int DefType = GetPrivateProfileInt(MANDATORY_SECT, "DefAutoTypeHideIMP", 0, lININame);

	for(i = 0;; i++) {
		sprintf(Key, "AutoType%dHide", i);
		if(!GetPrivateProfileString(MANDATORY_SECT, Key, "", buff, sizeof buff, lININame))
			break;
		if(strstr(marka.s, buff)) {
			sprintf(Key, "AutoType%dHideIMP", i);
			return GetPrivateProfileInt(MANDATORY_SECT, Key, DefType, lININame);
        }
	}

	return 2; //ERROR
}
           /*
int IsValidOwnerRecord(TOpenBase& base)
{
	int IsRussian, IsEnglish, IsDigit, IsBad;

	//Все даты на корректность диапазона!!!
	DATE _MinDate, _MaxDate;
	PXDateEncode(7, 1, 1999, &_MinDate);
	PXDateEncode(1, 1, 2010, &_MaxDate);
	for(int i = 0; i < base.GetFields(); i++) {
        char fType[10];
		PXFldType(base.getTblHandle(), base.getField(i), sizeof fType, fType);
		if(!strcmp(fType, "D") && !base.IsNull(i))
			if(base.GetDate(i) < _MinDate || base.GetDate(i) > _MaxDate) 
				return 1050;
	}

	//Проверить правильность адреса
	ExpStr(base.GetString(8), IsRussian, IsEnglish, IsDigit, IsBad, 1);
	if(IsEnglish) return 70;
	if(IsBad) return 80;

	//FIO
	ExpStr(base.GetString(2), IsRussian, IsEnglish, IsDigit, IsBad, 0);
	if(IsBad) return 40;

	return 0;
}
             */
int IsValidAvariaRecord(TOpenBase& base)
{
	int IsRussian, IsEnglish, IsDigit, IsBad;

	//Все даты на корректность диапазона!!!
	DATE _MinDate, _MaxDate;
	PXDateEncode(7, 1, 1999, &_MinDate);
	PXDateEncode(1, 1, 2010, &_MaxDate);
	for(int i = 0; i < base.GetFields(); i++) {
        char fType[10];
		PXFldType(base.getTblHandle(), base.getField(i), sizeof fType, fType);
		if(!strcmp(fType, "D") && !base.IsNull(i))
			if(base.GetDate(i) < _MinDate || base.GetDate(i) > _MaxDate)
                return 1050;
	}

	//Дата ДТП <= подачи
	if(base.GetDate(3) > base.GetDate(6))
		return 1051;

	//Проверить правильность адреса
	ExpStr(base.GetString(12), IsRussian, IsEnglish, IsDigit, IsBad, 1);
	if(IsEnglish) return 70;
	if(IsBad) return 80;
	if(strlen(base.GetString(12).s) < 3)
		return 1281;
	//letter
	ExpStr(base.GetString(44), IsRussian, IsEnglish, IsDigit, IsBad, 1);
	if(IsRussian) return 140;
	if(IsBad) return 150;
	//marka
	ExpStr(base.GetString(43), IsRussian, IsEnglish, IsDigit, IsBad, 1);
	if(IsBad) return 160;
	int cmr = IsImportMarka(base.GetString(43), base.GetString(44), ININame);
	if(cmr == 2 && base.GetString(43).s[0]) return 170;
	//S/N
	ExpStr(base.GetString(9), IsRussian, IsEnglish, IsDigit, IsBad, 1);
	if(IsEnglish) return 1472;
    //FIO
	ExpStr(base.GetString(10), IsRussian, IsEnglish, IsDigit, IsBad, 0);
	if(IsBad) return 40;

	//number body & shassi
	ExpStr(base.GetString(47), IsRussian, IsEnglish, IsDigit, IsBad, 1);
	if(IsBad) return 180;
	if(cmr == 1 && IsRussian) return 190;

    /*
	if(base.GetDouble(7) == 0.) { //оплачено
		if(base.IsNull(22) && base.IsNull(29) && base.IsNull(36))
			return 185;
		if(!base.IsNull(22) && base.GetDate(6) > base.GetDate(22))
			return 1052;
		if(!base.IsNull(29) && base.GetDate(6) > base.GetDate(29))
			return 1053;
		if(!base.IsNull(36) && base.GetDate(6) > base.GetDate(36))
			return 1054;
	}

	if(base.GetDouble(16) > 15000 || base.GetDouble(23) > 15000 || base.GetDouble(30) > 15000 || //ЕВРО Заяв
	   base.GetDouble(19) > 15000 || base.GetDouble(26) > 15000 || base.GetDouble(33) > 15000)   //ЕВРО Факт
			return 186;
	*/
    return 0;
}

int IsValidMandRecord(TOpenBase& base, int AskEngName, int AutoNmbCheck, const char* ININame, int Komplex)
{
	int IsRussian, IsEnglish, IsDigit, IsBad;
	int IsBadPolis = base.GetDouble(58) == 1.;

	char Agent_Code[5];
	base(Agent_Code, sizeof Agent_Code, 30);
	int IsOtherPolis = *Agent_Code == 0;

	//Проверить номер полиса (выдан ли компании!!!)
	if(!_IsValidPolisNumber(MANDATORY_SECT, base.GetString(0), base.GetDouble(1), 0, Agent_Code))
		return 1;
/*
	DATE _1_7_2010;
	PXDateEncode(7, 1, 2010, &_1_7_2010);

	if(base.GetDate(2) >= _1_7_2010)
	{
		if(Komplex && strcmp("КС", base.GetString(0).s) ||
		  !Komplex && strcmp("ВС", base.GetString(0).s))
		  {
		  DynStr msg;
		  sprintf(msg, "Вы уверены что серия '%s'", base.GetString(0).s);
		  if(MessageBox(0, msg, "?", MB_YESNO | MB_ICONQUESTION) == IDNO)
			return -1;
          }

    }
*/
	if(GetAsyncKeyState(VK_SHIFT) & 0x8000)
		if(GetAsyncKeyState(VK_CONTROL) & 0x8000)
			return 0;

	DATE _1_7_2000;
	PXDateEncode(7, 1, 2000, &_1_7_2000);

	if(!IsBadPolis) {
		//Проверить совпадение дат и версий полиса
		/*if(!IsOtherPolis)
		if(!base.GetString(44).s[0]) //prev seria
			if((base.GetDate(25) < _1_7_2000) && //pay1date
			   (base.GetDouble(57) != 1.) || //versian
			   (base.GetDate(25) >= _1_7_2000) &&
			   (base.GetDouble(57) == 1.))
				   return 20;
		*/
		//Проверить правильность имени
		ExpStr(base.GetString(3), IsRussian, IsEnglish, IsDigit, IsBad, 0);
		if(IsEnglish) {
			if(AskEngName && MessageBox(0, "Имя с английскими буквами?", base.GetString(3).s, MB_YESNO | MB_ICONQUESTION) == IDNO)
				return 30;
        }
		if(IsBad) return 40;
		if(!base.GetString(3).s[0]) return 50;
		if(base.GetString(3).s[0] >= '0' && base.GetString(3).s[0] <= '9') return 60;

		//Проверить правильность адреса
		ExpStr(base.GetString(4), IsRussian, IsEnglish, IsDigit, IsBad, 1);
		if(IsEnglish) return 70;
		if(IsBad) return 80;

		if(!IsOtherPolis)
		if(strlen(base.GetString(4).s) < 3)
                    return 81;

		//код местности проверить
		ExpStr(base.GetString(59), IsRussian, IsEnglish, IsDigit, IsBad, 1);
		if(IsRussian) return 100;
		if(IsBad) return 110;
		//letter
		ExpStr(base.GetString(8), IsRussian, IsEnglish, IsDigit, IsBad, 1);
		if(IsRussian) return 140;
		if(IsBad) return 150;
		//marka
		ExpStr(base.GetString(9), IsRussian, IsEnglish, IsDigit, IsBad, 1);
		if(IsBad) return 160;
		int cmr = IsImportMarka(base.GetString(9), base.GetString(8), ININame);
		if(!IsOtherPolis)
		  if(cmr == 2) return 172;

        //Номер авто
		_str s = base.GetString(10);
		if(AutoNmbCheck && strchr(s.s, ' '))
			return 171;

		//number body
		s = base.GetString(15);
		if(strlen(s.s) == 1 && (s.s[0] == ' ' || s.s[0] == '-')) {
		}
		else {
		    ExpStr(s, IsRussian, IsEnglish, IsDigit, IsBad, 1);
			if(IsBad || strchr(s.s, ' ')) return 180;
			if(cmr == 1 && IsRussian) return 190;
        }
		//number shassi
		s = base.GetString(16);
		if(strlen(s.s) == 1 && (s.s[0] == ' ' || s.s[0] == '-')) {
		}
        else {
			ExpStr(base.GetString(16), IsRussian, IsEnglish, IsDigit, IsBad, 1);
			if(IsBad) return 200;
			if(cmr == 1 && IsRussian) return 210;
		}

		//Даты
		if(!IsOtherPolis && base.GetDate(25) > base.GetDate(5)) //Дата оплаты после начала!!!
			return 1001;

		if(!IsOtherPolis) {
		  if(base.GetDouble(45) == 0) {//не дубликат
			  int delta = base.GetDouble(24) > 0 ? 31 : 365;
			  if( (base.GetDate(2) > (base.GetDate(25) + delta)) || (base.GetDate(2) < base.GetDate(25)) ) //Дата регистрации и оплаты
				  return 1010;
		  }
		  else {
			  if(base.GetDate(2) < base.GetDate(25)) //у дубликата дата выдачи после начала страхования - так быстро не теряют
				  return 1015;
		  }
        }

		if(!base.IsNull(54) && base.GetDate(54) >= base.GetDate(7)) //Дата остановки после окончания
			return 1211;

		//2я выплата
		if (!IsOtherPolis && !base.IsNull(36))   //25 - pay date
			if (base.GetDate(36) < base.GetDate(25) || base.GetDate(36) >= base.GetDate(7))
				return 1020;
	} //IsBadPolis

	//Все даты на корректность диапазона!!!
	DATE _MinDate, _MaxDate;
	PXDateEncode(7, 1, 1999, &_MinDate);

	struct date d;
	getdate(&d);
	PXDateEncode(d.da_mon, 1, d.da_year, &_MaxDate);
	_MaxDate = _MaxDate + 31*14; //14 monthes

	for(int i = 0; i < base.GetFields(); i++) {
        char fType[10];
		PXFldType(base.getTblHandle(), base.getField(i), sizeof fType, fType);
		if(!strcmp(fType, "D") && !base.IsNull(i))
			if(base.GetDate(i) < _MinDate || base.GetDate(i) > _MaxDate) {
				PXFldName(base.getTblHandle(), base.getField(i), sizeof fType, fType);
				MessageBox(GetApplicationObject()->MainWindow->HWindow, fType, fType, 0);
				return 1050;
            }
	}

	if(!IsBadPolis) {
		//Дату остановки полиса
		if (!base.IsNull(54)) {
			if (base.GetDate(54) < base.GetDate(2) || base.GetDate(54) >= base.GetDate(7))
				return 1500;
			if(base.IsNull(36) && !base.IsNull(29))    //!pay2date fee2date
  			   if (base.GetDate(54) >= base.GetDate(29))
			     return 1500;
		}

		//Тариф
		if(!IsOtherPolis && base.GetDouble(17) < 0.01) return 1530;
        //Сумма плетежа
		if(!IsOtherPolis && base.GetDouble(20) < 0.01) return 1540;
	}

	return 0;
}

typedef char string[30];

  string strLitValues[10][3] =  { {"",       "",            ""},
				 {"один",   "десять",      "сто"},
				 {"две",    "двадцать",    "двести"},
				 {"три",    "тридцать",    "триста"},
				 {"четыре", "сорок",       "четыреста"},
				 {"пять",   "пятьдесят",   "пятьсот"},
				 {"шесть",  "шестьдесят",  "шестьсот"},
				 {"семь",   "семьдесят",   "семьсот"},
				 {"восемь", "восемьдесят", "восемьсот"},
				 {"девять", "девяносто",   "девятьсот"}
				};

  string _10_20[10] = {"десять",
		       "одиннадцать",
		       "двенадцать",
		       "тринадцать",
		       "четырнадцать",
		       "пятнадцать",
		       "шестнадцать",
		       "семнадцать",
		       "восемнадцать",
		       "девятнадцать"
		       };
long GetNextDigit(long V)
{
    return V % 10;
}

int IsPodrostok(long V)
{
     V %= 100;
     return (V > 10) && (V < 20);
}

void InsS(char* dst, char* str, int lendst = 1024)
{
     DynStr buffer(2048);
     strcpy(buffer, str);
     strcat(buffer, dst);
     strcpy(dst, buffer);
     int minval = 2048;
     if(minval > (lendst - 5))
		minval = (lendst - 5);
	 if(strlen(buffer) >= minval)
	 {
        MessageBeep(0);
 	 	MessageBox(0, "ПЕРЕПОЛНЕНИЕ БУФЕРОВ!!!", "КРИТИЧЕСКАЯ ОШИБКА", MB_OK | MB_ICONSTOP);
	 }
}

char* s1 = new char[1024];
char* s2 = new char[1024];
char* sTemp = new char[1024];

char* NumberToWords(double Value)
{
	long intValue = Value;

	static int cnt = 0;
	++cnt;
	cnt = cnt%2;
	char* s = cnt ? s1 : s2;

    memset(s, 0, 1024);
    memset(sTemp, 0, sizeof sTemp);
    long Counter = 0;

    int IsBelowZero = Value < 0;
    if(Value < 0) Value *= -1;

    if(Value == 0) {
		strcpy(s, "ноль ");
    	return s;
    }

	while(intValue > 0)
	{
	  	if (((Counter % 3) == 0) && (intValue > 10))
	  	{
	    	if (((intValue - long((double)intValue / 100.) * 100) > 10) &&
				((intValue - long((double)intValue / 100.) * 100) < 20))
			{

				strcpy(sTemp, _10_20[(intValue - long((double)intValue / 100.) * 100) - 10]);
				strcat(sTemp, s);
	    	  	strcpy(s, sTemp);

		      	++Counter;
		      	intValue = long((double)intValue / 10.);
		    }
			else
			{
				goto Calculate;
            }
        }
        else {
		    Calculate :

			strcpy(sTemp, strLitValues[intValue % 10 /*- long((double)intValue / 10.) * 10*/][Counter % 3]);
			if(Counter > 3)
			{
				if((Counter % 3) == 0)
				{
					if((intValue%10/* - long((double)intValue / 10.) * 10*/) == 2)
						strcpy(sTemp, "два");
					if((intValue%10/* - long((double)intValue / 10.) * 10*/) == 1)
						strcpy(sTemp, "один");
				}
			}

			if(strlen(sTemp) > 0 && sTemp[strlen(sTemp)-1] != ' ') strcat(sTemp, " ");
			strcat(sTemp, s);
			strcpy(s, sTemp);
	    }

		++Counter;
		intValue = long((double)intValue / 10.);
		if(intValue % 1000 > 0)
		{
		  if(Counter == 3)
		  {
		  	if(IsPodrostok(intValue) || (GetNextDigit(intValue) >= 5) || (GetNextDigit(intValue) == 0))
				InsS(s, " тысяч ");
			else
			if(GetNextDigit(intValue) >= 2)
				InsS(s, " тысячи ");
			else
				InsS(s, " тысяча ");
		  }
		  if(Counter == 6)
		  {
			if(IsPodrostok(intValue) || (GetNextDigit(intValue) >= 5) || (GetNextDigit(intValue) == 0))
				InsS(s, " миллионов ");
			else
			if(GetNextDigit(intValue) >= 2)
				InsS(s, " миллиона ");
			else
				InsS(s, " миллион ");
		  }
		  if(Counter == 9)
		  {
			 if(IsPodrostok(intValue) || (GetNextDigit(intValue) >= 5) || (GetNextDigit(intValue) == 0))
			 	InsS(s, " миллиардов ");
			 else
			 if(GetNextDigit(intValue) >= 2)
				  InsS(s, " миллиарда ");
			 else
				  InsS(s, " миллиард ");
		  }
	  }

		if(intValue == 0)
			break;
	}

	while(!strcmp(s + strlen(s) - 2, "  "))
       s[strlen(s) - 1] = 0;

    if(IsBelowZero)
       InsS(s, "минус ");

	if(s[strlen(s)-1] == ' ')
		s[strlen(s)-1] = 0;

    return s;
}

class CTaxiFld : public TDialog {
public:
	TComboBox* Pay1Curr;
	TComboBox* PayVid;
	CTaxiFld(PTWindowsObject A);
	void SetupWindow();
	void Ok(RTMessage);
};

CTaxiFld::CTaxiFld(PTWindowsObject A)
		: TDialog(A, "TAXI_PAY")
{
	new NumberEdit(this, 103, 10, 1); //summa
	new TEdit(this, 104, 11);   //dateedit
	new NumberEdit(this, 105, 12, 0);   //pay number
	new TCheckBox(this, 101, 0);
	new TCheckBox(this, 106, 0);
	new TRadioButton(this, 107, 0);
	new TRadioButton(this, 150, 0);
	new TRadioButton(this, 151, 0);
	new TRadioButton(this, 152, 0);
	(Pay1Curr = new TComboBox(this, 102, 4))->DisableTransfer();
	(PayVid = new TComboBox(this, 108, 4))->DisableTransfer();
}

void CTaxiFld::Ok(RTMessage msg)
{
	TransferData(TF_GETDATA);
	taxi_data* data = (taxi_data*)TransferBuffer;
	if(!*data->Summa) return;
	if(atof(data->Summa) >= 0.01) {
  	  if(!GD(data->PayDate, 1)) return;
	  Pay1Curr->GetText(data->Curr, sizeof data->Curr);
	  if(!*data->Curr) return;
	}

	data->_PayVid = PayVid->GetSelIndex();
	TDialog::Ok(msg);
}

void CTaxiFld::SetupWindow()
{
	taxi_data* data = (taxi_data*)TransferBuffer;
	double s = atof(data->Summa);
	DblToStr(data->Summa, fabs(s), 2);

	TDialog::SetupWindow();

    Pay1Curr->AddString("BRB");
    Pay1Curr->AddString("USD");
	Pay1Curr->AddString("RUR");
	Pay1Curr->AddString("EUR");
	//Pay1Curr->AddString("DM");
	Pay1Curr->SetText(data->Curr);

	PayVid->AddString("Доплата");
	PayVid->AddString("Возврат");
	PayVid->SetSelIndex(s>=0.?0:1);
}

int MandatoryDlg::FillTaxiData(taxi_data& data)
{
	memset(&data, 0, sizeof data);
	strcpy(data.Curr, "BRB");

	char Seria[32];
	double Number;

	GetDlgItemText(HWindow, 51, Seria, sizeof Seria);
	Number = atof(Seria);
	GetDlgItemText(HWindow, 50, Seria, sizeof Seria);

	TaxiDb(0, Seria);
	TaxiDb(1, Number);

	int isfoundtexi = 0;
	if(PXSrchKey(TaxiDb.getTblHandle(), TaxiDb.getRecHandle(), 2, SEARCHFIRST) == PXSUCCESS) {
	   isfoundtexi = 1;
	   TaxiDb.Get();
	   double N;
	   TaxiDb(N, 2);
	   DblToStr(data.Summa, N, 2);

	   DateFBToStr(&TaxiDb, 6, data.PayDate);

	   TaxiDb(N, 7);
	   DblToStr(data.PayDoc, N, 0);

	   char buff[5];
	   TaxiDb(buff, sizeof buff, 5);
	   data._1SU = *buff = 'Y';

	   TaxiDb(buff, sizeof buff, 4);
	   data._IsNal = *buff = 'Y';

	   TaxiDb(data.Curr, sizeof data.Curr, 3);
	   TaxiDb(buff, sizeof buff, 8);
	   data._Taxi = *buff == 0;
	   data._AutoType = *buff == '2';
	   data._Avar = *buff == '3';
	   data._Other = *buff == '1';
	}
	return isfoundtexi;
}

void MandatoryDlg::Taxi(RTMessage)
{
	if(!SetDataToBase())
		return;

	taxi_data data;
	int isfoundtexi = FillTaxiData(data);

	CTaxiFld* dlg = new CTaxiFld(this);
	dlg->SetTransferBuffer(&data);
	if(GetApplication()->ExecDialog(dlg) == IDOK) {
		if(isfoundtexi)
			TaxiDb.Delete();
		if(atof(data.Summa) > 0.01) {
			TaxiDb(2, atof(data.Summa) * (data._PayVid==1?-1:1));
			TaxiDb(3, data.Curr);
			TaxiDb(4, data._IsNal ? "Y" : "N");
			TaxiDb(5, data._1SU ? "Y" : "N");
			TaxiDb(6, GD(data.PayDate));
			TaxiDb(7, atof(data.PayDoc));
			TaxiDb(8, data._Taxi ? "" : data._AutoType ? "2" : data._Avar ? "3" : "1");
			if(TaxiDb.AddRecord() != PXSUCCESS)
				MessageBox(HWindow, "Ошибка записи доплаты", "Ошибка", 0);
			else
				SetDlgItemText(HWindow, 504, "Доплата ЕСТЬ");
		}
        else
				SetDlgItemText(HWindow, 504, "Доплата");
	}
}
/*
void MandatoryDlg::PrintData(char*& dest, int Flag, int PrintVersion)
{
     int IsPrintMonth = ((TRulerWin*)(GetApplication()->MainWindow))->bIsMandMonth;
     if(Flag == 1) {
	Period->GetText(dest, 128);
	dest += strlen(dest) + 1;
	//FromDate
	strcpy(dest, strtok(buffer.FromDate, "./"));
	dest += strlen(dest) + 1;

	char* monthNumber = strtok(0, "./");
	strcpy(dest, monthNumber);
	dest += strlen(dest) + 1;

	if(IsPrintMonth) {
	    char key[16];
	    char buff[32] = "";
	    sprintf(key, "Month%d", atoi(monthNumber));
	    if(GetPrivateProfileString(MANDATORY_SECT, key, "", buff, 32, ININame)) {
	       strcpy(dest - 1, buff);
	       dest += strlen(dest) + 1;
	    }
	}

	strcpy(dest, strtok(0, "./"));
	dest += strlen(dest) + 1;

	strcpy(dest, buffer.FromTime);
	dest += strlen(dest) + 1;

	//ToDate
	strcpy(dest, strtok(buffer.ToDate, "./"));
	dest += strlen(dest) + 1;

	monthNumber = strtok(0, "./");
	strcpy(dest, monthNumber);
	dest += strlen(dest) + 1;
	if(IsPrintMonth) {
	    char key[16];
	    char buff[32] = "";
	    sprintf(key, "Month%d", atoi(monthNumber));
	    if(GetPrivateProfileString(MANDATORY_SECT, key, "", buff, 32, ININame)) {
  	       strcpy(dest - 1, buff);
	       dest += strlen(dest) + 1;
            }
	}

	strcpy(dest, strtok(0, "./"));
	dest += strlen(dest) + 1;
     }

     if(Flag == 2) {
	 strcpy(dest, buffer.Letter);
	 dest += strlen(dest) + 1;

	 strcpy(dest, buffer.Marka);
	 dest += strlen(dest) + 1;
	 strcpy(dest, buffer.AutoNumber);
	 dest += strlen(dest) + 1;
	 strcpy(dest, buffer.Volume);
	 dest += strlen(dest) + 1;
	 strcpy(dest, buffer.Tonnage);
	 dest += strlen(dest) + 1;
	 strcpy(dest, buffer.Quantity);
	 dest += strlen(dest) + 1;
	 strcpy(dest, buffer.Power);
	 dest += strlen(dest) + 1;
    }
	 if(Flag == 3) {
       if(buffer.IsFee2) {
  	    strcat(dest, buffer.AllFee);
		strcat(dest, "   ");
        }
	    strcat(dest, buffer.AllFeeText);

	dest += strlen(dest) + 1;

	strcpy(dest, buffer.Fee1);
	strcat(dest, "   ");
	strcat(dest, buffer.Fee1Text);
	dest += strlen(dest) + 1;

	strcpy(dest, buffer.Nal ? "X" : "");
	dest += strlen(dest) + 1;
	strcpy(dest, buffer.BezNal ? "X" : "");

	if(buffer.BezNal)
	 {
	   divDate(buffer.Fee1Date);
	   AddBufDate(dest, buffer.Fee1Date, 2);
	 }
	else
	 {
	   strcpy((dest += strlen(dest) + 1), "");
	   strcpy((dest += strlen(dest) + 1), "");
	   strcpy((dest += strlen(dest) + 1), "");
	 }

	dest += strlen(dest) + 1;

	strcpy(dest, buffer.DraftNumber);
	dest += strlen(dest) + 1;

	strcpy(dest, buffer.Fee2);

	if(PrintVersion < 3) {
	    strcat(dest, "   ");
	    strcat(dest, buffer.Fee2Text);
        }
	dest += strlen(dest) + 1;

	//Fee 2 Date
	strcpy(dest, buffer.Fee2Date);

	divDate(buffer.RegDate);
	AddBufDate(dest, buffer.RegDate, 2);
	dest += strlen(dest) + 1;
   }
}
*/

void MandatoryDlg::PrintData(char*& dest, int Flag, int PrintVersion)
{
   //if(UpdateFormData) return;
	 int IsPrintMonth = GetPrivateProfileInt("MANDATORY", "Месяцы", 0, ININame);
	 if(Flag == 1)
	 {
		  Period->GetText(dest, 128);
		  dest += strlen(dest) + 1;
		  //FromDate
		  strcpy(dest, strtok(buffer.FromDate, "./"));
		  dest += strlen(dest) + 1;

		  char* monthNumber = strtok(0, "./");
		  strcpy(dest, monthNumber);
		  dest += strlen(dest) + 1;

		  if(IsPrintMonth)
		  {
			  char key[16];
			  char buff[32] = "";
			  sprintf(key, "Month%d", atoi(monthNumber));
			  if(GetPrivateProfileString(MANDATORY_SECT, key, "", buff, 32, ::ININame))
			  {
				 strcpy(dest - 1, buff);
				 dest += strlen(dest) + 1;
			  }
		  }

		  strcpy(dest, strtok(0, "./"));
		  dest += strlen(dest) + 1;

		  strcpy(dest, buffer.FromTime);
		  dest += strlen(dest) + 1;

		  //ToDate
		  strcpy(dest, strtok(buffer.ToDate, "./"));
		  dest += strlen(dest) + 1;

		  monthNumber = strtok(0, "./");
		  strcpy(dest, monthNumber);
		  dest += strlen(dest) + 1;
		  if(IsPrintMonth)
		  {
			  char key[16];
			  char buff[32] = "";
			  sprintf(key, "Month%d", atoi(monthNumber));
			  if(GetPrivateProfileString(MANDATORY_SECT, key, "", buff, 32, ::ININame))
			  {
				 strcpy(dest - 1, buff);
				 dest += strlen(dest) + 1;
			  }
		  }

		  strcpy(dest, strtok(0, "./"));
		  dest += strlen(dest) + 1;
	 }

	 if(Flag == 2)
	 {
		strcpy(dest, buffer.Letter);
		dest += strlen(dest) + 1;

		strcpy(dest, buffer.Marka);
		dest += strlen(dest) + 1;
		strcpy(dest, buffer.AutoNumber);
		if(!strcmp(dest, "БН"))
		{
			GetPrivateProfileString(MANDATORY_SECT, "БН", "", dest, 32, ININame);
		}
		dest += strlen(dest) + 1;
		strcpy(dest, buffer.Volume);
		dest += strlen(dest) + 1;
		strcpy(dest, buffer.Tonnage);
		dest += strlen(dest) + 1;
		strcpy(dest, buffer.Quantity);
		dest += strlen(dest) + 1;
		strcpy(dest, buffer.Power);
		dest += strlen(dest) + 1;
	}

	if(Flag == 3)
	{
		strcat(dest, buffer.AllFee);
		if(!buffer.IsFee2)
		{
			strcat(dest, " ");
			if(Pay1Curr->GetSelIndex() == 0)
            {
				strcat(dest, "рублей");
			}
			else
			{
				//Pay1Curr->GetText(dest + strlen(dest), 10);
				strcat(dest, "Евро");
			}
		}
		else
		{
			strcat(dest, "  ");
 			strcat(dest, buffer.AllFeeText); 
		}

	dest += strlen(dest) + 1;

	sprintf(dest + strlen(dest), "%lu    ", atol(buffer.Fee1));
	//strcpy(dest, buffer.Fee1);
	//strcat(dest, "   ");
	strcat(dest, buffer.Fee1Text);
	dest += strlen(dest) + 1;

	strcpy(dest, buffer.Nal ? "X" : "");
	dest += strlen(dest) + 1;
	strcpy(dest, buffer.BezNal ? "X" : "");
	dest += strlen(dest) + 1;//++
	strcpy(dest, buffer.Karta ? "X" : "");//++
	//dest += strlen(dest) + 1;//++

	if(buffer.BezNal)
	 {
	   divDate(buffer.Fee1Date);
	   AddBufDate(dest, buffer.Fee1Date, 2);
	 }
	else
	 {
	   strcpy((dest += strlen(dest) + 1), "");
	   strcpy((dest += strlen(dest) + 1), "");
	   strcpy((dest += strlen(dest) + 1), "");
	 }

	dest += strlen(dest) + 1;

	strcpy(dest, buffer.DraftNumber);
	dest += strlen(dest) + 1;

	strcpy(dest, buffer.Fee2);

	if(PrintVersion < 3)
	{
	    strcat(dest, "   ");
	    strcat(dest, buffer.Fee2Text);
    }
	dest += strlen(dest) + 1;

	//Fee 2 Date
	strcpy(dest, buffer.Fee2Date);

	/*MessageBox(0, buffer.RegDate, 0, 0);*/
	divDate(buffer.RegDate);
/*	char * qqq = buffer.RegDate;
	MessageBox(0, qqq, 0, 0);
	qqq += strlen(qqq) + 1;
	MessageBox(0, qqq, 0, 0);
	qqq += strlen(qqq) + 1;
	MessageBox(0, qqq, 0, 0);*/
	AddBufDate(dest, buffer.RegDate, 2);
	dest += strlen(dest) + 1;

/*	dest += strlen(dest) + 1;
	strcat(dest, "1"); dest+=2;
	strcat(dest, "2"); dest+=2;
	strcat(dest, "3"); dest+=2;*/
   }
}

char* GetPolisState(int i)
{
    switch(i) {
       case 0 : return "";
       case 1 : return "ИСПОРЧЕН";
       case 2 : return "ДОСРОЧНО ЗАВЕРШЁН";
       case 3 : return "УТЕРЯН";
       case 4 : return "ЗАКОНЧИЛСЯ";
       case 5 : return "ЗАВЕРШЁН ПО НЕУПЛАТЕ";
    }
	return "<?>";
}

void MandatoryDlg::SetState(int S)
{
	m_State = S;

    DynStr b;
	sprintf(b, "%s страхование транспортных средств", Komplex ? "Комплексное" : "Обязательное" );
    if(strlen(GetPolisState(S))) {
       strcat(b, " [");
       strcat(b, GetPolisState(S));
       strcat(b, "]");
    }
	SetWindowText(HWindow, b);
}

void MandatoryDlg::NalFunc(RTMessage msg)
{
   Nal->Check();
   BezNal->Uncheck();
   Karta->Uncheck();
   e[25+2]->SetText("");
   if(ModifyMandatory == 1) InitTime(e[6+2]);
   if(ModifyMandatory) {
      msg.LP.Hi = EN_CHANGE;
      ChangePay1Date(msg);
	  CalculateSumma();
   }
}

void MandatoryDlg::BezNalFunc(RTMessage msg)
{
   Nal->Uncheck();
   BezNal->Check();
   Karta->Uncheck();
   e[6+2]->SetText("");
   InitTime(e[6+2]);
   if(ModifyMandatory) {
      msg.LP.Hi = EN_CHANGE;
      ChangePay1Date(msg);
      CalculateSumma();
   }
}

void MandatoryDlg::KartaFunc(RTMessage msg)
{
   Nal->Uncheck();
   BezNal->Uncheck();
   Karta->Check();
   e[6+2]->SetText("");
   InitTime(e[6+2]);
   if(ModifyMandatory) {
      msg.LP.Hi = EN_CHANGE;
      ChangePay1Date(msg);
	  CalculateSumma();
   }
}

/*void MandatoryDlg::AddOwnerToBuffer(char*& dest, int Index, int Version)
{
   char buf[64];
   char key[32];

       strcat(dest, autoowners[Index].Name);
       if(Version == 1) strcat(dest, "\r\n");
       else strcat(dest, ",  ");
       for(int fnd = 0;; fnd++) {
	  sprintf(key, "Base_of_possession%d", fnd);
	  assert(strlen(key) < sizeof key);
	  if(!GetPrivateProfileString(MANDATORY_SECT, key, "", buf, sizeof buf, ININame))
	     break;
	  if(atoi(buf) == autoowners[Index].Pravo)
	     break;
       }
       char* divider = strchr(buf, ',');
       if(!divider) divider = buf;
       else {
          divider++;
	  while(*divider && *divider == ' ') divider++;
       }
       if(Version == 1) {
	  strcat(dest, divider);
	  strcat(dest, " ");
	  strcat(dest, autoowners[Index].PravoTxt);
	  strcat(dest, "\r\n");
	  strcat(dest, autoowners[Index].Address);
	  strcat(dest, "\r\n");
	  strcat(dest, autoowners[Index].RegDate);
	  strcat(dest, "\r\n");
	  strcat(dest, "\r\n");
       }
       else {
	  strcat(dest, autoowners[Index].Address);
	  strcat(dest, ",   ");
	  strcat(dest, divider);
	  strcat(dest, " ");
	  strcat(dest, autoowners[Index].PravoTxt);
	  strcat(dest, ", ");
	  strcat(dest, autoowners[Index].RegDate);
       }
}
*/
void MandatoryDlg::Zajava(RTMessage)
{
   Dlg_base::SetDataToBase();

   PetitionData data;
   if(*buffer.PrevSeria)
	   sprintf(data.SN, "%s/%s", buffer.PrevSeria, buffer.PrevNumber);
   PetitionDlg* dlg = new PetitionDlg(this, &data);
   if(GetModule()->ExecDialog(dlg) != IDOK) return;

   char* str = new char [3000];
   char* dest = str;

   memset(str, 0, 3000);

   strcpy(dest, ""); //right upper corner
   dest += strlen(dest) + 1;

   strcpy(dest, buffer.Name); 
   strcat(dest, ", ");
   strcat(dest, buffer.Address);

   if(data.PrintNumbers)
   {
	   if(*buffer.MobilePhone)
	   {
		  strcat(dest, ", моб. ");
		  strcat(dest, buffer.MobilePhone);
	   }
	   if(*buffer.HomePhone)
	   {
		  strcat(dest, ", дом. ");
		  strcat(dest, buffer.HomePhone);
	   }
   }
   dest += strlen(dest) + 1;
   Period->GetText(dest, 50);

   dest += strlen(dest) + 1;
   strcpy(dest, buffer.FromDate);

   dest += strlen(dest) + 1;
   ((TComboBox*)ChildWithId(999))->GetText(dest, 50);
   strcat(dest, ", ");
   strcat(dest, data.OwnDoc);

   dest += strlen(dest) + 1;
   strcpy(dest, buffer.Letter);

   dest += strlen(dest) + 1;
   strcpy(dest, buffer.Marka);

   dest += strlen(dest) + 1;
   strcpy(dest, buffer.AutoNumber);
   if(!strcmp(dest, "БН")) *dest = 0;

   dest += strlen(dest) + 1;
   strcpy(dest, *buffer.NumberBody ? buffer.NumberBody : buffer.Chassis);

   dest += strlen(dest) + 1;
   strcpy(dest, buffer.Volume);

   dest += strlen(dest) + 1;
   strcpy(dest, !strcmp(buffer.Letter,"A6")?"X":""); //Taxi

   dest += strlen(dest) + 1;
   strcpy(dest, strcmp(buffer.Letter,"A6")?"X":""); //Not Taxi

   dest += strlen(dest) + 1;
   strcpy(dest, !strcmp(buffer.Letter,"L4")?"X":""); //Bus

   dest += strlen(dest) + 1;
   strcpy(dest, strcmp(buffer.Letter,"L4")?"X":""); //Not Bus

   dest += strlen(dest) + 1;
   strcpy(dest, data.Discount);

   dest += strlen(dest) + 1;
   strcpy(dest, buffer.IsFee2 ? "" : "X");

   dest += strlen(dest) + 1;
   strcpy(dest, !buffer.IsFee2 ? "" : "X");

   dest += strlen(dest) + 1;
   strcpy(dest, buffer.Nal ? "X" : "");

   dest += strlen(dest) + 1;
   strcpy(dest, !buffer.Nal ? "X" : ""); 

   dest += strlen(dest) + 1;
   strcpy(dest, *data.Company ? data.Company :
				*buffer.PrevSeria ? "ЗАСO \"ТАСК\"" : "");

   dest += strlen(dest) + 1;
   strcpy(dest, buffer.PrevSeria);
   if(*buffer.PrevSeria) strcat(dest, "/");
   strcat(dest, buffer.PrevNumber);

   if(*data.SN)
	   strcpy(dest, data.SN);

   dest += strlen(dest) + 1;

   char* pos = max(strrchr(data.From, '.'), strrchr(data.From, '/'));
   if(pos) *pos = 0;
   strcpy(dest, data.From);
   dest += strlen(dest) + 1;
   strcpy(dest, pos ? (pos + strlen(pos + 2)) : ""); //two last digits
   dest += strlen(dest) + 1;

   pos = max(strrchr(data.To, '.'), strrchr(data.To, '/'));
   if(pos) *pos = 0;
   strcpy(dest, data.To);
   dest += strlen(dest) + 1;
   strcpy(dest, pos ? (pos + strlen(pos+2)) : ""); //two last digits


   dest += strlen(dest) + 1;
	strcpy(dest, strtok(buffer.RegDate, "./"));
	dest += strlen(dest) + 1;

	char* monthNumber = strtok(0, "./");
	strcpy(dest, GetRCStr(99 + atoi(monthNumber)));
	dest += strlen(dest) + 1;

	strcpy(dest, strtok(0, "./"));
	dest += strlen(dest) + 1;

	strcpy(dest, data.Avar);

   dest += strlen(dest) + 1;
   strcpy(dest, data.before25?"X":""); //До 25 вкл

   dest += strlen(dest) + 1;
   strcpy(dest, data.after25?"X":""); //После 25

   dest += strlen(dest) + 1;
   strcpy(dest, data.before2?"X":""); //До 2х
   dest += strlen(dest) + 1;
   strcpy(dest, data.after2?"X":""); //После 2x
   dest += strlen(dest) + 1;
   strcpy(dest, data.noDriveLic?"X":""); //В/у отсутствует

   ((TRulerWin*)Parent)->LoadBlank("petition.blk");
   ((TRulerWin*)Parent)->SetValues(str);
   Dlg_base::Print1();
   ((TRulerWin*)Parent)->LoadBlank(0);

   delete str;
}

void MandatoryDlg::Find()
{
   base->Refresh();
   HMENU Popup = CreatePopupMenu();
   int State = MF_ENABLED;
   if(!IsWindowEnabled(e[3]->HWindow))
       State = MF_DISABLED | MF_GRAYED;
   AppendMenu(Popup, MF_ENABLED | MF_STRING, 700, "&1. По Серии/Номеру");
   AppendMenu(Popup, State | MF_STRING, 704, "&2. По Взамен");
   AppendMenu(Popup, MF_ENABLED | MF_STRING, 701, "&3. По номеру транспортного средства");
   AppendMenu(Popup, MF_ENABLED | MF_STRING, 703, "&4. По части номера транспортного средства");
   AppendMenu(Popup, MF_ENABLED | MF_STRING, 702, "&5. По ФИО владельца");
   State = MF_ENABLED;
   if(!m_LastFind.getItemsInContainer())
       State = MF_DISABLED | MF_GRAYED;
   AppendMenu(Popup, State | MF_STRING, 705, "&6. По ФИО владельца (повторить)");

   if(!Komplex) {
   AppendMenu(Popup, MF_SEPARATOR, 0, "");

   AppendMenu(Popup, MF_ENABLED | MF_STRING, 710, "&7. По ФИО виновника");
   AppendMenu(Popup, MF_ENABLED | MF_STRING, 711, "&8. По номеру TC виновника");
   AppendMenu(Popup, MF_ENABLED | MF_STRING, 712, "&9. По номеру дела");
   AppendMenu(Popup, MF_ENABLED | MF_STRING, 713, "10. По ФИО владельца из аварии");
   }
   //AppendMenu(Popup, MF_ENABLED | MF_STRING, 714, "11. По ФИО доп. владельца");

   AppendMenu(Popup, MF_SEPARATOR, 0, "");
   AppendMenu(Popup, MF_ENABLED | MF_STRING, 715, "12. По померу 1й платёжки");
   if(!Komplex) {
   AppendMenu(Popup, MF_ENABLED | MF_STRING, 716, "13. По померу 2й платёжки");
   }

   POINT ptCurrent;
   GetCursorPos(&ptCurrent);
   TrackPopupMenu(Popup, TPM_LEFTALIGN, ptCurrent.x, ptCurrent.y, 0, HWindow, NULL);
   DestroyMenu(Popup);
}

int  MandatoryDlg::FindRepeat()
{
	 char str[65];
     if(GetModule()->ExecDialog(new SelectStr(this, m_LastFind, str)) != IDOK) return -1;
     (*base)(0, strtok(str, "/"));
     (*base)(1, atof(strtok(0, " ")));
     if(!PXSrchKey(base->getTblHandle(), base->getRecHandle(), 2, SEARCHFIRST)) {
         base->Get();
         GetDataFromBase();
          return 1;
     }
     return 0;
}

int MandatoryDlg::FindPlat(int Part)
{
	char Maska[32] = "";
	char* CheckFields[3] = { "AllFeeText", "Y", " 1-СУ" };
	if(GetModule()->ExecDialog(new InputStringDlg(this, "Поиск полиса", Part == 1 ? "Номер 1 платёжки" : "Номер 2 платёжки", "", Maska)) != IDOK) return -1;
	return FindLike(0,
			Part == 1 ? "DraftNumber" : "Pay2DraftNumber",
		    Maska,
			0,
            1, //isdigit
			Part == 1 ? 0 : CheckFields);
}

int MandatoryDlg::FindAv(int Flag)
{
	char Maska[32] = "";
	if(GetModule()->ExecDialog(new InputStringDlg(this, "Поиск полиса", Flag == 1 ? "Виновник" : Flag == 2 ? "Номер ТС виновника" : Flag == 3 ? "Номер дела" : "Доп. владелец из аварии", "", Maska)) != IDOK) return -1;
    return FindLike("\\MANDAV2",
		    Flag == 1 ? "BADMAN" : Flag == 2 ? "AutoNmb_p" : Flag == 3 ? "NWork" : "FIO",
		    Maska,
		    0);
}

void MandatoryDlg::WMCommand(RTMessage msg)
{
   if(msg.Message == WM_COMMAND ) {
      int Find = -1;
      if(msg.WParam == 700) Find = FindSN(0);
      if(msg.WParam == 704) Find = FindSN(1);
      if(msg.WParam == 701) Find = FindTransport();
      if(msg.WParam == 703) Find = FindTransportLike();
      if(msg.WParam == 702) Find = FindOwner();
      if(msg.WParam == 705) Find = FindRepeat();
      if(msg.WParam == 706) Find = FindOwner(1);
      if(msg.WParam == 707) Find = FindOwner(2);
      if(msg.WParam == 710) Find = FindAv(1);
      if(msg.WParam == 711) Find = FindAv(2);
      if(msg.WParam == 712) Find = FindAv(3);
      if(msg.WParam == 713) Find = FindAv(4);
	  if(msg.WParam == 714) {
	      char Maska[32] = "";
		  if(GetModule()->ExecDialog(new InputStringDlg(this, "Поиск", "Доп. владелец", "", Maska)) != IDOK)
	          Find = -1;
          else
	          Find = FindLike("\\MANDOWN", "NAME", Maska, 0);
      }
      if(Find == 0)
		  MessageBox(HWindow, GetRCStr(613), "Ошибка поиска", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
	  if(msg.WParam == 715) Find = FindPlat(1);
	  if(msg.WParam == 716) Find = FindPlat(2);
   }
   Dlg_base::WMCommand(msg);
}

void MandatoryDlg::FindNumberAutoCmd(RTMessage)
{
   if(FindTransportLike() == 0)
	  MessageBox(HWindow, GetRCStr(613), "Ошибка поиска", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
}

void MandatoryDlg::FindSNCmd(RTMessage)
{
   if(FindSN(0) == 0)
	  MessageBox(HWindow, GetRCStr(613), "Ошибка поиска", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
}

int MandatoryDlg::FindSN(int ChangeSN)
{
   char b[32];
   char nmb[32];

   int IsNeedFind = 1;
   if(!ChangeSN) {
       IsNeedFind = e[0]->GetText(b, sizeof b) && e[1]->GetText(nmb, sizeof nmb);
   }
   else
       IsNeedFind = e[3]->GetText(b, sizeof b) && e[4]->GetText(nmb, sizeof nmb);

   if(IsNeedFind)
    {
       char* bad;
       double Number = strtod(nmb, &bad);
       (*base)(0, b);
       (*base)(1, Number);
       if(!PXSrchKey(base->getTblHandle(), base->getRecHandle(), 2, SEARCHFIRST) && !*bad)
        {
           base->Get();
	       GetDataFromBase();
	       return 1;
	   }
	}

   return 0;
}

int MandatoryDlg::FindOwner(int Part)
{
   char b[64];
   if(e[3+2]->GetText(b, sizeof b))
      return FindLike(0, "NAME", b, 1);
    return 0;
}

void FillPlacement3(TComboBox* PlacementCombo, const char* key_mask, const char* Sect, int IsDiv, int Mul, int StrKey, const char* ININame)
{
	PlacementCombo->ClearList();
    char buf[64];
    for(int i = 0;; i++) {
		char key[32];
		sprintf(key, key_mask, i);
		assert(strlen(key) < sizeof key);
		if(!GetPrivateProfileString(Sect ? Sect : MANDATORY_SECT, key, "", buf, sizeof buf, ININame))
			break;

		char* divider = strchr(buf, ',');
		if(IsDiv && divider)
		{
		   *divider = 0;
		   divider++;
		   while(*divider && *divider == ' ') divider++;
		   if(!*divider) break;
		}
		else
		{
		   divider = buf;
		}

		int Index = PlacementCombo->AddString(divider);
		if(StrKey)
		{
			DWORD VAL = 0;
			strncpy((char*)&VAL, buf, 4);
			SendMessage(PlacementCombo->HWindow, CB_SETITEMDATA, Index, VAL);
		}
		else
        {
			SendMessage(PlacementCombo->HWindow, CB_SETITEMDATA, Index, atof(buf) * Mul);
		}
	}
	AlignCombo(PlacementCombo);
}

void FillPlacement2(TComboBox* PlacementCombo, const char* key_mask, const char* Sect, int IsDiv, int Mul, const char* ININame)
{
	FillPlacement3(PlacementCombo, key_mask, Sect, IsDiv, 100, 0, ININame);
}

void FillPlacement(TComboBox* PlacementCombo, const char* key_mask, const char* Sect, int IsDiv)
{
	FillPlacement2(PlacementCombo, key_mask, Sect, IsDiv, 100, ININame);
}


