/*************************************************************************
 *
 *  $RCSfile: osl_ConnectorSocket.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: obo $ $Date: 2004-03-19 14:53:24 $
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

#define IP_PORT_MYPORT2 8883
#define IP_PORT_FTP     21
#define IP_PORT_MYPORT3 8884

//------------------------------------------------------------------------
// helper functions
//------------------------------------------------------------------------

class CloseSocketThread : public Thread
{
    ::osl::Socket &m_sSocket;
protected:	
    void SAL_CALL run( )
    {
        thread_sleep( 1 );
        m_sSocket.close( );
    }
public:
    CloseSocketThread(::osl::Socket & sSocket )
        : m_sSocket( sSocket )
    {
    }
    
    ~CloseSocketThread( )
    {
        if ( isRunning( ) )
        {
            t_print("# error: CloseSocketThread not terminated.\n" );
        }
    }
};

namespace osl_ConnectorSocket
{

    /** testing the method:
        ConnectorSocket(oslAddrFamily Family = osl_Socket_FamilyInet, 
                        oslProtocol	Protocol = osl_Socket_ProtocolIp,
                        oslSocketType	Type = osl_Socket_TypeStream);
    */

    class ctors : public CppUnit::TestFixture
    {
    public:
        void ctors_001()
        {
            /// Socket constructor.
            ::osl::ConnectorSocket csSocket( osl_Socket_FamilyInet, osl_Socket_ProtocolIp, osl_Socket_TypeStream );
    
            CPPUNIT_ASSERT_MESSAGE( "test for ctors_001 constructor function: check if the connector socket was created successfully.", 
                                    osl_Socket_TypeStream ==  csSocket.getType( ) );
        }
    
        CPPUNIT_TEST_SUITE( ctors );
        CPPUNIT_TEST( ctors_001 );
        CPPUNIT_TEST_SUITE_END();
        
    }; // class ctors
    
    /** testing the method:
        oslSocketResult SAL_CALL connect(const SocketAddr& TargetHost, const TimeValue* pTimeout = 0);
    */

    class connect : public CppUnit::TestFixture
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
        //	sHandle = osl_createSocket( osl_Socket_FamilyInet, osl_Socket_TypeStream, osl_Socket_ProtocolIp );
        }

        void tearDown( )
        {
            free( pTimeout );
        //	sHandle = NULL;
            asAcceptorSocket.close( );
            csConnectorSocket.close( );
        }

    
        void connect_001()
        {
            ::osl::SocketAddr saLocalSocketAddr( rtl::OUString::createFromAscii("127.0.0.1"), IP_PORT_MYPORT2 );
            ::osl::SocketAddr saTargetSocketAddr( rtl::OUString::createFromAscii("127.0.0.1"), IP_PORT_MYPORT2 );
            ::osl::SocketAddr saPeerSocketAddr( rtl::OUString::createFromAscii("129.158.217.202"), IP_PORT_FTP );
            ::osl::StreamSocket ssConnection;
            
            /// launch server socket 
            asAcceptorSocket.setOption( osl_Socket_OptionReuseAddr, 1 ); //sal_True);
            sal_Bool bOK1 = asAcceptorSocket.bind( saLocalSocketAddr );
            CPPUNIT_ASSERT_MESSAGE( "AcceptorSocket bind address failed.", sal_True == bOK1 );
            sal_Bool bOK2 = asAcceptorSocket.listen( 1 );
            CPPUNIT_ASSERT_MESSAGE( "AcceptorSocket listen failed.",  sal_True == bOK2 );
            
            //asAcceptorSocket.enableNonBlockingMode( sal_True );
            //oslSocketResult eResultAccept = asAcceptorSocket.acceptConnection(ssConnection); /// waiting for incoming connection...
            //CPPUNIT_ASSERT_MESSAGE( "accept failed.",  osl_Socket_Ok == eResultAccept );
            /// launch client socket 
            oslSocketResult eResult = csConnectorSocket.connect( saTargetSocketAddr, pTimeout );   /// connecting to server...
            CPPUNIT_ASSERT_MESSAGE( "connect failed.",  osl_Socket_Ok == eResult );

            /// get peer information
            csConnectorSocket.getPeerAddr( saPeerSocketAddr );/// connected.
            
            CPPUNIT_ASSERT_MESSAGE( "test for connect function: try to create a connection with remote host. and check the setup address.", 
                                    ( sal_True == compareSocketAddr( saPeerSocketAddr, saLocalSocketAddr ) ) &&
                                    ( osl_Socket_Ok == eResult ));
        }
        //non-blocking mode connect?
        void connect_002()
        {
            ::osl::SocketAddr saLocalSocketAddr( rtl::OUString::createFromAscii("127.0.0.1"), IP_PORT_MYPORT3 );
            ::osl::SocketAddr saTargetSocketAddr( rtl::OUString::createFromAscii("127.0.0.1"), IP_PORT_MYPORT3 );
            ::osl::SocketAddr saPeerSocketAddr( rtl::OUString::createFromAscii("129.158.217.202"), IP_PORT_FTP );
                        
            asAcceptorSocket.setOption( osl_Socket_OptionReuseAddr, 1 ); //sal_True);
            asAcceptorSocket.enableNonBlockingMode( sal_True );
            sal_Bool bOK1 = asAcceptorSocket.bind( saLocalSocketAddr );
            CPPUNIT_ASSERT_MESSAGE( "AcceptorSocket bind address failed.", sal_True == bOK1 );
            sal_Bool bOK2 = asAcceptorSocket.listen( 1 );
            CPPUNIT_ASSERT_MESSAGE( "AcceptorSocket listen failed.",  sal_True == bOK2 );
            
            csConnectorSocket.enableNonBlockingMode( sal_True );
            
            oslSocketResult eResult = csConnectorSocket.connect( saTargetSocketAddr, pTimeout );   /// connecting to server...
            CPPUNIT_ASSERT_MESSAGE( "connect failed.",  osl_Socket_InProgress == eResult ||  osl_Socket_Ok == eResult );

            /// get peer information
            csConnectorSocket.getPeerAddr( saPeerSocketAddr );
            
            CPPUNIT_ASSERT_MESSAGE( "test for connect function: try to create a connection with remote host. and check the setup address.", 
                sal_True == compareSocketAddr( saPeerSocketAddr, saLocalSocketAddr  )  ) ;
        }
        // really an error or just delayed
        // how to design senarios that will return osl_Socket_Interrupted, osl_Socket_TimedOut
        void connect_003()
        {
            ::osl::SocketAddr saTargetSocketAddr1( rtl::OUString::createFromAscii("127.0.0.1"), IP_PORT_MYPORT3 );
            ::osl::SocketAddr saTargetSocketAddr2( rtl::OUString::createFromAscii("123.345.67.89"), IP_PORT_MYPORT3 );
                        
            csConnectorSocket.enableNonBlockingMode( sal_False );
            
            oslSocketResult eResult1 = csConnectorSocket.connect( saTargetSocketAddr1, pTimeout );  
            oslSocketResult eResult2 = csConnectorSocket.connect( saTargetSocketAddr2, pTimeout ); 
            CloseSocketThread myCloseThread( csConnectorSocket );
            oslSocketResult eResult3 = csConnectorSocket.connect( saTargetSocketAddr2, pTimeout );
            myCloseThread.join();
            CPPUNIT_ASSERT_MESSAGE( "connect should failed.",  osl_Socket_Error == eResult1 && 
                osl_Socket_Error == eResult2 &&  osl_Socket_Error == eResult3 );

        }
        
        // really an error in non-blocking mode
        void connect_004()
        {
            ::osl::SocketAddr saTargetSocketAddr( rtl::OUString::createFromAscii("123.345.67.89"), IP_PORT_MYPORT3 );
                        
            csConnectorSocket.enableNonBlockingMode( sal_True );
            
            oslSocketResult eResult = csConnectorSocket.connect( saTargetSocketAddr, pTimeout );   /// connecting to server...
            CPPUNIT_ASSERT_MESSAGE( "connect should failed.",  osl_Socket_Error == eResult );
        }
        /** here need a case: immediate connection, say in non-blocking mode connect return osl_Socket_Ok
        */

        CPPUNIT_TEST_SUITE( connect );
        CPPUNIT_TEST( connect_001 );
        CPPUNIT_TEST( connect_002 );
        CPPUNIT_TEST( connect_003 );
        CPPUNIT_TEST( connect_004 );
        CPPUNIT_TEST_SUITE_END();
        
    }; // class connect

    
// -----------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(osl_ConnectorSocket::ctors, "osl_ConnectorSocket"); 
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(osl_ConnectorSocket::connect, "osl_ConnectorSocket"); 

} // namespace osl_ConnectorSocket

// -----------------------------------------------------------------------------

// this macro creates an empty function, which will called by the RegisterAllFunctions()
// to let the user the possibility to also register some functions by hand.
NOADDITIONAL;
