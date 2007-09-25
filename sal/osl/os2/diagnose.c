/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: diagnose.c,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: vg $ $Date: 2007-09-25 09:50:31 $
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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <osl/diagnose.h>
#include <osl/thread.h>

BYTE oslTraceEnv[] = "OSL_TRACE_TO_FILE";

typedef pfunc_osl_printDebugMessage oslDebugMessageFunc;
static oslDebugMessageFunc volatile g_pDebugMessageFunc = 0;

typedef pfunc_osl_printDetailedDebugMessage oslDetailedDebugMessageFunc;
static oslDetailedDebugMessageFunc volatile g_pDetailedDebugMessageFunc = 0;

/*----------------------------------------------------------------------------*/

void SAL_CALL osl_breakDebug()
{
    int* sigsev = 0;
    *sigsev = 0;
}

/************************************************************************/
/* osl_trace */
/************************************************************************/
/* comment this define to stop output thread identifier*/
#define OSL_TRACE_THREAD 1
void SAL_CALL osl_trace (
    const sal_Char* lpszFormat, ...)
{
    va_list args;

#if defined(OSL_PROFILING)
    fprintf(stderr, "Time: %06lu : ", osl_getGlobalTimer() );
#else
#if defined(OSL_TRACE_THREAD)
    fprintf(stderr,"Thread: %6d :",osl_getThreadIdentifier(NULL));
#else
    fprintf(stderr, "Trace Message: ");
#endif
#endif

    va_start(args, lpszFormat);
    vfprintf(stderr, lpszFormat, args);
    va_end(args);

    fprintf(stderr,"\n");
    fflush(stderr);
}

/*----------------------------------------------------------------------------*/

void SAL_CALL osl_trace__yd_os2(const sal_Char* lpszFormat, ...)
{

    int     nBuf;
    sal_Char    szBuffer[512];
    sal_Char    szPID[ 12 ];
    va_list args;
    FILE*   pFile;
    PID     pid;
    PSZ     pszOslTraceFile;

    /* if environment variable not set, do nothing */
    if(DosScanEnv(oslTraceEnv, (PSZ*)&pszOslTraceFile))
    {
        return;
    }
    
    va_start(args, lpszFormat);

    nBuf = vsprintf(szBuffer, lpszFormat, args);
    OSL_ASSERT(nBuf < sizeof(szBuffer));

    va_end(args);

    /* get process ID */
    {
    PTIB pptib = NULL;
    PPIB pppib = NULL;

    DosGetInfoBlocks( &pptib, &pppib );
    pid = pppib->pib_ulpid;
    }

    pFile = fopen( (const char*)pszOslTraceFile, "a+" );
    fputs(_itoa( pid, szPID, 10 ), pFile );
    fputs( ": ", pFile );
    fputs(szBuffer, pFile);
    fclose( pFile );

}

/*----------------------------------------------------------------------------*/

sal_Bool SAL_CALL osl_assertFailedLine( const sal_Char* pszFileName, sal_Int32 nLine, const sal_Char* pszMessage)
{
    sal_Char szMessage[512];

    /* get app name or NULL if unknown (don't call assert) */
    sal_Char* lpszAppName = "OSL";

    /* format message into buffer */
    sprintf(szMessage, "Assertion Failed: %s: File %s, Line %d:\n",
            lpszAppName, pszFileName, nLine);
    if(pszMessage != 0)
        strcat( szMessage, pszMessage );

    szMessage[sizeof(szMessage)-1] = '\0';

    fputs(szMessage, stderr);

    return sal_True;   /* abort */
}

/*----------------------------------------------------------------------------*/

sal_Int32 SAL_CALL osl_reportError(sal_uInt32 nType, const sal_Char* pszMessage)
{
    fputs(pszMessage, stderr);

    return 0;
}

/*----------------------------------------------------------------------------*/


/************************************************************************/
/* osl_setDebugMessageFunc */
/************************************************************************/
oslDebugMessageFunc SAL_CALL osl_setDebugMessageFunc (
    oslDebugMessageFunc pNewFunc)
{
    oslDebugMessageFunc pOldFunc = g_pDebugMessageFunc;
    g_pDebugMessageFunc = pNewFunc;
    return pOldFunc;
}

/************************************************************************/
/* osl_setDetailedDebugMessageFunc */
/************************************************************************/
pfunc_osl_printDetailedDebugMessage SAL_CALL osl_setDetailedDebugMessageFunc (
    pfunc_osl_printDetailedDebugMessage pNewFunc)
{
    oslDetailedDebugMessageFunc pOldFunc = g_pDetailedDebugMessageFunc;
    g_pDetailedDebugMessageFunc = pNewFunc;
    return pOldFunc;
}
