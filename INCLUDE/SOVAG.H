#ifndef _SOVAG__H
#define _SOVAG__H

#include <edit.h>
#include <pxtable.h>
#include <openbase.h>
#include <dlg_base.h>

#define SOVAG_NORMAL 0
#define SOVAG_LOST   1
#define SOVAG_BAD    2

/*

struct SOVAG_data
{
	char Seria[11];
	char Number[11];
	char Seria2[11];
	char Number2[11];
	char prSeria[11];
	char prNumber[11];

	char Place[32];
	char RegDate[11];
	char Time[6];
	char Insurer[48];
	char Address[48];

	char dtFrom[11];
	char dtTo[11];

	char Letter[4];

	char autoNmb[24];
	char Model[24];
	char autoNmb2[24];
	char Model2[24];

	char Tarif[10];
	char PayDate[11];
	char Pay1[10];
	char Pay2[10];

	char AgPercent[5];

	char reserve[16];
};

class SOVAG: public Dlg_base
{
	SOVAG_data   buffer;

	TEdit* m_Seria;
	NumberEdit* m_Number;
	TEdit* m_Seria2;
	NumberEdit* m_Number2;
	TEdit* m_pSeria;
	NumberEdit* m_pNumber;
	TEdit* m_Place;
	TEdit* m_RegDate;
	TEdit* m_Time;
	TEdit* m_Insurer;
	TEdit* m_Address;

	TEdit* m_dtFrom;
	TComboBox* m_Period;
	TEdit* m_dtTo;

	TComboBox* m_AutoType;
	TEdit* m_Letter;

	TEdit* m_autoNmb;
	TEdit* m_Model;
	TEdit* m_autoNmb2;
	TEdit* m_Model2;

	NumberEdit* m_Tarif;
	TEdit* m_PayDate;
	NumberEdit* m_Pay1;
	TComboBox* m_Curr1;
	NumberEdit* m_Pay2;
	TComboBox* m_Curr2;

	TComboBox* m_Agent;
	NumberEdit* m_AgPercent;
	TComboBox* m_State;

	TOpenBase agents;

public:
	     SOVAG(PTWindowsObject);
	     ~SOVAG();

       void  SetupWindow();

       void  GetDataFromBase();
        int  SetDataToBase();
       void  SetButtons();
       void* GetBuffer() { return &buffer; }
       void  SetState(int);


       void  InitDZK();
       void  SetVal() = [ID_FIRST + 115];
       void  auxFindSN(char* Seria, double Number);
       void  FindSN() = [ID_FIRST + 720];
       void  FindPrSN() = [ID_FIRST + 721];
       void  FindName() = [ID_FIRST + 722];
       void  ChAgent(RTMessage msg) = [ID_FIRST + 120];
       void  ChPeriod(RTMessage msg) = [ID_FIRST + 108];
       void  ChAutoType(RTMessage msg) = [ID_FIRST + 110];
       //void  ChCountry(RTMessage msg) = [ID_FIRST + 202];
       void  ChStartDate(RTMessage msg) = [ID_FIRST + 107];
       void  Clear();
       //void  Print1();
       void  Default();
       void  WMCommand(RTMessage msg);
       virtual void  Find() = [ID_FIRST + 400];

       void  CalcEndDate();
       void  CalcTarif();
};
*/

#endif