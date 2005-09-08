/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: module.hxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 14:28:53 $
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
 
/** @HTML */

#ifndef _OSL_MODULE_HXX_
#define _OSL_MODULE_HXX_

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif

#ifndef _OSL_MODULE_H_
#include <osl/module.h>
#endif 

namespace osl
{

class Module
{
    Module( const Module&);
    Module& operator = ( const Module&);

public:
    static sal_Bool getUrlFromAddress(void * addr, ::rtl::OUString & libraryUrl) {
        return osl_getModuleURLFromAddress(addr, &libraryUrl.pData);
    }

    /** Get module URL from the specified function address in the module.
    
        Similar to getUrlFromAddress, but use a function address to get URL of the Module. 
        Use Function pointer as symbol address to conceal type conversion.
       
        @param addr
        [in] function address in oslGenericFunction format.
               
        @param libraryUrl
        [in|out] receives the URL of the module.
           
        @return
        <dl>
        <dt>sal_True</dt>
        <dd>on success</dd>
        <dt>sal_False</dt>
        <dd>can not get the URL from the specified function address or the parameter is invalid.</dd>
        </dl>
           
        @see getUrlFromAddress
    */
    static sal_Bool getUrlFromAddress( oslGenericFunction addr, ::rtl::OUString & libraryUrl){
        return osl_getModuleURLFromFunctionAddress( addr, &libraryUrl.pData );
    }

    Module(): m_Module(0){}

    Module( const ::rtl::OUString& strModuleName, sal_Int32 nRtldMode = SAL_LOADMODULE_DEFAULT) : m_Module(0)
    {
        load( strModuleName, nRtldMode);
    }

    ~Module()
    {
        osl_unloadModule(m_Module);
    }

    sal_Bool SAL_CALL load( const ::rtl::OUString& strModuleName, 
        sal_Int32 nRtldMode = SAL_LOADMODULE_DEFAULT)
    {
        unload();
        m_Module= osl_loadModule( strModuleName.pData, nRtldMode );
        return is();
    }

    void SAL_CALL unload()
    {
        if (m_Module)
        {
            osl_unloadModule(m_Module);
            m_Module = 0;
        }
    }

    sal_Bool SAL_CALL is() const
    {
           return m_Module != NULL; 
    }

    void* SAL_CALL getSymbol( const ::rtl::OUString& strSymbolName)
    {
    return ( osl_getSymbol( m_Module, strSymbolName.pData ) );
    }

    /** Get function address by the function name in the module.

        getFunctionSymbol is an alternative function for getSymbol. 
        Use Function pointer as symbol address to conceal type conversion.

        @param ustrFunctionSymbolName
        [in] Function name to be looked up.
   
        @return
        <dl>
        <dt>oslGenericFunction format function address</dt>
        <dd>on success</dd>
        <dt>NULL</dt>
        <dd>lookup failed or parameter is somewhat invalid</dd>
        </dl>
   
        @see getSymbol
    */
    oslGenericFunction SAL_CALL getFunctionSymbol( const ::rtl::OUString& ustrFunctionSymbolName )
    {
        return ( osl_getFunctionSymbol( m_Module, ustrFunctionSymbolName.pData ) );
    }

    operator oslModule() const
    {
        return m_Module;
    }

private:
    oslModule m_Module;

};

}

#endif
