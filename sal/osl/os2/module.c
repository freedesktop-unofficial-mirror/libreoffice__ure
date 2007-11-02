/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: module.c,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: hr $ $Date: 2007-11-02 12:31:42 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/


#include "system.h"

#include <osl/module.h>
#include <osl/diagnose.h>

#ifndef _OSL_FILE_H_
#include <osl/file.h>
#endif

#ifndef _OSL_THREAD_H_
#include <osl/thread.h>
#endif

#include <stdlib.h>

int UnicodeToText(char *, size_t, const sal_Unicode *, sal_Int32);

// static data for holding SAL dll module and full path
static HMODULE hModSal;
static char	szSalDir[ _MAX_PATH];
static char	szSalDrive[ _MAX_PATH];

/*****************************************************************************/
/* osl_loadModule */
/*****************************************************************************/

ULONG APIENTRY _DosLoadModule (PSZ pszObject, ULONG uObjectLen, PCSZ pszModule,
    PHMODULE phmod)
{
    APIRET	rc;
    rc = DosLoadModule( pszObject, uObjectLen, pszModule, phmod);
    // YD 22/05/06 issue again if first call fails (why?)
    if (rc == ERROR_INVALID_PARAMETER)
        rc = DosLoadModule( pszObject, uObjectLen, pszModule, phmod);
    return rc;
}

oslModule SAL_CALL osl_loadModule(rtl_uString *ustrModuleName, sal_Int32 nRtldMode)
{
    HMODULE hModule;
    BYTE szErrorMessage[256];
    APIRET rc;
    oslModule pModule=0;
    rtl_uString* ustrTmp = NULL;

    OSL_ENSURE(ustrModuleName,"osl_loadModule : string is not valid");

    // to avoid registering dll with full path at every install inside
    // services.rbd, the dll are registere only once with empty path
    // then if drive is not specified, the SAL module path is used
    // to load components.
    if (hModSal == NULL) {
        ULONG	ObjNum;
        CHAR	Buff[2*_MAX_PATH];
        ULONG	Offset;
        char 	drive[_MAX_DRIVE], dir[_MAX_DIR];
        char 	fname[_MAX_FNAME], ext[_MAX_EXT];
        // get module handle (and name)
        rc = DosQueryModFromEIP( &hModSal, &ObjNum, sizeof( Buff), Buff, 
                                &Offset, (ULONG)osl_loadModule);
        if (rc) {
            sal_Char szError[ 120 ];
            sprintf( szError, "DosQueryModFromEIP failed (%s) rc=%d\n", Buff, rc);
            OSL_TRACE(szError);
            return NULL;
        }
        DosQueryModuleName(hModSal, sizeof(Buff), Buff);
        // extract path info
        _splitpath( Buff, szSalDrive, szSalDir, NULL, NULL);
        // update BeginLIBPATH, otherwise a rc=2 can be returned if someone
        // changes the current directory
        CHAR	ExtLIBPATH[1024];
        strcpy( ExtLIBPATH, szSalDrive);
        strcat( ExtLIBPATH, szSalDir);
        strcat( ExtLIBPATH, ";%BeginLIBPATH%");
        rc = DosSetExtLIBPATH( (PCSZ)ExtLIBPATH, BEGIN_LIBPATH);
    }

    /* ensure ustrTmp hold valid string */
    if( osl_File_E_None != osl_getSystemPathFromFileURL( ustrModuleName, &ustrTmp ) )
        rtl_uString_assign( &ustrTmp, ustrModuleName );

    if( ustrTmp )
    {
        char buffer[PATH_MAX];

        if( UnicodeToText( buffer, PATH_MAX, ustrTmp->buffer, ustrTmp->length ) )
        {
            char drive[_MAX_DRIVE], dir[_MAX_DIR];
            char fname[_MAX_FNAME], ext[_MAX_EXT];
            char* dot;
            // 21/02/2006 YD dll names must be 8.3: since .uno.dll files
            // have hardcoded names, I'm truncating names here and also in
            // the build system
            _splitpath (buffer, drive, dir, fname, ext);
            if (strlen(fname)>8)
                fname[8] = 0;	// truncate to 8.3
            dot = strchr( fname, '.');
            if (dot)
                *dot = '\0';	// truncate on dot
            _makepath( buffer, drive, dir, fname, ext);

            rc = _DosLoadModule( szErrorMessage, sizeof( szErrorMessage), (PCSZ)buffer, &hModule);
            if (rc == NO_ERROR )
                pModule = (oslModule)hModule;
            else
            {
                // try again using SAL path
                _makepath( buffer, szSalDrive, szSalDir, fname, ext);
                rc = _DosLoadModule( szErrorMessage, sizeof( szErrorMessage), (PCSZ)buffer, &hModule);
    
                if (rc == NO_ERROR )
                    pModule = (oslModule)hModule;
                else
                {
                    sal_Char szError[ 120 ];
                    sprintf( szError, "Module: %s; rc: %d;\nReason: %s;\n"
                            "Please contact technical support and report above informations.\n\n", 
                            buffer, rc, szErrorMessage );
#if OSL_DEBUG_LEVEL>0
                    fprintf( stderr, szError);
#endif
                    //OSL_TRACE(szError);
#ifndef OSL_DEBUG_LEVEL
                    WinMessageBox(HWND_DESKTOP,HWND_DESKTOP,
                        szError, "Critical error: DosLoadModule failed",
                        0, MB_ERROR | MB_OK | MB_MOVEABLE);
#endif
                }
            }
        }
    }

    rtl_uString_release( ustrTmp );

    return pModule;
}

/*****************************************************************************/
/* osl_getModuleHandle */
/*****************************************************************************/

sal_Bool SAL_CALL 
osl_getModuleHandle(rtl_uString *pModuleName, oslModule *pResult)
{
    HMODULE hmod;
    APIRET  rc;
    rc = DosQueryModuleHandle(pModuleName->buffer, &hmod);
    if( rc == NO_ERROR)
    {
        *pResult = (oslModule) hmod;
        return sal_True;
    }
    
    return sal_False;
}

/*****************************************************************************/
/* osl_unloadModule */
/*****************************************************************************/
void SAL_CALL osl_unloadModule(oslModule Module)
{
#if OSL_DEBUG_LEVEL>0
    if (!Module)
       fprintf( stderr, "osl_unloadModule NULL HANDLE.\n");
#endif

    DosFreeModule((HMODULE)Module);
}

/*****************************************************************************/
/* osl_getSymbol */
/*****************************************************************************/
void* SAL_CALL 
osl_getSymbol(oslModule Module, rtl_uString* pSymbolName)
{
    return (void *) osl_getFunctionSymbol(Module, pSymbolName);
}

/*****************************************************************************/
/* osl_getFunctionSymbol */
/*****************************************************************************/
oslGenericFunction SAL_CALL osl_getFunctionSymbol( oslModule Module, rtl_uString *strSymbolName )
{
    rtl_String *symbolName = NULL;
    oslGenericFunction address;

    OSL_ASSERT(Module);
    OSL_ASSERT(strSymbolName);

    rtl_uString2String( 
        &symbolName, 
        strSymbolName->buffer, 
        strSymbolName->length, 
        RTL_TEXTENCODING_UTF8,
        OUSTRING_TO_OSTRING_CVTFLAGS
    );

    address=osl_getAsciiFunctionSymbol(Module, rtl_string_getStr(symbolName));
    rtl_string_release(symbolName);

    return address;
}

/*****************************************************************************/
/* osl_getAsciiFunctionSymbol */
/*****************************************************************************/
oslGenericFunction SAL_CALL 
osl_getAsciiFunctionSymbol( oslModule Module, const sal_Char *pSymbol )
{
    PFN  pFunction;
    APIRET rc;
    void* pHandle=0;

    OSL_ENSURE(Module,"osl_getSymbol : module handle is not valid");
    OSL_ENSURE(Module,"osl_getSymbol : ustrSymbolName");

    if ( Module!= 0 && pSymbol != 0 )
    {

        rc = DosQueryProcAddr( (HMODULE) Module, 0, (PCSZ)pSymbol, &pFunction );
        if( rc == NO_ERROR )
        {
            pHandle = (void*)pFunction;
        }
        else
        {
            // YD try again adding the '_' prefix
            char _pszSymbolName[255];
            strcpy( _pszSymbolName, "_");
            strcat( _pszSymbolName, pSymbol);
            rc = DosQueryProcAddr( (HMODULE) Module, 0, (PCSZ)_pszSymbolName, &pFunction );
            if( rc == NO_ERROR )
                pHandle = (void*)pFunction;
        }

    }

    return pHandle;
}

/*****************************************************************************/
/* osl_getModuleURLFromAddress */
/*****************************************************************************/
sal_Bool SAL_CALL osl_getModuleURLFromAddress(void * addr, rtl_uString ** ppLibraryUrl)
{
    //APIRET APIENTRY DosQueryModFromEIP (HMODULE *phMod, ULONG *pObjNum, 
    //          ULONG BuffLen, PCHAR pBuff, ULONG *pOffset, ULONG Address) 
    HMODULE	hMod;
    ULONG	ObjNum;
    CHAR	Buff[2*_MAX_PATH];
    ULONG	Offset;
    APIRET	rc;

    // get module handle (and name)
    rc = DosQueryModFromEIP( &hMod, &ObjNum, sizeof( Buff), Buff, &Offset, (ULONG)addr);
    if (rc)
        return sal_False;

    // get module full path
    rc = DosQueryModuleName( hMod, sizeof( Buff), Buff);
    if (rc)
        return sal_False;

#if OSL_DEBUG_LEVEL > 1
    OSL_TRACE("module.c::osl_getModuleURLFromAddress - %s\n", Buff);
#endif

    // convert to URL
    rtl_uString	*ustrSysPath = NULL;
    rtl_string2UString( &ustrSysPath, Buff, strlen(Buff), osl_getThreadTextEncoding(), OSTRING_TO_OUSTRING_CVTFLAGS );
    OSL_ASSERT(ustrSysPath != NULL);
    osl_getFileURLFromSystemPath( ustrSysPath, ppLibraryUrl );
    rtl_uString_release( ustrSysPath );

    return sal_True;
}

/*****************************************************************************/
/* osl_getModuleURLFromFunctionAddress */
/*****************************************************************************/
sal_Bool SAL_CALL osl_getModuleURLFromFunctionAddress( oslGenericFunction addr, rtl_uString ** ppLibraryUrl )
{
    return osl_getModuleURLFromAddress( ( void * )addr, ppLibraryUrl ); 
}

/*****************************************************************************/

