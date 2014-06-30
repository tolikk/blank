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
//#include <string.h>
#include <ruler.h>
//#include <window.h>
//#include <combobox.h>
//#include <commdlg.h>
#include <mandator.h>
//#include <checkbox.h>
//#include <thwindow.h>
//#include <stdlib.h>
//#include <help.h>
#include "util.h"
#include "math.h"
//#include "stdlib.h"
//#include "GRNCARD.h"
//#include "io.h"
//#include "sortarry.h"
//#include "dateedit.h"

#define max(a, b) ((a) > (b) ? (a) : (b))

extern const char* MANDATORY_SECT;
int IsLetter(unsigned long Data);
char* GetSummaText(double Value, char* Valuta, char* Kop = NULL);
extern char ModifyMandatory;
extern char ActivateParadox;
extern char UpdateFormData;
void IntFBToStr(TOpenBase* base, int field, char* buffer);
void DateFBToStr(TOpenBase* base, int field, char* buffer);
extern void DblToStr(char* str, double V, int Digit = 2);
extern DATE GD(char* str, int = 0);
void SetAgent(char* ACode, TOpenBase& agents, PTComboBox AgName, int);
void divDate(char* b);
void AddBufDate(char*&dest, char* date, int flag);
double DecodeK3(char* K3);
AddIntoListBox(TListBox* CB, Avaria2Data data, int Index = -1);
//AddIntoListBox2(TListBox* CB, OwnersData data, int Index = -1);

const char* GetRCStr(int);


void MandatoryDlg::GetDataFromBase()
{
	if(!ActivateParadox)
	{
		Clear();
		return;
   	}

	UpdateFormData += 10;

   	memset(&buffer, 0, sizeof buffer);
   	memset(m_AVPayData, 0, sizeof m_AVPayData);

	double N;
   	(*base)(N, 41); //Resident
   	m_Resident->SetSelIndex(-1);

	for(int i = 0; i < m_Resident->GetCount(); i++)
    {
	   if(SendMessage(m_Resident->HWindow, CB_GETITEMDATA, i, 0) == N)
	   {
		   m_Resident->SetSelIndex(i);
           break;
	   }
    }

	(*base)(buffer.Seria, sizeof buffer.Seria, 0);
   	((TComboBox*)e[0])->SetText(buffer.Seria);
   	IntFBToStr(base, 1, buffer.Number);

   	(*base)(buffer.PrevSeria, sizeof buffer.PrevSeria, 44);
	((TComboBox*)e[3])->SetText(buffer.PrevSeria);

   //Check add taxi pay
	if(TaxiDb.pxErr == PXSUCCESS)
        {
	    TaxiDb(0, buffer.Seria);
	    TaxiDb(1, atof(buffer.Number));

	    if(PXSrchKey(TaxiDb.getTblHandle(), TaxiDb.getRecHandle(), 2, SEARCHFIRST) == PXSUCCESS)
		    SetDlgItemText(HWindow, 504, "Доплата ЕСТЬ");
	    else
		    SetDlgItemText(HWindow, 504, "Доплата");
	 }
   // end Check add taxi pay

   ::DateFBToStr(base, 2, buffer.RegDate);
   (*base)(buffer.Name, sizeof buffer.Name, 3);
   (*base)(buffer.Address, sizeof buffer.Address, 4);
   ::DateFBToStr(base, 5, buffer.FromDate);
   (*base)(buffer.FromTime, sizeof buffer.FromTime, 6);
   ::DateFBToStr(base, 7, buffer.ToDate);
   (*base)(buffer.Letter, sizeof buffer.Letter, 8);
   (*base)(buffer.Marka, sizeof buffer.Marka, 9);
   (*base)(buffer.AutoNumber, sizeof buffer.AutoNumber, 10);
   (*base)(buffer.PrevSeria, sizeof buffer.PrevSeria, 44);
   IntFBToStr(base, 45, buffer.PrevNumber);
   m_Vzamen->SetSelIndex(atof(buffer.PrevNumber) > 0 ? 0 : atof(buffer.PrevNumber) < 0 ? 1 : -1);
   VzamenFuncX();
   if(*buffer.PrevNumber == '-') *buffer.PrevNumber = ' ';

   (*base)(N, 11);
   DblToStr(buffer.Volume, N, 2);
   (*base)(N, 12);
   DblToStr(buffer.Tonnage, N, 3);
   IntFBToStr(base, 13, buffer.Quantity);
   (*base)(N, 14);
   DblToStr(buffer.Power, N, 2);

   (*base)(buffer.NumberBody, sizeof buffer.NumberBody, 15);
   (*base)(buffer.Chassis, sizeof buffer.Chassis, 16);

   (*base)(N, 17);
   DblToStr(buffer.Tarif, N, 2);
   (*base)(N, 18);
   DblToStr(buffer.Rate, N, 2);
   (*base)(N, 19);
   DblToStr(buffer.Discount, N, 2);
   (*base)(N, 20);
   DblToStr(buffer.AllFee, N, 2);

   DynStr Text_;
   (*base)(Text_, Text_._sizeof(), 21); //1СУ
   m_1SU->SetSelIndex(0);

   for(i = 1; i < m_1SU->GetCount(); i++)
   {
		if(Text_[0] == SendMessage(m_1SU->HWindow, CB_GETITEMDATA, i, 0))
		{
			m_1SU->SetSelIndex(i); break;
        }
   }
   //switch(Text_[0])
   //{
   //	case 'Y': m_1SU->SetSelIndex(1); break;
   //	case '0': m_1SU->SetSelIndex(2); break;
   //}
   //buffer._1SU = Text_[0] == 'Y';

   (*base)(N, 22);
   DblToStr(buffer.Fee1, N, 2);

   // to aux table (*base)(buffer.Fee1Text, sizeof buffer.Fee1Text, 23);
   ::DateFBToStr(base, 25, buffer.Fee1Date);

   IntFBToStr(base, 26, buffer.DraftNumber);

   (*base)(N, 34);
   buffer.IsFee2 = N;

   (*base)(N, 27);
   DblToStr(buffer.Fee2, N, 2);
   if(buffer.IsFee2) strcpy(buffer.Fee2Text, "евро");
   ::DateFBToStr(base, 29, buffer.Fee2Date);

   (*base)(N, 24);

   if((int)N > 1)
   {
	 buffer.Karta = 1;
   }
   else
   if((int)N > 0)
   {
	 buffer.Nal = 1;
   }
   else
   {
	 buffer.BezNal = 1;
   }

   AgName1->SetSelIndex(-1);
   AgName2->SetSelIndex(-1);
   char AGENT[10];
   (*base)(AGENT, sizeof AGENT, 30);
   SetAgent(AGENT, agents, AgName1, 0);
   int OnlySecPay = !*AGENT;

   (*base)(AGENT, sizeof AGENT, 28);
   if(*AGENT) SetAgent(AGENT, agents, AgName2, 0);

   Period->SetSelIndex(-1);
   DynStr _buffer2;
   (*base)(_buffer2, _buffer2._sizeof(), 31);
   Period->SetText(_buffer2);

   FillSuperCombo(-1, 0);
   (*base)(N, 32);
   for(i = 0; i < Super->GetCount() - 1; i++)
   {
	  if(SendMessage(Super->HWindow, CB_GETITEMDATA, i, 0) == (long)N)
	  {
	  		Super->SetSelIndex(i);
      	  	break;
	  }
   }

   while(i >= 0 && ::IsLetter(SendMessage(Super->HWindow, CB_GETITEMDATA, i, 0)) == 0)
   {
	 i--;
   }

   if((long)N != -1)
   {
	  SendMessage(Super->HWindow, CB_GETLBTEXT, i, (LPARAM)(char*)Text_);
   }
   SetDlgItemText(HWindow, 2000, Text_);

   (*base)(Text_, Text_._sizeof() , 33); //field inscomp
   m_UrFizich1->SetText(*Text_ == 'U' ? "Юрид." : "Физич.");

   (*base)(Text_, Text_._sizeof(), 69); //field inscomp
   m_UrFizich2->SetText(*Text_ == 'U' ? "Юрид." : *Text_ == 'F' ? "Физич." : "");


   ///////////////
   (*base)(N, 35);
   buffer.IsPay2 = N;
   ::DateFBToStr(base, 36, buffer.Pay2RealDate);
   (*base)(N, 37);
   DblToStr(buffer.Pay2Rate, N, 2);
   (*base)(N, 38);
   DblToStr(buffer.Pay2RubSumma, N, 2);
   (*base)(N, 39);
   buffer.Nal_2 = N;
   buffer.BezNal_2 = !buffer.Nal_2;
   (*base)(N, 40);
   DblToStr(buffer.Pay2DraftNumber, N, 2);

   Pay1Curr->SetSelIndex(-1);
   Pay2Curr->SetSelIndex(-1);
   (*base)(_buffer2, _buffer2._sizeof(), 42);
   Pay1Curr->SetText(_buffer2);
   (*base)(_buffer2, _buffer2._sizeof(), 43);
   Pay2Curr->SetText(_buffer2);

//PART2
   //1. "TarifGlass",    //46
   (*base)(N, 46);
   DblToStr(buffer.TarifGlass, N, 2);

   char PlacementStr[2];
   (*base)(PlacementStr, sizeof PlacementStr, 47);
   m_K3->SetSelIndex(-1);

   sprintf(_buffer2, "%g", DecodeK3(PlacementStr));
   m_K3->SetText(_buffer2);
   if(m_K3->GetSelIndex() == -1 && strcmp(_buffer2, "0"))
   {
	   m_K3->AddString(_buffer2);
	   m_K3->SetText(_buffer2);
	   MessageBeep(0);
   }
   //"Placement",     //47
   // don't init (*base)(buffer2, sizeof buffer2, 47);
   // don't init m_Placement->SetText(buffer2);
   m_Placement->SetSelIndex(-1);

   //"K",             //48
   (*base)(N, 48);

   for(i = 0; i < m_Placement->GetCount(); i++)
      if(SendMessage(m_Placement->HWindow, CB_GETITEMDATA, i, 0) == int(N * 100 + 0.0001))
	 m_Placement->SetSelIndex(i);


   //"ClassOldAv",    //49
   _m_OldClassAv->SetSelIndex(-1);
   (*base)(_buffer2, _buffer2._sizeof(), 49);
   _m_OldClassAv->SetText(_buffer2);
   sprintf(buffer.K1, "%05ld", long(N * 10000+0.5));
   memmove(buffer.K1 + 1, buffer.K1, strlen(buffer.K1));
   buffer.K1[1] = '.';
   while(buffer.K1[strlen(buffer.K1)-1] == '0')
	   buffer.K1[strlen(buffer.K1)-1] = 0;


   //"K2"             //53
   (*base)(N, 53);
   _m_OldClassAv->SetSelIndex(-1);
   for(i = 0; i < _m_OldClassAv->GetCount(); i++) {
	 if(SendMessage(_m_OldClassAv->HWindow, CB_GETITEMDATA, i, 0) == long(N * 100+0.00001))
	   _m_OldClassAv->SetSelIndex(i);
   }

   ::DateFBToStr(base, 54, buffer.StopDate);

   (*base)(N, 55);
   m_Osnovanie->SetSelIndex(-1);
   for(i = 0; i < m_Osnovanie->GetCount(); i++) {
     if(SendMessage(m_Osnovanie->HWindow, CB_GETITEMDATA, i, 0) == (long(N) * 100))
       m_Osnovanie->SetSelIndex(i);
   }

   (*base)(N, 56);
   buffer.Fizich_ = (fabs(N - 1.)<0.001);
   buffer.Uridich = (fabs(N - 0.)<0.001);
   buffer.IP = (fabs(N - 2.)<0.001);

   (*base)(N, 57);  //version
   // SetVersion(N);

   (*base)(N, 58);
   SetState(N);

   (*base)(buffer.CityCode, sizeof buffer.CityCode, 59);

   //field 60 updatedate

   *buffer.RetDate=0;
   (*base)(N, 61);
   int year, day, month;
  PXDateDecode(
        N,                // long value to decode
		&month,             // decoded month value
        &day,               // decoded date value
		&year);

   if(!base->IsNull(61) && N > 5000)
   	sprintf(buffer.RetDate, "%02u.%02u.%04u", day, month, year);

   (*base)(N, 62);
   buffer.ToReport = N==101.;
   //buffer.BaseCarCode = N;
   (*base)(N, 63);
   buffer._CarCode_ = N;

   (*base)(N, 64);
   DblToStr(buffer.RetBRB, N, 2);
   (*base)(N, 65);
   DblToStr(buffer.RetEUR, N, 2);

  (*base)(N, 66);
   if(!base->IsNull(66))
   {
      if(N > 0)  
	     DblToStr(buffer.AgPercent1, N, 0);
      else
	     sprintf(buffer.AgPercent1, "%0.1f", N);
      strcat(buffer.AgPercent1, "%");
   }

  	(*base)(N, 63);
   	if(!base->IsNull(63))
   	{
      if(N > 0)  
	     DblToStr(buffer.AgPercent2, N, 0);
      else
		 sprintf(buffer.AgPercent2, "%0.1f", N);
	  strcat(buffer.AgPercent2, "%");
	}

	::DateFBToStr(base, 67, buffer.RetDate1);
   	::DateFBToStr(base, 68, buffer.RetDate2);
	(*base)(buffer.MobilePhone, sizeof buffer.MobilePhone, 70);
	(*base)(buffer.HomePhone, sizeof buffer.HomePhone, 71);
	(*base)(buffer.Contact, sizeof buffer.Contact, 72);
	(*base)((char*)&buffer.SMS, 2, 73);
	buffer.SMS = !!buffer.SMS;

//PART2 END

	ModifyMandatory = 0;

   	if(OnlySecPay)
   	{
	    buffer.OwnPolis = 1;
   	}

	//Taxi

   if(Komplex )
   {
	   (*base)(N, 74);
	   DblToStr(buffer.TaxiSumma, fabs(N), 2);
		m_TaxiInOut->SetSelIndex(N == 0 ? 0 : N > 0 ? 1: 2);

	   (*base)(N, 79);
	   DblToStr(buffer.TaxiPayDoc, N, 0);

	   ::DateFBToStr(base, 78, buffer.TaxiPayDate);

       char buff[4];
	   (*base)(buff, sizeof buff, 75);
	   m_TaxiPayCurr->SetSelIndex(-1);
	   m_TaxiPayCurr->SetText(buff);

	   (*base)(buff, sizeof buff, 76);
	   buffer.TaxiIsNal = buff[0] = 'Y';
	   (*base)(buff, sizeof buff, 77);
	   buffer.Taxi1SU = buff[0] = 'Y';
	   (*base)(buff, sizeof buff, 80);

		m_TaxiVid->SetSelIndex(buff[0] == 0 ? 0 : (buff[0] - '0'));
   }

	Dlg_base::GetDataFromBase();

   	TMessage msg;
	CMOwnPolis(msg);
	ChangeInOut(msg);

	e[22+2]->GetText(Text_, Text_._sizeof());
	e[23+2]->SetText(GetSummaText(atof(Text_), Rubl));
	GetPrivateProfileString(MANDATORY_SECT, "Company", "", Text_, Text_._sizeof(), ::ININame);
   	e[29+2]->SetText(Text_);


   	EnableWindow(IsPay2->HWindow, buffer.IsFee2);
   	IsPay2Check(msg);
   	EnableWindow(GetDlgItem(HWindow, 406), FALSE);
   	int IsPay2OK = IsFee2->GetCheck() == BF_CHECKED;
   	EnableWindow(GetDlgItem(HWindow, 58), IsPay2OK && (m_State == 0));

	/// Init Avarias
	ClearAvarias(0);
	if(avaria2.pxErr == PXSUCCESS)
        {
	avaria2(0, buffer.Seria);
   	avaria2(1, atof(buffer.Number));

	if(PXSrchKey(avaria2.getTblHandle(), avaria2.getRecHandle(), 2, SEARCHFIRST) == PXSUCCESS)
	{
		for(;;)
		{
		   if(avaria2.Get() == 0)
		   {
		   		MessageBox(HWindow, "Ошибка чтения полиса", "Stop", MB_OK | MB_TASKMODAL | MB_ICONSTOP);
		   		break;
           }

	       ////////////
	       avaria2(N, 2);
	       sprintf(avarias2[avariaCount].N, "%d", (int)N);
	       ::DateFBToStr(&avaria2, 3, avarias2[avariaCount].AVDate);
	       (avaria2)(avarias2[avariaCount].AVPlace, sizeof avarias2[avariaCount].AVPlace, 4);
	       (avaria2)(avarias2[avariaCount].WorkNmb, sizeof avarias2[avariaCount].WorkNmb, 5);
	       ::DateFBToStr(&avaria2, 6, avarias2[avariaCount].WrDate);
	       avaria2(N, 7);
	       avarias2[avariaCount].Decision = N;
	       avaria2(N, 8);
	       avarias2[avariaCount].AVDocument = N;
	       (avaria2)(avarias2[avariaCount].SN, sizeof avarias2[avariaCount].SN, 9);
	       (avaria2)(avarias2[avariaCount].FIO, sizeof avarias2[avariaCount].FIO, 10);
	       avaria2(N, 11);
	       avarias2[avariaCount].IsBMFizich = N;
	       //avarias2[avariaCount].IUridich = !avarias2[avariaCount].Fizich;
	       (avaria2)(avarias2[avariaCount].FIOAddress, sizeof avarias2[avariaCount].FIOAddress, 12);
	       (avaria2)(avarias2[avariaCount].BadMan, sizeof avarias2[avariaCount].BadMan, 13);
	       avaria2(N, 14);
	       sprintf(avarias2[avariaCount].Staj, "%g", N);
	       avaria2(N, 15);
	       avarias2[avariaCount].VTR = N;

	       avaria2(N, 16);
	       DblToStr(avarias2[avariaCount].TC_1, N);
	       //sprintf(avarias2[avariaCount].TC_1, "%g", N);
	       avaria2(N, 17);
	       DblToStr(avarias2[avariaCount].TC_1V, N);
	       (avaria2)(avarias2[avariaCount].VALUTA, sizeof avarias2[avariaCount].VALUTA, 18);
	       avaria2(N, 19);
	       DblToStr(avarias2[avariaCount].TC_2_1, N);
	       avaria2(N, 20);
	       DblToStr(avarias2[avariaCount].TC_2_1V, N);
	       (avaria2)(avarias2[avariaCount].TC_2_CUR, sizeof avarias2[avariaCount].TC_2_CUR, 21);
	       ::DateFBToStr(&avaria2, 22, avarias2[avariaCount].TC_2_1VDate);

	       avaria2(N, 23);
	       DblToStr(avarias2[avariaCount].HL_1, N);
	       avaria2(N, 24);
	       DblToStr(avarias2[avariaCount].HL_1V, N);
	       (avaria2)(avarias2[avariaCount].HL_1_CUR, sizeof avarias2[avariaCount].HL_1_CUR, 25);
	       avaria2(N, 26);
	       DblToStr(avarias2[avariaCount].HL_2_1, N);
	       avaria2(N, 27);
	       DblToStr(avarias2[avariaCount].HL_2_1V, N);
	       (avaria2)(avarias2[avariaCount].HL_2_CUR, sizeof avarias2[avariaCount].HL_2_CUR, 28);
	       ::DateFBToStr(&avaria2, 29, avarias2[avariaCount].HL_2_1VDate);

	       avaria2(N, 30);
	       DblToStr(avarias2[avariaCount].IM_1, N);
	       avaria2(N, 31);
	       DblToStr(avarias2[avariaCount].IM_1V, N);
	       (avaria2)(avarias2[avariaCount].IM_1_CUR, sizeof avarias2[avariaCount].IM_1_CUR, 32);
	       avaria2(N, 33);
	       DblToStr(avarias2[avariaCount].IM_2_1, N);
	       avaria2(N, 34);
	       DblToStr(avarias2[avariaCount].IM_2_1V, N);
	       (avaria2)(avarias2[avariaCount].IM_2_CUR, sizeof avarias2[avariaCount].IM_2_CUR, 35);
	       ::DateFBToStr(&avaria2, 36, avarias2[avariaCount].IM_2_1VDate);

               ::DateFBToStr(&avaria2, 37, avarias2[avariaCount].UpdateDate);
	       (avaria2)(avarias2[avariaCount].CityCode, sizeof avarias2[avariaCount].CityCode, 38);

	       (avaria2)(avarias2[avariaCount].BMCityCode, sizeof avarias2[avariaCount].BMCityCode, 39);
	       avaria2(N, 40);
	       avarias2[avariaCount].BMDiscount = N;
	       avaria2(N, 41);
	       avarias2[avariaCount].IsBMResident = N;
	       avaria2(N, 42);
	       avarias2[avariaCount].BMOsnovan = N;

	       (avaria2)(avarias2[avariaCount].Marka, sizeof avarias2[avariaCount].Marka, 43);
	       (avaria2)(avarias2[avariaCount].BaseType, sizeof avarias2[avariaCount].BaseType, 44);
	       avaria2(N, 45);
	       DblToStr(avarias2[avariaCount].AutoCharact, N);
	       (avaria2)(avarias2[avariaCount].AutoNmb, sizeof avarias2[avariaCount].AutoNmb, 46);
	       (avaria2)(avarias2[avariaCount].BodyNo, sizeof avarias2[avariaCount].BodyNo, 47);

	       avaria2(N, 48);
	       avarias2[avariaCount].OwnerCode = N;
	       avaria2(N, 49);
	       avarias2[avariaCount].BaseCarCode = N;
	       avaria2(N, 50);
	       avarias2[avariaCount].CarCode = N;
	       avaria2(N, 51);
	       avarias2[avariaCount].PaymentCode = N;

	       avaria2(N, 52);
	       DblToStr(avarias2[avariaCount].DeclTC, N);
	       avaria2(N, 53);
	       DblToStr(avarias2[avariaCount].DeclHL, N);
	       avaria2(N, 54);
	       DblToStr(avarias2[avariaCount].DeclIM, N);
	       avaria2(avarias2[avariaCount].LossCode, sizeof avarias2[avariaCount].LossCode, 55);

	       ::DateFBToStr(&avaria2, 56, avarias2[avariaCount].DtFreeRz);
	       ::DateFBToStr(&avaria2, 57, avarias2[avariaCount].DtClLoss);

	       avaria2(N, 58);
	       DblToStr(avarias2[avariaCount].SumPay, N);
	       avaria2(N, 59);
	       DblToStr(avarias2[avariaCount].OcenTC, N);
	       avaria2(N, 60);
	       DblToStr(avarias2[avariaCount].OcenHL, N);
	       avaria2(N, 61);
	       DblToStr(avarias2[avariaCount].OcenIm, N);

  	       avaria2(N, 62);
	       DblToStr(avarias2[avariaCount].OthTC, N);
	       avaria2(N, 63);
	       DblToStr(avarias2[avariaCount].OthHL, N);
	       avaria2(N, 64);
	       DblToStr(avarias2[avariaCount].OthIM, N);

               char s[3];
	       avaria2(s, sizeof s, 65);
	       avarias2[avariaCount].IsTC1 = *s == 'Y';
	       avaria2(s, sizeof s, 66);
	       avarias2[avariaCount].IsTC2 = *s == 'Y';
	       avaria2(s, sizeof s, 67);
	       avarias2[avariaCount].IsHL1 = *s == 'Y';
	       avaria2(s, sizeof s, 68);
	       avarias2[avariaCount].IsHL2 = *s == 'Y';
	       avaria2(s, sizeof s, 69);
	       avarias2[avariaCount].IsIM1 = *s == 'Y';
	       avaria2(s, sizeof s, 70);
	       avarias2[avariaCount].IsIM2 = *s == 'Y';

	       AddIntoListBox(ListBAvaria, avarias2[avariaCount]);
	       avariaCount++;
		   if(PXSrchKey(avaria2.getTblHandle(), avaria2.getRecHandle(), 2, SEARCHNEXT) != PXSUCCESS)
           {
				break;
           }

	       if(avariaCount > COUNTAVARIAS)
		   MessageBox(HWindow, GetRCStr(503), "Stop", MB_OK | MB_TASKMODAL | MB_ICONSTOP);
	    }
	}
   	ListBAvaria->SetSelIndex(0);

	if(!CheckAvDups())
	{
		  MessageBox(HWindow, GetRCStr(633), avarias2[i].WorkNmb, MB_OK | MB_ICONSTOP);
	}
    } //avaria table is OK

   /// Init Owners
   /*	ClearOwners(0);
   

   	owners(0, buffer.Seria);
   	owners(1, atof(buffer.Number));
	if(PXSrchKey(owners.getTblHandle(), owners.getRecHandle(), 2, SEARCHFIRST) == PXSUCCESS)
	{
	   for(;;)
	   {
	   		owners.Get();

		   autoowners[ownersCount].IsSaved = 1;
		   owners(autoowners[ownersCount].Name, sizeof autoowners[ownersCount].Name, 2);
		   //DateFBToStr(&owners, 3, autoowners[ownersCount].UpdateDate);
		   DateFBToStr(&owners, 13, autoowners[ownersCount].RegDate);
		   owners(autoowners[ownersCount].Placement, sizeof autoowners[ownersCount].Placement, 4);
		   owners(N, 5);
		   //DblToStr(autoowners[ownersCount].K, N, 2);
		   autoowners[ownersCount].K = N;
		   owners(N, 6);
		   autoowners[ownersCount].Fizich = N;
		   autoowners[ownersCount].Uridich = !autoowners[ownersCount].Fizich;
		   owners(N, 7);
		   autoowners[ownersCount].Pravo = N;
		   owners(autoowners[ownersCount].Address, sizeof autoowners[ownersCount].Address, 8);
		   owners(autoowners[ownersCount].PravoTxt, sizeof autoowners[ownersCount].PravoTxt, 9);
		   owners(N, 10);
		   DblToStr(autoowners[ownersCount].WhenPrava, N, 0);
		   owners(autoowners[ownersCount].CityCode, sizeof autoowners[ownersCount].CityCode, 11);

		   owners(N, 12);
		   autoowners[ownersCount].OwnerCode = N;

		   owners(N, 14);
		   DblToStr(autoowners[ownersCount].Summa, N);

		   //owners(N, 15);
		   //DblToStr(autoowners[ownersCount].PayNumber, N);
		   owners(autoowners[ownersCount].PayDocNumber, sizeof autoowners[ownersCount].PayDocNumber, 15);

		   DateFBToStr(&owners, 16, autoowners[ownersCount].PayDate);

		   AddIntoListBox2(ListBOwners, autoowners[ownersCount]);
		   ownersCount++;
		   if(PXSrchKey(owners.getTblHandle(), owners.getRecHandle(), 2, SEARCHNEXT) != PXSUCCESS)
		   {
			  break;
		   }

		   if(ownersCount > COUNTOWNERS)
           {
			   MessageBox(HWindow, GetRCStr(504), "Stop", MB_OK | MB_TASKMODAL | MB_ICONSTOP);
           }
		}
   }
   ListBOwners->SetSelIndex(0);
   */

//   EnableWindow(m_VersionCombo->HWindow, (m_State == 0 || m_State == 4)); //Нормальный или закончилс

	LoadImagesList(buffer.Seria, atol(buffer.Number));

   UpdateFormData-=10;
}

