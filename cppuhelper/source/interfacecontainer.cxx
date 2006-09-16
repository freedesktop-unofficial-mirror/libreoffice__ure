/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: interfacecontainer.cxx,v $
 *
 *  $Revision: 1.16 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-16 12:41:22 $
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
#include "precompiled_cppuhelper.hxx"

#include <cppuhelper/interfacecontainer.hxx>
#include <cppuhelper/queryinterface.hxx>
#include <cppuhelper/propshlp.hxx>

#include <osl/diagnose.h>
#include <osl/mutex.hxx>

#include <hash_map>

#if OSL_DEBUG_LEVEL == 0
#  ifndef NDEBUG
#    define NDEBUG
#  endif
#endif
#include <assert.h>

#include <com/sun/star/lang/XEventListener.hpp>


using namespace osl;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;

namespace cppu
{

//===================================================================
//===================================================================
//===================================================================
/**
 * Reallocate the sequence.
 */
static void realloc( Sequence< Reference< XInterface > > & rSeq, sal_Int32 nNewLen )
    SAL_THROW( () )
{
    rSeq.realloc( nNewLen );
}

/**
 * Remove an element from an interface sequence.
 */
static void sequenceRemoveElementAt( Sequence< Reference< XInterface > > & rSeq, sal_Int32 index )
    SAL_THROW( () )
{
    sal_Int32 nNewLen = rSeq.getLength() - 1;

    Sequence< Reference< XInterface > > aDestSeq( rSeq.getLength() - 1 );
    // getArray on a const sequence is faster
    const Reference< XInterface > * pSource = ((const Sequence< Reference< XInterface > > &)rSeq).getConstArray();
    Reference< XInterface > * pDest = aDestSeq.getArray();
    sal_Int32 i = 0;
    for( ; i < index; i++ )
        pDest[i] = pSource[i];
    for( sal_Int32 j = i ; j < nNewLen; j++ )
        pDest[j] = pSource[j+1];
    rSeq = aDestSeq;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifdef SAL_WNT
#pragma warning( disable: 4786 )
#endif

//===================================================================
//===================================================================
//===================================================================
OInterfaceIteratorHelper::OInterfaceIteratorHelper( OInterfaceContainerHelper & rCont_ )
    SAL_THROW( () )
    : rCont( rCont_ )
{
    MutexGuard aGuard( rCont.rMutex );
    if( rCont.bInUse )
        // worst case, two iterators at the same time
        rCont.copyAndResetInUse();
    bIsList = rCont_.bIsList;
    pData = rCont_.pData;
    if( bIsList )
    {
        rCont.bInUse = sal_True;
        nRemain = ((Sequence< Reference< XInterface > >*)pData)->getLength();
    }
    else if( pData )
    {
        ((XInterface *)pData)->acquire();
        nRemain = 1;
    }
    else
        nRemain = 0;
}

OInterfaceIteratorHelper::~OInterfaceIteratorHelper() SAL_THROW( () )
{
    sal_Bool bShared;
    {
    MutexGuard aGuard( rCont.rMutex );
    // bResetInUse protect the iterator against recursion
    bShared = pData == rCont.pData && rCont.bIsList;
    if( bShared )
    {
        OSL_ENSURE( rCont.bInUse, "OInterfaceContainerHelper must be in use" );
        rCont.bInUse = sal_False;
    }
    }

    if( !bShared )
    {
        if( bIsList )
            // Sequence owned by the iterator
            delete (Sequence< Reference< XInterface > >*)pData;
        else if( pData )
            // Interface is acquired by the iterator
            ((XInterface*)pData)->release();
    }
}

XInterface * OInterfaceIteratorHelper::next() SAL_THROW( () )
{
    if( nRemain )
    {
        nRemain--;
        if( bIsList )
            // typecase to const,so the getArray method is faster
            return ((const Sequence< Reference< XInterface > >*)pData)->getConstArray()[nRemain].get();
        else if( pData )
            return (XInterface*)pData;
    }
    // exception
    return 0;
}

void OInterfaceIteratorHelper::remove() SAL_THROW( () )
{
    if( bIsList )
    {
        OSL_ASSERT( nRemain >= 0 &&
                    nRemain < ((const Sequence< Reference< XInterface > >*)pData)->getLength() );
        XInterface * p =
            ((const Sequence< Reference< XInterface > >*)pData)->getConstArray()[nRemain].get();
        rCont.removeInterface( * reinterpret_cast< const Reference< XInterface > * >( &p ) );
    }
    else
    {
        OSL_ASSERT( 0 == nRemain );
        rCont.removeInterface( * reinterpret_cast< const Reference< XInterface > * >(&pData));
    }
}

//===================================================================
//===================================================================
//===================================================================


OInterfaceContainerHelper::OInterfaceContainerHelper( Mutex & rMutex_ ) SAL_THROW( () )
    : pData( 0 )
    , rMutex( rMutex_ )
    , bInUse( sal_False )
    , bIsList( sal_False )
{
}

OInterfaceContainerHelper::~OInterfaceContainerHelper() SAL_THROW( () )
{
    OSL_ENSURE( !bInUse, "~OInterfaceContainerHelper but is in use" );
    if( bIsList )
        delete (Sequence< Reference< XInterface > >*)pData;
    else if( pData )
        ((XInterface*)pData)->release();
}

sal_Int32 OInterfaceContainerHelper::getLength() const SAL_THROW( () )
{
    MutexGuard aGuard( rMutex );
    if( bIsList )
        return ((Sequence< Reference< XInterface > >*)pData)->getLength();
    else if( pData )
        return 1;
    return 0;
}

Sequence< Reference<XInterface> > OInterfaceContainerHelper::getElements() const SAL_THROW( () )
{
    MutexGuard aGuard( rMutex );
    if( bIsList )
        return *(Sequence< Reference< XInterface > >*)pData;
    else if( pData )
    {
        Reference<XInterface> x( (XInterface *)pData );
        return Sequence< Reference< XInterface > >( &x, 1 );
    }
    return Sequence< Reference< XInterface > >();
}

void OInterfaceContainerHelper::copyAndResetInUse() SAL_THROW( () )
{
    OSL_ENSURE( bInUse, "OInterfaceContainerHelper not in use" );
    if( bInUse )
    {
        // this should be the worst case. If a iterator is active
        // and a new Listener is added.
        if( bIsList )
            pData = new Sequence< Reference< XInterface > >( *(Sequence< Reference< XInterface > >*)pData );
        else if( pData )
            ((XInterface*)pData)->acquire();

        bInUse = sal_False;
    }
}

sal_Int32 OInterfaceContainerHelper::addInterface( const Reference<XInterface> & rListener ) SAL_THROW( () )
{
    OSL_ASSERT( rListener.is() );
    MutexGuard aGuard( rMutex );
    if( bInUse )
        copyAndResetInUse();

    if( bIsList )
    {
        sal_Int32 nLen = ((Sequence< Reference< XInterface > >*)pData)->getLength();
        realloc( *(Sequence< Reference< XInterface > >*)pData, nLen +1 );
        ((Sequence< Reference< XInterface > >*)pData)->getArray()[ nLen ] = rListener;
        return nLen +1;
    }
    else if( pData )
    {
        Sequence< Reference< XInterface > > * pSeq = new Sequence< Reference< XInterface > >( 2 );
        Reference<XInterface> * pArray = pSeq->getArray();
        pArray[0] = (XInterface *)pData;
        pArray[1] = rListener;
        ((XInterface *)pData)->release();
        pData = pSeq;
        bIsList = sal_True;
        return 2;
    }
    else
    {
        pData = rListener.get();
        if( rListener.is() )
            rListener->acquire();
        return 1;
    }
}

sal_Int32 OInterfaceContainerHelper::removeInterface( const Reference<XInterface> & rListener ) SAL_THROW( () )
{
    OSL_ASSERT( rListener.is() );
    MutexGuard aGuard( rMutex );
    if( bInUse )
        copyAndResetInUse();

    if( bIsList )
    {
        const Reference<XInterface> * pL = ((const Sequence< Reference< XInterface > >*)pData)->getConstArray();
        sal_Int32 nLen = ((Sequence< Reference< XInterface > >*)pData)->getLength();
        sal_Int32 i;
        for( i = 0; i < nLen; i++ )
        {
            // It is not valid to compare the Pointer direkt, but is is is much
            // more faster.
            if( pL[i].get() == rListener.get() )
            {
                sequenceRemoveElementAt( *(Sequence< Reference< XInterface > >*)pData, i );
                break;
            }
        }

        if( i == nLen )
        {
            // interface not found, use the correct compare method
            for( i = 0; i < nLen; i++ )
            {
                if( pL[i] == rListener )
                {
                    sequenceRemoveElementAt(*(Sequence< Reference< XInterface > >*)pData, i );
                    break;
                }
            }
        }

        if( ((Sequence< Reference< XInterface > >*)pData)->getLength() == 1 )
        {
            XInterface * p = ((const Sequence< Reference< XInterface > >*)pData)->getConstArray()[0].get();
            p->acquire();
            delete (Sequence< Reference< XInterface > >*)pData;
            pData = p;
            bIsList = sal_False;
            return 1;
        }
        else
            return ((Sequence< Reference< XInterface > >*)pData)->getLength();
    }
    else if( pData && Reference<XInterface>( (XInterface*)pData ) == rListener )
    {
        ((XInterface *)pData)->release();
        pData = 0;
    }
    return pData ? 1 : 0;
}

void OInterfaceContainerHelper::disposeAndClear( const EventObject & rEvt ) SAL_THROW( () )
{
    ClearableMutexGuard aGuard( rMutex );
    OInterfaceIteratorHelper aIt( *this );
    // Container freigeben, falls im disposing neue Eintr�ge kommen
    OSL_ENSURE( !bIsList || bInUse, "OInterfaceContainerHelper not in use" );
    if( !bIsList && pData )
        ((XInterface *)pData)->release();
    // set the member to null, the iterator delete the values
    pData = NULL;
    bIsList = sal_False;
    bInUse = sal_False;
    aGuard.clear();
    while( aIt.hasMoreElements() )
    {
        try
        {
            Reference<XEventListener > xLst( aIt.next(), UNO_QUERY );
            if( xLst.is() )
                xLst->disposing( rEvt );
        }
        catch ( RuntimeException & )
        {
            // be robust, if e.g. a remote bridge has disposed already.
            // there is no way, to delegate the error to the caller :o(.
        }
    }
}


void OInterfaceContainerHelper::clear() SAL_THROW( () )
{
    ClearableMutexGuard aGuard( rMutex );
    OInterfaceIteratorHelper aIt( *this );
    // Container freigeben, falls im disposing neue Eintr�ge kommen
    OSL_ENSURE( !bIsList || bInUse, "OInterfaceContainerHelper not in use" );
    if( !bIsList && pData )
        ((XInterface *)pData)->release();
    // set the member to null, the iterator delete the values
    pData = 0;
    bIsList = sal_False;
    bInUse = sal_False;
    // release mutex before aIt destructor call
    aGuard.clear();
}

//##################################################################################################
//##################################################################################################
//##################################################################################################

// specialized class for type

typedef ::std::hash_map< Type , void* , hashType_Impl , ::std::equal_to< Type > > t_type2ptr;

OMultiTypeInterfaceContainerHelper::OMultiTypeInterfaceContainerHelper( Mutex & rMutex_ )
    SAL_THROW( () )
    : rMutex( rMutex_ )
{
    m_pMap = new t_type2ptr();
}
OMultiTypeInterfaceContainerHelper::~OMultiTypeInterfaceContainerHelper()
    SAL_THROW( () )
{
    t_type2ptr * pMap = (t_type2ptr *)m_pMap;
    t_type2ptr::iterator iter = pMap->begin();
    t_type2ptr::iterator end = pMap->end();

    while( iter != end )
    {
        delete (OInterfaceContainerHelper*)(*iter).second;
        (*iter).second = 0;
        ++iter;
    }
    delete pMap;
}
Sequence< Type > OMultiTypeInterfaceContainerHelper::getContainedTypes() const
    SAL_THROW( () )
{
    t_type2ptr * pMap = (t_type2ptr *)m_pMap;
    t_type2ptr::size_type nSize;

    ::osl::MutexGuard aGuard( rMutex );
    nSize = pMap->size();
    if( nSize )
    {
        ::com::sun::star::uno::Sequence< Type > aInterfaceTypes( nSize );
        Type * pArray = aInterfaceTypes.getArray();

        t_type2ptr::iterator iter = pMap->begin();
        t_type2ptr::iterator end = pMap->end();

        sal_Int32 i = 0;
        while( iter != end )
        {
            // are interfaces added to this container?
            if( ((OInterfaceContainerHelper*)(*iter).second)->getLength() )
                // yes, put the type in the array
                pArray[i++] = (*iter).first;
            ++iter;
        }
        if( (t_type2ptr::size_type)i != nSize ) {
            // may be empty container, reduce the sequence to the right size
            aInterfaceTypes = ::com::sun::star::uno::Sequence< Type >( pArray, i );
        }
        return aInterfaceTypes;
    }
    return ::com::sun::star::uno::Sequence< Type >();
}
OInterfaceContainerHelper * OMultiTypeInterfaceContainerHelper::getContainer( const Type & rKey ) const
    SAL_THROW( () )
{
    ::osl::MutexGuard aGuard( rMutex );

    t_type2ptr * pMap = (t_type2ptr *)m_pMap;
     t_type2ptr::iterator iter = pMap->find( rKey );
    if( iter != pMap->end() )
            return (OInterfaceContainerHelper*) (*iter).second;
    return 0;
}
sal_Int32 OMultiTypeInterfaceContainerHelper::addInterface(
    const Type & rKey, const Reference< XInterface > & rListener )
    SAL_THROW( () )
{
    ::osl::MutexGuard aGuard( rMutex );
    t_type2ptr * pMap = (t_type2ptr *)m_pMap;
    t_type2ptr::iterator iter = pMap->find( rKey );
    if( iter == pMap->end() )
    {
        OInterfaceContainerHelper * pLC = new OInterfaceContainerHelper( rMutex );
        (*pMap)[rKey] = pLC;
        return pLC->addInterface( rListener );
    }
    else
        return ((OInterfaceContainerHelper*)(*iter).second)->addInterface( rListener );
}
sal_Int32 OMultiTypeInterfaceContainerHelper::removeInterface(
    const Type & rKey, const Reference< XInterface > & rListener )
    SAL_THROW( () )
{
    ::osl::MutexGuard aGuard( rMutex );

    // search container with id nUik
    t_type2ptr * pMap = (t_type2ptr *)m_pMap;
    t_type2ptr::iterator iter = pMap->find( rKey );
        // container found?
    if( iter != pMap->end() )
        return ((OInterfaceContainerHelper*)(*iter).second)->removeInterface( rListener );

    // no container with this id. Always return 0
    return 0;
}
void OMultiTypeInterfaceContainerHelper::disposeAndClear( const EventObject & rEvt )
    SAL_THROW( () )
{
    t_type2ptr::size_type nSize = 0;
    OInterfaceContainerHelper ** ppListenerContainers = NULL;
    {
        ::osl::MutexGuard aGuard( rMutex );
        t_type2ptr * pMap = (t_type2ptr *)m_pMap;
        nSize = pMap->size();
        if( nSize )
        {
            typedef OInterfaceContainerHelper* ppp;
            ppListenerContainers = new ppp[nSize];
            //ppListenerContainers = new (ListenerContainer*)[nSize];

            t_type2ptr::iterator iter = pMap->begin();
            t_type2ptr::iterator end = pMap->end();

            t_type2ptr::size_type i = 0;
            while( iter != end )
            {
                ppListenerContainers[i++] = (OInterfaceContainerHelper*)(*iter).second;
                ++iter;
            }
        }
    }

    // create a copy, because do not fire event in a guarded section
    for( t_type2ptr::size_type i = 0;
            i < nSize; i++ )
    {
        if( ppListenerContainers[i] )
            ppListenerContainers[i]->disposeAndClear( rEvt );
    }

    delete [] ppListenerContainers;
}
void OMultiTypeInterfaceContainerHelper::clear()
    SAL_THROW( () )
{
    ::osl::MutexGuard aGuard( rMutex );
    t_type2ptr * pMap = (t_type2ptr *)m_pMap;
    t_type2ptr::iterator iter = pMap->begin();
    t_type2ptr::iterator end = pMap->end();

    while( iter != end )
    {
        ((OInterfaceContainerHelper*)(*iter).second)->clear();
        ++iter;
    }
}


//##################################################################################################
//##################################################################################################
//##################################################################################################

// specialized class for long

typedef ::std::hash_map< sal_Int32 , void* , hashInt32_Impl , equalInt32_Impl > t_long2ptr;

OMultiTypeInterfaceContainerHelperInt32::OMultiTypeInterfaceContainerHelperInt32( Mutex & rMutex_ )
    SAL_THROW( () )
    : rMutex( rMutex_ )
{
    m_pMap = new t_long2ptr();
}
OMultiTypeInterfaceContainerHelperInt32::~OMultiTypeInterfaceContainerHelperInt32()
    SAL_THROW( () )
{
    t_long2ptr * pMap = (t_long2ptr *)m_pMap;
    t_long2ptr::iterator iter = pMap->begin();
    t_long2ptr::iterator end = pMap->end();

    while( iter != end )
    {
        delete (OInterfaceContainerHelper*)(*iter).second;
        (*iter).second = 0;
        ++iter;
    }
    delete pMap;
}
Sequence< sal_Int32 > OMultiTypeInterfaceContainerHelperInt32::getContainedTypes() const
    SAL_THROW( () )
{
    t_long2ptr * pMap = (t_long2ptr *)m_pMap;
    t_long2ptr::size_type nSize;

    ::osl::MutexGuard aGuard( rMutex );
    nSize = pMap->size();
    if( nSize )
    {
        ::com::sun::star::uno::Sequence< sal_Int32 > aInterfaceTypes( nSize );
        sal_Int32 * pArray = aInterfaceTypes.getArray();

        t_long2ptr::iterator iter = pMap->begin();
        t_long2ptr::iterator end = pMap->end();
        
        sal_Int32 i = 0;
        while( iter != end )
        {
            // are interfaces added to this container?
            if( ((OInterfaceContainerHelper*)(*iter).second)->getLength() )
                // yes, put the type in the array
                pArray[i++] = (*iter).first;
            ++iter;
        }
        if( (t_long2ptr::size_type)i != nSize ) {
            // may be empty container, reduce the sequence to the right size
            aInterfaceTypes = ::com::sun::star::uno::Sequence< sal_Int32 >( pArray, i );
        }
        return aInterfaceTypes;
    }
    return ::com::sun::star::uno::Sequence< sal_Int32 >();
}
OInterfaceContainerHelper * OMultiTypeInterfaceContainerHelperInt32::getContainer( const sal_Int32 & rKey ) const
    SAL_THROW( () )
{
    ::osl::MutexGuard aGuard( rMutex );

    t_long2ptr * pMap = (t_long2ptr *)m_pMap;
     t_long2ptr::iterator iter = pMap->find( rKey );
    if( iter != pMap->end() )
            return (OInterfaceContainerHelper*) (*iter).second;
    return 0;
}
sal_Int32 OMultiTypeInterfaceContainerHelperInt32::addInterface(
    const sal_Int32 & rKey, const Reference< XInterface > & rListener )
    SAL_THROW( () )
{
    ::osl::MutexGuard aGuard( rMutex );
    t_long2ptr * pMap = (t_long2ptr *)m_pMap;
    t_long2ptr::iterator iter = pMap->find( rKey );
    if( iter == pMap->end() )
    {
        OInterfaceContainerHelper * pLC = new OInterfaceContainerHelper( rMutex );
        (*pMap)[rKey] = pLC;
        return pLC->addInterface( rListener );
    }
    else
        return ((OInterfaceContainerHelper*)(*iter).second)->addInterface( rListener );
}
sal_Int32 OMultiTypeInterfaceContainerHelperInt32::removeInterface(
    const sal_Int32 & rKey, const Reference< XInterface > & rListener )
    SAL_THROW( () )
{
    ::osl::MutexGuard aGuard( rMutex );

    // search container with id nUik
    t_long2ptr * pMap = (t_long2ptr *)m_pMap;
    t_long2ptr::iterator iter = pMap->find( rKey );
        // container found?
    if( iter != pMap->end() )
        return ((OInterfaceContainerHelper*)(*iter).second)->removeInterface( rListener );

    // no container with this id. Always return 0
    return 0;
}
void OMultiTypeInterfaceContainerHelperInt32::disposeAndClear( const EventObject & rEvt )
    SAL_THROW( () )
{
    t_long2ptr::size_type nSize = 0;
    OInterfaceContainerHelper ** ppListenerContainers = NULL;
    {
        ::osl::MutexGuard aGuard( rMutex );
        t_long2ptr * pMap = (t_long2ptr *)m_pMap;
        nSize = pMap->size();
        if( nSize )
        {
            typedef OInterfaceContainerHelper* ppp;
            ppListenerContainers = new ppp[nSize];
            //ppListenerContainers = new (ListenerContainer*)[nSize];

            t_long2ptr::iterator iter = pMap->begin();
            t_long2ptr::iterator end = pMap->end();

            t_long2ptr::size_type i = 0;
            while( iter != end )
            {
                ppListenerContainers[i++] = (OInterfaceContainerHelper*)(*iter).second;
                ++iter;
            }
        }
    }

    // create a copy, because do not fire event in a guarded section
    for( t_long2ptr::size_type i = 0;
            i < nSize; i++ )
    {
        if( ppListenerContainers[i] )
            ppListenerContainers[i]->disposeAndClear( rEvt );
    }

    delete [] ppListenerContainers;
}
void OMultiTypeInterfaceContainerHelperInt32::clear()
    SAL_THROW( () )
{
    ::osl::MutexGuard aGuard( rMutex );
    t_long2ptr * pMap = (t_long2ptr *)m_pMap;
    t_long2ptr::iterator iter = pMap->begin();
    t_long2ptr::iterator end = pMap->end();

    while( iter != end )
    {
        ((OInterfaceContainerHelper*)(*iter).second)->clear();
        ++iter;
    }
}

}

