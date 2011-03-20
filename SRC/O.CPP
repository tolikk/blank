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
#include "commdlg.h"
#include "string.h"
#include "edit.h"
#include "stdio.h"
#include "o.h"         
#include "paramdlg.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

extern int PageOffX;
extern int PageOffY;

void O::MyDrawText(HDC DC, LPSTR str, RECT r, int interline, int format)
{
    TEXTMETRIC tm;
    RECT dr = r;
    int len = r.right - r.left;
    GetTextMetrics(DC, &tm);

    int k = what_dc ? 1 : -1;
    dr.bottom = dr.top + k*tm.tmHeight;

    char* currstr = str;

    for(;;)
     {
       int currlen = strlen(currstr);
       if(!currlen) break;

       //

       while(len < LOWORD(GetTextExtent(DC, currstr, currlen)))
         {
           currlen--;
         }

       int alen = currlen;
       if(*(currstr + currlen) != '\0')
         while((*(currstr + currlen) != ' ') && (*(currstr + currlen) != '\r'))
           {
             currlen--;
             if(!currlen)
               {
                 currlen = alen;
                 break;
               }
           }

       if(strchr(currstr, '\r'))
         {
           if(currlen > (strchr(currstr, '\r') - currstr))
				currlen = strchr(currstr, '\r') - currstr;
         }

       int a = DT_LEFT;
       if(format & DT_RIGHT) a = DT_RIGHT;
       else if(format & DT_CENTER) a = DT_CENTER;

       DrawText(DC, currstr, currlen, &dr, DT_SINGLELINE | DT_BOTTOM | a);
       int k = what_dc ? 1 : -1;
       OffsetRect(&dr, 0, k*tm.tmHeight + interline);
       currstr += currlen;
       if(*currstr == '\r') currstr++;
       if(*currstr == '\n') currstr++;
       if(*currstr == 0) break;
       //
     }
}

char* Cross = "-- Крестик --";
char* NoPrint = "-- Не печатать --";
char* SimpleText = "-- Текст --";
/*int   AddCross;

UINT CALLBACK Hook(HWND wnd, UINT ui, WPARAM w, LPARAM l)
{
  DWORD dwVersion = GetVersion();
  int v1 = LOBYTE(LOWORD(dwVersion));
  int v2 = HIBYTE(LOWORD(dwVersion));

  if(v1 <= 3 && v2 < 95) {
    if(w == 1136 && AddCross)
     {
       HWND lb = GetDlgItem(wnd, w);
       if(SendMessage(lb, CB_FINDSTRINGEXACT, -1, (long)Cross) == CB_ERR)
	{
	 int index = SendMessage(lb, CB_ADDSTRING, 0, (long)Cross);
	 if(AddCross == 2)
	  {
	    SendMessage(lb, CB_SELECTSTRING, -1, (long)Cross);
	    PostMessage(lb, CB_SETEDITSEL, 0, MAKELPARAM(0, 55));
	  }
	}

       if(SendMessage(lb, CB_FINDSTRINGEXACT, -1, (long)NoPrint) == CB_ERR)
	{
	 int index = SendMessage(lb, CB_ADDSTRING, 0, (long)NoPrint);
	}
     }
  }
                
  if(!AddCross)
   ShowWindow(GetDlgItem(wnd, 121), SW_HIDE),
   ShowWindow(GetDlgItem(wnd, 122), SW_HIDE);

  return 0;
}

typedef UINT (CALLBACK* lpfnHook)(HWND, UINT, WPARAM, LPARAM);

*/
void cmFonts(LOGFONT& f)
{
  CHOOSEFONT CF;
  LOGFONT FontRec = f;

  CF.lStructSize = sizeof(CF);
  CF.hwndOwner = GetApplicationObject()->MainWindow->HWindow;
  CF.Flags = /*CF_ENABLEHOOK |*/ CF_ANSIONLY | CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT | CF_ENABLETEMPLATE;
  CF.nFontType = SCREEN_FONTTYPE;
  CF.lpLogFont = &FontRec;
  CF.nSizeMin = 20;
  CF.nSizeMax = 20;
  //CF.lpfnHook = (lpfnHook)MakeProcInstance((FARPROC)Hook, GetApplicationObject()->hInstance);
  CF.lpTemplateName = "FontDlg";
  CF.hInstance = GetApplicationObject()->hInstance;

  if (ChooseFont(&CF))
  {
    // Only get the font name, weight, and italics - we don't care about size }
    strcpy(f.lfFaceName, FontRec.lfFaceName);
	if(!strlen(f.lfFaceName)) strcpy(f.lfFaceName, "MS Sans Serif");
    f.lfHeight = abs(FontRec.lfHeight);
    f.lfItalic = FontRec.lfItalic;
    f.lfWeight = FontRec.lfWeight;
  }

  //FreeProcInstance((FARPROC)CF.lpfnHook);
}

O::O(int a, int b, int c, int d)
  :Object()
{
  TScroller& s = *((PTWindow)(GetApplicationObject()->MainWindow))->Scroller;

  lu.x = (min(a, c) - 10) + s.XUnit * s.XPos;
  lu.y = (min(b, d) - 10) + s.YUnit * s.YPos;
  rd.x = (max(a, c) - 10) + s.XUnit * s.XPos;
  rd.y = (max(b, d) - 10) + s.YUnit * s.YPos;

  format = DT_CENTER | DT_SINGLELINE;
  out = 0;
  interline = 0;
  strcpy(Text, "Толик");

  MainFontRec.lfHeight = abs(d - b);
  MainFontRec.lfWidth = 0;
  MainFontRec.lfEscapement = 0;
  MainFontRec.lfOrientation = 0;
  MainFontRec.lfWeight = 0;//FW_BOLD`;
  MainFontRec.lfItalic = 0;
  MainFontRec.lfUnderline = 0;
  MainFontRec.lfStrikeOut = 0;
  MainFontRec.lfCharSet = ANSI_CHARSET;
  MainFontRec.lfOutPrecision = OUT_DEFAULT_PRECIS;
  MainFontRec.lfClipPrecision = CLIP_DEFAULT_PRECIS;
  MainFontRec.lfQuality = PROOF_QUALITY;
  MainFontRec.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;
  strcpy(MainFontRec.lfFaceName, "MS Sans Serif");
  if(a || b || c  || d)
   {
     //AddCross = 1;
     cmFonts(MainFontRec);
     //AddCross = 0;
     RECT r = { a, b, c, d };
     InvalidateRect((GetApplicationObject()->MainWindow)->HWindow, &r, FALSE);
   }
}

void O::Print(FILE* f)
{
   // коорлд, фонт, размер, выравнивание
   while(strchr(Text, '\n'))
       *strchr(Text, '\n') = '\1';
   while(strchr(Text, '\r'))
       *strchr(Text, '\r') = '\2';
   fprintf(f, "%d %d %d %d \"%s\" %d %d %d %d %d %d \"%s\"\n", lu.x, lu.y, rd.x, rd.y,
                                            MainFontRec.lfFaceName,
                                            MainFontRec.lfHeight,
                                            (int)MainFontRec.lfItalic,
                                            format, (int)MainFontRec.lfWeight, out,
					    interline,
					    Text);
   while(strchr(Text, '\1'))
       *strchr(Text, '\1') = '\n';
   while(strchr(Text, '\2'))
       *strchr(Text, '\2') = '\r';
}

int O::Read(FILE* f)
{
   int Size = sizeof(Text) + 32;
   char* buffer = new char[Size];
   *buffer = 0;

   fgets(buffer, Size, f);
   if(strlen(buffer) < 20) return 1;

   lu.x = atoi(strtok(buffer, " "));
   lu.y = atoi(strtok(0, " "));
   rd.x = atoi(strtok(0, " "));
   rd.y = atoi(strtok(0, " "));
   strcpy(MainFontRec.lfFaceName, strtok(0, "\""));

   MainFontRec.lfHeight = atoi(strtok(0, " "));
   MainFontRec.lfItalic = atoi(strtok(0, " "));
   format = atoi(strtok(0, " "));
   MainFontRec.lfWeight = atoi(strtok(0, " "));
   out = atoi(strtok(0, " "));
   interline = atoi(strtok(0, " "));

   char* readText = strtok(0, "\"");
   *Text = 0;
   if(readText) strcpy(Text, readText);

   while(strchr(Text, '\1'))
       *strchr(Text, '\1') = '\n';
   while(strchr(Text, '\2'))
       *strchr(Text, '\2') = '\r';

   return 0;
}

void O::SetText(LPSTR Str)
{
   //Копировать текст если это не просто статический текст
   if(strcmp(MainFontRec.lfFaceName, ::SimpleText)) {
	   memset(Text, 0, sizeof Text);
	   strncpy(Text, Str, sizeof(Text) - 1);
   }
}

void O::Draw(HDC DC, int rect)
{
   RECT    r = { lu.x, lu.y, rd.x, rd.y };
   LOGFONT lf = MainFontRec;

   what_dc = rect;

   if(!rect)
    {
       r.left /= 0.3;
       r.top /= -0.3;
       r.right /= 0.3;
       r.bottom /= -0.3;
       lf.lfHeight /= 0.3;
    }
  else
   {
     HGDIOBJ pen = SelectObject(DC, CreatePen(PS_SOLID, 1, RGB(0, 255, 0)));
     HGDIOBJ br = SelectObject(DC, GetStockObject(NULL_BRUSH));
     Rectangle(DC, lu.x, lu.y, rd.x, rd.y);
     SelectObject(DC, br);
     DeleteObject(SelectObject(DC, pen));
     SetTextColor(DC, RGB(255, 0, 0));
     Rectangle(DC, rd.x - 1, rd.y - 1, rd.x + 1, rd.y + 1);
   }

   if(!rect) OffsetRect(&r, PageOffX, -PageOffY);

   if(!strcmp(MainFontRec.lfFaceName, ::NoPrint))
    {
      strcpy(Text, ::NoPrint);
      if(!rect) return;
    }

   if(!strcmp(MainFontRec.lfFaceName, ::Cross))
    {
	  if(!strcmp(Text, "X")) { //mm                         //pixels
	      HGDIOBJ pen = SelectObject(DC, CreatePen(PS_SOLID,
					                               0.7 / GetDeviceCaps(DC, HORZSIZE) * GetDeviceCaps(DC, HORZRES),
   					                               RGB(0, 0, 0) ));
 	      MoveTo(DC, r.left, r.top);
          LineTo(DC, r.right, r.bottom);
          MoveTo(DC, r.left, r.bottom);
          LineTo(DC, r.right, r.top);
	      DeleteObject(SelectObject(DC, pen));
      }
	  return;
    }

   if(!strcmp(MainFontRec.lfFaceName, ::SimpleText))
	   strcpy(lf.lfFaceName, "MS Sans Serif");

   HGDIOBJ f = SelectObject(DC, CreateFontIndirect(&lf));
   SetBkMode(DC, TRANSPARENT);

   char* _strout = new char[1024];
   memset(_strout, 0, 1024);

   if(out)
    {
      int W = LOWORD(GetTextExtent(DC, " ", 1));
      // W - ширина одного пробела

      int Spaces = (out / 10. * 3) / W;
	  if(!rect) Spaces = out / W;

      memset(_strout, ' ', min(Spaces, 1023));
    }

   strcat(_strout, Text);
   if(format & DT_SINGLELINE)
       DrawText(DC, _strout, strlen(_strout), &r, format | DT_BOTTOM);
   else {
       int il = interline / 10. * 3;
       if(!rect) il = -interline;
       MyDrawText(DC, _strout, r, il, format);
	 }
   delete _strout;

   DeleteObject(SelectObject(DC, f));
}

int O::A(int x, int y)
{
   TScroller& s = *((PTWindow)(GetApplicationObject()->MainWindow))->Scroller;

   x = (x - 10) + s.XUnit * s.XPos;
   y = (y - 10) + s.YUnit * s.YPos;

   int ret = ((x > lu.x) && (x < rd.x - 1) && (y > lu.y) && (y < rd.y - 1));

   if(!ret)
    if((x >= (rd.x - 1)) && (x <= (rd.x + 1)))
     if((y >= (rd.y - 1)) && (y <= (rd.y + 1)))
      return -1;

   return ret;
}

void O::Fonts(int x, int y)
{
   if(A(x, y)) {
      cmFonts(MainFontRec);
   }
}

void O::SimpleText(int x, int y)
{
   if(A(x, y)) {
      TDialog* dlg = new TDialog(GetApplicationObject()->MainWindow, "GETTEXT");
      new TEdit(dlg, 101, 256);
      dlg->SetTransferBuffer(Text);
      GetApplicationObject()->MainWindow->GetModule()->ExecDialog(dlg);
      strcpy(MainFontRec.lfFaceName, ::SimpleText);
   }
}

void O::Cross(int x, int y)
{
   if(A(x, y)) {
      strcpy(MainFontRec.lfFaceName, ::Cross);
   }
}

void O::NoPrint(int x, int y)
{
   if(A(x, y)) {
      strcpy(MainFontRec.lfFaceName, ::NoPrint);
   }
}

void O::ChangeParam(int x, int y)
{
   if(A(x, y))
    {
      RECT start = { lu.x, lu.y, rd.x, rd.y };

      int param[24] = { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1};

      TDialog* dlg = new ParamDlg(GetApplicationObject()->MainWindow);

      if(format & DT_RIGHT) param[1] = 1;
      else if(format & DT_CENTER) param[2] = 1;
      else param[0] = 1;


      if(format & DT_SINGLELINE) param[3] = 1;
      else if(format & DT_WORDBREAK) param[4] = 1;

      sprintf((char*)&param[5], "%d", rd.x - lu.x);
      sprintf(((char*)param + 15), "%d", out);
      sprintf(((char*)param + 20), "%d", interline);

      strcpy(((char*)param + 24), "0");
      strcpy(((char*)param + 28), "0");

      dlg->SetTransferBuffer(param);
      if(GetApplicationObject()->ExecDialog(dlg) == IDOK)
        {
           if(param[0]) format = DT_LEFT;
           if(param[1]) format = DT_RIGHT;
           if(param[2]) format = DT_CENTER;

           if(param[3]) format |= (DT_SINGLELINE | DT_EXTERNALLEADING);
           if(param[4]) format |= DT_WORDBREAK;

           rd.x = lu.x + atoi((char*)&param[5]);
           out = abs(atoi(((char*)&param + 15)));
           interline = atoi(((char*)&param + 20));

           lu.x -= atoi(((char*)&param + 24)) * 0.3;
           rd.x -= atoi(((char*)&param + 24)) * 0.3;
           lu.y -= atoi(((char*)&param + 28)) * 0.3;
           rd.y -= atoi(((char*)&param + 28)) * 0.3;
        }

       if(!param[4]) lu.y = rd.y -  MainFontRec.lfHeight;

       TScroller& s = *((PTWindow)(GetApplicationObject()->MainWindow))->Scroller;
       int a = lu.x - s.XUnit * s.XPos + 10;
       int b = lu.y - s.YUnit * s.YPos + 10;
       int c = rd.x - s.XUnit * s.XPos + 10;
       int d = rd.y - s.YUnit * s.YPos + 10;

       start.left -= s.XUnit * s.XPos + 10;
       start.top -= s.YUnit * s.YPos + 10;
       start.right -= s.XUnit * s.XPos + 10;
       start.bottom -= s.YUnit * s.YPos + 10;

       RECT r = { a, b, c, d };
       RECT res;
       UnionRect(&res, &start, &r);
       InvalidateRect((GetApplicationObject()->MainWindow)->HWindow, &res, FALSE);
    }
}