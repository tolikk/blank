#ifndef _O__H
#define _O__H

#include "stdio.h"
#include "pxengine.h"
#include "openbase.h"

class O : public Object
{
            int       what_dc;
public:
            POINT     lu, rd;
	    LOGFONT   MainFontRec;
	    char      Text[605];
            int       format;
            int       out;
            int       interline;
                      O(int, int, int, int);

            void      Draw(HDC, int rect = 1);
            void      Print(FILE*);
             int      Read(FILE*);
            void      SetText(LPSTR);
            void      ChangeParam(int, int);
	    void      Fonts(int, int);
	    void      Cross(int, int);
	    void      NoPrint(int, int);
	    void      SimpleText(int, int);
	     int      A(int x, int y);
            void      MyDrawText(HDC DC, LPSTR str, RECT r, int interline, int format);

    virtual classType isA() const { return __firstUserClass; }
    virtual Pchar     nameOf() const { return ""; }
    virtual           hashValueType hashValue() const { return 0; }
    virtual int       isEqual(RCObject APoint) const { return 0; }
    virtual void      printOn(Rostream outputStream) const {}

};
/*
class ODB1 : public Object
{
public:
            DATE      date;
            char      Polis[31];
            char      Man[81];
            char      Car_art[21];
            char      Car_num[16];
            DATE      from, to;
            double    sum;

                      ODB1();
            void      Draw(HDC, int);

            void      Read(TOpenBase*);

    virtual classType isA() const { return __firstUserClass; }
    virtual Pchar     nameOf() const { return ""; }
    virtual           hashValueType hashValue() const { return 0; }
    virtual int       isEqual(RCObject APoint) const { return 0; }
    virtual void      printOn(Rostream outputStream) const {}

};
  */
#endif

