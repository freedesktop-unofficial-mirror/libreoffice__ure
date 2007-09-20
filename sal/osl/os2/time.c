/*************************************************************************
 *
 *  $RCSfile: time.c,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: vg $ $Date: 2007-09-20 15:15:49 $
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


#include "system.h"

#include <osl/diagnose.h>
#include <osl/time.h>

/* FIXME: detection should be done in configure script */
#if defined(MACOSX) || defined(FREEBSD) || defined(NETBSD) || defined(LINUX)
#define STRUCT_TM_HAS_GMTOFF 1

#elif defined(SOLARIS)
#define HAS_ALTZONE 1
#endif

/*--------------------------------------------------
 * osl_getSystemTime
 *-------------------------------------------------*/

sal_Bool SAL_CALL osl_getSystemTime(TimeValue* TimeValue)
{
    struct timeval tp;

    /* FIXME: use higher resolution */
    gettimeofday(&tp, NULL);

    TimeValue->Seconds = tp.tv_sec;
    TimeValue->Nanosec = tp.tv_usec * 1000;

    return (sal_True);
}


/*--------------------------------------------------
 * osl_getDateTimeFromTimeValue
 *-------------------------------------------------*/

sal_Bool SAL_CALL osl_getDateTimeFromTimeValue( TimeValue* pTimeVal, oslDateTime* pDateTime )
{
    struct tm *pSystemTime;
    struct tm tmBuf;
    time_t atime;

    atime = (time_t)pTimeVal->Seconds;
    
    /* Convert time from type time_t to struct tm */
    pSystemTime = gmtime_r( &atime, &tmBuf );


    /* Convert struct tm to struct oslDateTime */
    if ( pSystemTime != NULL )
    {
        pDateTime->NanoSeconds	=	pTimeVal->Nanosec;
        pDateTime->Seconds		=	pSystemTime->tm_sec;
        pDateTime->Minutes		=	pSystemTime->tm_min;
        pDateTime->Hours		=	pSystemTime->tm_hour;
        pDateTime->Day			=	pSystemTime->tm_mday;
        pDateTime->DayOfWeek	=	pSystemTime->tm_wday;
        pDateTime->Month		=	pSystemTime->tm_mon + 1;
        pDateTime->Year			=	pSystemTime->tm_year  + 1900;

        return sal_True;
    }

    return sal_False;
}

/*--------------------------------------------------
 * osl_getTimeValueFromDateTime
 *--------------------------------------------------*/

sal_Bool SAL_CALL osl_getTimeValueFromDateTime( oslDateTime* pDateTime, TimeValue* pTimeVal )
{
    struct tm	aTime;
    time_t		nSeconds;

    /* Convert struct oslDateTime to struct tm */
    aTime.tm_sec  = pDateTime->Seconds;
    aTime.tm_min  = pDateTime->Minutes;
    aTime.tm_hour = pDateTime->Hours;
    aTime.tm_mday = pDateTime->Day;
    aTime.tm_wday = pDateTime->DayOfWeek;

    if ( pDateTime->Month > 0 )
        aTime.tm_mon = pDateTime->Month - 1;
    else 
        return sal_False;

    if ( pDateTime->Year >= 1900 )
        aTime.tm_year = pDateTime->Year - 1900;
    else 
        return sal_False;

    aTime.tm_isdst = -1;
    aTime.tm_wday  = 0;
    aTime.tm_yday  = 0;

    /* Convert time to calendar value */
    nSeconds = mktime( &aTime );

    /*
     * mktime expects the struct tm to be in local timezone, so we have to adjust 
     * the returned value to be timezone neutral.
     */
     
    if ( nSeconds != (time_t) -1 )
    {
        time_t bias;
        
        /* timezone corrections */
        tzset();

#if defined(STRUCT_TM_HAS_GMTOFF)
        /* members of struct tm are corrected by mktime */
        bias = 0 - aTime.tm_gmtoff;

#elif defined(HAS_ALTZONE)
        /* check if daylight saving time is in effect */
        bias = aTime.tm_isdst > 0 ? altzone : timezone;
#else
        /* exspect daylight saving time to be one hour */
        bias = aTime.tm_isdst > 0 ? timezone - 3600 : timezone;
#endif
        
        pTimeVal->Seconds = nSeconds;
        pTimeVal->Nanosec = pDateTime->NanoSeconds;

        if ( nSeconds > bias )
            pTimeVal->Seconds -= bias;
           
        return sal_True;
    }

    return sal_False;
}


/*--------------------------------------------------
 * osl_getLocalTimeFromSystemTime
 *--------------------------------------------------*/

sal_Bool SAL_CALL osl_getLocalTimeFromSystemTime( TimeValue* pSystemTimeVal, TimeValue* pLocalTimeVal )
{ 
    struct tm *pLocalTime;
    struct tm tmBuf;
    time_t bias;
    time_t atime;

    atime = (time_t) pSystemTimeVal->Seconds;
    pLocalTime = localtime_r( &atime, &tmBuf );

#if defined(STRUCT_TM_HAS_GMTOFF)
    /* members of struct tm are corrected by mktime */
    bias = 0 - pLocalTime->tm_gmtoff;
       
#elif defined(HAS_ALTZONE)
    /* check if daylight saving time is in effect */
    bias = pLocalTime->tm_isdst > 0 ? altzone : timezone;
#else
    /* exspect daylight saving time to be one hour */
    bias = pLocalTime->tm_isdst > 0 ? timezone - 3600 : timezone;
#endif

    if ( (sal_Int64) pSystemTimeVal->Seconds > bias )
    {
        pLocalTimeVal->Seconds = pSystemTimeVal->Seconds - bias;
        pLocalTimeVal->Nanosec = pSystemTimeVal->Nanosec;

        return sal_True;
    }

    return sal_False;
}

/*--------------------------------------------------
 * osl_getSystemTimeFromLocalTime
 *--------------------------------------------------*/

sal_Bool SAL_CALL osl_getSystemTimeFromLocalTime( TimeValue* pLocalTimeVal, TimeValue* pSystemTimeVal )
{
    struct tm *pLocalTime;
    struct tm tmBuf;
    time_t bias;
    time_t atime;

    atime = (time_t) pLocalTimeVal->Seconds;

    /* Convert atime, which is a local time, to it's GMT equivalent. Then, get
     * the timezone offset for the local time for the GMT equivalent time. Note
     * that we cannot directly use local time to determine the timezone offset
     * because GMT is the only reliable time that we can determine timezone
     * offset from.
     */

    atime = mktime( gmtime_r( &atime, &tmBuf ) );
    pLocalTime = localtime_r( &atime, &tmBuf );

#if defined(STRUCT_TM_HAS_GMTOFF)
    /* members of struct tm are corrected by mktime */
    bias = 0 - pLocalTime->tm_gmtoff;
       
#elif defined(HAS_ALTZONE)
    /* check if daylight saving time is in effect */
    bias = pLocalTime->tm_isdst > 0 ? altzone : timezone;
#else
    /* exspect daylight saving time to be one hour */
    bias = pLocalTime->tm_isdst > 0 ? timezone - 3600 : timezone;
#endif

    if ( (sal_Int64) pLocalTimeVal->Seconds + bias > 0 )
    {
        pSystemTimeVal->Seconds = pLocalTimeVal->Seconds + bias;
        pSystemTimeVal->Nanosec = pLocalTimeVal->Nanosec;

        return sal_True;
    }

    return sal_False; 
}



static struct timeval startTime;
static sal_Bool bGlobalTimer = sal_False;

sal_uInt32 SAL_CALL osl_getGlobalTimer()
{
  struct timeval currentTime;
  sal_uInt32 nSeconds;

  // FIXME: not thread safe !!
  if ( bGlobalTimer == sal_False )
  {
      gettimeofday( &startTime, NULL );
      bGlobalTimer=sal_True;
  }
  
  gettimeofday( &currentTime, NULL );
  
  nSeconds = (sal_uInt32)( currentTime.tv_sec - startTime.tv_sec );

  return ( nSeconds * 1000 ) + (long) (( currentTime.tv_usec - startTime.tv_usec) / 1000 );
}
