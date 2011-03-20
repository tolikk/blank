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

#include <static.h>

GetSizeBMP::GetSizeBMP(PTWindowsObject p, char* name, int* x, int* y)
           :TDialog(p, "DXDY")
{
   dx = x;
   dy = y;
   strcpy(bmp_name, name);
   e1 = new TEdit(this, 200, 4);
   e2 = new TEdit(this, 201, 4);
}

void GetSizeBMP::SetupWindow()
{
   TDialog::SetupWindow();

   int  scX = *dx, scY = *dy;
   char buff[20];

   if(strchr(bmp_name, '.')) *strchr(bmp_name, '.') = 0;
   strcat(bmp_name, ".sc");
   FILE* f = fopen(bmp_name, "rt");
   if(f)
    {
       fscanf(f, "%u %u", &scX, &scY);
       fclose(f);
    }                 

   sprintf(buff, "%u", scX);
   e1->SetText(buff);
   sprintf(buff, "%u", scY);
   e2->SetText(buff);
}

void GetSizeBMP::Ok(RTMessage msg)
{
   char buff[20];

   e1->GetText(buff, sizeof buff);
   *dx = atoi(buff);
   e2->GetText(buff, sizeof buff);
   *dy = atoi(buff);
   FILE* f = fopen(bmp_name, "wt");
   fprintf(f, "%u %u", *dx, *dy);
   fclose(f);
   TDialog::Ok(msg);
}
