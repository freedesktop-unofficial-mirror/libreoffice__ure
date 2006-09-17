/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: testpipe.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 09:10:41 $
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <osl/pipe.h>
#include <osl/process.h>
#include <rtl/ustring.h>

// eindeutiger Name f�r die Pipe
const char pszPipeName[] = "TestPipe";
const char szTestString[] = "This is a test";
char       szBuffer[256];

const char *  cp;
Size_t  n;
sSize_t nChars;

// osl specific variables
oslPipe          Pipe;
oslPipe          C1Pipe;
oslProcess       Process = NULL;
oslProcessError  ProcessError;
oslProcessInfo   ProcessInfo;

void fail( const char * pszText, int retval )
{
    fprintf( stderr, "TestPipe Server: %s", pszText );
    fprintf( stderr, "TestPipe Server: test failed, ErrNo: %d.\n", retval );

    if( Process ) osl_freeProcessHandle( Process );
    exit( retval );
}
    


/*
 * Teste die Pipe-Implementation in osl
 */

int main (int argc, const char *argv[])
{
    // erzeuge die Pipe
    rtl_uString* ustrPipeName=0;
    rtl_uString* ustrExeName=0;
    

    rtl_uString_newFromAscii(&ustrPipeName,pszPipeName);
    rtl_uString_newFromAscii(&ustrExeName, "//./tmp/testpip2.exe");
    
    Pipe = osl_createPipe( ustrPipeName, osl_Pipe_CREATE, 0 );
    
    if( !Pipe )
        fail( "unable to create Pipe.\n",
              osl_getLastPipeError(NULL));

    // starte client process
    ProcessError = osl_executeProcess( ustrExeName,
                                       NULL,
                                       0,
                                        osl_Process_NORMAL,
                                        0,
                                        NULL,
                                       NULL,
                                       0,
                                        NULL,
                                        &Process );

    if( ProcessError != osl_Process_E_None )
        fail( "unable to start client.\n", ProcessError );

    // wait for connection
    C1Pipe = osl_acceptPipe( Pipe );

    if( !C1Pipe )
        fail( "unable to connect to client.\n",
            osl_getLastPipeError( Pipe ));

    
    if( argc > 1 )
    {
        cp = argv[1];
        n  = strlen( cp ) + 1;
    }
    else
    {
        cp = szTestString;
        n  = sizeof(szTestString);
    }
        
    // sende TestString zum Client
    nChars = osl_sendPipe( C1Pipe, cp, n );
    
    if( nChars < 0 )
        fail( "unable to write on pipe.\n",
              osl_getLastPipeError( Pipe ) );
              
    // empfange Daten vom Server
    nChars = osl_receivePipe( C1Pipe, szBuffer, 256 );

    if( nChars < 0 )
        fail( "unable to read from pipe.\n",
              osl_getLastPipeError( C1Pipe ) );

    printf( "TestPipe Server: received data: %s.\n", szBuffer );

    // warte bis das Client-Programm sich beendet
    ProcessError = osl_joinProcess( Process );
    
    if( ProcessError != osl_Process_E_None )
        fail( "unable to wait for client.\n",
              ProcessError );

    // ermittle den R�ckgabewert des Client-Programms
    ProcessInfo.Size = sizeof( ProcessInfo );
    
    ProcessError = osl_getProcessInfo( Process, osl_Process_EXITCODE, &ProcessInfo );
    
    if( ProcessError != osl_Process_E_None )
        fail( "unable to receive return value of client process.\n",
              ProcessError );

    if( ProcessInfo.Code != 0 )
        fail( "client aborted.\n", ProcessInfo.Code );
    
    // gib das Handle fuer den Client-Prozess frei
    osl_freeProcessHandle( Process );

    // schliesse die Pipes
    osl_destroyPipe( C1Pipe );
    osl_destroyPipe( Pipe );

    printf( "TestPipe Server: test passed.\n" );
    return 0;
}

  

