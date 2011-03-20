#ifndef _GETDATE__H
#define _GETDATE__H

#include <edit.h>
#include <combobox.h>

struct Priod_Params {
	int m, y, dm;
	char filename[256];
	void* data;
};

class getDate : public TDialog
{
	int     *m, *y, *dm;
	char* file;

    PTComboBox  e1;
	PTComboBox  e1_1;
	PTEdit      e2;
	PTEdit      e3;

public:
		  getDate(PTWindowsObject, int*, int*, int*, char*);

    void  SetupWindow();
    void  Ok(RTMessage);

	void  SelectFile() = [ID_FIRST + 104];
};

#endif