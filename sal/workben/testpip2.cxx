/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: testpip2.cxx,v $
 * $Revision: 1.4 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sal.hxx"

#include <stdio.h>
#include <stdlib.h>

#define INCL_DOSPROCESS
#include <osl/pipe.h>
#include <os2.h>

// eindeutiger Name f�r die Pipe
const char pszPipeName[] = "TestPipe";

oslPipe Pipe;


void fail( const char * pszText, int retval )
{
    fprintf( stderr, "TestPipe Client: %s", pszText );
    fprintf( stderr, "TestPipe Client: test failed, ErrNo: %d.\n", retval );
    exit( retval );
}
    


/*
 * Teste die Pipe-Implementation in osl
 */

int main (void)
{
    char    szBuffer[ 256 ];
    sSize_t nChars;
    
    // gib dem Server die Chance, die Pipe zu �ffnen
    DosSleep( 1000 );
    
    // erzeuge die Pipe
    Pipe = osl_createPipe( pszPipeName, osl_Pipe_OPEN, 0 );
    
    if( !Pipe )
        fail( "unable to open pipe.\n",
              osl_getLastPipeError(NULL));


    // empfange Daten vom Server
    nChars = osl_receivePipe( Pipe, szBuffer, 256 );

    if( nChars < 0 )
        fail( "unable to read from pipe.\n",
              osl_getLastPipeError( Pipe ) );

    printf( "TestPipe Client: data received: %s.\n", szBuffer );

    // Sende die Daten wieder zur�ck.
    nChars = osl_sendPipe( Pipe, szBuffer, nChars );
    
    if( nChars < 0 )
        fail( "unable to write on pipe.\n",
              osl_getLastPipeError( Pipe ) );
              
    // schliesse die Pipe
    osl_destroyPipe( Pipe );

    printf( "TestPipe Client: test passed.\n" );
    return 0;
}



