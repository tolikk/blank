#ifndef _RUSSIAN__H
#define _RUSSIAN__H

#include <edit.h>
#include <pxtable.h>
#include <openbase.h>
#include <checkbox.h>
#include <dlg_base.h>
#include "editors.h"

#define RUSS_NORMAL 0
#define RUSS_LOST   1
#define RUSS_BAD    2
#define RUSS_STOP   3

struct Russ_data
{
	char Number[11];
	char prSeria[5];
	char prNumber[11];

	char Regdate[11];

	char dtFrom[11];
	char tmFrom[6];
	char dtTo[11];

    char Insurer[49];
	char InsurerType[10];
	char Owner[49];
	char OwnerType[10];

	char Marka[25];
	char IdNumber[18];
	char PassSer[5];
	char PassNmb[7];
	char AutoNmb[13];
	char Charact[4];

	char SpecSer[4];
	char SpecNumber[11];

    char Tarif[10];

	char Sum1[10];
	char Sum2[10];
	char PayDate[11];
	char RepDate[11];
	char Percent[6];
	char AgType[10];

	char StopDate[11];
	char RetSum[10];

	int  Resident;

	char Seria_[5];

	char reserve[16];
};                     

class Russian: public Dlg_base
{
	Russ_data   buffer;

	TComboBox* m_Seria;
	NumberEdit* m_Number;
	TEdit* m_prSeria;
	NumberEdit* m_prNumber;

	TEdit* m_dtFrom;
	TEdit* m_tmFrom;
	TEdit* m_dtTo;

	TEdit* m_RegDate;

	TEdit* m_Insurer;
	UrFizich* m_InsurerType;
	TEdit* m_Owner;
	UrFizich* m_OwnerType;

	TEdit* m_Marka;
	TEdit* m_IdNumber;
	TEdit* m_PassSer;
	TEdit* m_PassNmb;
	TEdit* m_AutoNmb;
	TEdit* m_SpecSer;
	NumberEdit* m_SpecNmb;
	NumberEdit* m_Charact;
	NumberEdit* m_Tarif;
	NumberEdit* m_Sum1;
	NumberEdit* m_Sum2;
	TEdit* m_PayDate;
	TEdit* m_RepDate;
	NumberEdit* m_Percent;
	UrFizich* m_AgType;
	TEdit* m_StopDate;
	NumberEdit* m_RetSum;

	TComboBox* m_Period;
	TComboBox* m_Val1;
	TComboBox* m_Val2;
	TComboBox* m_RetVal;
	TComboBox* m_Agent;
	TComboBox* m_State;
	TComboBox* m_Company;
	TComboBox* m_AutoType;

	TCheckBox* m_Resident;

	TOpenBase agents;

public:
	     Russian(PTWindowsObject);
		 ~Russian();

	   static void Import();
	   static void Export(PTWindowsObject Parent);

	   void  SetupWindow();

       void  GetDataFromBase();
        int  SetDataToBase();
       void  SetButtons();
       void* GetBuffer() { return &buffer; }
       void  SetState(int);

	   void  ChRegDate(RTMessage msg) = [ID_FIRST + 115];
	   void  SetVal(RTMessage) = [ID_FIRST + 118];
	   void  ChPayCurr(RTMessage) = [ID_FIRST + 203];
	   void  FindSN(RTMessage) = [ID_FIRST + 120];
	   void  FindByField(int, TEdit*);
	   void  FindIns(RTMessage) = [ID_FIRST + 123];
	   void  FindPrSN(RTMessage) = [ID_FIRST + 121];
	   void  FindName(RTMessage) = [ID_FIRST + 119];
       void  ChAgent(RTMessage msg) = [ID_FIRST + 205];
       void  ChPeriod(RTMessage msg) = [ID_FIRST + 200];
	   void  ChAutoType(RTMessage msg) = [ID_FIRST + 201];
	   void  ChStartDate(RTMessage msg) = [ID_FIRST + 104];
       void  FindAutoNumber(RTMessage msg) = [ID_FIRST + 127];
	   void  CopyInsurer(RTMessage msg) = [ID_FIRST + 129];
	   void  ChInsurerType(RTMessage msg) = [ID_FIRST + 125];
	   void  ChCharact(RTMessage msg) = [ID_FIRST + 111];
	   void  ChResident(RTMessage msg) = [ID_FIRST + 137];
	   void  ChNormal(RTMessage msg) = [ID_FIRST + 138];
	   void  Clear();
	   void  Print(RTMessage) = [ID_FIRST + 400];
	   void  Default();
	   virtual void  Help(RTMessage) = [WM_SYSCOMMAND];

       void  CalcEndDate();
       void  CalcTarif();
	   void  InitTime();

	   char  OldCurr[5];
};

#endif