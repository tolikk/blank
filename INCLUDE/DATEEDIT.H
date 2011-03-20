#include <owl.h>
#include <static.h>
#include <dialog.h>
#include <string.h>
#include <edit.h>
#include <stdio.h>

class TDateEdit : public TEdit
{
public:
	TDateEdit(PTWindowsObject AParent, int ResourceId, WORD ATextLen) : TEdit(AParent, ResourceId, ATextLen)
	{
	}

	int GetParts(int& pos1, int& pos2);

	void WMLButtonDown(RTMessage msg) = [WM_LBUTTONDOWN];

	void WMKeyDown(RTMessage msg) = [WM_KEYDOWN];

	int IsDivider(WORD Param);

	void WMChar(RTMessage msg) = [WM_CHAR];
	void Normalize(char* buff);
	void WMKillFocus(RTMessage msg) = [WM_KILLFOCUS];
	void WMSetFocus(RTMessage msg) = [WM_SETFOCUS];
	int isDigit(char c);
};

class TTimeEdit : public TEdit
{
public:
	TTimeEdit(PTWindowsObject AParent, int ResourceId, WORD ATextLen) : TEdit(AParent, ResourceId, ATextLen)
	{
	}

	void WMChar(RTMessage msg) = [WM_CHAR];
	void WMKillFocus(RTMessage msg) = [WM_KILLFOCUS];
};
