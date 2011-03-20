#ifndef _GREENCARD__H
#define _GREENCARD__H

#include <edit.h>
#include <pxtable.h>
#include <openbase.h>
#include <dlg_base.h>
#include <COMBOBOX.H>

/*
struct DB1_data
{
   char Seria[5];
   char Car_num[41];
   char Car_art[81];
   char Owner[81];
   char Firma[51];
   char Date_from[11];
   char Date_to[11];
   char Litera[2];
   char T1_1[9];
   char T2_1[9];
   char T1_2[9];
   char T2_2[9];
   char T1_3[9];
   char T2_3[9];
   char T1_4[9];
   char T2_4[9];
   char T1_5[9];
   char T2_5[9];
   char T2_6[9];
   char T2_7[11];
   char Location[21];
   char Date[11];
   char Time[6];
   char Number[7];
   char Greens_seria[11];
   char Greens_number[7];
   char Pay_Date[11];
   char ValutaSum[11];
   char ValutaSum2[9];
   char ValutaType[4];
};

class DB1 : public Dlg_base
{
     TComboBox*   AgName;
     TComboBox*   ValutaType;
     TComboBox*   ValutaType2;
     TOpenBase    agents;
     PTEdit     e[29];
     DB1_data   buffer;
     char       OLDAGENT[5];
     void       AuxGCGetData(TOpenBase* base, DB1_data& buffer);
     float      CalcSumma();
     int	InitData;

public:
             DB1(PTWindowsObject);
             ~DB1();

       void  SetupWindow();

       virtual int   Update();
       void  GetDataFromBase();
        int  SetDataToBase();
       void  SetButtons();
       void* GetBuffer() { return &buffer; }


       void  Find() = [ID_FIRST + 400];
       void  PrintAll() = [ID_FIRST + 410];
       void  ToGreens() = [ID_FIRST + 420];
       void  ToValuta() = [ID_FIRST + 600];

       int  FindSN();
       int  FindTransport();
       int  FindDZK();
       int  FindOwner();

       void  Clear();
       void  Print1();
       void  Print2();

       virtual void  Help(RTMessage) = [WM_SYSCOMMAND];

       virtual void  WMCommand(RTMessage);
};
*/

#endif