/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: cunomaker.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 03:37:36 $
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
#include "precompiled_codemaker.hxx"

#include <stdio.h>

#include "sal/main.h"

#ifndef _CODEMAKER_TYPEMANAGER_HXX_
#include <codemaker/typemanager.hxx>
#endif
#ifndef _CODEMAKER_DEPENDENCY_HXX_
#include <codemaker/dependency.hxx>
#endif

#include "cunooptions.hxx"
#include "cunotype.hxx"

using namespace rtl;

sal_Bool produceAllTypes(const OString& typeName,
                         TypeManager& typeMgr, 
                         TypeDependency& typeDependencies,
                         CunoOptions* pOptions,
                         sal_Bool bFullScope)
    throw( CannotDumpException )
{
    if (!produceType(typeName, typeMgr,	typeDependencies, pOptions))
    {
        fprintf(stderr, "%s ERROR: %s\n", 
                pOptions->getProgramName().getStr(), 
                OString("cannot dump Type '" + typeName + "'").getStr());
        exit(99);
    }

    RegistryKey	typeKey = typeMgr.getTypeKey(typeName);
    RegistryKeyNames subKeys;
    
    if (typeKey.getKeyNames(OUString(), subKeys))
        return sal_False;
    
    OString tmpName;
    for (sal_uInt32 i=0; i < subKeys.getLength(); i++)
    {
        tmpName = OUStringToOString(subKeys.getElement(i), RTL_TEXTENCODING_UTF8);

        if (pOptions->isValid("-B"))
            tmpName = tmpName.copy(tmpName.indexOf('/', 1) + 1);
        else
            tmpName = tmpName.copy(1);

        if (bFullScope)
        {
            if (!produceAllTypes(tmpName, typeMgr, typeDependencies, pOptions, sal_True))
                return sal_False;
        } else
        {
            if (!produceType(tmpName, typeMgr, typeDependencies, pOptions))
                return sal_False;
        }
    }
    
    return sal_True;			
}

SAL_IMPLEMENT_MAIN_WITH_ARGS(argc, argv)
{
    CunoOptions options;

    try 
    {
        if (!options.initOptions(argc, argv))
        {
            exit(1);
        }
    }
    catch( IllegalArgument& e)
    {
        fprintf(stderr, "Illegal option: %s\n", e.m_message.getStr());
        exit(99);
    }

    RegistryTypeManager typeMgr;
    TypeDependency		typeDependencies;
    
    if (!typeMgr.init(!options.isValid("-T"), options.getInputFiles()))
    {
        fprintf(stderr, "%s : init registries failed, check your registry files.\n", options.getProgramName().getStr());
        exit(99);
    }

    if (options.isValid("-B"))
    {
        typeMgr.setBase(options.getOption("-B"));
    }

    try 
    {
        if (options.isValid("-T"))
        {
            OString tOption(options.getOption("-T"));

            OString typeName, tmpName;
            sal_Bool ret = sal_False;
            sal_Int32 nIndex = 0;
            do
            {
                typeName = tOption.getToken(0, ';', nIndex);

                sal_Int32 nPos = typeName.lastIndexOf( '.' );
                tmpName = typeName.copy( nPos != -1 ? nPos+1 : 0 );
                if (tmpName == "*")
                {
                    // produce this type and his scope, but the scope is not recursively  generated.
                    if (typeName.equals("*"))
                    {
                        tmpName = "/";
                    } else
                    {
                        tmpName = typeName.copy(0, typeName.lastIndexOf('.')).replace('.', '/');
                        if (tmpName.getLength() == 0) 
                            tmpName = "/";
                        else
                            tmpName.replace('.', '/');
                    }
                    ret = produceAllTypes(tmpName, typeMgr, typeDependencies, &options, sal_False);
                } else
                {
                    // produce only this type
                    ret = produceType(typeName.replace('.', '/'), typeMgr, typeDependencies, &options);
                }

                if (!ret)
                {
                    fprintf(stderr, "%s ERROR: %s\n", 
                            options.getProgramName().getStr(), 
                            OString("cannot dump Type '" + typeName + "'").getStr());
                    exit(99);
                }
            } while( nIndex != -1 );
        } else
        {
            // produce all types
            if (!produceAllTypes("/", typeMgr, typeDependencies, &options, sal_True))
            {
                fprintf(stderr, "%s ERROR: %s\n", 
                        options.getProgramName().getStr(), 
                        "an error occurs while dumping all types.");
                exit(99);
            }
        }
    }
    catch( CannotDumpException& e)
    {
        fprintf(stderr, "%s ERROR: %s\n", 
                options.getProgramName().getStr(), 
                e.m_message.getStr());
        exit(99);
    }

    return 0;
}


