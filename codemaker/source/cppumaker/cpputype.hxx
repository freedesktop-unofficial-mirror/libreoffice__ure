/*************************************************************************
 *
 *  $RCSfile: cpputype.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2004-03-30 16:53:12 $
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

#ifndef _CPPUMAKER_CPPUTYPE_HXX_
#define _CPPUMAKER_CPPUTYPE_HXX_

#include "codemaker/dependency.hxx"
#include "codemaker/options.hxx"
#include "codemaker/typemanager.hxx"

#include "registry/reader.hxx"
#include "registry/types.h"

namespace codemaker { struct ExceptionTreeNode; }

enum CppuTypeDecl
{
    CPPUTYPEDECL_ALLTYPES,
    CPPUTYPEDECL_NOINTERFACES,
    CPPUTYPEDECL_ONLYINTERFACES
};	

class CppuOptions;
class FileStream;

class CppuType
{
public:
    CppuType(typereg::Reader& typeReader,
             const ::rtl::OString& typeName,
             const TypeManager& typeMgr, 
             const TypeDependency& typeDependencies);

    virtual ~CppuType();

    virtual sal_Bool dump(CppuOptions* pOptions) throw( CannotDumpException );
    virtual sal_Bool dumpFile(CppuOptions* pOptions,
                              const ::rtl::OString& sExtension,
                              const ::rtl::OString& sName,
                              const ::rtl::OString& sOutPath )
        throw( CannotDumpException );   
    void dumpDependedTypes(CppuOptions * options);
    virtual sal_Bool dumpHFile(FileStream& o) throw( CannotDumpException ) = 0;
    virtual sal_Bool dumpHxxFile(FileStream& o) throw( CannotDumpException ) = 0;

    virtual ::rtl::OString dumpHeaderDefine(FileStream& o, sal_Char* prefix, sal_Bool bExtended=sal_False);
    virtual void dumpDefaultHIncludes(FileStream& o);
    virtual void dumpDefaultHxxIncludes(FileStream& o);
    virtual void dumpInclude(FileStream& o, const ::rtl::OString& typeName, sal_Char* prefix, sal_Bool bExtended=sal_False, sal_Bool bCaseSensitive=sal_False);

    virtual void dumpDepIncludes(FileStream& o, const ::rtl::OString& typeName, sal_Char* prefix);

    virtual void dumpNameSpace(FileStream& o, sal_Bool bOpen = sal_True, sal_Bool bFull = sal_False, const ::rtl::OString& type="");
    virtual void dumpGetCppuType(FileStream& o);
    virtual void dumpCGetCppuType(FileStream& o);
    virtual void dumpLGetCppuType(FileStream& o);

    virtual void dumpType(FileStream& o, const ::rtl::OString& type, sal_Bool bConst=sal_False,
                          sal_Bool bRef=sal_False, sal_Bool bNative=sal_False)
                    throw( CannotDumpException );
    ::rtl::OString	getTypeClass(const ::rtl::OString& type="", sal_Bool bCStyle=sal_False);
    void	dumpCppuGetType(FileStream& o, const ::rtl::OString& type, sal_Bool bDecl=sal_False, CppuTypeDecl eDeclFlag=CPPUTYPEDECL_ALLTYPES);
    void	dumpTypeInit(FileStream& o, const ::rtl::OString& type);

    ::rtl::OString typeToIdentifier(const ::rtl::OString& type);

    void 	dumpConstantValue(FileStream& o, sal_uInt16 index);

    virtual sal_uInt32  getMemberCount();
    virtual sal_uInt32	getInheritedMemberCount();

    void 			inc(sal_uInt32 num=4);
    void 			dec(sal_uInt32 num=4);
    ::rtl::OString 	indent();
    ::rtl::OString	indent(sal_uInt32 num);
protected:
    virtual sal_uInt32 checkInheritedMemberCount(
        const typereg::Reader* pReader);

    bool passByReference(rtl::OString const & unoType);

    ::rtl::OString	resolveTypedefs(const ::rtl::OString& type);
    ::rtl::OString	checkRealBaseType(const ::rtl::OString& type, sal_Bool bResolveTypeOnly = sal_False);
    void	dumpCppuGetTypeMemberDecl(FileStream& o, CppuTypeDecl eDeclFlag);

    bool isGlobal() const;

     sal_Bool isNestedType() 
        { return m_bIsNestedType; };

     RegistryKeyNames& getNestedTypeNames() 
        { return  m_nestedTypeNames; };

     sal_Bool isNestedTypeByName(const ::rtl::OString& type);
     sal_Bool hasNestedType(const ::rtl::OString& type);

protected:
    sal_uInt32 			m_inheritedMemberCount;

    sal_Bool			m_cppuTypeLeak;
    sal_Bool			m_cppuTypeDynamic;
    sal_Bool			m_cppuTypeStatic;
    sal_uInt32			m_indentLength;
    ::rtl::OString		m_typeName;
    ::rtl::OString		m_name;
    typereg::Reader     m_reader;
    TypeManager&		m_typeMgr;	
    TypeDependency  	m_dependencies;	
    sal_Bool 			m_bIsNestedType;
    RegistryKeyNames 	m_nestedTypeNames;
};

class InterfaceType : public CppuType
{
public:
    InterfaceType(typereg::Reader& typeReader,
                 const ::rtl::OString& typeName,
                 const TypeManager& typeMgr, 
                 const TypeDependency& typeDependencies);

    virtual ~InterfaceType();

    sal_Bool	dumpDeclaration(FileStream& o) throw( CannotDumpException );
    sal_Bool	dumpHFile(FileStream& o) throw( CannotDumpException );
    sal_Bool	dumpHxxFile(FileStream& o) throw( CannotDumpException );

    void		dumpAttributes(FileStream& o);
    void		dumpMethods(FileStream& o);
    void		dumpGetCppuType(FileStream& o);
    void 		dumpCGetCppuType(FileStream& o);
    void		dumpCppuAttributeRefs(FileStream& o, sal_uInt32& index);
    void		dumpCppuMethodRefs(FileStream& o, sal_uInt32& index);
    void		dumpCppuAttributes(FileStream& o, sal_uInt32& index);
    void		dumpCppuMethods(FileStream& o, sal_uInt32& index);
    void		dumpAttributesCppuDecl(FileStream& o, StringSet* pFinishedTypes, CppuTypeDecl eDeclFlag);
    void		dumpMethodsCppuDecl(FileStream& o, StringSet* pFinishedTypes, CppuTypeDecl eDeclFlag );

    sal_uInt32	getMemberCount();
    sal_uInt32	getInheritedMemberCount();

protected:
    sal_uInt32	checkInheritedMemberCount(const typereg::Reader* pReader);

protected:
    sal_uInt32 	m_inheritedMemberCount;
    sal_Bool 	m_hasAttributes;
    sal_Bool 	m_hasMethods;

private:
    void dumpExceptionSpecification(
        FileStream & out, sal_uInt32 methodIndex, bool runtimeException);

    void dumpAttributeExceptionSpecification(
        FileStream & out, rtl::OUString const & name, RTMethodMode sort);

    void dumpExceptionTypeName(
        FileStream & out, char const * prefix, sal_uInt32 index,
        rtl::OUString name);

    sal_Int32 dumpExceptionTypeNames(
        FileStream & out, char const * prefix, sal_uInt16 methodIndex,
        bool runtimeException);

    sal_Int32 dumpAttributeExceptionTypeNames(
        FileStream & out, char const * prefix, rtl::OUString const & name,
        RTMethodMode sort);
};

class ModuleType : public CppuType
{
public:
    ModuleType(typereg::Reader& typeReader,
                  const ::rtl::OString& typeName,
               const TypeManager& typeMgr, 
               const TypeDependency& typeDependencies);

    virtual ~ModuleType();

    virtual sal_Bool 	dump(CppuOptions* pOptions) throw( CannotDumpException );

    sal_Bool			dumpDeclaration(FileStream& o) throw( CannotDumpException );
    sal_Bool			dumpHFile(FileStream& o) throw( CannotDumpException );
    sal_Bool			dumpHxxFile(FileStream& o) throw( CannotDumpException );
    sal_Bool			hasConstants();
};

class ConstantsType : public ModuleType
{
public:
    ConstantsType(typereg::Reader& typeReader,
                  const ::rtl::OString& typeName,
               const TypeManager& typeMgr, 
               const TypeDependency& typeDependencies);

    virtual ~ConstantsType();

    virtual sal_Bool 	dump(CppuOptions* pOptions) throw( CannotDumpException );
};

class StructureType : public CppuType
{
public:
    StructureType(typereg::Reader& typeReader,
                  const ::rtl::OString& typeName,
                  const TypeManager& typeMgr, 
                  const TypeDependency& typeDependencies);

    virtual ~StructureType();

    sal_Bool	dumpDeclaration(FileStream& o) throw( CannotDumpException );
    sal_Bool	dumpHFile(FileStream& o) throw( CannotDumpException );
    sal_Bool	dumpHxxFile(FileStream& o) throw( CannotDumpException );

    sal_Bool	dumpSuperMember(FileStream& o, const ::rtl::OString& super, sal_Bool bWithType);
};

class ExceptionType : public CppuType
{
public:
    ExceptionType(typereg::Reader& typeReader,
                  const ::rtl::OString& typeName,
                  const TypeManager& typeMgr, 
                  const TypeDependency& typeDependencies);

    virtual ~ExceptionType();

    sal_Bool	dumpDeclaration(FileStream& o) throw( CannotDumpException );
    sal_Bool	dumpHFile(FileStream& o) throw( CannotDumpException );
    sal_Bool	dumpHxxFile(FileStream& o) throw( CannotDumpException );

    sal_Bool	dumpSuperMember(FileStream& o, const ::rtl::OString& super, sal_Bool bWithType);
};

class EnumType : public CppuType
{
public:
    EnumType(typereg::Reader& typeReader,
              const ::rtl::OString& typeName,
              const TypeManager& typeMgr, 
              const TypeDependency& typeDependencies);

    virtual ~EnumType();

    sal_Bool	dumpDeclaration(FileStream& o) throw( CannotDumpException );
    sal_Bool	dumpHFile(FileStream& o) throw( CannotDumpException );
    sal_Bool	dumpHxxFile(FileStream& o) throw( CannotDumpException );

    void		dumpGetCppuType(FileStream& o);
    void 		dumpCGetCppuType(FileStream& o);
};

class TypeDefType : public CppuType
{
public:
    TypeDefType(typereg::Reader& typeReader,
              const ::rtl::OString& typeName,
              const TypeManager& typeMgr,
              const TypeDependency& typeDependencies);

    virtual ~TypeDefType();

    sal_Bool	dumpDeclaration(FileStream& o) throw( CannotDumpException );
    sal_Bool	dumpHFile(FileStream& o) throw( CannotDumpException );
    sal_Bool	dumpHxxFile(FileStream& o) throw( CannotDumpException );

    void		dumpGetCppuType(FileStream& o);
    void		dumpLGetCppuType(FileStream& o);
    void 		dumpCGetCppuType(FileStream& o);
};

class ConstructiveType: public CppuType {
public:
    ConstructiveType(
        typereg::Reader & reader, rtl::OString const & name,
        TypeManager const & manager, TypeDependency const & dependencies):
        CppuType(reader, name, manager, dependencies) {}

    virtual sal_Bool dumpFile(
        CppuOptions * pOptions, rtl::OString const & sExtension,
        rtl::OString const & sName, rtl::OString const & sOutPath)
        throw (CannotDumpException);

    virtual sal_Bool dumpHFile(FileStream & o) throw (CannotDumpException);
};

class ServiceType: public ConstructiveType {
public:
    ServiceType(
        typereg::Reader & reader, rtl::OString const & name,
        TypeManager const & manager, TypeDependency const & dependencies):
        ConstructiveType(reader, name, manager, dependencies) {}

    bool isSingleInterfaceBased();

    virtual sal_Bool dumpHxxFile(FileStream & o) throw (CannotDumpException);

private:
    bool hasRestParameter(sal_uInt16 ctorIndex) const;

    void dumpCatchClauses(
        FileStream & out, codemaker::ExceptionTreeNode const * node);
};

class SingletonType: public ConstructiveType {
public:
    SingletonType(
        typereg::Reader & reader, rtl::OString const & name,
        TypeManager const & manager, TypeDependency const & dependencies):
        ConstructiveType(reader, name, manager, dependencies) {}

    bool isInterfaceBased();

    virtual sal_Bool dumpHxxFile(FileStream & o) throw (CannotDumpException);
};

sal_Bool produceType(const ::rtl::OString& typeName,
                     TypeManager& typeMgr, 
                     TypeDependency& typeDependencies,
                     CppuOptions* pOptions)
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


#endif // _CPPUMAKER_CPPUTYPE_HXX_

