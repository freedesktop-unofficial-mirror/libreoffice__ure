/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: bootstrap.hxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 14:35:41 $
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
#ifndef _RTL_BOOTSTRAP_HXX_
#define _RTL_BOOTSTRAP_HXX_
#include <rtl/ustring.hxx>
#include <rtl/bootstrap.h>

namespace rtl
{
    class Bootstrap
    {
        void * _handle;
        
        /** @internal */
        inline Bootstrap( Bootstrap const & ); // not impl
        /** @internal */
        inline Bootstrap & operator = ( Bootstrap const & ); // not impl
        
    public:
        /**
           @see rtl_bootstrap_setIniFileName()
         */
        static inline void SAL_CALL setIniFilename( const ::rtl::OUString &sFile );

        /** Retrieves a bootstrap parameter
           @param sName name of the bootstrap value. case insensitive.
           @param outValue (out parameter). On success contains the value, otherwise
                  an empty string.
           @return sal_False, if no value could be retrieved, otherwise sal_True
           @see rtl_bootstrap_get()
         */
        static inline sal_Bool get(
            const ::rtl::OUString &sName,
            ::rtl::OUString &outValue );

        /** Retrieves a bootstrap parameter
           
           @param sName name of the bootstrap value. case insensitive.
           @param outValue (out parameter). Contains the value associated with sName.
           @param aDefault if none of the other methods retrieved a value, outValue
                           is assigned to a Default.
                  
           @see rtl_bootstrap_get()
         */
        static inline void get(
            const ::rtl::OUString &sName,
            ::rtl::OUString &outValue,
            const ::rtl::OUString &aDefault );

        /** Sets a bootstrap parameter.
            
            @param pName 
                   name of bootstrap parameter
            @param pValue
                   value of bootstrap parameter

            @see rtl_bootstrap_set()
        */
        static inline void set( ::rtl::OUString const & name, ::rtl::OUString const & value )
            SAL_THROW( () );
        
        /** default ctor.
         */
        inline Bootstrap();

        /** Opens a bootstrap argment container
            @see rtl_bootstrap_args_open()
         */
        inline Bootstrap(const OUString & iniName);

        /** Closes a bootstrap argument container
            @see rtl_bootstrap_args_close()
        */
        inline ~Bootstrap();

        /** Retrieves a bootstrap argument.

            It is first tried to retrieve the value via the global function
            and second via the special bootstrap container.
            @see rtl_bootstrap_get_from_handle()
        */
        
        inline sal_Bool getFrom(const ::rtl::OUString &sName,
                                ::rtl::OUString &outValue) const;

        /** Retrieves a bootstrap argument.

            It is first tried to retrieve the value via the global function
            and second via the special bootstrap container.
            @see rtl_bootstrap_get_from_handle()
        */
        inline void getFrom(const ::rtl::OUString &sName,
                            ::rtl::OUString &outValue,
                            const ::rtl::OUString &aDefault) const;

        /** Retrieves the name of the underlying ini-file.
            @see rtl_bootstrap_get_iniName_from_handle()
         */
        inline void getIniName(::rtl::OUString & iniName) const;
        
        /** Expands a macro using bootstrap variables.
            
            @param macro    [inout]  The macro to be expanded
        */
        inline void expandMacrosFrom( ::rtl::OUString & macro ) const SAL_THROW( () )
            { rtl_bootstrap_expandMacros_from_handle( _handle, &macro.pData ); }
        
        /** Expands a macro using default bootstrap variables.
            
            @param macro    [inout]  The macro to be expanded
        */
        static inline void expandMacros( ::rtl::OUString & macro ) SAL_THROW( () )
            { rtl_bootstrap_expandMacros( &macro.pData ); }

        /** Provides the bootstrap internal handle.

            @return bootstrap handle
        */
        inline rtlBootstrapHandle getHandle() const SAL_THROW( () )
            { return _handle; }
    };

    //----------------------------------------------------------------------------
    // IMPLEMENTATION
    //----------------------------------------------------------------------------
    inline void Bootstrap::setIniFilename( const ::rtl::OUString &sFile )
    {
        rtl_bootstrap_setIniFileName( sFile.pData );
    }

    inline sal_Bool Bootstrap::get( const ::rtl::OUString &sName,
                                    ::rtl::OUString & outValue )
    {
        return rtl_bootstrap_get( sName.pData , &(outValue.pData) , 0 );
    }

    inline void Bootstrap::get( const ::rtl::OUString &sName,
                                ::rtl::OUString & outValue,
                                const ::rtl::OUString & sDefault )
    {
        rtl_bootstrap_get( sName.pData , &(outValue.pData) , sDefault.pData );
    }

    inline void Bootstrap::set( ::rtl::OUString const & name, ::rtl::OUString const & value )
        SAL_THROW( () )
    {
        rtl_bootstrap_set( name.pData, value.pData );
    }

    inline Bootstrap::Bootstrap()
    {
        _handle = 0;
    }

    inline Bootstrap::Bootstrap(const OUString & iniName)
    {
        if(iniName.getLength())
            _handle = rtl_bootstrap_args_open(iniName.pData);

        else
            _handle = 0;
    }

    inline Bootstrap::~Bootstrap() 
    {
        rtl_bootstrap_args_close(_handle);
    }


    inline sal_Bool Bootstrap::getFrom(const ::rtl::OUString &sName,
                                       ::rtl::OUString &outValue) const
    {
        return rtl_bootstrap_get_from_handle(_handle, sName.pData, &outValue.pData, 0);
    }

    inline void Bootstrap::getFrom(const ::rtl::OUString &sName,
                                   ::rtl::OUString &outValue,
                                   const ::rtl::OUString &aDefault) const
    {
        rtl_bootstrap_get_from_handle(_handle, sName.pData, &outValue.pData, aDefault.pData);
    }

    inline void Bootstrap::getIniName(::rtl::OUString & iniName) const
    {
        rtl_bootstrap_get_iniName_from_handle(_handle, &iniName.pData);
    }

}
#endif
