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
#include "dialog.h"

#include <alloc.h>
#include <string.h>

extern PTWindowsObject DlgCreationWindow;

TDialog::TDialog(PTWindowsObject AParent, LPSTR AName, PTModule AModule)
        :TWindowsObject(AParent, AModule),
         Center(1)
{
  DisableAutoCreate();
  Title = (LPSTR)MAKEINTRESOURCE(-1);
  if ( HIWORD(AName) )   // Not NULL and not an int in disguise
    Attr.Name = _fstrdup(AName);
  else
    Attr.Name = AName;
  Attr.Param = 0L;
  IsModal = FALSE;
}

/*TDialog::TDialog(PTWindowsObject AParent, int ResourceId, PTModule AModule)
        :TWindowsObject(AParent, AModule),
         Center(1)
{
  DisableAutoCreate();
  Title = (LPSTR)MAKEINTRESOURCE(-1);
  Attr.Name = (LPSTR)MAKEINTRESOURCE(ResourceId);
  Attr.Param = 0L;
  IsModal = FALSE;
}
*/
TDialog::~TDialog()
{
  if ( HIWORD(Attr.Name) )
    farfree(Attr.Name);
}

static BOOL RegisterFails(void *AWindowsObject, void *)
{
  return !((PTWindowsObject)AWindowsObject)->Register();
}

BOOL TDialog::Create()
{
  HWND HParent;

  IsModal = FALSE; // GetClassName called from Register needs to know this.
  if ( Status == 0 && Register() )
  {
    DisableAutoCreate();
    EnableKBHandler();
    if ( !Parent )
      HParent = 0;
    else
      HParent = Parent->HWindow;
    DlgCreationWindow = this;
    /* Register all the dialog's child objects (for custom control
       support) */
    if ( FirstThat(RegisterFails, NULL) == NULL )
    {
      HWindow = CreateDialogParam(GetModule()->hInstance,
               Attr.Name, HParent, (DLGPROC)GetInstance(), Attr.Param);
      if ( !HWindow )
        Status = EM_INVALIDWINDOW;
    }
    else
      Status = EM_INVALIDCHILD;
    DlgCreationWindow = NULL;
  }
  return (Status == 0);
}

int TDialog::Execute()
{
  HWND HParent;
  int ReturnValue = -1;
  PTWindowsObject OldKBHandler;

  IsModal = TRUE;
  if ( Status == 0  && Register() )
  {
    DisableAutoCreate();

    EnableKBHandler();
    if ( GetApplication() )
      OldKBHandler = GetApplication()->KBHandlerWnd;

    if ( !Parent )
      HParent = 0;
    else
      HParent = Parent->HWindow;
    DlgCreationWindow = this;

    if ( FirstThat(RegisterFails, NULL) == NULL )
    {
      ReturnValue = DialogBoxParam(GetModule()->hInstance, Attr.Name,
                                HParent, (DLGPROC)GetInstance(), Attr.Param);
      // -1 if the function cannot create the dialog box
      if ( ReturnValue == -1)
        Status = EM_INVALIDWINDOW;
    }
    else
      Status = EM_INVALIDCHILD;
    DlgCreationWindow = NULL;
    if ( GetApplication() )
      GetApplication()->SetKBHandler(OldKBHandler);
  }
  if ( Status == 0 )
    delete this;
  else
      if (ReturnValue != -1)
          ReturnValue = BAD_DIALOG_STATUS;  // dialog ran, but status != 0
  return ReturnValue;
}

void TDialog::WMInitDialog(TMessage&)
{
  SetupWindow();
  if(Center)
   {
     RECT r;

     GetWindowRect(HWindow, &r);
     WORD X = (GetSystemMetrics(SM_CXSCREEN) - (r.right - r.left)) / 2;
     WORD Y = (GetSystemMetrics(SM_CYSCREEN) - (r.bottom - r.top)) / 2;

     SetWindowPos(HWindow, 0, X, Y, 0, 0, SWP_NOSIZE);
   }
}

void TDialog::SetupWindow()
{
  SetCaption(Title);
  TWindowsObject::SetupWindow();
}

void TDialog::WMQueryEndSession(TMessage& Msg)
{
  if ( GetApplication() && this == GetApplication()->MainWindow )
    Msg.Result = (long)!(GetApplication()->CanClose());
  else
    Msg.Result = (long)!CanClose();
}

void TDialog::CloseWindow(int ARetValue)
{
  if ( IsModal )
  {
    if ( CanClose() )
    {
      TransferData(TF_GETDATA);
      ShutDownWindow(ARetValue);
    }
  }
  else   // !IsModal
    TWindowsObject::CloseWindow();
}

void TDialog::CloseWindow()
{
  if ( IsModal )
    CloseWindow(IDCANCEL);
  else   // !IsModal
    TWindowsObject::CloseWindow();
}

void TDialog::ShutDownWindow(int ARetValue)
{
  if ( IsModal )
    Destroy(ARetValue);
  else
    TWindowsObject::ShutDownWindow();
}

void TDialog::ShutDownWindow()
{
  if ( IsModal )
    Destroy(IDCANCEL);
  else
  TWindowsObject::ShutDownWindow();
}

static void DoEnableAutoCreate(void *AWindowsObject, void *)
{
  if ( ((PTWindowsObject)AWindowsObject)->HWindow )
    ((PTWindowsObject)AWindowsObject)->EnableAutoCreate();
}

void TDialog::Destroy(int ARetValue)
{
  if ( IsModal && HWindow)
  {
    ForEach(DoEnableAutoCreate, NULL);
    EndDialog(HWindow, ARetValue);
  }
  else
    TWindowsObject::Destroy();
}

void TDialog::Destroy()
{
  if ( IsModal )
    Destroy(IDCANCEL);
  else
    TWindowsObject::Destroy();
}

void TDialog::SetCaption(LPSTR ATitle)
{
  if ( (int)ATitle != -1 )
    TWindowsObject::SetCaption(ATitle);
}

void TDialog::Ok(TMessage&)
{
  CloseWindow(IDOK);
}

void TDialog::Cancel(TMessage&)
{
  ShutDownWindow();
}

void TDialog::WMClose(TMessage&)
{
  ShutDownWindow();
}

LPSTR TDialog::GetClassName()
{
  if ( IsModal )
    return (LPSTR)32770L;
  else
    return "3D_SaveDlg";
}

void TDialog::GetWindowClass(WNDCLASS& AWndClass)
{
  AWndClass.style = CS_HREDRAW | CS_VREDRAW;
  AWndClass.lpfnWndProc = DefDlgProc;
  AWndClass.cbClsExtra = 0;
  AWndClass.cbWndExtra = DLGWINDOWEXTRA;
  AWndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  AWndClass.lpszMenuName = NULL;
  AWndClass.hInstance = GetModule()->hInstance;
  AWndClass.hIcon = LoadIcon(0, IDI_APPLICATION);
  AWndClass.hCursor = LoadCursor(0, IDC_ARROW);
  AWndClass.lpszClassName = GetClassName();
}

int R = 255, G = 255, B = 255;

void TDialog::WMCtlColor(RTMessage msg)
{

    if(msg.LP.Hi == CTLCOLOR_EDIT || msg.LP.Hi == CTLCOLOR_LISTBOX)
     {
        SetBkColor((HDC)msg.WParam, RGB(R, G, B));
        SetTextColor((HDC)msg.WParam, RGB(0, 0, 128));
        msg.Result = (long)White;
        return;
     }

    int NO_BUTTON = 0;
    char buff[20];

    if(::GetClassName((HWND)msg.LP.Lo, buff, sizeof buff))
     if(!strcmpi(buff, "BUTTON"))
      {
        if((GetWindowLong((HWND)msg.LP.Lo, GWL_STYLE) & BS_CHECKBOX) == BS_CHECKBOX) NO_BUTTON = CTLCOLOR_BTN;
        if((GetWindowLong((HWND)msg.LP.Lo, GWL_STYLE) & BS_AUTORADIOBUTTON) == BS_AUTORADIOBUTTON) NO_BUTTON = CTLCOLOR_BTN;
      }

    if(msg.LP.Hi == CTLCOLOR_DLG ||
       msg.LP.Hi == CTLCOLOR_SCROLLBAR ||
       msg.LP.Hi == CTLCOLOR_STATIC ||
       msg.LP.Hi == NO_BUTTON)
     {
        SetBkColor((HDC)msg.WParam, RGB(192, 192, 192));
        msg.Result = (long)Gray;
        return;
     }

    DefWndProc(msg);
}
/*
void *TDialog::read(ipstream& is)
{
}

void TDialog::write(opstream& os)
{
}

TStreamable *TDialog::build()
{
}
*/