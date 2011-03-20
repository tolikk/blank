#ifndef _WARTA__H
#define _WARTA__H

#include <edit.h>
#include <pxtable.h>
#include <openbase.h>
#include <dlg_base.h>

struct DB2_data
{
char Number[26];
char Branch[16];
char Phone[21];
char Telex[8];
char Fax[11];
char Iname[61];
char Iaddress[61];
char GetDate[11];
char Isum_n[8];
char Isum[71];
char Subject1[41];
char Subject2[71];
char Subject3[51];
char Subject4[31];
char Text_1[251];
char From[21];
char To[21];
char About[26];
char Clause[231];
char City[21];
char Agent[121];
char Payment[181];
};

class DB2: public Dlg_base
{
     PTEdit     e[22];
     DB2_data   buffer;

public:
             DB2(PTWindowsObject);
             ~DB2();

       void  SetupWindow();

       void  GetDataFromBase();
        int  SetDataToBase();
       void  SetButtons();
       void* GetBuffer() { return &buffer; }


       void  Find() = [ID_FIRST + 400];
       void  Clear();
       void  Print1();
       void  Default();
       virtual void  Help(RTMessage) = [WM_SYSCOMMAND];
};

#endif