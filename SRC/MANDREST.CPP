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
#include "GRNCARD.h"
#include "io.h"
#include "sortarry.h"

#define max(a, b) ((a) > (b) ? (a) : (b))

#undef NDEBUG
#include "assert.h"

extern const char* MANDATORY_SECT;
extern char* MandatoryFields[];
extern char MandatoryCnt;
extern char KomplexCnt;
extern char UpdateFormData;
extern char ModifyMandatory;
extern char ActivateParadox;
extern DATE GD(char* str, int = 0);

BOOL MyOpenFile(HWND HWindow, OPENFILENAME& ofn, char* szFilter, char* szDirName, char* szFile, char* Title, char* ext, int Load = 1 );

void CopyFieldValue(TOpenBase& From, TOpenBase& To, int field)
{
    double Value;
	DynStr  buff(1024);
	DATE   date_val;
	short sValue;

	strcpy(buff, "");
	PXFldType(
		From.getTblHandle(),      // table to get field type from
		From.getField(field),      // field number of field in table
        256,                // size of return buffer
		buff);         // field type of field as string

	if(!From.IsNull(field)) {
		if(buff[0] == 'A' || buff[0] == 'M') {
			From(buff, buff._sizeof(), field);
				To(field, buff);
		}
        else
		if(buff[0] == 'D') {
			From(date_val, field);
			To(field, date_val);
		}
		else
		if(buff[0] == 'N') {
			From(Value, field);
			To(field, Value);
		}
		else
		if(buff[0] == 'S') {
			From(sValue, field);
			To(field, sValue);
		}
		else {
			MessageBox(0, "Неизвестный тип поля", buff, MB_ICONSTOP);
        }
	}
	else
		PXPutBlank(To.getTblHandle(), To.getField(field));
}

void CopyFieldValue(TOpenBase& From, FILE* f, int field)
{
    double Value;
	DynStr  buff(1024);
	short  shortValue;

	strcpy(buff, "");
	PXFldType(
		From.getTblHandle(),      // table to get field type from
		From.getField(field),      // field number of field in table
        buff._sizeof(),                // size of return buffer
		buff);         // field type of field as string

	DATE   date_val;
    int IsNull = From.IsNull(field);
	if(buff[0] == 'A' || buff[0] == 'M') {
		From(buff, buff._sizeof(), field);
		int Len = strlen(buff);
		fwrite(&Len, sizeof Len, 1, f);
		fwrite(buff, Len, 1, f);
	}
    else
	if(buff[0] == 'D') {
		if(IsNull) date_val = -1234;
		else From(date_val, field);
		fwrite(&date_val, sizeof date_val, 1, f);
	}
    else
	if(buff[0] == 'N') {
		if(IsNull) Value = -1234.;
		else From(Value, field);
		fwrite(&Value, sizeof Value, 1, f);
	}
    else
	if(buff[0] == 'S') {
		if(IsNull) shortValue = -1234;
		else From(shortValue, field);
		fwrite(&shortValue, sizeof shortValue, 1, f);
	}
	else {
		MessageBox(0, buff, buff, 0);
	}
}

void MandatoryDlg::Restore(int Komplex)
{
    OPENFILENAME ofn;
	char* l_szDirName = new char [256];
    strcpy(l_szDirName, "D:\\");
    char* l_szFile = new char [256];
    strcpy(l_szFile, "MANDATOR.DB");
    char*  szFilter = "Файлы Paradox(*.db)|*.db||";

    if(MyOpenFile(GetApplicationObject()->MainWindow->HWindow, ofn, szFilter, l_szDirName, l_szFile, "Открыть таблицу для восстановления", "")) {
	 TOpenBase badTable(l_szFile, MandatoryFields, Komplex ? KomplexCnt : MandatoryCnt, 0);
	 if(badTable.pxErr)
		goto FreeMemory;;
	 strcpy(l_szDirName, l_szFile);
	 strcpy(strrchr(l_szFile, '\\'), "\\mandrest.db");

	 TOpenBase emptyTable(l_szFile, MandatoryFields, Komplex ? KomplexCnt : MandatoryCnt, 0);
	 if(emptyTable.pxErr)
	    return;

	 char* msg = new char[1024];
	 sprintf(msg, "В таблице %s %ld записей.\nДанные будут записаны в таблицу %s\nПерекачать с начала таблицы[Yes] или с конца[No]", l_szDirName, badTable.GetRecords(), l_szFile);

	 int FromFirst = MessageBox(GetApplicationObject()->MainWindow->HWindow, msg, "Ответь", MB_YESNO | MB_ICONSTOP) == IDYES;
	 delete msg;

	 int IsGoto = MessageBox(GetApplicationObject()->MainWindow->HWindow, "Пробегать по позиции", "Ответь", MB_YESNO | MB_ICONSTOP) == IDYES;

	 if(emptyTable.GetRecords()) {
	    int Ret = MessageBox(GetApplicationObject()->MainWindow->HWindow, "Таблица MANDREST не пустая. Почистить?", "Ошибка", MB_YESNOCANCEL | MB_ICONQUESTION);
            if(Ret == IDCANCEL)
		goto FreeMemory;
	    if(Ret == IDYES) {
	       SetCursor(LoadCursor(0, IDC_WAIT));
	       while(emptyTable.GetRecords())
		  emptyTable.Delete();
            }
	 }

	 SetCursor(LoadCursor(0, IDC_WAIT));

	 if(!FromFirst) badTable.Last();
	 else badTable.First();
	 int Bad = 0;
	 int Moved = 0;
	 int n;
	 for(int i = 0; i < badTable.GetRecords(); i++) {
	    if(!badTable.Get()) {
		   Bad++;
		   goto IgnoreMove;
	    }
	    PXRecBufEmpty(emptyTable.getRecHandle());

	    double Nmb;
	    badTable(Nmb, 1);
	    if(Nmb < 1 || Nmb > 1000000000.0) {
                Bad++;
	    }

		for(n = 0; n < (Komplex ? KomplexCnt : MandatoryCnt); n++)
			CopyFieldValue(badTable, emptyTable,  n);
/*		CopyFieldValue(badTable, emptyTable,  0, FIELDTYPE_CHAR);   //Seria
		CopyFieldValue(badTable, emptyTable,  1, FIELDTYPE_NUMBER); //Number
	    CopyFieldValue(badTable, emptyTable,  2, FIELDTYPE_DATE);   //RegDate
	    CopyFieldValue(badTable, emptyTable,  3, FIELDTYPE_CHAR);   //Name
	    CopyFieldValue(badTable, emptyTable,  4, FIELDTYPE_CHAR);   //Address
	    CopyFieldValue(badTable, emptyTable,  5, FIELDTYPE_DATE);   //FromDate
	    CopyFieldValue(badTable, emptyTable,  6, FIELDTYPE_CHAR);   //FromTime
	    CopyFieldValue(badTable, emptyTable,  7, FIELDTYPE_DATE);   //ToDate
	    CopyFieldValue(badTable, emptyTable,  8, FIELDTYPE_CHAR);   //Letter
	    CopyFieldValue(badTable, emptyTable,  9, FIELDTYPE_CHAR);   //Marka
	    CopyFieldValue(badTable, emptyTable, 10, FIELDTYPE_CHAR);   //AutoNumber
	    CopyFieldValue(badTable, emptyTable, 11, FIELDTYPE_NUMBER);   //Volume
	    CopyFieldValue(badTable, emptyTable, 12, FIELDTYPE_NUMBER);   //Tonnage
	    CopyFieldValue(badTable, emptyTable, 13, FIELDTYPE_NUMBER);   //Quantity
	    CopyFieldValue(badTable, emptyTable, 14, FIELDTYPE_NUMBER);   //Power
	    CopyFieldValue(badTable, emptyTable, 15, FIELDTYPE_CHAR);   //NumberBody
	    CopyFieldValue(badTable, emptyTable, 16, FIELDTYPE_CHAR);   //Chassis
	    CopyFieldValue(badTable, emptyTable, 17, FIELDTYPE_NUMBER); //Tarif
	    CopyFieldValue(badTable, emptyTable, 18, FIELDTYPE_NUMBER); //Rate
	    CopyFieldValue(badTable, emptyTable, 19, FIELDTYPE_NUMBER); //Discount
	    CopyFieldValue(badTable, emptyTable, 20, FIELDTYPE_NUMBER); //AllFee
	    CopyFieldValue(badTable, emptyTable, 21, FIELDTYPE_CHAR);   //AllFeeText
	    CopyFieldValue(badTable, emptyTable, 22, FIELDTYPE_NUMBER); //Pay1
	    CopyFieldValue(badTable, emptyTable, 23, FIELDTYPE_CHAR);   //Pay1Text
	    CopyFieldValue(badTable, emptyTable, 24, FIELDTYPE_NUMBER); //PayType
	    CopyFieldValue(badTable, emptyTable, 25, FIELDTYPE_DATE);   //PayDate
	    CopyFieldValue(badTable, emptyTable, 26, FIELDTYPE_NUMBER); //DraftNumber
	    CopyFieldValue(badTable, emptyTable, 27, FIELDTYPE_NUMBER); //Fee2
	    CopyFieldValue(badTable, emptyTable, 28, FIELDTYPE_CHAR);   //Fee2Text
	    CopyFieldValue(badTable, emptyTable, 29, FIELDTYPE_DATE);   //Fee2Date
	    CopyFieldValue(badTable, emptyTable, 30, FIELDTYPE_CHAR);   //AgentCode
	    CopyFieldValue(badTable, emptyTable, 31, FIELDTYPE_CHAR);   //Period
	    CopyFieldValue(badTable, emptyTable, 32, FIELDTYPE_NUMBER); //Super
	    CopyFieldValue(badTable, emptyTable, 33, FIELDTYPE_CHAR);   //InsComp
	    CopyFieldValue(badTable, emptyTable, 34, FIELDTYPE_NUMBER); //IsFee2

	    CopyFieldValue(badTable, emptyTable, 35, FIELDTYPE_NUMBER); //IsPay2
	    CopyFieldValue(badTable, emptyTable, 36, FIELDTYPE_DATE);   //Pay2Date
	    CopyFieldValue(badTable, emptyTable, 37, FIELDTYPE_NUMBER); //Pay2Rate
	    CopyFieldValue(badTable, emptyTable, 38, FIELDTYPE_NUMBER); //Pay2Summa
	    CopyFieldValue(badTable, emptyTable, 39, FIELDTYPE_NUMBER); //Pay2Type
	    CopyFieldValue(badTable, emptyTable, 40, FIELDTYPE_NUMBER); //Pay2DraftNumber
	    CopyFieldValue(badTable, emptyTable, 41, FIELDTYPE_NUMBER); //Resident

	    CopyFieldValue(badTable, emptyTable, 42, FIELDTYPE_CHAR);   //Pay1Curr
	    CopyFieldValue(badTable, emptyTable, 43, FIELDTYPE_CHAR);   //Pay2Curr
	    CopyFieldValue(badTable, emptyTable, 44, FIELDTYPE_CHAR);   //PrevSeria
	    CopyFieldValue(badTable, emptyTable, 45, FIELDTYPE_NUMBER); //PrevNumber

		CopyFieldValue(badTable, emptyTable, 46, FIELDTYPE_NUMBER); //TarifGlass
		CopyFieldValue(badTable, emptyTable, 47, FIELDTYPE_CHAR);   //Placement
		CopyFieldValue(badTable, emptyTable, 48, FIELDTYPE_NUMBER); //K
		CopyFieldValue(badTable, emptyTable, 49, FIELDTYPE_CHAR);   //ClassOldAv
		CopyFieldValue(badTable, emptyTable, 50, FIELDTYPE_NUMBER); //CountAv
		CopyFieldValue(badTable, emptyTable, 51, FIELDTYPE_CHAR);   //ClassAvCurr
		CopyFieldValue(badTable, emptyTable, 52, FIELDTYPE_NUMBER); //K1
		CopyFieldValue(badTable, emptyTable, 53, FIELDTYPE_NUMBER); //K2
		CopyFieldValue(badTable, emptyTable, 54, FIELDTYPE_DATE);   //StopDate
		CopyFieldValue(badTable, emptyTable, 55, FIELDTYPE_NUMBER); //Vladenie
		CopyFieldValue(badTable, emptyTable, 56, FIELDTYPE_NUMBER); //InsurerType
		CopyFieldValue(badTable, emptyTable, 57, FIELDTYPE_NUMBER); //Version
		CopyFieldValue(badTable, emptyTable, 58, FIELDTYPE_NUMBER); //State

		CopyFieldValue(badTable, emptyTable, 59, FIELDTYPE_CHAR);   //CityCode
		CopyFieldValue(badTable, emptyTable, 60, FIELDTYPE_DATE);   //UpdateDate
		CopyFieldValue(badTable, emptyTable, 61, FIELDTYPE_NUMBER); //OwnerCode
		CopyFieldValue(badTable, emptyTable, 62, FIELDTYPE_NUMBER); //BaseCarCode
		CopyFieldValue(badTable, emptyTable, 63, FIELDTYPE_NUMBER); //CarCode
		CopyFieldValue(badTable, emptyTable, 64, FIELDTYPE_NUMBER); //RetBRB
		CopyFieldValue(badTable, emptyTable, 65, FIELDTYPE_NUMBER); //RetEUR
		CopyFieldValue(badTable, emptyTable, 66, FIELDTYPE_NUMBER); //AgPercent
  */
		//CopyFieldValue(badTable, emptyTable, 67, FIELDTYPE_DATE);   //Ret1
		//CopyFieldValue(badTable, emptyTable, 68, FIELDTYPE_DATE);   //Ret2

	    emptyTable.AddRecord();
        Moved++;

		IgnoreMove:

		int isGotoOk = 0;
		if(!FromFirst) {
			if (IsGoto) {
				isGotoOk = PXRecGoto(badTable.getTblHandle(), badTable.GetRecords()-1-i-1);
			}
            else
			    badTable.Prev();
        }
		else {
			if(IsGoto) {
				isGotoOk = PXRecGoto(badTable.getTblHandle(), i+1);
			}
            else
				badTable.Next();
		}
		if (isGotoOk != PXSUCCESS)
			MessageBox(GetApplicationObject()->MainWindow->HWindow, "Не смог перейти", "Сообщаю", MB_OK | MB_ICONINFORMATION);
	 }

	 MessageBeep(0);
	 while(GetAsyncKeyState(VK_RETURN) & 0x8000);
	 while(GetAsyncKeyState(VK_ESCAPE) & 0x8000);
	 sprintf(msg, "Перенесено %d записей, из них подозрительных %d. Приложение завершит работу.", Moved, Bad);
	 //assert(strlen(msg) < sizeof msg);
	 MessageBox(GetApplicationObject()->MainWindow->HWindow, msg, "Сообщаю", MB_OK | MB_ICONINFORMATION);
	 PostQuitMessage(1);
    }

    FreeMemory:
    delete l_szDirName;
    delete l_szFile;
}

//Флаги для записи (использующиеся)
// N или Н(русская) //данные без RetBRB RetRUR
// not use Б //данные с RetBRB RetRUR
// P //данные с RetBRB RetRUR и процентом агента
// 8 данные датой сдачи в кассу

int aux_save_fields_to_file2(TOpenBase* base, FILE* f, char* Op, int Komplex)
{
	for(int x = 0; x <= 33; x++)
		CopyFieldValue(*base, f,  x);
	char UF[32];
    (*base)(UF, sizeof UF, 33);
    if(UF[0] != 'U' && UF[0] != 'F') {
	  double N;
	  (*base)(N, 17);
	  if(N > 0.01) { //Не испорчен
		(*base)(UF, sizeof UF, 0);
		(*base)(N, 1);
		strcat(UF, "/");
		sprintf(UF + strlen(UF), "%lu", (long)N);
		MessageBox(GetApplicationObject()->MainWindow->HWindow, "Не определён признак ЮРИД./ФИЗИЧ.", UF, MB_OK | MB_ICONSTOP);
		return 0;
	  }
	}

	for(x = 34; x <= 63; x++)
		CopyFieldValue(*base, f,  x);
	if(Op == NULL || *Op == 'P' || *Op == '8' || *Op == 'A') {
	    CopyFieldValue(*base, f, 64); //RetBRB
	    CopyFieldValue(*base, f, 65); //RetEUR
		CopyFieldValue(*base, f, 66); //AgPercent
    }
	if(Op == NULL || *Op == '8' || *Op == 'A') {
		CopyFieldValue(*base, f, 67); //Ret1
		CopyFieldValue(*base, f, 68); //Ret2
	}

	if(Op == NULL || *Op == 'A') {
		CopyFieldValue(*base, f, 69);
	}
	if(Op == NULL || *Op == 'C' || *Op == 'D') {
		CopyFieldValue(*base, f, 70); //Mob
		CopyFieldValue(*base, f, 71); //Home
	}
	if(Op == NULL || *Op == 'C' || *Op == 'D') {
		CopyFieldValue(*base, f, 72); //Contact
		CopyFieldValue(*base, f, 73); //SMS
	}

	if(Komplex)
	{
		for(x = 74; x <= 80; x++)
			CopyFieldValue(*base, f,  x);
	}

	return 1;
}
/*
void save_data(TOpenBase* base, char* Op, char* Ser, double Number, char* szModuleName)
{
    FILE* f = fopen(szModuleName, "ab");

    if(f) {
	fwrite(Op, 1, 1, f);

        if(*Op != 'D') {
		   aux_save_fields_to_file2(base, f, Op);
	} else {
	    int Len = strlen(Ser);
	    fwrite(&Len, sizeof Len, 1, f);
	    fwrite(Ser, strlen(Ser), 1, f);
	    fwrite(&Number, sizeof Number, 1, f);
	}
	fclose(f);
    }
}
/*
void MandatoryDlg::_SavePolisToFile(char* Op, char* Ser, double Number)
{
    char szModuleName[128];
    GetModuleFileName(GetApplication()->hInstance, szModuleName, sizeof(szModuleName)-32);
    *strrchr(szModuleName, '\\') = 0;
    strcat(szModuleName, "\\RTBK\\");

    date d;
    getdate(&d);

    char* fn = szModuleName + strlen(szModuleName);
    char filename[26];
    wsprintf(filename, "%02u%02u%04u.bkp", d.da_day, d.da_mon, d.da_year);
    strcat(szModuleName, filename);
    save_data(base, Op, Ser, Number, szModuleName);
    strcpy(fn, "fulldata.bkp");
    save_data(base, Op, Ser, Number, szModuleName);
}
*/
void MandatoryDlg::CMOwnPolis(RTMessage Msg)
{
	if(IsFee2->GetCheck() == 0)
		m_OwnPolis->SetCheck(0);

	ShowWindow(m_OwnPolis->HWindow, IsFee2->GetCheck() == 1);

	int OwnPolis = m_OwnPolis->GetCheck() == 0;

	static short list[] = {57, 300, 208, 200, 63, 64, 1000, 301, 992, 993, 999,  69, 79,80,84,52,/* ver 1437*/};

	UpdateFormData++;
	//101 can fill for 1SU
	for(int i = 101; i < 125; i++) {
		if(i == 102 || i == 104) continue;
		if(i != 101) EnableWindow(ChildWithId(i)->HWindow, OwnPolis);
		if(!OwnPolis) {
			SetWindowText(ChildWithId(i)->HWindow, "");
			SendMessage(ChildWithId(i)->HWindow, CB_SETCURSEL, -1, 0);
		}
	}
	for(i = 0; i < sizeof(list)/sizeof(*list); i++) {
		if(list[i] != 52) EnableWindow(ChildWithId(list[i])->HWindow, OwnPolis);
		if(!OwnPolis) {
			SetWindowText(ChildWithId(list[i])->HWindow, "");
			SendMessage(ChildWithId(list[i])->HWindow, CB_SETCURSEL, -1, 0);
		}
	}
	EnableWindow(ChildWithId(1001)->HWindow, OwnPolis);
	EnableWindow(ChildWithId(1002)->HWindow, OwnPolis);
	EnableWindow(ChildWithId(1003)->HWindow, OwnPolis);
	EnableWindow(ChildWithId(1005)->HWindow, OwnPolis);
	EnableWindow(ChildWithId(83)->HWindow, OwnPolis);
//	SendMessage(ChildWithId(1437)->HWindow, CB_SETCURSEL, 1, 0); //2 version
	UpdateFormData--;
}
/*
void MandatoryDlg::ChangeVersion(RTMessage msg)
{
	if(msg.LP.Hi == CBN_SELCHANGE) {
       MessageBeep(0);
       SetVersion(m_VersionCombo->GetSelIndex() + 1);
       EnableWindow(GetDlgItem(HWindow, 406), 1);
       if(Version == 2) {
	  m_Placement->SetSelIndex(0);
	  Placement(msg);
	  _m_OldClassAv->SetText("A0");
	  _m_CountAv->SetText("-1");
	  Calculate();
       }
       m_Resident->SetSelIndex(-1);
       m_Resident->ShowList();

       RECT r;
       GetWindowRect(m_Resident->HWindow, &r);
       SetCursorPos(r.left + 10, r.top + 30);
       //PostMessage(HWindow, 1, 0, 0);
    }
}
*/
void MandatoryDlg::OKRGL2(RTMessage msg)
{
	if(ModifyMandatory || IsWindowEnabled(GetDlgItem(HWindow, 406))) {
    	HWND W = GetDlgItem(HWindow, 67);
        ShowWindow(W, IsWindowVisible(W) ? SW_HIDE : SW_SHOW);
		ChangePay2Rate(msg);
	}
}

void MandatoryDlg::OKRGL(RTMessage msg)
{
	if(!ModifyMandatory) return;
    HWND W = GetDlgItem(HWindow, 972);
    ShowWindow(W, IsWindowVisible(W) ? SW_HIDE : SW_SHOW);
	CalculateSumma();
}

void MandatoryDlg::AfterDelete()
{
}

void MandatoryDlg::FindOwnerPart(RTMessage)
{

	FindOwner();
}

void MandatoryDlg::CityEditNotify(RTMessage)
{
	if(ModifyMandatory) {
	m_Placement->SetSelIndex(-1);
	char buff[10];
	m_CityEdit->GetText(buff, sizeof buff);
	char decode_val[10];
	if(GetPrivateProfileString(MANDATORY_SECT, buff, "", decode_val, sizeof decode_val, ::ININame)) {
	    long Val = atof(decode_val) * 100 + 0.0001;
	    for(int i = 0; i < m_Placement->GetCount(); i++)
	       if(SendMessage(m_Placement->HWindow, CB_GETITEMDATA, i, 0) == Val)
		  m_Placement->SetSelIndex(i);
	}
	if(m_Placement->GetSelIndex() == -1)
	    m_Placement->ShowList();
	Calculate();
	//PostMessage(HWindow, 1, 0, 0);
    }
}

void MandatoryDlg::SetCurrency()
{

	if(!ModifyMandatory) {
	MessageBeep(0);
	return;
    }
	SetCurrencyForPay(1);
}

void MandatoryDlg::ChangeRegDate(RTMessage msg)
{
	if(msg.LP.Hi != EN_CHANGE)
    	return;

	if(UpdateFormData) //Загрузка данных в форму из БД
  		return;

    if(!ModifyMandatory) {
		MessageBeep(0);
        return;
    }

    char regDateStr[32];
    char payDateStr[32];
    ((TEdit*)ChildWithId(52))->GetText(regDateStr, sizeof regDateStr);
    strcpy(payDateStr, regDateStr);
	DATE regDate = GD(regDateStr, 1);
    if(!regDate) return;

    if(Nal->GetCheck() == 1) {
		int m, d, y;
		int Delta = GetPrivateProfileInt(MANDATORY_SECT, "RegDateToStartAndPay", 1, ::ININame);
		PXDateDecode(regDate/* + Delta*/, &m, &d, &y);
		wsprintf(regDateStr, "%02d.%02d.%04d", d, m, y);
		((TEdit*)ChildWithId(121))->SetText(payDateStr); //PayDate
	}
	//e[6+2]->SetText("");
	InitTime(e[6+2]);
	Calculate();
}

char* MandatoryDlg::getMenuStr()
{

   static char menustr[32];
   wsprintf(menustr, "%lu/%lu/%lu", (long)base->NumberCurrent(), (long)base->GetRecords(), (long)avaria2.GetRecords());
   return menustr;
}

void MandatoryDlg::ChAgent1(RTMessage msg)
{

   int IsUr;
   if(msg.LP.Hi == CBN_SELCHANGE && ActivateParadox) {
	   m_AgPercent1->SetText(GetAgPercent(SendMessage(AgName1->HWindow, CB_GETITEMDATA, AgName1->GetSelIndex(), 0), MANDATOR_TBL, IsUr));
   }
}

void MandatoryDlg::ChAgent2(RTMessage msg)
{

   int IsUr;
   if(msg.LP.Hi == CBN_SELCHANGE && ActivateParadox) {
	   m_AgPercent2->SetText(GetAgPercent(SendMessage(AgName2->HWindow, CB_GETITEMDATA, AgName2->GetSelIndex(), 0), MANDATOR_TBL, IsUr));
   }
}

void MandatoryDlg::ChangeK2(RTMessage msg)
{

   if(UpdateFormData) return;

   if(ModifyMandatory && msg.LP.Hi == CBN_SELCHANGE) {
	  Calculate();
   }
}

void MandatoryDlg::CopyToAgent2(RTMessage)
{
	char buff[32];
	AgName2->SetSelIndex(AgName1->GetSelIndex());
	m_AgPercent1->GetText(buff, sizeof buff);
	m_AgPercent2->SetText(buff);
	m_UrFizich1->GetText(buff, sizeof buff);
	m_UrFizich2->SetText(buff);
}


void MandatoryDlg::BN(RTMessage)
{
	e[10+2]->SetText("БН");
}

double MandatoryDlg::GetK3()
{
}
