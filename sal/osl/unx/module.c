/*************************************************************************
 *
 *  $RCSfile: module.c,v $
 *
 *  $Revision: 1.21 $
 *  last change: $Author: mh $ $Date: 2001-12-19 15:08:23 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif

#ifndef _OSL_MODULE_H_
#include <osl/module.h>
#endif

#ifndef _OSL_THREAD_H_
#include <osl/thread.h>
#endif

#ifndef _OSL_FILE_H_
#include <osl/file.h>
#endif

#ifdef IRIX
#ifndef _RLD_INTERFACE_DLFCN_H_DLADDR
#define _RLD_INTERFACE_DLFCN_H_DLADDR
typedef struct DL_INFO {
       const char * dli_fname;
       void       * dli_fbase;
       const char * dli_sname;
       void       * dli_saddr;
       int          dli_version;
       int          dli_reserved1;
       long         dli_reserved[4];
} Dl_info;
#endif
#include <rld_interface.h>
#define _RLD_DLADDR             14
int dladdr(void *address, Dl_info *dl);

int dladdr(void *address, Dl_info *dl)
{
       void *v;
       v = _rld_new_interface(_RLD_DLADDR,address,dl);

       return (int)v;
}
#endif

#include "system.h"

#if defined(DEBUG)
#include <stdio.h>
#endif

#ifdef MACOSX
#ifndef _OSL_PROCESS_H_
#include <osl/process.h>
#endif
extern oslProcessError SAL_CALL osl_searchPath(const sal_Char* pszName, const sal_Char* pszPath, sal_Char Separator, sal_Char *pszBuffer, sal_uInt32 Max);
#else /* MACOSX */
extern int _dlclose(void *handle);
#endif /* MACOSX */

/* implemented in file.c */
extern UnicodeToText( char *, size_t, const sal_Unicode *, sal_Int32 );

oslModule SAL_CALL osl_psz_loadModule(const sal_Char *pszModuleName, sal_Int32 nRtldMode);
void* SAL_CALL osl_psz_getSymbol(oslModule hModule, const sal_Char* pszSymbolName);


/*****************************************************************************/
/* osl_loadModule */
/*****************************************************************************/

oslModule SAL_CALL osl_loadModule(rtl_uString *ustrModuleName, sal_Int32 nRtldMode)
{
    oslModule pModule=0;
    rtl_uString* ustrTmp = NULL;

    OSL_ENSURE(ustrModuleName,"osl_loadModule : string is not valid");
    
    /* ensure ustrTmp hold valid string */
    if( osl_File_E_None != osl_getSystemPathFromFileURL( ustrModuleName, &ustrTmp ) )
        rtl_uString_assign( &ustrTmp, ustrModuleName );
    
    if( ustrTmp )
    {    
        char buffer[PATH_MAX];
        
        if( UnicodeToText( buffer, PATH_MAX, ustrTmp->buffer, ustrTmp->length ) )
            pModule = osl_psz_loadModule( buffer, nRtldMode );
    }

    rtl_uString_release( ustrTmp );
    
    return pModule;
}

/*****************************************************************************/
/* osl_psz_loadModule */
/*****************************************************************************/

oslModule SAL_CALL osl_psz_loadModule(const sal_Char *pszModuleName, sal_Int32 nRtldMode)
{
#ifdef MACOSX

    CFStringRef		pPath=0;
    CFURLRef		pURL=0;
    CFBundleRef		pLib=0;
    sal_Char		*searchPath=0;
    sal_Char		path[PATH_MAX + 1];
    sal_Char		*pszModulePath=0;

    OSL_ASSERT(pszModuleName);

    if ( ! pszModuleName || *pszModuleName == '\0' )
        return NULL;

    /*
     * Try to construct the absolute path to the module name by searching
     * for the module name in the directories specified in the
     * DYLD_LIBRARY_PATH environment variable.
     */
    if ( osl_searchPath( pszModuleName, "DYLD_LIBRARY_PATH", '\0', path, sizeof(path) ) == osl_Process_E_None )
        pszModulePath = path;

    /* Try checking in the system directories */
    if ( ! pszModulePath )
    {
        strcpy( path, "/System/Library/Frameworks/" );
        strcat( path, pszModuleName );
        pszModulePath = path;
    }
    
    if ( pszModulePath )
    {
#ifndef NO_DL_FUNCTIONS
        /* Convert path in strModulePath to CFStringRef */
        pPath = CFStringCreateWithCString( NULL, pszModulePath,
            kCFStringEncodingUTF8 );

        /* Get the framework's CFURLRef using its path */
        if ( ! (pURL = CFURLCreateWithFileSystemPath( kCFAllocatorDefault,
            pPath, kCFURLPOSIXPathStyle, true) ) )
        {
#ifdef DEBUG
            fprintf( stderr,
                "osl_loadModule: cannot load module %s for reason: %s\n",
                pszModulePath, "path does not exist" );
#endif
            CFRelease(pPath);
            return NULL;
        }

        /* Load the framework */
        if ( ! ( pLib = CFBundleCreate( NULL, pURL) ) )
        {
#ifdef DEBUG
            fprintf( stderr,
                "osl_loadModule: cannot load module %s for reason: %s\n",
                pszModulePath, "path is not a bundle" );
#endif
            CFRelease( pPath );
            CFRelease( pURL );
            return NULL;
        }

        /* Load the library in the framework */
        if ( CFBundleLoadExecutable( pLib ) )
        {
            oslModule pModule=0;

            CFRelease( pPath );
            CFRelease( pURL );
            pModule = (oslModule)malloc( sizeof( struct _oslModule ) );

            if ( pModule )
            {
                // ignore trailing ".dylib*"
                int len = strlen( pszModuleName ) - 6;
                pModule->pModule = pLib;
                // find last ".dylib"
                for ( ; len > 0 ; len-- )
                {
                    if ( !strncmp( pszModuleName + len - 1, ".dylib", 6 ) )
                        break;
                }
                pModule->pModuleName = (sal_Char *)malloc( len );
                strncpy( pModule->pModuleName, pszModuleName, len );
                pModule->pModuleName[len - 1] = '\0';
            }

            return pModule;
        }
        else
        {
#ifdef DEBUG
            fprintf( stderr,
                "osl_loadModule: cannot load module %s for reason: %s\n",
                pszModulePath, "bundle does not contain a valid library" );
#endif
            CFRelease( pPath );
            CFRelease( pURL );
            CFRelease( pLib );
            return NULL;
        }

#endif 
    }
    return NULL;

#else /* MACOSX */

    sal_Int32 rtld_mode;	

    if ( nRtldMode == 0 )
    {
        rtld_mode = RTLD_LAZY;
    }
    else
    {
        rtld_mode=nRtldMode;
    }

    OSL_ASSERT(pszModuleName);
    
    if (pszModuleName)
    {
#ifndef NO_DL_FUNCTIONS
        void* pLib = dlopen(pszModuleName, rtld_mode );
#ifdef DEBUG
        if( ! pLib )
            fprintf( stderr, "osl_loadModule: cannot load module %s for reason: %s\n",
                     pszModuleName, dlerror() );
#endif		
        return ((oslModule)pLib);
#else
        printf("No DL Functions\n");
#endif 
    }
    return NULL;

#endif /* MACOSX */
}

/*****************************************************************************/
/* osl_unloadModule */
/*****************************************************************************/
void SAL_CALL osl_unloadModule(oslModule hModule)
{
    OSL_ENSURE(hModule,"osl_unloadModule : module handle is not valid");
    
#ifdef MACOSX


    if (hModule)
    {
        if ( hModule->pModule )
        {
            CFBundleUnloadExecutable((CFBundleRef)(hModule->pModule));
            CFRelease((CFBundleRef)(hModule->pModule));
        }
        if ( hModule->pModuleName )
            free( hModule->pModuleName );
        free( hModule );
    }

#else /* MACOSX */

    if (hModule)
    {
#ifndef NO_DL_FUNCTIONS
#ifndef GCC
        /*     gcc (2.9.1 (egcs), 295) registers atexit handlers for
         *     static destructors which obviously cannot
         *     be called after dlclose. A compiler "feature". The workaround for now
         *     is not to dlclose libraries. Since most of them are closed at shutdown
         *     this does not make that much a difference
         */
        
        int nRet = 0;
        
        nRet = dlclose(hModule);
#if defined(DEBUG)
        if ( nRet != 0 )
        {    
            fprintf( stderr, "osl_getsymbol: cannot close lib for reason: %s\n", dlerror() );
        }
#endif /* if DEBUG */
#endif /* ifndef GCC */
        
#endif /* ifndef NO_DL_FUNCTIONS */
    }

#endif /* MACOSX */
}

/*****************************************************************************/
/* osl_getSymbol */
/*****************************************************************************/
void* SAL_CALL osl_getSymbol(oslModule Module, rtl_uString* ustrSymbolName)
{
    void* pHandle=0;

    OSL_ENSURE(Module,"osl_getSymbol : module handle is not valid");
    OSL_ENSURE(Module,"osl_getSymbol : ustrSymbolName");
    
    if ( Module!= 0 && ustrSymbolName != 0 )
    {
        rtl_String* strSymbolName=0;
        sal_Char* pszSymbolName=0;
        
        rtl_uString2String( &strSymbolName,
                            rtl_uString_getStr(ustrSymbolName),
                            rtl_uString_getLength(ustrSymbolName),
                            osl_getThreadTextEncoding(),
                            OUSTRING_TO_OSTRING_CVTFLAGS );    

        pszSymbolName = rtl_string_getStr(strSymbolName);

        
        pHandle=osl_psz_getSymbol(Module,pszSymbolName);

        if ( strSymbolName != 0 )
        {
            rtl_string_release(strSymbolName);
        }
    }
    
    
    return pHandle;
}

void* SAL_CALL osl_psz_getSymbol(oslModule hModule, const sal_Char* pszSymbolName)
{
#ifdef MACOSX

    CFMutableStringRef pMutSymbolName=0;
    CFStringRef pSymbolName=0;
    void *pSymbol=0;

    OSL_ASSERT(hModule);
    OSL_ASSERT(pszSymbolName);

    if (hModule && hModule->pModule && pszSymbolName)
    {
#ifndef NO_DL_FUNCTIONS
        /* The need to concat the library name and function name is caused
           by a bug in MACOSX's loader */
        if ( hModule->pModuleName )  /* if module name is not null */
        {
            /* Convert char pointer to CFStringRef, make it mutable and
               append the symbol */
            pSymbolName = CFStringCreateWithCString(NULL, hModule->pModuleName, kCFStringEncodingUTF8);
            pMutSymbolName = CFStringCreateMutableCopy(NULL, 0, pSymbolName);
            CFStringAppendCString(pMutSymbolName, pszSymbolName, kCFStringEncodingUTF8);
            /* Try to get the symbol */
            pSymbol = CFBundleGetFunctionPointerForName((CFBundleRef)(hModule->pModule), (CFStringRef)pMutSymbolName);
            /* Release CFStringRef */
            if ( pSymbolName )
                CFRelease(pSymbolName);
            /* Release CFMutableStringRef */
            if ( pMutSymbolName )
                CFRelease(pMutSymbolName);
        }

        /* If a symbol with the module name as a prefix was not found, try to
           load the symbol without any prefix */
        if ( !pSymbol )
        {
            /* Convert char pointer to CFStringRef */
            pSymbolName = CFStringCreateWithCString(NULL, pszSymbolName, kCFStringEncodingUTF8);

            /* Try to get the symbol */
            pSymbol = CFBundleGetFunctionPointerForName((CFBundleRef)(hModule->pModule), pSymbolName);
            /* Release CFStringRef */
            if ( pSymbolName )
                CFRelease(pSymbolName);
        }

        return pSymbol;
#endif
    }
    return NULL;

#else /* MACOSX */

    OSL_ASSERT(hModule);
    OSL_ASSERT(pszSymbolName);

    if (hModule && pszSymbolName)
    {
#ifndef NO_DL_FUNCTIONS
        void* pSym = 0;

        pSym = dlsym(hModule, pszSymbolName);

#ifdef DEBUG
        if( ! pSym )
            fprintf( stderr, "osl_getsymbol: cannot get Symbol %s for reason: %s\n",
                     pszSymbolName, dlerror() );
        else
            fprintf( stderr, "osl_getsymbol: got Symbol %s \n", pszSymbolName);
#endif		
        
        return pSym;
        
#endif
    }
    return NULL;

#endif /* MACOSX */
}

sal_Bool SAL_CALL osl_getModuleURLFromAddress(void * addr, rtl_uString ** ppLibraryUrl)
{
    sal_Bool result = sal_False;
    Dl_info dl_info;

    if(result = dladdr(addr, &dl_info)) 
    {
        rtl_uString * workDir = NULL;
        osl_getProcessWorkingDir(&workDir);

#ifdef DEBUG
        OSL_TRACE("module.c::osl_getModuleUrl - %s\n", dl_info.dli_fname);
#endif

        rtl_string2UString(ppLibraryUrl, dl_info.dli_fname, strlen(dl_info.dli_fname), osl_getThreadTextEncoding(), OSTRING_TO_OUSTRING_CVTFLAGS);

        osl_getFileURLFromSystemPath(*ppLibraryUrl, ppLibraryUrl); // convert it to be a file url   

        osl_getAbsoluteFileURL(workDir, *ppLibraryUrl, ppLibraryUrl); // ensure it is an abosolute file url

        result = sal_True;
    }

    return result;
}

