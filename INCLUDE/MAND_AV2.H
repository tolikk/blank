#ifndef __MANDAV2_H
#define __MANDAV2_H

#include "owl.h"
#include "checkbox.h"

#define NM_CITYEDIT (WM_USER + 243)

class CityEdit : public TEdit
{
	char ININame[128];
    public:
		  CityEdit(PTWindowsObject AParent, int ResourceId, WORD ATextLen, const char* ININame) :
		  TEdit(AParent, ResourceId, ATextLen) {
		  strcpy(this->ININame, ININame);
		  }

	     void WMRButtonDown(RTMessage) = [WM_FIRST + WM_RBUTTONDOWN];
	     void WMLButtonDown(RTMessage) = [WM_FIRST + WM_LBUTTONDOWN];

	 void    WMCommand(RTMessage) = [WM_COMMAND];
};

struct AVPayData /*: public Object*/ {
	AVPayData() { memset(this, 0, sizeof (AVPayData)); Type1 = 1; Risk1 = 1; Rcvr1 = 1; SubType1 = 1; }
    int Type1;
	int Type2;

	int Risk1;
	int Risk2;
	int Risk3;

	int Rcvr1;
	int Rcvr2;

	int SubType1;
	int SubType2;

	char Name[33];

	char Date[11];
	char Sum[11];
	char Doc[11];
	int Nal;
	char Curr[4];
	int Zatr;



	int _Flag;

/*	virtual hashValueType hashValue() const { return 0; };
    virtual classType isA() const { return 0; }
    virtual int isEqual(RCObject) const { return 0; };
    virtual Pchar nameOf() const { return ""; }
	virtual void printOn(Rostream) const {}*/
	char* GetStr(char*);
};

struct Avaria2Data {
   char N[3];
   char UpdateDate[11];
   char AVDate[11];
   char AVPlace[51];
   char WorkNmb[15];
   char WrDate[11];

   char SN[15];
   char FIO[51];
   char FIOAddress[51];

   char Staj[5];
   char CityCode[4];
   char BMCityCode[4];

   char TC_1[10];
   char TC_1V[16];
   char TC_2_1[10];
   char TC_2_1V[16];
   char TC_2_1VDate[11];

   char HL_1[10];
   char HL_1V[16];
   char HL_2_1[10];
   char HL_2_1V[16];
   char HL_2_1VDate[11];

   char IM_1[10];
   char IM_1V[16];
   char IM_2_1[10];
   char IM_2_1V[16];
   char IM_2_1VDate[11];

   char Marka[31];
   char BaseType[3];
   char AutoCharact[11];
   char AutoNmb[11];
   char BodyNo[21];

   char DeclTC[15];
   char DeclHL[15];
   char DeclIM[15];
   char LossCode[6];
   char DtFreeRz[11];
   char DtClLoss[11];
   char SumPay[15];

   char OcenTC[15];
   char OcenHL[15];
   char OcenIm[15];
   char OthTC[15];
   char OthHL[15];
   char OthIM[15];
   int IsTC1;
   int IsTC2;
   int IsHL1;
   int IsHL2;
   int IsIM1;
   int IsIM2;

   //End Transfer Section

   //char TC_1_CUR[4];
   char VALUTA[4];
   char TC_2_CUR[4];
   char HL_1_CUR[4];
   char HL_2_CUR[4];
   char IM_1_CUR[4];
   char IM_2_CUR[4];

   char BadMan[51];
   int  Decision;
   int  AVDocument;
   int  VTR;

   int  IsBMFizich;
   int  IsBMResident;
   int  BMOsnovan;
   double  BMDiscount;

//   Auto_P data_P;

   long BaseCarCode;
   long CarCode;


///
   long OwnerCode;
   long PaymentCode;

   char SuperBuffer[32];
};

class MandatoryDlgAvaria2: public TDialog
{
   const char* ININame;
public:
   Avaria2Data* data;
   POINT ru;

   DATE MinDate, MaxDate;
   AVPayData* m_PayData;
   int m_PayDataSize;

   double AvNumber;
   char AvSeria[5];

public:
   MandatoryDlgAvaria2(PTWindowsObject, Avaria2Data* buffer, char*, double, AVPayData* PayData, const char* ININame);
   void  SetupWindow();
   void  Ok(RTMessage);
   virtual void TransferData(WORD Direction);

   Array  m_FIOList;

   PTEdit m_CityCode;
   PTComboBox m_Decision;
   PTComboBox m_DocType;
   PTComboBox m_BadMan;
   PTComboBox m_VTR;
   PTComboBox m_VALUTA;
/*   PTComboBox m_Val1_1;
   PTComboBox m_Val2;
   PTComboBox m_Val2_1;
   PTComboBox m_Val3;
   PTComboBox m_Val3_1;*/
	PTEdit Zajavl1;
	PTEdit Zajavl2;
	PTEdit Zajavl3;


   void  CityEditNotify(RTMessage) = [NM_CITYEDIT];
   void    WMCommand(RTMessage) = [WM_COMMAND];

   PTComboBox m_BMFizichUridichCombo;
   PTComboBox m_BMIsResidentCombo;
   PTEdit     m_BMCityCode;
   PTComboBox m_BMOsnovan;
   PTComboBox m_BMDiscountPlace;
   PTListBox  m_PayList;
//   PTCheckBox m_IsAutoP;

   //PTCheckBox m_Uridich;
   //void  Fizich(RTMessage) = [ID_FIRST + 109];
   void  OnChangeValuta(RTMessage) = [ID_FIRST + 173];
   void  SetValutaStr();

   void  OnAddPay(RTMessage) = [ID_FIRST + 180];
   void  OnDelPay(RTMessage) = [ID_FIRST + 179];
   void  OnEditPay(RTMessage) = [ID_FIRST + 178];

   PTEdit     m_N;
   PTEdit     m_UpdateDate;
   PTEdit     m_AvDate;
   PTEdit     m_AVPlace;
   PTEdit     m_WorkNmb;
   PTEdit     m_WriteDate;
   PTEdit     m_SN;
   PTEdit     m_FIO;
   PTEdit     m_FIOAddress;
   PTEdit     m_Staj;
   PTEdit     m_TC_S_FAC_EUR;
   PTEdit     m_TC_S_FAC_VAL;
   PTEdit     m_TC_S_VOZ_EUR;
   PTEdit     m_TC_S_VOZ_VAL;
   PTEdit     m_TC_DATE_VOZ;
   PTEdit     m_HL_S_FAC_EUR;
   PTEdit     m_HL_S_FAC_VAL;
   PTEdit     m_HL_S_VOZ_EUR;
   PTEdit     m_HL_S_VOZ_VAL;
   PTEdit     m_HL_DATE_VOZ;
   PTEdit     m_IM_S_FAC_EUR;
   PTEdit     m_IM_S_FAC_VAL;
   PTEdit     m_IM_S_VOZ_EUR;
   PTEdit     m_IM_S_VOZ_VAL;
   PTEdit     m_IM_DATE_VOZ;
};


#endif