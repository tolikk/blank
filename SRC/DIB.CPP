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
#include "windows.h"

#define MAXREAD         32768L		       /* Number of bytes to be read during */
#define WIDTHBYTES(i)	((i+31)/32*4)
#define PALVERSION      0x300
#define MAXPALETTE	256	  /* max. # supported palette entries */


DWORD PASCAL lread (int fh, void far * pv, DWORD ul)
{
    DWORD     ulT = ul;
    BYTE huge *hp = (BYTE huge *)pv;

    while (ul > (DWORD)MAXREAD)
     {
        if (_lread(fh, (LPSTR)hp, (UINT)MAXREAD) != MAXREAD)  return 0;
        ul -= MAXREAD;
        hp += MAXREAD;
     }

    if (_lread(fh, (LPSTR)hp, (WORD)ul) != (WORD)ul) return 0;
    return ulT;
}

DWORD PASCAL lwrite (int fh, void far * pv, DWORD ul)
{
    DWORD     ulT = ul;
    BYTE huge *hp = (BYTE huge *)pv;

    while (ul > MAXREAD)
     {
        if (_lwrite(fh, (LPSTR)hp, (WORD)MAXREAD) != MAXREAD)  return 0;
        ul -= MAXREAD;
        hp += MAXREAD;
     }

    if (_lwrite(fh, (LPSTR)hp, (WORD)ul) != (WORD)ul) return 0;
    return ulT;
}


WORD DibNumColors (void far *pv)
{
    int                 bits;
    LPBITMAPINFOHEADER  lpbi;
    LPBITMAPCOREHEADER  lpbc;

    lpbi = ((LPBITMAPINFOHEADER)pv);
    lpbc = ((LPBITMAPCOREHEADER)pv);

    /*  With the BITMAPINFO format headers, the size of the palette
     *  is in biClrUsed, whereas in the BITMAPCORE - style headers, it
     *  is dependent on the bits per pixel ( = 2 raised to the power of
     *  bits/pixel).
     */
    if (lpbi->biSize != sizeof(BITMAPCOREHEADER)){
        if (lpbi->biClrUsed != 0)
            return (WORD)lpbi->biClrUsed;
        bits = lpbi->biBitCount;
    }
    else
        bits = lpbc->bcBitCount;

    switch (bits){
        case 1:
                return 2;
        case 4:
                return 16;
        case 8:
                return 256;
        default:
                /* A 24 bitcount DIB has no color table */
                return 0;
    }
}


HPALETTE CreateBIPalette (LPBITMAPINFOHEADER lpbi)
{
    LOGPALETTE          *pPal;
    HPALETTE            hpal = NULL;
    WORD                nNumColors;
    BYTE                red;
    BYTE                green;
    BYTE                blue;
    WORD                i;
    RGBQUAD        FAR *pRgb;

    if (!lpbi)
        return NULL;

    if (lpbi->biSize != sizeof(BITMAPINFOHEADER))
        return NULL;

    /* Get a pointer to the color table and the number of colors in it */
    pRgb = (RGBQUAD FAR *)((LPSTR)lpbi + (WORD)lpbi->biSize);
    nNumColors = DibNumColors(lpbi);

    if (nNumColors){
        /* Allocate for the logical palette structure */
        pPal = (LOGPALETTE*)LocalAlloc(LPTR,sizeof(LOGPALETTE) + nNumColors * sizeof(PALETTEENTRY));
        if (!pPal)
            return NULL;

        pPal->palNumEntries = nNumColors;
        pPal->palVersion    = PALVERSION;

        /* Fill in the palette entries from the DIB color table and
         * create a logical color palette.
         */
        for (i = 0; i < nNumColors; i++){
            pPal->palPalEntry[i].peRed   = pRgb[i].rgbRed;
            pPal->palPalEntry[i].peGreen = pRgb[i].rgbGreen;
            pPal->palPalEntry[i].peBlue  = pRgb[i].rgbBlue;
            pPal->palPalEntry[i].peFlags = (BYTE)0;
        }

        hpal = CreatePalette(pPal);
        LocalFree((HANDLE)pPal);
    }
    else if (lpbi->biBitCount == 24){
        /* A 24 bitcount DIB has no color table entries so, set the number of
         * to the maximum value (256).
         */
        nNumColors = MAXPALETTE;
        pPal = (LOGPALETTE*)LocalAlloc(LPTR,sizeof(LOGPALETTE) + nNumColors * sizeof(PALETTEENTRY));
        if (!pPal)
            return NULL;

        pPal->palNumEntries = nNumColors;
        pPal->palVersion    = PALVERSION;

        red = green = blue = 0;

        /* Generate 256 (= 8*8*4) RGB combinations to fill the palette
         * entries.
         */
        for (i = 0; i < pPal->palNumEntries; i++){
            pPal->palPalEntry[i].peRed   = red;
            pPal->palPalEntry[i].peGreen = green;
            pPal->palPalEntry[i].peBlue  = blue;
            pPal->palPalEntry[i].peFlags = (BYTE)0;

            if (!(red += 32))
                if (!(green += 32))
                    blue += 64;
        }
        hpal = CreatePalette(pPal);
        LocalFree((HANDLE)pPal);
    }
    return hpal;
}


#define BFT_ICON   0x4349   /* 'IC' */
#define BFT_BITMAP 0x4d42   /* 'BM' */
#define BFT_CURSOR 0x5450   /* 'PT' */

#define ISDIB(bft) ((bft) == BFT_BITMAP)



HANDLE ReadDibBitmapInfo (int fh)
{
    DWORD     off;
    HANDLE    hbi = NULL;
    int       size;
    int       i;
    WORD      nNumColors;

    RGBQUAD FAR       *pRgb;
    BITMAPINFOHEADER   bi;
    BITMAPCOREHEADER   bc;
    LPBITMAPINFOHEADER lpbi;
    BITMAPFILEHEADER   bf;
    DWORD              dwWidth = 0;
    DWORD              dwHeight = 0;
    WORD               wPlanes, wBitCount;

    if (fh == -1)
        return NULL;

    /* Reset file pointer and read file header */
    off = _llseek(fh, 0L, SEEK_CUR);
    if (sizeof (bf) != _lread (fh, (LPSTR)&bf, sizeof (bf)))
        return FALSE;

    /* Do we have a RC HEADER? */
    if (!ISDIB (bf.bfType)) {
        bf.bfOffBits = 0L;
        _llseek (fh, off, SEEK_SET);
    }
    if (sizeof (bi) != _lread (fh, (LPSTR)&bi, sizeof(bi)))
        return FALSE;

    nNumColors = DibNumColors (&bi);

    /* Check the nature (BITMAPINFO or BITMAPCORE) of the info. block
     * and extract the field information accordingly. If a BITMAPCOREHEADER,
     * transfer it's field information to a BITMAPINFOHEADER-style block
     */
    switch (size = (int)bi.biSize){
        case sizeof (BITMAPINFOHEADER):
            break;

        case sizeof (BITMAPCOREHEADER):

            bc = *(BITMAPCOREHEADER*)&bi;

            dwWidth   = (DWORD)bc.bcWidth;
            dwHeight  = (DWORD)bc.bcHeight;
            wPlanes   = bc.bcPlanes;
            wBitCount = bc.bcBitCount;

            bi.biSize               = sizeof(BITMAPINFOHEADER);
            bi.biWidth              = dwWidth;
            bi.biHeight             = dwHeight;
            bi.biPlanes             = wPlanes;
            bi.biBitCount           = wBitCount;

            bi.biCompression        = BI_RGB;
            bi.biSizeImage          = 0;
            bi.biXPelsPerMeter      = 0;
            bi.biYPelsPerMeter      = 0;
            bi.biClrUsed            = nNumColors;
            bi.biClrImportant       = nNumColors;

            _llseek (fh, (LONG)sizeof (BITMAPCOREHEADER) - sizeof (BITMAPINFOHEADER), SEEK_CUR);
            break;

        default:
            /* Not a DIB! */
            return NULL;
    }

    /*  Fill in some default values if they are zero */
    if (bi.biSizeImage == 0)
    {
        bi.biSizeImage = WIDTHBYTES ((DWORD)bi.biWidth * bi.biBitCount)
                         * bi.biHeight;
    }
    if (bi.biClrUsed == 0)
        bi.biClrUsed = DibNumColors(&bi);

    /* Allocate for the BITMAPINFO structure and the color table. */
    hbi = GlobalAlloc (GHND, (LONG)bi.biSize + nNumColors * sizeof(RGBQUAD));
    if (!hbi) return NULL;
    lpbi = (LPBITMAPINFOHEADER)GlobalLock (hbi);
    *lpbi = bi;

    /* Get a pointer to the color table */
    pRgb = (RGBQUAD FAR *)((LPSTR)lpbi + bi.biSize);
    if (nNumColors){
        if (size == sizeof(BITMAPCOREHEADER)){
            /* Convert a old color table (3 byte RGBTRIPLEs) to a new
             * color table (4 byte RGBQUADs)
             */
            _lread (fh, (LPSTR)pRgb, nNumColors * sizeof(RGBTRIPLE));

            for (i = nNumColors - 1; i >= 0; i--)
            {
                RGBQUAD rgb;

                rgb.rgbRed      = ((RGBTRIPLE FAR *)pRgb)[i].rgbtRed;
                rgb.rgbBlue     = ((RGBTRIPLE FAR *)pRgb)[i].rgbtBlue;
                rgb.rgbGreen    = ((RGBTRIPLE FAR *)pRgb)[i].rgbtGreen;
                rgb.rgbReserved = (BYTE)0;

                pRgb[i] = rgb;
            }
        }
        else
            _lread(fh,(LPSTR)pRgb,nNumColors * sizeof(RGBQUAD));
    }

    if (bf.bfOffBits != 0L)
        _llseek(fh,off + bf.bfOffBits,SEEK_SET);

    GlobalUnlock(hbi);
    return hbi;
}


BOOL DibInfo (HANDLE hbi, LPBITMAPINFOHEADER lpbi)
{
    if (hbi){
        *lpbi = *(LPBITMAPINFOHEADER)GlobalLock (hbi);

        /* fill in the default fields */
        if (lpbi->biSize != sizeof (BITMAPCOREHEADER)){
            if (lpbi->biSizeImage == 0L)
                lpbi->biSizeImage =
                    WIDTHBYTES(lpbi->biWidth*lpbi->biBitCount) * lpbi->biHeight;

            if (lpbi->biClrUsed == 0L)
                lpbi->biClrUsed = DibNumColors (lpbi);
        }
        GlobalUnlock (hbi);
        return TRUE;
    }
    return FALSE;
}

HPALETTE CreateDibPalette (HANDLE hbi)
{
    HPALETTE hpal;

    if (!hbi) return NULL;
    hpal = CreateBIPalette((LPBITMAPINFOHEADER)GlobalLock(hbi));
    GlobalUnlock(hbi);
    return hpal;
}

WORD PaletteSize (void far * pv)
{
    LPBITMAPINFOHEADER lpbi;
    WORD               NumColors;

    lpbi      = (LPBITMAPINFOHEADER)pv;
    NumColors = DibNumColors(lpbi);

    if (lpbi->biSize == sizeof(BITMAPCOREHEADER))
        return NumColors * sizeof(RGBTRIPLE);
    else
        return NumColors * sizeof(RGBQUAD);
}



HANDLE OpenDIB (LPSTR szFile)
{
    unsigned            fh;
    BITMAPINFOHEADER    bi;
    LPBITMAPINFOHEADER  lpbi;
    DWORD               dwLen = 0;
    DWORD               dwBits;
    HANDLE              hdib;
    HANDLE              h;
    OFSTRUCT            of;

    /* Open the file and read the DIB information */
    fh = OpenFile(szFile, &of, OF_READ);
    if ((signed)fh == -1)
        return NULL;

    hdib = ReadDibBitmapInfo(fh);
    if (!hdib)
        return NULL;
    DibInfo(hdib,&bi);

    /* Calculate the memory needed to hold the DIB */
    dwBits = bi.biSizeImage;
    dwLen  = bi.biSize + (DWORD)PaletteSize (&bi) + dwBits;

    /* Try to increase the size of the bitmap info. buffer to hold the DIB */
    h = GlobalReAlloc(hdib, dwLen, GHND);
    if (!h){
        GlobalFree(hdib);
        hdib = NULL;
    }
    else
        hdib = h;

    /* Read in the bits */
    if (hdib){

        lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib);
        lread(fh, (LPSTR)lpbi + (WORD)lpbi->biSize + PaletteSize(lpbi), dwBits);
        GlobalUnlock(hdib);
    }
    _lclose(fh);

    return hdib;
}

HBITMAP BitmapFromDib (HANDLE hdib, HPALETTE   hpal)
{
    LPBITMAPINFOHEADER  lpbi;
    HPALETTE            hpalT;
    HDC                 hdc;
    HBITMAP             hbm;

    if (!hdib)
        return NULL;

    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib);

    if (!lpbi)
        return NULL;

    hdc = GetDC(NULL);

    if (hpal){
        hpalT = SelectPalette(hdc, hpal, FALSE);
        RealizePalette(hdc);     // GDI Bug...????
    }

    hbm = CreateDIBitmap(hdc,
                (LPBITMAPINFOHEADER)lpbi,
                (LONG)CBM_INIT,
                (LPSTR)lpbi + lpbi->biSize + PaletteSize(lpbi),
                (LPBITMAPINFO)lpbi,
                DIB_RGB_COLORS );

    if (hpal)
        SelectPalette(hdc, hpalT, FALSE);

    ReleaseDC(NULL,hdc);
    GlobalUnlock(hdib);

    return hbm;
}


HBITMAP ReadBitMap(char * fileName )
{
    unsigned           fh;
    LPBITMAPINFOHEADER lpbi;
    WORD FAR *         pw;
    int                i;
    BITMAPINFOHEADER   bi;
    OFSTRUCT           of;
    HANDLE	       hbiCurrent;   /* Handle to current bitmap info struct    */
    HPALETTE           hpalCurrent;  /* Handle to current palette	       */
    HBITMAP	       hbmCurrent;   /* Handle to current memory BITMAP	       */
    HANDLE	       hdibCurrent;


    fh = OpenFile (fileName, (LPOFSTRUCT)&of, OF_READ);
    if (fh == -1)
        return NULL;

    hbiCurrent = ReadDibBitmapInfo(fh);

    _lclose (fh);

    if (hbiCurrent == NULL)
           return NULL;

    DibInfo(hbiCurrent, &bi);

    hpalCurrent = CreateDibPalette(hbiCurrent);

    if (hpalCurrent == NULL)
    {
       if ( hbiCurrent )
          GlobalFree(hbiCurrent);
       return NULL;
    }

    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hbiCurrent);
    if (lpbi->biBitCount != 24)
    {
        pw = (WORD FAR *)((LPSTR)lpbi + lpbi->biSize);

        for (i=0; i<(int)lpbi->biClrUsed; i++)
            *pw++ = (WORD)i;
    }
    GlobalUnlock(hbiCurrent);

    hdibCurrent = OpenDIB(fileName);

    if (hdibCurrent )
    {
        hbmCurrent = BitmapFromDib(hdibCurrent,hpalCurrent);

        if ( hpalCurrent )
          DeleteObject(hpalCurrent);

        if ( hdibCurrent )
          GlobalFree(hdibCurrent);

        if ( hbiCurrent )
          GlobalFree(hbiCurrent);

        return hbmCurrent;
    }
    else
    {
       if ( hpalCurrent )
         DeleteObject(hpalCurrent);

        if ( hdibCurrent )
          GlobalFree(hdibCurrent);

        if ( hbiCurrent )
          GlobalFree(hbiCurrent);
    }

   return NULL;
}