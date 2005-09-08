/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: time.c,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 15:13:22 $
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

#include "system.h"

#include <osl/diagnose.h> 
#include <osl/time.h>
#include <sys/timeb.h>

extern sal_Bool TimeValueToFileTime(const TimeValue *cpTimeVal, FILETIME *pFTime);

extern BOOL FileTimeToTimeValue( const FILETIME *cpFTime, TimeValue *pTimeVal );

//--------------------------------------------------
// osl_getSystemTime
//--------------------------------------------------

sal_Bool SAL_CALL osl_getSystemTime(TimeValue* pTimeVal)
{
    SYSTEMTIME SystemTime; 
    FILETIME   CurTime, OffTime;
    __int64	   Value;

    OSL_ASSERT(pTimeVal != 0);

    GetSystemTime(&SystemTime);
    SystemTimeToFileTime(&SystemTime, &CurTime);

    SystemTime.wYear         = 1970;
    SystemTime.wMonth        = 1;
    SystemTime.wDayOfWeek    = 0;
    SystemTime.wDay          = 1;
    SystemTime.wHour         = 0;
    SystemTime.wMinute       = 0;
    SystemTime.wSecond       = 0;
    SystemTime.wMilliseconds = 0;

    SystemTimeToFileTime(&SystemTime, &OffTime);

    Value = *((__int64 *)&CurTime) - *((__int64 *)&OffTime);

    pTimeVal->Seconds  = (unsigned long) (Value / 10000000L);
    pTimeVal->Nanosec  = (unsigned long)((Value % 10000000L) * 100);

    return (sal_True);
}

//--------------------------------------------------
// osl_getDateTimeFromTimeValue
//--------------------------------------------------

sal_Bool SAL_CALL osl_getDateTimeFromTimeValue( TimeValue* pTimeVal, oslDateTime* pDateTime )
{
    FILETIME	aFileTime;
    SYSTEMTIME	aSystemTime;

    if ( TimeValueToFileTime(pTimeVal, &aFileTime) )
    {
        if ( FileTimeToSystemTime( &aFileTime, &aSystemTime ) )
        {
            pDateTime->NanoSeconds	=	pTimeVal->Nanosec;

            pDateTime->Seconds		=	aSystemTime.wSecond;
            pDateTime->Minutes		=	aSystemTime.wMinute;
            pDateTime->Hours		=	aSystemTime.wHour;
            pDateTime->Day			=	aSystemTime.wDay;
            pDateTime->DayOfWeek	=	aSystemTime.wDayOfWeek;
            pDateTime->Month		=	aSystemTime.wMonth;
            pDateTime->Year			=	aSystemTime.wYear;

            return sal_True;
        }
    } 

    return sal_False;
}

//--------------------------------------------------
// osl_getTimeValueFromDateTime
//--------------------------------------------------

sal_Bool SAL_CALL osl_getTimeValueFromDateTime( oslDateTime* pDateTime, TimeValue* pTimeVal )
{
    FILETIME	aFileTime;
    SYSTEMTIME	aSystemTime;

    aSystemTime.wMilliseconds	=	0;
    aSystemTime.wSecond			=	pDateTime->Seconds;
    aSystemTime.wMinute			=	pDateTime->Minutes; 
    aSystemTime.wHour			=	pDateTime->Hours;
    aSystemTime.wDay			=	pDateTime->Day;
    aSystemTime.wDayOfWeek		=	pDateTime->DayOfWeek;
    aSystemTime.wMonth			=	pDateTime->Month;
    aSystemTime.wYear			=	pDateTime->Year;

    if ( SystemTimeToFileTime( &aSystemTime, &aFileTime ) )
    {
        if (FileTimeToTimeValue( &aFileTime, pTimeVal  ) )
        {
            pTimeVal->Nanosec = pDateTime->NanoSeconds;
            return sal_True;
        }
    }

    return sal_False;
}


//--------------------------------------------------
// osl_getLocalTimeFromSystemTime
//--------------------------------------------------

sal_Bool SAL_CALL osl_getLocalTimeFromSystemTime( TimeValue* pSystemTimeVal, TimeValue* pLocalTimeVal )
{ 
    TIME_ZONE_INFORMATION aTimeZoneInformation;
    DWORD Success;
    sal_Int64	bias;

    // get timezone information
    if ( ( Success=GetTimeZoneInformation( &aTimeZoneInformation ) ) != TIME_ZONE_ID_INVALID)
    {
        bias=aTimeZoneInformation.Bias;

        // add bias for daylight saving time
        if ( Success== TIME_ZONE_ID_DAYLIGHT )
            bias+=aTimeZoneInformation.DaylightBias;

        if ( (sal_Int64) pSystemTimeVal->Seconds > ( bias * 60 ) )
        {
            pLocalTimeVal->Seconds = (sal_uInt32) (pSystemTimeVal->Seconds - ( bias * 60) );
            pLocalTimeVal->Nanosec = pSystemTimeVal->Nanosec;

            return sal_True;
        }
    }

    return sal_False; 
}

//--------------------------------------------------
// osl_getSystemTimeFromLocalTime
//--------------------------------------------------

sal_Bool SAL_CALL osl_getSystemTimeFromLocalTime( TimeValue* pLocalTimeVal, TimeValue* pSystemTimeVal )
{
    TIME_ZONE_INFORMATION aTimeZoneInformation;
    DWORD Success;
    sal_Int64	bias;

    // get timezone information
    if ( ( Success=GetTimeZoneInformation( &aTimeZoneInformation ) ) != TIME_ZONE_ID_INVALID)
    {
        bias=aTimeZoneInformation.Bias;

        // add bias for daylight saving time
        if ( Success== TIME_ZONE_ID_DAYLIGHT )
            bias+=aTimeZoneInformation.DaylightBias; 

        if ( (sal_Int64) pLocalTimeVal->Seconds + ( bias * 60 ) > 0 )
        {
            pSystemTimeVal->Seconds = (sal_uInt32) ( pLocalTimeVal->Seconds + ( bias * 60) );
            pSystemTimeVal->Nanosec = pLocalTimeVal->Nanosec;

            return sal_True;
        }
    }

    return sal_False; 
}


static struct _timeb startTime;
static sal_Bool bGlobalTimer = sal_False;

sal_uInt32 SAL_CALL osl_getGlobalTimer(void)
{
  struct _timeb currentTime;
  sal_uInt32 nSeconds;

  if ( bGlobalTimer == sal_False )
  {
      _ftime( &startTime );
      bGlobalTimer=sal_True;
  }
  
  _ftime( &currentTime );
  
  nSeconds = (sal_uInt32)( currentTime.time - startTime.time );

  return ( nSeconds * 1000 ) + (long)( currentTime.millitm - startTime.millitm );
}

