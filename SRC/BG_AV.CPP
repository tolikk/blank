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
#include <openbase.h>
#include <array.h>
#include <grncard.h>
#include <stdio.h>
#include <string.h>
#include <ruler.h>
#include <window.h>
#include <commdlg.h>
#include <belgreen.h>
#include "help.h"
#include "dlg_base.h"
             /*

BELGREEN_AV::BELGREEN_AV(PTWindowsObject o)
	   : TDialog(o, "BG_IE")
{
    memset(&buffer, 0, sizeof (buffer));
    TransferBuffer = &buffer;

    NWork_Insurer = new TEdit(this, 101, 11);
    NWork_Buro = new TEdit(this, 102, 11);
    Date_IE = new TEdit(this, 103, 11);
    Date_Ask_Country = new TEdit(this, 104, 11);
    Date_Answ_Ask = new TEdit(this, 105, 11);
    Date_Say_BBTC = new TEdit(this, 106, 11);
    Date_Ask_Bad = new TEdit(this, 107, 11);
    Date_Answ_Bad = new TEdit(this, 108, 11);

    Country = new TEdit(this, 109, 16);
    InfoSource = new TEdit(this, 110, 21);
    ClaimSumma = new NumberEdit(this, 111, 11, 1);

    ClaimAsk = new TEdit(this, 112, 21);
    ClaimNumber = new TEdit(this, 113, 11);
    ClaimDateAsk = new TEdit(this, 114, 11);
    ClaimDateGet = new TEdit(this, 115, 11);
    ClaimDateBBTC = new TEdit(this, 116, 11);
    ClaimPayPlus = new TEdit(this, 117, 11);
    ClaimPay = new NumberEdit(this, 118, 11, 1);

    PayDateOnClaim = new TEdit(this, 119, 11);
    PayDate_NotifBBTC = new TEdit(this, 120, 11);
    PaySumm = new TEdit(this, 121, 11);
    DocDate = new TEdit(this, 122, 11);
    DocNumber = new TEdit(this, 123, 11);

    ClaimCurr = new TComboBox(this, 124, 4);
    ClaimCurr->DisableTransfer();
    RetType = new TComboBox(this, 125, 1);
    RetType->DisableTransfer();
    ClaimPayCurr = new TComboBox(this, 126, 4);
    ClaimPayCurr->DisableTransfer();
    PayCurr = new TComboBox(this, 127, 4);
    PayCurr->DisableTransfer();
}

void AlignCombo(TComboBox*);
void InitVal(TComboBox* cb);
void InitVal2(TComboBox* cb);

void BELGREEN_AV::SetupWindow()
{
    TDialog::SetupWindow();

    InitVal(ClaimCurr);
    ClaimCurr->DeleteString(ClaimCurr->FindExactString("DM", -1));
    InitVal2(ClaimPayCurr);
    ClaimPayCurr->DeleteString(ClaimPayCurr->FindExactString("DM", -1));
    InitVal2(PayCurr);
    PayCurr->DeleteString(PayCurr->FindExactString("DM", -1));
    AlignCombo(ClaimCurr);
    AlignCombo(RetType);
    AlignCombo(ClaimPayCurr);
}

void BELGREEN_AV::Ok(RTMessage msg)
{
    TDialog::Ok(msg);	
}
	       */