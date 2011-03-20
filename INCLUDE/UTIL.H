class SelectStr : public TDialog {
	TListBox*    List;
	Array&       array;
        char*        buffer;
public:
		SelectStr(PTWindowsObject p, Array&, char*);

	void	SetupWindow();
	void    Ok(RTMessage Msg);
};

class Process : public TDialog {
TStatic* s1;
TStatic* s2;
TStatic* s_time;
long inittime;
public:
 Process(PTWindowsObject p);
 void SetAll(long);
 void SetPercent(long);
};

