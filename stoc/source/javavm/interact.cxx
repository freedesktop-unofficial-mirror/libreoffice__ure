/*************************************************************************
 *
 *  $RCSfile: interact.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: jl $ $Date: 2002-09-25 15:37:49 $
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
#include "interact.hxx"

#ifndef _COM_SUN_STAR_JAVA_JAVADISABLEDEXCEPTION_HPP_
#include <com/sun/star/java/JavaDisabledException.hpp>
#endif
#ifndef _COM_SUN_STAR_JAVA_JAVAVMCREATIONFAILUREEXCEPTION_HPP_
#include <com/sun/star/java/JavaVMCreationFailureException.hpp>
#endif


namespace stoc_javavm
{

InteractionAbort::InteractionAbort( const WeakReference<XInterface>& xJVM,
                                    JavaVirtualMachine_Impl * _pJVM):
    m_xJVM(xJVM),
    m_pJVM(_pJVM)
{
    OSL_ASSERT( _pJVM);
}

void SAL_CALL InteractionAbort::select(  ) throw (RuntimeException)
{
    //If we get a hard reference to JavaVirtualMachine_Impl
    //then we may call the callback
    Reference<XInterface> xIntJVM= m_xJVM;
    if( xIntJVM.is())
        m_pJVM->selectAbort();
}
//================================================================================
InteractionRetry::InteractionRetry( const WeakReference<XInterface>& xJVM,
                                    JavaVirtualMachine_Impl * _pJVM):
    m_xJVM(xJVM),
    m_pJVM(_pJVM)
{
    OSL_ASSERT( _pJVM);
}

void SAL_CALL InteractionRetry::select(  ) throw (RuntimeException)
{
    //If we get a hard reference to JavaVirtualMachine_Impl
    //then we may call the callback
    Reference<XInterface> xIntJVM= m_xJVM;
    if( xIntJVM.is())
        m_pJVM->selectRetry();
}
//================================================================================
InteractionRequest::InteractionRequest( const Reference<XInterface>& xJVM,
                                        JavaVirtualMachine_Impl* _pJVM,
                                        Any& _request):
    m_xJVM(xJVM),
    m_pJVM(_pJVM),
    m_anyRequest(_request),
    m_pseqContinuations(NULL)
{
}

Any SAL_CALL InteractionRequest::getRequest(  ) throw (RuntimeException)
{
    return m_anyRequest;
}

Sequence< Reference< XInteractionContinuation > >
SAL_CALL InteractionRequest::getContinuations(  ) throw (RuntimeException)
{
    if ( ! m_pseqContinuations)
    {
        MutexGuard guard(javavm_getMutex());
        if( ! m_pseqContinuations)
        {
#ifdef LINUX
            // Only if java is disabled we allow retry
            if( m_anyRequest.getValueType() == getCppuType( (JavaDisabledException*)0))
            {
                Reference<XInteractionContinuation> arObjs[2];
                arObjs[0]= Reference< XInteractionContinuation >(
                    static_cast<XWeak*> (new InteractionRetry(m_xJVM, m_pJVM)), UNO_QUERY);
                arObjs[1]= Reference< XInteractionContinuation> (
                    static_cast<XWeak*> (new InteractionAbort(m_xJVM, m_pJVM)), UNO_QUERY);
                m_seqContinuations= Sequence< Reference< XInteractionContinuation> >( arObjs, 2);
            }
            else
            {
                Reference<XInteractionContinuation> arObjs[1];
                arObjs[0]= Reference< XInteractionContinuation> (
                    static_cast<XWeak*> (new InteractionAbort(m_xJVM, m_pJVM)), UNO_QUERY);
                m_seqContinuations= Sequence< Reference< XInteractionContinuation> >( arObjs, 1);
            }
#else
            // If the creation of JVM failed then don't offer retry, because java might crash
            // next time
            if( m_anyRequest.getValueType() == getCppuType( (JavaVMCreationFailureException*)0))
            {
                Reference<XInteractionContinuation> arObjs[1];
                arObjs[0]= Reference< XInteractionContinuation> (
                    static_cast<XWeak*> (new InteractionAbort(m_xJVM, m_pJVM)), UNO_QUERY);
                m_seqContinuations= Sequence< Reference< XInteractionContinuation> >( arObjs, 1);
            }
            else
            {
                Reference<XInteractionContinuation> arObjs[2];
                arObjs[0]= Reference< XInteractionContinuation >(
                    static_cast<XWeak*> (new InteractionRetry(m_xJVM, m_pJVM)), UNO_QUERY);
                arObjs[1]= Reference< XInteractionContinuation> (
                    static_cast<XWeak*> (new InteractionAbort(m_xJVM, m_pJVM)), UNO_QUERY);
                m_seqContinuations= Sequence< Reference< XInteractionContinuation> >( arObjs, 2);
            }
#endif            
        }
    }
    return m_seqContinuations;
}



} // end namespace
