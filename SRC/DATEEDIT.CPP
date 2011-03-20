#include <dateedit.h>

	int TDateEdit::GetParts(int& pos1, int& pos2)
	{
		int StartPos = 0, EndPos = 0;
		GetSelection(StartPos, EndPos);

		char buff[32];
		GetWindowText(HWindow, buff, sizeof buff);

		char* p2 = 0;
		char* p1 = strchr(buff, '.');
		if(p1) p2 = strchr(p1 + 1 , '.');

		if(p1 && p2)
		{
			pos1 = p1 - buff;
			pos2 = p2 - buff;
			if(StartPos <= pos1 && EndPos <= pos1) return 0;
			if(StartPos > pos2 && EndPos > pos2) return 2;
			if(StartPos > pos1 && EndPos > pos1 && StartPos <= pos2 && EndPos <= pos2) return 1;
		}

        return -1;
	}

	void TDateEdit::WMLButtonDown(RTMessage msg)
	{
		DefWndProc(msg);

		int pos1 = 0, pos2 = 0;
		int Part = GetParts(pos1, pos2);
		if(Part != -1)
		{
				if(Part == 0)
				{
					SetSelection(0, pos1);
				}
				if(Part == 1)
				{
					SetSelection(pos1+1, pos2);
				}
				if(Part == 2)
				{
					SetSelection(pos2+1, GetWindowTextLength(HWindow));
				}
		}
	}

	void TDateEdit::WMKeyDown(RTMessage msg) 
	{
		int pos1 = 0, pos2 = 0;
		int Part = GetParts(pos1, pos2);
		if(Part != -1)
		{
			if(msg.WParam == VK_RIGHT)
			{
				if(Part == 0)
				{
					SetSelection(pos1+1, pos2);
					return;
				}
				if(Part == 1)
				{
					SetSelection(pos2+1, GetWindowTextLength(HWindow));
					return;
				}
			}
			if(msg.WParam == VK_LEFT)
			{
				if(Part == 2)
				{
					SetSelection(pos1+1, pos2);
					return;
				}
				if(Part == 1)
				{
					SetSelection(0, pos1);
					return;
				}
			}
		}
		DefWndProc(msg);
	}

	int TDateEdit::IsDivider(WORD Param)
	{
		return Param == 254 || Param == 46 || Param == '/';
    }

	void TDateEdit::WMChar(RTMessage msg) 
	{
		char buff[32];
		int StartPos, EndPos;
		if(!isDigit(msg.WParam) && !IsDivider(msg.WParam) && msg.WParam != VK_BACK) return;
		if(IsDivider(msg.WParam) && GetWindowTextLength(HWindow) == 0) return;
		GetWindowText(HWindow, buff, sizeof buff);
		for(int i = 0; i < strlen(buff); i++) buff[i] = buff[i] == '/' ? '.' : buff[i];
		GetSelection(StartPos, EndPos);
		if(StartPos == EndPos && StartPos == strlen(buff) && strlen(buff) > 0 && IsDivider(msg.WParam) && buff[strlen(buff)-1] == '.') return;

		DefWndProc(msg);
		if(isDigit(msg.WParam))
        {
			GetWindowText(HWindow, buff, sizeof buff);
			for(int i = 0; i < strlen(buff); i++) buff[i] = buff[i] == '/' ? '.' : buff[i];
			GetSelection(StartPos, EndPos);
			char* p1 = strchr(buff, '.');
			if(StartPos == EndPos && StartPos == strlen(buff) && strlen(buff) > 0)
			{
				//first divider
				if(p1 == 0 && (strlen(buff) == 2 || strlen(buff) == 1 && atoi(buff) > 3))
				{
					if(atoi(buff) > 31) strcpy(buff, "31");
					strcat(buff, ".");
					SetWindowText(HWindow, buff);
					SetSelection(strlen(buff), strlen(buff));
				}
				if(p1)
				{
					char* p2 = strchr(p1+1, '.');
					if(p2 == 0 && (strlen(p1+1) == 2 || strlen(p1+1) == 1 && atoi(p1+1) > 1))
					{
						if(atoi(p1+1) > 12) strcpy(p1+1, "12");
						strcat(buff, ".");
						SetWindowText(HWindow, buff);
						SetSelection(strlen(buff), strlen(buff));
						return;
					}
				}
			}
			else
			{
				//Edit date mode
				int pos1 = 0, pos2 = 0;
				int Part = GetParts(pos1, pos2);
				if(Part == 0)
				{
					buff[pos1] = 0;
					if(atoi(buff) > 3 || strlen(buff) == 2)
					{
						SetSelection(pos1 + 1, pos2);
						return;
					}
				}
				if(Part == 1)
				{
					buff[pos2] = 0;
					if(atoi(buff+pos1+1) > 1 || strlen(buff+pos1+1) == 2)
					{
						SetSelection(pos2 + 1, GetWindowTextLength(HWindow));
						return;
					}
				}
			}
		}
	}

	void TDateEdit::Normalize(char* buff)
	{
		for(int i = 0; i < strlen(buff); i++)
		{
			if(!isDigit(buff[i]))
			{
				buff[i] = '.';
			}
		}
		int d = 0, m = 0, y = 0;
		sscanf(buff, "%d.%d.%d", &d, &m, &y);
		*buff = 0;
		if(d+m+y>0)
		{
        	struct date dt; getdate(&dt);
			if(y == 0) y = dt.da_year;
			if(m == 0) m = dt.da_mon;
			if(y < 20) y += 2000;
			sprintf(buff, "%02d.%02d.%04d", d, m, y);
        }
	}

	void TDateEdit::WMKillFocus(RTMessage msg) 
	{
		char buff[32];
		GetWindowText(HWindow, buff, sizeof buff);
		Normalize(buff);
		SetWindowText(HWindow, buff);
        DefWndProc(msg);
	}

	void TDateEdit::WMSetFocus(RTMessage msg)
	{
		char buff[32];
		GetWindowText(HWindow, buff, sizeof buff);
		char* ch = strchr(buff, '.');
		if(ch)
		{
			SetSelection(0, ch - buff);
		}

		DefWndProc(msg);
	}

	int TDateEdit::isDigit(char c)
	{
    	return c >= '0' && c <= '9';
    }



	void TTimeEdit::WMChar(RTMessage msg)
	{
		int IsDigit = msg.WParam >= '0' && msg.WParam <= '9';
		if(IsDigit || msg.WParam == ':' || msg.WParam == VK_BACK)
		{
			DefWndProc(msg);
			if(IsDigit)
            {
				char buff[10];
				GetWindowText(HWindow, buff, sizeof buff);
				if(strlen(buff) == 2 || atoi(buff) > 2 && strchr(buff, ':') == 0)
				{
					strcat(buff, ":");
					SetWindowText(HWindow, buff);
					SetSelection(strlen(buff), strlen(buff));
				}
        	}
		}
	}

	void TTimeEdit::WMKillFocus(RTMessage msg)
	{
    	DefWndProc(msg);
		char buff[10];
		GetWindowText(HWindow, buff, sizeof buff);
		char* ch = strchr(buff, ':');
		if(ch == 0 && strlen(buff))
		{
			if(strlen(buff) == 1)
			{
            	strcat(buff, ":00");
			}
			if(strlen(buff) == 2)
			{
				if(*buff > '2')
				{
                	buff[3] = 0;
					buff[2] = buff[1];
					buff[1] = ':';
				}
				else
				{
					strcat(buff, ":00");
				}
            }
			SetWindowText(HWindow, buff);
		}
	}
