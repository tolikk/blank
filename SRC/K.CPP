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
#include "dialog.h"
#include "edit.h"
#include "stdio.h"
#include "k.h"

K* koord;
int litle = 1;

K::K(PTWindowsObject p)
  : TDialog(p, "KRD")
{
   koord = this;
   Center = 0;
   for(int i = 0; i < 4; i++)
     S[i] = new TEdit(this, 101 + i, 20);
}

void K::WMDestroy(RTMessage)
{
   koord = 0;
}

K::~K()
{
   koord = 0;
}

void K::SetupWindow()
{
    TDialog::SetupWindow();
    FlashWindow(HWindow, TRUE);
}

void K::SetK(int lx, int ly, int rx, int ry)
{ 
    char str [50];
    TScroller& s = *((PTWindow)(GetApplicationObject()->MainWindow))->Scroller;


    lx -= 10;
    ly -= 10;
    rx -= 10;
    ry -= 10;

    sprintf(str, "%5.1f", (lx + s.XUnit * s.XPos) / 3.);
    if(ry == ly) S[0]->SetText(str);
    sprintf(str, "%5.1f", (ly + s.YUnit * s.YPos) / 3.);
    if(ry == ly) S[1]->SetText(str);
    if(rx != lx) sprintf(str, "%5.1f", (rx + s.XUnit * s.XPos) / 3.);
    else goto hide;

    S[2]->SetText(str);

    if(ry != ly)
     {
        sprintf(str, "%5.1f", (ry + s.YUnit * s.YPos) / 3.);
        if(litle)
         {
           litle = 0;
           RECT r = { 0, 0, 97, 70 };
           MapDialogRect(HWindow, &r);
           SetWindowPos(HWindow, 0, 0, 0, r.right, r.bottom, SWP_NOMOVE);
           ShowWindow(S[2]->HWindow, SW_SHOW);
           ShowWindow(GetDlgItem(HWindow, 200), SW_SHOW);
         }
     }
    else
     { hide:
       if(!litle && (rx == lx))
        {
          litle = 1;
          RECT r = { 0, 0, 97, 46 };
          MapDialogRect(HWindow, &r);
          SetWindowPos(HWindow, 0, 0, 0, r.right, r.bottom, SWP_NOMOVE);
          ShowWindow(S[2]->HWindow, SW_HIDE);
          ShowWindow(GetDlgItem(HWindow, 200), SW_HIDE);
        }
     }

    S[3]->SetText(str);
    if(GetActiveWindow() == HWindow) SetFocus(Parent->HWindow);
}