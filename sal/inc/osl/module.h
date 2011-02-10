/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/
 
/** @HTML */

#ifndef _OSL_MODULE_H_
#define _OSL_MODULE_H_

#	include <rtl/ustring.h>
#	include <rtl/tencinfo.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SAL_DLLPREFIX
#define SAL_MODULENAME(name) SAL_DLLPREFIX name SAL_DLLEXTENSION
#else
#define SAL_MODULENAME(name) name SAL_DLLEXTENSION
#endif

#if defined(SAL_W32) || defined(SAL_OS2)
#define SAL_MODULENAME_WITH_VERSION(name, version) name version SAL_DLLEXTENSION

#elif defined(SAL_UNX)
#if defined(MACOSX)
#define SAL_MODULENAME_WITH_VERSION(name, version) SAL_DLLPREFIX name ".dylib." version
#else
#define SAL_MODULENAME_WITH_VERSION(name, version) SAL_DLLPREFIX name SAL_DLLEXTENSION "." version
#endif

#endif

#define SAL_LOADMODULE_DEFAULT    0x00000
#define SAL_LOADMODULE_LAZY       0x00001
#define SAL_LOADMODULE_NOW        0x00002
#define SAL_LOADMODULE_GLOBAL     0x00100

typedef void* oslModule;

/** Generic Function pointer type that will be used as symbol address.
    @see osl_getFunctionSymbol.
    @see osl_getModuleURLFromFunctionAddress.
*/
typedef void ( SAL_CALL *oslGenericFunction )( void ); 

/** Load a shared library or module.
    @param strModuleName denotes the name of the module to be loaded.
    @return NULL if the module could not be loaded, otherwise a handle to the module.
*/
oslModule SAL_CALL osl_loadModule(rtl_uString *strModuleName, sal_Int32 nRtldMode);

/** Load a module located relative to some other module.

    @param baseModule
    must point to a function that is part of the code of some loaded module;
    must not be NULL.

    @param relativePath
    a relative URL; must not be NULL.

    @param mode
    the SAL_LOADMODULE_xxx flags.

    @return
    a non-NULL handle to the loaded module, or NULL if an error occurred.

    @since UDK 3.2.8
*/
oslModule SAL_CALL osl_loadModuleRelative(
    oslGenericFunction baseModule, rtl_uString * relativePath, sal_Int32 mode);

/** Retrieve the handle of an already loaded module.

    This function can be used to search for a function symbol in the process address space.
    Do not use the returned handle as an argument to osl_unloadModule. On Unix platforms,
    pModuleName gets ignored and the special handle RTLD_DEFAULT is returned.
    
    @param pModuleName
    [in] denotes the name of the module to search for. Ignored on Unix
    
    @param pResult
    [out] a pointer to a oslModule that is updated with the requested module handle
    on success.
    
    @return 
    sal_True if the module handle could be retrieved and has been copied to *pResult.
    sal_False if the module has not been loaded yet.
    
    @see osl_getFunctionSymbol
    @see osl_getAsciiFunctionSymbol
*/
sal_Bool SAL_CALL osl_getModuleHandle(rtl_uString *pModuleName, oslModule *pResult);

/** Release the module 
*/
void SAL_CALL osl_unloadModule(oslModule Module);

/** lookup the specified symbol name. 
    @return address of the symbol or NULL if lookup failed.
*/
void* SAL_CALL osl_getSymbol( oslModule Module, rtl_uString *strSymbolName);

/** Lookup the specified function symbol name.

    osl_getFunctionSymbol is an alternative function for osl_getSymbol. 
    Use Function pointer as symbol address to conceal type conversion.

    @param Module
    [in] the handle of the Module.

    @param ustrFunctionSymbolName
    [in] Name of the function that will be looked up.
   
    @return
    <dl>
    <dt>Function address.</dt>
    <dd>on success</dd>
    <dt>NULL</dt>
    <dd>lookup failed or the parameter are invalid.</dd>
    </dl>
   
    @see osl_getSymbol
    @see osl_getAsciiFunctionSymbol
*/
oslGenericFunction SAL_CALL osl_getFunctionSymbol( oslModule Module, rtl_uString *ustrFunctionSymbolName );

/** Lookup the specified function symbol name.

    osl_getAsciiFunctionSymbol is an alternative function for osl_getFunctionSymbol. 
    It expects the C-style function name string to contain ascii characters only.

    @param Module
    [in] a module handle as returned by osl_loadModule or osl_getModuleHandle

    @param pFunctionSymbolName
    [in] Name of the function that will be looked up. 
   
    @return
    <dl>
    <dt>Function address.</dt>
    <dd>on success</dd>
    <dt>NULL</dt>
    <dd>lookup failed or the parameter are invalid.</dd>
    </dl>
   
    @see osl_getModuleHandle
    @see osl_getFunctionSymbol
*/
oslGenericFunction SAL_CALL osl_getAsciiFunctionSymbol(oslModule Module, const sal_Char *pSymbol);


/** Lookup URL of module which is mapped at the specified address.
    @param pv specifies an address in the process memory space.
    @param pustrURL receives the URL of the module that is mapped at pv.
    @return sal_True on success, sal_False if no module can be found at the specified address.
*/
sal_Bool SAL_CALL osl_getModuleURLFromAddress( void *pv, rtl_uString **pustrURL ); 

/** Lookup URL of module which is mapped at the specified function address.

    osl_getModuleURLFromFunctionAddress is an alternative function for osl_getModuleURLFromAddress. 
    Use Function pointer as symbol address to conceal type conversion.
   
    @param pf
    [in] function address in oslGenericFunction format.
           
    @param pustrFunctionURL
    [out] receives the URL of the module that is mapped at pf.
   
    @return
    <dl>
    <dt>sal_True</dt>
    <dd>on success</dd>
    <dt>sal_False</dt>
    <dd>no module can be found at the specified function address or parameter is somewhat invalid.</dd>
    </dl>
   
    @see osl_getModuleURLFromAddress
*/
sal_Bool SAL_CALL osl_getModuleURLFromFunctionAddress( oslGenericFunction pf, rtl_uString **pustrFunctionURL );

#ifdef __cplusplus
}
#endif

#endif  /* _OSL_MODULE_H_  */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
