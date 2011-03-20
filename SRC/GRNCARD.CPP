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
#include "help.h"
#include "listbox.h"
#include "array.h"
#include "util.h"
#include "editors.h"

/*
int canmovefocus = 1;

void AlignCombo(TComboBox*);
char* GetRCStr(int);

void NormalizeDate(int& y)
{
     if(y < 30) y += 2000;
     else if(y < 100 && y >= 30) y += 1900;
}

void SetAgent(char* ACode, TOpenBase& agents, PTComboBox AgName, int IsAddCode = 1);
void SetMenuText(HWND, char*, int);
void SetLittleFont(HWND, int*, int);
void DblToStr(char* str, double V, int Digit = 2);
*/
Process::Process(PTWindowsObject p)
       : TDialog(p, "PROCESS")
{
     s1 = new TStatic(this, 100, 100);
	 s2 = new TStatic(this, 101, 100);
	 s_time = new TStatic(this, 99, 100);
	 inittime = GetCurrentTime();
}

void Process::SetAll(long n)
{
     char buff[100];
     wsprintf(buff, "Найдено %lu", n);
     s2->SetText(buff);
}

void Process::SetPercent(long n)
{
     static long lastTime;
     static long lastPercent = 100;
     static double minTime;
	 char time_value[16];

	 if(n < lastPercent) {
		lastTime = GetCurrentTime();
		lastPercent = n;
		minTime = 9999999.;
		SetAll(0);
	 }

	 if(((GetCurrentTime() - inittime) / 1000) < 60)
		sprintf(time_value, "%d сек", (GetCurrentTime() - inittime) / 1000);
     else
		sprintf(time_value, "%d мин", (GetCurrentTime() - inittime) / 1000 / 60);
	 s_time->SetText(time_value);

     if(n > lastPercent) {
	RECT r;
	GetWindowRect(s1->HWindow, &r);
	ScreenToClient(HWindow, (POINT*)&r);
	ScreenToClient(HWindow, (POINT*)&r.right);
	HDC dc = GetDC(HWindow);
	SetBkMode(dc, TRANSPARENT);
	SetTextColor(dc, RGB(255, 128, 128));
	RECT rleft = r, rright = r;
	rleft.right = rright.left = r.left + (r.right - r.left) * n / 100;
	FillRect(dc, &rright, (HBRUSH)GetStockObject(WHITE_BRUSH));
	FillRect(dc, &rleft, (HBRUSH)GetStockObject(BLACK_BRUSH));

	double Speed1Percent = (GetCurrentTime() - lastTime) / 1000. / (n - lastPercent);
	double TotalTime = Speed1Percent * (100 - n);
	if(TotalTime > minTime )
	   TotalTime = minTime;
	wsprintf(time_value, "%d", int(TotalTime + 1));
	char* str_time = "сек.";
	minTime = TotalTime;
	if(TotalTime > 60) {
	   TotalTime /= 60;
	   str_time = "мин.";
	   sprintf(time_value, "%0.1f", TotalTime);
	}
	char buffer[32];
	wsprintf(buffer, "Осталось %s %s", time_value, str_time);
	r.left += 10;
	DrawText(dc, buffer, strlen(buffer), &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        ReleaseDC(HWindow, dc);

        lastTime = GetCurrentTime();
        lastPercent = n;
     }
}

//////////////////////////////////////////////

SelectStr::SelectStr(PTWindowsObject p, Array& a, char* s)
	   : TDialog(p, "SelectStr"),
	     array(a),
             buffer(s)
{
    List = new TListBox(this, 101);
}

void SelectStr::SetupWindow()
{
   TDialog::SetupWindow();
   for(int i = 0; i < array.getItemsInContainer(); i++)
      List->AddString((LPSTR)(LPCSTR)(String&)(array[i]));
   List->SetSelIndex(0);
}

void SelectStr::Ok(RTMessage Msg)
{
    List->GetString(buffer, List->GetSelIndex());
    TDialog::Ok(Msg);
}
/*
/////////////////////////////////////////////////////////////////////////

//TEdits
#define EDITORS 29
BYTE lenDB1[EDITORS] = { 5, 41, 81, 81, 51, 11, 11, 2, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 11, 21, 11, 6,
		   7, 11, 7, 11, 11, 9 };

extern void (*printFunc)(HDC);
extern int NextPage;

char DB1::Agent_Code[10];
char DB1::Agent_Name[70];

char* grn_cardFields[] = { "Seria",
                           "Car_num",
                           "Car_art",
                           "Owner",
                           "Firma",
                           "Date_from",
                           "Date_to",
                           "Litera",
                           "T1_1",
                           "T1_2",
                           "T1_3",
			   "T1_4",
                           "T1_5",
                           "T2_1",
                           "T2_2",
                           "T2_3",
                           "T2_4",
                           "T2_5",
                           "T2_6",
                           "T2_7",
                           "Location",
                           "Date",
			   "Time",
			   "Number",
			   "Greens_seria",
			   "Greens_number",
			   "Agent_code",
			   "Pay_Date",
			   "ValutaSum",
			   "ValutaType",
			   "ValutaSum2",
			   "ValutaType2"
			 };

int  grn_cardCount = 32;
char* GCName = "\\bases\\grn_card";

DB1::DB1(PTWindowsObject p)
    :Dlg_base(p, "DB1", GCName, grn_cardFields, grn_cardCount),
     agents(AgentDBName, AgentFields, 2)
{
   for(int i = 0; i < EDITORS; i++) {
     if ((i >=8 && i <= 17))
	 e[i] = new NumberEdit(this, 300 + i, lenDB1[i], i % 2);
     else
     if((i == 18) || (i == 27) || (i == 28))
	 e[i] = new NumberEdit(this, 300 + i, lenDB1[i], 1);
//     else if(i == 26) {
//	 e[i] = new NewMaskEdit(this, 300 + i, lenDB1[i]);
//     }
     else
	 e[i] = new TEdit(this, 300 + i, lenDB1[i]);
   }
   AgName = new TComboBox(this, 500, 200);
   AgName->DisableTransfer();
   ValutaType = new TComboBox(this, 501, 4);
   ValutaType->DisableTransfer();
   ValutaType2 = new TComboBox(this, 502, 4);
   ValutaType2->DisableTransfer();
   *OLDAGENT = 0;
}

void DB1::SetupWindow()
{
   Dlg_base::SetupWindow();
   if(agents.pxErr) 
      CloseWindow();
   else {
      agents.First();
      for(int i = 0; i < agents.GetRecords(); i++) {
	 char Tmp[100];
	 agents.Get();
	 agents(Tmp, 100, 1);
	 char AgCode[10];
	 agents(AgCode, 10, 0);
	 int Index = AgName->AddString(Tmp);
	 SendMessage(AgName->HWindow, CB_SETITEMDATA, Index, atoi(AgCode));
	 agents.Next();
     }
   }
   SetAgent(OLDAGENT, agents, AgName);
   //SetMenuText(GetApplication()->MainWindow->HWindow, DB1::Agent_Name, 2);

   EnableMenuItem(GetSubMenu(GetMenu(Parent->HWindow), 2), 0, MF_BYPOSITION | MF_GRAYED);
   int N = 420;
   SetLittleFont(HWindow, &N, 1);
   N = 327;
   SetLittleFont(HWindow, &N, 1);

   ValutaType->AddString("DM");
   ValutaType->AddString("USD");
   ValutaType->AddString("RUR");
   ValutaType->AddString("BRB");
   ValutaType->AddString("EUR");

   ValutaType2->AddString("");
   ValutaType2->AddString("DM");
   ValutaType2->AddString("USD");
   ValutaType2->AddString("RUR");
   ValutaType2->AddString("BRB");
   ValutaType2->AddString("EUR");

   GetDataFromBase();

   if(!base->GetRecords()) InitData = 0;

   AlignCombo(AgName);
}
*/
void SetAgent(char* ACode, TOpenBase& agents, PTComboBox AgName, int IsAddCode)
{
   agents(0, ACode);
   char Tmp[100];
   strcpy(Tmp, ACode);
   strcat(Tmp, " <код>");
   if(PXSrchFld(agents.getTblHandle(), agents.getRecHandle(), agents.getField(0), SEARCHFIRST)) {
	if(AgName->SetSelString(Tmp, -1) == -1) {
	   if(IsAddCode)
	     AgName->SetSelIndex(AgName->AddString(Tmp));
	}
   }
   else {
      agents.Get();
      agents(Tmp, 100, 1);
      AgName->SetSelIndex(AgName->FindExactString(Tmp, -1));
   }
}

void DblToStr(char* str, double V, int Digit)
{
   if(V)
   {
	 char format[10];
	 sprintf(format, "%s2.%df", "%", Digit);
	 char Tmp[25];
	 sprintf(Tmp, Digit == 2 ? "%2.2f" : format, V);
	 //MessageBox(0, Tmp, format, 0);
	 char* Comma = strchr(Tmp, '.');
	 BOOL Is3Null = Comma && (Comma[3] == '0');
	 if(Is3Null && Digit == 2) Comma[3] = 0;
     if(strstr(Tmp, ".00") && (Is3Null || Digit == 2)) *strstr(Tmp, ".0") = 0;
     strcpy(str, Tmp);
   }
   else str[0] = 0;
}
/*
void DB1::ToGreens()
{
   char str[10];
   e[0]->GetText(str, sizeof str);
   e[24]->SetText(str);
   e[23]->GetText(str, sizeof str);
   e[25]->SetText(str);
   SetFocus(e[25]->HWindow);
   e[25]->SetSelection(0, 10);
}

void SetDateToBuffer(char* Buffer, DATE date);

void DB1::AuxGCGetData(TOpenBase* base, DB1_data& buffer)
{
   DATE   data;
   double N;
   int    m, d, y;

   (*base)(buffer.Seria, sizeof buffer.Seria, 0);
   (*base)(buffer.Car_num, sizeof buffer.Car_num, 1);
   (*base)(buffer.Car_art, sizeof buffer.Car_art, 2);
   (*base)(buffer.Owner, sizeof buffer.Owner, 3);
   (*base)(buffer.Firma, sizeof buffer.Firma, 4);

   (*base)(data, 5);
   if(ISBLANKDATE(data))
     *buffer.Date_from = 0;
   else {
     //PXDateDecode(data, &m, &d, &y);
     //sprintf(buffer.Date_from, "%02u.%02u.%02u", d, m, y % 100);
     SetDateToBuffer(buffer.Date_from, data);
   }

   (*base)(data, 6);
   if(ISBLANKDATE(data))
     *buffer.Date_to = 0;
   else {
     //PXDateDecode(data, &m, &d, &y);
     //sprintf(buffer.Date_to, "%02u.%02u.%02u", d, m, y % 100);
     SetDateToBuffer(buffer.Date_to, data);
   }

   (*base)(buffer.Litera, sizeof buffer.Litera, 7);

   (*base)(N, 8);
   DblToStr(buffer.T1_1, N);
   (*base)(N, 9);
   DblToStr(buffer.T2_1, N, 3);
   (*base)(N, 10);
   DblToStr(buffer.T1_2, N);
   (*base)(N, 11);
   DblToStr(buffer.T2_2, N, 3);
   (*base)(N, 12);
   DblToStr(buffer.T1_3, N);
   (*base)(N, 13);
   DblToStr(buffer.T2_3, N, 3);
   (*base)(N, 14);
   DblToStr(buffer.T1_4, N);
   (*base)(N, 15);
   DblToStr(buffer.T2_4, N, 3);
   (*base)(N, 16);
   DblToStr(buffer.T1_5, N);
   (*base)(N, 17);
   DblToStr(buffer.T2_5, N, 3);
   (*base)(N, 18);
   DblToStr(buffer.T2_6, N);
   (*base)(N, 19);
   DblToStr(buffer.T2_7, N, 3);

   (*base)(buffer.Location, sizeof buffer.Location, 20);

   (*base)(data, 21);
   if(ISBLANKDATE(data))
     *buffer.Date = 0;
   else {
//     PXDateDecode(data, &m, &d, &y);
  //   sprintf(buffer.Date, "%02u.%02u.%02u", d, m, y % 100);
     SetDateToBuffer(buffer.Date, data);
   }

   (*base)(buffer.Time, sizeof buffer.Time, 22);

   (*base)(N, 23);
   DblToStr(buffer.Number, N);

   // Серия и номер доп. бланка можно не указывать
   (*base)(buffer.Greens_seria, sizeof buffer.Greens_seria, 24);
   if(strlen(buffer.Greens_seria)) {
	(*base)(N, 25);
	DblToStr(buffer.Greens_number, N);
   }
   else *buffer.Greens_number = 0;

   // Чтение установленного Агента из БД
   (*base)(OLDAGENT, sizeof OLDAGENT, 26);
   SetAgent(OLDAGENT, agents, AgName, 0);

   // Чтение возможно пустой даты платежа
   (*base)(data, 27);
   if(ISBLANKDATE(data))
     *buffer.Pay_Date = 0;
   else {
     SetDateToBuffer(buffer.Pay_Date, data);
      //PXDateDecode(data, &m, &d, &y);
      //sprintf(buffer.Pay_Date, "%02u.%02u.%04u", d, m, y);
   }

   //Валюта другая
   (*base)(N, 28);
   DblToStr(buffer.ValutaSum, N);

   (*base)(buffer.ValutaType, sizeof buffer.ValutaType, 29);
   ValutaType->SetSelIndex(-1);
   ValutaType->SetSelString(buffer.ValutaType, -1);

   //Валюта оплаты другая
   (*base)(N, 30);
   DblToStr(buffer.ValutaSum2, N);
   (*base)(buffer.ValutaType, sizeof buffer.ValutaType, 31);
   ValutaType2->SetSelIndex(0);
   ValutaType2->SetSelString(buffer.ValutaType, -1);

   SetAgent(OLDAGENT, agents, AgName);
}

void DB1::GetDataFromBase()
{
   InitData = 1;
   AuxGCGetData(base, buffer);
   Dlg_base::GetDataFromBase();
   InitData = 0;
}

void DB1::SetButtons()
{
   Dlg_base::SetButtons();
}

float DB1::CalcSumma()
{
   float res = 0;
   char str_tmp[100];
   int list[] = { 309, 311, 313, 315, 317, 318 };
   for(int i = 0; i < sizeof(list) / sizeof(*list); i++) {
	PTEdit edit_tmp = (PTEdit)ChildWithId(list[i]);
	edit_tmp->GetText(str_tmp, sizeof(str_tmp));
	res += atof(str_tmp);
   }
   return res;
}

int DB1::SetDataToBase()
{
   DATE   date;
   DATE   date_from;
   DATE   date_to;
   DATE   date_put;
   double N;
   int    m, d, y;
   int    badIndex = 5;
   int 	  DAYS = 0;

   Dlg_base::SetDataToBase();

   while(buffer.Seria[0] == ' ')
     strcpy(buffer.Seria, buffer.Seria + 1);
   float res = CalcSumma();

   int BADBLK = 0;
   if(res == 0) {
      if(MessageBox(HWindow, GetRCStr(506), "Отвечай поскорей", MB_YESNO | MB_ICONINFORMATION | MB_SYSTEMMODAL) == IDYES)
      {
	strcpy(buffer.Owner, "ИСПОРЧЕН");
	e[3]->SetText("ИСПОРЧЕН");
	BADBLK = 1;
       *buffer.T1_1 =
       *buffer.T1_2 =
       *buffer.T1_3 =
       *buffer.T1_4 =
       *buffer.T1_5 =
       *buffer.Litera =
       *buffer.Car_num =
       *buffer.Car_art =
       *buffer.Date_from =
       *buffer.Date_to =
       *buffer.ValutaSum =
       *buffer.ValutaSum2 =
       *buffer.Time =
       *buffer.Location =
       *buffer.Firma = 0;

      }
      else {
	 MessageBox(HWindow, "Укажите сумму страхования", "Сделай", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
         return 0;
      }
   }

   (*base)(0, buffer.Seria);
   (*base)(1, buffer.Car_num);
   (*base)(2, buffer.Car_art);
   (*base)(3, buffer.Owner);
   (*base)(4, buffer.Firma);

   char* ch;

   // Дата 1
   if(!BADBLK || *buffer.Date_from) {
     ch = strtok(buffer.Date_from, "./");
     if(!ch)
       {
	  bad_DATA:MessageBox(HWindow, "Некорректная дата", "Сообщаю", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
	  SetFocus(e[badIndex]->HWindow);
	  return 0;
       }
     d = atoi(ch);

     ch = strtok(0, "./");
     if(!ch)  goto bad_DATA;
     m = atoi(ch);

     ch = strtok(0, "./");
     if(!ch) goto bad_DATA;
     y = atoi(ch);
     NormalizeDate(y);

     if(PXDateEncode(m, d, y, &date_from)) goto bad_DATA;
     (*base)(5, date_from);
   }

   (*base)(7, buffer.Litera);

   InitData = 1; //Что бы сумма не сбрасывалась
   if(!strcmp(buffer.Litera, "A") || !strcmp(buffer.Litera, "А")) { //Только легковые
	for(int i = 310; i <= 317; i++) 
	   e[i-300]->SetText("");
	Dlg_base::SetDataToBase();
   }
   if(strchr("BDВ", *buffer.Litera)) { //Мотоцикл и мопед
	for(int i = 308; i <= 317; i++) 
	  if(i != 310 && i != 311)
	    e[i-300]->SetText("");
	Dlg_base::SetDataToBase();
   }
   if(strchr("CEСЕ", *buffer.Litera)) {
	for(int i = 308; i <= 317; i++)
	  if(i != 312 && i != 313)
	   e[i-300]->SetText("");
	Dlg_base::SetDataToBase();
   }
   if(*buffer.Litera == 'F') {
	for(int i = 308; i <= 313; i++)
	   e[i-300]->SetText("");
	Dlg_base::SetDataToBase();
   }
   InitData = 0;

   N = atof(buffer.T1_1);
   if(N) DAYS = N;
   (*base)(8, N);
   N = atof(buffer.T2_1);
   (*base)(9, N);
   N = atof(buffer.T1_2);
   if(N) DAYS = N;
   (*base)(10, N);
   N = atof(buffer.T2_2);
   (*base)(11, N);
   N = atof(buffer.T1_3);
   if(N) DAYS = N;
   (*base)(12, N);
   N = atof(buffer.T2_3);
   (*base)(13, N);
   N = atof(buffer.T1_4);
   if(N) DAYS = N;
   (*base)(14, N);
   N = atof(buffer.T2_4);
   (*base)(15, N);
   N = atof(buffer.T1_5);
   if(N) DAYS = N;
   (*base)(16, N);
   N = atof(buffer.T2_5);
   (*base)(17, N);
   N = atof(buffer.T2_6);
   (*base)(18, N);
   N = atof(buffer.T2_7);
   (*base)(19, N);

   if(!DAYS && !BADBLK) {
	  MessageBox(HWindow, GetRCStr(558), "Сообщаю", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
	  return 0;
   }

   // Дата 2
   if(!BADBLK || *buffer.Date_to) {
      if(!*buffer.Date_to) {
	  date_to = date_from + DAYS - 1;
	  PXDateDecode(date_to, &m, &d, &y);
          if(y < 1900) y+=1900;
	  wsprintf(buffer.Date_to, "%02u.%02u.%04u", d, m, y);
	  e[6]->SetText(buffer.Date_to);
      }
      badIndex = 6;
      ch = strtok(buffer.Date_to, "./");
      if(!ch) goto bad_DATA;
      d = atoi(ch);

      ch = strtok(0, "./");
      if(!ch)  goto bad_DATA;
      m = atoi(ch);

      ch = strtok(0, "./");
      if(!ch) goto bad_DATA;
      y = atoi(ch);

      NormalizeDate(y);
      if(PXDateEncode(m, d, y, &date_to)) goto bad_DATA;
      (*base)(6, date_to);
   }

   (*base)(20, buffer.Location);

   // Дата 3
   if(!BADBLK || *buffer.Date) {
      badIndex = 21;
      ch = strtok(buffer.Date, "./");
      if(!ch) goto bad_DATA;
      d = atoi(ch);

      ch = strtok(0, "./");
      if(!ch)  goto bad_DATA;
      m = atoi(ch);

      ch = strtok(0, "./");
      if(!ch) goto bad_DATA;
      y = atoi(ch);
      NormalizeDate(y);

      PXDateEncode(m, d, y, &date_put);
      (*base)(21, date_put);
   }

   if ((date_from >= date_to) && !BADBLK) {
	MessageBox(HWindow, GetRCStr(553), "Сообщаю", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
	return 0;
   }

   if ((abs(date_to - date_from + 1 - DAYS) > 5) && !BADBLK) {
	MessageBox(HWindow, GetRCStr(554), "Сообщаю", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
	return 0;
   }

   if ((date_put > date_from) && !BADBLK) {
	MessageBox(HWindow, GetRCStr(555), "Сообщаю", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
	return 0;
   }

   (*base)(22, buffer.Time);

   N = atof(buffer.Number);
   (*base)(23, N);

   // Возможно не указана серия доп. полиса
   if(*buffer.Greens_seria) {
      (*base)(24, buffer.Greens_seria);

      N = atof(buffer.Greens_number);
      (*base)(25, N);
   }

   //(*base)(26, Agent_Code); //Write Current Agent
   //SetAgent(Agent_Code, agents, AgName);//MessageBox(0, Agent_Code, "set", 0);


   int Index = AgName->GetSelIndex();
   if(Index == -1) {
      MessageBox(HWindow, "Введи агента", "Ошибка", MB_OK | MB_ICONSTOP);
      return 0;	
   }
   char AgCode[10];
   wsprintf(AgCode, "%04d", SendMessage(AgName->HWindow, CB_GETITEMDATA, Index, 9));
   (*base)(26, AgCode);


   // Дата Pay
   if (*buffer.Pay_Date) {
      badIndex = 26;
      ch = strtok(buffer.Pay_Date, "./");
      if(!ch) goto bad_DATA;
      d = atoi(ch);

      ch = strtok(0, "./");
      if(!ch)  goto bad_DATA;
      m = atoi(ch);

      ch = strtok(0, "./");
      if(!ch) goto bad_DATA;
      y = atoi(ch);
      NormalizeDate(y);

      if(PXDateEncode(m, d, y, &date)) goto bad_DATA;
      (*base)(27, date);
    }

//  if(!BADBLK) {
       N = atof(buffer.ValutaSum);
       if(BADBLK) N = 0;
       (*base)(28, N);

       if(N==0) *buffer.ValutaType = 0;
       else ValutaType->GetText(buffer.ValutaType, sizeof buffer.ValutaType);
       (*base)(29, buffer.ValutaType);


       N = atof(buffer.ValutaSum2);
       if(BADBLK) N = 0;
       (*base)(30, N);

       if(N==0) *buffer.ValutaType = 0;
       else ValutaType2->GetText(buffer.ValutaType, sizeof buffer.ValutaType);

       if(N != 0 && ValutaType2->GetSelIndex() < 1) {
	    MessageBeep(0);
	    return 0;
       }

       (*base)(31, buffer.ValutaType);
//   }

   return 1;
}

int DB1::Update()
{
   // Если операция обновления, то перезаписать агента старого
   //(*base)(26, OLDAGENT);
   //SetAgent(OLDAGENT, agents, AgName); //MessageBox(0, OLDAGENT, "update", 0);
}

DB1::~DB1()
{
    //EnableMenuItem(GetSubMenu(GetMenu(Parent->HWindow), 1), 5, MF_BYPOSITION | MF_ENABLED);
    //EnableMenuItem(GetSubMenu(GetMenu(Parent->HWindow), 1), 6, MF_BYPOSITION | MF_ENABLED);
    //EnableMenuItem(GetSubMenu(GetMenu(Parent->HWindow), 1), 7, MF_BYPOSITION | MF_ENABLED);
    EnableMenuItem(GetSubMenu(GetMenu(Parent->HWindow), 2), 0, MF_BYPOSITION | MF_ENABLED);
    SetMenuText(GetApplication()->MainWindow->HWindow, "", 2);
}

int DB1::FindSN()
{
   char b[100];
   char nmb[50];

   if(e[0]->GetText(b, sizeof b) && e[23]->GetText(nmb, sizeof nmb))
    {
       char* bad;
       double Number = strtod(nmb, &bad);
       (*base)(0, b);
       (*base)(23, Number);
       if(!PXSrchKey(base->getTblHandle(), base->getRecHandle(), 2, SEARCHFIRST) && !*bad)
        {
           base->Get();
	   GetDataFromBase();
	   //MessageBox(0, "FindOK", "", 0);
	   return 1;                
        }
    }
   return 0;
}

int DB1::FindTransport()
{
   char b[100];
   if(e[1]->GetText(b, sizeof b)) {
       (*base)(1, b);
       if(!PXSrchFld(base->getTblHandle(), base->getRecHandle(), base->getField(1), SEARCHFIRST))
	{
           base->Get();
	   GetDataFromBase();
           return 1;
	}
   }
   return 0;
}

int DB1::FindDZK()
{
   char b[100];
   char nmb[50];
   if(e[24]->GetText(b, sizeof b) && e[25]->GetText(nmb, sizeof nmb))
    {
       char* bad;
       double Number = strtod(nmb, &bad);
       if(*bad) return -1;
       (*base)(25, Number);
       int findMode = SEARCHFIRST;
       int Find = 0;
       while(!PXSrchFld(base->getTblHandle(), base->getRecHandle(), base->getField(25), findMode)) {
	   base->Get();
	   char seria[5];
	   (*base)(seria, sizeof seria, 24);
	   if(!strcmp(b, seria)) {
               Find = 1;
	       GetDataFromBase();
               return 1;
	       //break;
	   }
	   findMode = SEARCHNEXT;
       }
       if(!Find) return 0;
    }
   return 0;
}

int DB1::FindOwner()
{
   char b[100];
   if(e[3]->GetText(b, sizeof b))
    {
       Array Strings(10, 0, 10);
       (*base)(3, b);
       base->First();
       char str[100], seria[15], owner[100];
       double number;
       Process*	findWnd = base->GetRecords() > 50 ? (Process*)GetModule()->MakeWindow(new Process(this)) : 0;
       SetCursor(LoadCursor(0, IDC_WAIT));
       for(long i = 0; i < base->GetRecords(); i++) {
	   base->Get();
	   double number;
	   (*base)(seria, sizeof seria, 0);
	   (*base)(owner, sizeof owner, 3);
	   if((strlen(owner) >= strlen(b) &&
	      !strncmpi(owner, b, strlen(b))) || (strstr(owner, b))) {
	       (*base)(number, 23);
	       wsprintf(str, "%s/%lu %s", seria, (long)number, owner);
	       Strings.add(*new String(str));
	       (*base)(3, b);
	       if(findWnd) findWnd->SetAll(Strings.getItemsInContainer());
	   }
	   base->Next();
	   if(findWnd) findWnd->SetPercent(i * 100 / base->GetRecords());
       }
       if(findWnd) delete findWnd;
       int nCount = Strings.getItemsInContainer();
       if(nCount == 0) {
	 MessageBox(HWindow, GetRCStr(557), "Сообщаю", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
	 return -1;
       }
       else {
	  strcpy(str, (LPSTR)(LPCSTR)(String&)(Strings[0]));
	  if(nCount > 1)
	   if(GetModule()->ExecDialog(new SelectStr(this, Strings, str)) != IDOK) return -1;
       }
       (*base)(0, strtok(str, "/"));
       (*base)(23, atof(strtok(0, " ")));
       if(!PXSrchKey(base->getTblHandle(), base->getRecHandle(), 2, SEARCHFIRST)) {
	   base->Get();
	   GetDataFromBase();
           return 1;
       }
    }
    return 0;
}

void DB1::Find()
{
   base->Refresh();
   HMENU Popup = CreatePopupMenu();
   AppendMenu(Popup, MF_ENABLED | MF_STRING, 700, "По Серии/Номеру");
   AppendMenu(Popup, MF_ENABLED | MF_STRING, 701, "По номеру транспортного средства");
   AppendMenu(Popup, MF_ENABLED | MF_STRING, 702, "По Серии/Номеру ДЗК");
   AppendMenu(Popup, MF_ENABLED | MF_STRING, 703, "По ФИО владельца");
   POINT ptCurrent;
   GetCursorPos(&ptCurrent);
   TrackPopupMenu(Popup, TPM_LEFTALIGN, ptCurrent.x, ptCurrent.y, 0, HWindow, NULL);
   DestroyMenu(Popup);
}

void  DB1::WMCommand(RTMessage msg)
{
   if(msg.Message == WM_COMMAND) {
      int Find = -1;
      if(msg.WParam == 700) Find = FindSN();
      if(msg.WParam == 701) Find = FindTransport();
      if(msg.WParam == 702) Find = FindDZK();
      if(msg.WParam == 703) Find = FindOwner();
      if(Find == 0)
	 MessageBox(HWindow, GetRCStr(556), "Ошибка поиска", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
   }
   if(msg.LP.Hi == EN_CHANGE)
    if(msg.WParam == 309 || msg.WParam == 311 || msg.WParam == 313 || msg.WParam == 315 || msg.WParam == 317 || msg.WParam == 318)
      if(!InitData) {
           char str[15];
	   DblToStr(str, CalcSumma(), 3);
	   e[19]->SetText(strcpy(buffer.T2_7, str));
	   canmovefocus = 0;
	   ToValuta();
	   canmovefocus = 1;
      }
   Dlg_base::WMCommand(msg);
}

void DB1::Print1()
{
   char str[500];
   char* ch = str;
   char* b = (char*)&buffer;
   int j = lenDB1[0];

   memset(str, 0, sizeof str);
   if(!SetDataToBase()) return;

   for(int i = 1; i < 8; i++)
     {
        if((i > 4) && (i < 7))
         {
	    int copy = e[i]->GetText(ch, 11); //Печать дат
            strtok(ch, "./");
            strtok(0, "./");
            ch += copy + 1;
         }
        else
         {
           strcpy(ch, &b[j]);
           ch += strlen(ch) + 1;
         }

        j += lenDB1[i];
     }

   ((TRulerWin*)Parent)->SetValues(str);
   Dlg_base::Print1();
}

void DB1::Print2()
{
   char str[500];
   char* ch = str;
   char* b = (char*)&buffer;
   int j = 0;

   ch += 11;

   for(int i = 0; i < 8; i++)
    j += lenDB1[i];

   memset(str, 0, sizeof str);
   if(!SetDataToBase()) return;

   for(i = 8; i < 23; i++)
     {
        if((i == 21))
         {
            e[i]->GetText(ch, 11);
         }
        else strcpy(ch, &b[j]);

        ch += strlen(ch) + 1;
        j += lenDB1[i];
     }

   ((TRulerWin*)Parent)->SetValues(str);
   Dlg_base::Print1();
}

void DB1::PrintAll()
{
   char str[500];
   char* ch = str;
   char* b = (char*)&buffer;
   int j = lenDB1[0];

   memset(str, 0, sizeof str);
   if(!SetDataToBase()) return;

   for(int i = 1; i < 8; i++)
     {
	if((i > 4) && (i < 7)) //Печать дат
         {
            int copy = e[i]->GetText(ch, 11);
            strtok(ch, "./");
            strtok(0, "./");
            ch += copy + 1;
         }
        else
         {
           strcpy(ch, &b[j]);
           ch += strlen(ch) + 1;
         }

        j += lenDB1[i];
     }

   for(i = 8; i < 23; i++)
     {
        if((i == 21))
         {
            e[i]->GetText(ch, 11);
         }
        else strcpy(ch, &b[j]);

        ch += strlen(ch) + 1;
        j += lenDB1[i];
     }

   ((TRulerWin*)Parent)->SetValues(str);
   Dlg_base::Print1();
}

void DB1::Clear()
{
   for(int i = 1; i < EDITORS; i++)
    if(i != 24)
     e[i]->SetText("");

   ValutaType->SetSelIndex(-1);
}

void DB1::Help(RTMessage msg)
{
   if(msg.WParam == 1000)
      WinHelp(GetApplication()->MainWindow->HWindow, "blank.hlp", HELP_CONTEXT, HELP_GREEN);

   Dlg_base::Help(msg);
}

void  DB1::ToValuta()
{       
   ValutaType->SetSelIndex(0);
   char buff[10];
   e[19]->GetText(buff, sizeof buff);
   e[EDITORS-2]->SetText(buff);

   if(canmovefocus)
     SetFocus(e[EDITORS-2]->HWindow);

   e[EDITORS-1]->SetText("");
   ValutaType2->SetSelIndex(0);
}
/////////////////////////////////////////////////////////////////////////
/*
char* Capt = "Отчет \"Зеленая карта\" за %s %d г.";

rptDB1::rptDB1(PTWindowsObject p, int m, int y)
       : TWindow(p, ""),
        array(10, 0, 10)

{
   static offX = 10;
   month = m;
   year = y;
   Attr.Style = WS_POPUP | WS_VISIBLE | WS_SYSMENU | WS_HSCROLL | WS_VSCROLL | WS_THICKFRAME | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
   Scroller = new TScroller(this, 20, 20, 30, 30);
   Scroller->XPos = Scroller->YPos = 5;
   CurPage = 0;
   report = this;
   Attr.X = 50 + offX;
   Attr.Y = 30 + offX;
   offX = (offX + 10) % 100;

   fontrpt.lfHeight = 45;
   fontrpt.lfWidth = 0;
   fontrpt.lfEscapement = 0;
   fontrpt.lfOrientation = 0;
   fontrpt.lfWeight = 0;
   fontrpt.lfItalic = 0;
   fontrpt.lfUnderline = 0;
   fontrpt.lfStrikeOut = 0;
   fontrpt.lfCharSet = ANSI_CHARSET;
   fontrpt.lfOutPrecision = OUT_DEFAULT_PRECIS;
   fontrpt.lfClipPrecision = CLIP_DEFAULT_PRECIS;
   fontrpt.lfQuality = PROOF_QUALITY;
   fontrpt.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;
   strcpy(fontrpt.lfFaceName, "NTHelvetica Narrow");

   char Caption[100];
   char mn[20] = "";

   LoadString(GetApplication()->hInstance, m + 99, mn, sizeof mn);
   sprintf(Caption, Capt, mn, year);
   SetCaption(Caption);
}

rptDB1::~rptDB1()
{
   if(base) delete base;
   report = 0;
}

void rptDB1::SetupWindow()
{
   TWindow::SetupWindow();

   AssignMenu(101);
   char fullName[100];
   GetModuleFileName(GetModule()->hInstance, fullName, sizeof fullName);
   *strrchr(fullName, '\\') = 0;
   strcat(fullName, GCName);
   base = new TOpenBase(fullName, grn_cardFields, grn_cardCount);

   if(base->pxErr)
    {
       ShowWindow(HWindow, SW_HIDE);
       CloseWindow();
    }

   DATE start, end;

   PXDateEncode(month, 1, year, &start);
   int m = (month == 12) ? 1 : month + 1;
   int y = (month == 12) ? year + 1 : year;
   PXDateEncode(m, 1, y, &end);

   for(int i = 0; i < base->GetRecords(); i++)
    {
       base->Get();
       DATE blank;

      (*base)(blank, 5);

      if((blank >= start) && (blank < end))
       {
          Object&o = *new ODB1();
          ((ODB1&)o).Read(base);
          array.add(o);
       }

      base->Next();
    }

   SetEnabled();
}

extern int PageOffX;
extern int PageOffY;

void DrawFrame(HDC DC, int m, int y)
{
   HGDIOBJ pen = SelectObject(DC, CreatePen(PS_SOLID, 3, RGB(0, 0, 128)));

   int offX[7] = { 80, 260, 310, 490, 250, 270, 400 };
   int startX = 300;

   Rectangle(DC, 300 + PageOffX, -(550 + PageOffY), PageOffX + 300 + 2350, -(PageOffY + 550 + 980));
   for(int i = 0; i < 7; i++)
    {
       MoveTo(DC, PageOffX + startX + offX[i], -(PageOffY + 550));
       LineTo(DC, PageOffX + startX + offX[i], -(PageOffY + 550 + 980));
       startX += offX[i];
    }

   startX = 550 + 160;

   for(i = 0; i < 10; i++)
    {
       MoveTo(DC, PageOffX + 300, -(PageOffY + startX));
       LineTo(DC, PageOffX + 300 + 2350, -(PageOffY + startX));
       startX += 80;
    }

   DeleteObject(SelectObject(DC, pen));

   char* a = "ОТЧЕТ ЗАСО \"ТАСК\"";
   RECT r = { 1100, -220, 1700, -320 };
   OffsetRect(&r, PageOffX, -PageOffY);
   DrawText(DC, a, strlen(a), &r, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

   SetRect(&r, 800, -320, 2080, -440);
   OffsetRect(&r, PageOffX, -PageOffY);
   a = "по страхованию гражданской ответственности владельцев\nсредств автотранспорта \"Зеленая Карта\"";
   DrawText(DC, a, strlen(a), &r, DT_CENTER);

   SetRect(&r, 300, -(580), 380, -(550 + 160));
   OffsetRect(&r, PageOffX, -PageOffY);
   a = "N\nп/п";
   DrawText(DC, a, strlen(a), &r, DT_CENTER);

   SetRect(&r, 380, -(560), 640, -(550 + 160));
   OffsetRect(&r, PageOffX, -PageOffY);
   a = "Дата\nвыдачи\nполиса";
   DrawText(DC, a, strlen(a), &r, DT_CENTER);

   SetRect(&r, 640, -(560), 950, -(550 + 160));
   OffsetRect(&r, PageOffX, -PageOffY);
   a = "Номер\nстрахового\nполиса";
   DrawText(DC, a, strlen(a), &r, DT_CENTER);

   SetRect(&r, 950, -(550), 1440, -(550 + 160));
   OffsetRect(&r, PageOffX, -PageOffY);
   a = "Страхователь";
   DrawText(DC, a, strlen(a), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

   SetRect(&r, 1440, -(560), 1690, -(550 + 160));
   OffsetRect(&r, PageOffX, -PageOffY);
   a = "Тип транс-\nпортного\nсредства";
   DrawText(DC, a, strlen(a), &r, DT_CENTER);

   SetRect(&r, 1690, -(580), 1960, -(550 + 160));
   OffsetRect(&r, PageOffX, -PageOffY);
   a = "Регистрац.\nномер а/м";
   DrawText(DC, a, strlen(a), &r, DT_CENTER);

   SetRect(&r, 1960, -(580), 2360, -(550 + 160));
   OffsetRect(&r, PageOffX, -PageOffY);
   a = "Период\nстрахования";
   DrawText(DC, a, strlen(a), &r, DT_CENTER | DT_VCENTER);

   SetRect(&r, 2360, -(560), 2660, -(550 + 160));
   OffsetRect(&r, PageOffX, -PageOffY);            
   a = "Страховая\nпремия\n( в DM )";
   DrawText(DC, a, strlen(a), &r, DT_CENTER | DT_VCENTER);

   SetRect(&r, 1000, -(430), 1900, -(530));
   OffsetRect(&r, PageOffX, -PageOffY);
   char buff[40];
   char b[20];
   LoadString(GetApplicationObject()->hInstance, 99 + m, b, sizeof b);
   sprintf(a = buff, "за %s месяц %d г.", b, y);
   DrawText(DC, a, strlen(a), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

void DrawFrameEnd(HDC DC, int n, double sum)
{
   HGDIOBJ pen = SelectObject(DC, CreatePen(PS_SOLID, 3, RGB(0, 0, 128)));

   MoveTo(DC, PageOffX + 1000, - PageOffY - 1650);
   LineTo(DC, PageOffX + 1450, - PageOffY -1650);

   MoveTo(DC, PageOffX + 1700, - PageOffY -1650);
   LineTo(DC, PageOffX + 2350, - PageOffY -1650);

   MoveTo(DC, PageOffX + 630, - PageOffY -1740);
   LineTo(DC, PageOffX + 1350, - PageOffY -1740);

   MoveTo(DC, PageOffX + 1450, - PageOffY -1740);
   LineTo(DC, PageOffX + 1900, - PageOffY -1740);

   MoveTo(DC, PageOffX + 2100, - PageOffY -1740);
   LineTo(DC, PageOffX + 2500, - PageOffY -1740);

   DeleteObject(SelectObject(DC, pen));

   RECT r;

   SetRect(&r, 370, -1600, 520, -1652);
   OffsetRect(&r, PageOffX, -PageOffY);
   char* a = "ИТОГО:";
   DrawText(DC, a, strlen(a), &r, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

   SetRect(&r, 600, -1600, 1100, -1652);
   OffsetRect(&r, PageOffX, -PageOffY);
   a = "Количество полисов";
   DrawText(DC, a, strlen(a), &r, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

   SetRect(&r, 1570, -1600, 1700, -1652);
   OffsetRect(&r, PageOffX, -PageOffY);
   a = "Сумма";
   DrawText(DC, a, strlen(a), &r, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

   SetRect(&r, 370, -1660, 620, -1749);
   OffsetRect(&r, PageOffX, -PageOffY);
   a = "Отчет сдал";
   DrawText(DC, a, strlen(a), &r, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

   SetRect(&r, 1990, -1660, 2100, -1745);
   OffsetRect(&r, PageOffX, -PageOffY);
   a = "Дата:";
   DrawText(DC, a, strlen(a), &r, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

   SetRect(&r, 800, -1700, 1300, -1788);
   OffsetRect(&r, PageOffX, -PageOffY);
   a = "( Ф.И.О. исполнителя )";
   DrawText(DC, a, strlen(a), &r, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

   SetRect(&r, 1570, -1700, 1800, -1788);
   OffsetRect(&r, PageOffX, -PageOffY);
   a = "( подпись )";
   DrawText(DC, a, strlen(a), &r, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

   char buff[40];
   sprintf(a = buff, "%u", n);

   SetRect(&r, 1100, -1600, 1450, -1652);
   OffsetRect(&r, PageOffX, -PageOffY);
   DrawText(DC, a, strlen(a), &r, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

   sprintf(a = buff, "%2.2f", sum);

   SetRect(&r, 1800, -1600, 2350, -1652);
   OffsetRect(&r, PageOffX, -PageOffY);
   DrawText(DC, a, strlen(a), &r, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
}


void rptDB1::Draw(HDC DC)
{
   HGDIOBJ font = SelectObject(DC, CreateFontIndirect(&fontrpt));

   DrawFrame(DC, month, year);

   for(int i = CurPage * 10; (i < (CurPage + 1) * 10) && (i < array.getItemsInContainer()); i++)
    {
       ((ODB1&)array[i]).Draw(DC, i + 1);
    }

   if(CurPage == (array.getItemsInContainer() / 11))
    {
      double s = 0;
      for(int i = 0; i < array.getItemsInContainer(); i++)
        {
          s += ((ODB1&)array[i]).sum;
        }

      DrawFrameEnd(DC, array.getItemsInContainer(), s);
    }

   SetEnabled();
   DeleteObject(SelectObject(DC, font));
}

void rptDB1::Paint(HDC DC, PAINTSTRUCT _FAR & PaintInfo)
{
   SaveDC(DC);
   SetMapMode(DC, MM_LOMETRIC);

   Rectangle(DC, 0, 0, 2970, -2100);

   SetBkMode(DC, TRANSPARENT);

   int aPageOffX = PageOffX;
   int aPageOffY = PageOffY;
   PageOffX = 0;
   PageOffY = 0;
   Draw(DC);
   PageOffX = aPageOffX;
   PageOffY = aPageOffY;
   RestoreDC(DC, -1);
}

void rptDB1::WMMinMax(RTMessage Msg)
{
   ((POINT far *)Msg.LParam)[3].x = 400;
   ((POINT far *)Msg.LParam)[3].y = 300;
}

void rptDB1::SetEnabled()
{
   int flag = MF_ENABLED;
   if(array.getItemsInContainer() / 10 == CurPage) flag = MF_GRAYED;
   EnableMenuItem(GetSubMenu(GetMenu(HWindow), 0), 200, MF_BYCOMMAND | flag);

   flag = MF_ENABLED;
   if(0 == CurPage) flag = MF_GRAYED;
   EnableMenuItem(GetSubMenu(GetMenu(HWindow), 0), 201, MF_BYCOMMAND | flag);
}

void printReport(HDC DC)
{
   report->CurPage = report->array.getItemsInContainer() / 10 - NextPage;
   SaveDC(DC);
   SetMapMode(DC, MM_LOMETRIC);
   SetBkMode(DC, TRANSPARENT);
   report->Draw(DC);
   RestoreDC(DC, -1);
   InvalidateRect(report->HWindow, 0, 0);
   report->SetEnabled();
}

extern void ShowRuler(HDC);

void rptDB1::Print()
{
   TMessage m;
   printFunc = printReport;
   CurPage = 0;
   NextPage = report->array.getItemsInContainer() / 10;
   ((TRulerWin*)(Parent))->CMFilePrint(m);
   printFunc = ShowRuler;
   SetFocus(HWindow);
}

void rptDB1::Next()
{
   if(array.getItemsInContainer() / 10 != CurPage)
    {
      CurPage++;
      InvalidateRect(HWindow, 0, 0);
    }
}

void rptDB1::Prev()
{
   if(CurPage)
    {
      CurPage--;
      InvalidateRect(HWindow, 0, 0);
    }
}

void rptDB1::PrintSetup()
{
     TMessage m;
     ((TRulerWin*)Parent)->CMFilePrinterSetup(m);
     SetFocus(HWindow);
}

void rptDB1::Page()
{
     TMessage m;
     ((TRulerWin*)Parent)->PageSetup(m);
     SetFocus(HWindow);
}

extern void cmFonts(LOGFONT&);

void rptDB1::Fonts()
{
   cmFonts(fontrpt);
   InvalidateRect(HWindow, 0, 0);
   SetFocus(HWindow);
}

void rptDB1::Zoom()
{
   ShowWindow(HWindow, IsZoomed(HWindow) ? SW_SHOWNORMAL : SW_SHOWMAXIMIZED);
}

void rptDB1::Left()
{
   Scroller->ScrollBy(-1, 0);
}

void rptDB1::Right()
{
   Scroller->ScrollBy(1, 0);
}

void rptDB1::Up()
{
   Scroller->ScrollBy(0, -1);
}

void rptDB1::Down()
{
   Scroller->ScrollBy(0, 1);
}

*/