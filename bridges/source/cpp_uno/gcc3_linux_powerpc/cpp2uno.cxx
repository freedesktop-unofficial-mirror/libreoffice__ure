/*************************************************************************
 *
 *  $RCSfile: cpp2uno.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: obo $ $Date: 2004-03-19 13:25:27 $
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

#include <com/sun/star/uno/genfunc.hxx>
#include <uno/data.h>
#include <typelib/typedescription.hxx>

#include "bridges/cpp_uno/shared/bridge.hxx"
#include "bridges/cpp_uno/shared/cppinterfaceproxy.hxx"
#include "bridges/cpp_uno/shared/types.hxx"
#include "bridges/cpp_uno/shared/vtablefactory.hxx"

#include "share.hxx"
// #include <stdio.h>


using namespace ::com::sun::star::uno;

namespace
{

//==================================================================================================
static typelib_TypeClass cpp2uno_call(
    bridges::cpp_uno::shared::CppInterfaceProxy * pThis,
    const typelib_TypeDescription * pMemberTypeDescr,
    typelib_TypeDescriptionReference * pReturnTypeRef, // 0 indicates void return
    sal_Int32 nParams, typelib_MethodParameter * pParams,
        void ** gpreg, void ** fpreg, void ** ovrflw,
    sal_Int64 * pRegisterReturn /* space for register return */ )
{
        int ng = 0; //number of gpr registers used 
        int nf = 0; //number of fpr regsiters used
        void ** pCppStack; //temporary stack pointer
       
        // gpreg:  [ret *], this, [gpr params]
        // fpreg:  [fpr params]
        // ovrflw: [gpr or fpr params (properly aligned)]

    // return
    typelib_TypeDescription * pReturnTypeDescr = 0;
    if (pReturnTypeRef)
        TYPELIB_DANGER_GET( &pReturnTypeDescr, pReturnTypeRef );
    
    void * pUnoReturn = 0;
    void * pCppReturn = 0; // complex return ptr: if != 0 && != pUnoReturn, reconversion need
    
    if (pReturnTypeDescr)
    {
        if (bridges::cpp_uno::shared::isSimpleType( pReturnTypeDescr ))
        {
            pUnoReturn = pRegisterReturn; // direct way for simple types
        }
        else // complex return via ptr (pCppReturn)
        {
            pCppReturn = *(void **)gpreg;
                        gpreg++;
                        ng++;
            
            pUnoReturn = (bridges::cpp_uno::shared::relatesToInterfaceType( pReturnTypeDescr )
                          ? alloca( pReturnTypeDescr->nSize )
                          : pCppReturn); // direct way
        }
    }
    // pop this
        gpreg++; 
        ng++;

    // stack space
    OSL_ENSURE( sizeof(void *) == sizeof(sal_Int32), "### unexpected size!" );
    // parameters
    void ** pUnoArgs = (void **)alloca( 4 * sizeof(void *) * nParams );
    void ** pCppArgs = pUnoArgs + nParams;
    // indizes of values this have to be converted (interface conversion cpp<=>uno)
    sal_Int32 * pTempIndizes = (sal_Int32 *)(pUnoArgs + (2 * nParams));
    // type descriptions for reconversions
    typelib_TypeDescription ** ppTempParamTypeDescr = (typelib_TypeDescription **)(pUnoArgs + (3 * nParams));
    
    sal_Int32 nTempIndizes   = 0;
    
    for ( sal_Int32 nPos = 0; nPos < nParams; ++nPos )
    {
        const typelib_MethodParameter & rParam = pParams[nPos];
        typelib_TypeDescription * pParamTypeDescr = 0;
        TYPELIB_DANGER_GET( &pParamTypeDescr, rParam.pTypeRef );

        if (!rParam.bOut && bridges::cpp_uno::shared::isSimpleType( pParamTypeDescr )) 
                // value
        {

            switch (pParamTypeDescr->eTypeClass)
            {

              case typelib_TypeClass_DOUBLE:
               if (nf < 8) {
                  pCppArgs[nPos] = fpreg;
                  pUnoArgs[nPos] = fpreg;
                  nf++;
                  fpreg += 2;
               } else {
                if (((long)ovrflw) & 4) ovrflw++;
                pCppArgs[nPos] = ovrflw;
                pUnoArgs[nPos] = ovrflw;
                    ovrflw += 2;
               }
               break;

               case typelib_TypeClass_FLOAT:
                // fpreg are all double values so need to
                // modify fpreg to be a single word float value
                if (nf < 8) {
                   float tmp = (float) (*((double *)fpreg));
                   (*((float *) fpreg)) = tmp;
                   pCppArgs[nPos] = fpreg;
                   pUnoArgs[nPos] = fpreg;
                   nf++;
                   fpreg += 2;
                } else {
#if 0 /* abi is not being followed correctly */
                  if (((long)ovrflw) & 4) ovrflw++;
                  float tmp = (float) (*((double *)ovrflw));
                  (*((float *) ovrflw)) = tmp;
                  pCppArgs[nPos] = ovrflw;
                  pUnoArgs[nPos] = ovrflw;
                  ovrflw += 2;
#else
                              pCppArgs[nPos] = ovrflw;
                  pUnoArgs[nPos] = ovrflw;
                  ovrflw += 1;
#endif
                            }
                break;

            case typelib_TypeClass_HYPER:
            case typelib_TypeClass_UNSIGNED_HYPER:
             if (ng < 7) {
                if (ng & 1) {
                  ng++;
                  gpreg++;
                }  
                pCppArgs[nPos] = gpreg;
                pUnoArgs[nPos] = gpreg;
                ng += 2;
                gpreg += 2;
             } else {
                if (((long)ovrflw) & 4) ovrflw++;
                pCppArgs[nPos] = ovrflw;
                pUnoArgs[nPos] = ovrflw;
                ovrflw += 2;
              }
              break;

            case typelib_TypeClass_BYTE:
            case typelib_TypeClass_BOOLEAN:
             if (ng < 8) {
                  pCppArgs[nPos] = (((char *)gpreg) + 3);
                  pUnoArgs[nPos] = (((char *)gpreg) + 3);
                  ng++;
                  gpreg++;
             } else {
                  pCppArgs[nPos] = (((char *)ovrflw) + 3);
                  pUnoArgs[nPos] = (((char *)ovrflw) + 3);
                  ovrflw++;
             }
             break;


               case typelib_TypeClass_CHAR:
               case typelib_TypeClass_SHORT:
               case typelib_TypeClass_UNSIGNED_SHORT:
            if (ng < 8) {
                  pCppArgs[nPos] = (((char *)gpreg)+ 2);
                  pUnoArgs[nPos] = (((char *)gpreg)+ 2);
                  ng++;
                  gpreg++;
            } else {
                  pCppArgs[nPos] = (((char *)ovrflw) + 2);
                  pUnoArgs[nPos] = (((char *)ovrflw) + 2);
                  ovrflw++;
            }
            break;


              default:
            if (ng < 8) {
                  pCppArgs[nPos] = gpreg;
                  pUnoArgs[nPos] = gpreg;
                  ng++;
                  gpreg++;
            } else {
                  pCppArgs[nPos] = ovrflw;
                  pUnoArgs[nPos] = ovrflw;
                  ovrflw++;
            }
                        break;

                }
                // no longer needed
            TYPELIB_DANGER_RELEASE( pParamTypeDescr );
        }
        else // ptr to complex value | ref
        {

                if (ng < 8) { 
                  pCppArgs[nPos] = *(void **)gpreg;
                  pCppStack = gpreg;
                  ng++;
                  gpreg++;
                } else {
                  pCppArgs[nPos] = *(void **)ovrflw;
                  pCppStack = ovrflw;
                 ovrflw++;
                }

            if (! rParam.bIn) // is pure out
            {
                // uno out is unconstructed mem!
                pUnoArgs[nPos] = alloca( pParamTypeDescr->nSize );
                pTempIndizes[nTempIndizes] = nPos;
                // will be released at reconversion
                ppTempParamTypeDescr[nTempIndizes++] = pParamTypeDescr;
            }
            // is in/inout
            else if (bridges::cpp_uno::shared::relatesToInterfaceType( pParamTypeDescr ))
            {
                uno_copyAndConvertData( pUnoArgs[nPos] = alloca( pParamTypeDescr->nSize ),
                                        *(void **)pCppStack, pParamTypeDescr,
                                        pThis->getBridge()->getCpp2Uno() );
                pTempIndizes[nTempIndizes] = nPos; // has to be reconverted
                // will be released at reconversion
                ppTempParamTypeDescr[nTempIndizes++] = pParamTypeDescr;
            }
            else // direct way
            {
                pUnoArgs[nPos] = *(void **)pCppStack;
                // no longer needed
                TYPELIB_DANGER_RELEASE( pParamTypeDescr );
            }
        }
    }
    
    // ExceptionHolder
    uno_Any aUnoExc; // Any will be constructed by callee
    uno_Any * pUnoExc = &aUnoExc;

    // invoke uno dispatch call
    (*pThis->getUnoI()->pDispatcher)( pThis->getUnoI(), pMemberTypeDescr, pUnoReturn, pUnoArgs, &pUnoExc );
    
    // in case an exception occured...
    if (pUnoExc)
    {
        // destruct temporary in/inout params
        for ( ; nTempIndizes--; )
        {
            sal_Int32 nIndex = pTempIndizes[nTempIndizes];
            
            if (pParams[nIndex].bIn) // is in/inout => was constructed
                uno_destructData( pUnoArgs[nIndex], ppTempParamTypeDescr[nTempIndizes], 0 );
            TYPELIB_DANGER_RELEASE( ppTempParamTypeDescr[nTempIndizes] );
        }
        if (pReturnTypeDescr)
            TYPELIB_DANGER_RELEASE( pReturnTypeDescr );
        
        CPPU_CURRENT_NAMESPACE::raiseException( &aUnoExc, pThis->getBridge()->getUno2Cpp() ); 
                // has to destruct the any
        // is here for dummy
        return typelib_TypeClass_VOID;
    }
    else // else no exception occured...
    {
        // temporary params
        for ( ; nTempIndizes--; )
        {
            sal_Int32 nIndex = pTempIndizes[nTempIndizes];
            typelib_TypeDescription * pParamTypeDescr = ppTempParamTypeDescr[nTempIndizes];
            
            if (pParams[nIndex].bOut) // inout/out
            {
                // convert and assign
                uno_destructData( pCppArgs[nIndex], pParamTypeDescr, cpp_release );
                uno_copyAndConvertData( pCppArgs[nIndex], pUnoArgs[nIndex], pParamTypeDescr,
                                        pThis->getBridge()->getUno2Cpp() );
            }
            // destroy temp uno param
            uno_destructData( pUnoArgs[nIndex], pParamTypeDescr, 0 );
            
            TYPELIB_DANGER_RELEASE( pParamTypeDescr );
        }
        // return
        if (pCppReturn) // has complex return
        {
            if (pUnoReturn != pCppReturn) // needs reconversion
            {
                uno_copyAndConvertData( pCppReturn, pUnoReturn, pReturnTypeDescr,
                                        pThis->getBridge()->getUno2Cpp() );
                // destroy temp uno return
                uno_destructData( pUnoReturn, pReturnTypeDescr, 0 );
            }
            // complex return ptr is set to return reg
            *(void **)pRegisterReturn = pCppReturn;
        }
        if (pReturnTypeDescr)
        {
            typelib_TypeClass eRet = (typelib_TypeClass)pReturnTypeDescr->eTypeClass;
            TYPELIB_DANGER_RELEASE( pReturnTypeDescr );
            return eRet;
        }
        else
            return typelib_TypeClass_VOID;
    }
}


//==================================================================================================
static typelib_TypeClass cpp_mediate(
    sal_Int32 nFunctionIndex,
        sal_Int32 nVtableOffset,
        void ** gpreg, void ** fpreg, void ** ovrflw,
    sal_Int64 * pRegisterReturn /* space for register return */ )
{
    OSL_ENSURE( sizeof(sal_Int32)==sizeof(void *), "### unexpected!" );
    
    // gpreg:  [ret *], this, [other gpr params]
    // fpreg:  [fpr params]
    // ovrflw: [gpr or fpr params (properly aligned)]

        void * pThis;
        if (nFunctionIndex & 0x80000000 )
    {
        nFunctionIndex &= 0x7fffffff;
        pThis = gpreg[1];
    }
    else
        {
        pThis = gpreg[0];
        }
    
        pThis = static_cast< char * >(pThis) - nVtableOffset;
        bridges::cpp_uno::shared::CppInterfaceProxy * pCppI
          = bridges::cpp_uno::shared::CppInterfaceProxy::castInterfaceToProxy(
            pThis);

    typelib_InterfaceTypeDescription * pTypeDescr = pCppI->getTypeDescr();
    
    OSL_ENSURE( nFunctionIndex < pTypeDescr->nMapFunctionIndexToMemberIndex, "### illegal vtable index!" );
    if (nFunctionIndex >= pTypeDescr->nMapFunctionIndexToMemberIndex)
    {
        throw RuntimeException(
            rtl::OUString::createFromAscii("illegal vtable index!"),
            (XInterface *)pThis );
    }
    
    // determine called method
    sal_Int32 nMemberPos = pTypeDescr->pMapFunctionIndexToMemberIndex[nFunctionIndex];
    OSL_ENSURE( nMemberPos < pTypeDescr->nAllMembers, "### illegal member index!" );

    TypeDescription aMemberDescr( pTypeDescr->ppAllMembers[nMemberPos] );
    
    typelib_TypeClass eRet;
    switch (aMemberDescr.get()->eTypeClass)
    {
    case typelib_TypeClass_INTERFACE_ATTRIBUTE:
    {
        if (pTypeDescr->pMapMemberIndexToFunctionIndex[nMemberPos] == nFunctionIndex)
        {
            // is GET method
            eRet = cpp2uno_call(
                pCppI, aMemberDescr.get(),
                ((typelib_InterfaceAttributeTypeDescription *)aMemberDescr.get())->pAttributeTypeRef,
                0, 0, // no params
                gpreg, fpreg, ovrflw, pRegisterReturn );
        }
        else
        {
            // is SET method
            typelib_MethodParameter aParam;
            aParam.pTypeRef =
                ((typelib_InterfaceAttributeTypeDescription *)aMemberDescr.get())->pAttributeTypeRef;
            aParam.bIn		= sal_True;
            aParam.bOut		= sal_False;
            
            eRet = cpp2uno_call(
                pCppI, aMemberDescr.get(),
                0, // indicates void return
                1, &aParam,
                gpreg, fpreg, ovrflw, pRegisterReturn );
        }
        break;
    }
    case typelib_TypeClass_INTERFACE_METHOD:
    {
        // is METHOD
        switch (nFunctionIndex)
        {
        case 1: // acquire()
            pCppI->acquireProxy(); // non virtual call!
            eRet = typelib_TypeClass_VOID;
            break;
        case 2: // release()
            pCppI->releaseProxy(); // non virtual call!
            eRet = typelib_TypeClass_VOID;
            break;
        case 0: // queryInterface() opt
        {
            typelib_TypeDescription * pTD = 0;
            TYPELIB_DANGER_GET( &pTD, reinterpret_cast< Type * >( gpreg[2] )->getTypeLibType() );
            if (pTD)
            {
                XInterface * pInterface = 0;
                (*pCppI->getBridge()->getCppEnv()->getRegisteredInterface)(
                    pCppI->getBridge()->getCppEnv(),
                    (void **)&pInterface, pCppI->getOid().pData, 
                    (typelib_InterfaceTypeDescription *)pTD );
            
                if (pInterface)
                {
                    ::uno_any_construct(
                        reinterpret_cast< uno_Any * >( gpreg[0] ),
                        &pInterface, pTD, cpp_acquire );
                    pInterface->release();
                    TYPELIB_DANGER_RELEASE( pTD );
                    *(void **)pRegisterReturn = gpreg[0];
                    eRet = typelib_TypeClass_ANY;
                    break;
                }
                TYPELIB_DANGER_RELEASE( pTD );
            }
        } // else perform queryInterface()
        default:
            eRet = cpp2uno_call(
                pCppI, aMemberDescr.get(),
                ((typelib_InterfaceMethodTypeDescription *)aMemberDescr.get())->pReturnTypeRef,
                ((typelib_InterfaceMethodTypeDescription *)aMemberDescr.get())->nParams,
                ((typelib_InterfaceMethodTypeDescription *)aMemberDescr.get())->pParams,
                gpreg, fpreg, ovrflw, pRegisterReturn );
        }
        break;
    }
    default:
    {
        throw RuntimeException(
            rtl::OUString::createFromAscii("no member description found!"),
            (XInterface *)pThis );
        // is here for dummy
        eRet = typelib_TypeClass_VOID;
    }
    }

    return eRet;
}

//==================================================================================================
/**
 * is called on incoming vtable calls
 * (called by asm snippets)
 */
static void cpp_vtable_call( int nFunctionIndex, int nVtableOffset, void** gpregptr, void** fpregptr, void** ovrflw)
{
        sal_Int32     gpreg[8];
        double        fpreg[8];

        memcpy( gpreg, gpregptr, 32);
        memcpy( fpreg, fpregptr, 64);

    volatile long nRegReturn[2];

        // fprintf(stderr,"in cpp_vtable_call nFunctionIndex is %x\n",nFunctionIndex);
        // fprintf(stderr,"in cpp_vtable_call nVtableOffset is %x\n",nVtableOffset);
        // fflush(stderr);

        sal_Bool bComplex = nFunctionIndex & 0x80000000 ? sal_True : sal_False;

    typelib_TypeClass aType = 
             cpp_mediate( nFunctionIndex, nVtableOffset, (void**)gpreg, (void**)fpreg, ovrflw, (sal_Int64*)nRegReturn );
    
    switch( aType )
    {

                // move return value into register space
                // (will be loaded by machine code snippet)

                case typelib_TypeClass_BOOLEAN:
                case typelib_TypeClass_BYTE:
                  __asm__( "lbz 3,%0\n\t" : :
               "m"(nRegReturn[0]) );
                  break;
 
                case typelib_TypeClass_CHAR:
                case typelib_TypeClass_SHORT:
                case typelib_TypeClass_UNSIGNED_SHORT:
                  __asm__( "lhz 3,%0\n\t" : :
               "m"(nRegReturn[0]) );
                  break;

        case typelib_TypeClass_FLOAT:
                  __asm__( "lfs 1,%0\n\t" : :
                           "m" (*((float*)nRegReturn)) );
          break;

        case typelib_TypeClass_DOUBLE:
          __asm__( "lfd 1,%0\n\t" : :
                           "m" (*((double*)nRegReturn)) );
          break;

        case typelib_TypeClass_HYPER:
        case typelib_TypeClass_UNSIGNED_HYPER:
          __asm__( "lwz 4,%0\n\t" : :
                           "m"(nRegReturn[1]) );  // fall through

        default:
          __asm__( "lwz 3,%0\n\t" : :
                           "m"(nRegReturn[0]) );
          break;
    }
}


int const codeSnippetSize = 108;

unsigned char *  codeSnippet( unsigned char * code, sal_Int32 functionIndex, sal_Int32 vtableOffset, 
                              bool simpleRetType)
{

  // fprintf(stderr,"in codeSnippet functionIndex is %x\n", functionIndex);
  // fprintf(stderr,"in codeSnippet vtableOffset is %x\n", vtableOffset);
  // fflush(stderr);

    if (! simpleRetType )
        functionIndex |= 0x80000000;

    unsigned long * p = (unsigned long *) code;
    
    // OSL_ASSERT( sizeof (long) == 4 );
    OSL_ASSERT((((unsigned long)code) & 0x3) == 0 );  //aligned to 4 otherwise a mistake

    /* generate this code */
    // # so first save gpr 3 to gpr 10 (aligned to 4)
    //	stw	r3,-2048(r1)
    //	stw	r4,-2044(r1)
    //	stw	r5,-2040(r1)
    //	stw	r6,-2036(r1)
    //	stw	r7,-2032(r1)
    //	stw	r8,-2028(r1)
    //	stw	r9,-2024(r1)
    //	stw	r10,-2020(r1)


    // # next save fpr 1 to fpr 8 (aligned to 8)
    //	stfd	f1,-2016(r1)
    //	stfd	f2,-2008(r1)
    //	stfd	f3,-2000(r1)
    //	stfd	f4,-1992(r1)
    //	stfd	f5,-1984(r1)
    //	stfd	f6,-1976(r1)
    //	stfd	f7,-1968(r1)
    //	stfd	f8,-1960(r1)

    // # now here is where cpp_vtable_call must go
    //	lis	r3,-8531
    //	ori	r3,r3,48879
    //	mtctr	r3

    // # now load up the functionIndex
    //	lis	r3,-8531
    //	ori	r3,r3,48879

    // # now load up the vtableOffset 
    //	lis	r4,-8531
    //	ori	r4,r4,48879

    // #now load up the pointer to the saved gpr registers
    //	addi	r5,r1,-2048

    // #now load up the pointer to the saved fpr registers
    //	addi	r6,r1,-2016

    // #now load up the pointer to the overflow call stack
    //	addi	r7,r1,8
    //	bctr

      * p++ = 0x9061f800;
      * p++ = 0x9081f804;
      * p++ = 0x90a1f808;
      * p++ = 0x90c1f80c;
      * p++ = 0x90e1f810;
      * p++ = 0x9101f814;
      * p++ = 0x9121f818;
      * p++ = 0x9141f81c;
      * p++ = 0xd821f820;
      * p++ = 0xd841f828;
      * p++ = 0xd861f830;
      * p++ = 0xd881f838;
      * p++ = 0xd8a1f840;
      * p++ = 0xd8c1f848;
      * p++ = 0xd8e1f850;
      * p++ = 0xd901f858;
      * p++ = 0x3c600000 | (((unsigned long)cpp_vtable_call) >> 16);
      * p++ = 0x60630000 | (((unsigned long)cpp_vtable_call) & 0x0000FFFF);
      * p++ = 0x7c6903a6;
      * p++ = 0x3c600000 | (((unsigned long)functionIndex) >> 16);
      * p++ = 0x60630000 | (((unsigned long)functionIndex) & 0x0000FFFF);
      * p++ = 0x3c800000 | (((unsigned long)vtableOffset) >> 16);
      * p++ = 0x60840000 | (((unsigned long)vtableOffset) & 0x0000FFFF);
      * p++ = 0x38a1f800;
      * p++ = 0x38c1f820;
      * p++ = 0x38e10008;
      * p++ = 0x4e800420;
      return (code + codeSnippetSize);

}


}


#define MIN_LINE_SIZE 32

void bridges::cpp_uno::shared::VtableFactory::flushCode(unsigned char const * bptr, unsigned char const * eptr)
{
  unsigned char * eaddr = (unsigned char *) eptr + MIN_LINE_SIZE + 1;
  for (  unsigned char * addr  = (unsigned char *) bptr; addr < eaddr; addr += MIN_LINE_SIZE) {
      __asm__ volatile ( "dcbf 0,%0;" "icbi 0,%0;" : : "r"(addr) : "memory");
  }
  __asm__ volatile ( "sync;" "isync;" : : : "memory");
}



void ** bridges::cpp_uno::shared::VtableFactory::mapBlockToVtable(char * block)
{
    return reinterpret_cast< void ** >(block) + 2;
}


char * bridges::cpp_uno::shared::VtableFactory::createBlock(
    sal_Int32 slotCount, void *** slots)
{
    char * block = new char[
        (slotCount + 2) * sizeof (void *) + slotCount * codeSnippetSize];
    *slots = mapBlockToVtable(block);
    (*slots)[-2] = 0;
    (*slots)[-1] = 0;
    return block;
}

unsigned char * bridges::cpp_uno::shared::VtableFactory::addLocalFunctions(
    void ** slots, unsigned char * code,
    typelib_InterfaceTypeDescription const * type, sal_Int32 functionOffset,
    sal_Int32 functionCount, sal_Int32 vtableOffset)
{

  // fprintf(stderr, "in addLocalFunctions functionOffset is %x\n",functionOffset);
  // fprintf(stderr, "in addLocalFunctions vtableOffset is %x\n",vtableOffset);
  // fflush(stderr);

    for (sal_Int32 i = 0; i < type->nMembers; ++i) {
        typelib_TypeDescription * member = 0;
        TYPELIB_DANGER_GET(&member, type->ppMembers[i]);
        OSL_ASSERT(member != 0);
        switch (member->eTypeClass) {
        case typelib_TypeClass_INTERFACE_ATTRIBUTE:
            // Getter:
            *slots++ = code;
            code = codeSnippet(
                code, functionOffset++, vtableOffset,
                bridges::cpp_uno::shared::isSimpleType(
                    reinterpret_cast<
                    typelib_InterfaceAttributeTypeDescription * >(
                        member)->pAttributeTypeRef));

            // Setter:
            if (!reinterpret_cast<
                typelib_InterfaceAttributeTypeDescription * >(
                    member)->bReadOnly)
            {
                *slots++ = code;
                code = codeSnippet(code, functionOffset++, vtableOffset, true);
            }
            break;

        case typelib_TypeClass_INTERFACE_METHOD:
            *slots++ = code;
            code = codeSnippet(
                code, functionOffset++, vtableOffset,
                bridges::cpp_uno::shared::isSimpleType(
                    reinterpret_cast<
                    typelib_InterfaceMethodTypeDescription * >(
                        member)->pReturnTypeRef));
            break;

        default:
            OSL_ASSERT(false);
            break;
        }
        TYPELIB_DANGER_RELEASE(member);
    }
    return code;
}

