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
#include "array.h"
#include "ids.h"
#include "printer.h"
#include "ruler.h"
#include "rulerout.h"

void (*printFunc)(HDC);

void ShowRuler(HDC DC)
{
  SaveDC(DC);

  SetMapMode(DC, MM_LOMETRIC);
  ((PTRulerWin)(GetApplicationObject()->MainWindow))->Printing(DC);

  RestoreDC(DC, -1);
}

void TRulerOut::PrintPage(HDC DC, WORD, POINT, LPRECT, WORD)
{
   printFunc(DC);
}