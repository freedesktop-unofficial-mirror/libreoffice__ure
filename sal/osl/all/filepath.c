/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: filepath.c,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 14:51:31 $
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

#include <osl/file.h>
#include <rtl/ustring.h>

static sal_uInt32 SAL_CALL osl_defCalcTextWidth( rtl_uString *ustrText )
{
    return ustrText ? ustrText->length : 0;
}


oslFileError SAL_CALL osl_abbreviateSystemPath( rtl_uString *ustrSystemPath, rtl_uString **pustrCompacted, sal_uInt32 uMaxWidth, oslCalcTextWidthFunc pfnCalcWidth )
{
    oslFileError	error = osl_File_E_None;
    rtl_uString		*ustrPath = NULL;
    rtl_uString		*ustrFile = NULL;
    sal_uInt32		uPathWidth, uFileWidth;

    if ( !pfnCalcWidth )
        pfnCalcWidth = osl_defCalcTextWidth;

    {
        sal_Int32	iLastSlash = rtl_ustr_lastIndexOfChar_WithLength( ustrSystemPath->buffer, ustrSystemPath->length, SAL_PATHDELIMITER );

        if ( iLastSlash >= 0 )
        {
            rtl_uString_newFromStr_WithLength( &ustrPath, ustrSystemPath->buffer, iLastSlash );
            rtl_uString_newFromStr_WithLength( &ustrFile, &ustrSystemPath->buffer[iLastSlash], ustrSystemPath->length - iLastSlash );
        }
        else
        {
            rtl_uString_new( &ustrPath );
            rtl_uString_newFromString( &ustrFile, ustrSystemPath );
        }
    }

    uPathWidth = pfnCalcWidth( ustrPath );
    uFileWidth = pfnCalcWidth( ustrFile );

    /* First abbreviate the directory component of the path */

    while ( uPathWidth + uFileWidth > uMaxWidth )
    {
        if ( ustrPath->length > 3 )
        {
            ustrPath->length--;
            ustrPath->buffer[ustrPath->length-3] = '.';
            ustrPath->buffer[ustrPath->length-2] = '.';
            ustrPath->buffer[ustrPath->length-1] = '.';
            ustrPath->buffer[ustrPath->length] = 0;

            uPathWidth = pfnCalcWidth( ustrPath );
        }
        else
            break;
    }

    /* Now abbreviate file component */

    while ( uPathWidth + uFileWidth > uMaxWidth )
    {
        if ( ustrFile->length > 4 )
        {
            ustrFile->length--;
            ustrFile->buffer[ustrFile->length-3] = '.';
            ustrFile->buffer[ustrFile->length-2] = '.';
            ustrFile->buffer[ustrFile->length-1] = '.';
            ustrFile->buffer[ustrFile->length] = 0;

            uFileWidth = pfnCalcWidth( ustrFile );
        }
        else
            break;
    }

    rtl_uString_newConcat( pustrCompacted, ustrPath, ustrFile );

    /* Event now if path was compacted to ".../..." it can be to large */

    uPathWidth += uFileWidth;

    while ( uPathWidth > uMaxWidth )
    {
        (*pustrCompacted)->length--;
        (*pustrCompacted)->buffer[(*pustrCompacted)->length] = 0;
        uPathWidth = pfnCalcWidth( *pustrCompacted );
    }

    if ( ustrPath )
        rtl_uString_release( ustrPath );

    if ( ustrFile )
        rtl_uString_release( ustrFile );

    return error;
}


