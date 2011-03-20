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
#include <dialog.h>
#include <edit.h>
#include <pxengine.h>
#include <pxtable.h>
#include <array.h>
#include <openbase.h>
#include <grncard.h>
#include <stdio.h>
#include <string.h>
#include <ruler.h>
#include <dlg_base.h>

char Dlg_base::Agent_Code[10];
char Dlg_base::Agent_Name[70];
extern char ActivateParadox;
double GetRateCurrencyFull(DATE, char*, DATE&);
char* GetRCStr(int);

char* ININame = 0;
TDialog* ActiveForm;

class ShowTimedMsg : public TDialog {
public:
   ShowTimedMsg(PTWindowsObject P) : TDialog(P, "MSG_DLG") {};
   void SetupWindow();

   void WMTimer(RTMessage) = [WM_TIMER];
};

void ShowTimedMsg::WMTimer(RTMessage msg)
{
   KillTimer(HWindow, 1);
   Ok(msg);	
}

void ShowTimedMsg::SetupWindow()
{
	TDialog::SetupWindow();
    SetTimer(HWindow, 1, 1000, 0);
}

char* Dlg_base::GetAgPercent(long Code, int Flag, int& IsUr)
{
   if(!ActivateParadox)
    	return "";

     TOpenBase agents(AgentDBName, AgentFields, AgFieldsCount);
	 if(agents.pxErr) return "Err";
     double N;
	 //sprintf(AgPercentBuffer, "%04d", Code);
	 memmove(AgPercentBuffer, &Code, 4);
	 AgPercentBuffer[4] = 0;
     agents(0, AgPercentBuffer);
     if(PXSrchKey(agents.getTblHandle(), agents.getRecHandle(), 1, SEARCHFIRST) != PXSUCCESS)
	 	return "";
     char buff[2];
     agents.Get();
	 agents(N, 4 + Flag);
	 agents(buff, sizeof buff, 3);
	 IsUr = buff[0] == 'U';
	 sprintf(AgPercentBuffer, "%g%", N);
     return AgPercentBuffer;
}

Dlg_base::Dlg_base(PTWindowsObject p, char* DLG, char* Name, char** Fields, int Count)
	:TDialog(p, DLG),
     base(0) 
{
	HMENU menu = GetSubMenu(GetMenu(Parent->HWindow), 1);
   for(int i = 0; i < GetMenuItemCount(menu); i++)
	if(EnableMenuItem(menu, i, MF_BYPOSITION | MF_GRAYED) == -1) break;

   char fullName[100];
   GetModuleFileName(GetModule()->hInstance, fullName, sizeof fullName);
   *strrchr(fullName, '\\') = 0;
   strcat(fullName, Name);
   base = new TOpenBase(fullName, Fields, Count);

   if(ActivateParadox)
   {
	   struct date d;
	   DATE today;
	   getdate(&d);
	   DATE foundDate;
	   DATE foundDate2;
	   PXDateEncode(d.da_mon, d.da_day, d.da_year, &today);
	   double Rate = GetRateCurrencyFull(today, "USD_NAL", foundDate);
	   double Rate2 = GetRateCurrencyFull(today, "EUR_NAL", foundDate2);
	   //sprintf(fullName, "%f %f", Rate2, Rate);
	   //	MessageBox(0, fullName, "!", MB_OK | MB_ICONINFORMATION);
	   if(foundDate > today || foundDate2 > today)
	   {
		MessageBox(0, GetRCStr(707), "!", MB_OK | MB_ICONINFORMATION);
	   }
	   if(Rate <= 0 || foundDate < today || Rate2 <= 0 || foundDate2 < today)
	   {
		  if(MessageBox(0, GetRCStr(708), "!", MB_YESNO | MB_ICONINFORMATION) == IDYES) {
          	((TRulerWin*)p)->RunReporter32("RATES");
          }
	   }
	}
}

void Dlg_base::SetupWindow()
{
   ActiveForm	= this;
   TDialog::SetupWindow();
   if(!ActivateParadox)
		return;

   DeleteMenu(GetSystemMenu(HWindow, FALSE), SC_ZOOM, MF_BYCOMMAND);
   DeleteMenu(GetSystemMenu(HWindow, FALSE), SC_SIZE, MF_BYCOMMAND);
   AppendMenu(GetSystemMenu(HWindow, FALSE), MF_SEPARATOR, 0, 0);
   AppendMenu(GetSystemMenu(HWindow, FALSE), MF_ENABLED, 1000, "Справка");
   if(base->pxErr)
    {
       MoveWindow(HWindow, 0, 0, 0, 0, 0);
       EnableWindow(HWindow, FALSE);
       PostMessage(HWindow, WM_CLOSE, 0, 0);
    }
   else {
       First();
   }
}

void SetMenuText(HWND Wnd, char* Text, int Item)
{
   HMENU menu = GetMenu(Wnd);
   ModifyMenu(menu, Item, MF_BYCOMMAND | MF_STRING | MF_HELP, Item, Text);
   DrawMenuBar(Wnd);
}

char* Dlg_base::getMenuStr()
{
   static char menustr[32];
   wsprintf(menustr, "%lu/%lu", (long)base->NumberCurrent(), (long)base->GetRecords());
   return menustr;
}

void Dlg_base::GetDataFromBase()
{
   TransferBuffer = GetBuffer();
   TransferData(TF_SETDATA);
   TransferBuffer = 0;
   SetButtons();
   SetMenuText(GetApplication()->MainWindow->HWindow, getMenuStr(), 1);
}

void Dlg_base::SetButtons()
{
   int flag1 = TRUE;
   if(base->NumberCurrent() == 1 || base->NumberCurrent() == 0) flag1 = FALSE;
   EnableWindow(GetDlgItem(HWindow, 401), flag1);
   EnableWindow(GetDlgItem(HWindow, 403), flag1);

   flag1 = TRUE;
   if(base->NumberCurrent() == base->GetRecords()) flag1= FALSE;
   EnableWindow(GetDlgItem(HWindow, 402), flag1);
   EnableWindow(GetDlgItem(HWindow, 404), flag1);

   EnableWindow(GetDlgItem(HWindow, 400), base->GetRecords() < 1 ? FALSE : TRUE);
}

int Dlg_base::SetDataToBase()
{
   TransferBuffer = GetBuffer();
   TransferData(TF_GETDATA);
   TransferBuffer = 0;
   return 1;
}

Dlg_base::~Dlg_base()
{
   ActiveForm = NULL;
   SetMenuText(GetApplication()->MainWindow->HWindow, "", 1);

   for(int i = 0;; i++)
   //	if(i != 1 && i != 2 && i != 3 && i != 11 && i != 7 && i != 8 && i != 9 && i != 10) //Warta //Green Card //Leben //foreign
	 if(EnableMenuItem(GetSubMenu(GetMenu(Parent->HWindow), 1), i, MF_BYPOSITION | MF_ENABLED) == -1) break;

   if(base) delete base;
}

void Dlg_base::First()
{
   base->Refresh();
   if(base->First())
    {
       base->Get();
       GetDataFromBase();
    }
   else {
	   MessageBox(GetApplication()->MainWindow->HWindow, "База данных пуста", "Сообщаю", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
   }
   SetButtons();
}

void Dlg_base::Last()
{
   base->Refresh();
   if(base->Last())
    {
       base->Get();
       GetDataFromBase();
    }
   else MessageBox(GetApplication()->MainWindow->HWindow, "База данных пуста", "Сообщаю", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
   SetButtons();
}

void Dlg_base::Prev()
{
   base->Refresh();
   if(base->Prev())
    {
       base->Get();
       GetDataFromBase();
    }
   else MessageBox(GetApplication()->MainWindow->HWindow, "Предыдущей записи не существует", "Сообщаю", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
}

void Dlg_base::Next()
{
   base->Refresh();
   if(base->Next())
    {
       base->Get();
       GetDataFromBase();
    }
   else MessageBox(GetApplication()->MainWindow->HWindow, "Следующей записи не существует", "Сообщаю", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
}

static InsideSave;

int Dlg_base::SavePolis()
{
   base->Refresh();
   PXRecBufEmpty(base->getRecHandle());
   if(!SetDataToBase()) return 0;
   int InsErr = base->AddRecord();
   if((InsErr != PXSUCCESS) && (InsErr != PXERR_KEYVIOL))
       return 0;

   if(InsErr == PXERR_KEYVIOL) {
       Update();
       if(PXRecUpdate(base->getTblHandle(), base->getRecHandle())) return 0;
   }
   if(AfterSave()) {
       GetModule()->ExecDialog(new ShowTimedMsg(this));
       return 1;
   }
   return 0;
}

void Dlg_base::Save()
{
   if(InsideSave) {
      MessageBox(HWindow, "Повторный вход в сохранение данных!", "Сообщаю", MB_ICONSTOP | MB_SYSTEMMODAL);
      return;
   }

   InsideSave = 1;
   base->Refresh();
   PXRecBufEmpty(base->getRecHandle());
   if(!SetDataToBase()) {
      InsideSave = 0;
      return;
   }

   int InsErr = base->AddRecord();

   if((InsErr != PXSUCCESS) && (InsErr != PXERR_KEYVIOL)) {
      char ErrMsg[128];
      MessageBeep(0);
      sprintf(ErrMsg, "ОШИБКА СОXРАНЕНИЯ ДАННЫХ (N=%d)! ОБРАТИТЕСЬ К РАЗРАБОТЧИКАМ!", InsErr);
      WriteProfileString("Paradox Engine", "Stop Blank (::Save)", ErrMsg);
      MessageBox(HWindow, ErrMsg, "Сообщаю", MB_ICONSTOP | MB_SYSTEMMODAL);
      InsideSave = 0;
      return;
   }

   if(InsErr == PXERR_KEYVIOL)
   {
      if(MessageBox(HWindow, "Такой полис уже есть в базе.\nВы хотите обновить его?", "Сообщаю", MB_YESNO | MB_ICONQUESTION | MB_SYSTEMMODAL) == IDYES)
	  {
		  Update();
		  int status = PXRecUpdate(base->getTblHandle(), base->getRecHandle());
		  if(!status)
		    goto OK;
		  else
          {
			MessageBox(HWindow, "Ошибка при обновлении записи в Базе данных", PXErrMsg(status), MB_ICONINFORMATION | MB_OK | MB_SYSTEMMODAL);
		  }
	  }
   }
   else
   {
      OK:
      if(AfterSave())
	     GetModule()->ExecDialog(new ShowTimedMsg(this));
   }

   SetButtons();
   PXSave();

   char menustr[50];
   wsprintf(menustr, "%lu/%lu", (long)base->NumberCurrent(), (long)base->GetRecords());
   SetMenuText(GetApplication()->MainWindow->HWindow, menustr, 1);

   InsideSave = 0;
}

void Dlg_base::Delete()
{
   GetDataFromBase();
   if(!CanDeleteRecord()) return;
   base->Refresh();
   if(MessageBox(Parent->HWindow, "Вы хотите удалить текущую запись из базы?", "Подтвердите", MB_YESNO | MB_ICONQUESTION | MB_SYSTEMMODAL) == IDYES)
    {
       if(!BeforeDelete()) return;
       base->Delete();
       AfterDelete();
       Clear();
       First();
    }
}

void Dlg_base::Clear()
{
}

void Dlg_base::Print1()
{
//   InvalidateRect(Parent->HWindow, 0, 0);
   SendMessage(Parent->HWindow, WM_COMMAND, 101, 0L);
}

void Dlg_base::Print2()
{
//   InvalidateRect(Parent->HWindow, 0, 0);
   SendMessage(Parent->HWindow, WM_COMMAND, 101, 0L);
}

void  Dlg_base::Help(RTMessage msg)
{
   DefWndProc(msg);
//   DefDlgProc(HWindow, msg.Message, msg.WParam, msg.LParam);
}


DynStr::DynStr(int Size)
{
    m_data = new char[Size];
    m_size = Size;
    memset(m_data, 0, Size);	
}

DynStr::~DynStr()
{
    delete m_data;	
}

Dlg_base::ShowArray(Array& list_errors)
{
	if(list_errors.getItemsInContainer() > 0)
	{
	   DynStr ppath;
	   strcpy(ppath, "notepad.exe ");
	   char* path = ppath + strlen(ppath);
	   GetModuleFileName(GetApplicationObject()->hInstance, path, 128);
	   *(strrchr(path, '\\') + 1) = 0;
	   strcat(path, "experr.txt");
	   FILE* f = fopen(path, "wt");
	   if(f)
	   {
		   fprintf(f, "%s\n", GetRCStr(634));
		   for(int i = 0; i < list_errors.getItemsInContainer(); i++)
			   fprintf(f, "%s\n", (const char*)(String&)list_errors[i]);
		   fclose(f);
		   WinExec(ppath, SW_SHOW);
	   }
	   else
	   {
		   MessageBox(GetApplicationObject()->MainWindow->HWindow, "Не могу создать файл с ошибками", path, MB_OK | MB_ICONSTOP);
	   }
	}
 }