#ifndef _PRIVATE3__H
#define _PRIVATE3__H

#include <edit.h>
#include <checkbox.h>
#include <pxtable.h>
#include <openbase.h>
#include <dlg_base.h>
#include <radiobut.h>

#pragma error
 /*
struct Private3_data
{
char Seria[10];
char Number[11];
char InsComp[48];
char RegDate[11];
char RegTime[6];
char FIO[64];
char State[32];
char Passport[32];
char Srok[4];
char DateFrom[11];
char DateTo[11];
char Insurer[64];
char Summa[11];
char Fee[6];
char PaySumm[11];
char PayDate[11];
char AgPercent[4];
int  IsUridich;

char Danger[16];
};

//extern char* AgentFields[10];
extern char* AgentDBName;

class Private3Dlg: public Dlg_base
{
     Private3_data  buffer;
     TOpenBase     agents;

     PTEdit        m_Seria;
     PTEdit        m_Number;
     PTEdit        m_InsComp;
     PTEdit        m_RegDate;
     PTEdit        m_RegTime;
     PTEdit        m_FIO;
     PTEdit        m_State;
     PTEdit        m_Passport;
     PTEdit        m_Srok;
     PTEdit        m_DateFrom;
     PTEdit        m_DateTo;
     PTEdit        m_Insurer;
     PTEdit        m_Summa;
     PTEdit        m_Fee;
     PTEdit        m_PaySumma;
     PTEdit        m_PayDate;
     PTEdit        m_AgPercent;

     PTComboBox    m_AgentCombo;
     PTComboBox    m_NalBezNalCombo;
     PTComboBox    m_PayCurr;

public:
	     Private3Dlg(PTWindowsObject);
	     ~Private3Dlg();

       void  SetupWindow();

       void  GetDataFromBase();
        int  SetDataToBase();
       void* GetBuffer() { return &buffer; }

       void  ChAgent(RTMessage msg) = [ID_FIRST + 202];
       void  Find() = [ID_FIRST + 400];
       void  FindSN();
       void  FindName();

       void  Clear();
       void  Print1();

       void  ChangeStartDate(RTMessage) = [ID_FIRST + 210];
       void  ChangeSrok(RTMessage) = [ID_FIRST + 212];
       void  SetDates();

       void  SetInsurer2(RTMessage) = [ID_FIRST + 50];
       void  StatesMenu(RTMessage) = [ID_FIRST + 51];

       //void  Clear(int Command);
       void  ChangeNumber(RTMessage) = [ID_FIRST + 201];
       void  Change(RTMessage) = [ID_FIRST + 420]; //button to change

       void  Calculate(RTMessage) = [WM_FIRST + 1];
       virtual void  Help(RTMessage) = [WM_SYSCOMMAND];
       virtual void  WMCtlColor(RTMessage) = [WM_FIRST + WM_CTLCOLOR];

       void WMMove(RTMessage) = [WM_FIRST + WM_MOVE];
       void WMCommand(RTMessage);
};
*/

#endif