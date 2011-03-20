#ifndef _BULSTRAD__H
#define _BULSTRAD__H

#include <edit.h>
#include <pxtable.h>
#include <openbase.h>
#include <dlg_base.h>

#define BULSTRAD_NORMAL 0
#define BULSTRAD_LOST   1
#define BULSTRAD_BAD    2

/*

struct Bulstrad_data
{
	char Seria[5];
	char Number[11];
	char prSeria[5];
	char prNumber[11];

        char dSeria[5];
	char dNumber[11];
	char dTarif[4];
	char pTarif[5];

	char dtFrom[11];
	char dtTo[11];
	char autoNmb[15];
	char shassiNmb[15];
	char Marka[21];
	char Model[21];
	char YearMade[5];
	char Owner[48];
	char Address[48];
	char Tarif[10];
	char Sum1[10];
	char Sum2[10];
	char PayDate[11];
	char Regdate[11];
	char RegTime[6];
	char Percent[6];
	char Country[15];

	//
	//char Period[24];
	//int  KategorTS;
	//int  Territoria;
	//char Curr1[4];
	//char Curr2[4];
	//int  AgentCode;
	//int  State;
	char reserve[16];
};                     

class Bulstrad: public Dlg_base
{
	Bulstrad_data   buffer;

	TEdit* m_Seria;
	NumberEdit* m_Number;
	TEdit* m_prSeria;
	NumberEdit* m_prNumber;

	TEdit* m_dSeria;
	NumberEdit* m_dNumber;
	NumberEdit* m_dTarif;
	NumberEdit* m_pTarif;

	TEdit* m_dtFrom;
	TEdit* m_dtTo;
	TEdit* m_autoNmb;
	TEdit* m_shassiNmb;
	TEdit* m_Marka;
	TEdit* m_Model;
	TEdit* m_CountryGet;
	NumberEdit* m_YearMade;
	TEdit* m_Owner;
	TEdit* m_Address;
	NumberEdit* m_Tarif;
	NumberEdit* m_Sum1;
	NumberEdit* m_Sum2;
	TEdit* m_PayDate;
	TEdit* m_RegDate;
	TEdit* m_RegTime;
	NumberEdit* m_Percent;

        TComboBox* m_State;
	TComboBox* m_Period;
	TComboBox* m_Country;
	TComboBox* m_Kategor;
	TComboBox* m_Val1;
	TComboBox* m_Val2;
	TComboBox* m_Agent;

	TOpenBase agents;

public:
	     Bulstrad(PTWindowsObject);
	     ~Bulstrad();

       void  SetupWindow();

       void  GetDataFromBase();
        int  SetDataToBase();
       void  SetButtons();
       void* GetBuffer() { return &buffer; }
       void  SetState(int);


       void  ChRegDate(RTMessage msg) = [ID_FIRST + 115];
       void  SetVal() = [ID_FIRST + 118];
       void  FindSN() = [ID_FIRST + 120];
       void  ChDNumber(RTMessage) = [ID_FIRST + 123];
       void  FindPrSN() = [ID_FIRST + 121];
       void  FindName() = [ID_FIRST + 119];
       void  ChAgent(RTMessage msg) = [ID_FIRST + 205];
       void  ChPeriod(RTMessage msg) = [ID_FIRST + 200];
       void  ChAutoType(RTMessage msg) = [ID_FIRST + 201];
       void  ChCountry(RTMessage msg) = [ID_FIRST + 202];
       void  ChStartDate(RTMessage msg) = [ID_FIRST + 104];
       void  FindAutoNumber(RTMessage msg) = [ID_FIRST + 127];
       void  Clear();
       void  Print1();
       void  Default();
       char* GetCurrency(char* TarifTableName);
       virtual void  Help(RTMessage) = [WM_SYSCOMMAND];

       void  CalcEndDate();
       void  CalcTarif();
};
*/
#endif