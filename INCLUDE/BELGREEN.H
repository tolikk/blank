#ifndef _BELGREEN__H
#define _BELGREEN__H

#include <edit.h>
#include <pxtable.h>
#include <openbase.h>
#include <dlg_base.h>
#include "checkbox.h"
#include "editors.h"
#include "radiobut.h"
#include "mand_av2.h"

#define BELGREEN_NORMAL 0
#define BELGREEN_LOST   5
#define BELGREEN_BAD    2
#define BELGREEN_LOST_BY_AGENT 3
#define BELGREEN_STOPPED  4
#define BELGREEN_REPLACED 1
#define BELGREEN_STATE_LAST 5

struct BELGREEN_data
{
	char Seria[10];
	char Number[11];
	char prcpNumber[11];
	char prSeria[10];
	char prNumber[11];

	char autoNmb[11];
	char Model[30 + 1];
	char Body[24];
	char Motor[24];

	char Insurer[48];
	char Organ[48];
	char Address[65 + 1];

	char dtFrom[11];
	char dtTo[11];

   	char Place[32];
	char RegDate[11];
	char Time[6];

	char Tarif[7][10];

	char PayDate[11];
	char RepDate[11];
	char Pay1[10];
	char Pay2[10];

	char AgPercent[5];

    char RetDate[11];
	char RetSumma[10];

	int AgUr;
	int InsFiz;
	int InsUr_;
	int InsIP;
	int InsResid;
	int Beznal;

	char reserve[16];
};

class LockEdit : public TEdit {
    int m_N;
public:
	LockEdit(PTWindowsObject, int Id, int Len, int N);
	void WMChar(RTMessage) = [WM_FIRST + WM_CHAR];
	void WMLButtonDown(RTMessage) = [WM_FIRST + WM_LBUTTONDOWN];
};
/*
struct  BELGREEN_AV_data {
	char NWork_Insurer[11];
	char NWork_Buro[11];
	char Date_IE[11];
	char Date_Ask_Country[11];
	char Date_Answ_Ask[11];
	char Date_Say_BBTC[11];
	char Date_Ask_Bad[11];
	char Date_Answ_Bad[11];

    char Country[16];
	char InfoSource[21];
	char ClaimSumma[11];

	char ClaimAsk[21];
	char ClaimNumber[11];
	char ClaimDateAsk[11];
	char ClaimDateGet[11];
	char ClaimDateBBTC[11];
	char ClaimPayPlus[11];
	char ClaimPay[11];

	char PayDateOnClaim[11];
	char PayDate_NotifBBTC[11];
	char PaySumm[11];
	char DocDate[11];
	char DocNumber[11];

	char ClaimCurr[4];
	char RetType;
	char ClaimPayCurr[4];
	char PayCurr[4];

};

class BELGREEN_AV: public TDialog
{
public:
	struct BELGREEN_AV_data buffer;
	BELGREEN_AV(PTWindowsObject);

	TEdit* NWork_Insurer;
	TEdit* NWork_Buro;
	TEdit* Date_IE;
	TEdit* Date_Ask_Country;
	TEdit* Date_Answ_Ask;
	TEdit* Date_Say_BBTC;
	TEdit* Date_Ask_Bad;
	TEdit* Date_Answ_Bad;

	TEdit* Country;
	TEdit* InfoSource;
	TEdit* ClaimSumma;

	TEdit* ClaimAsk;
	TEdit* ClaimNumber;
	TEdit* ClaimDateAsk;
	TEdit* ClaimDateGet;
	TEdit* ClaimDateBBTC;
	TEdit* ClaimPayPlus;
	TEdit* ClaimPay;

	TEdit* PayDateOnClaim;
	TEdit* PayDate_NotifBBTC;
	TEdit* PaySumm;
	TEdit* DocDate;
	TEdit* DocNumber;

	TComboBox* ClaimCurr;
    TComboBox* RetType;
	TComboBox* ClaimPayCurr;
	TComboBox* PayCurr;

    void  SetupWindow();

    void  Ok(RTMessage msg) = [ID_FIRST + IDOK];
};
  */
class BELGREEN: public Dlg_base
{
	BELGREEN_data   buffer;

	TEdit* m_Seria;
	NumberEdit* m_Number;
	NumberEdit* m_cepNumber;
	TEdit* m_pSeria;
	NumberEdit* m_pNumber;

	TEdit* m_autoNmb;
	INIEdit* m_Model;

	TEdit* m_Body;
	TEdit* m_Motor;

	TEdit* m_Insurer;
	TEdit* m_Organ;
	TEdit* m_Address;

	TEdit* m_dtFrom;
	TComboBox* m_Period;
	TEdit* m_dtTo;

	TEdit* m_Place;
	TEdit* m_RegDate;
	TEdit* m_Time;

	TEdit* m_RetDate;
	TEdit* m_RetSumma;

	LockEdit* m_Tarif[7];

	TEdit* m_PayDate;
	TEdit* m_RepDate;
	NumberEdit* m_Pay1;
	TComboBox* m_Curr1;
	NumberEdit* m_Pay2;
	TComboBox* m_Curr2;
	TCheckBox* m_IsPricep;
	TComboBox* m_Country;

	TCheckBox* m_AgUr;
	TRadioButton* m_InsUr_;
	TRadioButton* m_InsFiz;
	TRadioButton* m_InsIP;
	TCheckBox* m_InsResid;
	TCheckBox* m_Beznal;

	TComboBox* m_Agent;
	NumberEdit* m_AgPercent;

	TComboBox* m_RetCurr;
	TOpenBase agents;
	TComboBox* m_DupType1;

	int m_State;
	DATE m_pRegDate;

	char OldCurr[5];

	char BaseSumm[10];

public:
		 BELGREEN(PTWindowsObject);
		 ~BELGREEN();

	   static void Export(PTWindowsObject Parent);
	   static void Import();

	   void  SetupWindow();

	   void  GetDataFromBase();
        int  SetDataToBase();
       void  SetButtons();
       void* GetBuffer() { return &buffer; }
	   void  SetState(int, char* = 0);

	   virtual void  Print1() = [ID_FIRST + 408];
	   void  Print2() = [ID_FIRST + 409];
	   void  PrintAll() = [ID_FIRST + 333];
	   void  PrintData(int Flag);
	   void  SetVal() = [ID_FIRST + 600];
	   int  auxFindSN(char* Seria, double Number, int Msg = 1);
	   void  FindSN() = [ID_FIRST + 720];
       void  FindPrSN() = [ID_FIRST + 721];
       void  FindName() = [ID_FIRST + 722];
	   void  CreatePricep();
	   void  CreatePolisPricep() = [ID_FIRST + 337];
	   void  Avaria() = [ID_FIRST + 105];
	   void  ChPS(RTMessage msg) = [ID_FIRST + 324];
	   void  ChPN(RTMessage msg) = [ID_FIRST + 325];
	   void  ChAgent(RTMessage msg) = [ID_FIRST + 500];
       void  ChPeriod(RTMessage msg) = [ID_FIRST + 307];
	   void  ChCountry(RTMessage msg) = [ID_FIRST + 314];
	   void  ChStartDate(RTMessage msg) = [ID_FIRST + 305];
	   void  Increment(RTMessage msg) = [ID_FIRST + 308];
	   void  LOST(RTMessage msg) = [ID_FIRST + 338];
	   void  BAD(RTMessage msg) = [ID_FIRST + 339];
	   void  FindByNmb(RTMessage msg) = [ID_FIRST + 106];
	   void  FindByAutoNmb(RTMessage msg) = [ID_FIRST + 111];
	   void  FindByFIO(RTMessage msg) = [ID_FIRST + 107];
	   void  ChangeCurrency(RTMessage msg) = [ID_FIRST + 501];
	   void  ChangeAutoType(RTMessage msg) = [WM_USER + 1];
	   void  WMCtlColor(RTMessage msg) = [WM_CTLCOLOR];

	   void  Fiz(RTMessage msg) = [ID_FIRST + 700];
	   void  Ur(RTMessage msg) = [ID_FIRST + 330];
	   void  IP(RTMessage msg) = [ID_FIRST + 701];

	   void  ReplacePolis(RTMessage msg) = [ID_FIRST + 110];
	   void  DupPolis(RTMessage msg) = [ID_FIRST + 109];

	   void  Clear();
	   void  Default();
       void  WMCommand(RTMessage msg);
       virtual void  Find() = [ID_FIRST + 400];
	   int IsExistsNext(int Reposition);
       void  SetFlagPriced(int);
	   char* CreateAddText(char* smsg, char* nmb);
	   void FindAutoNmb();
       void FindOrg();
       void FindPricep();

       void  CalcEndDate();
	   void  CalcTarif(int IndexAutoType);
	   void  CreateNextPolis(int State);
};

#endif