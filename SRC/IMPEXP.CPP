#include <owl.h>
#include <string.h>
#include <stdio.h>
#include <array.h>
#include <window.h>
#include <pxengine.h>
#include <pxtable.h>
#include <openbase.h>
#include <commdlg.h>
#include "impexp.h"
#include "dlg_base.h"
#include "listbox.h"
#include "util.h"
#include "sys\stat.h"
#include "time.h"
#include "DLG_BASE.H"

int _IsValidPolisNumber(const char* SECTION, _str s, long n, char* INI, char* Agent);
extern char ValidateInternalImport;
void CopyFieldValue(TOpenBase& From, FILE* f, int field);
int RestoreFieldValue(FILE* f, TOpenBase& To, int field);
BOOL MyOpenFile(HWND HWindow, OPENFILENAME& ofn, char* szFilter, char* szDirName, char* szFile, char* Title, char* ext, int Load = 1 );
void auxAddAgentCode(Array& AgentsCodes, char* Code);
const char* GetMyAgCode(char* AgentCode, Array& file, Array& base, int);

int SaveAgents(Array& AgentsCodes, FILE* f, TOpenBase& AGTable, Array* list_errors, TOpenBase& base, int AgField)
{
	DynStr Buffer;

	int AgCnt = AgentsCodes.getItemsInContainer();
    fwrite(&AgCnt, sizeof AgCnt, 1, f);
	for(int i = 0; i < AgentsCodes.getItemsInContainer(); i++) {
		AGTable.First();
		AGTable(0, (char*)(const char*)((String&)AgentsCodes[i]));
		int FindCode = PXSrchKey(AGTable.getTblHandle(), AGTable.getRecHandle(), 1, SEARCHFIRST);
		if(FindCode == PXSUCCESS) {
			AGTable.Get();

			int Len = strlen((const char*)((String&)AgentsCodes[i]));
			fwrite(&Len, sizeof Len, 1, f);
			fwrite((const char*)((String&)AgentsCodes[i]), Len, 1, f);

			AGTable(Buffer, Buffer._sizeof(), 1);
			Len = strlen(Buffer);
			fwrite(&Len, sizeof Len, 1, f);
			fwrite((char*)Buffer, Len, 1, f);
			//MessageBox(0, Buffer, "", 0);
		}
		else {
			wsprintf(Buffer, "[%d] Не найден код агента '%s'. ", FindCode, (const char*)((String&)AgentsCodes[i]));
			base(AgField, (char*)(const char*)((String&)AgentsCodes[i]));
			if(PXSrchFld(base.getTblHandle(), base.getRecHandle(), base.getField(AgField), SEARCHFIRST) == PXSUCCESS) {
				base.Get();
				base(Buffer + strlen(Buffer), 10, 0);
				double N;
				base(N, 1);
				sprintf(Buffer + strlen(Buffer), "/%010.0f", N);
			}
			if(list_errors) (*list_errors).add(*new String(Buffer));
			else MessageBox(GetApplicationObject()->MainWindow->HWindow, Buffer, "", 0);
			return 0;
		}
	}
	return 1;
}

int ReadAgents(FILE* f, Array& AgentsCodes, Array& AgentsNames, Array& NewAgentsCodes)
{
	SetCursor(LoadCursor(0, IDC_WAIT));

	//Чтение агентов
	int AgCount = 0;
	fread(&AgCount, sizeof AgCount, 1, f);

    char str[64] = "";
	int Size;

	//wsprintf(str, "Agents %d", AgCount);
	//MessageBox(0, str, str, 0);

	for(int i = 0; i < AgCount; i++) {
	   Size = 0;
	   memset(str, 0, sizeof str);
	   if(fread(&Size, sizeof Size, 1, f) == 0 || fread(str, 1, Size, f) < Size) {
		   MessageBox(0, "e", str, 0);
		   return 0;
       }
	   str[Size] = 0;

	   if(strcmp(str, "АЛВН")) {
		   AgentsCodes.add(*new String(str));
		   if(fread(&Size, sizeof Size, 1, f) == 0 || fread(str, 1, Size, f) < Size) {
			  return 0;
		   }
		   str[Size] = 0;
		   if(Size == 0) strcpy(str, "<БЕЗ ИМЕНИ>");
		   AgentsNames.add(*new String(str));
		   NewAgentsCodes.add(*new String(""));
	   }
	   else {
		   if(fread(&Size, sizeof Size, 1, f) == 0 || fread(str, 1, Size, f) < Size) {
		   MessageBox(0, "@", str, 0);
			  return 0;
		   }
	   }
	}

   AgentsCodes.add(*new String("АЛВН"));
   NewAgentsCodes.add(*new String("АЛВН"));

	return 1;
}

long ExportData(char* TableName, char** Fields, int FieldsCount, ImpExpCallback func, void* Data, char* filename, int m, int y, int dm, int AgField, int IgnoreCondField)
{
	Array AgentsCodes(10, 0, 10);
    AgentsCodes.ownsElements(1);

	FILE* f = fopen(filename, "wb");
	if(!f) return 1;

	fwrite("v3", 2, 1, f);

	DynStr name;
	GetModuleFileName(GetApplicationObject()->hInstance, name, name._sizeof());
	struct stat statbuf;
	stat(name, &statbuf);
	struct tm* t = gmtime(&statbuf.st_ctime);
	sprintf(name, "%02d.%02d.%d", t->tm_mday, t->tm_mon+1, 1900 + t->tm_year);
	fwrite(name, 11, 1, f);

	TOpenBase agents(AgentDBName, AgentFields, 2);
	if(agents.pxErr) {
        fclose(f);	
		unlink(filename);
		return 2;
    }

	agents.Get();
	CopyFieldValue(agents, f, 0);
	CopyFieldValue(agents, f, 1);

	fwrite(&m, sizeof(m), 1, f);
	fwrite(&y, sizeof(y), 1, f);
	fwrite(&dm, sizeof(dm), 1, f);

	short Size = strlen(TableName);
	fwrite(&Size, 2, 1, f);
	fprintf(f, TableName);
	Size = FieldsCount;
	fwrite(&Size, 2, 1, f);
	for(int i = 0; i < FieldsCount; i++) {
		Size = strlen(Fields[i]);
		fwrite(&Size, 2, 1, f);
		fprintf(f, Fields[i]);
	}

	TOpenBase base(TableName, Fields, FieldsCount);
	if(base.pxErr) { 
		fclose(f);
		unlink(filename);
		return 3;
    }

	DATE MinDate, MaxDate;
	PXDateEncode(m, 1, y, &MinDate);
	m += (dm + 1);
	if(m > 12) {
		m -= 12;
        y++;
	}
	PXDateEncode(m, 1, y, &MaxDate);

	long AllRecords = base.GetRecords();
	long SavedRecords = 0;

	Process* findWnd = (Process*)GetApplicationObject()->MainWindow->GetModule()->MakeWindow(new Process(GetApplicationObject()->MainWindow));
    SetCursor(LoadCursor(0, IDC_WAIT));

	for(long n = 0; n < AllRecords; n++) {
		if((n % 50) == 0) findWnd->SetAll(SavedRecords);
		findWnd->SetPercent(n * 100 / AllRecords);

		base.Get();

		int IgnoreRecord = 1;
		for(int j = 0; IgnoreRecord == 1 && j < FieldsCount; j++) {
        	char fldType[5];
			PXFldType(base.getTblHandle(),
						base.getField(j),
						sizeof(fldType),
						fldType);
			if(*fldType == 'D' && !base.IsNull(j) && j != IgnoreCondField) {
				DATE dt;
				(base)(dt, j);
				if(dt >= MinDate && dt < MaxDate) IgnoreRecord = 0;
			}
		}

		if(!IgnoreRecord) {
			if(func && !func(&base, 0, Data)) {
				delete findWnd;
				fclose(f);
				unlink(filename);
				return 4;
            }
			fprintf(f, "+");
			for(int j = 0; j < FieldsCount; j++)
				CopyFieldValue(base, f, j);
			SavedRecords++;

			if(AgField != -1) {
				char Agent_Code[10];
				base(Agent_Code, sizeof Agent_Code, AgField);
				auxAddAgentCode(AgentsCodes, Agent_Code);
            }
		}
		base.Next();
    }
	fprintf(f, ".");

	if(!SaveAgents(AgentsCodes, f, agents, 0, base, AgField)) {
		delete findWnd;
		fclose(f);
		unlink(filename);
		return 0;
    }

	if(func && !func(0, 0, Data)) {
		delete findWnd;
		fclose(f);
		unlink(filename);
		return 5;
    }

	fclose(f);
	delete findWnd;

	if(SavedRecords == 0L) unlink(filename);

	return -SavedRecords;
}

void CopyFieldValue(TOpenBase& From, TOpenBase& To, int field);

//если есть OnlyFields то только они обновляются else все
long ImportData(char* TableName, char** Fields, int FieldsCount, ImpExpCallback func, void* Data, int AgField, int StateField, int BadState, char* OnlyFields1, char* OnlyFields2)
{
	ValidateInternalImport = GetPrivateProfileInt("DIVISION", "ValidateImport", 0, ININame);

	Array list(100);
	list.ownsElements(TRUE);
	OPENFILENAME ofn;
	DynStr szDirName(1025);
	DynStr buffer(1025);
	strcpy(szDirName, "D:\\");

	DynStr filename;

	char  l_szFilter[] = "Обменные файлы(*.bdt)|*.bdt||";

	if(!MyOpenFile(GetApplicationObject()->MainWindow->HWindow, ofn, l_szFilter, szDirName, filename, "Импорт данных", "", 1)) return 0;

	FILE* f = fopen(filename, "rb");
	if(!f) return 1;

	SetCursor(LoadCursor(0, IDC_WAIT));

    char v;
	fread(&v, 1, 1, f);
	if(v != 'v') {
		fclose(f);
		return 2;
    }
	fread(&v, 1, 1, f);
	if(v != '2' && v != '3') {
		fclose(f);
		return 3;
	}

	char AppDate[11];
	if(v >= '3') {
		fread(AppDate, 11, 1, f);
	}

	TOpenBase agents(AgentDBName, AgentFields, 2);
	if(agents.pxErr) {
        fclose(f);	
		return 4;
    }

	RestoreFieldValue(f, agents, 0);
	RestoreFieldValue(f, agents, 1);

	int m, y, dm;

	fread(&m, sizeof(m), 1, f);
	fread(&y, sizeof(y), 1, f);
	fread(&dm, sizeof(dm), 1, f);

	short Size;
	fread(&Size, 2, 1, f);
	fread(buffer, Size, 1, f); //TableName
	((char*)buffer)[Size] = 0;

	if(strcmp(buffer, TableName)) {
		fclose(f);
		return 5;
    }

	fread(&Size, 2, 1, f);

	if(Size != FieldsCount) {
		fclose(f);
		return 6;
    }

	for(int i = 0; i < Size; i++) {
		short fSize;
		fread(&fSize, 2, 1, f);
		fread(buffer, fSize, 1, f); //FieldName
		((char*)buffer)[fSize] = 0;
		if(strcmp(buffer, Fields[i])) {
			fclose(f);
			return 7;
        }
	}

	TOpenBase base(TableName, Fields, FieldsCount);
	if(base.pxErr) { 
		fclose(f);
		return 8;
	}

	TOpenBase base2(TableName, Fields, FieldsCount);
	if(base2.pxErr) {
		fclose(f);
		return 18;
	}          

	strcpy(buffer, "Отчёт от ");
	agents(buffer + strlen(buffer), buffer._sizeof(), 1);
	wsprintf(buffer + strlen(buffer), " за %02d.%04d", m, y);
	if(dm > 0) wsprintf(buffer + strlen(buffer), " + %d месяцев", dm);
	if(v >= '3') wsprintf(buffer + strlen(buffer), "\r\nСоздан программой от %s", AppDate);
	strcpy(szDirName, buffer);
	wsprintf(buffer + strlen(buffer), "\r\nИмпортировать?");

	if(MessageBox(GetApplicationObject()->MainWindow->HWindow, buffer, "?", MB_YESNO | MB_ICONQUESTION) == IDNO) return 0;

	char* OnlyFields = 0;
	if(OnlyFields1 && OnlyFields2) {
		int Answer = MessageBox(GetApplicationObject()->MainWindow->HWindow, GetRCStr(625), "?", MB_YESNOCANCEL | MB_ICONQUESTION);
		if(Answer == IDNO) OnlyFields = OnlyFields1;
		if(Answer == IDCANCEL) OnlyFields = OnlyFields2;
	}

	SetCursor(LoadCursor(0, IDC_WAIT));

	long start_file_pos = ftell(f);
	for(;;)
	{
    	char ch = 0;
		if(fread(&ch, 1, 1, f) == 0) return 12;
		if(ch == '.') break;
		if(ch != '+') {
			fclose(f);
			return 10;
        }

		for(int j = 0; j < FieldsCount; j++)
        {
			if(!RestoreFieldValue(f, base, j)) {
				fclose(f);
				return 9;
			}
        }

	}

	Array AgentsCodes(10, 0, 10);
	Array AgentsNames(10, 0, 10);
	Array NewAgentsCodes(10, 0, 10);
    AgentsCodes.ownsElements(1);
    AgentsNames.ownsElements(1);
	NewAgentsCodes.ownsElements(1);

	if(!ReadAgents(f, AgentsCodes, AgentsNames, NewAgentsCodes)) {
		fclose(f);
		return 12;
    }

	LinkAgDlg* dlg = new LinkAgDlg(GetApplicationObject()->MainWindow);
	dlg->Msg = buffer;
    dlg->AgCodesPtr = &AgentsCodes;
    dlg->NewAgCodesPtr = &NewAgentsCodes;
    dlg->AgNamesPtr = &AgentsNames;
    if(GetApplicationObject()->ExecDialog(dlg) != IDOK) {
       fclose(f);
       return 0;
	}

	fseek(f, start_file_pos, SEEK_SET);

	SetCursor(LoadCursor(0, IDC_WAIT));

	long NewRec = 0, NoChRec = 0, UpdRec = 0, IgnoreErr = 0;

	for(;;) {
    	char ch = 0;
		if(fread(&ch, 1, 1, f) == 0) return 12;
		if(ch == '.') break;
		if(ch != '+') {
			fclose(f);
			return 10;
        }

		PXRecBufEmpty(base2.getRecHandle());
		for(int j = 0; j < FieldsCount; j++)
			if(!RestoreFieldValue(f, base2, j)) {
				fclose(f);
				return 9;
			}

		if(AgField != -1) {
			base2(buffer, buffer._sizeof(), AgField);
			base2(AgField, (char*)GetMyAgCode(buffer, AgentsCodes, NewAgentsCodes, 10));
        }

		if(ValidateInternalImport && !_IsValidPolisNumber("MANDATORY_SECT", base2.GetString(0), base2.GetDouble(1), 0, base2.GetString(AgField).s))
		{
			DynStr errMsg;
			sprintf(errMsg, GetRCStr(709), base2.GetString(0).s, (long)base2.GetDouble(1), base2.GetString(AgField).s);
			MessageBox(GetApplicationObject()->MainWindow->HWindow, errMsg, "!", MB_OK | MB_ICONSTOP);
			fclose(f);
			return 0;
		}

		char Seria[10];
		double N;
        base2(Seria, sizeof Seria, 0);
		base2(N, 1);
        base(0, Seria);
		base(1, N);
		if(PXSrchKey(base.getTblHandle(), base.getRecHandle(), 2, SEARCHFIRST) == PXSUCCESS) {
			base.Get();

			int skip = 0;

			if(strcmp(base.GetString(AgField).s, base2.GetString(AgField).s))
			{
				DynStr errMsg;
				sprintf(errMsg, GetRCStr(710), base2.GetString(0).s, (long)base2.GetDouble(1), base.GetString(AgField).s, base2.GetString(AgField).s);
				int answer = MessageBox(GetApplicationObject()->MainWindow->HWindow, errMsg, "!", MB_YESNOCANCEL | MB_ICONSTOP);
				if(answer == IDCANCEL)
				{
					fclose(f);
					return 0;
                }
				if(answer == IDNO)
				{
					list.add(*new String(errMsg));
					skip = 1;
				}
			}

			if(func && !func(&base2, &base, Data))
			 {
				IgnoreErr++;
				skip = 1;
			}


			if(!skip && StateField != -1) {
				char fldType[5];
				PXFldType(base.getTblHandle(),
							base.getField(StateField),
							sizeof(fldType),
							fldType);

                int State1, State2;
				if(*fldType == 'A') {
					base(buffer, buffer._sizeof(), StateField);
					State1 = atoi(buffer);
					base2(buffer, buffer._sizeof(), StateField);
					State2 = atoi(buffer);
				}
                else
				if(*fldType == 'N') {
					double St;
					base(St, StateField);
					State1 = St + 0.0001;
					base2(St, StateField);
					State2 = St + 0.0001;
				}
				else {
					MessageBox(0, fldType, fldType, 0);
					return 51;
                }

				if(State1 != State2 && (State1 == BadState || State2 == BadState)) {
					IgnoreErr++;
					continue;
				}
			}


			memset(buffer, buffer._sizeof(), 0);
			memset(szDirName, szDirName._sizeof(), 0);
			PXRawGet(base.getRecHandle(), buffer, buffer._sizeof());
			PXRawGet(base2.getRecHandle(), szDirName, szDirName._sizeof());
			if(memcmp(buffer, szDirName, buffer._sizeof()) == 0)
			{
				NoChRec++;
			}
			else
			{
				if(!OnlyFields)
					PXRecBufCopy(base2.getRecHandle(), base.getRecHandle());
				else {
					for(int n = 0; OnlyFields[n]; n++)
						CopyFieldValue(base2, base, OnlyFields[n]);
				}
				if(!skip)
                {
					int updRes = base.UpdateRecord();
					if(updRes != PXSUCCESS) {
						sprintf(buffer, "%d", updRes);
						MessageBox(GetApplicationObject()->MainWindow->HWindow, PXErrMsg(updRes), buffer, MB_ICONSTOP);
						fclose(f);
						return 22;
					}
					UpdRec++;
				}
			} 
		} 
		else
		{
			PXRecBufCopy(base2.getRecHandle(), base.getRecHandle());
			int addRes = base.AddRecord();
			if(addRes != PXSUCCESS) {
				sprintf(buffer, "%d", addRes);
				MessageBox(GetApplicationObject()->MainWindow->HWindow, PXErrMsg(addRes), buffer, MB_ICONSTOP);
				fclose(f);
				return 24;
            }
			NewRec++;
		}
    }
	sprintf(buffer, "Добавлено %ld, Пропущено %ld, Обновлено %ld, Пропущено по ошибкам %d", NewRec, NoChRec, UpdRec, IgnoreErr);
	MessageBox(GetApplicationObject()->MainWindow->HWindow, buffer, "!", MB_OK | MB_ICONINFORMATION);
	Dlg_base::ShowArray(list);

	return 0;
}
