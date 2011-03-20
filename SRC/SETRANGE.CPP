#include <owl.h>
#include <setrange.h>
#include <grncard.h>
#include "editors.h"

PolisRange tempRange;
void _FillAgents(TOpenBase* agents, TComboBox* AgName, int index);
void FillPlacement(TComboBox* PlacementCombo, const char* Key, const char* = 0, int = 1);
void AlignCombo(TComboBox*);
extern const char* MANDATORY_SECT;
void SetAgent(char* ACode, TOpenBase& agents, PTComboBox AgName, int);
void DblToStr(char* str, double V, int Digit = 2);
void SetAgent(char* ACode, TOpenBase& agents, PTComboBox AgName, int);

TSetRange::TSetRange(PTWindowsObject AParent)
	: TDialog(AParent, "SET_RANGE")
{
	m_Agent = new TComboBox(this, 101, 1);
	m_Seria = new TComboBox(this, 102, 1);
	m_From = new NumberEdit(this, 103, 10, 0);
	m_To = new NumberEdit(this, 104, 10, 0);
}

void TSetRange::SetupWindow()
{
	TDialog::SetupWindow();
	TOpenBase agents(AgentDBName, AgentFields, AgFieldsCount);
	_FillAgents(&agents, m_Agent, -1);
	SetAgent(tempRange.Agent, agents, m_Agent, 0);
	AlignCombo(m_Agent);
	char buff[15];
	FillPlacement(m_Seria, "Series%d", (char*)MANDATORY_SECT);
	if(GetPrivateProfileString("BELGREEN", "DefSeria", "", buff, sizeof buff, ::ININame) > 0)
	{
		m_Seria->AddString(buff);
	}
	AlignCombo(m_Seria);
	DblToStr(buff, tempRange.From, 0);
	m_From->SetText(buff);
	DblToStr(buff, tempRange.To, 0);
	m_To->SetText(buff);
	m_Seria->SetText(tempRange.Seria);
	SetAgent(tempRange.Agent, agents, m_Agent, 0);
}

void TSetRange::Ok(RTMessage msg)
{
	if(m_Agent->GetSelIndex() < 0)
	{
		SetFocus(m_Agent->HWindow);
		return;
	}
    long ag_code = SendMessage(m_Agent->HWindow, CB_GETITEMDATA, m_Agent->GetSelIndex(), 0);
	memmove(tempRange.Agent, &ag_code, 4);
	tempRange.Agent[4] = 0;

	m_Seria->GetText(tempRange.Seria, sizeof tempRange.Seria);
	char buff[15];
	m_From->GetText(buff, sizeof buff);
	tempRange.From = atof(buff);
	m_To->GetText(buff, sizeof buff);
	tempRange.To = atof(buff);
	if(tempRange.To < tempRange.From || tempRange.From - tempRange.To > 1000)
	{
		SetFocus(m_From->HWindow);
		return;
	}
	TDialog::Ok(msg);
}
