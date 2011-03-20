#ifndef __IMPEXP_H
#define __IMPEXP_H

#include "listbox.h"
#include "checkbox.h"
#include "combobox.h"

extern char* AgentFields[14];
extern char* AgentDBName;
typedef int (*ImpExpCallback)(TOpenBase* FileDb, TOpenBase* Db, void* Data);
const char* GetRCStr(int strID);
long ExportData(char* TableName, char** Fields, int FieldsCount, ImpExpCallback func, void* Data, char* filename, int m, int y, int dm, int AgField, int IgnoreCondField);
long ImportData(char* TableName, char** Fields, int FieldsCount, ImpExpCallback func, void* Data, int AgField, int StateField, int BadState, char*, char*);
int ReadAgents(FILE* f, Array& AgentsCodes, Array& AgentsNames, Array& NewAgentsCodes);

class LinkAgDlg : public TDialog {
    TListBox* m_lb;
    TCheckBox* m_cb;
    TListBox* m_combo;
public:
    Array* AgCodesPtr;
    Array* NewAgCodesPtr;
    Array* AgNamesPtr;
    char* Msg;
    int* pIgnoreNames;

    int PrevIndex;

    LinkAgDlg(PTWindowsObject A);
    void SetupWindow();
    void Ok(RTMessage);

	void SetComboCode(char*);
    void SetByCode(RTMessage msg) = [ID_FIRST + 120];
    void SaveLinks(RTMessage msg) = [ID_FIRST + 102];
    void LoadLinks(RTMessage msg) = [ID_FIRST + 103];
    void AllEq(RTMessage msg) = [ID_FIRST + 104];
	int  IsValid();

    void ChLb(RTMessage msg) = [ID_FIRST + 101];
    void ChLbIndex();
};

#endif