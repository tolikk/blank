#ifndef _PRIVATE__H
#define _PRIVATE__H

#include <edit.h>
#include <pxtable.h>
#include <openbase.h>
#include <dlg_base.h>

#pragma error
/*
struct Private_data
{
  char   Seria[3];
  char   Number[11];
  char   Insured[66];
  char   Count[4];
  char   Address[66];
  char   Birthdate[11];
  char   Destination[61];
  char   Summa[125];
  char   OneSumma[25];
  char   Period[41];
  char   Begin[11];
  char   End[11];
  char   Premium[125];
  char   OnePremium[25];
  char   Payment[11];
  char   Conditions[255];
  char   Place[61];
  char   Date[11];
  char   Company[125];
  char   AdrCompany[125];
  int    Year[2];
  int    Plan[3];
  char   Mans[4];
  char   Old[4];
  char   Child[4];
  char   Valuta[10];
};

extern char* AgentFields[10];
extern char* AgentDBName;

class PrivateDlg: public Dlg_base
{
     Private_data  buffer;
     PTEdit        e[24];
     PTComboBox    AgName;
     PTComboBox    YearCombo;
     TOpenBase     agents;
     char          OLDAGENT[5];

     PTComboBox	   CurrencyCode;

public:
	     PrivateDlg(PTWindowsObject);
             ~PrivateDlg();

       void  SetupWindow();

       virtual int   Update();
       void  GetDataFromBase();
        int  SetDataToBase();
       void  SetButtons();
       void* GetBuffer() { return &buffer; }


       void  Find() = [ID_FIRST + 400];
       void  Clear();
       void  Print1();
       void  Default();

       void  NoAutoCheck1(RTMessage) = [ID_FIRST + 302];
       void  NoAutoCheck2(RTMessage) = [ID_FIRST + 303];
       void  NoAutoCheck3(RTMessage) = [ID_FIRST + 304];

//       void  SetVisibleElements();

       void  NoAutoCheckYN1(RTMessage) = [ID_FIRST + 300];
       void  NoAutoCheckYN2(RTMessage) = [ID_FIRST + 301];

       void  SetSummaCurr(RTMessage) = [ID_FIRST + 570];

       void  Period(RTMessage) = [ID_FIRST + 209];
       void  PeriodYear(RTMessage) = [ID_FIRST + 224];
       void  Vozr(RTMessage) = [ID_FIRST + 205];
       void  How(RTMessage) = [ID_FIRST + 203];
       void  Begin(RTMessage) = [ID_FIRST + 210];
       void  Number(RTMessage) = [ID_FIRST + 201];
       void  Number1(RTMessage) = [ID_FIRST + 220];
       void  Number2(RTMessage) = [ID_FIRST + 221];
       void  Number3(RTMessage) = [ID_FIRST + 222];
       void  Change(RTMessage) = [ID_FIRST + 420];

       void  makeCond(char* cond, char* val, int, int, int, double sum);

       void  Calculate(RTMessage) = [WM_FIRST + 1];
       void  Disen(RTMessage) = [CM_FIRST + 199];
       void  AuxDisen();
       virtual void  Help(RTMessage) = [WM_SYSCOMMAND];
       virtual void  WMCtlColor(RTMessage) = [WM_FIRST + WM_CTLCOLOR];
};
*/
#endif