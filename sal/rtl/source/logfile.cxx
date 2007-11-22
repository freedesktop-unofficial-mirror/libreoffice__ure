/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: logfile.cxx,v $
 *
 *  $Revision: 1.12 $
 *
 *  last change: $Author: ihi $ $Date: 2007-11-22 12:21:18 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sal.hxx"
#include <cstdarg>
#include <cstdio>
#include <stdio.h>
#include <stdarg.h>

#include <rtl/logfile.h>

#ifndef _OSL_PROCESS_H_
#include <osl/process.h>
#endif
#ifndef _OSL_FILE_H_
#include <osl/time.h>
#endif
#ifndef _OSL_TIME_H_
#include <osl/time.h>
#endif
#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif
#ifndef _RTL_BOOTSTRAP_H_
#include <rtl/bootstrap.h>
#endif
#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif
#ifndef _RTL_STRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif
#ifndef _RTL_ALLOC_H_
#include <rtl/alloc.h>
#endif
#include "osl/thread.h"

#include <algorithm>

#ifdef _MSC_VER
#define vsnprintf _vsnprintf
#endif

using namespace rtl;
using namespace osl;
using namespace std;

namespace {

static oslFileHandle g_aFile = 0;
static sal_Bool g_bHasBeenCalled = sal_False;
static const sal_Int32 g_BUFFERSIZE = 4096;
static sal_Char *g_buffer = 0;	
    
class	LoggerGuard
{
public:
    ~LoggerGuard();
};

LoggerGuard::~LoggerGuard()
{
    if( g_buffer )
    {
        sal_Int64 nWritten, nConverted =
            sprintf( g_buffer, "closing log file at %06" SAL_PRIuUINT32, osl_getGlobalTimer() );
        if( nConverted > 0 )
            osl_writeFile( g_aFile, g_buffer, nConverted, (sal_uInt64 *)&nWritten );
        osl_closeFile( g_aFile );
        g_aFile = 0;
        
        rtl_freeMemory( g_buffer );
        g_buffer = 0;
        g_bHasBeenCalled = sal_False;
    }
}

// The destructor of this static LoggerGuard is "activated" by the assignment to
// g_buffer in init():
LoggerGuard loggerGuard;

Mutex & getLogMutex()
{
    static Mutex *pMutex = 0;
    if( !pMutex )
    {
        MutexGuard guard( Mutex::getGlobalMutex() );
        if( ! pMutex )
        {
            static Mutex mutex;
            pMutex = &mutex;
        }
    }
    return *pMutex;
}

OUString getFileUrl( const OUString &name )
{
    OUString aRet;
    if ( osl_getFileURLFromSystemPath( name.pData, &aRet.pData )
         != osl_File_E_None )
    {
        OSL_ASSERT( false );
    }

    OUString aWorkingDirectory;
    osl_getProcessWorkingDir( &(aWorkingDirectory.pData) );
    osl_getAbsoluteFileURL( aWorkingDirectory.pData, aRet.pData, &(aRet.pData) );

    return aRet;
}

void init() {
    if( !g_bHasBeenCalled )
    {
        MutexGuard guard( getLogMutex() );
        if( ! g_bHasBeenCalled )
        {
            OUString name( RTL_CONSTASCII_USTRINGPARAM( "RTL_LOGFILE" ) );
            OUString value;
            if( rtl_bootstrap_get( name.pData, &value.pData, 0 ) )
            {
                //	Obtain process id.
                oslProcessIdentifier aProcessId = 0;
                oslProcessInfo info;
                info.Size = sizeof (oslProcessInfo);
                if (osl_getProcessInfo (0, osl_Process_IDENTIFIER, &info) == osl_Process_E_None)
                    aProcessId = info.Ident;
                
                //	Construct name of log file and open the file.
                OUStringBuffer buf( 128 );
                buf.append( value );
                
                // if the filename ends with .nopid, the incoming filename is not modified
                if( value.getLength() < 6 /* ".nopid" */ ||
                    rtl_ustr_ascii_compare_WithLength(
                        value.getStr() + (value.getLength()-6) , 6 , ".nopid" ) )
                {
                    buf.appendAscii( "_" );
                    buf.append( (sal_Int32) aProcessId );
                    buf.appendAscii( ".log" );
                }

                OUString o = getFileUrl( buf.makeStringAndClear() );
                oslFileError e = osl_openFile(
                    o.pData, &g_aFile, osl_File_OpenFlag_Write|osl_File_OpenFlag_Create);

                if( osl_File_E_None == e )
                {
                    TimeValue aCurrentTime;
                    g_buffer = ( sal_Char * ) rtl_allocateMemory( g_BUFFERSIZE );
                    sal_Int64 nConverted = 0;
                    if (osl_getSystemTime (&aCurrentTime))
                    {
                        nConverted = (sal_Int64 ) sprintf (
                                g_buffer,
                                "opening log file %f seconds past January 1st 1970\n"
                                "corresponding to %" SAL_PRIuUINT32 " ms after timer start\n",
                                aCurrentTime.Seconds + 1e-9 * aCurrentTime.Nanosec,
                                osl_getGlobalTimer());

                        if( nConverted > 0 )
                        {
                            sal_Int64 nWritten;
                            osl_writeFile( g_aFile, g_buffer, nConverted , (sal_uInt64 *)&nWritten );
                        }
                    }
                    
                    nConverted = sprintf (g_buffer, "Process id is %" SAL_PRIuUINT32 "\n", aProcessId);
                    if( nConverted )
                    {
                        sal_Int64 nWritten;
                        osl_writeFile( g_aFile, g_buffer, nConverted, (sal_uInt64 *)&nWritten );
                    }
                }
                else
                {
                    OSL_TRACE( "Couldn't open logfile %s(%d)" , o.getStr(), e );
                }
            }
            g_bHasBeenCalled = sal_True;
        }
    }
}

}

extern "C" void	SAL_CALL rtl_logfile_trace	( const char *pszFormat, ... )
{
    init();
    if( g_buffer )
    {
        va_list args;
        va_start(args, pszFormat);
        {
            sal_Int64 nConverted, nWritten;
            MutexGuard guard( getLogMutex() );
            nConverted = vsnprintf( g_buffer , g_BUFFERSIZE, pszFormat, args );
            nConverted = (nConverted > g_BUFFERSIZE ? g_BUFFERSIZE : nConverted );
            if( nConverted > 0 )
                osl_writeFile( g_aFile, g_buffer, nConverted, (sal_uInt64*)&nWritten );
        }
        va_end(args);
    }
}

extern "C" void SAL_CALL rtl_logfile_longTrace(char const * format, ...) {
    init();
    if (g_buffer != 0) {
        sal_uInt32 time = osl_getGlobalTimer();
        oslThreadIdentifier threadId = osl_getThreadIdentifier(0);
        va_list args;
        va_start(args, format);
        {
            MutexGuard g(getLogMutex());
            int n1 = snprintf(
                g_buffer, g_BUFFERSIZE, "%06" SAL_PRIuUINT32 " %" SAL_PRIuUINT32 " ", time, threadId);
            if (n1 >= 0) {
                sal_uInt64 n2;
                osl_writeFile(
                    g_aFile, g_buffer,
                    static_cast< sal_uInt64 >(
                        std::min(n1, static_cast< int >(g_BUFFERSIZE))),
                    &n2);
                n1 = vsnprintf(g_buffer, g_BUFFERSIZE, format, args);
                if (n1 > 0) {
                    osl_writeFile(
                        g_aFile, g_buffer,
                        static_cast< sal_uInt64 >(
                            std::min(n1, static_cast< int >(g_BUFFERSIZE))),
                        &n2);
                }
            }
        }
        va_end(args);
    }
}
