/*************************************************************************
 *
 *  $RCSfile: acceptor.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: jbu $ $Date: 2001-06-22 16:32:55 $
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

#ifndef _OSL_PIPE_HXX_
#include <osl/pipe.hxx>
#endif

#ifndef _OSL_SOCKET_HXX_
#include <osl/socket.hxx>
#endif

#ifndef _RTL_UNLOAD_H_
#include <rtl/unload.h>
#endif

#include <com/sun/star/connection/XConnection.hpp>

namespace io_acceptor {

    extern rtl_StandardModuleCount g_moduleCount;

    class PipeAcceptor
    {
    public:
        PipeAcceptor( const ::rtl::OUString &sPipeName , const ::rtl::OUString &sConnectionDescription );
        
        void init();
        ::com::sun::star::uno::Reference < ::com::sun::star::connection::XConnection >  accept(  );
        
        void stopAccepting();
        
        ::osl::Pipe m_pipe;
        ::rtl::OUString m_sPipeName;
        ::rtl::OUString m_sConnectionDescription;
        sal_Bool m_bClosed;
    };
    
    class SocketAcceptor
    {
    public:
        SocketAcceptor( const ::rtl::OUString & sSocketName ,
                        sal_uInt16 nPort,
                        sal_Bool bTcpNoDelay,
                        const ::rtl::OUString &sConnectionDescription );

        void init();
        ::com::sun::star::uno::Reference < ::com::sun::star::connection::XConnection > accept();

        void stopAccepting();
        
        ::osl::SocketAddr m_addr;
        ::osl::AcceptorSocket m_socket;
        ::rtl::OUString m_sSocketName;
        ::rtl::OUString m_sConnectionDescription;
        sal_uInt16 m_nPort;
        sal_Bool m_bTcpNoDelay;
        sal_Bool m_bClosed;
    };
    
};

