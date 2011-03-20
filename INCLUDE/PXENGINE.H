// pxengine.h
//    Copyright (c) 1991 by Borland International, Inc.

#ifndef PXENGINE_H
#  define PXENGINE_H

#ifdef __cplusplus
   extern "C" {                     // avoid name-mangling if used from C++
#endif // __cplusplus


#define InternalVersion 0x0300000FL // version number, same as in .lib files

// Type definitions

typedef int       PXCODE;                 // engine call return code
typedef unsigned  TABLEHANDLE;            // table handle
typedef unsigned  RECORDHANDLE;           // record handle
typedef unsigned  FIELDHANDLE;            // field number 1..
typedef int       LOCKHANDLE;             // lock handle
typedef int       BLOBHANDLE;
typedef long      RECORDNUMBER;           // record number 1..
typedef long      TDATE;                  // representation of date
typedef long      DATE;                   // representation of date (old)
                                          // (provided for backwards compat.)
typedef TABLEHANDLE  far * PTABLEHANDLE;  // pointer to table handle
typedef RECORDHANDLE far * PRECORDHANDLE; // pointer to record handle
typedef FIELDHANDLE  far * PFIELDHANDLE;  // pointer to field handle
typedef LOCKHANDLE   far * PLOCKHANDLE;   // pointer to lockhandle
typedef RECORDNUMBER far * PRECORDNUMBER; // pointer to record number
typedef TDATE        far * PDATE;         // pointer to a date


// commonly used constants

#ifndef TRUE
#  define TRUE    1
#endif // TRUE

#ifndef FALSE
#  define FALSE   0
#endif // FALSE

#ifndef NULL
#  define NULL  0L                
#endif // NULL


// Maximum and default values for dynamic configuration.
// The default values are used if not overwritten in PXSetDefaults (DOS)
// or in WIN.INI (Windows)

#define PXDEFAULT             0L // use internal default value
#define MAXTABLEHANDLES      64  // maximum number of tables open at a time
#define DEFTABLEHANDLES       5  // default number of tables open at a time
#define MAXRECORDHANDLES    512  // maximum number of record buffers available
#define DEFRECORDHANDLES     10  // default number of record buffres available
#define MAXLOCKHANDLES      128  // maximum number of lock handles per table
#define DEFLOCKHANDLES       32  // default number of lock handles per table
#define MAXFILEHANDLES      255  // maximum number of DOS file handles to use
#define MINFILEHANDLES        3  // minimum number of DOS file handles to use
#define DEFFILEHANDLES       10  // default number of DOS file handles to use

#ifdef _Windows            // set by -W flag on bcc
#  ifndef WINDOWS
#    define WINDOWS
#    define _PX_DEFED_WINDOWS
#  endif // !WINDOWS
#endif // _Windows

#ifndef PXFUNC
#  ifdef WINDOWS
#     define PXFUNC  PXCODE far pascal _export
#  else // DOS
#     define PXFUNC  PXCODE far pascal
#  endif // DOS
#endif // PXFUNC

#ifdef WINDOWS
#  define DEFSORTORDER   (char) (0)          // default sort order (ASCII)
#else // DOS
#  define DEFSORTORDER   (char far *) (0)    // default sort order (ASCII)
#endif // DOS

// Swap buffer size
#define MAXSWAPSIZE   256  // maximum buffer size allowed (k)
#define MINSWAPSIZE     8  // minimum buffer size allowed (k)
#define DEFSWAPSIZE    32  // default buffer size (k)


// Network types used in PXNetInit
// The type is made up of two sections: locking protocol, and a sharing
// specification indicating which tables are vulnerable to sharing and need
// to be protected with the locking protocol.
// The locking protocol and the sharing specification must be or'ed together.

//   locking protocol - choose one of the following (PX40LOCKING is default)
#define PX35LOCKING    64  // locking protocol compatible with Paradox 3.5
#define PX40LOCKING     0  // locking protocol compatible with Paradox 4.0

//   sharing specification - choose one of the following:
#define LOCALSHARE     32  // all tables can be shared (local and remote)
                           // e.g. from another Window or DOS shell. locking
                           // is needed on all tables
#define NETSHARE        2  // only tables on the net can be shared, tables
                           // on the local drive can be treated as private
#define NOSHARE         1  // all tables can be treated as private, no locking
                           // is needed - behave like PXInit

// The following network codes are provided strictly for compatibility with
// the Paradox Engine 2.0; it is no longer necessary to be explicit as to
// which network you are running on:
#define NOTONNET        1  // not on a network
#define NOVELLNET       2  // Novell
#define THREECOMNET     3  // 3Com
#define THREEOPENNET    4  // 3Com 3+Open
#define OTHERNET        5  // other: unknown DOS 3.1 compatible
#define STARLANNET      7  // Starlan
#define BANYANNET       8  // Banyan
#define IBMPCNET        9  // IBM PC




#define DEFUSERNAME  NULL  // use default username in PXNetInit


// used in PXKeyAdd

#define PRIMARY        0   // primary index (key)
#define SECONDARY      1   // not maintained secondary index
#define INCSECONDARY   2   // maintained secondary index


// used in PXSrchFld, PXSrchKey

#define SEARCHFIRST    0   // search from beginning of table
#define SEARCHNEXT     1   // search from next record in table
#define CLOSESTRECORD  2   // search for 'nearest' record if no match found


// Lock types (used in PXNetFileLock/Unlock, PXNetTblLock/Unlock)

#define FL             1   // full lock, no concurrency
#define WL             2   // write lock
#define PWL            3   // prevent write lock
#define PFL            4   // prevent full lock, full concurrency


// used in functions operating on Blobs (Binary Large OBjects)

#define PXBLOBREJECT 0     // for use in PXBlobClose
#define PXBLOBACCEPT 1     // for use in PXBlobClose
#define PXBLOBNEW    0     // for use in PXBlobOpenWrite
#define PXBLOBCOPY   1     // for use in PXBlobOpenWrite


// macros for checking blank values

#define BLANKDATE       0x80000000L
#define BLANKLONG       0x80000000L
#define BLANKSHORT      0x8000

#define ISBLANKALPHA(x)  ((x)[0]                == 0x0)
#define ISBLANKSHORT(x)  ((unsigned short)(x)   == BLANKSHORT)
#define ISBLANKLONG(x)   ((unsigned long)(x)    == BLANKLONG)
#define ISBLANKDATE(x)   ((unsigned long)(x)    == BLANKDATE)


// successful Engine function operation returns

#define PXSUCCESS    0  


// errorcodes from Engine functions


// initialization errors

#define PXERR_NOTINITERR        78   // Engine not initialized
#define PXERR_ALREADYINIT       82   // Engine already initialized
#define PXERR_NOTLOGGEDIN       98   // Could not login on network (to PARADOX.NET)
#define PXERR_NONETINIT        107   // Engine not initialized with PXNetInit
#define PXERR_NETMULTIPLE       15   // multiple PARADOX.NET files
#define PXERR_CANTSHAREPDOXNET 134   // can't lock PARADOX.NET -- is SHARE.EXE loaded?
#define PXERR_WINDOWSREALMODE  135   // can't run Engine in Windows real mode


// hardware related errors

#define PXERR_DRIVENOTREADY     1    // Drive not ready
#define PXERR_DISKWRITEPRO      124  // Disk is write protected
#define PXERR_GENERALFAILURE    126  // General hardware error


// directory reg error codes

#define PXERR_DIRNOTFOUND       2    // Directory not found
#define PXERR_DIRBUSY           10   // Sharing violation
#define PXERR_DIRLOCKED         11   // Sharing violation
#define PXERR_DIRNOACCESS       12   // No access to directory
#define PXERR_DIRNOTPRIVATE     14   // Single user, but directory is shared


// file oriented errors

#define PXERR_FILEBUSY          3    // File is busy
#define PXERR_FILELOCKED        4    // File is locked
#define PXERR_FILENOTFOUND      5    // Could not find file


// table oriented errors

#define PXERR_TABLEBUSY        118   // Table is busy
#define PXERR_TABLELOCKED      119   // Table is locked
#define PXERR_TABLENOTFOUND    120   // Table was not found
#define PXERR_TABLEOPEN         83   // Unable to perform operation on open table
#define PXERR_TABLEINDEXED      94   // Table is indexed
#define PXERR_TABLENOTINDEXED   95   // Table is not indexed
#define PXERR_TABLEEMPTY       105   // Operation on empty table
#define PXERR_TABLEWRITEPRO     22   // Table is write protected
#define PXERR_TABLEPRE40        93   // Feature not available for pre Paradox 4.0 tables

#define PXERR_TABLECORRUPTED     6   // Table is corrupted
#define PXERR_TABLEFULL         128  // Table is full
#define PXERR_TABLESQL          130  // Table is SQL replica
#define PXERR_INSUFRIGHTS       21   // Insufficient password rights
#define PXERR_CANTUPGRADE       92   // Table too old to upgrade, or
                                     // existing table header too small.
#define PXERR_LOCKTIMEOUT       137  // Timed out trying to achieve a lock.


// index oriented errors

#define PXERR_XCORRUPTED        7    // Primary index is corrupted
#define PXERR_XOUTOFDATE        8    // Primary index is out of date
#define PXERR_XSORTVERSION      13   // Sort for index different from table

#define PXERR_SXCORRUPTED       122  // Secondary index is corrupted
#define PXERR_SXOUTOFDATE       96   // Secondary index is out of date
#define PXERR_SXNOTFOUND        121  // Secondary index was not found
#define PXERR_SXOPEN            123  // Not used-here for backwards compatability
#define PXERR_SXCANTUPDATE      136  // Can't update table open on non-maintained secondary

#define PXERR_RECTOOBIG         125  // Record too big for index


// record oriented errors

#define PXERR_RECDELETED        50   // Another user deleted record
#define PXERR_RECLOCKED          9   // Record is locked
#define PXERR_RECNOTFOUND       89   // Record was not found
#define PXERR_KEYVIOL           97   // Key violation

#define PXERR_ENDOFTABLE       101   // End of table
#define PXERR_STARTOFTABLE     102   // Start of table


// errors specific for Windows Engine DLL

#define PXERR_TOOMANYCLIENTS   131
#define PXERR_EXCEEDSCONFIGLIMITS 132
#define PXERR_CANTREMAPFILEHANDLE 133


// resource errors

#define PXERR_OUTOFMEM          40   // Not enough memory to complete operation
#define PXERR_OUTOFDISK         41   // Not enough disk space to complete operation
#define PXERR_OUTOFSTACK        127  // Not enough stack space to complete operation
#define PXERR_OUTOFSWAPBUF      129  // Not enough swap buffer space to complete operation

#define PXERR_OUTOFFILEHANDLES  70   // No more file handles available
#define PXERR_OUTOFTABLEHANDLES 72   // No more table handles available
#define PXERR_OUTOFRECHANDLES  103   // No more record handles available
#define PXERR_OUTOFLOCKHANDLES 111   // Too many locks on table

#define PXERR_NOMORETMPNAMES    86   // No more temporary names available
#define PXERR_TOOMANYPASSW     115   // Too many passwords specified


// invalid parameters to functions

#define PXERR_TYPEMISMATCH      30   // Data type mismatch
#define PXERR_OUTOFRANGE        31   // Argument out of range
#define PXERR_INVPARAMETER      33   // Invalid argument
#define PXERR_INVDATE           73   // Invalid date given

#define PXERR_INVFIELDHANDLE    75   // Invalid field handle
#define PXERR_INVRECHANDLE     104   // Invalid record handle
#define PXERR_INVTABLEHANDLE    76   // Invalid table handle
#define PXERR_INVLOCKHANDLE    110   // Invalid lock handle

#define PXERR_INVDIRNAME       114   // Invalid directory name
#define PXERR_INVFILENAME      108   // Invalid file name
#define PXERR_INVTABLENAME      99   // Invalid table name
#define PXERR_INVFIELDNAME      74   // Invalid field name

#define PXERR_INVLOCKCODE      106   // Invalid lock code
#define PXERR_INVUNLOCK        109   // Invalid unlock
#define PXERR_INVSORTORDER     112   // Invalid sort order table
#define PXERR_INVPASSW         116   // Invalid password
#define PXERR_INVNETTYPE       113   // Invalid net type (PXNetInit)
#define PXERR_BUFTOOSMALL      117   // Buffer too small for result

#define PXERR_STRUCTDIFFER      81   // Table structures are different

#define PXERR_INVENGINESTATE    79   // Previous fatal error; cannot proceed


// Blob error codes

#define PXERR_BLOBMODE           51     // Operation not applicable for Blob's
                                        // open mode.
#define PXERR_BLOBOPEN           52     // Blob already open.
#define PXERR_BLOBINVOFFSET      53     // Invalid offset into Blob.
#define PXERR_BLOBINVSIZE        54     // Invalid size for Blob.
#define PXERR_BLOBMODIFIED       55     // Another user modified Blob.
#define PXERR_BLOBCORRUPT        56     // Blob file corrupted.
#define PXERR_BLOBNOINDEX        57     // Cannot index on a Blob.
#define PXERR_BLOBINVHANDLE      59     // Invalid Blob handle.
#define PXERR_BLOBNOSEARCH       60     // Can't search on a Blob field.


// Locking related error codes

#define PXERR_INUSEBYPDOX35      16     // Directory is in use by Paradox 3.5
                                        // or Paradox Engine 2.0


// values for ShareMode argument to PXWinInit

#define PXSINGLECLIENT  0     // allow no other client access to Engine DLL
#define PXEXCLUSIVE     1     // open all tables with FULL LOCK
#define PXSHARED        2     // open all tables with PREVENT FULL LOCK


// values for Mode argument to PXTblCreateMode

#define PARADOX35       0     // create Paradox 3.5 compatible tables
#define PARADOX40       1     // create Paradox 4.0 compatible tables


//----------------------------------------------------------------------
// prototypes for engine functions

// declarations of sort order tables, used in PXSetDefaults
#ifdef WINDOWS
#  define SortOrderAscii   'a'   // ASCII sort order
#  define SortOrderIntl    'i'   // international sort order
#  define SortOrderNorDan  'n'   // Norwegian/Danish sort order
#  define SortOrderSwedFin 's'   // Swedish/Finnish sort order
#  define SortOrderNorDan4 'd'   // Norwegian/Danish sort order for Paradox 4.0
#else // DOS
  extern char far * pascal far SortOrderAscii;  // ASCII sort order
  extern char far * pascal far SortOrderIntl;   // international sort order
  extern char far * pascal far SortOrderNorDan; // Norwegian/Danish sort order
  extern char far * pascal far SortOrderSwedFin;// Swedish/Finnish sort order
  extern char far * pascal far SortOrderNorDan4;// Norwegian/Danish sort order
                                                // for Paradox 4.0
#endif // DOS


// INITIALIZATION AND FINALIZATION FUNCTIONS

#ifdef WINDOWS
// initialize Engine connection in Windows environment
PXFUNC PXWinInit(
        char far *clientName,       // string containing name of application
        int shareMode);             // Share mode: PXSINGLECLIENT, PXEXCLUSIVE,
                                    // or PXSHARED


#else // DOS

// initialize Engine
PXFUNC PXInit(void);

// initialize Engine and net support
PXFUNC PXNetInit(
        char far *netNamePath,      // directory containing .NET file
        int  netType,               // type of network
        char far *userName);        // user name, as put in .NET file


#endif // DOS

// exit and deallocate
PXFUNC PXExit(void);

// overwrites internal default values
PXFUNC PXSetDefaults(
        int bufSize,                // internal swap buffer size
                                    // MINSWAPSIZE..MAXSWAPSIZE (8..256),
                                    // default DEFSWAPSIZE (32)
                                    // allocated at initialization time
        int maxTables,              // max number of tables open at a time
                                    // range 1..MAXTABLEHANDLES ,   (1..64)
                                    // default  DEFTABLEHANDLES     (5)
        int maxRecBufs,             // max number of record buffers at a time
                                    // range 1..MAXRECORDHANDLES ,  (1..512)
                                    // default  DEFRECORDHANDLES    (10)
        int maxLocks,               // max number of locks per table
                                    // range 1..MAXLOCKHANDLES  ,   (1..128)
                                    // default DEFLOCKHANDLES       (32)
        int maxFiles,               // max number of file handles to use
                                    // range MINFILEHANDLES..MAXFILEHANDLES
                                    // default DEFFILEHANDLES       (10)
#ifdef WINDOWS
        char sortOrder              // code for sort order table defined
                                    // internally in the Engine :
                                    // SortOrderAscii/SortOrderIntl/
                                    // SortOrderNorDan/SortOrderSwedFin/
                                    // SortOrderNorDan4
                                    // default : SortOrderAscii
#else // DOS
        char far *sortOrder         // address of sort order table defined
                                    // internally in the Engine :
                                    // SortOrderAscii/SortOrderIntl/
                                    // SortOrderNorDan/SortOrderSwedFin
                                    // SortOrderNorDan4
                                    // default : SortOrderAscii
#endif // DOS
        );


// returns current default settings
PXFUNC PXGetDefaults(
        int far *swapSize,          // returns internal swap buffer size
        int far *maxTables,         // returns max number of tables at a time
        int far *maxRecBufs,        // returns max number of record buffers
        int far *maxLocks,          // returns max number of locks per table
        int far *maxFiles,          // returns max number of file handles to use
#ifdef WINDOWS
        char far *sortTable         // returns code indicating default sort table
#else // DOS
        char far * far *sortTable   // returns address of default sort table
#endif // DOS
        );


// enables/disables internal hardware error handler
PXFUNC PXSetHWHandler(int hwHandler); // enable(TRUE) / disable (FALSE)
                                      // default TRUE

// UTILITY FUNCTIONS

PXFUNC ISBLANKDOUBLE(double x);
void pascal far BLANKDOUBLE(double far *x);


// TABLE FUNCTIONS

// open table for access, returning table handle
PXFUNC PXTblOpen(
        char far *tblName,             // name of table to open
        TABLEHANDLE far *tblHandlePtr, // returns handle for opened table
        int indexID,                   // =0 mastertable else indexnumber
        int saveEveryChange);          // save each record as it is changed
                                       // don't buffer changes

// close access to table
PXFUNC PXTblClose(TABLEHANDLE tblHandle);

// determine whether PXTblCreate creates Paradox 3.5 or 4.0 file formats
PXFUNC PXTblCreateMode(int mode);

// create empty table
PXFUNC PXTblCreate(
        char far *tblName,          // name of table to create
        int nFields,                // number of fields in table
        char far * far *fields,     // field names
        char far * far *types);     // field types (N, S..)

// clear table for records
PXFUNC PXTblEmpty(char far *tblName);

// delete table and its family
PXFUNC PXTblDelete(char far *tblName);

// copy table and its family
PXFUNC PXTblCopy(
        char far *srcTable,         // source table of copy
        char far *destTable);       // destination table of copy

// rename table and its family
PXFUNC PXTblRename(
        char far *srcTable,         // source table of copy
        char far *destTable);       // destination table of copy

// add records from one table to another table
PXFUNC PXTblAdd(
        char far *srcTable,         // source table of add
        char far *destTable);       // destination table of add

// upgrades table to Paradox 4.0 format
PXFUNC PXTblUpgrade(
        TABLEHANDLE tblHandle);     // table to upgrade


// RECORD FUNCTIONS

// insert record buffer in database (as last record if Heap)
PXFUNC PXRecAppend(
        TABLEHANDLE tblHandle,      // table to append record to
        RECORDHANDLE recHandle );   // record to append

// insert record buffer in database (before current if Heap)
PXFUNC PXRecInsert(
        TABLEHANDLE tblHandle,      // table to insert record into
        RECORDHANDLE recHandle);    // record to insert

// updates current record in database with contents of the record buffer
PXFUNC PXRecUpdate(
        TABLEHANDLE tblHandle,      // table to update record into
        RECORDHANDLE recHandle);    // changed record to post

// delete current record in table
PXFUNC PXRecDelete(TABLEHANDLE tblHandle);

// creates a record buffer for a table
PXFUNC PXRecBufOpen(
        TABLEHANDLE tblHandle,           // table to create buffer for
        RECORDHANDLE far *recHandlePtr); // returns handle to record buffer

// deletes a record buffer for a table
PXFUNC PXRecBufClose(RECORDHANDLE recHandle);

// clears the record buffer (to blanks)
PXFUNC PXRecBufEmpty(RECORDHANDLE recHandle);

// copy record from a record buffer to another (compatible) record buffer
PXFUNC PXRecBufCopy(
        RECORDHANDLE srcRecHandle,  // record buffer to copy from
        RECORDHANDLE destRecHandle);// record buffer to copy to

// gets the current record from the database into the record buffer
PXFUNC PXRecGet(
        TABLEHANDLE tblHandle,      // table to get record from
        RECORDHANDLE recHandle);    // record buffer to put record in

// gets the content of the record buffer without any translation
PXFUNC PXRawGet(
        RECORDHANDLE recHandle,     // record buffer containing the record
        void far *buffer,           // user's buffer area of size BufSize
        int bufSize);               // size of user's buffer area in bytes

// copies the user's buffer into the record buffer without any translation
PXFUNC PXRawPut(
        RECORDHANDLE recHandle,     // record buffer to contain the record
        void far *buffer,           // user's buffer area
        int bufSize);               // size of user's buffer area in bytes

// FIELD FUNCTIONS

// put short value into N/$/S field in record buffer
PXFUNC PXPutShort(
        RECORDHANDLE recHandle,     // record buffer to put value in
        FIELDHANDLE fldHandle,      // field in record
        short val);                 // value to put

// put double value into N/$/S field in record buffer
PXFUNC PXPutDoub(
        RECORDHANDLE recHandle,     // record buffer to put value in
        FIELDHANDLE fldHandle,      // field in record
        double Dval);               // value to put

// put long value into N/$/S field in record buffer
PXFUNC PXPutLong(
        RECORDHANDLE recHandle,     // record buffer to put value in
        FIELDHANDLE fldHandle,      // field in record
        long val);                  // value to put

// put string into Alpha field in record buffer
PXFUNC PXPutAlpha(
        RECORDHANDLE recHandle,     // record buffer to put value in
        FIELDHANDLE fldHandle,      // field in record
        char far *str);             // value to put

// put long value into date field (encoded value) in record buffer
PXFUNC PXPutDate(
        RECORDHANDLE recHandle,     // record buffer to put value in
        FIELDHANDLE fldHandle,      // field in record
        TDATE val);                 // value to put

// put blank value into field in record buffer
PXFUNC PXPutBlank(
        RECORDHANDLE recHandle,     // record buffer to put blank in
        FIELDHANDLE fldHandle);     // field in record

// get value from N/$/S field in record buffer, into short
PXFUNC PXGetShort(
        RECORDHANDLE recHandle,     // record buffer to get value from
        FIELDHANDLE fldHandle,      // field to get value from
        short far *shortPtr);       // returns value

// get value from N/$/S field in record buffer, into double
PXFUNC PXGetDoub(
        RECORDHANDLE recHandle,     // record buffer to get value from
        FIELDHANDLE fldHandle,      // field to get value from
        double far *doubPtr);       // returns value

// get value from N/$/S field in record buffer, into long
PXFUNC PXGetLong(
        RECORDHANDLE recHandle,     // record buffer to get value from
        FIELDHANDLE fldHandle,      // field to get value from
        long far *longPtr);         // returns value

// get string from alpha field in record buffer
PXFUNC PXGetAlpha(
        RECORDHANDLE recHandle,     // record buffer to get value from
        FIELDHANDLE fldHandle,      // field to get value from
        int bufSize,                // size of return buffer
        char far *dest);            // return buffer

// get value from date field in record buffer, into long  (encoded value)
PXFUNC PXGetDate(
        RECORDHANDLE recHandle,     // record buffer to get value from
        FIELDHANDLE fldHandle,      // field to get value from
        TDATE far *datePtr);        // returns value

// is value in specified field in record buffer a blank?
PXFUNC PXFldBlank(
        RECORDHANDLE recHandle,     // record to test value in
        FIELDHANDLE fldHandle,      // field to test
        int far *blankPtr);         // returns TRUE/FALSE




//----------------------------------------------------------------------

// move to record with specified record number
PXFUNC PXRecGoto(
        TABLEHANDLE tblHandle,      // tblHandle of table to move in
        RECORDNUMBER recNum);       // record number to move to

// move to first record in table
PXFUNC PXRecFirst(TABLEHANDLE tblHandle);

// move to last record in table
PXFUNC PXRecLast(TABLEHANDLE tblHandle);

// move to next record in table
PXFUNC PXRecNext(TABLEHANDLE tblHandle);

// move to previous record in table
PXFUNC PXRecPrev(TABLEHANDLE tblHandle);

// INDEX FUNCTIONS

// add a primary or secondary (maintained/nonmaintained) index
PXFUNC PXKeyAdd(
        char far *tblName,          // name of table to add index for
        int nFlds,                  // number of fields in index
        FIELDHANDLE far *fldHandles,// array of field numbers in index
        int mode);                  // type of index to create
                                    // PRIMARY/SECONDARY/INCSECONDARY

// delete an index for a table (primary/secondary)
PXFUNC PXKeyDrop(
        char far *tblName,          // name of table to delete index for
        FIELDHANDLE indexID);       // 0 if primary key, else field number
                                    // of secondary index

// return a field handle for a composite index
PXFUNC PXKeyMap(
        char far *tblName,          // name of table for composite fields
        int nFlds,                  // number of fields in composite index
        FIELDHANDLE far *fldHandles,// array of field numbers in index
        char far *fldName,          // field name associated with index
        int mode,                   // 0 for case-sensitive, 1 for case-insensitive
        FIELDHANDLE far *indexId);  // field handle for composite index

// return information from a composite index file
PXFUNC PXKeyQuery(
        char far *indexFileName,    // name of file that is the index
        char far *fldName,          // field name associated with index
        int far *nfields,           // number of fields in composite index
        int far *mode,              // mode (case-sensitive or not) of index
        FIELDHANDLE far *fldArray,  // array of field numbers in index
        FIELDHANDLE far *indexId);  // field handle for composite index

// DATE FUNCTIONS

// decodes a date value stored in the Paradox format
PXFUNC PXDateDecode(
        TDATE adate,                // long value to decode
        int far *month,             // decoded month value
        int far *day,               // decoded date value
        int far *year);             // decoded year value

// encodes a date value to a long value in Paradox format
PXFUNC PXDateEncode(
        int month,                  // month value to encode
        int day,                    // date value to encode
        int year,                   // year value to encode
        TDATE far *adate);          // encoded date value



// SEARCH FUNCTIONS

// searches a table for a given (sub) key
PXFUNC PXSrchKey(
        TABLEHANDLE tblHandle,      // table to search in
        RECORDHANDLE recHandle,     // record buffer containing key to find
        int nFlds,                  // number of fields in key
        int mode);                  // searching from first/next record

// searches a table for a given field value
PXFUNC PXSrchFld(
        TABLEHANDLE tblHandle,      // table to search in
        RECORDHANDLE recHandle,     // record buffer containing field to find
        FIELDHANDLE fldHandle,      // field number to search on
        int mode);                  // searching from first/next record

// PASSWORD FUNCTIONS

// checks if table is encrypted
PXFUNC PXTblProtected(
        char far *tblName,          // name of table to check
        int far *Protected);        // returns TRUE/FALSE

// enters a password to the Engine
PXFUNC PXPswAdd(char far *password);

// deletes a password previously entered
PXFUNC PXPswDel(char far *password);

// encrypt a table and make it password protected
PXFUNC PXTblEncrypt(
        char far *tblName,          // name of table to encrypt
        char far *password);        // password for encrypted table

// decrypt a table, password must already have been entered
PXFUNC PXTblDecrypt(char far *tblName);


// INFORMATIONAL FUNCTIONS

// checks if table exists
PXFUNC PXTblExist(
        char far *tblName,          // name of table to check
        int far *Exist) ;           // returns TRUE/FALSE

// returns table name corresponding to a table handle
PXFUNC PXTblName(
        TABLEHANDLE tblHandle,      // table to return name of
        int bufSize,                // size of return buffer
        char far *tblName);         // name of table, without extension

// returns record number of current record in table
PXFUNC PXRecNum(
        TABLEHANDLE tblHandle,       // table to get record number from
        RECORDNUMBER far *recNumPtr);// returns record number

// returns number of records in table
PXFUNC PXTblNRecs(
        TABLEHANDLE tblHandle,      // table to get number of records from
        RECORDNUMBER far *nRecsPtr);// returns number of records

// returns number of fields in a record
PXFUNC PXRecNFlds(
        TABLEHANDLE tblHandle,      // table to get number of fields from
        int far *nFldsPtr);         // returns number of fields in a record

// return number of fields in key for table
PXFUNC PXKeyNFlds(
        TABLEHANDLE tblHandle,      // table to get key size for
        int far *nKeyFldsPtr);      // returns number of fields in key

// returns field number of a given field name in a table
PXFUNC PXFldHandle(
        TABLEHANDLE tblHandle,         // table to get field number from
        char far *fldName,             // name of field in table
        FIELDHANDLE far *fldHandlePtr);// returns field number
                                
// returns field type of a given field in a table
PXFUNC PXFldType(
        TABLEHANDLE tblHandle,      // table to get field type from
        FIELDHANDLE fldHandle,      // field number of field in table
        int bufSize,                // size of return buffer
        char far *fldType);         // field type of field as string

// returns field name of a given field in a table
PXFUNC PXFldName(
        TABLEHANDLE tblHandle,      // table to get field name from
        FIELDHANDLE fldHandle,      // field number of field in table
        int bufSize,                // size of return buffer
        char far *fldName);         // returns name of field



// MISCELLANEOUS FUNCTIONS
// sets maximum size of tables created with PXTblCreat()
PXFUNC PXTblMaxSize(
	int maxTblSize);            // maximum size of table 64/128/256 Mbytes

// saves all buffered changes to disk
PXFUNC PXSave(void);


// CONCURRENCY FUNCTIONS
// can be used only if PXNetInit() or PXWinInit() was successful

// returns name of user as known on network
PXFUNC PXNetUserName(
        int bufSize,                // size of return buffer
        char far *userName);        // returns user name

// locks a file with specified lock (general function)
PXFUNC PXNetFileLock(
        char far *fileName,         // name of file to lock
        int lockType);              // type of lock to put on file
                                    // (FL,WL,PWL,PWL)

// unlocks a file with specified lock (general function)
PXFUNC PXNetFileUnlock(
        char far *fileName,         // name of file to unlock
        int lockType);              // type of lock to remove from file
                                    // (FL,WL,PWL,PWL)

// locks an open table with specified lock
PXFUNC PXNetTblLock(
        TABLEHANDLE tblHandle,      // table to lock
        int lockType);              // type of lock to put on table
                                    // (FL,WL,PWL,PWL)

// unlocks an open table with specified lock
PXFUNC PXNetTblUnlock(
        TABLEHANDLE tblHandle,      // table to unlock
        int lockType);              // type of lock to remove from table
                                    // (FL,WL,PWL,PWL)

// locks the current record in a table
PXFUNC PXNetRecLock(
        TABLEHANDLE tblHandle,         // table to lock record in
        LOCKHANDLE far *lckHandlePtr); // returns handle to lock

// unlocks record associated with lock handle in the table
PXFUNC PXNetRecUnlock(
        TABLEHANDLE tblHandle,      // table to unlock record in
        LOCKHANDLE lckHandle);      // lock handle of record to unlock

// checks if current record in table is locked (by any user)
PXFUNC PXNetRecLocked(
        TABLEHANDLE tblHandle,      // table to check record in
        int far *Locked);           // returns TRUE/FALSE

// moves to the record in the table associated with the lock handle
PXFUNC PXNetRecGotoLock(
        TABLEHANDLE tblHandle,      // table to move in
        LOCKHANDLE lckHandle);      // lock handle to record

// checks if table was changed by other user since last refresh
PXFUNC PXNetTblChanged(
        TABLEHANDLE tblHandle,      // table to test
        int far *Changed);          // returns TRUE/FALSE

// forces a refresh of a table if it was changed by another user
PXFUNC PXNetTblRefresh(TABLEHANDLE tblHandle);



// ERROR FUNCTIONS

// returns error text associated with the error number
// NOTE: PXErrMsg is no longer included in the Engine Library itself,
//       but is shipped as a separate source file pxmsg.c
//
char far *pascal far PXErrMsg(int errCode);


// returns name of user causing a locking error
PXFUNC PXNetErrUser(
        int bufSize,                // size of return buffer
        char far *userName);        // returns user name


// BLOB FUNCTIONS

// open a Blob for Read access
PXFUNC PXBlobOpenRead(
        RECORDHANDLE recHandle,     // record handle containing Blob field
        FIELDHANDLE fldHandle,      // field handle of Blob to read
        BLOBHANDLE far *blbHandle); // Blob handle used for reading

// open a Blob for Write access (create a Blob)
PXFUNC PXBlobOpenWrite(
        RECORDHANDLE recHandle,     // record handle containing Blob field
        FIELDHANDLE fldHandle,      // field handle of Blob to create
        BLOBHANDLE far *blbHandle,  // Blob handle used for writing
        unsigned long size,         // size of Blob to create
        int saveCurrent);           // save current Blob in field?

// close an open Blob
PXFUNC PXBlobClose(
        BLOBHANDLE blbHandle,       // Blob handle to close
        int accept);                // if opened for writing, save changes?

// delete a Blob
PXFUNC PXBlobDrop(
        RECORDHANDLE recHandle,     // record handle containing Blob field
        FIELDHANDLE fldHandle);     // field handle of Blob to delete

// get size of a Blob
PXFUNC PXBlobGetSize(
        BLOBHANDLE blbHandle,       // Blob handle to get size of
        unsigned long far *size);   // size of the Blob

// gets data from a Blob, when opened for reading
PXFUNC PXBlobGet(
        BLOBHANDLE blbHandle,       // Blob handle to read from
        unsigned size,              // size of data to be read
        unsigned long offset,       // offset from start of Blob to read
        void far *buff);            // buffer to contain data from Blob

// writes data to a Blob, when opened for writing
PXFUNC PXBlobPut(
        BLOBHANDLE blbHandle,       // Blob handle to write to
        unsigned size,              // size of data to be written
        unsigned long offset,       // offset from start of Blob to write
        void far *buff);            // buffer containing data to be written

// used to make a private copy (clone) of a public Blob
PXFUNC PXBlobClone(
        RECORDHANDLE recHandle,     // record handle containing Blob field
        FIELDHANDLE fldHandle);     // field handle of Blob to be cloned

// read the part of the Blob data stored in the table (the leader)
PXFUNC PXBlobQuickGet( 
        RECORDHANDLE recHandle,     // record handle containing Blob field
        FIELDHANDLE fldHandle,      // field handle of Blob to be read
        int bufSize,                // size of buffer to hold data
        void far *dest,             // buffer to hold data read from Blob
        int far *bytesRead);        // number of bytes read from the Blob


#ifdef _PX_DEFED_WINDOWS
#  undef WINDOWS
#  undef _PX_DEFED_WINDOWS
#endif // _PX_DEFED_WINDOWS

#ifdef __cplusplus
   }
#endif // __cplusplus

#endif // PXENGINE_H
