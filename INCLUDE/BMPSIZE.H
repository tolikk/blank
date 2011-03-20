#ifndef _BMPSIZE__H
#define _BMPSIZE__H

#include <edit.h>

class GetSizeBMP : public TDialog
{
    int     *dx, *dy;
    char    bmp_name[100];
    PTEdit  e1;
    PTEdit  e2;

public:
          GetSizeBMP(PTWindowsObject, char*, int*, int*);

    void  SetupWindow();
    void  Ok(RTMessage);
};

#endif