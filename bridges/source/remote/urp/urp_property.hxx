/*************************************************************************
 *
 *  $RCSfile: urp_property.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: svesik $ $Date: 2004-04-21 13:46:04 $
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
#ifndef _URP_PROPERTY_HXX_
#define _URP_PROPERTY_HXX_
#ifndef _BRIDGES_REMOTE_REMOTE_H_
#include <bridges/remote/remote.h>
#endif

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif

#ifndef _RTL_BYTESEQ_HXX_
#include <rtl/byteseq.hxx>
#endif

namespace bridges_urp
{	
    struct Properties
    {
        ::rtl::ByteSequence seqBridgeID;
        sal_Int32           nTypeCacheSize;
        sal_Int32           nOidCacheSize;
        sal_Int32           nTidCacheSize;
        ::rtl::OUString     sSupportedVersions;
        ::rtl::OUString     sVersion;
        sal_Int32           nFlushBlockSize;
        sal_Int32           nOnewayTimeoutMUSEC;
        sal_Bool            bSupportsMustReply;
        sal_Bool            bSupportsSynchronous;
        sal_Bool            bSupportsMultipleSynchronous;
        sal_Bool            bClearCache;
        sal_Bool            bNegotiate;
        sal_Bool            bForceSynchronous;

        inline Properties()
            : nTypeCacheSize( 256 )
            , nOidCacheSize( 256 )
            , nTidCacheSize( 256 )
            , sSupportedVersions( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "1.0" ) ) )
            , sVersion( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "1.0" )))
            , nFlushBlockSize( 4*1024 )
            , nOnewayTimeoutMUSEC( 10000 )
            , bSupportsMustReply( sal_False )
            , bSupportsSynchronous( sal_False )
            , bSupportsMultipleSynchronous( sal_False )
            , bClearCache( sal_False )
            , bNegotiate( sal_True )
            , bForceSynchronous( sal_False )
            {}
        
        inline Properties & SAL_CALL operator = ( const Properties &props )
        {
            seqBridgeID                  = props.seqBridgeID;
            nTypeCacheSize               = props.nTypeCacheSize;
            nOidCacheSize                = props.nOidCacheSize;
            nTidCacheSize                = props.nTidCacheSize;
            sSupportedVersions           = props.sSupportedVersions;
            sVersion                     = props.sVersion;
            nFlushBlockSize              = props.nFlushBlockSize;
            nOnewayTimeoutMUSEC          = props.nOnewayTimeoutMUSEC;
            bSupportsMustReply           = props.bSupportsMustReply;
            bSupportsSynchronous         = props.bSupportsSynchronous;
            bSupportsMultipleSynchronous = props.bSupportsMultipleSynchronous;
            bClearCache                  = props.bClearCache;
            bNegotiate                   = props.bNegotiate;
            bForceSynchronous            = props.bForceSynchronous;
            return *this;
        }
    };
} // end namespace bridges_urp
#endif
