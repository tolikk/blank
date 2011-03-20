#ifndef _PRIVATE2005__H
#define _PRIVATE2005__H

#include <edit.h>
#include <checkbox.h>
#include <pxtable.h>
#include <openbase.h>
#include <dlg_base.h>
#include <radiobut.h>

#pragma error

struct Private2005_data
{
  char   Number[11];
  char   PSeria[3];
  char   PNumber[11];
  char   RegDate[11];
  char   Insurer[66];
  char   InsCnt[3];
  char   Address[66];
  char   BirthDate[11];
  char   Passport[41];
  char   Conditions[129];
  char   Country[50];
  char   Insurer2[66];
  char   Passport2[41];
  int	 InsType;
  char   FromDate[11];
  char   FromTime[6];
  char   Period[4];
  char   ToDate[11];
  char   Duration[4];
  char   UridSumm[10];
  char   Tarif[10];
  char   Payment[10];
  char   PaymentDate[11];
  char   PaymentNumber[11];
  char   ReportDate[11];
  int    PayNal;
  char   Sum1[10];
  char   Sum2[10];
  char   AgPercent[5];
  int    AgUrid;
  char   StopDate[11];
  char   RetSum[10];
  char   _Seria[3];
};

class Private2005Dlg: public Dlg_base
{
     Private2005_data  buffer;
     TOpenBase     agents;

	 PTComboBox	   m_Seria;
     PTEdit        m_Number;
	 PTEdit        m_PSeria;
	 PTEdit        m_PNumber;
	 PTEdit        m_RegDate;
	 PTEdit        m_Insurer;
	 PTEdit        m_InsCnt;
	 PTEdit        m_InsurerAddress;
	 PTEdit        m_InsurerBirthDate;
	 PTEdit        m_Passport;
	 PTEdit        m_Insurer2;
	 PTEdit        m_Passport2;
	 PTEdit        m_Conditions;
	 PTEdit        m_Country;
	 PTEdit        m_FromDate;
	 PTEdit        m_FromTime;
	 PTEdit        m_ToDate;
     PTEdit        m_Duration;
	 PTEdit        m_UridSum;
	 PTEdit        m_Tarif;
     PTEdit        m_Payment;
     PTEdit        m_PaymentDate;
	 PTEdit        m_PaymentNumber;
	 PTEdit        m_ReportDate;
	 PTEdit        m_Sum1;
     PTEdit        m_Sum2;
	 PTEdit        m_StopDate;
	 PTEdit        m_RetSum;
	 PTEdit        m_AgPercent;
	 PTEdit    	   m_Period1;

     PTComboBox    m_AgentCombo;
     PTComboBox    m_Countries;
     PTComboBox    m_InsSum;
     PTComboBox    m_InsSumCurr;
	 PTComboBox    m_Sum1Combo;
     PTComboBox    m_Sum2Combo;
	 PTComboBox    m_RetSumCombo;
	 PTComboBox    m_Assistant;

	 PTListBox     m_Ks;

	 PTCheckBox    m_InsFiz;
	 PTCheckBox    m_PayNal;
	 PTCheckBox    m_AgUr;

	 char OldCurr[5];

	 char BaseSumm[15];

public:
	     Private2005Dlg(PTWindowsObject);
	     ~Private2005Dlg();

	   static void Import();
	   static void Export(PTWindowsObject);

	   void  SetupWindow();

       void  GetDataFromBase();
        int  SetDataToBase();
       void* GetBuffer() { return &buffer; }

       void  Find() = [ID_FIRST + 400];
       void  FindSN();
	   void  FindName(int);

       void  Clear();
       void  Print1();

       void  ChAgent(RTMessage msg) = [ID_FIRST + 202];
       void  ChangeCountry(RTMessage) = [ID_FIRST + 209];
	   void  ChangeInsSumma(RTMessage) = [ID_FIRST + 207];
	   void  ChangeKs(RTMessage) = [ID_FIRST + 220];
	   void  ChMainCurr(RTMessage) = [ID_FIRST + 208];


       void  SetStatesKs(char* States);
       void  GetStatesKs(char* strStates);
       void  FillKs();
       void  ChangeStartDate(RTMessage) = [ID_FIRST + 213];
	   void  ChangeDuration(RTMessage) = [ID_FIRST + 215];
	   void  ChangePeriod3(RTMessage) = [ID_FIRST + 62];
	   void  SetDates();
	   void  ChPayCurr(RTMessage) = [ID_FIRST + 219];

       void  SetInsurer2(RTMessage) = [ID_FIRST + 50];
	   void  RoundSumma(RTMessage) = [ID_FIRST + 52];

	   void  ChangeGroup(RTMessage) = [ID_FIRST + 228];
       void  ChangeNumber(RTMessage) = [ID_FIRST + 201];
	   void  Change(RTMessage) = [ID_FIRST + 420]; //button to change

	   void  Calculate(RTMessage) = [1000];
       virtual void  Help(RTMessage) = [WM_SYSCOMMAND];
       virtual void  WMCtlColor(RTMessage) = [WM_FIRST + WM_CTLCOLOR];

       void WMMove(RTMessage) = [WM_FIRST + WM_MOVE];
	   void WMCommand(RTMessage);
	   void SetState(char*);
};

#endif