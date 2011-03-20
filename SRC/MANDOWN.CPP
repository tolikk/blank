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
#include "editors.h"

#undef NDEBUG
#include "assert.h"

PTWindowsObject MainDlg;
void FillPlacement(TComboBox* PlacementCombo, const char* Key = "OwnerPlace%d", const char* = 0, int IsDiv = 1);
void AlignCombo(TComboBox* Combo);
char* TrimStr(char* str);
extern DATE GD(char* str, int = 0);

extern char* MANDATORY_SECT;

MandatoryDlgOwners::MandatoryDlgOwners(PTWindowsObject parent, OwnersData* buffer, DATE AMinDate, DATE AMaxDate)
		  : TDialog(GetApplicationObject()->MainWindow, "CAR_OWNERS", 0)
{
    MainDlg = parent;
    TransferBuffer = buffer;
    data = buffer;

    MinDate = AMinDate;
    MaxDate = AMaxDate;

    new UPSTREdit(ININame, MANDATORY_SECT, this, 101, 51, "Owner%d", "Owner%d_%d", 1); //name
    new TEdit(this, 102, 11); //Reg date
    new NumberEdit(this, 104, 15, 1); //Доплата
    new TEdit(this, 110, 11); //Платёжка
    new TEdit(this, 111, 11); //Дата платежа
    m_Fizich = new TCheckBox(this, 1001, 0);
    m_Uridich = new TCheckBox(this, 1002, 0);
    new TEdit(this, 106, 51); //Address
    new TEdit(this, 107, 31); //PravoTxt
    new NumberEdit(this, 108, 5, 0);
    new CityEdit(this, 109, 5, ININame);

    (PlacementCombo = new TComboBox(this, 103, 32))->DisableTransfer();
    (PravoCombo = new TComboBox(this, 105, 32))->DisableTransfer();
}

void MandatoryDlgOwners::ChCityCode()
{
     PlacementCombo->SetSelIndex(-1);
     char buff[10];
     ((PTEdit)ChildWithId(109))->GetText(buff, sizeof buff);
     char decode_val[10];
     if(GetPrivateProfileString(MANDATORY_SECT, buff, "", decode_val, sizeof decode_val, ININame)) {
	 long Val = atof(decode_val) * 100 + 0.0001;
	 for(int i = 0; i < PlacementCombo->GetCount(); i++)
	    if(SendMessage(PlacementCombo->HWindow, CB_GETITEMDATA, i, 0) == Val)
	       PlacementCombo->SetSelIndex(i);
     }
     if(PlacementCombo->GetSelIndex() == -1)
	 PlacementCombo->ShowList();
}

void MandatoryDlgOwners::DefWndProc(RTMessage Msg)
{
     if(Msg.Message == NM_CITYEDIT)
	 ChCityCode();

/*     if(Msg.Message == (ID_FIRST + 1001)) { //Fizich
         m_Uridich->SetCheck(0);
         m_Fizich->SetCheck(1);
     }
     if(Msg.Message == (ID_FIRST + 1002)) { //Uridich
	 m_Uridich->SetCheck(1);
         m_Fizich->SetCheck(0);
     }*/

     TDialog::DefWndProc(Msg);
}

/*void MandatoryDlgOwners::QQQ()
{
     PlacementCombo->SetSelIndex(-1);
     char buff[10];
     ((PTEdit)ChildWithId(109))->GetText(buff, sizeof buff);
     char decode_val[10];
     if(GetPrivateProfileString(MANDATORY_SECT, buff, "", decode_val, sizeof decode_val, ININame)) {
	 long Val = atof(decode_val) * 100 + 0.0001;
	 for(int i = 0; i < PlacementCombo->GetCount(); i++)
	    if(SendMessage(PlacementCombo->HWindow, CB_GETITEMDATA, i, 0) == Val)
	       PlacementCombo->SetSelIndex(i);
     }
     if(PlacementCombo->GetSelIndex() == -1)
	 PlacementCombo->ShowList();
}
*/
void MandatoryDlgOwners::TransferData(WORD Direction)
{
    TDialog::TransferData(Direction);
    if(Direction == TF_GETDATA) {
       PlacementCombo->GetText(data->Placement, sizeof data->Placement);
       data->Pravo = SendMessage(PravoCombo->HWindow, CB_GETITEMDATA, PravoCombo->GetSelIndex(), 0) / 100;
    }
    //if(Direction == TF_SETDATA) {
    //   struct date date_s;
    //   getdate(&date_s);
    //   sprintf(data->UpdateDate, "%02d.%02d.%04d", date_s.da_day, date_s.da_mon, date_s.da_year);
    //   SendDlgItemMessage(HWindow, 102, WM_SETTEXT, 0, (long)data->UpdateDate);
    //}
}

void MandatoryDlgOwners::SetupWindow()
{
    Center = 0;

    TDialog::SetupWindow();

    FillPlacement(PlacementCombo);
    AlignCombo(PlacementCombo);
    FillPlacement(PravoCombo, "Base_of_possession%d");
    AlignCombo(PravoCombo);

    EnableWindow(ChildWithId(101)->HWindow, data->OwnerCode <= 0);

    char buff[32];
    if(GetPrivateProfileString(MANDATORY_SECT, "CityCodeDef", "", buff, sizeof buff, ININame))
	SetDlgItemText(HWindow, 109, buff);

    ChCityCode();

    PlacementCombo->SetText(data->Placement);

    for(int i = 0; i < PravoCombo->GetCount(); i++)
      if(SendMessage(PravoCombo->HWindow, CB_GETITEMDATA, i, 0) == (int(data->Pravo) * 100))
	 PravoCombo->SetSelIndex(i);

    RECT r, pr, prr;
    GetWindowRect(HWindow, &r);
    GetClientRect(MainDlg->HWindow, &pr);
    GetWindowRect(MainDlg->HWindow, &prr);

    POINT p;
    p.x = pr.right - (r.right - r.left);
    p.y = (pr.bottom - (r.bottom - r.top)) / 2;
    ClientToScreen(MainDlg->HWindow, &p);
    //ScreenToClient(GetParent(HWindow), &p);
    SetWindowPos(HWindow, 0, p.x, p.y, 0, 0, SWP_NOSIZE);

    EnableWindow(GetDlgItem(HWindow, 1), data->OwnerCode <= 0);
}

void MandatoryDlgOwners::Fizich(RTMessage msg)
{
    m_Fizich->SetCheck(1);
    m_Uridich->SetCheck(0);
}

void MandatoryDlgOwners::Uridich(RTMessage msg)
{
    m_Uridich->SetCheck(1);
    m_Fizich->SetCheck(0);
}

void MandatoryDlgOwners::Ok(RTMessage msg)
{
   TransferData(TF_GETDATA);

   long K100 = SendMessage(PlacementCombo->HWindow, CB_GETITEMDATA, PlacementCombo->GetSelIndex(), 0);
   data->K = K100 / 100.;

   if(strlen(data->Name) == 0) {
      MessageBox(HWindow, "Введите имя владельца", "Ошибка", MB_OK | MB_ICONSTOP);
      SetFocus(ChildWithId(101)->HWindow);
      return;
   }
   if(strlen(data->CityCode) == 0) {
      MessageBox(HWindow, "Введите код города или района", "Ошибка", MB_OK | MB_ICONSTOP);
      SetFocus(ChildWithId(109)->HWindow);
      return;
   }
   if(data->K < 0.1 || data->K > 10) {
      MessageBox(HWindow, "Коэффициент не правильный", "Ошибка", MB_OK | MB_ICONSTOP);
      SetFocus(ChildWithId(103)->HWindow);
      return;
   }
   if(PravoCombo->GetSelIndex() == -1) {
      SetFocus(PravoCombo->HWindow);
      return;
   }

   DATE RegDate = GD(data->RegDate);
   if(!RegDate || RegDate < MinDate || RegDate >= MaxDate) {
      MessageBox(HWindow, "Дата регистрации не правильная", "Ошибка", MB_OK | MB_ICONSTOP);
      return;
   }

   if(atof(data->Summa) > 0.01) {
      if(RegDate == MinDate) {
	  MessageBox(HWindow, "Доплата нужна, если регистрировали после выдачи полиса", "Ошибка", MB_OK | MB_ICONSTOP);
          return;
      }
      else
      if(PlaceK_Form >= data->K) {
	  MessageBox(HWindow, "Доплата не нужна", "Ошибка", MB_OK | MB_ICONSTOP);
          return;
      }
   }
   else
   if((RegDate > MinDate) && (PlaceK_Form < data->K))
       //if(atof(data->Summa) < 0.01)
	  if(MessageBox(HWindow, "По моему доплата нужна. Да?", "Ответь", MB_YESNO | MB_ICONQUESTION) == IDYES)
	      return;
//   else
//      if(RegDate > MinDate) {
//	  if(MessageBox(HWindow, "Вы уверены, что доплата не нужна", "Ошибка", MB_YESNO | MB_ICONQUESTION) == IDNO)
//	      return;
//      }

//   if((RegDate > MinDate) && (PlaceK_Form < data->K))
//       if(atof(data->Summa) < 0.01)
//	  if(MessageBox(HWindow, "По моему доплата нужна. Да?", "Ошибка", MB_YESNO | MB_ICONQUESTION) == IDYES)
//	      return;

   if(atof(data->Summa) < 0.01) {
   	SetDlgItemText(HWindow, 110, "");
	SetDlgItemText(HWindow, 111, "");
   }
   else { //Есть доплата
       DATE PayDate = GD(data->PayDate);
       if(!PayDate) {
	   SetFocus(GetDlgItem(HWindow, 111));
	   return;
       }
       if(data->Uridich) {
	   if(strlen(data->PayDocNumber) < 1) {
	       SetFocus(GetDlgItem(HWindow, 110));
	       return;
	   }
       }
       //else {
       //	if(atof(data->PayNumber) > 0)
       //	  if(MessageBox(HWindow, "Вы уверены, что физическое лицо платило по безналу", "Ответь", MB_YESNO | MB_ICONQUESTION) == IDNO)
       //	      return;
       //}
   }

   TDialog::Ok(msg);

   TrimStr(data->Name);
   TrimStr(data->Address);
   TrimStr(data->Placement);
}

void MandatoryDlgOwners::Placement(RTMessage msg)
{
//   if(msg.LP.Hi == CBN_SELCHANGE) {
//      long data = SendMessage(PlacementCombo->HWindow, CB_GETITEMDATA, PlacementCombo->GetSelIndex(), 0);
//      char buf[32];
//      DblToStr(buf, data / 100., 1);
//      SendDlgItemMessage(HWindow, 104, WM_SETTEXT, 0, (long)buf);
//   }
}

