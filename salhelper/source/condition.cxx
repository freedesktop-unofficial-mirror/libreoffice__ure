/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: condition.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 17:00:47 $
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


#include <salhelper/condition.hxx>

#ifndef _OSL_TIME_H_
#include <osl/time.h>
#endif


using namespace salhelper;


/******************************************************************
 *                                                                *
 *                     Condition                                  *
 *                                                                *
 ******************************************************************/

Condition::Condition(osl::Mutex& aMutex)
    : m_aMutex(aMutex),
      m_aCondition(osl_createCondition())
{
}


Condition::~Condition()
{
    osl_destroyCondition(m_aCondition);
}


/******************************************************************
 *                                                                *
 *              ConditionModifier                                 *
 *                                                                *
 ******************************************************************/

ConditionModifier::ConditionModifier(Condition& aCond)
    : m_aCond(aCond)
{
    m_aCond.m_aMutex.acquire();
}


ConditionModifier::~ConditionModifier()
{
    if(m_aCond.applies())
        osl_setCondition(m_aCond.m_aCondition);
    
    m_aCond.m_aMutex.release();
}



/******************************************************************
 *                                                                *
 *              ConditionWaiter                                   *
 *                                                                *
 ******************************************************************/

ConditionWaiter::timedout::timedout() {}

ConditionWaiter::timedout::timedout(timedout const &) {}

ConditionWaiter::timedout::~timedout() {}

ConditionWaiter::timedout &
ConditionWaiter::timedout::operator =(timedout const &) { return *this; }

ConditionWaiter::ConditionWaiter(Condition& aCond)
    : m_aCond(aCond)
{
    while(true) {
        osl_waitCondition(m_aCond.m_aCondition,0);
        m_aCond.m_aMutex.acquire();
        
        if(m_aCond.applies())
            break;
        else {
            osl_resetCondition(m_aCond.m_aCondition);
            m_aCond.m_aMutex.release();
        }
    }
}


ConditionWaiter::ConditionWaiter(Condition& aCond,sal_uInt32 milliSec)
    throw(
        ConditionWaiter::timedout
    )
    : m_aCond(aCond)
{
    TimeValue aTime;
    aTime.Seconds = milliSec / 1000;
    aTime.Nanosec = 1000000 * ( milliSec % 1000 );
    
    while(true) {
        if( osl_waitCondition(m_aCond.m_aCondition,&aTime) ==
            osl_cond_result_timeout )
            throw timedout();
        
        m_aCond.m_aMutex.acquire();
        
        if(m_aCond.applies())
            break;
        else {
            osl_resetCondition(m_aCond.m_aCondition);
            m_aCond.m_aMutex.release();
        }
    }
}


ConditionWaiter::~ConditionWaiter()
{
    if(! m_aCond.applies())
        osl_resetCondition(m_aCond.m_aCondition);
    m_aCond.m_aMutex.release();
}
