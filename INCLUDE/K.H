#ifndef _K__H
#define _K__H

class K : public TDialog
{
      PTEdit S[4];
      public:
             K(PTWindowsObject p);
             ~K();

        void SetupWindow(); 
        void SetK(int, int, int, int);
	void WMDestroy(RTMessage) = [WM_FIRST + WM_DESTROY];
};

extern K* koord;

#endif

