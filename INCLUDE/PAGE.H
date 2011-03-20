#ifndef _PAGE__H
#define _PAGE__H

#include <edit.h>

class Page : public TDialog
{
public:
          PTEdit a, b;

          Page(PTWindowsObject);
          void SetupWindow();
          void Ok(RTMessage m);
};

class OffBlank : public Page
{
public:
          OffBlank(PTWindowsObject p, void* buff);
          void SetupWindow();
          void Ok(RTMessage m);
};

#endif