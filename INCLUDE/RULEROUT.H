#ifndef _RULEROUT__H
#define _RULEROUT__H

_CLASSDEF(TRulerOut)
class TRulerOut : public TPrintout
{
public:
  TRulerOut(Pchar ATitle) : TPrintout(ATitle) {}
  virtual void PrintPage(HDC DC, WORD Page, POINT Size, LPRECT Rect,
      WORD Flags);
  void SetBanding( BOOL b ) { Banding = b; }
};

#endif
