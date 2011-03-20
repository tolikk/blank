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
/*
char* GetRCStr(int strID);

class SizeEdit : public TEdit
{
        int       oldX, oldY, oldW, oldH;
        int       Size;

    public:
                  SizeEdit(PTWindowsObject AParent, int ResourceId, WORD ATextLen) :
                  TEdit(AParent, ResourceId, ATextLen) { Size = 0; }

             void SetupWindow();
             void WMRButtonDown(RTMessage) = [WM_FIRST + WM_RBUTTONDOWN];
	     void WMKeyDown(RTMessage) = [WM_FIRST + WM_KEYDOWN];
};

void SizeEdit::WMRButtonDown(RTMessage)
{
   SetFocus(HWindow);

   char NameEdit[50] = "?";
   char Caption[100];

   SendDlgItemMessage(Parent->HWindow, Attr.Id - 1, WM_GETTEXT, sizeof NameEdit, (long)NameEdit);
   wsprintf(Caption, Size ? "Страхование" : "Страхование [%s]", NameEdit);
   ((PTDialog)Parent)->SetCaption(Caption);

   for(int i = 101; i <= 103; i++) EnableWindow(GetDlgItem(Parent->HWindow, i), Size ? TRUE : FALSE);
   for(i = 406; i <= 409; i++) EnableWindow(GetDlgItem(Parent->HWindow, i), Size ? TRUE : FALSE);
   EnableWindow(GetDlgItem(Parent->HWindow, 500), Size ? TRUE : FALSE);

   if(Size)
    {
      SetSelection(0, 0);
      SetWindowPos(HWindow, 0, oldX, oldY, oldW, oldH, SWP_NOZORDER);
      Size = 0;
    }
   else
    {
      RECT r;
      GetWindowRect(GetDlgItem(Parent->HWindow, 555), &r);
      ScreenToClient(Parent->HWindow, (POINT*)&r);
      ScreenToClient(Parent->HWindow, (POINT*)&r.right);
      SetWindowPos(HWindow, HWND_TOP, r.left, r.top, r.right - r.left, r.bottom - r.top, SWP_SHOWWINDOW);
      Size = 1;
    }

   int Start = (Attr.Id / 50) * 50;
   for(i = Start; i < Start + 50; i++)
    {
      HWND w = GetDlgItem(Parent->HWindow, i);
      if(w && i != Attr.Id) ShowWindow(w, !Size ? SW_SHOW : SW_HIDE);
    }

   SetFocus(HWindow);
}

void SizeEdit::SetupWindow()
{
   TEdit::SetupWindow();
   oldX = Attr.X;
   oldY = Attr.Y;
   oldW = Attr.W;
   oldH = Attr.H;
}

int* HWNDarray;

char* Currency[5] = { "USD",
                      //"DM",
                      "RUR",
		      "BRB",
		      "EUR",
                      " "
                    };

int lenDB3[41] = { 3,   10, 101, 66, 66,  101,  101, 11,  7,  11,
		   356, 10, 11,  11, 11,  13,   4,   256, 13, 4,
		   256, 71, 11,  9,  13,  4,    121, 11,  41, 11,
		   4,   41, 13,  4,  121, 41,   501, 601, 11, 12, 91 };

char* StraxFields[] = {
                        "Seria",
                        "Number",
                        "Insurance",
                        "Address",
                        "Ins_form",
                        "Insurant",
                        "I_address",
                        "Regul_date",
						"Regul_number",
						"Statment_date",
						"Ins_object",
						"Persent",
						"Period",
						"Date_from",
						"Date_to",
						"Ins_sum",
						"Ins_cur",
						"Ins_sum_say",
						"Ins_pay",
						"Ins_pay_cur",
						"Ins_pay_say",
						"Payer",
						"Pay_date",
						"Pay_doc",
						"Ins_pay2",
						"Ins_cur2",
						"Ins_pay2_say",
						"Pay_date2",
						"Ins_term",
						"Forse_date",
						"Other",
						"Ins_other",
						"Sum_other",
						"Cur_other",
						"Sum_other_say",
						"Continue",
						"Incidents",
						"Special",
			"Date",
			"Original",
			"Sec_Number",
			"Invoice",
			"Incidents2",
			"Special2",
			"InsObj2",
			"Special3"
		      };

/////////////////////////////////////////////
class Mcombo : public TComboBox
{
    public:
                   Mcombo(PTWindowsObject, int, int);
      virtual WORD Transfer(Pvoid DataPtr, WORD TransferFlag);
              void SetupWindow();

              void WMCtlColor(RTMessage) = [WM_FIRST + WM_CTLCOLOR];
};

Mcombo::Mcombo(PTWindowsObject p, int a, int b)
       : TComboBox(p, a, b)
{
}

void Mcombo::WMCtlColor(RTMessage msg)
{
   ((PTDialog)Parent)->WMCtlColor(msg);
}

void Mcombo::SetupWindow()
{
   TComboBox::SetupWindow();
   for(int j = 0; j < 6; j++) AddString(Currency[j]);
}

WORD Mcombo::Transfer(Pvoid DataPtr, WORD TransferFlag)
{
   if(TransferFlag == TF_SETDATA)
    {
       SetSelIndex(-1);
       if(strlen((char*)DataPtr) < 2) SetSelIndex(-1);
       else SetText((char*)DataPtr);
    }

   if(TransferFlag == TF_GETDATA)
    {
       GetText((char*)DataPtr, TextLen);
    }

   return TextLen;
}

/////////////////////////////////////////////
class Mradio : public TRadioButton
{

    public:
       Mradio(PTWindowsObject, int);

       void ODADrawEntire(DRAWITEMSTRUCT _FAR &);

       void WMSetFocus(RTMessage) = [WM_FIRST + WM_SETFOCUS];

       int Check;
};

void Mradio::WMSetFocus(RTMessage)
{
   ((DB3*)Parent)->NoFocus();
   SendMessage(Parent->HWindow, WM_COMMAND, Attr.Id, -1);
   Check = 1;
}

                                            
Mradio::Mradio(PTWindowsObject p, int res)
       :TRadioButton(p, res, 0)
{
   Check = 0;
}

void Mradio::ODADrawEntire(DRAWITEMSTRUCT _FAR & i)
{
    COLORREF CR1 = RGB(255, 255, 255);
    COLORREF CR2 = RGB(128, 128, 128);

    FillRect(i.hDC, &i.rcItem, (HBRUSH)GetStockObject(LTGRAY_BRUSH));

    HDC Quadro = GetDC(GetDlgItem(Parent->HWindow, 501));
    RECT r;
    GetClientRect(GetDlgItem(Parent->HWindow, 501), &r);
    SelectObject(Quadro, GetStockObject(WHITE_PEN));
    MoveTo(Quadro, 0, 0);
    LineTo(Quadro, 0, r.bottom - 1);
    HGDIOBJ oldPen = SelectObject(Quadro, CreatePen(PS_SOLID, 1, RGB(128, 128, 128)));
    LineTo(Quadro, r.right - 1, r.bottom - 1);
    LineTo(Quadro, r.right - 1, 0);
    DeleteObject(SelectObject(Quadro, oldPen));
    ReleaseDC(GetDlgItem(Parent->HWindow, 501), Quadro);

    if(!Check)
     {
        CR1 ^= CR2 ^= CR1 ^= CR2;
	SetTextColor(i.hDC, RGB(0, 0, 0));

	HGDIOBJ p = SelectObject(i.hDC, CreatePen(PS_SOLID, 1, RGB(255, 255, 255)));
	MoveTo(i.hDC, i.rcItem.left, i.rcItem.bottom - 1);
        LineTo(i.hDC, i.rcItem.right, i.rcItem.bottom - 1);
        DeleteObject(SelectObject(i.hDC, p));
     }
    else
     {
        HGDIOBJ p = SelectObject(i.hDC, CreatePen(PS_SOLID, 1, RGB(192, 192, 192)));
        MoveTo(i.hDC, i.rcItem.left, i.rcItem.bottom - 1);
        LineTo(i.hDC, i.rcItem.right, i.rcItem.bottom - 1);
        DeleteObject(SelectObject(i.hDC, p));
        SetTextColor(i.hDC, RGB(0, 0, 255));
     }

    HGDIOBJ p = SelectObject(i.hDC, CreatePen(PS_SOLID, 1, CR1));
    MoveTo(i.hDC, i.rcItem.left, i.rcItem.bottom);
    LineTo(i.hDC, i.rcItem.left, i.rcItem.top + 5);
    LineTo(i.hDC, i.rcItem.left + 5, i.rcItem.top);
    LineTo(i.hDC, i.rcItem.right - 6, i.rcItem.top);
    DeleteObject(SelectObject(i.hDC, CreatePen(PS_SOLID, 1, CR2)));
    LineTo(i.hDC, i.rcItem.right - 1, i.rcItem.top + 5);
    LineTo(i.hDC, i.rcItem.right - 1, i.rcItem.bottom);
    DeleteObject(SelectObject(i.hDC, p));

	HGDIOBJ f = SelectObject(i.hDC, CreateFont(8, 0, 0, 0, Check ? FW_MEDIUM : FW_THIN, 0, 0, 0, 0, 0, 0, 0, 0, "MS Sans Serif"));
    SetBkMode(i.hDC, TRANSPARENT);

    char Text[25];
    GetWindowText(HWindow, Text, sizeof Text);
    DrawText(i.hDC, Text, strlen(Text), &i.rcItem, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
    DeleteObject(SelectObject(i.hDC, f));
}

int indexCheck;

void DB3::Check1(RTMessage)
{
   if(indexCheck == 1) return;
   indexCheck = 1;
   SetWindowPos(GetDlgItem(HWindow, 555), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
   for(int i  = 200; i < 350; i++)
    {
       HWND w;
       if(HWNDarray[i - 200] == -1) continue;
       w = GetDlgItem(HWindow, i);
       if(HWNDarray[i - 200] == 0) HWNDarray[i - 200] = (!w ? -1 : i);
       if(w) ShowWindow(w, i < 250 ? SW_SHOW : SW_HIDE);
    }
   SetWindowPos(GetDlgItem(HWindow, 555), HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);

   NoFocus();
   ((Mradio*)r[0])->Check = 1;
}

void DB3::Check2(RTMessage)
{
   if(indexCheck == 2) return;
   indexCheck = 2;
   SetWindowPos(GetDlgItem(HWindow, 555), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
   for(int i  = 200; i < 350; i++)
    {
       HWND w;
       if(HWNDarray[i - 200] == -1) continue;
       w = GetDlgItem(HWindow, i);
       if(HWNDarray[i - 200] == 0) HWNDarray[i - 200] = i;
       if(w) ShowWindow(w, ((i >= 250) && (i < 300)) ? SW_SHOW : SW_HIDE);
    }
   SetWindowPos(GetDlgItem(HWindow, 555), HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);

   NoFocus();
   ((Mradio*)r[1])->Check = 1;
}

void DB3::Check3(RTMessage)
{
   if(indexCheck == 3) return;
   indexCheck = 3;
   SetWindowPos(GetDlgItem(HWindow, 555), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
   for(int i = 200; i < 350; i++)
    {
       HWND w;
       if(HWNDarray[i - 200] == -1) continue;
       w = GetDlgItem(HWindow, i);
       if(HWNDarray[i - 200] == 0) HWNDarray[i - 200] = i;
       if(w) ShowWindow(w, i >= 300 ? SW_SHOW : SW_HIDE);
    }
   SetWindowPos(GetDlgItem(HWindow, 555), HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);

   NoFocus();
   ((Mradio*)r[2])->Check = 1;
}

void DB3::NoFocus()
{
   for(int i = 0; i < 3; i++)
    {
      ((Mradio*)r[i])->Check = 0;
      InvalidateRect(((Mradio*)r[i])->HWindow, 0, 0);
    }
}

/////////////////////////////////////////////

int  StraxCount = 46;
char* SName = "\\bases\\strax";

DB3::DB3(PTWindowsObject p)
    :Dlg_base(p, "STRAX", SName, StraxFields, StraxCount)
{
   if(!HWNDarray)
    {
      HWNDarray = new int[150];
      memset(HWNDarray, 0, sizeof(int) * 150);
    }

    for(int i = 0; i < 3; i++)
      {
        r[i] = new Mradio(this, 101 + i);
        r[i] -> DisableTransfer();
      }


   memset(e, 0, sizeof e);


   e[0] = new TEdit(this, 104, lenDB3[0]);
   e[1] = new TEdit(this, 105, lenDB3[1]);

   e[2] = new SizeEdit(this, 201, lenDB3[2]);

   for(i = 3; i < 10; i++)
       e[i] = new TEdit(this, 200 + (i - 1) * 2 - 1, lenDB3[i]);


   //???
   e[11] = new SizeEdit(this, 219, lenDB3[10]);
   e[10] = new TEdit(this, 217, lenDB3[11]);

   e[12] = new TEdit(this, 221, lenDB3[12]);
   e[13] = new TEdit(this, 223, lenDB3[13]);
   e[14] = new TEdit(this, 225, lenDB3[14]);

   e[15] = new TEdit(this, 251, lenDB3[15]);
//   e[16] = new TEdit(this, 253, lenDB3[16]);
   v[0] = new Mcombo(this, 253, lenDB3[16]);

   e[17] = new SizeEdit(this, 255, lenDB3[17]);
   e[18] = new TEdit(this, 257, lenDB3[18]);

//   e[19] = new TEdit(this, 259, lenDB3[19]);
   v[1] = new Mcombo(this, 259, lenDB3[19]);

   e[20] = new SizeEdit(this, 261, lenDB3[20]);
   e[21] = new TEdit(this, 263, lenDB3[21]);
   e[22] = new TEdit(this, 265, lenDB3[22]);
   e[23] = new TEdit(this, 267, lenDB3[23]);
   e[24] = new TEdit(this, 269, lenDB3[24]);

//   e[25] = new TEdit(this, 271, lenDB3[25]);
   v[2] = new Mcombo(this, 271, lenDB3[25]);

   e[26] = new SizeEdit(this, 273, lenDB3[26]);
   e[27] = new TEdit(this, 275, lenDB3[27]);
   e[28] = new TEdit(this, 277, lenDB3[28]);

   e[29] = new TEdit(this, 301, lenDB3[29]);
   e[30] = new TEdit(this, 303, lenDB3[30]);
   e[31] = new TEdit(this, 305, lenDB3[31]);
   e[32] = new TEdit(this, 307, lenDB3[32]);

//   e[33] = new TEdit(this, 309, lenDB3[33]);
   v[3] = new Mcombo(this, 309, lenDB3[33]);

   e[34] = new SizeEdit(this, 319, lenDB3[34]);
   e[35] = new TEdit(this, 311, lenDB3[35]);
   e[36] = new SizeEdit(this, 313, lenDB3[36]);
   e[37] = new SizeEdit(this, 315, lenDB3[37]);
   e[38] = new TEdit(this, 317, lenDB3[38]);

   e[39] = new TEdit(this, 107, lenDB3[39]);
   e[40] = new TEdit(this, 108, lenDB3[40]);

   Word = new TComboBox(this, 106, 26);
   Word->DisableTransfer();
}

extern void SetLittleFont(HWND HWindow, short* Numb, int nNumb);

void DB3::SetupWindow()
{
short Numb[] = { 316, 274, 250, 202, 204, 208, 210, 218, 220 };

   Dlg_base::SetupWindow();
   SetLittleFont(HWindow, Numb, 9);
   Word->AddString("ORIGINAL");
   Word->AddString("NON NEGOTIABLE COPY");
}

extern void DblToStr(char* str, double V, int Digit = 2);

void _DblToStr(char* str, double V)
{
   DblToStr(str, V);
   if(!strlen(str))
     GetPrivateProfileString("OPTIONS", "EMPTY_NUMBER", "", str, 5, ININame);
}
*/
void SetDateToBuffer(char* Buffer, DATE date)
{
   int    m, d, y;
   PXDateDecode(date, &m, &d, &y);
   if(y >= 2000)
       sprintf(Buffer, "%02u.%02u.%04u", d, m, y);
   else
       sprintf(Buffer, "%02u.%02u.%02u", d, m, y % 100);
}
/*

void DB3::GetDataFromBase()
{
   DATE   date;
   double N;
   int    m, d, y;

   memset(&buffer, 0, sizeof buffer);
   (*base)(buffer.Seria, 300, 0);

   (*base)(N, 1);
   DblToStr(buffer.Number, N);

   (*base)(buffer.Insurance, 300, 2);
   (*base)(buffer.Address, 300, 3);
   (*base)(buffer.Ins_form, 300, 4);
   (*base)(buffer.Insurant, 300, 5);
   (*base)(buffer.I_address, 300, 6);

   (*base)(date, 7);
   //PXDateDecode(date, &m, &d, &y);
   //sprintf(buffer.Regul_date, "%02u.%02u.%02u", d, m, y % 100);
   SetDateToBuffer(buffer.Regul_date, date);

   (*base)(buffer.Regul_number, 300, 8);

   (*base)(date, 9);
//   PXDateDecode(date, &m, &d, &y);
//   sprintf(buffer.Statment_date, "%02u.%02u.%02u", d, m, y % 100);
   SetDateToBuffer(buffer.Statment_date, date);

   (*base)(buffer.Ins_object, sizeof buffer.Ins_object, 10);
   (*base)(buffer.Ins_object + strlen(buffer.Ins_object), sizeof buffer.Ins_object, 44);

   (*base)(N, 11);
   _DblToStr(buffer.Persent, N);

   (*base)(buffer.Period, 300, 12);

   (*base)(date, 13);
   if(ISBLANKDATE(date))
      strcpy(buffer.Date_from, "*****");
   else {
//      PXDateDecode(date, &m, &d, &y);
//      sprintf(buffer.Date_from, "%02u.%02u.%01u", d, m, y % 100);
	SetDateToBuffer(buffer.Date_from, date);
   }

   (*base)(date, 14);
   if(ISBLANKDATE(date))
      strcpy(buffer.Date_to, "*****");
   else {
      if(date)
       {
         //PXDateDecode(date, &m, &d, &y);
	 //sprintf(buffer.Date_to, "%02u.%02u.%02u", d, m, y % 100);
	 SetDateToBuffer(buffer.Date_to, date);
       }
      else buffer.Date_to[0] = 0;
   }

   (*base)(N, 15);
   _DblToStr(buffer.Ins_sum, N);

   (*base)(buffer.Ins_cur, 300, 16);
   (*base)(buffer.Ins_sum_say, 300, 17);

   (*base)(N, 18);
   _DblToStr(buffer.Ins_pay, N);

   (*base)(buffer.Ins_pay_cur, 300, 19);
   (*base)(buffer.Ins_pay_say, 300, 20);
   (*base)(buffer.Payer, 300, 21);

   (*base)(date, 22);
   //PXDateDecode(date, &m, &d, &y);
   //sprintf(buffer.Pay_date, "%02u.%02u.%02u", d, m, y % 100);
   SetDateToBuffer(buffer.Pay_date, date);

   (*base)(buffer.Pay_doc, 300, 23);

   (*base)(N, 24);
   _DblToStr(buffer.Ins_pay2, N);

   (*base)(buffer.Ins_cur2, 300, 25);
   (*base)(buffer.Ins_pay2_say, 300, 26);

   (*base)(date, 27);
   if(date)
    { //PXDateDecode(date, &m, &d, &y);
      //sprintf(buffer.Pay_date2, "%02u.%02u.%02u", d, m, y % 100);
      SetDateToBuffer(buffer.Pay_date2, date);
    }
   else buffer.Pay_date2[0] = 0;

   (*base)(buffer.Ins_term, 300, 28);

   (*base)(date, 29);
//   PXDateDecode(date, &m, &d, &y);
//   sprintf(buffer.Forse_date, "%02u.%02u.%02u", d, m, y % 100);
   SetDateToBuffer(buffer.Forse_date, date);

   (*base)(buffer.Other, 300, 30);
   (*base)(buffer.Ins_other, 300, 31);

   (*base)(N, 32);
   _DblToStr(buffer.Sum_other, N);

   (*base)(buffer.Cur_other, 300, 33);
   (*base)(buffer.Sum_other_say, 300, 34);
   (*base)(buffer.Continue, 300, 35);
   (*base)(buffer.Incidents, sizeof buffer.Incidents, 36);
   (*base)(buffer.Special, sizeof buffer.Special, 37);
   (*base)(buffer.Incidents + strlen(buffer.Incidents), 300, 42);
   (*base)(buffer.Special + strlen(buffer.Special), 300, 43);
   (*base)(buffer.Special + strlen(buffer.Special), 300, 45);

   (*base)(date, 38);
//   PXDateDecode(date, &m, &d, &y);
//   sprintf(buffer.Date, "%02u.%02u.%02u", d, m, y % 100);
   SetDateToBuffer(buffer.Date, date);

   char str[50];
   (*base)(str, sizeof str, 39);
   Word->SetText(str);
   (*base)(buffer.Sec_Number, sizeof buffer.Sec_Number, 40);
   (*base)(buffer.Invoice, 300, 41);

   Dlg_base::GetDataFromBase();
}

void DB3::SetButtons()
{
   Dlg_base::SetButtons();
}
*/
DATE GD(char* str, int msg = 0)
{
   int  m, d, y;
   DATE D;

   char buff[15];
   memset(buff, 0, sizeof buff);
   strncpy(buff, str, 12);
   str[10] = 0;
   char* ch = strtok(str, "./");
   if(!ch)
     {
        bad_DATA:
		if(!msg) {
		  DynStr s;
		  sprintf(s, "Некорректная дата %s", buff);
		  MessageBox(GetActiveWindow(), s, "Сообщаю", MB_OK | MB_ICONINFORMATION | MB_TASKMODAL);
		}
        return 0;
     }
   d = atoi(ch);

   ch = strtok(0, "./");
   if(!ch)  goto bad_DATA;
   m = atoi(ch);

   ch = strtok(0, "./");
   if(!ch) goto bad_DATA;
   y = atoi(ch);

   if((y > 99 && y < 1910) || y > 2020 || y < 10)
	   goto bad_DATA;

   if(PXDateEncode(m, d, y, &D)) goto bad_DATA;
   return D;
}
/*
int DB3::SetDataToBase()
{
   memset(&buffer, 0, sizeof buffer);

   Dlg_base::SetDataToBase();

   DATE date;
   TMessage m;

   if(!strlen(buffer.Seria)) {
       MessageBeep(0);
       SetFocus(e[0]->HWindow);
       return 0;
   }
   (*base)(0, buffer.Seria);
   (*base)(1, atof(buffer.Number));
   (*base)(2, buffer.Insurance);
   (*base)(3, buffer.Address);
   (*base)(4, buffer.Ins_form);
   (*base)(5, buffer.Insurant);
   (*base)(6, buffer.I_address);

   date = GD(buffer.Regul_date);
   if(!date)
    {
       Check1(m);
       SetFocus(e[7]->HWindow);
       //MessageBox(0, "", "", 0);
       return 0;
    }

   (*base)(7, date);

   (*base)(8, buffer.Regul_number);
   date = GD(buffer.Statment_date);
   if(!date)
    {
       Check1(m);
       SetFocus(e[9]->HWindow);
       return 0;
    }

   (*base)(9, date);
   (*base)(10, buffer.Ins_object);
   (*base)(44, buffer.Ins_object + 255);
   (*base)(11, atof(buffer.Persent));
   (*base)(12, buffer.Period);

   if(*buffer.Date_from == '*') {
      date = BLANKDATE;
   }
   else {
      date = GD(buffer.Date_from);
      if(!date) {
         Check1(m);
         SetFocus(e[13]->HWindow);
         return 0;
      }
   }
   (*base)(13, date);

   if(*buffer.Date_to == '*') {
      date = BLANKDATE; //0x80000000
      //MessageBox(0, "", "blank", 0);
   }
   else {
      if(strlen(buffer.Date_to))
       {
         date = GD(buffer.Date_to);
         if(!date) {
           Check1(m);
           SetFocus(e[14]->HWindow);
           return 0;
         }
       }
      else date = 0;
   }

   (*base)(14, date);

   (*base)(15, atof(buffer.Ins_sum));
   (*base)(16, buffer.Ins_cur);
   (*base)(17, buffer.Ins_sum_say);
   (*base)(18, atof(buffer.Ins_pay));
   (*base)(19, buffer.Ins_pay_cur);
   (*base)(20, buffer.Ins_pay_say);
   (*base)(21, buffer.Payer);

   date = GD(buffer.Pay_date);
   if(!date)
    {
       Check2(m);
       SetFocus(e[22]->HWindow);
       return 0;
    }
   (*base)(22, date);

   (*base)(23, buffer.Pay_doc);
   (*base)(24, atof(buffer.Ins_pay2));
   (*base)(25, buffer.Ins_cur2);
   (*base)(26, buffer.Ins_pay2_say);

   if(strlen(buffer.Pay_date2))
    {
       date = GD(buffer.Pay_date2);
      if(!date)
        {
          Check2(m);
          SetFocus(e[27]->HWindow);
          return 0;
        }
    }
   else date = 0;
   (*base)(27, date);

   (*base)(28, buffer.Ins_term);

   date = GD(buffer.Forse_date);
   if(!date)
    {
       Check3(m);
       SetFocus(e[29]->HWindow);
       return 0;
    }
   (*base)(29, date);

   (*base)(30, buffer.Other);
   (*base)(31, buffer.Ins_other);
   (*base)(32, atof(buffer.Sum_other));
   (*base)(33, buffer.Cur_other);

   (*base)(34, buffer.Sum_other_say);
   (*base)(35, buffer.Continue);
   (*base)(36, buffer.Incidents);
   (*base)(37, buffer.Special);
   (*base)(42, buffer.Incidents + 255);
   (*base)(43, buffer.Special + 255);
   (*base)(45, buffer.Special + 510);

   date = GD(buffer.Date);
   if(!date)
    {
       Check3(m);
       SetFocus(e[38]->HWindow);
       return 0;
    }
   (*base)(38, date);

   char str[50];
   Word->GetText(str, sizeof str);
   (*base)(39, str);
   (*base)(40, buffer.Sec_Number);
   (*base)(41, buffer.Invoice);

   return 1;
}

DB3::~DB3()
{
    indexCheck = 0;
}

int DB3::Find()
{
    int mode = SEARCHFIRST;
    while(1)
     {
        int f = PXSrchFld(base->getTblHandle(), base->getRecHandle(), base->getField(1), mode);
        if(f != PXSUCCESS) break;

        mode = SEARCHNEXT;
        base->Get();
        //double N;
	//(*base)(N, 1);
	char Seria[25];
	(*base)(Seria, sizeof Seria, 0);
	if(!strcmp(Seria, buffer.Seria)) return 1;
	//if(N == atof(buffer.Number)) return 1;
     }

    return 0;
}

void DB3::Find(RTMessage)
{
   char b[100];
   if(!e[0]->GetText(b, sizeof b)) return;
   (*base)(0, b);
   if(!e[1]->GetText(b, sizeof b)) return;
   (*base)(1, atof(b));
   Dlg_base::SetDataToBase();
   //TransferData(TF_GETDATA);
   //if(!SetDataToBase()) return;

   if(Find())
    {
	  base->Get();
      GetDataFromBase();
    }
   else MessageBox(GetApplication()->MainWindow->HWindow, "Ничего не найдено...", "Сообщаю", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
}
*/
// Копирует в буфер печати dest дату date (с названием месяца)
// flag = 0 - год только последняя цыфра
// flag = 1 - год как есть
// flag = 2 - год полностью (с 19хх)
// 2000е полностью
void AddBufDate(char*&dest, char* date, int flag = 0)
{
    //int Language = GetMenuState(GetMenu(GetApplicationObject()->MainWindow->HWindow), 112, MF_BYCOMMAND) == MF_UNCHECKED ? 0 : 20;
	int Language = ((TRulerWin*)(GetApplicationObject()->MainWindow))->Language == 0 ? 0 : 20;
	dest += strlen(dest) + 1;
    strcpy(dest, date); // число копируем
    date += strlen(date) + 1; //Переходим в дате на месяц
	dest += strlen(dest) + 1; //Переходим в буфере на конец

	LoadString(GetApplicationObject()->hInstance, 120 - 1 + atoi(date) + Language, dest, 20); //Загружаем название месяца

    switch(((TRulerWin*)GetApplicationObject()->MainWindow)->MonthRegister) {
	case 0 : AnsiLower(dest);
	         break;
	case 1 : AnsiUpper(dest);
	         break;
	case 2 : AnsiLower(dest);
		 AnsiUpperBuff(dest, 1);
		 break;
    }

    date += strlen(date) + 1;  //Переходим в дате на год

    // Проверка на 2000
	if(atoi(date) < 2000 && !((TRulerWin*)GetApplicationObject()->MainWindow)->Is2000Full) { //Если условие не верно, (дата >= 2000), то пропускаем всё
		if(flag == 0)
			date += strlen(date) - 1; //Последняя цифра
		if(flag == 2)                           //
		 {
	    	strcpy((dest += strlen(dest) + 1), "19");
		    date += strlen(date) - 2;
		    strcat(dest, date);
		    return;
		 }
	}

    strcpy((dest += strlen(dest) + 1), date);
}

void divDate(char* b)
{
    int len = strlen(b);
    for(int i = 0; i < len; i++)
      if((b[i] == '.') || (b[i] == '/')) b[i] = 0;
}     
/*
void DB3::Print1()
{
/*   char* str = new char [4000];
   char* dest = str;
   int IsKomma = ((TRulerWin*)(GetApplication()->MainWindow))->bIsKomma;

   memset(str, 0, 4000);
   if(!SetDataToBase()) return;
   GetDataFromBase();
   if(!SetDataToBase()) return;

   Dlg_base::SetDataToBase();

   strcpy(dest, buffer.Insurance);
   strcat((dest += strlen(dest) + 1), buffer.Address);

   strcpy((dest += strlen(dest) + 1), buffer.Ins_form);

   strcpy((dest += strlen(dest) + 1), buffer.Insurant);
   if(IsKomma) strcat(dest, ", ");
   strcat(dest, buffer.I_address);

   strcpy((dest += strlen(dest) + 1), buffer.Regul_date);
   strcat(dest, " , ");
   strcat(dest, buffer.Regul_number);
   strcpy((dest += strlen(dest) + 1), buffer.Statment_date);
   strcpy((dest += strlen(dest) + 1), buffer.Ins_object);
   strcpy((dest += strlen(dest) + 1), buffer.Persent);
   strcpy((dest += strlen(dest) + 1), buffer.Period);

   divDate(buffer.Date_from);
   AddBufDate(dest, buffer.Date_from, 2);

   if(strlen(buffer.Date_to))
    {
      divDate(buffer.Date_to);
      AddBufDate(dest, buffer.Date_to, 2);
    }
   else
    {
      strcpy((dest += strlen(dest) + 1), "");
      strcpy((dest += strlen(dest) + 1), "");
      strcpy((dest += strlen(dest) + 1), "");
    }

   strcpy((dest += strlen(dest) + 1), buffer.Ins_sum);
   strcat(dest, "  ");
   strcat(dest, buffer.Ins_sum_say);
   strcpy((dest += strlen(dest) + 1), buffer.Ins_pay);
   strcat(dest, "  ");
   strcat(dest, buffer.Ins_pay_say);

   strcpy((dest += strlen(dest) + 1), buffer.Payer);
   strcpy((dest += strlen(dest) + 1), buffer.Pay_date);
   strcpy((dest += strlen(dest) + 1), buffer.Pay_doc);

   strcpy((dest += strlen(dest) + 1), buffer.Ins_pay2);
   strcat(dest, "  ");
   strcat(dest, buffer.Ins_pay2_say);

   strcpy((dest += strlen(dest) + 1), buffer.Pay_date2);
   strcpy((dest += strlen(dest) + 1), buffer.Ins_term);

   divDate(buffer.Forse_date);
   AddBufDate(dest, buffer.Forse_date, 2);
   strcpy((dest += strlen(dest) + 1), buffer.Other);
   strcpy((dest += strlen(dest) + 1), buffer.Ins_other);
   strcpy((dest += strlen(dest) + 1), buffer.Sum_other);
   strcpy((dest += strlen(dest) + 1), buffer.Continue);
   strcpy((dest += strlen(dest) + 1), buffer.Incidents);
   strcpy((dest += strlen(dest) + 1), buffer.Special);

   divDate(buffer.Date);
   AddBufDate(dest, buffer.Date, 2);

   Word->GetText((dest += strlen(dest) + 1), 50);
   strcpy((dest += strlen(dest) + 1), buffer.Sec_Number);

   ((TRulerWin*)Parent)->SetValues(str);
   Dlg_base::Print1();
   delete str;* /
}

void DB3::PreparePrintBlank2(char* str, int Is__)
{
/*   char* dest = str;
   int IsKomma = ((TRulerWin*)(GetApplication()->MainWindow))->bIsKomma;

   if(!SetDataToBase()) return;
   GetDataFromBase();
   if(!SetDataToBase()) return;

   strcpy(dest, buffer.Insurance);
   strcpy((dest += strlen(dest) + 1), buffer.Address);
   strcpy((dest += strlen(dest) + 1), buffer.Ins_form);
   strcpy((dest += strlen(dest) + 1), buffer.Insurant);
   if(IsKomma) strcat(dest, ", ");
   strcat(dest, buffer.I_address);
   AddBufDate(dest, buffer.Regul_date);
   strcpy((dest += strlen(dest) + 1), buffer.Regul_number);
   AddBufDate(dest, buffer.Statment_date);
   strcpy((dest += strlen(dest) + 1), buffer.Ins_object);
   strcpy((dest += strlen(dest) + 1), buffer.Persent);
   strcpy((dest += strlen(dest) + 1), buffer.Period);

   if(*buffer.Date_from != '*')
      AddBufDate(dest, buffer.Date_from);
   else {
      strcpy((dest += strlen(dest) + 1), "*****");
      strcpy((dest += strlen(dest) + 1), "******************");
      strcpy((dest += strlen(dest) + 1), "***");
   }

   //В поле даты что-то есть и не звёздочка
   if(strlen(buffer.Date_to) && (*buffer.Date_to != '*'))
    {
      AddBufDate(dest, buffer.Date_to, 2);
    }
   else
    if(*buffer.Date_to == '*') {
      strcpy((dest += strlen(dest) + 1), "*****");
      strcpy((dest += strlen(dest) + 1), "***************");
      strcpy((dest += strlen(dest) + 1), "*****");
    }
    else
    {
      //В поле впечатываем текст
      strcpy((dest += strlen(dest) + 1), ((TRulerWin*)(GetApplicationObject()->MainWindow))->TextTo2RP);
      strcpy((dest += strlen(dest) + 1), "");
      strcpy((dest += strlen(dest) + 1), "");
    }
   if(Is__ || (atof(buffer.Ins_sum) != 0)) {
	strcpy((dest += strlen(dest) + 1), buffer.Ins_sum);
	strcat(dest, "  ");
	strcat(dest, buffer.Ins_sum_say);
   }
   else
	strcpy((dest += strlen(dest) + 1), buffer.Ins_sum_say);
   if(Is__ || (atof(buffer.Ins_pay) != 0)) {
   	strcpy((dest += strlen(dest) + 1), buffer.Ins_pay);
	strcat(dest, "  ");
	strcat(dest, buffer.Ins_pay_say);
   }
   else
	strcpy((dest += strlen(dest) + 1), buffer.Ins_pay_say);
   strcpy((dest += strlen(dest) + 1), buffer.Ins_term);
   strcpy((dest += strlen(dest) + 1), buffer.Special);
   AddBufDate(dest, buffer.Forse_date);
   AddBufDate(dest, buffer.Date);

   Word->GetText(dest += strlen(dest) + 1, 50);
   strcpy((dest += strlen(dest) + 1), buffer.Sec_Number);
   strcpy((dest += strlen(dest) + 1), buffer.Invoice);* /
}

void DB3::Print2()
{
   char* str = new char [4000];
   memset(str, 0, 4000);
   PreparePrintBlank2(str, 1);
   ((TRulerWin*)Parent)->SetValues(str);
   Dlg_base::Print1();
   delete str;
}

void DB3::Print3()
{
/*   char* str = new char [4000];
   char* dest = str;
   int IsKomma = ((TRulerWin*)(GetApplication()->MainWindow))->bIsKomma;

   memset(str, 0, 4000);
   if(!SetDataToBase()) return;
   GetDataFromBase();
   if(!SetDataToBase()) return;

   strcpy(dest, buffer.Address);
   strcpy((dest += strlen(dest) + 1), buffer.Ins_form);

   strcpy((dest += strlen(dest) + 1), buffer.Insurant);
   if(IsKomma) strcat(dest, ", ");
   strcat(dest, buffer.I_address);

   AddBufDate(dest, buffer.Regul_date);
   strcpy((dest += strlen(dest) + 1), buffer.Regul_number);

   AddBufDate(dest, buffer.Statment_date);
   strcpy((dest += strlen(dest) + 1), buffer.Ins_object);
   strcpy((dest += strlen(dest) + 1), buffer.Persent);
   strcpy((dest += strlen(dest) + 1), buffer.Period);
   AddBufDate(dest, buffer.Date_from);


//   AddBufDate(dest, buffer.Date_to);
   if(strlen(buffer.Date_to))
    {
      AddBufDate(dest, buffer.Date_to);
    }
   else
    {
      strcpy((dest += strlen(dest) + 1), "");
      strcpy((dest += strlen(dest) + 1), "");
      strcpy((dest += strlen(dest) + 1), "");
    }



   strcpy((dest += strlen(dest) + 1), buffer.Ins_sum);
   strcat(dest, "  ");
   strcat(dest, buffer.Ins_sum_say);
   strcpy((dest += strlen(dest) + 1), buffer.Ins_pay);
   strcat(dest, "  ");
   strcat(dest, buffer.Ins_pay_say);

   AddBufDate(dest, buffer.Pay_date, 1);
   strcpy((dest += strlen(dest) + 1), buffer.Pay_doc);
   AddBufDate(dest, buffer.Pay_date);

   strcpy((dest += strlen(dest) + 1), buffer.Ins_pay2);
   strcat(dest, "  ");
   strcat(dest, buffer.Ins_pay2_say);
   AddBufDate(dest, buffer.Pay_date2);
   strcpy((dest += strlen(dest) + 1), buffer.Ins_term);

   AddBufDate(dest, buffer.Forse_date);
   strcpy((dest += strlen(dest) + 1), buffer.Other);
   strcpy((dest += strlen(dest) + 1), buffer.Ins_other);

   strcpy((dest += strlen(dest) + 1), buffer.Sum_other);
   strcat(dest, "  ");
   strcat(dest, buffer.Sum_other_say);

   strcpy((dest += strlen(dest) + 1), buffer.Incidents);
   strcpy((dest += strlen(dest) + 1), buffer.Special);

   AddBufDate(dest, buffer.Date);

   Word->GetText(dest += strlen(dest) + 1, 50);
   strcpy((dest += strlen(dest) + 1), buffer.Sec_Number);

   ((TRulerWin*)Parent)->SetValues(str);
   Dlg_base::Print1();
   delete str;* /
}

void DB3::Clear()
{
   for(int i = 0; i < 40; i++)
     if(e[i]) e[i]->SetText("");

   for(i = 0; i < 3; i++)
     v[i]->SetSelIndex(3);

   v[3]->SetSelIndex(-1);

   Word->SetText("");

   Default();
}

void DB3::Default()
{
   char ini[100];

   if(GetPrivateProfileString("STRAX", "Insurant", "", ini, sizeof ini, ININame))
     e[2]->SetText(ini);
   if(GetPrivateProfileString("STRAX", "Ins_Address", "", ini, sizeof ini, ININame))
     e[3]->SetText(ini);
   if(GetPrivateProfileString("STRAX", "Regul", "", ini, sizeof ini, ININame))
     e[7]->SetText(ini);
   if(GetPrivateProfileString("STRAX", "RegulNumb", "", ini, sizeof ini, ININame))
     e[8]->SetText(ini);
}

void DB3::Save()
{
   if(!SetDataToBase()) return;

   SetCursor(LoadCursor(0, IDC_WAIT));

   if(Find())
    {
      if(MessageBox(GetApplication()->MainWindow->HWindow, GetRCStr(568), "Сообщаю", MB_YESNO | MB_ICONQUESTION | MB_SYSTEMMODAL) == IDYES)
        {
           SetDataToBase();
           if(!PXRecUpdate(base->getTblHandle(), base->getRecHandle())) goto OK;
        }
    }
   else
    {
      SetDataToBase();
      if(base->AddRecord() == PXSUCCESS)
	 OK:MessageBox(Parent->HWindow, GetRCStr(569), "Сообщаю", MB_ICONINFORMATION | MB_OK | MB_SYSTEMMODAL);
      else {
         MessageBeep(0);
	 MessageBox(Parent->HWindow, GetRCStr(570), "Сообщаю", MB_ICONINFORMATION | MB_OK | MB_SYSTEMMODAL);
      }
    }

   SetButtons();
   PXSave();
}

void DB3::Help(RTMessage msg)
{
   if(msg.WParam == 1000)
      WinHelp(GetApplication()->MainWindow->HWindow, "blank.hlp", HELP_CONTEXT, HELP_STRAX);

   Dlg_base::Help(msg);
}

void DB3::Print4()
{
   char* str = new char [4000];

   PreparePrintBlank2(str, 0);

   ((TRulerWin*)Parent)->SetValues(str);
   Dlg_base::Print1();
   delete str;
}

short RedWindows[] = { 200, 218, 254, 260, 272, 312, 314, 318 };

void DB3::WMCtlColor(RTMessage msg)
{
   Dlg_base::WMCtlColor(msg);
   int Ok = FALSE;
   for(int i = 0; i < sizeof(RedWindows) / sizeof(*RedWindows); i++)
     if((HWND)msg.LP.Lo == GetDlgItem(HWindow, RedWindows[i]))
       Ok = TRUE;
   if(msg.LP.Hi == CTLCOLOR_STATIC && Ok)
	SetTextColor((HDC)msg.WParam, RGB(255, 0, 0));
}

void SizeEdit::WMKeyDown(RTMessage msg)
{
   if(msg.WParam == VK_F5)
     WMRButtonDown(msg);
   DefWndProc(msg);
}
				*/