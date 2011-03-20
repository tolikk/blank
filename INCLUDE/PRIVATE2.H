#ifndef _PRIVATE2__H
#define _PRIVATE2__H

#include <edit.h>
#include <checkbox.h>
#include <pxtable.h>
#include <openbase.h>
#include <dlg_base.h>
#include <radiobut.h>

#pragma error

struct Private2_data
{
  char   Seria[3];
  char   Number[11];
  char   Insurer[66];
  char   BirthDate[11];
  char   Address[66];
  char   Passport[41];
  char   State[81];
  char   Conditions[251];
  char   Insurer2[66];
  char   FromDate[11];
  char   ToDate[11];
  char   Duration[31];
  char   AllSumma[31];
  char   Tarif[31];
  char   Payment[31];
  char   PaymentDate[11];
  char   WriteDate[11];
  char   Sum1[20];
  char   Sum2[20];
  char   Group[3];
  char   AgPercent[5];
  int    IsUridich;
  int    InsurType;
  char   StopDate[11];
  char   RetSum[10];
};

class Private2Dlg: public Dlg_base
{
     Private2_data  buffer;
     TOpenBase     agents;

     PTEdit        m_Seria;
     PTEdit        m_Number;
     PTEdit        m_Insurer;
     PTEdit        m_InsurerBirthDate;
     PTEdit        m_InsurerAddress;
     PTEdit        m_Passport;
     PTEdit        m_State;
     PTEdit        m_Conditions;
     PTEdit        m_Insurer2;
     PTEdit        m_FromDate;
     PTEdit        m_ToDate;
     PTEdit        m_Duration;
     PTEdit        m_AllSumma;
     PTEdit        m_Tarif;
     PTEdit        m_Payment;
     PTEdit        m_PaymentDate;
     PTEdit        m_Group;
     PTEdit        m_WriteDate;
     PTEdit        m_Sum1;
     PTEdit        m_Sum2;
	 PTEdit        m_StopDate;
	 PTEdit        m_RetSum;

     PTComboBox    m_AgentCombo;
     PTComboBox    m_Vid1Combo;
     PTComboBox    m_Vid2Combo;
     PTComboBox    m_Vid3Combo;
     PTComboBox    m_NalBezNalCombo;
     PTComboBox    m_Sum1Combo;
     PTComboBox    m_Sum2Combo;
	 PTComboBox    m_RetSumCombo;

     PTComboBox    m_Restrax;

     PTListBox     m_Ks;


public:
	     Private2Dlg(PTWindowsObject);
	     ~Private2Dlg();

//	   static void Import();
//	   static void Export(PTWindowsObject);

	   void  SetupWindow();

       void  GetDataFromBase();
        int  SetDataToBase();
       void* GetBuffer() { return &buffer; }

       void  Find() = [ID_FIRST + 400];
       void  FindSN();
       void  FindName();

       void  Clear();
       void  Print1();

       void  ChAgent(RTMessage msg) = [ID_FIRST + 202];
       void  ChangeCombo1(RTMessage) = [ID_FIRST + 207];
       void  ChangeCombo2(RTMessage) = [ID_FIRST + 208];
       void  ChangeCombo3(RTMessage) = [ID_FIRST + 209];
       void  ChangeKs(RTMessage) = [ID_FIRST + 220];
       void  SetStatesControls();
       void  FillCombo2();
       void  FillCombo3();


       void  SetStatesKs(char* States);
       void  GetStatesKs(char* strStates);
       void  FillKs();
       void  ChangeStartDate(RTMessage) = [ID_FIRST + 213];
       void  SetDates();

       void  SetInsurer2(RTMessage) = [ID_FIRST + 50];
       void  StatesMenu(RTMessage) = [ID_FIRST + 51];
       void  RoundSumma(RTMessage) = [ID_FIRST + 52];

       void  Clear(int Command);
       void  ChangeGroup(RTMessage) = [ID_FIRST + 228];
       void  ChangeNumber(RTMessage) = [ID_FIRST + 201];
       //void  Begin(RTMessage) = [ID_FIRST + 210];
       //void  Number1(RTMessage) = [ID_FIRST + 220];
       //void  Number2(RTMessage) = [ID_FIRST + 221];
       //void  Number3(RTMessage) = [ID_FIRST + 222];
       void  Change(RTMessage) = [ID_FIRST + 420]; //button to change


	   void  Calculate(RTMessage) = [1000];
       virtual void  Help(RTMessage) = [WM_SYSCOMMAND];
       virtual void  WMCtlColor(RTMessage) = [WM_FIRST + WM_CTLCOLOR];

       void WMMove(RTMessage) = [WM_FIRST + WM_MOVE];
	   void WMCommand(RTMessage);
	   void SetState(char*);
	   //void DefWndProc(RTMessage);
};

#endif