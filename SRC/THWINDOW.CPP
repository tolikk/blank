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
#include <thwindow.h>
#include <string.h>


// THelpWindow

THWindow::THWindow(LPSTR msg, int X, int Y)
{
//#define SHADOW 4
   Attr.X = X;
   Attr.Y = Y;
   Text = strdup(msg);

   HWND wind = GetApplicationObject()->MainWindow->HWindow;
   HDC testDC = GetDC(wind);
   font = CreateFont(8, 0, 0, 0, FW_THIN, 0, 0, 0, 0, 0, 0, 0, 0, "MS Sans Serif");
   font = (HFONT)SelectObject(testDC, font);
   SIZE wh;
   GetTextExtentPoint(testDC, Text, strlen(Text), &wh);
   font = (HFONT)SelectObject(testDC, font);
   ReleaseDC(wind, testDC);

   Attr.W = wh.cx + 16;
   Attr.H = wh.cy + 6;

   int wX = GetSystemMetrics(SM_CXSCREEN);
   if(Attr.X + Attr.W > wX)
    {
      int delta = wX - Attr.W;
      if(delta < 0) Attr.X = 0;
      else if(delta < Attr.X) Attr.X = delta;
    }
   
   DC = GetDC(0);
   SavedArea = CreateCompatibleBitmap(DC, Attr.W /*+ SHADOW*/, Attr.H /*+ SHADOW*/);
   SavedDC = CreateCompatibleDC(DC);
   SelectObject(SavedDC, SavedArea);
   BitBlt(SavedDC, 0, 0, Attr.W /*+ SHADOW*/, Attr.H /*+ SHADOW*/, DC, Attr.X, Attr.Y, SRCCOPY);
   Paint(DC);
}

void THWindow::Paint(HDC PaintDC)
{
//   HGDIOBJ brush = SelectObject(PaintDC, GetStockObject(BLACK_BRUSH));
//   RoundRect(PaintDC, Attr.X /*+ SHADOW*/, Attr.Y /*+ SHADOW*/, Attr.X + Attr.W /*+ SHADOW*/, Attr.Y + Attr.H /*+ SHADOW*/, Attr.H / 2, Attr.H / 2);

   HGDIOBJ brush = SelectObject(PaintDC, CreateSolidBrush(RGB(255, 255, 128)));
   RoundRect(PaintDC, Attr.X, Attr.Y, Attr.X + Attr.W, Attr.Y + Attr.H, Attr.H / 2, Attr.H / 2);
   SetTextColor(PaintDC, 0);
   SetBkMode(PaintDC, TRANSPARENT);
   SelectObject(PaintDC, font);
   TextOut(PaintDC, Attr.X + 7, Attr.Y + 2, Text, lstrlen(Text));
   DeleteObject(SelectObject(PaintDC, brush));
}

void THWindow::~THWindow()
{
   BitBlt(DC, Attr.X, Attr.Y, Attr.W /*+ SHADOW*/, Attr.H /*+ SHADOW*/, SavedDC, 0, 0, SRCCOPY);
   DeleteObject(SavedDC);
   DeleteObject(SavedArea);
   //DeleteDC(DC);
   ReleaseDC(0, DC);
   delete Text;
   DeleteObject(font);
}
