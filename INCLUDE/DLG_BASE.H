#ifndef _DLG_BASE__H
#define _DLG_BASE__H

#include <edit.h>
#include <pxtable.h>
#include <openbase.h>

class UrFizich : public TEdit {
public:
	UrFizich(PTWindowsObject AParent, int ResourceId, WORD ATextLen) :
	TEdit(AParent, ResourceId, ATextLen) {}
	void WMRButtonDown(RTMessage) = [WM_RBUTTONDOWN];
	void WMLButtonDown(RTMessage) = [WM_LBUTTONDOWN];
	void ChText();
};

extern char* ININame;

enum { MANDATOR_TBL, UNIVERSAL_TBL, POLAND_TBL, LEBEN2_TBL, BULSTRAD_TBL, SOVAG_TBL, LITVA_TBL, BELGREEN_TBL, RUSSIAN_TBL };

class Dlg_base : public TDialog
{
protected:
     TOpenBase* base;
     char AgPercentBuffer[32];

public:
             Dlg_base(PTWindowsObject, char*, char*, char**, int);
             ~Dlg_base();

	   TOpenBase* getBase() { return base; }

       void  SetupWindow();

       virtual int   CanDeleteRecord() { return TRUE; }
       virtual void  GetDataFromBase();
       virtual int   SetDataToBase();
       virtual void  SetButtons();
       virtual int   Update() { return TRUE; }
       virtual int   Insert() { return TRUE; }
       virtual int   AfterSave() { return TRUE; }

	   virtual char* GetAgPercent(long Code, int Flag, int&);

       virtual int   SavePolis();
       virtual void* GetBuffer() = 0;
       virtual void  First() = [ID_FIRST + 401];
       virtual void  Last() = [ID_FIRST + 402];
       virtual void  Prev() = [ID_FIRST + 403];
       virtual void  Next() = [ID_FIRST + 404];
       virtual void  Clear() = [ID_FIRST + 405];
       virtual void  Save() = [ID_FIRST + 406];
       virtual void  Delete() = [ID_FIRST + 407];
       virtual void  Print1() = [ID_FIRST + 408];
       virtual void  Print2() = [ID_FIRST + 409];

       virtual void  Help(RTMessage) = [WM_SYSCOMMAND];

       virtual int   BeforeDelete() {}
       virtual void  AfterDelete() {}

       virtual char* getMenuStr();

	   static ShowArray(Array& list_errors);
	   static  char Agent_Code[10];
	   static  char Agent_Name[70];
};

extern int AgFieldsCount;
extern char* AgentFields[14];
extern char* AgentDBName;

#endif