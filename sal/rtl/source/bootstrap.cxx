/*************************************************************************
 *
 *  $RCSfile: bootstrap.cxx,v $
 *
 *  $Revision: 1.32 $
 *
 *  last change: $Author: vg $ $Date: 2004-12-23 11:35:36 $
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

#include "rtl/bootstrap.h"

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif
#ifndef _OSL_MODULE_H_
#include <osl/module.h>
#endif
#ifndef _OSL_PROCESS_H_
#include <osl/process.h>
#endif
#ifndef _OSL_FILE_HXX_
#include <osl/file.hxx>
#endif
#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif

#ifndef _RTL_ALLOC_H_
#include <rtl/alloc.h>
#endif
#ifndef _RTL_STRING_HXX_
#include <rtl/string.hxx>
#endif
#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif
#ifndef _RTL_BYTESEQ_HXX_
#include <rtl/byteseq.hxx>
#endif
#ifndef INCLUDED_RTL_INSTANCE_HXX
#include <rtl/instance.hxx>
#endif

#ifndef INCLUDED_SAL_INTERNAL_ALLOCATOR_HXX
#include "internal/allocator.hxx"
#endif

#include "macro.hxx"

#include <hash_map>
#include <list>

//----------------------------------------------------------------------------

using osl::DirectoryItem;
using osl::FileStatus;

using rtl::OString;
using rtl::OUString;
using rtl::OUStringToOString;

//----------------------------------------------------------------------------

struct rtl_bootstrap_NameValue
{
    OUString sName;
    OUString sValue;

    inline rtl_bootstrap_NameValue() SAL_THROW( () )
        {}
    inline rtl_bootstrap_NameValue(
        OUString const & name, OUString const & value ) SAL_THROW( () )
        : sName( name ),
          sValue( value )
        {}
};

typedef std::list<
    rtl_bootstrap_NameValue,
    sal::Allocator< rtl_bootstrap_NameValue >
> NameValueList;

namespace {
    struct rtl_bootstrap_set_list :
        public rtl::Static< NameValueList, rtl_bootstrap_set_list > {};
}

//----------------------------------------------------------------------------

static sal_Bool getFromCommandLineArgs(
    rtl_uString **ppValue , rtl_uString *pName )
{
    static NameValueList *pNameValueList = 0;
    if( ! pNameValueList )
    {
        static NameValueList nameValueList;

        sal_Int32 nArgCount = osl_getCommandArgCount();
        for(sal_Int32 i = 0; i < nArgCount; ++ i)
        {
            rtl_uString *pArg = 0;
            osl_getCommandArg( i, &pArg );
            if( ('-' == pArg->buffer[0] || '/' == pArg->buffer[0] ) &&
                'e' == pArg->buffer[1] &&
                'n' == pArg->buffer[2] &&
                'v' == pArg->buffer[3] &&
                ':' == pArg->buffer[4] )
            {
                sal_Int32 nIndex = rtl_ustr_indexOfChar( pArg->buffer, '=' );
                if( nIndex >= 0 )
                {

                    rtl_bootstrap_NameValue nameValue;
                    nameValue.sName = OUString( &(pArg->buffer[5]), nIndex - 5  );
                    nameValue.sValue = OUString( &(pArg->buffer[nIndex+1]) );
                    if( i == nArgCount-1 &&
                        nameValue.sValue.getLength() &&
                        nameValue.sValue[nameValue.sValue.getLength()-1] == 13 )
                    {
                        // avoid the 13 linefeed for the last argument,
                        // when the executable is started from a script,
                        // that was edited on windows
                        nameValue.sValue = nameValue.sValue.copy(0,nameValue.sValue.getLength()-1);
                    }
                    nameValueList.push_back( nameValue );
                }
            }
            rtl_uString_release( pArg );
        }
        pNameValueList = &nameValueList;
    }

    sal_Bool found = sal_False;

    OUString name( pName );
    for( NameValueList::iterator ii = pNameValueList->begin() ;
         ii != pNameValueList->end() ;
         ++ii )
    {
        if( (*ii).sName.equals(name) )
        {
            rtl_uString_assign( ppValue, (*ii).sValue.pData );
            found = sal_True;
            break;
        }
    }

    return found;
}

//----------------------------------------------------------------------------

extern "C" oslProcessError SAL_CALL osl_bootstrap_getExecutableFile_Impl (
    rtl_uString ** ppFileURL) SAL_THROW_EXTERN_C();

inline void getExecutableFile_Impl (rtl_uString ** ppFileURL)
{
    osl_bootstrap_getExecutableFile_Impl (ppFileURL);
}

//----------------------------------------------------------------------------

static void getExecutableDirectory_Impl (rtl_uString ** ppDirURL)
{
    OUString fileName;
    getExecutableFile_Impl (&(fileName.pData));

    sal_Int32 nDirEnd = fileName.lastIndexOf('/');
    OSL_ENSURE(nDirEnd >= 0, "Cannot locate executable directory");

    rtl_uString_newFromStr_WithLength(ppDirURL,fileName.getStr(),nDirEnd);
}

//----------------------------------------------------------------------------

static OUString & getIniFileName_Impl()
{
    static OUString *pStaticName = 0;
    if( ! pStaticName )
    {
        OUString fileName;

        OUString sVarName(RTL_CONSTASCII_USTRINGPARAM("INIFILENAME"));
        if(!getFromCommandLineArgs(&fileName.pData, sVarName.pData))
        {
            getExecutableFile_Impl (&(fileName.pData));

            // get rid of a potential executable extension
            OUString progExt (RTL_CONSTASCII_USTRINGPARAM(".bin"));
            if(fileName.getLength() > progExt.getLength()
            && fileName.copy(fileName.getLength() - progExt.getLength()).equalsIgnoreAsciiCase(progExt))
                fileName = fileName.copy(0, fileName.getLength() - progExt.getLength());

            progExt = OUString::createFromAscii(".exe");
            if(fileName.getLength() > progExt.getLength()
            && fileName.copy(fileName.getLength() - progExt.getLength()).equalsIgnoreAsciiCase(progExt))
                fileName = fileName.copy(0, fileName.getLength() - progExt.getLength());

            // append config file suffix
            fileName += OUString(RTL_CONSTASCII_USTRINGPARAM(SAL_CONFIGFILE("")));
        }

        static OUString theFileName;
        if(fileName.getLength())
            theFileName = fileName;

        pStaticName = &theFileName;
    }

    return *pStaticName;
}

//----------------------------------------------------------------------------

static void getFromEnvironment( rtl_uString **ppValue, rtl_uString *pName )
{
    if( osl_Process_E_None != osl_getEnvironment( pName , ppValue ) )
    {
        // osl behaves different on win or unx.
        if( *ppValue )
        {
            rtl_uString_release( *ppValue );
            *ppValue = 0;
        }
    }

}

//----------------------------------------------------------------------------

static void getFromList(
    NameValueList const  * pNameValueList,
    rtl_uString         ** ppValue,
    rtl_uString          * pName )
{
    OUString const & name = * reinterpret_cast< OUString const * >( &pName );
    
    NameValueList::const_iterator iEnd( pNameValueList->end() );
    for( NameValueList::const_iterator ii = pNameValueList->begin(); ii != iEnd; ++ii )
    {
        if( (*ii).sName.equals(name) )
        {
            rtl_uString_assign( ppValue, (*ii).sValue.pData );
            break;
        }
    }
}

//----------------------------------------------------------------------------

static inline bool path_exists( OUString const & path )
{
    DirectoryItem dirItem;
    return (DirectoryItem::E_None == DirectoryItem::get( path, dirItem ));
}

//----------------------------------------------------------------------------
// #111772#
// ensure the given file url has no final slash

inline void EnsureNoFinalSlash (rtl_uString** file_url)
{
    sal_Int32 l = rtl_uString_getLength(*file_url);
    if (rtl_ustr_lastIndexOfChar((*file_url)->buffer, '/') == (l-1))
    {
        (*file_url)->buffer[l-1] = 0;
        (*file_url)->length--;
    }
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

struct Bootstrap_Impl
{
    sal_Int32 _nRefCount;
    Bootstrap_Impl * _base_ini;
    
    NameValueList _nameValueList;
    OUString      _iniName;
    
    explicit Bootstrap_Impl (OUString const & rIniName);
    ~Bootstrap_Impl();
    
    static void * operator new (std::size_t n) SAL_THROW(())
        { return rtl_allocateMemory (sal_uInt32(n)); }
    static void operator delete (void * p , std::size_t) SAL_THROW(())
        { rtl_freeMemory (p); }
    
    sal_Bool getValue( rtl_uString * pName, rtl_uString ** ppValue, rtl_uString * pDefault ) const;
};

//----------------------------------------------------------------------------

Bootstrap_Impl::Bootstrap_Impl( OUString const & rIniName )
    : _nRefCount( 0 ),
      _iniName (rIniName),
      _base_ini( 0 )
{
    OUString base_ini( getIniFileName_Impl() );
    // normalize path
    FileStatus status( FileStatusMask_FileURL );
    DirectoryItem dirItem;
    if (DirectoryItem::E_None == DirectoryItem::get( base_ini, dirItem ) &&
        DirectoryItem::E_None == dirItem.getFileStatus( status ))
    {
        base_ini = status.getFileURL();
        if (! rIniName.equals( base_ini ))
        {
            _base_ini = static_cast< Bootstrap_Impl * >(
                rtl_bootstrap_args_open( base_ini.pData ) );
        }
    }
    
#if OSL_DEBUG_LEVEL > 1    
    OString sFile = OUStringToOString(_iniName, RTL_TEXTENCODING_ASCII_US);
    OSL_TRACE(__FILE__" -- Bootstrap_Impl() - %s\n", sFile.getStr());
#endif /* OSL_DEBUG_LEVEL > 1 */

    oslFileHandle handle;
    if (_iniName.getLength() &&
        osl_File_E_None == osl_openFile(_iniName.pData, &handle, osl_File_OpenFlag_Read))
    {
        rtl::ByteSequence seq;

        while (osl_File_E_None == osl_readLine(handle , (sal_Sequence **)&seq))
        {
            OString line( (const sal_Char *) seq.getConstArray(), seq.getLength() );
            sal_Int32 nIndex = line.indexOf('=');
            if (nIndex >= 1)
            {
                struct rtl_bootstrap_NameValue nameValue;
                nameValue.sName = OStringToOUString(
                    line.copy(0,nIndex).trim(), RTL_TEXTENCODING_ASCII_US );
                nameValue.sValue = OStringToOUString(
                    line.copy(nIndex+1).trim(), RTL_TEXTENCODING_UTF8 );

#if OSL_DEBUG_LEVEL > 1
                OString name_tmp = OUStringToOString(nameValue.sName, RTL_TEXTENCODING_ASCII_US);
                OString value_tmp = OUStringToOString(nameValue.sValue, RTL_TEXTENCODING_UTF8);
                OSL_TRACE(
                    __FILE__" -- pushing: name=%s value=%s\n",
                    name_tmp.getStr(), value_tmp.getStr() );
#endif /* OSL_DEBUG_LEVEL > 1 */

                _nameValueList.push_back(nameValue);
            }
        }
        osl_closeFile(handle);
    }
#if OSL_DEBUG_LEVEL > 1
    else
    {
        OString file_tmp = OUStringToOString(_iniName, RTL_TEXTENCODING_ASCII_US);
        OSL_TRACE( __FILE__" -- couldn't open file: %s", file_tmp.getStr() );
    }
#endif /* OSL_DEBUG_LEVEL > 1 */
}

//----------------------------------------------------------------------------

Bootstrap_Impl::~Bootstrap_Impl()
{
    if (_base_ini != 0)
        rtl_bootstrap_args_close( _base_ini );
}

//----------------------------------------------------------------------------

sal_Bool Bootstrap_Impl::getValue(
    rtl_uString * pName, rtl_uString ** ppValue, rtl_uString * pDefault ) const
{
    // lookup this ini
    sal_Bool result = sal_True;
    bool further_macro_expansion = true;
    
    OUString const & name = *reinterpret_cast< OUString const * >( &pName );
    if (name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("_CPPU_ENV") ))
    {
#define MSTR_(x) # x
#define MSTR(x) MSTR_(x)
#define STR_CPPU_ENV() MSTR(CPPU_ENV)
        OUString val( RTL_CONSTASCII_USTRINGPARAM(STR_CPPU_ENV()) );
        rtl_uString_assign( ppValue, val.pData );
        further_macro_expansion = false;
    }
    else if (name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("ORIGIN") ))
    {
        OUString iniPath;
        sal_Int32 last_slash = _iniName.lastIndexOf( '/' );
        if (last_slash >= 0)
            iniPath = _iniName.copy( 0, last_slash );
        rtl_uString_assign( ppValue, iniPath.pData );
        further_macro_expansion = false;
    }
    else if (name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("_OS") ))
    {
        OUString val( RTL_CONSTASCII_USTRINGPARAM(THIS_OS) );
        rtl_uString_assign( ppValue, val.pData );
        further_macro_expansion = false;
    }
    else if (name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("_ARCH") ))
    {
        OUString val( RTL_CONSTASCII_USTRINGPARAM(THIS_ARCH) );
        rtl_uString_assign( ppValue, val.pData );
        further_macro_expansion = false;
    }
    else
    {
        if (0 != *ppValue)
        {
            rtl_uString_release( *ppValue );
            *ppValue = 0;
        }
        getFromList( &rtl_bootstrap_set_list::get(), ppValue, pName );
        if (! *ppValue)
        {
            getFromCommandLineArgs( ppValue, pName );
            if(!*ppValue)
            {
                getFromEnvironment( ppValue, pName );
                if( ! *ppValue )
                {
                    if (name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("SYSUSERCONFIG") ))
                    {
                        oslSecurity security = osl_getCurrentSecurity();
                        osl_getConfigDir(security, ppValue);
                        EnsureNoFinalSlash(ppValue);
                        osl_freeSecurityHandle(security);
                        further_macro_expansion = false;
                    }
                    else if (name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("SYSUSERHOME") ))
                    {
                        oslSecurity security = osl_getCurrentSecurity();
                        osl_getHomeDir(security, ppValue);
                        EnsureNoFinalSlash(ppValue);
                        osl_freeSecurityHandle(security);
                        further_macro_expansion = false;
                    }
                    else if (name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("SYSBINDIR") ))
                    {
                        getExecutableDirectory_Impl(ppValue);
                        EnsureNoFinalSlash(ppValue);
                        further_macro_expansion = false;
                    }
                    else if (_base_ini && (_base_ini->getValue(pName, ppValue, NULL)))
                    {
                        further_macro_expansion = false;
                    }
                    else
                    {
                        // note:  _base_ini->getValue assigns ppValue in any case
                        if (0 != *ppValue)
                        {
                            rtl_uString_release( *ppValue );
                            *ppValue = 0;
                        }

                        getFromList( &_nameValueList, ppValue, pName );
                        if( ! *ppValue )
                        {
                            if(pDefault)
                                rtl_uString_assign( ppValue, pDefault );

                            else
                                result = sal_False;
                        }
                    }
                }
            }
        }
    }
    
    if (0 == *ppValue)
    {
        rtl_uString_new(ppValue);
    }
    else
    {
        if (further_macro_expansion)
        {
            OUString result = expandMacros(
                static_cast< rtlBootstrapHandle >( const_cast< Bootstrap_Impl * >( this ) ),
                * reinterpret_cast< OUString const * >( ppValue ) );
            rtl_uString_assign( ppValue, result.pData );
        }
    }
    
#if OSL_DEBUG_LEVEL > 1
    OString sName = OUStringToOString(OUString(pName), RTL_TEXTENCODING_ASCII_US);
    OString sValue = OUStringToOString(OUString(*ppValue), RTL_TEXTENCODING_ASCII_US);
    OSL_TRACE(
    __FILE__ " -- Bootstrap_Impl::getValue() - name:%s value:%s result:%i\n",
    sName.getStr(), sValue.getStr(), result );
#endif /* OSL_DEBUG_LEVEL > 1 */
    
    return result;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// map<> may be preferred here, but hash_map<> is implemented fully inline,
// thus there is no need to link against the stlport.

typedef std::hash_map<
    rtl::OUString, Bootstrap_Impl *,
    rtl::OUStringHash, std::equal_to< rtl::OUString >,
    sal::Allocator< OUString > > t_bootstrap_map;

namespace {
    struct bootstrap_map :
        public rtl::Static< t_bootstrap_map, bootstrap_map > {};
}

//----------------------------------------------------------------------------

rtlBootstrapHandle SAL_CALL rtl_bootstrap_args_open (
    rtl_uString * pIniName
) SAL_THROW_EXTERN_C()
{
    OUString workDir;
    OUString iniName( pIniName );

    osl_getProcessWorkingDir( &workDir.pData );
    osl::FileBase::getAbsoluteFileURL( workDir, iniName, iniName );

    // normalize path
    FileStatus status( FileStatusMask_FileURL );
    DirectoryItem dirItem;
    if (DirectoryItem::E_None != DirectoryItem::get( iniName, dirItem ) ||
        DirectoryItem::E_None != dirItem.getFileStatus( status ))
    {
        return 0;
    }
    iniName = status.getFileURL();
    
    Bootstrap_Impl * that;
    osl::ResettableMutexGuard guard( osl::Mutex::getGlobalMutex() );
    t_bootstrap_map& r_bootstrap_map = bootstrap_map::get();
    t_bootstrap_map::const_iterator iFind( r_bootstrap_map.find( iniName ) );
    if (iFind == r_bootstrap_map.end())
    {
        guard.clear();
        that = new Bootstrap_Impl( iniName );
        guard.reset();
        iFind = r_bootstrap_map.find( iniName );
        if (iFind == r_bootstrap_map.end())
        {
            ++that->_nRefCount;
            ::std::pair< t_bootstrap_map::iterator, bool > insertion(
                r_bootstrap_map.insert(
                    t_bootstrap_map::value_type( iniName, that ) ) );
            OSL_ASSERT( insertion.second );
        }
        else
        {
            Bootstrap_Impl * obsolete = that;
            that = iFind->second;
            ++that->_nRefCount;
            guard.clear();
            delete obsolete;
        }
    }
    else
    {
        that = iFind->second;
        ++that->_nRefCount;
    }
    return static_cast< rtlBootstrapHandle >( that );
}

//----------------------------------------------------------------------------

void SAL_CALL rtl_bootstrap_args_close (
    rtlBootstrapHandle handle
) SAL_THROW_EXTERN_C()
{
    if (handle == 0)
        return;
    Bootstrap_Impl * that = static_cast< Bootstrap_Impl * >( handle );
    
    osl::MutexGuard guard( osl::Mutex::getGlobalMutex() );
    t_bootstrap_map& r_bootstrap_map = bootstrap_map::get();
    OSL_ASSERT(
        r_bootstrap_map.find( that->_iniName )->second == that );
    --that->_nRefCount;
    if (that->_nRefCount == 0)
    {
        ::std::size_t nLeaking = 8; // only hold up to 8 files statically

#if OSL_DEBUG_LEVEL == 1 // nonpro
        nLeaking = 0;
#elif OSL_DEBUG_LEVEL > 1 // debug
        nLeaking = 1;
#endif /* OSL_DEBUG_LEVEL */

        if (r_bootstrap_map.size() > nLeaking)
        {
            ::std::size_t erased = r_bootstrap_map.erase( that->_iniName );
            OSL_ASSERT( erased == 1 );
            delete that;
        }
    }
}

//----------------------------------------------------------------------------

static rtlBootstrapHandle get_static_bootstrap_handle() SAL_THROW(())
{
    osl::MutexGuard guard( osl::Mutex::getGlobalMutex() );
    static rtlBootstrapHandle s_handle = 0;
    if (s_handle == 0)
    {
        OUString iniName (getIniFileName_Impl());
        s_handle = rtl_bootstrap_args_open( iniName.pData );
        if (s_handle == 0)
        {
            Bootstrap_Impl * that = new Bootstrap_Impl( iniName );
            ++that->_nRefCount;
            s_handle = static_cast<rtlBootstrapHandle>(that);
        }
    }
    return s_handle;
}

//----------------------------------------------------------------------------

sal_Bool SAL_CALL rtl_bootstrap_get_from_handle (
    rtlBootstrapHandle handle,
    rtl_uString      * pName,
    rtl_uString     ** ppValue,
    rtl_uString      * pDefault
) SAL_THROW_EXTERN_C()
{
    osl::MutexGuard guard( osl::Mutex::getGlobalMutex() );

    sal_Bool found = sal_False;
    if(ppValue && pName)
    {
        if (handle == 0)
            handle = get_static_bootstrap_handle();
        found = static_cast< Bootstrap_Impl * >( handle )->getValue(
            pName, ppValue, pDefault );
    }

    return found;
}

//----------------------------------------------------------------------------

void SAL_CALL rtl_bootstrap_get_iniName_from_handle (
    rtlBootstrapHandle handle,
    rtl_uString     ** ppIniName
) SAL_THROW_EXTERN_C()
{
    if(ppIniName)
    {
        if(handle)
        {
            Bootstrap_Impl * pImpl = static_cast<Bootstrap_Impl*>(handle);
            rtl_uString_assign(ppIniName, pImpl->_iniName.pData);
        }
        else
        {
            const OUString & iniName = getIniFileName_Impl();       
            rtl_uString_assign(ppIniName, iniName.pData);
        }
    }
}

//----------------------------------------------------------------------------

void SAL_CALL rtl_bootstrap_setIniFileName (
    rtl_uString * pName
) SAL_THROW_EXTERN_C()
{
    osl::MutexGuard guard( osl::Mutex::getGlobalMutex() );
    OUString & file = getIniFileName_Impl();
    file = pName;
}

//----------------------------------------------------------------------------

sal_Bool SAL_CALL rtl_bootstrap_get (
    rtl_uString  * pName,
    rtl_uString ** ppValue,
    rtl_uString  * pDefault
) SAL_THROW_EXTERN_C()
{
    return rtl_bootstrap_get_from_handle(0, pName, ppValue, pDefault);
}

//----------------------------------------------------------------------------

void SAL_CALL rtl_bootstrap_set (
    rtl_uString * pName,
    rtl_uString * pValue
) SAL_THROW_EXTERN_C()
{
    OUString const & name = *reinterpret_cast< OUString const * >( &pName );
    OUString const & value = *reinterpret_cast< OUString const * >( &pValue );

    osl::MutexGuard guard( osl::Mutex::getGlobalMutex() );

    NameValueList& r_rtl_bootstrap_set_list = rtl_bootstrap_set_list::get();
    NameValueList::iterator iPos( r_rtl_bootstrap_set_list.begin() );
    NameValueList::iterator iEnd( r_rtl_bootstrap_set_list.end() );
    for ( ; iPos != iEnd; ++iPos )
    {
        if (iPos->sName.equals( name ))
        {
            iPos->sValue = value;
            return;
        }
    }

#if OSL_DEBUG_LEVEL > 1
    OString cstr_name( OUStringToOString( name, RTL_TEXTENCODING_ASCII_US ) );
    OString cstr_value( OUStringToOString( value, RTL_TEXTENCODING_ASCII_US ) );
    OSL_TRACE(
        "bootstrap.cxx: explicitly setting: name=%s value=%s\n",
        cstr_name.getStr(), cstr_value.getStr() );
#endif /* OSL_DEBUG_LEVEL > 1 */

    r_rtl_bootstrap_set_list.push_back( rtl_bootstrap_NameValue( name, value ) );
}

//----------------------------------------------------------------------------

void SAL_CALL rtl_bootstrap_expandMacros_from_handle (
    rtlBootstrapHandle handle,
    rtl_uString     ** macro
) SAL_THROW_EXTERN_C()
{
    OUString expanded( expandMacros( handle,
                                     * reinterpret_cast< OUString const * >( macro ) ) );
    rtl_uString_assign( macro, expanded.pData );
}

//----------------------------------------------------------------------------

void SAL_CALL rtl_bootstrap_expandMacros(
    rtl_uString ** macro )
    SAL_THROW_EXTERN_C()
{
    OUString expanded( expandMacros( get_static_bootstrap_handle(),
                                     * reinterpret_cast< OUString const * >( macro ) ) );
    rtl_uString_assign( macro, expanded.pData );
}

//----------------------------------------------------------------------------
