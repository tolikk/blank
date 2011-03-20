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
#include <bmpsize.h>
#include <stdio.h>
#include <string.h>
#include <page.h>
#include <stdio.h>
#include <stdlib.h>

extern int PageOffX;
extern int PageOffY;

Page::Page(PTWindowsObject p)
     :TDialog(p, "PAGE")
{
    a = new TEdit(this, 101, 5);
    b = new TEdit(this, 102, 5);
}

void Page::SetupWindow()
{
    TDialog::SetupWindow();
    char fullName[100];

    GetModuleFileName(GetModule()->hInstance, fullName, sizeof fullName);
    *strrchr(fullName, '.') = 0;
    strcat(fullName, ".cnf");
    FILE* f = fopen(fullName, "rt");
    if(f)
     {
        fscanf(f, "%d %d", &PageOffX, &PageOffY);
        fclose(f);
     }

    sprintf(fullName, "%d", PageOffX);
    a->SetText(fullName);
    sprintf(fullName, "%d", PageOffY);
    b->SetText(fullName);
}

void Page::Ok(RTMessage m)
{
    char fullName[100];

    a->GetText(fullName, sizeof fullName);
    PageOffX = atoi(fullName);

    b->GetText(fullName, sizeof fullName);
    PageOffY = atoi(fullName);

    GetModuleFileName(GetModule()->hInstance, fullName, sizeof fullName);
    *strrchr(fullName, '.') = 0;
    strcat(fullName, ".cnf");
    FILE* f = fopen(fullName, "wt");
    fprintf(f, "%d %d", PageOffX, PageOffY);
    fclose(f);

    TDialog::Ok(m);
}

////////////////////////////////////////////////////////////////////////////

OffBlank::OffBlank(PTWindowsObject p, void* buff)
         :Page(p)
{
    TransferBuffer = buff;
}

void OffBlank::SetupWindow()
{
    TDialog::SetupWindow();

#define Text "сколько сместить все поля."

    SetCaption("Сместить все поля");
    SendDlgItemMessage(HWindow, 50, WM_SETTEXT, 0, (long)Text);
/*
    a->SetText("0");
    b->SetText("0");*/
}

void OffBlank::Ok(RTMessage m)
{
/*    char fullName[100];

    a->GetText(fullName, sizeof fullName);
    PageOffX = atoi(fullName);

    b->GetText(fullName, sizeof fullName);
    PageOffY = atoi(fullName);

    GetModuleFileName(GetModule()->hInstance, fullName, sizeof fullName);
    *strrchr(fullName, '.') = 0;
    strcat(fullName, ".cnf");
    FILE* f = fopen(fullName, "wt");
    fprintf(f, "%d %d", PageOffX, PageOffY);
    fclose(f); */

    TDialog::Ok(m);
}
