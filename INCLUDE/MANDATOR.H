#ifndef _MANDATORY__H
#define _MANDATORY__H

#include <edit.h>
#include <pxtable.h>
#include <openbase.h>
#include <dlg_base.h>
#include <CHECKBOX.H>
#include <RADIOBUT.H>
#include "mand_av2.h"

#pragma error

#define FIELDTYPE_CHAR   0
#define FIELDTYPE_DATE   1
#define FIELDTYPE_NUMBER 2

#define STATE_POLIS_NORMAL 0
#define STATE_POLIS_BAD    1
#define STATE_POLIS_ENDED  2
#define STATE_POLIS_LOST   3
#define STATE_POLIS_STOPPAY   5

struct MandatoryPrivate_data
{
char Number[12 + 1];
char RegDate[11];
char PrevNumber[12 + 1];
char Name[70 + 1];
char Address[65 + 1];
char FromDate[11];
char FromTime[5 + 1];
char ToDate[11];
char Letter[3 + 1];
char Marka[30 + 1];
char AutoNumber[10 + 1]; //N10
char Volume[11];
char Tonnage[11];
char Quantity[11];
char Power[11];
char NumberBody[21];
char Chassis[21];
char Tarif[11];
char Rate[11];
char Discount[3];
char AllFee[11];            //N20
char AllFeeText[129];
char Fee1[11];
char Fee1Text[129];
char Fee1Date[11];
char DraftNumber[11];
char Fee2[11];
char Fee2Text[16 + 1];
char Fee2Date[11];
char InsComp[96 + 1];      //N29
char Pay2RealDate[11];
char Pay2Rate[11];
char Pay2RubSumma[15];
char Pay2DraftNumber[11];
int  OwnPolis;
int  Nal;
int  BezNal;
int  Karta;
int  IsFee2;
//int  _1SU;
int  Nal_2;
int  BezNal_2;
int  IsPay2;
int  ToReport;

//char K[5];
char TarifGlass[11];
char K1[11];
char _CountAv[3];
char _CurrClassAv[3];

char StopDate[11];
char RetDate[11];
char MobilePhone[30];
char HomePhone[30];
char Contact[30];
int  SMS;
int  Fizich_;
int  Uridich;
int  IP;
char CityCode[4];

char RetBRB[12];
char RetEUR[12];
char AgPercent1[6];
char AgPercent2[6];

char RetDate1[11];
char RetDate2[11];


char IdNumber[30 + 1];
char NameOwn[70 + 1];
char AddressOwn[65 + 1];
char BuildYear[5];



/////////// TAXI
   char TaxiSumma[10];
   char TaxiPayDate[11];
   char TaxiPayDoc[12];

   int Taxi1SU;
   int TaxiIsNal;
   //int _Taxi;
   //int _AutoType;
   //int _Avar;
   //int _Other;
   //char Curr[4];
   int TaxiInOut;
   int TaxiVid;
///////////



int _OwnerCode_;
int _BaseCarCode_;
int _CarCode_;

char Seria[4 + 1];
char PrevSeria[4 + 1];


   char SuperBuffer[32];
};

int aux_save_fields_to_file2(TOpenBase* base, FILE* f, char* Op, int Komplex);
void CopyFieldValue(TOpenBase& From, FILE* f, int field);

#define MANDATORY_ED_COUNT 36
#define COUNTAVARIAS       15
#define COUNTOWNERS	   15
/*
struct OwnersData {
   char Name[50 + 1];
   char RegDate[11];
   char Summa[15];
   char PayDocNumber[11];
   char PayDate[11];
   int  Fizich;
   int  Uridich;
   char Address[51];
   char PravoTxt[31];
   char WhenPrava[5];
   char CityCode[5];
   char Placement[32 + 1];
   int  Pravo;

   double K;

   long OwnerCode;

   char SuperBuffer[32];
   int  IsSaved;  //Flag coхраняли или нет данные (касается добавления владельцев потом)
};

class MandatoryDlgOwners: public TDialog
{
public:
   OwnersData* data;
   TCheckBox* m_Fizich;
   TCheckBox* m_Uridich;
   PTComboBox PlacementCombo;
   PTComboBox PravoCombo;

   DATE MinDate, MaxDate;

   double PlaceK_Form;

public:
   MandatoryDlgOwners(PTWindowsObject, OwnersData* buffer, DATE, DATE);
   void  SetupWindow();
   void  Ok(RTMessage);
   virtual void TransferData(WORD Direction);
   void  Placement(RTMessage) = [ID_FIRST + 103];

   virtual void DefWndProc(RTMessage Msg);
   void  ChCityCode();

   void  Fizich(RTMessage) = [ID_FIRST + 1001];
   void  Uridich(RTMessage) = [ID_FIRST + 1002];
};
*/

struct taxi_data {
   char Summa[10];
   char PayDate[11];
   char PayDoc[12];
   int _1SU;
   int _IsNal;
   int _Taxi;
   int _AutoType;
   int _Avar;
   int _Other;
   char Curr[4];
   int _PayVid;
};

class MandatoryDlg: public Dlg_base
{
	 char ININame[128];
	 int           Version;
	 int 	   	   m_State;
	 int Komplex;

//     void          SetVersion(int V);
     void          SetState(int V);

     MandatoryPrivate_data  buffer;
     PTEdit        e[MANDATORY_ED_COUNT + 5];

     PTEdit        m_UrFizich1;
	 PTEdit        m_UrFizich2;

	 Avaria2Data   avarias2[COUNTAVARIAS + 3];
	 AVPayData	   m_AVPayData[60];

     int	   	   avariaCount;
//     OwnersData    autoowners[COUNTOWNERS + 3];
//     int	  	   ownersCount;

     //Part2
     PTComboBox    m_Placement;
	 PTEdit        m_TarifGlass;
     PTEdit        m_K1;
	 PTComboBox    _m_OldClassAv;
     PTEdit        _m_CountAv;
	 PTEdit        _m_CurrClassAv;
     PTEdit        m_StopDate;
	 PTEdit        m_RetDate;
	 PTEdit        m_CityEdit;
     PTEdit        m_RetBRB;
     PTEdit        m_RetEUR;
	 PTEdit        m_MobilePhone;
     PTEdit        m_HomePhone;
	 PTEdit        m_Contact;
	 PTCheckBox    m_SMS;

	 PTComboBox    m_K3;
	 PTEdit        m_AgPercent1;
	 PTEdit        m_AgPercent2;
	 PTComboBox    m_Osnovanie;
	 TRadioButton* m_Fizich;
	 TRadioButton* m_Uridich;
	 TRadioButton* m_IP;
	 PTComboBox	   m_1SU;
	 PTCheckBox    m_OwnPolis;
	 PTCheckBox    m_ToReport;

//     PTComboBox    m_VersionCombo;
     //Part2 END

	 TOpenBase     TaxiDb;
	 TOpenBase     agents;
	 TOpenBase     avaria2;
//     TOpenBase     owners_;

     PTComboBox    AgName1;
	 PTComboBox    AgName2;
	 PTComboBox    Period;
     PTComboBox    Super;
     TCheckBox*    Nal;
     TCheckBox*    BezNal;
	 TCheckBox*    Karta;
	 TCheckBox*    IsFee2;

     TCheckBox*    Nal_2;
     TCheckBox*    BezNal_2;
	 TCheckBox*    IsPay2;

	 TEdit*        m_RetDate1;
	 TEdit*        m_RetDate2;

	 TEdit*        _m_IdNumber;
	 TEdit*        _m_NameOwn;
	 TEdit*        _m_AddrOwn;
	 TEdit*  _m_BuildYear;

	 TComboBox*    m_Resident;
     Array         m_TableKeyNames;
     Array         m_TarifKeyNames;
     Array         m_CheckAuto;
     Array         m_CheckShassi;
     Array         m_CheckNumberBody;

     PTListBox     ListBAvaria;
     PTListBox     ListBOwners;
     PTListBox     ListBPhoto;

     PTComboBox    Pay1Curr;
     PTComboBox    Pay2Curr;

	 PTComboBox    m_Vzamen;

	 ///////////TAXI
	TComboBox* m_TaxiPayCurr;
	TComboBox* m_TaxiInOut;
	TComboBox* m_TaxiVid;
    PTEdit m_TaxiSumma;
	PTEdit m_TaxiPayDate;
	PTEdit m_TaxiDoc;
	PTCheckBox m_Taxi1SU;
	PTCheckBox m_TaxiNal;
	 //////////////


     int           IsOpenPart2; //0 close 1 - avaria 2 = owners 3 - foto -1 - temporary

     Array         m_LastFind;

public:

	 static char*         Rubl;

		 MandatoryDlg(PTWindowsObject, int = 0);
	     ~MandatoryDlg();
       virtual char* getMenuStr();

	   static long initPosition;

	   void InitTime(TEdit* e);
	   void PrintData(char*& dest, int Flag, int PrintVersion);
//	   void AddOwnerToBuffer(char*& dest, int Index, int Version);
       char* GetSeria();
       double GetNumber();
       static void Restore(int);
//	   static void Restore2(int);
       static void Export(int);
	   static void Import(int);

	     int StartWidth;

       void  ShowPolisAvInfo();
       void  FillSuperCombo(int, int Notify);
	   void  InitDates();

	void ClearPhotoList();
		void LoadImagesList(const char* Seria, long Number);
	   void  SetupWindow();

       void  GetDataFromBase();
        int  SetDataToBase();
       void* GetBuffer() { return &buffer; }

       void  MeasureItem(RTMessage) = [WM_MEASUREITEM];
       void  WMDrawItem(RTMessage) = [WM_DRAWITEM];
	   void  GetDublicat(RTMessage) = [ID_FIRST + 65];
	   void  RefreshPhotos(RTMessage) = [WM_USER + 1];

       virtual int   CanDeleteRecord();

       virtual int   BeforeDelete();

	   void CMOwnPolis(RTMessage Msg) = [ID_FIRST + 75];
	   void Taxi(RTMessage Msg) = [ID_FIRST + 504];
	   void  ChAgent1(RTMessage) = [ID_FIRST + 200];
	   void  ChAgent2(RTMessage) = [ID_FIRST + 71];
	   void  ChangeRegDate(RTMessage) = [ID_FIRST + 52];
	   void  ChangePay2Date(RTMessage) = [ID_FIRST + 127];
       void  SetCurrency() = [ID_FIRST + 60];
       void  SetCurrencyForPay(int PayNmb);
       void  Find() = [ID_FIRST + 400];
       void  FindSNCmd(RTMessage) = [ID_FIRST + 2512];
       void  FindNumberAutoCmd(RTMessage) = [ID_FIRST + 2513];
	   void  FindMobTel(RTMessage) = [ID_FIRST + 81];
	   void  FindHomeTel(RTMessage) = [ID_FIRST + 82];
	   void  BN(RTMessage) = [ID_FIRST + 2521];
	   void  FindOwnerPart(RTMessage) = [ID_FIRST + 59];
	   void  K3Change(RTMessage) = [ID_FIRST + 77];
	   void  PrintViewAct(RTMessage) = [ID_FIRST + 95];
	   void  Clear();
       void  Print1();
       void  Print2();
       void  Default();
	   void  Help(RTMessage) = [WM_SYSCOMMAND];
       double GetK3();
	   void  CityEditNotify(RTMessage) = [NM_CITYEDIT];

	   void  CopyToAgent2(RTMessage) = [ID_FIRST + 74];
	   void  Zajava(RTMessage) = [ID_FIRST + 4081];
	   void  Fizich(RTMessage) = [ID_FIRST + 1001];
	   void  Uridich(RTMessage) = [ID_FIRST + 1002];
	   void  IP(RTMessage) = [ID_FIRST + 1003];

       //void  SetKNBody(RTMessage) = [ID_FIRST + 55];
       //void  SetKShassi(RTMessage) = [ID_FIRST + 56];
       //void  SetKToEdit(TEdit*);

       void  ClearAvarias(int*);
//       void  ClearOwners(int*);
       void  ChSer(RTMessage) = [ID_FIRST + 50];
       void  ChNmb(RTMessage) = [ID_FIRST + 51];
       void  StopPay(RTMessage) = [ID_FIRST + 58];

       void  ChangePay1Date(RTMessage) = [ID_FIRST + 121];

//       void  ChangeVersion(RTMessage) = [ID_FIRST + 1437];

       void  ChangePay2Rate(RTMessage) = [ID_FIRST + 128];

       void  Nal_2Check(RTMessage) = [ID_FIRST + 206];
       void  BezNal_2Check(RTMessage) = [ID_FIRST + 207];

       void  ChCurr1(RTMessage) = [ID_FIRST + 301];
       void  ChCurr2(RTMessage) = [ID_FIRST + 302];
       void  OKRGL(RTMessage) = [ID_FIRST + 971];
	   void  OKRGL2(RTMessage) = [ID_FIRST + 68];

       void  NalFunc(RTMessage) = [ID_FIRST + 201];
       void  BezNalFunc(RTMessage) = [ID_FIRST + 202];
	   void  KartaFunc(RTMessage) = [ID_FIRST + 85];
	   void  IsFee2Check(RTMessage) = [ID_FIRST + 203];
       void  IsPay2Check(RTMessage) = [ID_FIRST + 205];

	   void  Print1CY_Pay2(RTMessage) = [ID_FIRST + 56];
	   void  Print1CY_AddPay(RTMessage) = [ID_FIRST + 78];
	   void  Print1CY(char* PayRealDate, char* PayRubSumma, char* from, char* to, char* what);
	    int  FillTaxiData(taxi_data& data);

	   void  ChangeResident(RTMessage) = [ID_FIRST + 208];
       void  Change(RTMessage) = [ID_FIRST + 420];
	   void  Continue(RTMessage) = [ID_FIRST + 498];
	   void  ChangePeriod(RTMessage) = [ID_FIRST + 300];
       void  ChangeSuper(RTMessage) = [ID_FIRST + 1000];
       void  ChangeStartDate(RTMessage) = [ID_FIRST + 102];
       void  ChangePay1(RTMessage) = [ID_FIRST + 119];

       void  ChangeSumma1(RTMessage) = [ID_FIRST + 115];
       void  ChangeSumma2(RTMessage) = [ID_FIRST + 116];
       void  IsAvaria(RTMessage) = [ID_FIRST + 450];
//       void  IsOwners(RTMessage) = [ID_FIRST + 451];
       void  IsPhoto(RTMessage) = [ID_FIRST + 86];
       void  OpenClose();

       void  StopData(RTMessage) = [ID_FIRST + 998];

       void  AddAvaria(RTMessage) = [ID_FIRST + 460];
       void  EditAvaria(RTMessage) = [ID_FIRST + 462];
       void  DeleteAvaria(RTMessage) = [ID_FIRST + 461];
	   void  ChangeK2(RTMessage) = [ID_FIRST + 994];
	   void  ChangeInOut(RTMessage) = [ID_FIRST + 88];

       void  Calculate() = [WM_FIRST + 1];
       void  CalculateSumma();
       void  CalculatePeriod();
	//   void  DeleteFromAux(char* Seria, double Number);
	//int  DeleteOwner(OwnersData data);
	//int  SaveOwner(OwnersData data);
	int  DeleteAvaria2(Avaria2Data data);
	int  SaveAvaria2(Avaria2Data data);

	   void  VzamenFunc(RTMessage) = [ID_FIRST + 209];
       void  VzamenFuncX();

       void  Placement(RTMessage) = [ID_FIRST + 990];

	   void  ChangeCountAv(RTMessage) = [ID_FIRST + 995];

	   void  WMCommand(RTMessage msg);
	   void  WMMove(RTMessage msg) = [WM_FIRST + WM_MOVE];
       virtual void DefWndProc(RTMessage Msg);
       virtual void DispatchAMessage(WORD AMsg, RTMessage AMessage, void (TWindowsObject::* _FAR) (RTMessage));
	int  FindLike(char* TableName, char* FldName, char* Maska, int Collect, int IsDigit = 0, char** CheckFields = NULL);
    int  FindPlat(int Part);
	int  FindAv(int Flag);
	int  FindSN(int);
	int  FindTransport();
	int  FindTransportLike();
	int  FindOwner(int = -1);
	int  CheckAvDups();
	int  FindRepeat();

       virtual int   AfterSave();
       virtual void  AfterDelete();
//	   void _SavePolisToFile(char*, char* S = 0, double N = -1);

       int FindOnServer(char* FldName, char* Maska, Array& Strings);

	   virtual void  WMCtlColor(RTMessage) = [WM_FIRST + WM_CTLCOLOR];
};

#endif