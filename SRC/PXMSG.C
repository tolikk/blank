#include <stdlib.h>
#include <string.h>
#ifdef WINDOWS
#include <windows.h>
#endif
#include "pxengine.h"


typedef struct
{
   int code;
   char far *msg;
} Message;

Message KnownMessages[] = {
   {PXSUCCESS,             "No error"},
   {PXERR_DRIVENOTREADY,   "Drive not ready"},
   {PXERR_DIRNOTFOUND,     "Directory not found"},
   {PXERR_FILEBUSY,        "File is busy"},
   {PXERR_FILELOCKED,      "File is locked"},
   {PXERR_FILENOTFOUND,    "Could not find file"},
   {PXERR_TABLECORRUPTED,  "Таблица разрушена"},
   {PXERR_XCORRUPTED,      "Первичный индекс разрушен"},
   {PXERR_XOUTOFDATE,      "Primary index is out of date"},
   {PXERR_RECLOCKED,       "Запись блокирована"},
   {PXERR_DIRBUSY,         "Sharing violation"},
   {PXERR_DIRLOCKED,       "Sharing violation"},
   {PXERR_DIRNOACCESS,     "No access to directory"},
   {PXERR_XSORTVERSION,    "Sort for index different from table"},
   {PXERR_DIRNOTPRIVATE,   ""},//Single user but directory is shared"},
   {PXERR_NETMULTIPLE,     "Multiple Paradox net files found"},
   {PXERR_TOOMANYPASSW,    ""},//Too many passwords specified"},
   {PXERR_INSUFRIGHTS,     ""},//Insufficient password rights"},
   {PXERR_TABLEWRITEPRO,   "Защита от записи"},
   {PXERR_INVPASSW,        "Пароль?"},
   {PXERR_TYPEMISMATCH,    ""},//"Data type mismatch"},
   {PXERR_OUTOFRANGE,      "Argument is out of range"},
   {PXERR_BUFTOOSMALL,     ""},//"Buffer is too small for result"},
   {PXERR_INVPARAMETER,    "Invalid argument"},
   {PXERR_TABLEBUSY,       "Table is busy"},
   {PXERR_TABLELOCKED,     "Таблица блокирована"},
   {PXERR_OUTOFMEM,        "Нет памяти для операции"},
   {PXERR_OUTOFDISK,       "Нет места на диске"},
   {PXERR_TABLENOTFOUND,   "Таблица не найдена"},
   {PXERR_SXNOTFOUND,      ""},//"Secondary index was not found"},
   {PXERR_RECDELETED,      ""},//"Another user deleted record"},
   {PXERR_SXCORRUPTED,     ""},//"Secondary index is corrupted"},
   {PXERR_DISKWRITEPRO,    "Диск защищен от записи"},
   {PXERR_RECTOOBIG,       ""},//Record too big"},
   {PXERR_GENERALFAILURE,  ""},//Фатальная ошибка"},
   {PXERR_OUTOFSTACK,      "Нет стека для операции"},
   {PXERR_OUTOFFILEHANDLES,"Увеличте параметр FILES"},
   {PXERR_TABLEFULL,       "Таблица переполнена"},
   {PXERR_OUTOFTABLEHANDLES,"No more table handles available"},
   {PXERR_INVDATE,         ""},//"Invalid date given"},
   {PXERR_INVFIELDNAME,    "Не то имя поля"},
   {PXERR_INVFIELDHANDLE,  "Invalid field handle"},
   {PXERR_INVTABLEHANDLE,  "Invalid table handle"},
   {PXERR_OUTOFSWAPBUF,    ""},//"Not enough swap buffer space to complete operation"},
   {PXERR_NOTINITERR,      "Engine not initialized"},
   {PXERR_INVENGINESTATE,  "Была фатальная ошибка"},
   {PXERR_TABLESQL,        ""},//"Table is SQL replica"},
   {PXERR_STRUCTDIFFER,    "Table structures are different"},
   {PXERR_ALREADYINIT,     ""},//"Engine already initialized"},
   {PXERR_TABLEOPEN,       "Unable to perform operation on open table"},
   {PXERR_TOOMANYCLIENTS,  ""},//"Too many clients for the Engine DLL"},
   {PXERR_EXCEEDSCONFIGLIMITS, ""},//"Exceeds limits specified in WIN.INI"},
   {PXERR_NOMORETMPNAMES,  ""},//"No more temporary names available"},
   {PXERR_CANTREMAPFILEHANDLE, ""},//"No more slots for file handle remapping"},
   {PXERR_CANTSHAREPDOXNET,"Can't share Paradox net file -- is SHARE.EXE loaded?"},
   {PXERR_RECNOTFOUND,     "Запись не найдена"},
   {PXERR_WINDOWSREALMODE, ""},//"Can't run Engine in Windows real mode"},
   {PXERR_SXCANTUPDATE,    "Can't modify table opened on non-maintained secondary index"},
   {PXERR_TABLEPRE40,      "Feature not available for tables older than Paradox 4.0"},
   {PXERR_TABLEINDEXED,    "Table is indexed"},
   {PXERR_TABLENOTINDEXED, "Table is not indexed"},
   {PXERR_SXOUTOFDATE,     "Secondary index is out of date"},
   {PXERR_KEYVIOL,         "Key violation"},
   {PXERR_NOTLOGGEDIN,     ""},//"Could not login on network"},
   {PXERR_INVTABLENAME,    ""},//"Table name is invalid"},
   {PXERR_INUSEBYPDOX35,   ""},//"Directory is in use by Paradox 3.5"},
   {PXERR_ENDOFTABLE,      "End of table"},
   {PXERR_STARTOFTABLE,    "Start of table"},
   {PXERR_OUTOFRECHANDLES, ""},//"No more record handles available"},
   {PXERR_INVRECHANDLE,    "Invalid record handle"},
   {PXERR_TABLEEMPTY,      "Operation on empty table"},
   {PXERR_INVLOCKCODE,     "Invalid lock code" },
   {PXERR_NONETINIT,       ""},//"Engine not initialized with PXNetInit"},
   {PXERR_INVFILENAME,     "Invalid file name"},
   {PXERR_INVUNLOCK,       "Invalid unlock"},
   {PXERR_INVLOCKHANDLE,   "Invalid lock handle"},
   {PXERR_OUTOFLOCKHANDLES,"Too many locks on table"},
   {PXERR_INVSORTORDER,    "Invalid sort order table"},
   {PXERR_INVNETTYPE,      "Invalid net type"},
   {PXERR_INVDIRNAME,      "Invalid directory name"},
   {PXERR_BLOBMODE,        ""},//"Operation N/A for Blob open mode"},
   {PXERR_BLOBOPEN,        ""},//"Blob already open"},
   {PXERR_BLOBINVOFFSET,   ""},//"Invalid offset into Blob"},
   {PXERR_BLOBINVSIZE,     ""},//"Invalid size for Blob"},
   {PXERR_BLOBMODIFIED,    ""},//"Another user modified Blob"},
   {PXERR_BLOBCORRUPT,     ""},//"Blob file corrupted"},
   {PXERR_BLOBNOINDEX,     ""},//"Cannot index on a Blob"},
   {PXERR_BLOBINVHANDLE,   ""},//"Invalid Blob handle"},
   {PXERR_BLOBNOSEARCH,    ""},//"Cannot search on a Blob field"},
   {PXERR_CANTUPGRADE,     ""},//Can't handle version number encountered"},
   {PXERR_LOCKTIMEOUT,     "Timed out trying to get a lock"}
};
#define KnownMsgSize ( sizeof(KnownMessages) / sizeof(KnownMessages[0]) )


// internal error >200
//
char far *interror = "ЋиЁЎЄ  N: ";
char far *undef    = "ЌҐЁ§ўҐбв­ п ®иЁЎЄ ";


static char errMsg[128];         // area in which we store the message



// returns the error message string for the given error code
//
char far * pascal far PXErrMsg(int errCode)
{
   int i;
   char far *pOemStr = NULL;
   static char buff[7];
   sprintf(buff, "%d", errCode);

   for( i=0 ; i<KnownMsgSize ; i++ )
   {
      if( KnownMessages[i].code == errCode )
      {
		 pOemStr = *KnownMessages[i].msg ? KnownMessages[i].msg : buff;
         break;
      }
   }

   if( !pOemStr )
   {
      if( errCode>=200 && errCode<=250 )
      {
#ifdef WINDOWS
         lstrcpy((LPSTR)errMsg, (LPSTR)interror);
#else
         strcpy(errMsg, interror);
#endif
         itoa(errCode, &errMsg[strlen(errMsg)], 10);
         pOemStr = errMsg;
      }
      else
         pOemStr = undef;
   }
#ifdef WINDOWS
   OemToAnsi(pOemStr, errMsg);     // note: this will work "in place"
   return errMsg;
#else // DOS
   return pOemStr;
#endif // DOS
}
