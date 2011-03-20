#ifndef __UNIVERSAL_H
#define __UNIVERSAL_H

#include "dateedit.h"
#include "editors.h"

typedef struct
{
	char Number[11];
	int  Fiz;
	int  Ur;
	char RegDate[11];
	char StartDate[11];
	char Another[21];
	char AgentFU[5];
	char AgentPcnt[7];

	DWORD dwVid;
	char Seria[4];
    char Agent[5];
} UNIVERSAL_DATA;

typedef struct
{
	char From[11];
	char To[11];

	char Insurer[61];
	char InsAddr[61];
	char InsOther[31];
	char InsObj[501];
	char InsPcnt[10];
	char Zastr[61];
	char Vigod[61];
	char VigodAddr[61];
	char LizoZast[61];
	char PlaceIns[61];
	char InsEvents[501];
	char PropertyIns[501];
	char Variant[61];

	int Terr;
	char Period[11];
} UNIVERSAL1_DATA;

typedef struct
{
	char RealSum[11];
	char InsSum[11];
	char Fransh[501];
	char Wait[31];
	char PremSum[11];
	char Feer[31];
	char FeeSum[11];
	char FeeDate[11];
	char RepDate[11];
	int  FeeTypBeznal;
	char FeeDoc[11];
	char SrokPay[101];
	char OtherCond[501];

	char RealCurr[4];
	char InsCur[4];
	char PremCur[4];
	char FeeCur[4];
} UNIVERSAL2_DATA;

#include "radiobut.h"
#include "combobox.h"

class UNIVERSAL1: public TDialog
{
public:
			 UNIVERSAL1(PTWindowsObject, UNIVERSAL1_DATA* data);
			 ~UNIVERSAL1();

	   INIEdit*		Insurer;
	   INIEdit*		InsurerAddr;
	   PTEdit 		InsurerOther;
	   PTEdit 		InsObject;
	   NumberEdit*	InsPercent;
	   PTEdit 		ZastraxLizo;
	   PTEdit 		Vigoder;
	   PTEdit 		VigoderAddr;
	   PTEdit 		LizoWhosOtfvet;
	   PTComboBox 	Territory;
	   PTEdit 		PlaceInsur;
	   PTComboBox 	Period;
	   TDateEdit*	DtFrom;
	   TDateEdit*	DtTo;
	   INIEdit*		SetInsEvents;
	   PTEdit 		PropertyIns;
	   INIEdit*		Variant;

	   UNIVERSAL1_DATA* buffer;

	   void  SetupWindow();

	   void InitGUI(long Id);

	   void ChangePeriod(RTMessage) = [ID_FIRST + 111];
	   void FindName(RTMessage) = [ID_FIRST + 117];
	   void FindAddr(RTMessage) = [ID_FIRST + 118];
	   void FindObj(RTMessage) = [ID_FIRST + 119];

	   void SetVigoder(RTMessage) = [ID_FIRST + 120];
};

class UNIVERSAL2: public TDialog
{

public:
			 UNIVERSAL2(PTWindowsObject, UNIVERSAL2_DATA* data);
			 ~UNIVERSAL2();

	   UNIVERSAL1*  dlg1;

	   PTEdit 		RealSum;
	   PTComboBox	RealCurr;
	   PTEdit 		InsSum;
	   PTComboBox	InsCurr;
	   INIEdit*		Fransh;
	   PTEdit 		Wait;
	   PTEdit 		PremSum;
	   PTComboBox	PremCur;
	   PTEdit 		Feer;
	   PTEdit 		FeeSum;
	   PTComboBox	FeeCur;
	   TDateEdit*	FeeDate;
	   TDateEdit*	RepDate;
	   PTEdit 		FeeDoc;
	   PTCheckBox	FeeTypBeznal;
	   INIEdit*		SrokPay;
	   INIEdit*		OtherCond;

	   UNIVERSAL2_DATA* buffer;

       void  SetupWindow();

	   void InitGUI(long Id);

	   void UpdateSummTexts();
	   void ChangeSumm1(RTMessage) = [ID_FIRST + 100];
	   void ChangeSumm2(RTMessage) = [ID_FIRST + 102];
	   void ChangeSumm3(RTMessage) = [ID_FIRST + 106];
	   void ChangeSummCurr1(RTMessage) = [ID_FIRST + 101];
	   void ChangeSummCurr2(RTMessage) = [ID_FIRST + 103];
	   void ChangeSummCurr3(RTMessage) = [ID_FIRST + 107];
	   void NB(RTMessage) 		   = [ID_FIRST + 112];
	   void SetFeer(RTMessage)     = [ID_FIRST + 120];
	   void FindOtherCond(RTMessage) = [ID_FIRST + 116];

	   void Pay1(RTMessage) = [ID_FIRST + 117];
	   void Pay2(RTMessage) = [ID_FIRST + 118];
	   void Pay3(RTMessage) = [ID_FIRST + 119];
	   void Pay4(RTMessage) = [ID_FIRST + 121];
	   void PayAndSumm1(RTMessage) = [ID_FIRST + 123];
	   void PayAndSumm2(RTMessage) = [ID_FIRST + 124];
	   void PayAndSumm3(RTMessage) = [ID_FIRST + 125];

	   void GeneratePayments(int Count, int WithCurrency);
};

class UNIVERSAL: public Dlg_base
{
	 PTComboBox   	Seria;
	 PTEdit 		Number;
	 PTComboBox   	Vid;

	 PTRadioButton 	Fiz;
	 PTRadioButton 	Ur;

	 TDateEdit*		RegDate;
	 TDateEdit*		StartDate;
	 INIEdit*		Another;

	 TComboBox*      Agent;
	 UrFizich*       AgentFU;
     NumberEdit*    AgentPcnt;

	 UNIVERSAL_DATA    buffer;
	 UNIVERSAL1_DATA   buffer1;
	 UNIVERSAL2_DATA   buffer2;

	 TOpenBase     agents;

public:
			 UNIVERSAL(PTWindowsObject);
			 ~UNIVERSAL();

	   UNIVERSAL1 dlg1;
	   UNIVERSAL2 dlg2;

	   void  SetupWindow();

	   void FindSN(RTMessage) = [ID_FIRST + 111];
	   void ChangeVid(RTMessage) = [ID_FIRST + 100];
	   void ShowInfo(RTMessage) = [ID_FIRST + 108];
	   void ShowPays(RTMessage) = [ID_FIRST + 109];
	   void Invalid(RTMessage)  = [ID_FIRST + 110];
	   void ChAgent(RTMessage)  = [ID_FIRST + 112];
	   void SetStartDate(RTMessage)  = [ID_FIRST + 120];

	   void* GetBuffer() { return &buffer; }

	   virtual void Clear();
	   void ClearFields();
	   void    InitGUI();
	   void    Print() = [ID_FIRST + 400];

	   void  GetDataFromBase();
        int  SetDataToBase();

	   void  Default();

       static void Import();
	   static void Export(TWindowsObject* Parent);
};

#endif