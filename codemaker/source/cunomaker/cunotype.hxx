/*************************************************************************
 *
 *  $RCSfile: cunotype.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: jsc $ $Date: 2001-04-20 13:58:54 $
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

#ifndef _CUNOMAKER_CUNOTYPE_HXX_
#define _CUNOMAKER_CUNOTYPE_HXX_

#ifndef _CODEMAKER_TYPEMANAGER_HXX_
#include	<codemaker/typemanager.hxx>
#endif

#ifndef _CODEMAKER_DEPENDENCY_HXX_
#include	<codemaker/dependency.hxx>
#endif

enum BASETYPE
{
    BT_INVALID,
    BT_VOID,
    BT_ANY,
    BT_TYPE,
    BT_BOOLEAN,
    BT_CHAR,		
    BT_STRING,
    BT_FLOAT,
    BT_DOUBLE,
    BT_OCTET,
    BT_BYTE,
    BT_SHORT,
    BT_LONG,
    BT_HYPER,
    BT_UNSIGNED_SHORT,
    BT_UNSIGNED_LONG,
    BT_UNSIGNED_HYPER
};


enum CunoTypeDecl
{
    CUNOTYPEDECL_ALLTYPES,
    CUNOTYPEDECL_NOINTERFACES,
    CUNOTYPEDECL_ONLYINTERFACES
};	

class CunoOptions;
class FileStream;

class CunoType
{
public:
    CunoType(TypeReader& typeReader,
             const ::rtl::OString& typeName,
             const TypeManager& typeMgr, 
             const TypeDependency& typeDependencies);

    virtual ~CunoType();

    virtual sal_Bool dump(CunoOptions* pOptions) throw( CannotDumpException );
    virtual sal_Bool dumpDependedTypes(CunoOptions* pOptions)  throw( CannotDumpException );
    virtual sal_Bool dumpHFile(FileStream& o) throw( CannotDumpException ) = 0;
    virtual sal_Bool dumpCFile(FileStream& o) throw( CannotDumpException ) = 0;

    virtual ::rtl::OString dumpHeaderDefine(FileStream& o, sal_Char* prefix, sal_Bool bExtended=sal_False);
    virtual void dumpDefaultHIncludes(FileStream& o);
    virtual void dumpDefaultCIncludes(FileStream& o);
    virtual void dumpInclude(FileStream& o, const ::rtl::OString& typeName, sal_Char* prefix, sal_Bool bExtended=sal_False, sal_Bool bCaseSensitive=sal_False);
    virtual void dumpDepIncludes(FileStream& o, const ::rtl::OString& typeName, sal_Char* prefix);

    virtual void dumpOpenExternC(FileStream& o);
    virtual void dumpCloseExternC(FileStream& o);

    virtual void dumpGetCunoType(FileStream& o);
    virtual void dumpCGetCunoType(FileStream& o);
    virtual void dumpLGetCunoType(FileStream& o);

    virtual void dumpType(FileStream& o, const ::rtl::OString& type, sal_Bool bConst=sal_False,
                          sal_Bool bPointer=sal_False, sal_Bool bParam=sal_False)
                    throw( CannotDumpException );
    ::rtl::OString	getTypeClass(const ::rtl::OString& type="", sal_Bool bCStyle=sal_False);
    ::rtl::OString	getBaseType(const ::rtl::OString& type);
    void	dumpCppuGetType(FileStream& o, const ::rtl::OString& type, sal_Bool bDecl=sal_False, CunoTypeDecl eDeclFlag=CUNOTYPEDECL_ALLTYPES);
    void	dumpTypeInit(FileStream& o, const ::rtl::OString& type);
    BASETYPE isBaseType(const ::rtl::OString& type);

    ::rtl::OString typeToIdentifier(const ::rtl::OString& type);

    void 	dumpConstantValue(FileStream& o, sal_uInt16 index);

    virtual sal_uInt32  getMemberCount();
    virtual sal_uInt32	getInheritedMemberCount();
    void 	dumpInheritedMembers(FileStream& o, rtl::OString& superType);

    sal_Bool isSeqType(const ::rtl::OString& type, ::rtl::OString& baseType, ::rtl::OString& seqPrefix);
    sal_Bool isArrayType(const ::rtl::OString& type, ::rtl::OString& baseType, ::rtl::OString& arrayPrefix);
    sal_Bool isVoid(const ::rtl::OString& type)
        { return type.equals("void"); }
    void 			inc(sal_uInt32 num=4);
    void 			dec(sal_uInt32 num=4);
    ::rtl::OString 	indent();
    ::rtl::OString	indent(sal_uInt32 num);
protected:
    virtual sal_uInt32	checkInheritedMemberCount(const TypeReader* pReader);

    ::rtl::OString	checkSpecialCunoType(const ::rtl::OString& type);
    ::rtl::OString	checkRealBaseType(const ::rtl::OString& type, sal_Bool bResolveTypeOnly = sal_False);
    void	dumpCppuGetTypeMemberDecl(FileStream& o, CunoTypeDecl eDeclFlag);

     sal_Bool isNestedType() 
        { return m_bIsNestedType; };

     RegistryKeyNames& getNestedTypeNames() 
        { return  m_nestedTypeNames; };

     sal_Bool isNestedTypeByName(const ::rtl::OString& type);
     sal_Bool hasNestedType(const ::rtl::OString& type);

protected:
    sal_uInt32 			m_inheritedMemberCount;

    sal_Bool			m_cunoTypeLib;
    sal_Bool			m_cunoTypeLeak;
    sal_Bool			m_cunoTypeDynamic;
    sal_uInt32			m_indentLength;
    ::rtl::OString		m_typeName;
    ::rtl::OString		m_name;
    TypeReader			m_reader;
    TypeManager&		m_typeMgr;	
    TypeDependency  	m_dependencies;	
    sal_Bool 			m_bIsNestedType;
    RegistryKeyNames 	m_nestedTypeNames;
};

class InterfaceType : public CunoType
{
public:
    InterfaceType(TypeReader& typeReader,
                 const ::rtl::OString& typeName,
                 const TypeManager& typeMgr, 
                 const TypeDependency& typeDependencies);

    virtual ~InterfaceType();

    sal_Bool	dumpDeclaration(FileStream& o) throw( CannotDumpException );
    sal_Bool	dumpHFile(FileStream& o) throw( CannotDumpException );
    sal_Bool	dumpCFile(FileStream& o) throw( CannotDumpException );

    void 		dumpInheritedFunctions(FileStream& o, rtl::OString& superType);
    void		dumpAttributes(FileStream& o, const ::rtl::OString& interfaceType, TypeReader& reader );
    void		dumpMethods(FileStream& o, const ::rtl::OString& interfaceType, TypeReader& reader );
    void		dumpGetCunoType(FileStream& o);
    void 		dumpCGetCunoType(FileStream& o);
    void 		dumpCUnoAttributeTypeNames(FileStream& o, sal_Bool bRelease=sal_False);
    void 		dumpCUnoMethodTypeNames(FileStream& o, sal_Bool bRelease=sal_False);
    void		dumpCUnoAttributeRefs(FileStream& o, sal_uInt32& index);
    void		dumpCUnoMethodRefs(FileStream& o, sal_uInt32& index);
    void		dumpCUnoAttributes(FileStream& o, sal_uInt32& index);
    void		dumpCUnoMethods(FileStream& o, sal_uInt32& index);
    void		dumpAttributesCppuDecl(FileStream& o, StringSet* pFinishedTypes, CunoTypeDecl eDeclFlag);
    void		dumpMethodsCppuDecl(FileStream& o, StringSet* pFinishedTypes, CunoTypeDecl eDeclFlag );

    sal_uInt32	getMemberCount();
    sal_uInt32	getInheritedMemberCount();

protected:
    sal_uInt32	checkInheritedMemberCount(const TypeReader* pReader);

protected:
    sal_uInt32 	m_inheritedMemberCount;
    sal_Bool 	m_hasAttributes;
    sal_Bool 	m_hasMethods;
};

class ModuleType : public CunoType
{
public:
    ModuleType(TypeReader& typeReader,
                  const ::rtl::OString& typeName,
               const TypeManager& typeMgr, 
               const TypeDependency& typeDependencies);

    virtual ~ModuleType();

    virtual sal_Bool 	dump(CunoOptions* pOptions) throw( CannotDumpException );

    sal_Bool			dumpDeclaration(FileStream& o) throw( CannotDumpException );
    sal_Bool			dumpHFile(FileStream& o) throw( CannotDumpException );
    sal_Bool			dumpCFile(FileStream& o) throw( CannotDumpException );
    sal_Bool			hasConstants();
};

class ConstantsType : public ModuleType
{
public:
    ConstantsType(TypeReader& typeReader,
                  const ::rtl::OString& typeName,
               const TypeManager& typeMgr, 
               const TypeDependency& typeDependencies);

    virtual ~ConstantsType();

    virtual sal_Bool 	dump(CunoOptions* pOptions) throw( CannotDumpException );
};

class StructureType : public CunoType
{
public:
    StructureType(TypeReader& typeReader,
                  const ::rtl::OString& typeName,
                  const TypeManager& typeMgr, 
                  const TypeDependency& typeDependencies);

    virtual ~StructureType();

    sal_Bool	dumpDeclaration(FileStream& o) throw( CannotDumpException );
    sal_Bool	dumpHFile(FileStream& o) throw( CannotDumpException );
    sal_Bool	dumpCFile(FileStream& o) throw( CannotDumpException );
};

class ExceptionType : public CunoType
{
public:
    ExceptionType(TypeReader& typeReader,
                  const ::rtl::OString& typeName,
                  const TypeManager& typeMgr, 
                  const TypeDependency& typeDependencies);

    virtual ~ExceptionType();

    sal_Bool	dumpDeclaration(FileStream& o) throw( CannotDumpException );
    sal_Bool	dumpHFile(FileStream& o) throw( CannotDumpException );
    sal_Bool	dumpCFile(FileStream& o) throw( CannotDumpException );
};

class EnumType : public CunoType
{
public:
    EnumType(TypeReader& typeReader,
              const ::rtl::OString& typeName,
              const TypeManager& typeMgr, 
              const TypeDependency& typeDependencies);

    virtual ~EnumType();

    sal_Bool	dumpDeclaration(FileStream& o) throw( CannotDumpException );
    sal_Bool	dumpHFile(FileStream& o) throw( CannotDumpException );
    sal_Bool	dumpCFile(FileStream& o) throw( CannotDumpException );

    void		dumpGetCunoType(FileStream& o);
    void 		dumpCGetCunoType(FileStream& o);
};

class TypeDefType : public CunoType
{
public:
    TypeDefType(TypeReader& typeReader,
              const ::rtl::OString& typeName,
              const TypeManager& typeMgr,
              const TypeDependency& typeDependencies);

    virtual ~TypeDefType();

    sal_Bool	dumpDeclaration(FileStream& o) throw( CannotDumpException );
    sal_Bool	dumpHFile(FileStream& o) throw( CannotDumpException );
    sal_Bool	dumpCFile(FileStream& o) throw( CannotDumpException );

    void		dumpGetCunoType(FileStream& o);
    void		dumpLGetCunoType(FileStream& o);
    void 		dumpCGetCunoType(FileStream& o);
};


sal_Bool produceType(const ::rtl::OString& typeName,
                     TypeManager& typeMgr, 
                     TypeDependency& typeDependencies,
                     CunoOptions* pOptions)
                 throw( CannotDumpException );

/**
 * This function returns a C++ scoped name, represents the namespace 
 * scoping of this type, e.g. com:.sun::star::uno::XInterface. If the scope of
 * the type is equal scope, the relativ name will be used.
 */
::rtl::OString scopedName(const ::rtl::OString& scope, const ::rtl::OString& type, 
                   sal_Bool bNoNameSpace=sal_False);

::rtl::OString shortScopedName(const ::rtl::OString& scope, const ::rtl::OString& type, 
                           sal_Bool bNoNameSpace=sal_False);


#endif // _CUNOMAKER_CUNOTYPE_HXX_

