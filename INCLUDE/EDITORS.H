#ifndef __EDITORS_H
#define __EDITORS_H

#include "windows.h"

struct MenuCashe {
	char maska1[32];
	HMENU menu;
};

class INIEdit : public TEdit
{
    char Maska1 [32];
    char Maska2 [32];
	int	 IsAdd;
	char* _ININame;
	char* Section;

	public:
		
	void SetININame(const char*);
	void SetSectionName(const char*);
	void SetMask(const char* s) { strcpy(Maska1, s); }
	void SetSubMask(const char* s) { strcpy(Maska2, s); }

	public:
		  INIEdit(const char* Ini, const char* Sect, PTWindowsObject AParent, int ResourceId, WORD ATextLen, char* m1, char* m2, int Add = 0) ;
		  ~INIEdit();

	     void WMRButtonDown(RTMessage) = [WM_FIRST + WM_RBUTTONDOWN];

	 void    WMCommand(RTMessage) = [WM_COMMAND];
	 void    Text(char* str);

	   virtual void DefWndProc(RTMessage Msg);
	char DivString[16];

    static MenuCashe* Cash;
};

class UPSTREdit : public INIEdit
{
    public:
		  UPSTREdit(const char* Ini, const char* Sect, PTWindowsObject AParent, int ResourceId, WORD ATextLen, char* m1 = 0, char* m2 = 0, int Add = 0) :
		  INIEdit(Ini, Sect, AParent, ResourceId, ATextLen, m1, m2, Add) {}

	     void WMChar(RTMessage) = [WM_CHAR];
};

class NumberEdit : public INIEdit {
public:
	int CanPoint;
	int CanMinus;
	NumberEdit(PTWindowsObject, int Id, int Len, int IsPoint, int IsMunus = 0);
	NumberEdit(PTWindowsObject, int Id, int Len, int IsPoint, char* mask);
	void WMChar(RTMessage) = [WM_FIRST + WM_CHAR];
};

#endif