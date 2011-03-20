#include <owl.h>
#include <static.h>
#include <dialog.h>
#include <string.h>
#include <edit.h>
#include <stdio.h>
#include <array.h>
#include <window.h>
#pragma hdrfile
#include <pxengine.h>
#include <owl.h>
#include <dialog.h>
#include <edit.h>
#include <pxengine.h>
#include <pxtable.h>
#include <openbase.h>
#include <array.h>
#include <grncard.h>
#include <stdio.h>
#include <string.h>
#include <ruler.h>
#include <window.h>
#include <combobox.h>
#include <commdlg.h>
#include <mandator.h>
#include <checkbox.h>
#include <thwindow.h>
#include <stdlib.h>
#include <help.h>
#include "util.h"
#include "math.h"
#include "GRNCARD.h"
#include "io.h"
#include "sortarry.h"
#include "editors.h"

#define max(a, b) ((a) > (b) ? (a) : (b))

#undef NDEBUG
#include "assert.h"

extern const char* MANDATORY_SECT;

INIEdit::INIEdit(const char* Ini, const char* Sect, PTWindowsObject AParent, int ResourceId, WORD ATextLen, char* m1, char* m2, int Add)
	   : TEdit(AParent, ResourceId, ATextLen)
{
    if(m1) strcpy(Maska1, m1);
    else Maska1[0] = 0;
    if(m2) strcpy(Maska2, m2);
    else Maska2[0] = 0;
    IsAdd = Add;
    strcpy(DivString, "");
	_ININame = strdup(Ini);
	Section = strdup(Sect);
}

void INIEdit::Text(char* str)
{
     DynStr buffer;
	 GetText(buffer, buffer._sizeof());
	 if(IsAdd)
	 {
		if(strlen(buffer) && buffer[strlen(buffer)] != *DivString)
	    	strcat(buffer, DivString);
		strcat(buffer, str);
		strcat(buffer, DivString);
     }
     else
		strcpy(buffer, str);
     SetText(buffer);      
     SetFocus(HWindow);
     SetSelection(strlen(buffer), strlen(buffer));
}

// По 35 пунктов на меню

#define MENU_ITEMS_CNT 50

void INIEdit::SetSectionName(const char* sect)
{
	delete Section;
	Section = strdup(sect);
}

void INIEdit::SetININame(const char* ini)
{
	delete _ININame;
	_ININame = strdup(ini);
}

void INIEdit::WMCommand(RTMessage msg)
{
    int MainMenu = msg.WParam / MENU_ITEMS_CNT;
    int SubMenu = msg.WParam % MENU_ITEMS_CNT;

    char Key[32];
    DynStr buffer;
    int IsOK = 0;

    if(SubMenu) {
		wsprintf(Key, Maska1, MainMenu);
		if(GetPrivateProfileString(Section, Key, "", buffer, buffer._sizeof(), this->_ININame))
		{
			wsprintf(Key, Maska2, MainMenu, SubMenu - 1);
			strcat(buffer, " ");
			if(GetPrivateProfileString(Section, Key, "", (char*)(const char*)buffer + strlen(buffer), buffer._sizeof() - strlen(buffer), this->_ININame))
			IsOK = 1;
		}
	}
    else 
    {
		wsprintf(Key, Maska1, MainMenu);
		if(GetPrivateProfileString(Section, Key, "", buffer, buffer._sizeof(), this->_ININame))
      	    IsOK = 1;
	}

    if(IsOK) {
		while(strstr(buffer, "&"))
		{
		   char* pos = strstr(buffer, "&");
		   memmove(pos, pos + 1, strlen(pos));
		}

		Text(buffer);
		return;
	}

	TEdit::WMCommand(msg);
}

class MySortList : public SortedArray {
public:
	  MySortList() : SortedArray(100, 0, 100) {}

      int find( const Object& o ) const { return SortedArray::find(o); }
};

void split(DynStr& buffer1)
{
/*	int pos = 0;
	int looking = 0;
			MessageBox(0, buffer1, "", 0);
	for(int ws = 0; ws < strlen(buffer1); ws++)
	{
		if(pos < ws - 50) {
			looking = 1;
		}
		else {
			looking = 0;
		}
		if(looking && buffer1[ws] == ' ') {
			pos = ws;
			buffer1.set('\n', ws);
			MessageBox(0, buffer1, "", 0);
		}
	}    */
	if(strlen(buffer1) > 100)
	{
		char* buff = buffer1;
		int len = strlen(buff);
		strcpy(buff + 47, "...");
		strcat(buff, buff + len - 47);
	}
}

HMENU BuildMenu(char* Maska1, char* Maska2, char* ININame, char* Section)
{
//	MessageBox(0, Maska1, "", 0);

    HMENU hmenu = CreatePopupMenu();

    DynStr buffer;
	DynStr buffer1;
	char Key[32];

    int BlockMenuItems = GetSystemMetrics(SM_CYSCREEN) / GetSystemMetrics(SM_CYMENU);

    int IsShift = GetAsyncKeyState(VK_SHIFT) & 0x8000;
    int IsCtrl = GetAsyncKeyState(VK_CONTROL) & 0x8000;

    MySortList list;
    list.ownsElements(1);

	for(int i = 0;; i++)
	{
		wsprintf(Key, Maska1, i);
		if(!GetPrivateProfileString(Section, Key, "", buffer, buffer._sizeof(), ININame))
		   break;

		HMENU SubMenu = CreatePopupMenu();

        MySortList listSub;
		listSub.ownsElements(1);
		for(int j = 0;; j++)
		{
			wsprintf(Key, Maska2, i, j);
			if(!GetPrivateProfileString(Section, Key, "", buffer1, buffer1._sizeof(), ININame))
	       		break;

			split(buffer1);

			Object* obj = new String(buffer1);
	   		listSub.add(*obj);

	   		if(!GetMenuItemCount(SubMenu) || IsShift)
		       AppendMenu(SubMenu, MF_ENABLED | MF_STRING, i * MENU_ITEMS_CNT + j + 1, buffer1);
    	    else
	    		InsertMenu(SubMenu, listSub.find(*obj), MF_BYPOSITION | MF_ENABLED | MF_STRING, i * MENU_ITEMS_CNT + j + 1, buffer1);
		}

			split(buffer);
	if(!GetMenuItemCount(SubMenu))
	{
	   DestroyMenu(SubMenu);
	   Object* obj = new String(buffer);
	   list.add(*obj);
	   if(GetMenuItemCount(hmenu) == 0 || IsShift)
	       AppendMenu(hmenu, MF_ENABLED | MF_STRING, i * MENU_ITEMS_CNT, buffer);
	   else
	       InsertMenu(hmenu, list.find(*obj), MF_BYPOSITION | MF_ENABLED | MF_STRING, i * MENU_ITEMS_CNT, buffer);
	}
	else {
	   Object* obj = new String(buffer);
	   list.add(*obj);
	   if(!GetMenuItemCount(hmenu) || IsShift)
	       AppendMenu(hmenu, MF_BYPOSITION | MF_POPUP, (UINT)SubMenu, buffer);
           else
	       InsertMenu(hmenu, list.find(*obj), MF_BYPOSITION | MF_POPUP, (UINT)SubMenu, buffer);
	}
    }

	for(i = 0; !IsCtrl && i < GetMenuItemCount(hmenu); i++) {
	if(i && (i % BlockMenuItems) == 0) {
             char buff[64]; 
	     GetMenuString(hmenu, i, buff, sizeof buff, MF_BYPOSITION);

	     if(GetMenuState(hmenu, i, MF_BYPOSITION) & MF_POPUP) {
		 HMENU oldPopup = GetSubMenu(hmenu, i);

		 HMENU newPopup = CreatePopupMenu();
		 for(int m = 0; m < GetMenuItemCount(oldPopup); m++) {
		     char bmBuff[64];
		     GetMenuString(oldPopup, m, bmBuff, sizeof bmBuff, MF_BYPOSITION);
		     AppendMenu(newPopup, MF_ENABLED | MF_STRING, GetMenuItemID(oldPopup, m), bmBuff);
		 }

		 ModifyMenu(hmenu, i, MF_BYPOSITION | MF_MENUBREAK | MF_POPUP, (UINT)newPopup, buff);
	     }
             else
		 ModifyMenu(hmenu, i, MF_BYPOSITION | MF_MENUBREAK | MF_STRING, GetMenuItemID(hmenu, i), buff);
	}
    }

    //delete buffer;
    //delete buffer1;
    //delete Key;

    return hmenu;
}

#define CACH_SIZE 50

INIEdit::~INIEdit()
{
	delete _ININame;
	delete Section;
	if(Cash) {
	   for(int i = 0; i < CACH_SIZE; i++) {
		   if(!Cash[i].maska1[0]) break;
		   DestroyMenu(Cash[i].menu);
	   }
	   memset(Cash, 0, sizeof(MenuCashe) * CACH_SIZE);
	}
}

void INIEdit::WMRButtonDown(RTMessage)
{
    if(!Cash) {
	   Cash = new MenuCashe[CACH_SIZE];
	   memset(Cash, 0, sizeof(MenuCashe) * CACH_SIZE);
    }

    HMENU hmenu = 0;
	for(int i = 0; i < CACH_SIZE; i++)
	{
		if(!Cash[i].maska1[0])
            break;
		if(!strcmp(Cash[i].maska1, Maska1))
	    hmenu = Cash[i].menu;
    }

	if(!hmenu) {
		hmenu = BuildMenu(Maska1, Maska2, _ININame, Section);
		strcpy(Cash[i].maska1, Maska1);
		Cash[i].menu = hmenu;
    }

    POINT ptCurrent;
    GetCursorPos(&ptCurrent);
    TrackPopupMenu(hmenu, TPM_LEFTALIGN, ptCurrent.x, ptCurrent.y, 0, HWindow, NULL);
}

void CityEdit::WMCommand(RTMessage msg)
{
    int i = msg.WParam / 100;
    int j = msg.WParam % 100;
    char buffer[64];
    char Key[32];
    wsprintf(Key, "CityCode%d_%d", i, j);
    assert(strlen(Key) < sizeof Key);
	if(GetPrivateProfileString(MANDATORY_SECT, Key, "", buffer, sizeof buffer, ::ININame)) {
        char* div = strchr(buffer, ',');
        *div = 0;
	SetText(buffer);
	PostMessage(GetParent(HWindow), NM_CITYEDIT, 0, (long)HWindow);
    }
}

void CityEdit::WMLButtonDown(RTMessage msg)
{
    WMRButtonDown(msg);
}

void CityEdit::WMRButtonDown(RTMessage)
{
    HMENU hmenu = CreatePopupMenu();

    DynStr buffer;

	HMENU SubMenu = 0;
	for(int i = 0;; i++)
    {
		for(int j = 0;; j++)
		{
			char Key[32];
			wsprintf(Key, "CityCode%d_%d", i, j);
			assert(strlen(Key) < sizeof Key);
			if(!GetPrivateProfileString(MANDATORY_SECT, Key, "", buffer, buffer._sizeof(), ::ININame))
			{
			   if(!j) goto loopsExit;
			   else break;
			}

			if(j)
			{
			   char* div = strchr(buffer, ',');
			   if(!div) {
				  goto loopsExit;
			   }
			   div++;
			   while(*div == ' ' && *div != 0) div++;
			   AppendMenu(SubMenu, MF_ENABLED, i * 100 + j, div);
			}
			else
			{
			   SubMenu = CreatePopupMenu();
			   AppendMenu(hmenu, MF_POPUP, (UINT)SubMenu, buffer);
			}
	  }
	}

    loopsExit:

    POINT ptCurrent;
    GetCursorPos(&ptCurrent);
    TrackPopupMenu(hmenu, TPM_LEFTALIGN, ptCurrent.x, ptCurrent.y, 0, HWindow, NULL);
    DestroyMenu(hmenu);
}

MenuCashe* INIEdit::Cash = 0;

NumberEdit::NumberEdit(PTWindowsObject Parent, int Id, int Len, int IsPoint, int IsMinus)
	  : INIEdit("", "", Parent, Id, Len, "", ""),
	    CanPoint(IsPoint),
        CanMinus(IsMinus)
{
}

NumberEdit::NumberEdit(PTWindowsObject Parent, int Id, int Len, int IsPoint, char* Mask)
	  : INIEdit("", "", Parent, Id, Len, Mask, ""),
	    CanPoint(IsPoint),
		CanMinus(0)
{
}

void NumberEdit::WMChar(RTMessage msg)
{
     char buffer[32];
     GetText(buffer, sizeof buffer);
     int nfPoint = !strchr(buffer, '.');
     int nfMinus = !strchr(buffer, '-');
	 if((msg.WParam >= '0' && msg.WParam <= '9') || msg.WParam == 8 || (CanPoint && nfPoint && (msg.WParam == '.' || msg.WParam == ',')) || (CanMinus && msg.WParam == '-')) {
  	    if(CanMinus && msg.WParam == '-') {
	      if(nfMinus) {
  		      memmove(buffer + 1, buffer, strlen(buffer) + 1);
		      *buffer = '-';
		      SetText(buffer);
          }
	      else
		     SetText(buffer + 1);
	         return;
		}

		if(msg.WParam == ',') msg.WParam = '.';
	    TEdit::DefWndProc(msg);
     }
     else
	    MessageBeep(0);
}

void INIEdit::DefWndProc(RTMessage Msg)
{
	//if(Msg.Message == WM_GETDLGCODE) Msg.Result = DLGC_WANTTAB;
    //else
	//if(Msg.Message == WM_KEYDOWN && Msg.WParam == VK_TAB) PostMessage(Parent->HWindow, WM_NEXTDLGCTL, GetAsyncKeyState(VK_SHIFT), 0);
	//else
	TEdit::DefWndProc(Msg);
}
