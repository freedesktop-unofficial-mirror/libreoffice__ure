/*************************************************************************
 *
 *  $RCSfile: osl_AcceptorSocket.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: obo $ $Date: 2004-01-05 21:20:55 $
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
 *  Copyright 2000 by Sun Microsystems, 
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

/**  test coder preface:
    1. the BSD socket function will meet "unresolved external symbol error" on Windows platform
    if you are not including ws2_32.lib in makefile.mk,  the including format will be like this:
    
    .IF "$(GUI)" == "WNT"
    SHL1STDLIBS +=	$(SOLARLIBDIR)$/cppunit.lib
    SHL1STDLIBS +=  ws2_32.lib
    .ENDIF

    likewise on Solaris platform.
    .IF "$(GUI)" == "UNX"
    SHL1STDLIBS+=$(SOLARLIBDIR)$/libcppunit$(DLLPOSTFIX).a
    SHL1STDLIBS += -lsocket -ldl -lnsl
    .ENDIF

    2. since the Socket implementation of osl is only IPv4 oriented, our test are mainly focus on IPv4
    category.

    3. some fragment of Socket source implementation are lack of comment so it is hard for testers 
    guess what the exact functionality or usage of a member.  Hope the Socket section's comment 
    will be added. 

    4. following functions are declared but not implemented:
    inline sal_Bool SAL_CALL operator== (const SocketAddr & Addr) const; 
 */

//------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------

#include <cppunit/simpleheader.hxx>

#include "osl_Socket_Const.h"
#include "sockethelper.hxx"

using namespace osl;
using namespace rtl;

#define IP_PORT_FTP     21
#define IP_PORT_MYPORT9 8897
#define IP_PORT_MYPORT4 8885
#define IP_PORT_MYPORT3 8884

//------------------------------------------------------------------------
// helper functions
//------------------------------------------------------------------------

// just used to test socket::close() when accepting
class AcceptorThread : public Thread
{
    ::osl::AcceptorSocket asAcceptorSocket;
    ::rtl::OUString aHostIP;
    sal_Bool bOK;
protected:	
    void SAL_CALL run( )
    {
        ::osl::SocketAddr saLocalSocketAddr( aHostIP, IP_PORT_MYPORT9 );
        ::osl::StreamSocket ssStreamConnection;
        
        asAcceptorSocket.setOption( osl_Socket_OptionReuseAddr, 1 ); //integer not sal_Bool : sal_True);
        sal_Bool bOK1 = asAcceptorSocket.bind( saLocalSocketAddr );
        if  ( sal_True != bOK1 )
        {
            printf( "# AcceptorSocket bind address failed.\n" ) ;
            return;
        }
        sal_Bool bOK2 = asAcceptorSocket.listen( 1 );
        if  ( sal_True != bOK2 )
        { 
            printf( "# AcceptorSocket listen address failed.\n" ) ;
            return;
        }

        asAcceptorSocket.enableNonBlockingMode( sal_False );
        
        oslSocketResult eResult = asAcceptorSocket.acceptConnection( ssStreamConnection );
        if (eResult != osl_Socket_Ok )
        {
            bOK = sal_True;
            printf("# AcceptorThread: acceptConnection failed! \n");			
        }	
    }
public:
    AcceptorThread(::osl::AcceptorSocket & asSocket, ::rtl::OUString const& aBindIP )
        : asAcceptorSocket( asSocket ), aHostIP( aBindIP )
    {
        bOK = sal_False;
    }
    
    sal_Bool isOK() { return bOK; }
    
    ~AcceptorThread( )
    {
        if ( isRunning( ) )
        {
            asAcceptorSocket.shutdown();
            printf( "# error: Acceptor thread not terminated.\n" );
        }
    }
};

namespace osl_AcceptorSocket
{

    /** testing the methods:
        inline AcceptorSocket(oslAddrFamily Family = osl_Socket_FamilyInet, 
                              oslProtocol	Protocol = osl_Socket_ProtocolIp,
                              oslSocketType	Type = osl_Socket_TypeStream);
    */

    class ctors : public CppUnit::TestFixture
    {
    public:
        
        void ctors_001()
        {
            /// Socket constructor.
            ::osl::AcceptorSocket asSocket( osl_Socket_FamilyInet, osl_Socket_ProtocolIp, osl_Socket_TypeStream );
    
            CPPUNIT_ASSERT_MESSAGE( "test for ctors_001 constructor function: check if the acceptor socket was created successfully.", 
                                    osl_Socket_TypeStream ==  asSocket.getType( ) );
        }
        
        CPPUNIT_TEST_SUITE( ctors );
        CPPUNIT_TEST( ctors_001 );
        CPPUNIT_TEST_SUITE_END();
        
    }; // class ctors
    
#if 0  /* OBSOLETE */
    class operator_assign : public CppUnit::TestFixture
    {
    public:
        
        void assign_001()
        {
#if defined(LINUX)
            ::osl::AcceptorSocket asSocket( osl_Socket_FamilyInet, osl_Socket_ProtocolIp, osl_Socket_TypeStream );
            ::osl::AcceptorSocket asSocketAssign( osl_Socket_FamilyInet, osl_Socket_ProtocolIp, osl_Socket_TypeStream );
            asSocket.setOption( osl_Socket_OptionReuseAddr, 1);
            ::osl::SocketAddr saSocketAddr( rtl::OUString::createFromAscii("127.0.0.1"), IP_PORT_MYPORT4 );			
            asSocket.bind( saSocketAddr );
                        
            AcceptorThread myAcceptorThread( asSocketAssign, rtl::OUString::createFromAscii("127.0.0.1") );
            myAcceptorThread.create();
            
            thread_sleep( 1 );
            //when accepting, assign another socket to the socket, the thread will not be closed, so is blocking
            asSocketAssign = asSocket;
            
            printf("#asSocketAssign port number is %d\n", asSocketAssign.getLocalPort() );
            
            asSocketAssign.shutdown();
            myAcceptorThread.join();
                        
            CPPUNIT_ASSERT_MESSAGE( "test for close when is accepting: the socket will quit accepting status.", 
                                myAcceptorThread.isOK()	== sal_True );
            
            
#endif /* LINUX */
        }
        
            
        CPPUNIT_TEST_SUITE( operator_assign  );
        CPPUNIT_TEST( assign_001 );
        CPPUNIT_TEST_SUITE_END();
        
    }; // class operator_assign
#endif /* OBSOLETE */
    
    /** testing the method:
        inline sal_Bool SAL_CALL listen(sal_Int32 MaxPendingConnections= -1);
        inline oslSocketResult SAL_CALL acceptConnection( StreamSocket& Connection);
        inline oslSocketResult SAL_CALL acceptConnection( StreamSocket&	Connection, SocketAddr & PeerAddr);
    */

    class listen_accept : public CppUnit::TestFixture
    {
    public:
        TimeValue *pTimeout;
        ::osl::AcceptorSocket asAcceptorSocket;
        ::osl::ConnectorSocket csConnectorSocket;
        
        
        // initialization
        void setUp( )
        {
            pTimeout  = ( TimeValue* )malloc( sizeof( TimeValue ) );
            pTimeout->Seconds = 3;
            pTimeout->Nanosec = 0;
            asAcceptorSocket.setOption( osl_Socket_OptionReuseAddr, 1);
        //	sHandle = osl_createSocket( osl_Socket_FamilyInet, osl_Socket_TypeStream, osl_Socket_ProtocolIp );
        }

        void tearDown( )
        {
            free( pTimeout );
        //	sHandle = NULL;
            asAcceptorSocket.close( );
            csConnectorSocket.close( );
        }

    
        void listen_accept_001()
        {
            ::osl::SocketAddr saLocalSocketAddr( rtl::OUString::createFromAscii("127.0.0.1"), IP_PORT_MYPORT3 );
            ::osl::SocketAddr saTargetSocketAddr( rtl::OUString::createFromAscii("127.0.0.1"), IP_PORT_MYPORT3 );
            ::osl::StreamSocket ssConnection;
            
            /// launch server socket 			
            sal_Bool bOK1 = asAcceptorSocket.bind( saLocalSocketAddr );
            CPPUNIT_ASSERT_MESSAGE( "AcceptorSocket bind address failed.", sal_True == bOK1 );
            sal_Bool bOK2 = asAcceptorSocket.listen( 1 );
            CPPUNIT_ASSERT_MESSAGE( "AcceptorSocket listen failed.",  sal_True == bOK2 );
            asAcceptorSocket.enableNonBlockingMode( sal_True );

            /// launch client socket 
            csConnectorSocket.connect( saTargetSocketAddr, pTimeout );   /// connecting to server...

            oslSocketResult eResult = asAcceptorSocket.acceptConnection(ssConnection); /// waiting for incoming connection...
            
            CPPUNIT_ASSERT_MESSAGE( "test for listen_accept function: try to create a connection with remote host, using listen and accept.", 
                ( osl_Socket_Ok == eResult ) );
        }

        void listen_accept_002()
        {
            ::osl::SocketAddr saLocalSocketAddr( rtl::OUString::createFromAscii("127.0.0.1"), IP_PORT_MYPORT4 );
            ::osl::SocketAddr saTargetSocketAddr( rtl::OUString::createFromAscii("127.0.0.1"), IP_PORT_MYPORT4 );
            ::osl::SocketAddr saPeerSocketAddr( rtl::OUString::createFromAscii("129.158.217.202"), IP_PORT_FTP );
            ::osl::StreamSocket ssConnection;
            
            /// launch server socket 
            sal_Bool bOK1 = asAcceptorSocket.bind( saLocalSocketAddr );
            CPPUNIT_ASSERT_MESSAGE( "AcceptorSocket bind address failed.", sal_True == bOK1 );
            sal_Bool bOK2 = asAcceptorSocket.listen( 1 );
            CPPUNIT_ASSERT_MESSAGE( "AcceptorSocket listen failed.",  sal_True == bOK2 );
            asAcceptorSocket.enableNonBlockingMode( sal_True );

            /// launch client socket 
            csConnectorSocket.connect( saTargetSocketAddr, pTimeout );   /// connecting to server...

            oslSocketResult eResult = asAcceptorSocket.acceptConnection(ssConnection, saPeerSocketAddr); /// waiting for incoming connection...
            
            CPPUNIT_ASSERT_MESSAGE( "test for listen_accept function: try to create a connection with remote host, using listen and accept, accept with peer address.", 
                                    ( sal_True == bOK2 ) &&
                                    ( osl_Socket_Ok == eResult ) && 
                                    ( sal_True == compareSocketAddr( saPeerSocketAddr, saLocalSocketAddr ) ) );
        }
        
            
        CPPUNIT_TEST_SUITE( listen_accept );
        CPPUNIT_TEST( listen_accept_001 );
        CPPUNIT_TEST( listen_accept_002 );
        CPPUNIT_TEST_SUITE_END();
        
    }; // class listen_accept

    
// -----------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(osl_AcceptorSocket::ctors, "osl_AcceptorSocket"); 
//CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(osl_AcceptorSocket::operator_assign, "osl_AcceptorSocket");
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(osl_AcceptorSocket::listen_accept, "osl_AcceptorSocket"); 

} // namespace osl_AcceptorSocket

// -----------------------------------------------------------------------------

// this macro creates an empty function, which will called by the RegisterAllFunctions()
// to let the user the possibility to also register some functions by hand.
NOADDITIONAL;
