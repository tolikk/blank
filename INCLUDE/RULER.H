#ifndef _RULER__H
#define _RULER__H

// TRulerWin
#include <printer.h>
#include <array.h>
#include <ids.h>
#include <o.h>

  _CLASSDEF(TRulerWin)
  class TRulerWin : public TWindow
  {
		PTPrinter m_Printer;    // Pointer to a TPrinter object
                Array     array;
                int       press;
                POINT     lu, rd;
		HBITMAP   m_bmp;
                char      BlankName[150];
                int       BmpDX, BmpDY;
				O*        prtO;
				char 	  CurrentBlkFile[128];

  public:
                TRulerWin(PTWindowsObject AParent, LPSTR ATitle, PTModule AModule = NULL);
                ~TRulerWin( void );

            void SetupWindow();

		void SysCommand(RTMessage Msg) = [WM_SYSCOMMAND];
		void CMDelete(RTMessage Msg) = [CM_FIRST + 500];
        void CMParam(RTMessage Msg) = [CM_FIRST + 502];
	    void CMFonts(RTMessage Msg) = [CM_FIRST + 501];
	    void CMCross(RTMessage Msg) = [CM_FIRST + 504];
		void BlankIni(RTMessage Msg) = [CM_FIRST + 146];
		void CMSimpleText(RTMessage Msg) = [CM_FIRST + 505];
	    void CMNoPrint(RTMessage Msg) = [CM_FIRST + 503];
	    void OffsetBlank(RTMessage Msg) = [CM_FIRST + 110];

	    void ExportMand(RTMessage Msg) = [CM_FIRST + 153];
		void ImportMand(RTMessage Msg) = [CM_FIRST + 154];

		void ExportKomplex(RTMessage Msg) = [CM_FIRST + 152];
		void ImportKomplex(RTMessage Msg) = [CM_FIRST + 155];

		void ReportGreen(RTMessage Msg) = [CM_FIRST + 130];
	    void ReportKomplex(RTMessage Msg) = [CM_FIRST + 156];
//		void MandNewCalc(RTMessage Msg) = [CM_FIRST + 137];
//		void MandOldPrint(RTMessage) = [CM_FIRST + 143];

		void ExportBelGreen(RTMessage) = [CM_FIRST + 114];
		void ImportBelGreen(RTMessage Msg) = [CM_FIRST + 132];

//		void ExportRuss(RTMessage) = [CM_FIRST + 135];
  //		void ImportRuss(RTMessage Msg) = [CM_FIRST + 136];

//		void PrintLeben2006(RTMessage) = [CM_FIRST + 184];
		void Rates(RTMessage Msg) = [CM_FIRST + 121];

		void ExportPrivate2005(RTMessage) = [CM_FIRST + 168];
		void ImportPrivate2005(RTMessage Msg) = [CM_FIRST + 171];

		void TOOL(RTMessage Msg) = [CM_FIRST + 160];
		void RebuildIndexMand(RTMessage Msg) = [CM_FIRST + 133];
//	    void BulstradRpt(RTMessage) = [CM_FIRST + 156];
		void FIZFACE(RTMessage) = [CM_FIRST + 117];
		void URFACE(RTMessage) = [CM_FIRST + 118];
		void PrintAddr(RTMessage) = [CM_FIRST + 144];
		//void ExportAllNmbrs(RTMessage) = [CM_FIRST + 235];

//	    void Russian(RTMessage Msg) = [CM_FIRST + 162];
		void ImportAlvena(RTMessage Msg) = [CM_FIRST + 161];
		void ReportUnivers(RTMessage Msg) = [CM_FIRST + 165];
		void StartAutoMarka(RTMessage Msg) = [CM_FIRST + 116];

	    void ReportMandatory(RTMessage Msg) = [CM_FIRST + 139];
//		void ReportSovag(RTMessage Msg) = [CM_FIRST + 158];
//	    void Litva(RTMessage Msg) = [CM_FIRST + 159];
		void Agents(RTMessage Msg) = [CM_FIRST + 119];
		void RestoreMandator(RTMessage Msg) = [CM_FIRST + 127];
	    void RestoreMandatorFromBk(RTMessage Msg) = [CM_FIRST + 128];
//	    void RunReporter(char* What);
	    void RunReporter32(char* What);
		//int Executor(char* Module, char* What, int AddLockalPath = 1);

			void About(RTMessage) = [CM_FIRST + 200];
			void Zoom() = [CM_FIRST + 300];
			void Left() = [CM_FIRST + 304];
            void Right() = [CM_FIRST + 302];
            void Up() = [CM_FIRST + 301];
            void Down() = [CM_FIRST + 303];
            void WMRButtonDown(RTMessage Msg) = [WM_FIRST + WM_RBUTTONDOWN];
            void WMLButtonDown(RTMessage Msg) = [WM_FIRST + WM_LBUTTONDOWN];
            void WMLButtonUp(RTMessage Msg) = [WM_FIRST + WM_LBUTTONUP];
            void WMMouseMove(RTMessage Msg) = [WM_FIRST + WM_MOUSEMOVE];
            void WMMinMax(RTMessage Msg) = [WM_FIRST + WM_GETMINMAXINFO];
			void LoadLastFile(char Ltr);

	virtual void CMFilePrint(RTMessage Msg)  = [CM_FIRST + CM_FILEPRINT];
    virtual void CMFilePrinterSetup(RTMessage Msg)  = [CM_FIRST + CM_FILEPRINTERSETUP];
    virtual void CMFileExit(RTMessage Msg)  = [CM_FIRST + CM_FILEEXIT];
            void CMRange(RTMessage Msg)  = [CM_FIRST + 122];

		void WMSetXRange(RTMessage Msg) = [CM_FIRST + 123];
	virtual void Paint(HDC PaintDC, PAINTSTRUCT& PaintInfo);
            void GetWindowClass(WNDCLASS & AWndClass);

            void Caption(LPSTR);
            void Open(RTMessage) = [CM_FIRST + 104];
            void SaveAs(RTMessage) = [CM_FIRST + 105];
            void Save(RTMessage) = [CM_FIRST + 107];
	    void NewFile(RTMessage) = [CM_FIRST + 106];
	    void EndFind(RTMessage) = [WM_USER + 274];
	    void Read(LPSTR name);
            void Save(LPSTR name);
			void LoadBlank(LPSTR name);
			void Printing(HDC DC);
            void OpenBmp(RTMessage) = [CM_FIRST + 108];
            void PageSetup(RTMessage) = [CM_FIRST + 109];
	    void AgentSetup(RTMessage) = [CM_FIRST + 113];
		void Settings(RTMessage) = [CM_FIRST + 115];
	    void AgentSetup1(RTMessage msg) = [CM_FIRST + 2] { AgentSetup(msg); }
	    void GreenCard(RTMessage) = [CM_FIRST + 120];
	    void RepRuss(RTMessage) = [CM_FIRST + 163];
			void Strax(RTMessage) = [CM_FIRST + 164];
			//void Private(RTMessage) = [CM_FIRST + 123];
	    void MandatoryInsur(RTMessage) = [CM_FIRST + 124];
	    void Komplex(RTMessage) = [CM_FIRST + 169];
		//void Private2(RTMessage) = [CM_FIRST + 125];
		void Private2005(RTMessage) = [CM_FIRST + 166];
//		void BulStrad(RTMessage) = [CM_FIRST + 155];

		void ExportUnivers(RTMessage) = [CM_FIRST + 134];
		void ImportUnivers(RTMessage) = [CM_FIRST + 138];
	    //void ImportStrax(RTMessage) = [CM_FIRST + 136];
//	    void ExportLocal(RTMessage) = [CM_FIRST + 137];
		//void ExportLebenToOwner(RTMessage) = [CM_FIRST + 138];
		//void Leben2(RTMessage) = [CM_FIRST + 140];
		void Leben2005(RTMessage) = [CM_FIRST + 167];
		//void ForeignRpt(RTMessage) = [CM_FIRST + 152];
//	    void Poland(RTMessage) = [CM_FIRST + 141];
	    //void Foreign(RTMessage) = [CM_FIRST + 142];
	    //void SOVAG(RTMessage) = [CM_FIRST + 157];

            void Help(RTMessage) = [CM_FIRST + 210];
            void Clear();
            void Numbers();
	    void SetValues(char* buff);

		int Is2000Full;
		int Language;
	    int MonthRegister; //0 lower 1 upper 2 as is
		//char TextTo2RP[130];

	    HWND msg_reciever;
  };

#endif


