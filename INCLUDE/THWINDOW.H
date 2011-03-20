#ifndef __THWINIDW_H
#define __THWINIDW_H

class THWindow// : public TWindow
{
       LPSTR   Text;
       HBITMAP SavedArea;
       HDC     DC, SavedDC;
       HFONT   font;

       struct { int X, Y, W, H; } Attr;
        
    public:
       THWindow(/*PTWindowsObject, */LPSTR, int, int);
       ~THWindow();

//       virtual void SetupWindow();
                 void Paint(HDC PaintDC);//, PAINTSTRUCT _FAR & PaintInfo);
};

#endif