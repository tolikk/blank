#include "combobox.h"
#include "edit.h"
#include "dlg_base.h"

class TSetRange : public TDialog
{
	TComboBox* m_Agent;
	TComboBox* m_Seria;
	TEdit* m_From;
	TEdit* m_To;
public:
	TSetRange(PTWindowsObject AParent);
	void SetupWindow();
	void Ok(RTMessage msg);
};

extern PolisRange tempRange;
