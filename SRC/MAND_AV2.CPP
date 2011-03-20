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
#include <mand_av2.h>
#include <checkbox.h>
#include <thwindow.h>
#include <help.h>
#include "radiobut.h"
#include "util.h"
#include "math.h"
#include "dateedit.h"
#include "editors.h"

void FillPlacement(TComboBox* PlacementCombo, const char* Key = "OwnerPlace%d", const char* = 0, int = 1);
void AlignCombo(TComboBox* Combo);
char* TrimStr(char* str);
extern const char* MANDATORY_SECT;
char* GetRCStr(int strID);
void SetLittleFont(HWND, short*, int);
extern DATE GD(char* str, int = 0);

void InitVal(TComboBox* Combo)
{
     Combo->AddString("BRB");
     Combo->AddString("EUR");
     Combo->AddString("RUR");
     Combo->AddString("USD");
 //    Combo->AddString("DM");
     Combo->SetSelIndex(0);
     AlignCombo(Combo);
}


class TAVPayDlg : public TDialog {
public:
	 TAVPayDlg(PTWindowsObject, AVPayData* d);
	 void SetupWindow();
	 void Ok(RTMessage Msg);

	 TRadioButton* m_Type1;
	 TRadioButton* m_Type2;

	 TRadioButton* m_SubType1;
	 TRadioButton* m_SubType2;

	 TRadioButton* m_Risk1;
	 TRadioButton* m_Risk2;
	 TRadioButton* m_Risk3;

	 TRadioButton* m_Recv1;
	 TRadioButton* m_Recv2;

	 TComboBox* m_Zatr;

	 TEdit* m_Name;
	 TEdit* m_Date;
	 NumberEdit* m_Sum;
	 TEdit* m_Doc;
	 TComboBox* m_Curr;
	 TCheckBox* m_Nal;
};

TAVPayDlg::TAVPayDlg(PTWindowsObject Obj, AVPayData* d)
		 : TDialog(Obj, "AVPAY")
{
	 m_Type1 = new TRadioButton(this, 114, 0);
	 m_Type2 = new TRadioButton(this, 115, 0);

	 m_Risk1 = new TRadioButton(this, 116, 0);
	 m_Risk2 = new TRadioButton(this, 117, 0);
	 m_Risk3 = new TRadioButton(this, 118, 0);

	 m_Zatr = new TComboBox(this, 106, 0);
	 m_Zatr->DisableTransfer();

	 m_Recv1 = new TRadioButton(this, 109, 0);
	 m_Recv2 = new TRadioButton(this, 119, 0);

	 m_SubType1 = new TRadioButton(this, 107, 0);
	 m_SubType2 = new TRadioButton(this, 108, 0);

	 m_Name = new TEdit(this, 110, 33);

	 m_Date = new TDateEdit(this, 101, 11);
	 m_Sum = new NumberEdit(this, 102, 11, 1);
	 m_Doc = new TEdit(this, 105, 11);
	 m_Nal = new TCheckBox(this, 104, 0);

	 m_Curr = new TComboBox(this, 103, 0);
	 m_Curr->DisableTransfer();

	 TransferBuffer = d;
}

void TAVPayDlg::Ok(RTMessage Msg)
{
	TransferData(TF_GETDATA);

	AVPayData* d = (AVPayData*)TransferBuffer;
	if(!GD(d->Date)) {
		SetFocus(m_Date->HWindow);
		return;
    }
	m_Curr->GetText(d->Curr, sizeof d->Curr);
	if(!*d->Curr) {
		SetFocus(m_Curr->HWindow);
		return;
    }
	if(atof(d->Sum) <= 0.01) {
		SetFocus(m_Sum->HWindow);
		return;
    }
	if(m_Zatr->GetSelIndex() < 0) {
		SetFocus(m_Zatr->HWindow);
		return;
    }
	d->Zatr = SendMessage(m_Zatr->HWindow, CB_GETITEMDATA, m_Zatr->GetSelIndex(), 0) / 100;

	TDialog::Ok(Msg);
}

void TAVPayDlg::SetupWindow()
{
	TDialog::SetupWindow();
	InitVal(m_Curr);
	m_Curr->SetText(((AVPayData*)TransferBuffer)->Curr);
	FillPlacement(m_Zatr, "Zatr%d"/*((AVPayData*)TransferBuffer)->Type1 ? "AvAskType%d" : "AvPayType%d"*/);

	AlignCombo(m_Zatr);
	for(int i = 0; i < m_Zatr->GetCount(); i++)
		if(SendMessage(m_Zatr->HWindow, CB_GETITEMDATA, i, 0) == ((AVPayData*)TransferBuffer)->Zatr * 100)
			m_Zatr->SetSelIndex(i);
}

/*void TAVPayDlg::OnChangeType1(RTMessage msg)
{
	if(msg.LP.Hi == BN_CLICKED) 
		FillPlacement(m_SubType, m_Type1->GetCheck() ? "AvAskType%d" : "AvPayType%d");
}

void TAVPayDlg::OnChangeType2(RTMessage msg)
{
	OnChangeType1(msg);
	//if(msg.LP.Hi == BN_CLICKED) 
	//	FillPlacement(m_SubType, m_Type1->GetCheck() ? "AvAskType%d" : "AvPayType%d");
}
*/
void DateFBToStr(TOpenBase* base, int field, char* buffer);
extern void DblToStr(char* str, double V, int Digit = 2);
//void FillPlacement(TComboBox* PlacementCombo, char* Key = "OwnerPlace%d", char* = 0, int = 1);
int NeedCalc = 0;

MandatoryDlgAvaria2::MandatoryDlgAvaria2(PTWindowsObject parent, Avaria2Data* buffer, char* Ser, double Nmb, AVPayData* PayData, const char* ININame)
		   : TDialog(GetApplicationObject()->MainWindow, "AVARIA2", 0),
			 m_FIOList(10, 0, 10),
			 m_PayData(PayData)
{
   this->ININame = ININame;
   strcpy(AvSeria, Ser);
   AvNumber = Nmb;
	
   TransferBuffer = buffer;
   data = buffer;

   m_Decision = new TComboBox(this, 105, 1);
   m_DocType =  new TComboBox(this, 106, 1);
   m_BadMan =   new TComboBox(this, 112, 1);
   m_VTR =      new TComboBox(this, 114, 1);
   m_VALUTA =   new TComboBox(this, 173, 1);
   m_VALUTA->DisableTransfer();
   m_PayList = new TListBox(this, 177);
   m_PayList->DisableTransfer();
   m_Decision->DisableTransfer();
   m_DocType->DisableTransfer();
   m_BadMan->DisableTransfer();
   m_VTR->DisableTransfer();

   m_N = new TEdit(this, 99, 3);
   m_UpdateDate = new TDateEdit(this, 100, 11);
   m_AvDate = new TDateEdit(this, 101, 11);
   m_AVPlace = new TEdit(this, 102, 51);
   m_WorkNmb = new TEdit(this, 103, 15);
   m_WriteDate = new TDateEdit(this, 104, 11);

   m_SN = new TEdit(this, 107, 15);
   m_FIO = new UPSTREdit(ININame, MANDATORY_SECT, this, 108, 51, "Owner%d", "Owner%d_%d", 1);
   m_FIOAddress = new UPSTREdit(ININame, MANDATORY_SECT, this, 111, 51);

   m_Staj = new NumberEdit(this, 113, 5, 1);
   m_CityCode = new CityEdit(this, 136, 4, ININame);
   m_BMCityCode = new CityEdit(this, 137, 4, ININame);

   m_TC_S_FAC_EUR = new NumberEdit(this, 115, 10, 1);
   m_TC_S_FAC_VAL = new NumberEdit(this, 116, 16, 1);
   m_TC_S_VOZ_EUR = new NumberEdit(this, 118, 10, 1);
   m_TC_S_VOZ_VAL = new NumberEdit(this, 119, 16, 1);
   m_TC_DATE_VOZ = new TEdit(this, 121, 11);

   m_HL_S_FAC_EUR = new NumberEdit(this, 122, 10, 1);
   m_HL_S_FAC_VAL = new NumberEdit(this, 123, 16, 1);
   m_HL_S_VOZ_EUR = new NumberEdit(this, 125, 10, 1);
   m_HL_S_VOZ_VAL = new NumberEdit(this, 126, 16, 1);
   m_HL_DATE_VOZ = new TEdit(this, 128, 11);

   m_IM_S_FAC_EUR = new NumberEdit(this, 129, 10, 1);
   m_IM_S_FAC_VAL = new NumberEdit(this, 130, 16, 1);
   m_IM_S_VOZ_EUR = new NumberEdit(this, 132, 10, 1);
   m_IM_S_VOZ_VAL = new NumberEdit(this, 133, 16, 1);
   m_IM_DATE_VOZ = new TEdit(this, 135, 11);
		 
   INIEdit* X = new INIEdit(ININame, MANDATORY_SECT, this, 140, 31, "AutoType%d", "AutoType%d_%d");

   DynStr buff;
   GetPrivateProfileString(MANDATORY_SECT, "AutoMarkFile", ININame, buff, buff._sizeof(), ININame);
   X->SetININame(buff);

   new TEdit(this, 148, 3);
   new NumberEdit(this, 149, 11, 1);
   new INIEdit(ININame, MANDATORY_SECT, this, 150, 11, "AutoNumber%d", "AutoNumber%d_%d", 1);
   new TEdit(this, 151, 21);

   //Decl
   Zajavl1 = new NumberEdit(this, 141, 15, 1);
   Zajavl2 = new NumberEdit(this, 142, 15, 1);
   Zajavl3 = new NumberEdit(this, 143, 15, 1);

   //LossCode + Date
   new TEdit(this, 144, 6);
   new TDateEdit(this, 145, 11);
   new TDateEdit(this, 147, 11);

   //SumPay
   new NumberEdit(this, 146, 15, 1);

   new NumberEdit(this, 153, 15, 1);
   new NumberEdit(this, 163, 15, 1);
   new NumberEdit(this, 165, 15, 1);
   new NumberEdit(this, 155, 15, 1);
   new NumberEdit(this, 164, 15, 1);
   new NumberEdit(this, 166, 15, 1);

   new TCheckBox(this, 152, 0);
   new TCheckBox(this, 154, 0);
   new TCheckBox(this, 159, 0);
   new TCheckBox(this, 160, 0);
   new TCheckBox(this, 161, 0);
   new TCheckBox(this, 162, 0);

   m_BMFizichUridichCombo = new TComboBox(this, 109, 1);
   m_BMIsResidentCombo = new TComboBox(this, 110, 1);
   m_BMOsnovan = new TComboBox(this, 138, 1);
   m_BMDiscountPlace = new TComboBox(this, 139, 1);
   m_BMFizichUridichCombo->DisableTransfer();
   m_BMIsResidentCombo->DisableTransfer();
   m_BMOsnovan->DisableTransfer();
   m_BMDiscountPlace->DisableTransfer();
}

void MandatoryDlgAvaria2::TransferData(WORD Direction)
{
    TDialog::TransferData(Direction);
    if(Direction == TF_GETDATA) {
       m_BadMan->GetText(data->BadMan, sizeof data->BadMan);
    }
    if(Direction == TF_SETDATA) {
       struct date date_s;
       getdate(&date_s);
       sprintf(data->UpdateDate, "%02d.%02d.%04d", date_s.da_day, date_s.da_mon, date_s.da_year);
       SendDlgItemMessage(HWindow, 100, WM_SETTEXT, 0, (long)data->UpdateDate);
    }
}
/*
int check_summ(char* s1, char* s2)
{
    if(*s1 && !s2 || !*s1 && *s2)
	return 0;
    if(*s1 && atof(s1) > atof(s2))
	return 0;
    return 1;
}
*/
char* AVPayFields[] = { "SER", "NMB", "N", "D", "SUM", "CURR", "NAL", "DOC", "TYPE", "ZATR", "RISK", "RCVR", "SUBTYPE2", "NAME" };
char* AVPayTableName = "\\AVPAYS";

void MandatoryDlgAvaria2::SetupWindow()
{
    NeedCalc = 0;
    Center = 0;
	char buff[85];

    TDialog::SetupWindow();

	TOpenBase base(AVPayTableName, AVPayFields, 14);

	int flag = SEARCHFIRST;

//	sprintf(buff, "%s %f %s", AvSeria, AvNumber, data->N);
//	MessageBox(0, buff, "", 0);

	base(0, AvSeria);
	base(1, AvNumber);
	base(2, atof(data->N));
	m_PayDataSize = 0;
	memset(m_PayData, 0, sizeof (*m_PayData) * 60);
	while(PXSrchKey(base.getTblHandle(), base.getRecHandle(), 3, flag) == PXSUCCESS) {
		base.Get();
		AVPayData* data2 = &m_PayData[m_PayDataSize];
		double V;
		DateFBToStr(&base, 3, data2->Date);
        base(V, 4);
		DblToStr(data2->Sum, V);
		base(data2->Curr, sizeof(data2->Curr), 5);
		short ShortVal;
		char Symb[2];
		base(Symb, sizeof Symb, 6);
		data2->Nal = *Symb == '1';
		base(data2->Doc, sizeof(data2->Doc), 7);

		base(Symb, sizeof Symb, 8);
		data2->Type1 = *Symb == '0';
		data2->Type2 = *Symb == '1';
		//MessageBox(0, Symb, Symb, 0);

		base(ShortVal, 9);
		data2->Zatr = ShortVal;

		base(Symb, sizeof Symb, 10);
		data2->Risk1 = *Symb == '0';
		data2->Risk2 = *Symb == '1';
		data2->Risk3 = *Symb == '2';

		base(Symb, sizeof Symb, 11);
		data2->Rcvr1 = *Symb == '0';
		data2->Rcvr2 = *Symb == '1';

		base(Symb, sizeof Symb, 12);
		data2->SubType1 = *Symb == '1';
		data2->SubType2 = *Symb == '2';

		base(data2->Name, sizeof data2->Name, 13);

		//m_PayData.add(*data2);
		m_PayList->AddString(data2->GetStr(buff));
		flag = SEARCHNEXT;
		m_PayDataSize++;
    }                                                

	short Little[] = { 5000, 5001, 5002, 5003, 6000, 6001, 6002, 6003, 7000, 7001, 7002, 7003, 152, 154, 159, 160, 161, 162, 177 };
	SetLittleFont(HWindow, Little, sizeof(Little) / sizeof Little[0]);

    RECT r;
    GetWindowRect(HWindow, &r);

    MoveWindow(HWindow, ru.x - (r.right - r.left), ru.y, r.right - r.left, r.bottom - r.top, 1);

    EnableWindow(GetDlgItem(HWindow, 140), data->BaseCarCode <= 0 && data->CarCode <= 0);
    EnableWindow(GetDlgItem(HWindow, 148), data->BaseCarCode <= 0 && data->CarCode <= 0);
    EnableWindow(GetDlgItem(HWindow, 149), data->BaseCarCode <= 0 && data->CarCode <= 0);
    EnableWindow(GetDlgItem(HWindow, 150), data->BaseCarCode <= 0 && data->CarCode <= 0);
	EnableWindow(GetDlgItem(HWindow, 151), data->BaseCarCode <= 0 && data->CarCode <= 0);

    AlignCombo(m_Decision);
    AlignCombo(m_DocType);
    AlignCombo(m_BadMan);
    AlignCombo(m_VTR);
    InitVal(m_VALUTA);

    FillPlacement(m_Decision, "AVDesision%d");
    for(int i = 0; i < m_Decision->GetCount(); i++)
	if(SendMessage(m_Decision->HWindow, CB_GETITEMDATA, i, 0) == (data->Decision * 100))
	    m_Decision->SetSelIndex(i);
    FillPlacement(m_DocType, "AVDocument%d");
    for(i = 0; i < m_DocType->GetCount(); i++)
	if(SendMessage(m_DocType->HWindow, CB_GETITEMDATA, i, 0) == (data->AVDocument * 100))
	    m_DocType->SetSelIndex(i);

    for(i = 0; i < m_FIOList.getItemsInContainer(); i++)
       m_BadMan->AddString((char*)(const char*)(String&)(m_FIOList[i]));

    if(*data->BadMan) {
       m_BadMan->SetText(data->BadMan);
       //if(m_BadMan->GetSelIndex() == -1)
	   //	MessageBox(HWindow, GetRCStr(518), data->BadMan, MB_OK | MB_ICONSTOP);
    }

    FillPlacement(m_VTR, "VTR%d");
    for(i = 0; i < m_VTR->GetCount(); i++)
	if(SendMessage(m_VTR->HWindow, CB_GETITEMDATA, i, 0) == (data->VTR * 100))
	    m_VTR->SetSelIndex(i);

   m_BMFizichUridichCombo->AddString("Физическое");
   m_BMFizichUridichCombo->AddString("Юридическое");
   m_BMFizichUridichCombo->AddString("ИП без Юр.лица");
   m_BMFizichUridichCombo->SetSelIndex(data->IsBMFizich == 1 ? 0 : data->IsBMFizich == 2 ? 2 : 1);
   AlignCombo(m_BMFizichUridichCombo);

   m_BMIsResidentCombo->AddString("Резидент");
   m_BMIsResidentCombo->AddString("Нерезидент");
   m_BMIsResidentCombo->SetSelIndex(data->IsBMResident ? 0 : 1);
   AlignCombo(m_BMIsResidentCombo);

   FillPlacement(m_BMOsnovan, "Base_of_possession%d");
   AlignCombo(m_BMOsnovan);
    for(i = 0; i < m_BMOsnovan->GetCount(); i++)
	if(SendMessage(m_BMOsnovan->HWindow, CB_GETITEMDATA, i, 0) == (data->BMOsnovan * 100))
	    m_BMOsnovan->SetSelIndex(i);

   FillPlacement(m_BMDiscountPlace, "OwnerPlaceAV%d");
   AlignCombo(m_BMDiscountPlace);
    for(i = 0; i < m_BMDiscountPlace->GetCount(); i++)
	if(SendMessage(m_BMDiscountPlace->HWindow, CB_GETITEMDATA, i, 0) == int(data->BMDiscount * 100 + 0.0000001))
	    m_BMDiscountPlace->SetSelIndex(i);

/*   m_VALUTA->SetText(data->VALUTA);
   m_Val1->SetText(data->TC_1_CUR);
   m_Val1_1->SetText(data->TC_2_CUR);
   m_Val2->SetText(data->HL_1_CUR);
   m_Val2_1->SetText(data->HL_2_CUR);
   m_Val3->SetText(data->IM_1_CUR);
   m_Val3_1->SetText(data->IM_2_CUR);
*/

   AVPayData x;
   if(m_PayDataSize == 0) {
	if(*data->TC_2_1VDate) { //ЗАЯВЛЕНО ОПЛАЧЕНО
		//AVPayData* data2 = &m_PayData[m_PayDataSize++];
	    memset(&x, 0, sizeof x);
	    x.Rcvr1 = 1;
		x.Risk1 = 1;
		strcpy(x.Date, data->TC_2_1VDate);
		strcpy(x.Curr, data->VALUTA);
		x.Zatr = 1; //Ущерб транспортному средству

		if(atof(data->DeclTC) > 0.) { //заявлено
			x.Type1 = 1; //Сумма выплаты - заявлено
			x.Type2 = !x.Type1;
			strcpy(x.Sum, data->DeclTC);
			m_PayList->AddString(x.GetStr(buff));
			m_PayData[m_PayDataSize++] = x;
		}
		if(atof(data->TC_2_1V) > 0.) { //оплачено
			//if(x.Rcvr1) MessageBox(0, "22", "", 0);
			x.Type1 = 0; //Сумма выплаты - оплачено
			x.Type2 = !x.Type1;
			x.SubType1 = 1;
			x.SubType2 = 0;
			strcpy(x.Sum, data->TC_2_1V);
			m_PayList->AddString(x.GetStr(buff));
			m_PayData[m_PayDataSize++] = x;
			//MessageBox(0, "11111111111", "", 0);
			//if(m_PayData[0].Rcvr1) MessageBox(0, "", "", 0);
			//if(x.Rcvr1) MessageBox(0, "22", "", 0);
			//m_PayData[0].Rcvr1 = 1;
		}
		if(data->IsTC1) { //оплачено
			x.Type1 = 0; //Сумма выплаты - заявлено
			x.Type2 = !x.Type1;
			x.SubType1 = 0;
			x.SubType2 = 1;
			strcpy(x.Sum, data->OcenTC);
			m_PayList->AddString(x.GetStr(buff));
			m_PayData[m_PayDataSize++] = x;
		}
		if(data->IsTC2) { //оплачено
			x.Type1 = 0; //Сумма выплаты - заявлено
			x.Type2 = !x.Type1;
			x.SubType1 = 0;
			x.SubType2 = 1;
			strcpy(x.Sum, data->OthTC);
			m_PayList->AddString(x.GetStr(buff));
			m_PayData[m_PayDataSize++] = x;
		}
	}
	if(*data->HL_2_1VDate) {
		//AVPayData* data2 = &m_PayData[m_PayDataSize++];
		//memset(data2, 0, sizeof *data2);
		memset(&x, 0, sizeof x);
		x.Risk2 = 1;
		strcpy(x.Date, data->HL_2_1VDate);
		strcpy(x.Curr, data->VALUTA);
		x.Zatr = 3; //Ущерб жизни и здоровья

		if(atof(data->DeclHL) > 0.) { //заявлено
			x.Type1 = 1; //Сумма выплаты - заявлено
			x.Type2 = !x.Type1;
			strcpy(x.Sum, data->DeclHL);
			m_PayList->AddString(x.GetStr(buff));
			m_PayData[m_PayDataSize++] = x;
		}
		if(atof(data->HL_2_1V) > 0.) { //оплачено
			x.Type1 = 0; //Сумма выплаты - оплачено
			x.SubType1 = 1;
			x.SubType2 = 0;
			x.Type2 = !x.Type1;
			strcpy(x.Sum, data->HL_2_1V);
			m_PayList->AddString(x.GetStr(buff));
			m_PayData[m_PayDataSize++] = x;
		}

		if(data->IsHL1) { //оплачено
			x.Type1 = 0; //Сумма выплаты - заявлено
			x.Type2 = !x.Type1;
			x.SubType1 = 0;
			x.SubType2 = 1;
			strcpy(x.Sum, data->OcenHL);
			m_PayList->AddString(x.GetStr(buff));
			m_PayData[m_PayDataSize++] = x;
		}
		if(data->IsHL2) { //оплачено
			x.Type1 = 0; //Сумма выплаты - заявлено
			x.Type2 = !x.Type1;
			x.SubType1 = 0;
			x.SubType2 = 1;
			strcpy(x.Sum, data->OthHL);
			m_PayList->AddString(x.GetStr(buff));
			m_PayData[m_PayDataSize++] = x;
		}
		//x.Type1 = atof(data->DeclHL) > 0; //Сумма выплаты - заявлено
		//x.Type2 = !x.Type1;
		//strcpy(x.Sum, atof(data->DeclHL) > 0 ? data->DeclHL : data->HL_2_1V);
		//if(x.Type2) x.SubType = 1;
		//else x.SubType = 3;
		//m_PayList->AddString(x.GetStr(buff));
	}
	if(*data->IM_2_1VDate) {
		//AVPayData* data2 = &m_PayData[m_PayDataSize++];
		//memset(data2, 0, sizeof *data2);
		memset(&x, 0, sizeof x);
		x.Risk3 = 1;
		strcpy(x.Date, data->IM_2_1VDate);
		strcpy(x.Curr, data->VALUTA);
		x.Zatr = 20; //Ущерб имуществу

		if(atof(data->DeclIM) > 0.) { //заявлено
			x.Type1 = 1; //Сумма выплаты - заявлено
			x.Type2 = !x.Type1;
			x.SubType1 = 1;
			x.SubType2 = 0;
			strcpy(x.Sum, data->DeclIM);
			m_PayList->AddString(x.GetStr(buff));
			m_PayData[m_PayDataSize++] = x;
		}
		if(atof(data->IM_2_1V) > 0.) { //оплачено
			x.Type1 = 0; //Сумма выплаты - оплачено
			x.SubType1 = 0;
			x.SubType2 = 1;
			x.Type2 = !x.Type1;
			strcpy(x.Sum, data->IM_2_1V);
			m_PayList->AddString(x.GetStr(buff));
			m_PayData[m_PayDataSize++] = x;
		}
		if(data->IsIM1) { //оплачено
			x.Type1 = 0; //Сумма выплаты - заявлено
			x.Type2 = !x.Type1;
			x.SubType1 = 0;
			x.SubType2 = 1;
			strcpy(x.Sum, data->OcenIm);
			m_PayList->AddString(x.GetStr(buff));
			m_PayData[m_PayDataSize++] = x;
		}
		if(data->IsIM2) { //оплачено
			x.Type1 = 0; //Сумма выплаты - заявлено
			x.Type2 = !x.Type1;
			x.SubType1 = 0;
			x.SubType2 = 1;
			strcpy(x.Sum, data->OthIM);
			m_PayList->AddString(x.GetStr(buff));
			m_PayData[m_PayDataSize++] = x;
		}
		//x.Type1 = atof(data->DeclIM) > 0.; //Сумма выплаты - заявлено
		//x.Type2 = !x.Type1;
		//strcpy(x.Sum, atof(data->DeclIM) > 0. ? data->DeclIM : data->IM_2_1V);
		//if(x.Type2) x.SubType = 1;
		//else x.SubType = 20;
		//m_PayList->AddString(x.GetStr(buff));
	}
   }

   SetValutaStr();
   EnableWindow(GetDlgItem(HWindow, 1), data->PaymentCode <= 0);

   if(data->OwnerCode > 0) {
       EnableWindow(GetDlgItem(HWindow, 108), 0);
       EnableWindow(GetDlgItem(HWindow, 109), 0);
       EnableWindow(GetDlgItem(HWindow, 110), 0);
       EnableWindow(GetDlgItem(HWindow, 111), 0);
       EnableWindow(GetDlgItem(HWindow, 137), 0);
       EnableWindow(GetDlgItem(HWindow, 138), 0);
       EnableWindow(GetDlgItem(HWindow, 139), 0);
   }

   NeedCalc = 1;
}

void ExpStr(_str s, int& IsRussian, int& IsEnglish, int& IsDigit, int& IsBad, int CheckMinus);

void MandatoryDlgAvaria2::Ok(RTMessage msg)
{
   int IsRussian, IsEnglish, IsDigit, IsBad;

   TransferData(TF_GETDATA);

   _str s;
   strcpy(s.s, data->BaseType);
   ExpStr(s, IsRussian, IsEnglish, IsDigit, IsBad, 1);
   if(IsRussian || IsBad) {
	   MessageBox(HWindow, GetRCStr(619), "Ошибка", MB_OK | MB_ICONSTOP);
	   return;
   }

   if(m_BMFizichUridichCombo->GetSelIndex() == -1) {
	 SetFocus(m_BMFizichUridichCombo->HWindow);
	 return;
   }
   data->IsBMFizich = m_BMFizichUridichCombo->GetSelIndex() == 0 ? 1 : m_BMFizichUridichCombo->GetSelIndex() == 2 ? 2 : 0;
   if(m_BMIsResidentCombo->GetSelIndex() == -1) {
	 SetFocus(m_BMIsResidentCombo->HWindow);
	 return;
   }
   data->IsBMResident = m_BMIsResidentCombo->GetSelIndex() == 0 ? 1 : 0;
   if(m_BMOsnovan->GetSelIndex() == -1) {
	 SetFocus(m_BMOsnovan->HWindow);
	 return;
   }
   data->BMOsnovan = SendMessage(m_BMOsnovan->HWindow, CB_GETITEMDATA, m_BMOsnovan->GetSelIndex(), 0) / 100;

   if(m_BMDiscountPlace->GetSelIndex() == -1) {
	 SetFocus(m_BMDiscountPlace->HWindow);
	 return;
   }
   data->BMDiscount = SendMessage(m_BMDiscountPlace->HWindow, CB_GETITEMDATA, m_BMDiscountPlace->GetSelIndex(), 0) / 100.;

   DATE AVdate = GD(data->AVDate, 1);
   if(!AVdate || AVdate < MinDate || AVdate > MaxDate) {
	MessageBox(HWindow, GetRCStr(519), "Ошибка", MB_OK | MB_ICONSTOP);
	SetFocus(m_AvDate->HWindow);
	return;
   }
   DATE date1 = GD(data->WrDate);
   if(!date1 || date1 < AVdate) {
   	SetFocus(m_WriteDate->HWindow);
	return;
   }

   for(int i = 0; i < m_PayDataSize; i++) {
		char buff[32];
		strcpy(buff, m_PayData[i].Date);
		if(GD(buff, 1) < AVdate) {
			MessageBox(HWindow, "Выплата до аварии", "Ошибка", MB_OK | MB_ICONSTOP);
			return;
        }
   }


   if(*data->SN) {
     if(!strchr(data->SN, '/') || strchr(data->SN, '/') == data->SN) {
	   MessageBox(HWindow, GetRCStr(520), "Ошибка", MB_OK | MB_ICONSTOP);
	   SetFocus(m_SN->HWindow);
	   return;
     }
   }

   if(m_Decision->GetSelIndex() == -1) {
	 SetFocus(m_Decision->HWindow);
	 return;
   }
   data->Decision = SendMessage(m_Decision->HWindow, CB_GETITEMDATA, m_Decision->GetSelIndex(), 0) / 100;

   
   if(*data->DtFreeRz) {
	   DATE FreeDt = GD(data->DtFreeRz);
	   if(!FreeDt || FreeDt<AVdate) {
	   SetFocus(ChildWithId(145)->HWindow);
		return;
       }
   }

   if(*data->DtClLoss) {
   		DATE Loss = GD(data->DtClLoss);
	   if(!Loss || Loss<AVdate) {
	   SetFocus(ChildWithId(147)->HWindow);
		return;
       }
   }
   else {
		if(!data->DtClLoss)  {
			if(MessageBox(HWindow, "Дата закрытия не указана. Продолжить?", "Ошибка", MB_YESNO | MB_ICONQUESTION) == IDNO) { 
				SetFocus(ChildWithId(147)->HWindow);
				return;
			}
        }
   }

   if(m_DocType->GetSelIndex() == -1) {
	 SetFocus(m_DocType->HWindow);
	 return;
   }
   data->AVDocument = SendMessage(m_DocType->HWindow, CB_GETITEMDATA, m_DocType->GetSelIndex(), 0) / 100;

   //if(m_BadMan->GetSelIndex() == -1) {
   //	 SetFocus(m_BadMan->HWindow);
   //	 return;
   //}
   if(m_CityCode->GetTextLen() == 0) {
	 SetFocus(m_CityCode->HWindow);
	 return;
   }
   if(m_BMCityCode->GetTextLen() == 0) {
	 SetFocus(m_BMCityCode->HWindow);
	 return;
   }
   if(m_VTR->GetSelIndex() == -1) {
	 SetFocus(m_VTR->HWindow);
	 return;
   }
   data->VTR = SendMessage(m_VTR->HWindow, CB_GETITEMDATA, m_VTR->GetSelIndex(), 0) / 100;

/*
   m_VALUTA->GetText(data->VALUTA, sizeof data->VALUTA);
   if(!*data->VALUTA) {
	 SetFocus(m_VALUTA->HWindow);
	 return;
   }

   if(*data->TC_2_1VDate && !GD(data->TC_2_1VDate)) {
	 SetFocus(m_TC_DATE_VOZ->HWindow);
	 return;
   }
   if(*data->HL_2_1VDate && !GD(data->HL_2_1VDate)) {
	 SetFocus(m_HL_DATE_VOZ->HWindow);
	 return;
   }
   if(*data->IM_2_1VDate && !GD(data->IM_2_1VDate)) {
	 SetFocus(m_IM_DATE_VOZ->HWindow);
	 return;
   }

   if(!check_summ(data->TC_1, data->TC_1V) ||
      !check_summ(data->TC_2_1, data->TC_2_1V) ||
      !check_summ(data->HL_1, data->HL_1V) ||
      !check_summ(data->HL_2_1, data->HL_2_1V) ||
      !check_summ(data->IM_1, data->IM_1V) ||
      !check_summ(data->IM_2_1, data->IM_2_1V)) {
       MessageBox(HWindow, "Проблема с суммами выплат", "Ошибка", MB_OK | MB_ICONSTOP);
	   return;
   }

   if(Zajavl1->GetTextLen() < 2 && Zajavl2->GetTextLen() < 2 && Zajavl3->GetTextLen() < 2) {
	   MessageBox(HWindow, "Нет заявленных сумм", "Ошибка", MB_OK | MB_ICONSTOP);
	return;
   }
*/
   if(m_PayDataSize == 0 || m_PayDataSize == 1 && m_PayData[0].Type1 == 0) {
	   MessageBox(HWindow, "Нет заявленных сумм", "Ошибка", MB_OK | MB_ICONSTOP);
	   return;
   }

   TrimStr(data->AVPlace);
   TrimStr(data->WorkNmb);
   TrimStr(data->FIO);
   TrimStr(data->FIOAddress);
   TrimStr(data->Marka);
   TrimStr(data->BaseType);
   TrimStr(data->BodyNo);
   TrimStr(data->AutoNmb);

   if(!*data->FIO) {
	MessageBox(HWindow, "Где ФИО потерпевшего?", "Ошибка", MB_OK | MB_ICONSTOP);
	return;
   }

//   char Marka[31];
//   char BaseType[3];
//   char AutoCharact[11];
//   char AutoNmb[11];
//   char BodyNo[21];
   if(*data->Marka || *data->BaseType || *data->AutoCharact || *data->AutoNmb || *data->BodyNo) 
     if(!*data->Marka || !*data->BaseType || !*data->AutoCharact || !*data->AutoNmb || !*data->BodyNo) {
		MessageBox(HWindow, "Не все данные о машине потерпевшего?", "Ошибка", MB_OK | MB_ICONSTOP);
	    return;
     }

   TDialog::Ok(msg);
}

void Calc(MandatoryDlgAvaria2* dlg)
{
    if(!NeedCalc) return;

    double Sum = 0;
    char text[16];

	GetDlgItemText(dlg->HWindow, 119, text, sizeof text);
    Sum += atof(text);
    GetDlgItemText(dlg->HWindow, 126, text, sizeof text);
    Sum += atof(text);
    GetDlgItemText(dlg->HWindow, 133, text, sizeof text);
    Sum += atof(text);

    if(IsDlgButtonChecked(dlg->HWindow, 152)) {
        GetDlgItemText(dlg->HWindow, 153, text, sizeof text);
        Sum += atof(text);
    }
    if(IsDlgButtonChecked(dlg->HWindow, 154)) {
	GetDlgItemText(dlg->HWindow, 155, text, sizeof text);
        Sum += atof(text);
    }

    if(IsDlgButtonChecked(dlg->HWindow, 159)) {
        GetDlgItemText(dlg->HWindow, 163, text, sizeof text);
        Sum += atof(text);
    }
    if(IsDlgButtonChecked(dlg->HWindow, 160)) {
        GetDlgItemText(dlg->HWindow, 164, text, sizeof text);
        Sum += atof(text);
    }

    if(IsDlgButtonChecked(dlg->HWindow, 161)) {
        GetDlgItemText(dlg->HWindow, 165, text, sizeof text);
        Sum += atof(text);
    }
    if(IsDlgButtonChecked(dlg->HWindow, 162)) {
        GetDlgItemText(dlg->HWindow, 166, text, sizeof text);
        Sum += atof(text);
	}

	for(int i = 0; i < dlg->m_PayDataSize; i++)
		Sum += atof(dlg->m_PayData[i].Sum);

	sprintf(text, "%lu", (long)Sum);
    SetDlgItemText(dlg->HWindow, 146, text);
}

void MandatoryDlgAvaria2::WMCommand(RTMessage msg)
{
    TDialog::WMCommand(msg);
    if(msg.LP.Hi == EN_CHANGE) {
	if (msg.WParam == 119 || msg.WParam == 153 || msg.WParam == 155 ||
	    msg.WParam == 126 || msg.WParam == 163 || msg.WParam == 164 ||
	    msg.WParam == 133 || msg.WParam == 165 || msg.WParam == 166)
	    Calc(this);
    }
    if(msg.LP.Hi == BN_CLICKED) { 
	if (msg.WParam == 152 || msg.WParam == 154 || msg.WParam == 159 ||
	    msg.WParam == 160 || msg.WParam == 161 || msg.WParam == 162)
	    Calc(this);

	int StartCopy = 0;
	if(msg.WParam == 170) StartCopy  = 115;
	if(msg.WParam == 171) StartCopy  = 122;
	if(msg.WParam == 172) StartCopy  = 129;

	if(StartCopy > 0) {
	    char buff[20];
	    GetDlgItemText(HWindow, StartCopy, buff, sizeof buff);
	    SetDlgItemText(HWindow, StartCopy + 3, buff);
	    GetDlgItemText(HWindow, StartCopy + 1, buff, sizeof buff);
	    SetDlgItemText(HWindow, StartCopy + 4, buff);
	    Calc(this);
	}

//	|| msg.WParam == 171 ||msg.WParam == 172) {
//           
//	  15->16 18->19
//	  122->123 125->126
//	  129->130 132->133
//	}
    }
}

void MandatoryDlgAvaria2::CityEditNotify(RTMessage msg)
{
    if(msg.LParam != (long)m_BMCityCode->HWindow) return;
    m_BMDiscountPlace->SetSelIndex(-1);
    char buff[10];
    m_BMCityCode->GetText(buff, sizeof buff);
    char decode_val[10];
    if(GetPrivateProfileString(MANDATORY_SECT, buff, "", decode_val, sizeof decode_val, ::ININame)) {
	long Val = atof(decode_val) * 100 + 0.0001;
	for(int i = 0; i < m_BMDiscountPlace->GetCount(); i++)
	   if(SendMessage(m_BMDiscountPlace->HWindow, CB_GETITEMDATA, i, 0) == Val)
	      m_BMDiscountPlace->SetSelIndex(i);
    }
    if(m_BMDiscountPlace->GetSelIndex() == -1)
	m_BMDiscountPlace->ShowList();
}

void UPSTREdit::WMChar(RTMessage msg)
{
    if(msg.WParam >= '0' && msg.WParam <= '9') {
	TEdit::DefWndProc(msg);
        return;
    }
    if(msg.WParam >= 'A' && msg.WParam <= 'Z') {
	TEdit::DefWndProc(msg);
        return;
    }
    if(msg.WParam >= 'a' && msg.WParam <= 'z') {
	TEdit::DefWndProc(msg);
        return;
    }
    if((char)msg.WParam >= 'А' && (char)msg.WParam <= 'Я') {
	TEdit::DefWndProc(msg);
        return;
    }
    if((char)msg.WParam >= 'а' && (char)msg.WParam <= 'я') {
	TEdit::DefWndProc(msg);
        return;
    }
    if(msg.WParam == '-') {
	TEdit::DefWndProc(msg);
        return;
    }
    if(msg.WParam == 8) {
	TEdit::DefWndProc(msg);
        return;
    }
    if(msg.WParam == ' ') {
	int s, e;
	GetSelection(s, e);
	if(s == e) {
	   if(s == 0)
	      return;
	   char Text[64];
	   GetText(Text, 64);
	   if(Text[s-1] == ' ')
	      return;
	   if(Text[s] == ' ')
	      return;
        }
	TEdit::DefWndProc(msg);
        return;
    }
    MessageBeep(0);
}

void MandatoryDlgAvaria2::OnChangeValuta(RTMessage msg)
{
    if(msg.LP.Hi == CBN_SELCHANGE)
	SetValutaStr();
}

void MandatoryDlgAvaria2::SetValutaStr()
{
    char s[5];
    m_VALUTA->GetText(s, sizeof s);
    SetDlgItemText(HWindow, 167, s);
    SetDlgItemText(HWindow, 168, s);
    SetDlgItemText(HWindow, 169, s);
    SetDlgItemText(HWindow, 174, s);
    SetDlgItemText(HWindow, 175, s);
    SetDlgItemText(HWindow, 176, s);
}

void MandatoryDlgAvaria2::OnAddPay(RTMessage)
{
	AVPayData data;
	TAVPayDlg* dlg = new TAVPayDlg(this, &data);
	if(GetApplication()->ExecDialog(dlg) == IDOK) {
		//AVPayData* data2 = new AVPayData;
        //*data2 = data;
		m_PayData[m_PayDataSize++] = data;
		char buff[85];
		m_PayList->AddString(data.GetStr(buff));
		Calc(this);
	}
}

void MandatoryDlgAvaria2::OnDelPay(RTMessage)
{                        
	if(m_PayList->GetSelIndex() >= 0) {
		delete &m_PayData[m_PayList->GetSelIndex()];
		memmove(&m_PayData[m_PayList->GetSelIndex()], &m_PayData[m_PayList->GetSelIndex() + 1], sizeof(m_PayData[0]) * (m_PayDataSize - m_PayList->GetSelIndex()));
		m_PayList->DeleteString(m_PayList->GetSelIndex());
		m_PayDataSize--;
		Calc(this);
	}
}

void MandatoryDlgAvaria2::OnEditPay(RTMessage)
{
	if(m_PayList->GetSelIndex() < 0) return;

	int Index = m_PayList->GetSelIndex();
	AVPayData* data2 = /*(AVPayData*)*/&m_PayData[m_PayList->GetSelIndex()];
	AVPayData data;
	data = *data2;

	//if(data.Type1) MessageBox(0, "1", "1", 0);
	//if(data.Type2) MessageBox(0, "2", "2", 0);

	TAVPayDlg* dlg = new TAVPayDlg(this, &data);
	if(GetModule()->ExecDialog(dlg) == IDOK) {
		//if(data.Type1) MessageBox(0, "1", "1", 0);
		//if(data.Type2) MessageBox(0, "2", "2", 0);
		char buff[85];
		m_PayList->DeleteString(Index);
		m_PayList->InsertString(data.GetStr(buff), Index);
		m_PayList->SetSelIndex(Index);
		*data2 = data;
        Calc(this);
	}
}

char* AVPayData::GetStr(char* buff)
{
   sprintf(buff, "%s. %s, (", (Type1 == 1 ? "Заявл" : "Выпл"), (Risk1 == 1 ? "ТС" : Risk2 == 1 ? "Ж/З" : Risk3 == 1 ? "ИМ" : "?"));
   char Key[10];
   char* s = buff + strlen(buff);
   for(int i = 0; i < 100; i++) {
	   sprintf(Key, "Zatr%d", i);
	   if(GetPrivateProfileString(MANDATORY_SECT, Key, "", s, 30, ININame) && atoi(s)== Zatr)
			break;
       else *s = 0;
   }
   sprintf(buff + strlen(buff), "), %s, %s %s, ", Date, Sum, Curr);
   return buff;
}

