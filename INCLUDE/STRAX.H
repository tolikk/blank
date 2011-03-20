#ifndef _STRAX__H
#define _STRAX__H

#include <edit.h>
#include <pxtable.h>
#include <openbase.h>
#include <dlg_base.h>
/*
struct DB3_data
{
   char Seria[3];
   char Number[10];
   char Insurance[101];
   char Address[66];
   char Ins_form[66];
   char Insurant[101];
   char I_address[101];
   char Regul_date[11];
   char Regul_number[7];
   char Statment_date[11];
   char Ins_object[356];
   char Persent[10];
   char Period[11];
   char Date_from[11];
   char Date_to[11];
   char Ins_sum[13];
   char Ins_cur[4];
   char Ins_sum_say[256];
   char Ins_pay[13];
   char Ins_pay_cur[4];
   char Ins_pay_say[256];
   char Payer[71];
   char Pay_date[11];
   char Pay_doc[9];
   char Ins_pay2[13];
   char Ins_cur2[4];
   char Ins_pay2_say[121];
   char Pay_date2[11];
   char Ins_term[41];
   char Forse_date[11];
   char Other[4];
   char Ins_other[41];
   char Sum_other[13];
   char Cur_other[4];
   char Sum_other_say[121];
   char Continue[41];
   char Incidents[501];
   char Special[601];
   char Date[11];
   char Sec_Number[12];
   char Invoice[91];
};

#include "radiobut.h"
#include "combobox.h"

class DB3: public Dlg_base
{
     PTEdit       e[41];
     PTComboBox   v[4];
     PTComboBox   Word;
     PTRadioButton r[3];
     DB3_data   buffer;

public:
             DB3(PTWindowsObject);
             ~DB3();

       void  SetupWindow();

       void  GetDataFromBase();
        int  SetDataToBase();
       void  SetButtons();
       void* GetBuffer() { return &buffer; }

       void  Check1(RTMessage) = [ID_FIRST + 101];
       void  Check2(RTMessage) = [ID_FIRST + 102];
       void  Check3(RTMessage) = [ID_FIRST + 103];
       void  NoFocus();
       void  Save();


       void  Find(RTMessage) = [ID_FIRST + 400];
       int   Find();
       void  Clear();
       void  PreparePrintBlank2(char* str, int Is__);
       void  Print1();
       void  Print2();
       void  Print3() = [ID_FIRST + 500];
       void  Print4() = [ID_FIRST + 501];
       void  Default();

       virtual void  WMCtlColor(RTMessage) = [WM_FIRST + WM_CTLCOLOR];
       virtual void  Help(RTMessage);
};
*/

#endif