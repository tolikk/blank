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

#define max(a, b) ((a) > (b) ? (a) : (b))

#undef NDEBUG
#include "assert.h"

#define ASS()
extern char ActivateParadox;
extern DATE fstSept;

char* GetCheckErrorText(int Err);
int IsCompany(const char* s);
int TestTextValue(char* buffer, char* key, char* value, char* errCaption, HWND Wnd);

const char* GetRCStr(int);
void DateFBToStr(TOpenBase* base, int field, char* buffer);
extern void DblToStr(char* str, double V, int Digit = 2);
extern DATE GD(char* str, int = 0);
char* TrimStr(char* str);

AddIntoListBox(TListBox* CB, Avaria2Data data, int Index = -1);
//AddIntoListBox2(TListBox* CB, OwnersData data, int Index = -1);
void InitTime(TEdit* e);
extern const char* MANDATORY_SECT;
char* NumberToWords(double Value);
extern char CurrentRateEUR[10];
void SetLittleFont(HWND, short*, int);
void SetAgent(char* ACode, TOpenBase& agents, PTComboBox AgName, int);
void divDate(char* b);
void AddBufDate(char*&dest, char* date, int flag);

int TestAux(char* buff, TWindow* e, char* msg, int CheckValue = 0)
{
	ASS();
	if(!*buff || (CheckValue && atof(buff) < 0.01)) {
       DynStr message;
       wsprintf(message, GetRCStr(505), msg);
       SetFocus(e->HWindow);
       MessageBox(GetParent(e->HWindow), message, "!", MB_OK | MB_ICONSTOP);
	   return 0;
    }
    return 1;
}

char CodeK3(double K3);
int IsValidMandRecord(TOpenBase& base, int, int, const char*, int);

int CheckExists(const char* KeyMask, char* findValue)
{
	ASS();
	for(int i = 0;; i++) {
	char Key[16];
	char NABuffer[32];
	wsprintf(Key, KeyMask, i);
	assert(strlen(Key) < sizeof Key);
	if(!GetPrivateProfileString(MANDATORY_SECT, Key, "", NABuffer, sizeof NABuffer, ::ININame))
	    break;
	if(!strcmp(NABuffer, findValue))
	    return 1;
    }
    return 0;
}


int MandatoryDlg::SetDataToBase()
{

/*if(Komplex)
{
	struct date d;
    getdate(&d);
	if(d.da_year > 2010) return 0;
 }
*/

   ASS();
   Dlg_base::SetDataToBase();
   if(!ActivateParadox) return 0;

   if(!CheckAvDups()) {
		  MessageBox(HWindow, GetRCStr(633), "!", MB_OK | MB_ICONSTOP);
		  return 0;
   }

   int OwnPolis = m_OwnPolis->GetCheck() == 0;

   ((TComboBox*)e[0])->GetText(buffer.Seria, sizeof buffer.Seria);
   ((TComboBox*)e[3])->GetText(buffer.PrevSeria, sizeof buffer.PrevSeria);

   (*base)(0, buffer.Seria);
   if(!buffer.Seria[0])
    {
       MessageBeep(0);
       SetFocus(e[0]->HWindow);
       return 0;
    }

   (*base)(1, atof(buffer.Number));
   if(!buffer.Number[0] || atof(buffer.Number) < 1)
    {
       MessageBeep(0);
       SetFocus(e[1]->HWindow);
       return 0;
    }

   DATE date = OwnPolis ? GD(buffer.RegDate) : BLANKDATE;
   DATE regdate = date;
   if(!date) {
      MessageBeep(0);
      SetFocus(e[2]->HWindow);
      return 0;
   }

   (*base)(2, date);

   int Index = AgName1->GetSelIndex();
   if(Index == -1 && OwnPolis) {
      MessageBox(HWindow, "Введи агента", "Ошибка", MB_OK | MB_ICONSTOP);
      SetFocus(AgName1->HWindow);
      return 0;
   }

   BOOL Bad = OwnPolis && (strlen(buffer.Name) == 0) || (!strcmp("ИСПОРЧЕН", buffer.Name));
   if(Bad) {
      if(/*ownersCount > 0 ||*/ avariaCount > 0) {
	  MessageBox(HWindow, GetRCStr(559), "!", MB_OK | MB_ICONSTOP);
		  return 0;
      }
      if(MessageBox(HWindow, GetRCStr(506), "?", MB_YESNO | MB_ICONINFORMATION | MB_SYSTEMMODAL) == IDYES)
      {
	 strcpy(buffer.Name, "ИСПОРЧЕН");

	 buffer.RegDate[0] = 0;
	 buffer.Address[0] = 0;
	 buffer.FromDate[0] = 0;
	 buffer.FromTime[0] = 0;
	 buffer.ToDate[0] = 0;
	 buffer.Letter[0] = 0;
	 buffer.Marka[0] = 0;
	 buffer.AutoNumber[0] = 0;
	 buffer.Volume[0] = 0;
	 buffer.Tonnage[0] = 0;
	 buffer.Quantity[0] = 0;
	 buffer.Power[0] = 0;
	 buffer.NumberBody[0] = 0;
	 buffer.Chassis[0] = 0;
	 buffer.Tarif[0] = 0;
	 buffer.Rate[0] = 0;
	 buffer.Discount[0] = 0;
	 buffer.AllFee[0] = 0;
	 buffer.AllFeeText[0] = 0;
	 buffer.Fee1[0] = 0;
	 buffer.Fee1Text[0] = 0;
	 buffer.Fee1Date[0] = 0;
	 buffer.DraftNumber[0] = 0;
	 buffer.Fee2[0] = 0;
	 buffer.Fee2Text[0] = 0;
	 buffer.Fee2Date[0] = 0;
	 buffer.Nal = 0;
	 buffer.BezNal = 0;
	 buffer.Karta = 0;
	 buffer.IsPay2 = 0;
	 Super->SetSelIndex(-1);
	 Period->SetSelIndex(-1);
	 Pay1Curr->SetSelIndex(-1);
	 Pay2Curr->SetSelIndex(-1);
	 m_Vzamen->SetSelIndex(-1);
     buffer.PrevSeria[0] = 0;
	 buffer.PrevNumber[0] = 0;
	 buffer.CityCode[0] = 0;

	 //State = STATE_POLIS_BAD;
		 SetState(STATE_POLIS_BAD);
      }
      else return 0;
   }
   else {

   if(m_State == STATE_POLIS_BAD)
		 SetState(STATE_POLIS_NORMAL);

	//Check no Auto
	int NoAuto = OwnPolis ? CheckExists((String&)m_CheckAuto[m_Resident->GetSelIndex()], buffer.Letter) : 1;
	int NoShassi = OwnPolis ? CheckExists((String&)m_CheckShassi[m_Resident->GetSelIndex()], buffer.Letter) : 1;
	int NoNumberBody = OwnPolis ? CheckExists((String&)m_CheckNumberBody[m_Resident->GetSelIndex()], buffer.Letter) : 1;

	//Test Fields
	if(buffer.FromTime[0]) {
	    if(strchr(buffer.FromTime, ':') == 0) {
		    MessageBox(HWindow, "Набери время через двоеточие", "Ошибка", MB_OK | MB_ICONSTOP);
		    SetFocus(e[6+2]->HWindow);
            return 0;
        }
	}

	if(OwnPolis && !TestAux(buffer.FromDate, e[5+2], "Начало страхования")) return 0;
	if(OwnPolis && buffer.Nal && !TestAux(buffer.FromTime, e[6+2], "Время")) return 0;
	if(OwnPolis && !TestAux(buffer.ToDate, e[7+2], "Конец страхования")) return 0;
	if(OwnPolis && !TestAux(buffer.Letter, e[8+2], "Буква")) return 0;
	if(!NoAuto) {
	   if(!TestAux(buffer.Marka, e[9+2], "Марка автомобиля")) return 0;
	   if(!TestAux(buffer.AutoNumber, e[10+2], "Номер автомобиля")) return 0;
	}
	if(!NoShassi)
	   if(!TestAux(buffer.Chassis, e[16+2], "Шасси")) return 0;
	if(!NoNumberBody)
	   if(!TestAux(buffer.NumberBody, e[15+2], "Номер кузова")) return 0;
	if(OwnPolis && !TestAux(buffer.Tarif, e[17+2], "Тариф", 1)) return 0;
	if(OwnPolis && !TestAux(buffer.Rate, e[18+2], "Курс", 1)) return 0;
	if(OwnPolis && !TestAux(buffer.AllFee, e[20+2], "Страховой взнос", 1)) return 0;
	if(OwnPolis && !TestAux(buffer.Fee1, e[22+2], "Как уплачен взнос", 1)) return 0;
	if(OwnPolis && !buffer.Nal && !buffer.BezNal && !buffer.Karta) {
	   MessageBox(HWindow, "Не указан вид платежа.", "Ошибка", MB_OK | MB_ICONSTOP);
	   SetFocus(Nal->HWindow);
           return 0;
	}
	if(OwnPolis && !TestAux(buffer.Fee1Date, e[24+2], "Дата платежа")) return 0;
	if(OwnPolis && buffer.BezNal) {
	    if(!TestAux(buffer.DraftNumber, e[25+2], "Номер платёжки", 1)) return 0;
	}

	if(!OwnPolis || buffer.IsPay2) {
	    if(!TestAux(buffer.Pay2RealDate, e[30+2], "Дата уплаты 2го взноса")) return 0;
	    if(buffer.BezNal_2)
	       if(!TestAux(buffer.Pay2DraftNumber, e[33+2], "Номер платёжки", 1)) return 0;
	    if(Pay2Curr->GetSelIndex() == -1) {
	       SetFocus(Pay2Curr->HWindow);
	       return 0;
	    }
	}
	else
	    Pay2Curr->SetSelIndex(-1);

	if(OwnPolis && Pay1Curr->GetSelIndex() == -1) {
	   SetFocus(Pay1Curr->HWindow);
       return 0;
	}

	strcpy(CurrentRateEUR, buffer.Rate);
	if(OwnPolis && Pay1Curr->GetSelIndex() == 0)
	   WritePrivateProfileString(MANDATORY_SECT, "LastRate", CurrentRateEUR, ::ININame);

	//Test Params
	char buff[64];
	Super->GetText(buff, sizeof buff);
	if(OwnPolis && !TestTextValue(buff, "куб.", buffer.Volume, "Объем двигателя", HWindow))
           return 0;
	if(OwnPolis && !TestTextValue(buff, "тонн", buffer.Tonnage, "Грузоподъёмность", HWindow))
           return 0;
	if(OwnPolis && !TestTextValue(buff, "л.с.", buffer.Power, "Мощность", HWindow))
           return 0;
	if(OwnPolis && !TestTextValue(buff, "чел", buffer.Quantity, "Колич. пассажиров", HWindow))
           return 0;
	if(OwnPolis && !TestTextValue(buff, "пасс", buffer.Quantity, "Колич. пассажиров", HWindow))
           return 0;
   }

   (*base)(3, TrimStr(buffer.Name));
   if(!Bad) {
	   if(IsCompany(buffer.Name) && !buffer.Uridich) {
	   if(MessageBox(HWindow, GetRCStr(608), buffer.Name, MB_YESNO | MB_ICONSTOP) == IDNO)
   	       return 0;
       }
	   if(!IsCompany(buffer.Name) && !(buffer.Fizich_||buffer.IP)) {
	   if(MessageBox(HWindow, GetRCStr(609), buffer.Name, MB_YESNO | MB_ICONSTOP) == IDNO)
   	       return 0;
       }
   }

   (*base)(4, TrimStr(buffer.Address));

   date = OwnPolis ? GD(buffer.FromDate, 1) : BLANKDATE;
   DATE FROM_DATE = date;
   if(date)
      (*base)(5, date);
   (*base)(6, buffer.FromTime);
   date = OwnPolis ? GD(buffer.ToDate, 1) : BLANKDATE;
   if(date)
      (*base)(7, date);
   DATE TO_DATE = date;

   if(OwnPolis && !Bad && (TO_DATE - FROM_DATE) < 360)
	   if(buffer.IsFee2) {
           Period->ShowList();
	   return 0;
       }

   //Проверка владельцев
/*   for(int i = 0; i < ownersCount; i++) {
       char b[32];
       memcpy(b, autoowners[i].RegDate, sizeof(b));
	   DATE ownRegDate = GD(b);
	   if((!*buffer.PrevSeria && ownRegDate < regdate) || ownRegDate >= TO_DATE)
	   {
	   		MessageBox(HWindow, GetRCStr(610), autoowners[i].Name, MB_OK | MB_ICONSTOP);
	   		return 0;
       }
   }
  */
   //Проверка аварий
   for(int i = 0; i < avariaCount; i++) {
       char b[32];
       memcpy(b, avarias2[i].AVDate, sizeof(b));
	   DATE avDate = GD(b);
	   if(avDate < FROM_DATE || avDate > TO_DATE)
	   {
	   		MessageBox(HWindow, GetRCStr(611), "Авария", MB_OK | MB_ICONSTOP);
			return 0;
       }
   }

   (*base)(8, buffer.Letter);
   (*base)(9, TrimStr(buffer.Marka));
   (*base)(10, TrimStr(buffer.AutoNumber));

   if(!Bad)
   {
       (*base)(11, atof(buffer.Volume));
       (*base)(12, atof(buffer.Tonnage));
       (*base)(13, atof(buffer.Quantity));
       (*base)(14, atof(buffer.Power));
   }

   (*base)(15, TrimStr(buffer.NumberBody));
   (*base)(16, TrimStr(buffer.Chassis));
   if(!Bad)
   {
       (*base)(17, atof(buffer.Tarif));
       (*base)(18, atof(buffer.Rate));
       (*base)(19, atof(buffer.Discount));
	   (*base)(20, atof(buffer.AllFee));
	   if(buffer.IsPay2 && buffer.Nal_2 && m_1SU->GetSelIndex() < 1)
	   {
			if(MessageBox(HWindow, GetRCStr(712), "?", MB_YESNO | MB_ICONQUESTION) == IDNO)
			{
				SetFocus(m_1SU->HWindow);
				return 0;
            }
       }
	   if(buffer.IsPay2 && buffer.BezNal_2 && m_1SU->GetSelIndex() > 0)
	   {
			if(MessageBox(HWindow, GetRCStr(713), "?", MB_YESNO | MB_ICONQUESTION) == IDNO)
            {
				SetFocus(m_1SU->HWindow);
				return 0;
            }
	   }
	   char buff[2] = "N";
	   buff[0] = SendMessage(m_1SU->HWindow, CB_GETITEMDATA, m_1SU->GetSelIndex(), 0);
	   (*base)(21, buff);
   }

   if(!Bad) {
	   (*base)(22, atof(buffer.Fee1));
   }
   // to aux table (*base)(23, buffer.Fee1Text);

   (*base)(24, (double)(buffer.Nal == 1 ? 1 : buffer.Karta ? 2 : 0));
   date = GD(buffer.Fee1Date, 1);
   DATE PayDate = date;
   if(PayDate) {
	  if(PayDate > FROM_DATE) {
		  MessageBox(HWindow, GetRCStr(629), "!", MB_OK | MB_ICONSTOP);
		  SetFocus(ChildWithId(121)->HWindow);
		  return 0;
	  }
	  (*base)(25, date);

	  if(!Bad) {
		  DATE Ret1Date = date;
		  if(*buffer.RetDate1)
			  Ret1Date = GD(buffer.RetDate1, 1);
		  if(!Ret1Date)
		  {
				SetFocus(m_RetDate1->HWindow);
				return 0;
		  }
		  (*base)(67, Ret1Date);
      }
   }

   if(!Bad) {
       (*base)(26, atof(buffer.DraftNumber));
       (*base)(27, atof(buffer.Fee2));

	   if(OwnPolis)
       {
		   if(m_Uridich->GetCheck() == 1 && !buffer.BezNal || m_Uridich->GetCheck() != 1 && buffer.BezNal)
		   {
			  if(MessageBox(HWindow, "Тип оплаты и лицо совпадают?", "!", MB_YESNO | MB_ICONQUESTION) == IDNO)
			  {
				return 0;
			  }
		   }

		   if(atoi(buffer.PrevNumber) == 0)
		   {
			  int delta = buffer.BezNal ? 365 : 31;
			  if(OwnPolis && ((PayDate+delta) < FROM_DATE) || (regdate < PayDate))
			  {
				 MessageBox(HWindow, ":(", "Ошибка", MB_OK | MB_ICONSTOP);
				 SetFocus(ChildWithId(121)->HWindow);
				 return 0;
			  }
		   }
	   }
   }
   date = GD(buffer.Fee2Date, 1);
   if(date)
      (*base)(29, date);

   Index = AgName1->GetSelIndex();
   char AgCode_[10];

   long acode = SendMessage(AgName1->HWindow, CB_GETITEMDATA, Index, 0);
   memmove(AgCode_, &acode, 4);
   AgCode_[4] = 0;
   if(!OwnPolis) AgCode_[0] = 0;
   (*base)(30, AgCode_);

   if(OwnPolis && Period->GetSelIndex() == -1 && buffer.Nal != buffer.BezNal) {
	MessageBox(HWindow, "Выбери период страхования.", "!", MB_OK | MB_ICONSTOP);
	SetFocus(Period->HWindow);
	return 0;
   }


   DynStr _buffer2;
   Period->GetText(_buffer2, _buffer2._sizeof());
   (*base)(31, _buffer2);

   Index = Super->GetSelIndex();
   if(OwnPolis && Index == -1 && buffer.Nal != buffer.BezNal) {
	MessageBox(HWindow, "Выбери вид страхования.", "!", MB_OK | MB_ICONSTOP);
	SetFocus(Super->HWindow);
	return 0;
   }
   if(Index != -1)
	(*base)(32, (double)(unsigned long)SendMessage(Super->HWindow, CB_GETITEMDATA, Index, 0));
   else
	(*base)(32, (double)(-1));

   m_UrFizich1->GetText(_buffer2, _buffer2._sizeof());
   if(*_buffer2 == 'Ю')
       (*base)(33, "U");
   else
   if(*_buffer2 == 'Ф')
       (*base)(33, "F");
   else {
	   if(OwnPolis) {
           SetFocus(m_UrFizich1->HWindow);
	       return 0;
       }
   }

   if(!Bad)
        (*base)(34, (double)buffer.IsFee2);

   ///////////////////

//"IsPay2",         //35
//"Pay2Date",       //36
//"Pay2Rate",       //37
//"Pay2Summa",      //38
//"Pay2Type",       //39
//"Pay2DraftNumber" //40

   if(!Bad)
       (*base)(35, (double)buffer.IsPay2);
       if(buffer.IsPay2) {

		  int Index = AgName2->GetSelIndex();
		  if(Index == -1) {
			MessageBox(HWindow, "Введи агента", "!", MB_OK | MB_ICONSTOP);
			SetFocus(AgName2->HWindow);
			return 0;
		  }
		   long acode = SendMessage(AgName2->HWindow, CB_GETITEMDATA, Index, 0);
		   memmove(AgCode_, &acode, 4);
		   AgCode_[4] = 0;
		   (*base)(28, AgCode_);

		  if(buffer.AgPercent2[0] == 0 || atof(buffer.AgPercent2) > 50 || atof(buffer.AgPercent2) < 0.0) {
			 MessageBox(HWindow, GetRCStr(603), "Ошибка", MB_ICONSTOP);
			 return 0;
		  }

	      (*base)(63, atof(buffer.AgPercent2));

		  m_UrFizich2->GetText(_buffer2, _buffer2._sizeof());
		  if(*_buffer2 == 'Ю')
			  (*base)(69, "U");
		  else
		  if(*_buffer2 == 'Ф')
			  (*base)(69, "F");
		  else {
			  SetFocus(m_UrFizich2->HWindow);
			  return 0;
		  }

		  date = GD(buffer.Pay2RealDate, 1);
		  if(date) {
			 (*base)(36, date);

			 DATE Ret2Date = date;
			 if(*buffer.RetDate2) 
				 Ret2Date = GD(buffer.RetDate2, 1);
			 if(!Ret2Date) {
			   SetFocus(m_RetDate2->HWindow);
			   return 0;
		     }
			(*base)(68, Ret2Date);
		  }
          else
	         return 0;

       if(atof(buffer.Pay2RubSumma) < 0.01) {
	       MessageBox(HWindow, "Где деньги?", "!", MB_OK | MB_ICONSTOP);
	       SetFocus(e[32+2]->HWindow);
	       return 0;
       }
       (*base)(37, atof(buffer.Pay2Rate));
       (*base)(38, atof(buffer.Pay2RubSumma));
       (*base)(39, (double)buffer.Nal_2);
       (*base)(40, atof(buffer.Pay2DraftNumber));
   }

   //(*base)(41, float(buffer.IsResident));
   (*base)(41, double(SendMessage(m_Resident->HWindow, CB_GETITEMDATA, m_Resident->GetSelIndex(), 0)));

   Pay1Curr->GetText(_buffer2, _buffer2._sizeof());
   (*base)(42, _buffer2);
   Pay2Curr->GetText(_buffer2, _buffer2._sizeof());
   (*base)(43, _buffer2);

   if(!Bad)
   {
		if((m_Vzamen->GetSelIndex() == 0) && (!buffer.PrevSeria[0] || atof(buffer.PrevNumber) < 1))
		{
			MessageBox(HWindow, GetRCStr(560), "!", MB_OK | MB_ICONSTOP);
			return 0;
		}

	 	if(atof(buffer.PrevNumber) == atof(buffer.Number) && !strcmpi(buffer.Seria, buffer.PrevSeria))
	 	{
			MessageBox(HWindow, GetRCStr(561), "!", MB_OK | MB_ICONSTOP);
			return 0;
	 	}

		if(m_Vzamen->GetSelIndex() == 0) 
		{
  	     sprintf(_buffer2, "Вы уверены, что полис %s/%s утерян?", buffer.PrevSeria, buffer.PrevNumber);
   	     //assert(strlen(buffer2) < sizeof buffer2);
	     if(MessageBox(HWindow, _buffer2, "Отвечай", MB_YESNO | MB_ICONQUESTION) == IDNO)
	         return 0;
		 char* ShortFields[] = { "Seria", "Number", "State", "UpdateDate", "StopDate", "RegDate", "ToDate" };
		 TOpenBase I("\\bases\\mandator", ShortFields, 7);
		 if(I.pxErr) {
			Err:MessageBox(HWindow, GetRCStr(552), "!", MB_OK | MB_ICONSTOP);
			return 0;
		 }
		 I(0, buffer.PrevSeria);
		 I(1, atof(buffer.PrevNumber));
		 if(PXSrchKey(I.getTblHandle(), I.getRecHandle(), 2, SEARCHFIRST) == PXSUCCESS)
		 {
			 I.Get();
			 DATE lost_date;
			 I(lost_date, 5);
			 if(lost_date > regdate)
			 {
				MessageBox(HWindow, GetRCStr(635), "!", MB_OK | MB_ICONSTOP);
				return 0;
			 }
			 I(lost_date, 6);
			 if(lost_date != TO_DATE)
			 {
				MessageBox(HWindow, GetRCStr(636), "!", MB_OK | MB_ICONSTOP);
				return 0;
			 }
			 struct date d;
			 getdate(&d);
			 PXDateEncode(d.da_mon, d.da_day, d.da_year, &lost_date);
			 I(2, (double)STATE_POLIS_LOST);
			 I(3, lost_date);
			 I(4, regdate);
			 if(PXRecUpdate(I.getTblHandle(), I.getRecHandle()) != PXSUCCESS)
			goto Err;
		 }
		 else {
			MessageBox(HWindow, GetRCStr(551), "!", MB_OK | MB_ICONSTOP);
			return 0;
		 }
	 }

	  if(OwnPolis && m_Vzamen->GetSelIndex() != 0 && regdate != PayDate && (m_Fizich->GetCheck())) {
		  MessageBox(HWindow, GetRCStr(628), "!", MB_OK | MB_ICONSTOP);
		  SetFocus(ChildWithId(121)->HWindow);
		  return 0;
	  }

	 if(fabs(atof(buffer.PrevNumber)) > 0.01)
     {
	 	(*base)(44, buffer.PrevSeria);
		(*base)(45, m_Vzamen->GetSelIndex() == 1 ? -atof(buffer.PrevNumber) : atof(buffer.PrevNumber));
	 }

     int IsFizIP = m_Fizich->GetCheck() ? 1 : m_IP->GetCheck() ? 2 : 0;
	 if(regdate >= fstSept/* && IsFizIP*/)
	 {
		char K3[5];
		m_K3->GetText(K3, sizeof K3);
		 if(atof(K3)< 0.01 || atof(K3)>4.55)
		 {
			SetFocus(m_K3->HWindow);
			return 0;
		 }

		char PlaceStr[5];
		PlaceStr[0] = CodeK3(atof(K3));
		PlaceStr[1] = 0;
		(*base)(47, PlaceStr);
	 }

	//OwnPolis

	 //if(Version == 2)
	 {
		(*base)(46, atof(buffer.TarifGlass));
		_m_OldClassAv->GetText(_buffer2, _buffer2._sizeof());
		(*base)(52, atof(buffer.K1));
        if(OwnPolis)
		if(_m_OldClassAv->GetSelIndex()<0)
		{
			SetFocus(_m_OldClassAv->HWindow);
			return 0;
        }
		(*base)(53, SendMessage(_m_OldClassAv->HWindow, CB_GETITEMDATA, _m_OldClassAv->GetSelIndex(), 0) / 100.);
	 }

	if(OwnPolis && m_Placement->GetSelIndex() == -1)
	{
        SetFocus(m_Placement->HWindow);
        return 0;
    }
     (*base)(48, (SendMessage(m_Placement->HWindow, CB_GETITEMDATA, m_Placement->GetSelIndex(), 0) / 100.));
	 if(OwnPolis && m_Osnovanie->GetSelIndex() == -1)
	 {
	   MessageBeep(0);
	   SetFocus(m_Osnovanie->HWindow);
           return 0;
     }
     (*base)(55, (double)(SendMessage(m_Osnovanie->HWindow, CB_GETITEMDATA, m_Osnovanie->GetSelIndex(), 0) / 100));
	 (*base)(56, (double)buffer.Fizich_?1.0:buffer.IP?2.0:0.0);

     //PART2 END

   } //Flag Bad

   if(!Bad || !OwnPolis)
   {
		if(!*buffer.RetDate)
        {	
			strcpy(buffer.RetDate, buffer.StopDate);
		}
		if(!*buffer.StopDate && *buffer.RetBRB)
        {	
			strcpy(buffer.StopDate, buffer.RetDate);
		}
		if(*buffer.StopDate)
		{
	   		DATE date = GD(buffer.StopDate);
			if(!date)
			{
              MessageBeep(0);
              SetFocus(m_StopDate->HWindow);
              return 0;
	   		}

		   if(OwnPolis && (date < FROM_DATE || date >= TO_DATE))
		   {
			  MessageBox(HWindow, GetRCStr(562), "!", MB_ICONSTOP);
			  return 0;
		   }

		   (*base)(54, date);
		   if(m_State != STATE_POLIS_LOST)
		   {
				 if( (atof(buffer.RetBRB) > 0.01 && atof(buffer.RetEUR) < 0.01) || (atof(buffer.RetBRB) < 0.01 && atof(buffer.RetEUR) > 0.01))
				 {
					MessageBox(HWindow, "Суммы возврата д.б. в 2х валютах", "!", MB_ICONSTOP);
					return 0;
				 }

				 if(OwnPolis && MessageBox(HWindow, "Закрыть полис досрочно?", "Отвечай", MB_YESNO | MB_ICONQUESTION) == IDNO)
				 {
					return 0;
				 }

				 if(atof(buffer.RetBRB) < 0.01)
				 {
					 if(MessageBox(HWindow, "Вы хотите указать выплату?", "Отвечай", MB_YESNO | MB_ICONQUESTION) == IDYES)
					 {
						return 0;
					 }
				 }

				 (*base)(54, date);
				 (*base)(64, atof(buffer.RetBRB));
				 (*base)(65, atof(buffer.RetEUR));
				 if(OwnPolis) SetState(STATE_POLIS_ENDED);
		   }
		   else
		   {
				 SetState(STATE_POLIS_LOST);
		   }
	    }
   }

   (*base)(57, (double)Version);
   (*base)(58, (double)m_State);

   //if(Version > 1) {
       if(OwnPolis && !Bad && !buffer.CityCode[0]) {
           SetFocus(m_CityEdit->HWindow);
           return 0;
       }
      (*base)(59, buffer.CityCode);
   //}

  (*base)(62, buffer.ToReport?101.:100.);
   if(!Bad) {
      // if (buffer.OwnerCode > 0)
	  //(*base)(61, (double)buffer.OwnerCode);
      // if (buffer.BaseCarCode > 0)
	  // if (buffer.CarCode > 0)
	  //(*base)(63, (double)buffer.CarCode);

	  if(*buffer.RetDate && *buffer.StopDate)
	  {
		(*base)(61, (double)GD(buffer.RetDate));

      }

      if(OwnPolis)
       if(buffer.AgPercent1[0] == 0 || atof(buffer.AgPercent1) > 50 || atof(buffer.AgPercent1) < 0.0) {
		   MessageBox(HWindow, GetRCStr(603), "!", MB_ICONSTOP);
           return 0;
       }

       (*base)(66, atof(buffer.AgPercent1));
       int incorrect_number = 0;
       char phoneprefix[10];
       GetPrivateProfileString(MANDATORY_SECT, "MobilePrefix", "375", phoneprefix, sizeof phoneprefix, ININame);
       int prefixLen = strlen(phoneprefix);

       for(int n = 0; buffer.MobilePhone[n]; n++) {
	if(buffer.MobilePhone[n] < '0' || buffer.MobilePhone[n] > '9') {
	   if(!strchr(phoneprefix, buffer.MobilePhone[n]))
		incorrect_number = 1;
        }
       }

       if(incorrect_number || *buffer.MobilePhone && (strlen(buffer.MobilePhone) < (8+prefixLen) || strlen(buffer.MobilePhone) > (9+prefixLen)))
       {
	m_MobilePhone->SetSelection(prefixLen, strlen(buffer.MobilePhone));
	SetFocus(m_MobilePhone->HWindow);
       	return 0;
       }
	   (*base)(70, buffer.MobilePhone);
	   (*base)(71, buffer.HomePhone);
	   (*base)(72, buffer.Contact);
	   (*base)(73, buffer.SMS ? "1" : "");
   }

   struct date d;
   getdate(&d);
   PXDateEncode(d.da_mon, d.da_day, d.da_year, &date);
   if(OwnPolis && date < regdate) {
	   MessageBox(HWindow, GetRCStr(612), "!", MB_ICONSTOP);
	   return 0;
   }
   (*base)(60, date); //update date

   int nSaveData = OwnPolis ? IsValidMandRecord(*base, 1, 1, ININame, Komplex) : 0;
   if(nSaveData) {
	   MessageBox(HWindow, GetCheckErrorText(nSaveData), "Полис", MB_ICONSTOP);
	   return 0;
   }

   if(Komplex)
   {
	   if(m_TaxiInOut->GetSelIndex() > 0)
	   {
		   double summ = atof(buffer.TaxiSumma);
		   if(summ == 0)
		   {
			SetFocus(m_TaxiSumma->HWindow);
            return 0;
           }
		   if(m_TaxiInOut->GetSelIndex() == 2) summ = -summ;
		  (*base)(74, summ);

		  DATE dt = GD(buffer.TaxiPayDate);
	   if(dt < FROM_DATE || dt > TO_DATE)
	   {
			MessageBox(HWindow, "Даипазон", "!", MB_OK | MB_ICONSTOP);
			return 0;
       }
		  (*base)(78, dt);

		   if(dt == 0)
		   {
			SetFocus(m_TaxiPayDate->HWindow);
            return 0;
           }

		  char buff[4];
		  m_TaxiPayCurr->GetText(buff, sizeof buff);
		   if(buff[0] == 0)
		   {
			SetFocus(m_TaxiPayCurr->HWindow);
            return 0;
           }
		  (*base)(75, buff);
		  buff[1] = 0;

		  buff[0] = (buffer.TaxiIsNal == 1 ? 'Y' : 'N');
		  (*base)(76, buff);
		  buff[0] = (buffer.Taxi1SU == 1 ? 'Y' : 'N');
		  (*base)(77, buff);

		  buff[0] = '0' + m_TaxiVid->GetSelIndex();
		  (*base)(80, m_TaxiVid->GetSelIndex() == 0 ? "" : buff);

		  (*base)(79, atof(buffer.TaxiPayDoc));
		   if(buffer.Taxi1SU && atof(buffer.TaxiPayDoc) == 0)
		   {
			SetFocus(m_TaxiDoc->HWindow);
            return 0;
           }
		   if(m_TaxiVid->GetSelIndex() < 0)
		   {
			SetFocus(m_TaxiVid->HWindow);
            return 0;
           }
	   }
   }

   return 1;
}

	 //