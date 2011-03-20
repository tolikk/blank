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
#include <bmpsize.h>
#include <stdio.h>
#include <string.h>
#include <paramdlg.h>

ParamDlg::ParamDlg(PTWindowsObject p)
           :TDialog(p, "PARAM")
{
   for(int i = 0; i < 5; i++)
     new TRadioButton(this, 101 + i, 0);

   new TEdit(this, 106, 5);
   new TEdit(this, 107, 5);
   new TEdit(this, 108, 4);
   new TEdit(this, 109, 4);
   new TEdit(this, 110, 4);
}
