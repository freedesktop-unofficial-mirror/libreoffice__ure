/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: process_impl.cxx,v $
 * $Revision: 1.6 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/


#define INCL_DOS
#include <os2.h>

#include "osl/process.h"
#include <osl/mutex.h>

#ifndef INCLUDED_LIMITS_H
#include <limits.h>
#define INCLUDED_LIMITS_H
#endif

#ifndef INCLUDED_PTHREAD_H
#include <pthread.h>
#define INCLUDED_PTHREAD_H
#endif

#ifndef INCLUDED_STDLIB_H
#include <stdlib.h>
#define INCLUDED_STDLIB_H
#endif

#ifndef INCLUDED_STRING_H
#include <string.h>
#define INCLUDED_STRING_H
#endif
#include "osl/diagnose.h"
#include <osl/file.h>
#include "osl/module.h"
#include "osl/thread.h"
#include "rtl/ustring.hxx"

#ifndef _OSL_FILE_PATH_HELPER_H_
#include "file_path_helper.h"
#endif

#ifndef _OSL_UUNXAPI_H_
#include "uunxapi.h"
#endif

/***************************************
  osl_bootstrap_getExecutableFile_Impl().

  @internal
  @see rtl_bootstrap
  @see #i37371#

 **************************************/

extern "C" oslProcessError SAL_CALL osl_bootstrap_getExecutableFile_Impl (
    rtl_uString ** ppFileURL
) SAL_THROW_EXTERN_C();


oslProcessError SAL_CALL osl_bootstrap_getExecutableFile_Impl (
    rtl_uString ** ppFileURL
) SAL_THROW_EXTERN_C()
{
    oslProcessError result = osl_Process_E_NotFound;
    CHAR    szName[CCHMAXPATH];
    PPIB    ppib;
    PTIB    ptib;
    APIRET	rc;

    rc = DosGetInfoBlocks(&ptib, &ppib);
    rc = DosQueryModuleName(ppib->pib_hmte, sizeof(szName), szName);
    
    if (rc == 0)
    {
        rtl_uString * pAbsPath = 0;
        
        rtl_string2UString( 
            &pAbsPath, 
            szName, strlen(szName), osl_getThreadTextEncoding(),
            OSTRING_TO_OUSTRING_CVTFLAGS);
        
        if (pAbsPath)
        {
            /* Convert from path to url. */
            if (osl_getFileURLFromSystemPath (pAbsPath, ppFileURL) == osl_File_E_None)
            {
                /* Success. */
                result = osl_Process_E_None;
            }
            rtl_uString_release (pAbsPath);
        }
    }

    return (result);
}

/***************************************
 CommandArgs_Impl.
 **************************************/
struct CommandArgs_Impl
{
    oslMutex		m_mutex;
    sal_uInt32      m_nCount;
    rtl_uString **  m_ppArgs;
};

static struct CommandArgs_Impl g_command_args =
{
    osl_createMutex(),
    0,
    0
};

/***************************************
  osl_getExecutableFile().
 **************************************/
oslProcessError SAL_CALL osl_getExecutableFile (rtl_uString ** ppustrFile)
{
    oslProcessError result = osl_Process_E_NotFound;

    osl_acquireMutex(g_command_args.m_mutex);
    OSL_ASSERT(g_command_args.m_nCount > 0);
    if (g_command_args.m_nCount > 0)
    {
        /* CommandArgs set. Obtain argv[0]. */
        rtl_uString_assign (ppustrFile, g_command_args.m_ppArgs[0]);
        result = osl_Process_E_None;
    }
    osl_releaseMutex(g_command_args.m_mutex);

    return (result);
}

/***************************************
 osl_getCommandArgCount().
 **************************************/
sal_uInt32 SAL_CALL osl_getCommandArgCount (void)
{
    sal_uInt32 result = 0;

    osl_acquireMutex(g_command_args.m_mutex);
    OSL_ASSERT(g_command_args.m_nCount > 0);
    if (g_command_args.m_nCount > 0)
        result = g_command_args.m_nCount - 1;
    osl_releaseMutex(g_command_args.m_mutex);

    return (result);
}

/***************************************
 osl_getCommandArg().
 **************************************/
oslProcessError SAL_CALL osl_getCommandArg (sal_uInt32 nArg, rtl_uString ** strCommandArg)
{
    oslProcessError result = osl_Process_E_NotFound;

    osl_acquireMutex(g_command_args.m_mutex);
    OSL_ASSERT(g_command_args.m_nCount > 0);
    if (g_command_args.m_nCount > (nArg + 1))
    {
        rtl_uString_assign (strCommandArg, g_command_args.m_ppArgs[nArg + 1]);
        result = osl_Process_E_None;
    }
    osl_releaseMutex(g_command_args.m_mutex);

    return (result);
}

/***************************************
 osl_setCommandArgs().
 **************************************/
void SAL_CALL osl_setCommandArgs (int argc, char ** argv)
{
    OSL_ASSERT(argc > 0);
    osl_acquireMutex(g_command_args.m_mutex);
    OSL_ENSURE (g_command_args.m_nCount == 0, "osl_setCommandArgs(): CommandArgs already set.");
    if (g_command_args.m_nCount == 0)
    {
        rtl_uString** ppArgs = (rtl_uString**)rtl_allocateZeroMemory (argc * sizeof(rtl_uString*));
        if (ppArgs != 0)
        {
            rtl_TextEncoding encoding = osl_getThreadTextEncoding();
            for (int i = 0; i < argc; i++)
            {
                rtl_string2UString (
                    &(ppArgs[i]),
                    argv[i], rtl_str_getLength (argv[i]), encoding,
                    OSTRING_TO_OUSTRING_CVTFLAGS);
            }
            if (ppArgs[0] != 0)
            {
                /* see @ osl_getExecutableFile(). */
                if (rtl_ustr_indexOfChar (rtl_uString_getStr(ppArgs[0]), sal_Unicode('/')) == -1)
                {
                    const rtl::OUString PATH (RTL_CONSTASCII_USTRINGPARAM("PATH"));

                    rtl_uString * pSearchPath = 0;
                    osl_getEnvironment (PATH.pData, &pSearchPath);
                    if (pSearchPath)
                    {
                        rtl_uString * pSearchResult = 0;
                        osl_searchPath (ppArgs[0], pSearchPath, &pSearchResult);
                        if (pSearchResult)
                        {
                            rtl_uString_assign (&(ppArgs[0]), pSearchResult);
                            rtl_uString_release (pSearchResult);
                        }
                        rtl_uString_release (pSearchPath);
                    }
                }

                rtl_uString * pArg0 = 0;
                if (realpath_u (ppArgs[0], &pArg0))
                {
                    osl_getFileURLFromSystemPath (pArg0, &(ppArgs[0]));
                    rtl_uString_release (pArg0);
                }
            }
            g_command_args.m_nCount = argc;
            g_command_args.m_ppArgs = ppArgs;
        }
    }
    osl_releaseMutex(g_command_args.m_mutex);
 
}

/***************************************
 osl_getEnvironment().
 **************************************/
oslProcessError SAL_CALL osl_getEnvironment(rtl_uString* pustrEnvVar, rtl_uString** ppustrValue)
{
    oslProcessError  result   = osl_Process_E_NotFound;
    rtl_TextEncoding encoding = osl_getThreadTextEncoding();
    rtl_String* pstr_env_var  = 0;

    OSL_PRECOND(pustrEnvVar, "osl_getEnvironment(): Invalid parameter");
    OSL_PRECOND(ppustrValue, "osl_getEnvironment(): Invalid parameter");

    rtl_uString2String(
        &pstr_env_var,
        rtl_uString_getStr(pustrEnvVar), rtl_uString_getLength(pustrEnvVar), encoding,
        OUSTRING_TO_OSTRING_CVTFLAGS);
    if (pstr_env_var != 0)
    {
        const char* p_env_var = getenv (rtl_string_getStr (pstr_env_var));
        if (p_env_var != 0)
        {
            rtl_string2UString(
                ppustrValue,
                p_env_var, strlen(p_env_var), encoding,
                OSTRING_TO_OUSTRING_CVTFLAGS);
            OSL_ASSERT(*ppustrValue != NULL);

            result = osl_Process_E_None;
        }
        rtl_string_release(pstr_env_var);
    }

    return (result);
}

/***************************************
 osl_getProcessWorkingDir().
 **************************************/
oslProcessError SAL_CALL osl_getProcessWorkingDir(rtl_uString **ppustrWorkingDir)
{
    oslProcessError result = osl_Process_E_Unknown;
    char buffer[PATH_MAX];
    
    OSL_PRECOND(ppustrWorkingDir, "osl_getProcessWorkingDir(): Invalid parameter");

    if (getcwd (buffer, sizeof(buffer)) != 0)
    {
        rtl_uString* ustrTmp = 0;

        rtl_string2UString( 
            &ustrTmp, 
            buffer, strlen(buffer), osl_getThreadTextEncoding(),
            OSTRING_TO_OUSTRING_CVTFLAGS);
        if (ustrTmp != 0)
        {
            if (osl_getFileURLFromSystemPath (ustrTmp, ppustrWorkingDir) == osl_File_E_None)
                result = osl_Process_E_None;
            rtl_uString_release (ustrTmp);
        }
    }

    return (result);
}

/******************************************************************************
 *
 *              new functions to set/return the current process locale
 *
 *****************************************************************************/

struct ProcessLocale_Impl
{
    oslMutex		m_mutex;
    rtl_Locale *    m_pLocale;
};

static struct ProcessLocale_Impl g_process_locale =
{
    osl_createMutex(),
    0
};

extern "C" void _imp_getProcessLocale( rtl_Locale ** );
extern "C" int  _imp_setProcessLocale( rtl_Locale * );

/**********************************************
 osl_getProcessLocale().
 *********************************************/
oslProcessError SAL_CALL osl_getProcessLocale( rtl_Locale ** ppLocale )
{
    OSL_PRECOND(ppLocale, "osl_getProcessLocale(): Invalid parameter.");

    osl_acquireMutex(g_process_locale.m_mutex);

    if (g_process_locale.m_pLocale == 0)
        _imp_getProcessLocale (&(g_process_locale.m_pLocale));
    *ppLocale = g_process_locale.m_pLocale;

    osl_releaseMutex(g_process_locale.m_mutex);

    return (osl_Process_E_None);
}

/**********************************************
 osl_setProcessLocale().
 *********************************************/
oslProcessError SAL_CALL osl_setProcessLocale( rtl_Locale * pLocale )
{
    oslProcessError result = osl_Process_E_Unknown;

    OSL_PRECOND(pLocale, "osl_setProcessLocale(): Invalid parameter.");

    osl_acquireMutex(g_process_locale.m_mutex);
    if (_imp_setProcessLocale (pLocale) == 0)
    {
        g_process_locale.m_pLocale = pLocale;
        result = osl_Process_E_None;
    }
    osl_releaseMutex(g_process_locale.m_mutex);

    return (result);
}

