/*************************************************************************
 *
 *  $RCSfile: tres.hxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: bmahbod $ $Date: 2001-06-21 02:20:59 $
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
#ifndef _RTL_TRES_HXX_
#define _RTL_TRES_HXX_

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum
{
    eTerse         = 0,
    eVerboseOnPass = 1,
    eVerboseOnFail = 2,
    eVerbose       = 3
};

static const sal_Char *kTestCasePassedStr = "Passed";
static const sal_Char *kTestCaseFailedStr = "Failed";

#define TST_BOOM(c, m)  OSL_ENSURE(c, m)

// <namespace_rtl>
namespace rtl {

// <class_TestResult>
class TestResult {

    // <private_members>
    sal_Char* m_name;
    sal_Char* m_result;
    sal_Char* m_msg;
    sal_Bool m_state;
    sal_Bool m_boom;
    sal_Int32 nVerbosityLevel;
    // </private_members>

    // <private_ctors>
    TestResult();
    TestResult( const TestResult& oRes );
    // </private_ctors>

    // <private_methods>
    // <method_cpy>
    sal_Char* cpy( sal_Char** dest, const sal_Char* src ) {

        if ( *dest ) {
            delete [] *dest;
        }
        *dest = new sal_Char[ ln(src)+1 ];
        // set pointer
        sal_Char* pdest = *dest;
        const sal_Char* psrc = src;

        // copy string by char
        while( *pdest++ = *psrc++ );

        return ( *dest );

    } // </method_cpy>

    // <method_cat>
    sal_Char* cat( const sal_Char* str1, const sal_Char* str2 ) {

        // allocate memory for destination string
        sal_Char* dest = new sal_Char[ ln(str1)+ln(str2)+1 ];

        // set pointers
        sal_Char* pdest = dest;
        const sal_Char* psrc = str1;

        // copy string1 by char to dest
        while( *pdest++ = *psrc++ );
        pdest--;
        psrc = str2;
        while( *pdest++ = *psrc++ );

        return ( dest );

    } // </method_cat>

    // <method_ln>
    sal_uInt32 ln( const sal_Char* str ) {

        sal_uInt32 len = 0;
        const sal_Char* ptr = str;

        if( ptr ) {
            while( *ptr++ ) len++;
        }

        return(len);
    } // </method_ln>

    //--------------------------------------------------------------------
    //--------------------------------------------------------------------

    // <method_getLogTCSResultStr>
    const sal_Char *getLogTCSResultStr( const sal_Bool bTestCaseState )
    {
        if ( bTestCaseState )
        {
            return  kTestCasePassedStr;
        } // if
        else
        {
            return kTestCaseFailedStr;
        } // else
    } // </method_getLogTCSResultStr>

    //--------------------------------------------------------------------

    // <method_getLogInfoStartString>
    sal_Char *getLogInfoStartString( const sal_Bool   bTestCaseState,
                                     const sal_Bool   bPrintTestCaseName
                                   )
    {
        sal_Char  pBuffer[256];
        
        if ( bPrintTestCaseName )
        {
            const sal_Char *pTestCaseName = getName();
            
            if ( pTestCaseName != NULL )
            {
                const sal_Char *pTestCaseState 
                                  = getLogTCSResultStr( bTestCaseState );
                
                sprintf ( pBuffer, 
                          ">> [%s]..[%s] ",
                          pTestCaseName,
                          pTestCaseState
                        );
            } // if
            else
            {
                const sal_Char *pTestCaseState 
                                  = getLogTCSResultStr( bTestCaseState );
                
                sprintf ( pBuffer, 
                          ">> [%s] ",
                          pTestCaseState
                        );
            } // else
        } // if
        else
        {
            const sal_Char *pTestCaseState 
                              = getLogTCSResultStr( bTestCaseState );
            
            sprintf ( pBuffer, 
                      ">> [%s] ",
                      pTestCaseState
                    );
        } // else
        
        return  pBuffer;
    } // </method_getLogInfoStartString>

    //--------------------------------------------------------------------

    // <method_getPrintToLog>
    sal_Bool getPrintToLog ( const sal_Bool  bTestCaseState )
    {
        sal_Bool  bPrintToLog = false;

        switch ( nVerbosityLevel )
        {
            case eTerse:
                
                break;
                    
            case eVerboseOnPass:
                
                if ( bTestCaseState )
                {
                    bPrintToLog = true;
                } // if
                
                break;
            
            case eVerboseOnFail:
                
                if ( !bTestCaseState )
                {
                    bPrintToLog = true;
                } // if
                
                break;
                
            case eVerbose:
            
                bPrintToLog = true;
                
                break;
        } // switch
        
        return  bPrintToLog;
    } // </method_getPrintToLog>

    //--------------------------------------------------------------------
    //--------------------------------------------------------------------

    // </private_methods>

public:

    // <public_ctors>
    TestResult( const sal_Char* meth, sal_Bool boom = sal_False, sal_Int32 nVerbosity = 0 )
            : m_name(0)
            , m_result(0)
            , m_msg(0)
            , m_state( sal_False )
            , m_boom( boom )
            , nVerbosityLevel(nVerbosity) {

        cpy( &m_name, meth );
    } // </public_ctors>


    // <dtor>
    ~TestResult() {
        if( m_name )
            delete( m_name );
        if( m_result )
            delete( m_result );
        if( m_msg )
            delete( m_msg );

    } // </dtor>

    // <public_methods>
    
    // <method_state>
    inline void state( sal_Bool state, sal_Char* msg = 0 ) {
        m_state = state;
        if( msg ) {
            cpy( &m_msg, msg );
        }
        if( ! state && m_boom ) {
            TST_BOOM( m_state, m_msg );
        }
    } // </method_state>

    // <method_getState>
    inline sal_Bool getState() {
        return m_state;
    } // </method_getState>

    // <method_end>
    inline void end( sal_Char* msg = 0 ) {

        sal_Char* okStr = "#OK#";
        sal_Char* fdStr = "#FAILED#";

        if ( ! msg ) {
            msg = "PASSED";
        }

        if( m_state )
            cpy( &m_result, cat( msg, okStr ) );
        else
            cpy( &m_result, cat( msg, fdStr ) );

    } // </method_end>

    // <method_getName>
    sal_Char* getName() {
        return m_name;
    } // </method_getName>

    // <method_getResult>
    sal_Char* getResult() {
        return m_result;
    } // </method_getResult>

    // <method_getMsg>
    sal_Char* getMsg() {
        return m_msg;
    } // </method_getMsg>

    // <method_getVerbosityLevel>
    inline sal_Int32 getVerbosityLevel()
    {
        if ( ( nVerbosityLevel < 0 ) || ( nVerbosityLevel > 3 ) )
         {
              return 3;
         }
         else
         {
             return nVerbosityLevel;
         }

        return nVerbosityLevel;
    } // </method_getVerbosityLevel>

    // <method_setVerbosityLevel>
    inline sal_Int32 setVerbosityLevel( const sal_Int32 nNewVerbosityLevel )
    {
      if ( ( nNewVerbosityLevel < 0 ) || ( nNewVerbosityLevel > 3 ) )
      {
        nVerbosityLevel = 3;
      }
      else
      {
        nVerbosityLevel = nNewVerbosityLevel;
      }

      return nVerbosityLevel;
    }  // </method_setVerbosityLevel>

    //--------------------------------------------------------------------
    //--------------------------------------------------------------------

    // <method_logPrintf>
    inline sal_Bool logPrintf ( const sal_Bool   bTestCaseState,
                                const sal_Bool   bPrintTestCaseName,
                                const char      *pFormatStr, ... 
                              )
    {
        const sal_Bool bPrintToLog = getPrintToLog( bTestCaseState );

        if ( bPrintToLog )
        {
            const sal_Char *pBeginBuffer 
                              = getLogInfoStartString( bTestCaseState,   
                                                       bPrintTestCaseName 
                                                     );
            
            sal_Char  pEndBuffer[256];
            
            strcpy( pEndBuffer, pBeginBuffer );
                            
            if ( pFormatStr != NULL )
            {
                va_list   vArgumentList;
                sal_Char  pInfoStr[256];
                
                // copy all arguments into the buffer
                                
                va_start ( vArgumentList, pFormatStr );
                                
                    if ( vArgumentList != NULL )
                    {
                        // one or more variable arguments do exist
                                            
                        vsprintf ( pInfoStr, pFormatStr, vArgumentList );
                    } // if
                    else
                    {
                        strcpy ( pInfoStr, pFormatStr );
                    } // else
                    
                va_end ( vArgumentList );
                
                strcat( pEndBuffer, pInfoStr );
            } // if
                
            printf ( pEndBuffer );
        } // if

        return bPrintToLog;
    } // </method_logPrintf>

    //--------------------------------------------------------------------
    //--------------------------------------------------------------------

    // </public_methods>

}; // </class_TestResult>

} // </namespace_rtl>
#endif





