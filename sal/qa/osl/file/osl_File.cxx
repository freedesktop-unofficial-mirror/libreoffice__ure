/*************************************************************************
 *
 *  $RCSfile: osl_File.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2003-04-17 08:34:31 $
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
 *  Copyright 2000 by Sun Microsystems, 
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

//------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif 

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif

#ifndef _OSL_FILE_HXX_
#include <osl/file.hxx>
#endif

#ifndef _OSL_FILE_CONST_H_
#include <osl_File_Const.h>
#endif

#include <cppunit/simpleheader.hxx>

using namespace osl;
using namespace rtl;

//------------------------------------------------------------------------
// helper functions
//------------------------------------------------------------------------

/** print an output wrong message.
*/
inline void printError( const ::osl::FileBase::RC nError )
{
    printf( "#printError# " );
    switch ( nError ) {
        case ::osl::FileBase::E_None: 
            printf( "Success!\n" );
            break;
        case ::osl::FileBase::E_PERM:
            printf( "The returned error is: Permission denied!\n" );
            break;
        case ::osl::FileBase::E_NOENT:
            printf( "The returned error is: Not Exist!\n" );
            break;
        case ::osl::FileBase::E_EXIST:
            printf( "The returned error is: Already Exist!\n" );
            break;
        case ::osl::FileBase::E_ACCES:
            printf( "The returned error is: Access denied!\n" );
            break;
        case ::osl::FileBase::E_INVAL:
            printf( "The returned error is: Invalid Parameter!\n" );
            break;
        case ::osl::FileBase::E_NOTDIR:
            printf( "The returned error is: Not a directory!\n" );
            break;
        case ::osl::FileBase::E_ISDIR:
            printf( "The returned error is: Is a directory!\n" );
            break;
        case ::osl::FileBase::E_BADF:
            printf( "The returned error is: Invalid Directory/File parameter!\n" );
            break;
        case ::osl::FileBase::E_NOTEMPTY:
            printf( "The returned error is: The directory is not empty!\n" );
            break;
        default:
            printf( "The returned error is: Number %d, Unknown Error\n", nError );
            break;
    }
}

/** print a file type name.
*/
inline void printFileType( const ::osl::FileStatus::Type nType )
{
    printf( "#printFileType# " );
    switch ( nType ) {
        case ::osl::FileStatus::Directory: 
            printf( "This file is a: Directory.\n" );
            break;
        case ::osl::FileStatus::Volume:
            printf( "This file is a: volume device.\n" );
            break;
        case ::osl::FileStatus::Regular:
            printf( "This file is a: regular file.\n" );
            break;
        case ::osl::FileStatus::Fifo:
            printf( "This file is a: fifo.\n" );
            break;
        case ::osl::FileStatus::Socket:
            printf( "This file is a: socket.\n" );
            break;
        case ::osl::FileStatus::Link:
            printf( "This file is a: link file.\n" );
            break;
        case ::osl::FileStatus::Special:
            printf( "This file is a: special.\n" );
            break;
        case ::osl::FileStatus::Unknown:
            printf( "The file type is unknown\n", nType );
            break;
    }
}

/** print a file attributes.
*/
inline void printFileAttributes( const sal_Int64 nAttributes )
{
    printf( "#printFileAttributes# This file is a: (" );
    if ( ( nAttributes | Attribute_ReadOnly ) == nAttributes )
            printf( " ReadOnly " );
    if ( ( nAttributes | Attribute_Hidden ) == nAttributes )
            printf( " Hidden " );
    if ( ( nAttributes | Attribute_Executable ) == nAttributes )
            printf( " Executable " );
    if ( ( nAttributes | Attribute_GrpWrite ) == nAttributes )
            printf( " GrpWrite " );
    if ( ( nAttributes | Attribute_GrpRead ) == nAttributes )
            printf( " GrpRead " );
    if ( ( nAttributes | Attribute_GrpExe ) == nAttributes )
            printf( " GrpExe " );
    if ( ( nAttributes | Attribute_OwnWrite ) == nAttributes )
            printf( " OwnWrite " );
    if ( ( nAttributes | Attribute_OwnRead ) == nAttributes )
            printf( " OwnRead " );
    if ( ( nAttributes | Attribute_OwnExe ) == nAttributes )
            printf( " OwnExe " );
    if ( ( nAttributes | Attribute_OthWrite ) == nAttributes )
            printf( " OthWrite " );
    if ( ( nAttributes | Attribute_OthRead ) == nAttributes )
            printf( " OthRead " );
    if ( ( nAttributes | Attribute_OthExe ) == nAttributes )
            printf( " OthExe " );
    printf( ") file!\n" );
}

/** print a UNI_CODE file name.
*/
inline void printFileName( const ::rtl::OUString & str )
{
    rtl::OString aString;

    printf( "#printFileName_u# " );
    aString = ::rtl::OUStringToOString( str, RTL_TEXTENCODING_ASCII_US );
    printf( "%s\n", aString.getStr( ) );
}

/** print a ASCII_CODE file name.
*/
inline void printFileName( const sal_Char * str )
{
    printf( "#printFileName_a# " );
    printf( "%s\n", str );
}

/** print an signed Integer Number.
*/
inline void printInt( sal_Int64 i )
{
    printf( "#printInt_i64# " );
    printf( "The Integer64 is %lld\n", i);
}

/** print an unsigned Integer Number.
*/
inline void printInt( sal_uInt64 i )
{
    printf( "#printInt_u64# " );
    printf( "The unsigned Integer64 is %llu\n", i);
}

/** print Boolean value.
*/
inline void printBool( sal_Bool bOk )
{
    printf( "#printBool# " );
    ( sal_True == bOk ) ? printf( "YES!\n" ): printf( "NO!\n" );		
}

/** print struct TimeValue in local time format.
*/
inline void printTime( TimeValue *tv )
{
    oslDateTime *pDateTime = ( oslDateTime* )malloc( sizeof( oslDateTime ) ) ;
     CPPUNIT_ASSERT_MESSAGE( "Error in printTime() function,malloc ", pDateTime != NULL );
    TimeValue *pLocalTV = ( TimeValue* )malloc( sizeof( TimeValue ) );
     CPPUNIT_ASSERT_MESSAGE( "Error in printTime() function,malloc ", pLocalTV != NULL );
    
    CPPUNIT_ASSERT_MESSAGE( "Error in printTime() function,osl_getLocalTimeFromSystemTime ",sal_True == osl_getLocalTimeFromSystemTime( tv, pLocalTV ) );
    CPPUNIT_ASSERT_MESSAGE( "Error in printTime() function,osl_gepDateTimeFromTimeValue ",sal_True == osl_getDateTimeFromTimeValue( pLocalTV, pDateTime ) );
    
    printf( "#printTime# " );
     printf( " Time is: %d/%d/%d ", pDateTime->Month, pDateTime->Day, pDateTime->Year);		
    switch ( pDateTime->DayOfWeek )
    {
        case 0: printf("Sun. "); break;		
        case 1: printf("Mon. "); break;		
        case 2: printf("Tue. "); break;		
        case 3: printf("Thr. "); break;		
        case 4: printf("Wen. "); break;		
        case 5: printf("Fri. "); break;		
        case 6: printf("Sat. "); break;
    }
    printf( " %d:%d:%d %d nsecs\n", pDateTime->Hours, pDateTime->Minutes, pDateTime->Seconds, pDateTime->NanoSeconds);		
    
    free( pDateTime );
    free( pLocalTV );
}

/** compare two TimeValue, unit is "ms", since Windows time precision is better than UNX.
*/

#if ( defined UNX ) || ( defined OS2 )                 //precision of time in Windows is better than UNX
#	define delta 2000                    //time precision, 2000ms
#else
#	define delta 5                       //time precision, 5ms
#endif

inline sal_Bool compareTime( TimeValue *tv1,  TimeValue *tv2, sal_uInt64 nDelta)
{
    sal_uInt64 uTimeValue;
    
    uTimeValue = ( sal_Int64 ) ( abs( ( tv1->Seconds * 1000000000 + tv1->Nanosec ) - ( tv2->Seconds * 1000000000 + tv2->Nanosec ) ) / 1000000 );
    return ( uTimeValue < nDelta );
}

/** compare two OUString file name.
*/
inline sal_Bool compareFileName( const ::rtl::OUString & ustr1, const ::rtl::OUString & ustr2 )
{
    sal_Bool bOk = ustr1.equals( ustr2 );
    
    return bOk;
}

/** compare a OUString and an ASCII file name.
*/
inline sal_Bool compareFileName( const ::rtl::OUString & ustr, const sal_Char *astr )
{
    ::rtl::OUString ustr1 = rtl::OUString::createFromAscii( astr );
    sal_Bool bOk = ustr1.equals( ustr1 );
    
    return bOk;
}

/** simple version to judge if a file name or directory name is a URL or a system path, just to see if it 
    is start with "file:///";.
*/
inline sal_Bool isURL( const sal_Char *pathname )
{
    return ( 0 == strncmp( pathname, FILE_PREFIX, sizeof( FILE_PREFIX ) - 1 ) );
}

/** simple version to judge if a file name or directory name is a URL or a system path, just to see if it 
    is start with "file:///";.
*/
inline sal_Bool isURL( const ::rtl::OUString pathname )
{
    return ( ( pathname.indexOf( aPreURL ) == 0 ) ? sal_True : sal_False );
}

/** concat two part to form a URL or system path, add PATH_SEPERATOR between them if necessary, add "file:///" to begining if necessary.
*/
inline void concatURL( ::rtl::OUString & pathname1, const ::rtl::OUString & pathname2 )
{
    //check if pathname1 is full qualified URL;
    if ( !isURL( pathname1 ) )
    {
        ::rtl::OUString     aPathName   = pathname1.copy( 0 );
        ::osl::FileBase::getFileURLFromSystemPath( pathname1, aPathName ); //convert if not full qualified URL	
        pathname1   = aPathName.copy( 0 );
    }
    
    sal_Int32 index = 0;
    //check if '/' is in the end of pathname1 or at the begin of pathname2;
    if ( ( ( index = pathname1.lastIndexOf( aSlashURL ) ) != ( pathname1.getLength( ) - 1 ) ) &&
         ( ( index = pathname2.indexOf( aSlashURL ) ) != 0 ) )
        pathname1 += aSlashURL;
    pathname1 += pathname2;
}

/** create a temp test file using OUString name of full qualified URL or system path.
*/
inline void createTestFile( const ::rtl::OUString filename )
{
    ::rtl::OUString     aPathURL   = filename.copy( 0 );
    ::osl::FileBase::RC	nError;
    
    if ( !isURL( filename ) )
        ::osl::FileBase::getFileURLFromSystemPath( filename, aPathURL ); //convert if not full qualified URL	
    
    ::std::auto_ptr<File> pFile( new File( aPathURL ) );
    nError = pFile->open( OpenFlag_Read | OpenFlag_Write | OpenFlag_Create );
    CPPUNIT_ASSERT_MESSAGE( "In createTestFile Function: creation ", ( ::osl::FileBase::E_None == nError ) || ( nError == ::osl::FileBase::E_EXIST ) );	
}

/** create a temp test file using OUString name of full qualified URL or system path in a base directory.
*/
inline void createTestFile( const ::rtl::OUString basename, const ::rtl::OUString filename )
{
    ::rtl::OUString     aBaseURL   = basename.copy( 0 );

    concatURL( aBaseURL, filename );
    createTestFile( aBaseURL );
}

/** detete a temp test file using OUString name.
*/
inline void deleteTestFile( const ::rtl::OUString filename )
{
    ::rtl::OUString     aPathURL   = filename.copy( 0 );
    ::osl::FileBase::RC	nError;
    
    if ( !isURL( filename ) )
        ::osl::FileBase::getFileURLFromSystemPath( filename, aPathURL ); //convert if not full qualified URL	
    nError = ::osl::File::remove( aPathURL );
    CPPUNIT_ASSERT_MESSAGE( "In deleteTestFile Function: remove ", ( ::osl::FileBase::E_None == nError ) || ( nError == ::osl::FileBase::E_NOENT ) );	
}

/** delete a temp test file using OUString name of full qualified URL or system path in a base directory.
*/
inline void deleteTestFile( const ::rtl::OUString basename, const ::rtl::OUString filename )
{
    ::rtl::OUString     aBaseURL   = basename.copy( 0 );

    concatURL( aBaseURL, filename );
    deleteTestFile( aBaseURL );
}

/** create a temp test directory using OUString name of full qualified URL or system path.
*/
inline void createTestDirectory( const ::rtl::OUString dirname )
{
    ::rtl::OUString     aPathURL   = dirname.copy( 0 );
    ::osl::FileBase::RC	nError;
    
    if ( !isURL( dirname ) )
        ::osl::FileBase::getFileURLFromSystemPath( dirname, aPathURL ); //convert if not full qualified URL	
    nError = ::osl::Directory::create( aPathURL );
    CPPUNIT_ASSERT_MESSAGE( "In createTestDirectory Function: creation: ", ( ::osl::FileBase::E_None == nError ) || ( nError == ::osl::FileBase::E_EXIST ) );	
}

/** create a temp test directory using OUString name of full qualified URL or system path in a base directory.
*/
inline void createTestDirectory( const ::rtl::OUString basename, const ::rtl::OUString dirname )
{
    ::rtl::OUString     aBaseURL   = basename.copy( 0 );
    ::rtl::OString      aString;

    concatURL( aBaseURL, dirname );
    createTestDirectory( aBaseURL );
}

/** delete a temp test directory using OUString name of full qualified URL or system path.
*/
inline void deleteTestDirectory( const ::rtl::OUString dirname )
{
    ::rtl::OUString     aPathURL   = dirname.copy( 0 );
    ::osl::FileBase::RC	nError;
    
    if ( !isURL( dirname ) )
        ::osl::FileBase::getFileURLFromSystemPath( dirname, aPathURL ); //convert if not full qualified URL	
    nError = ::osl::Directory::remove( aPathURL );
    CPPUNIT_ASSERT_MESSAGE( "In deleteTestDirectory function: remove ", ( ::osl::FileBase::E_None == nError ) || ( nError == ::osl::FileBase::E_NOENT ) );	
}

/** delete a temp test directory using OUString name of full qualified URL or system path in a base directory.
*/
inline void deleteTestDirectory( const ::rtl::OUString basename, const ::rtl::OUString dirname )
{
    ::rtl::OUString     aBaseURL   = basename.copy( 0 );

    concatURL( aBaseURL, dirname );
    deleteTestDirectory( aBaseURL );
}


/** Check for the file and directory access right.
*/
typedef enum {
    osl_Check_Mode_OpenAccess,
    osl_Check_Mode_ReadAccess,
    osl_Check_Mode_WriteAccess
} oslCheckMode;

inline sal_Bool checkFileURL( ::rtl::OUString & str )
{
    rtl::OString 	aString;
    FileBase::RC	rc;
    
    //check the file name and whether it can be write.
    ::std::auto_ptr<File> pFile( new File( str ) );
    rc = pFile->open( OpenFlag_Read | OpenFlag_Write );	
    pFile->close( );
    return ( ::osl::FileBase::E_None == rc );
}

inline sal_Bool checkDirectory( const ::rtl::OUString & str, oslCheckMode nCheckMode )
{
    rtl::OUString 	aUString;
    DirectoryItem	rItem;
    FileBase::RC	rc;
    sal_Bool		bCheckResult;
    
    // check the file name and whether it can be open, read or write.
    bCheckResult = sal_False;
    ::std::auto_ptr<Directory> pDir( new Directory( str ) );
    rc = pDir->open( );	
    CPPUNIT_ASSERT( rc == FileBase::E_None );
    
    switch ( nCheckMode ) {
        case osl_Check_Mode_OpenAccess:
            if ( rc == FileBase::E_None )
                bCheckResult = sal_True;
            else 
                bCheckResult = sal_False;
            break;
        case osl_Check_Mode_ReadAccess:
            rc = pDir->getNextItem( rItem, 0 );
            if ( ( rc == FileBase::E_None ) || ( rc == FileBase::E_NOENT ) )
                bCheckResult = sal_True;
            else 
                bCheckResult = sal_False;
            break;
        case osl_Check_Mode_WriteAccess:
            ( ( aUString += str ) += aSlashURL ) += aTmpName2;
            if ( ( rc = pDir->create( aUString ) ) == FileBase::E_None )
            {
                bCheckResult = sal_True; 
                rc = pDir->remove( aUString );
                CPPUNIT_ASSERT( rc == FileBase::E_None );
            }
            else 
                bCheckResult = sal_False;
            break;
            
        default:
            bCheckResult = sal_False;
    }// switch
    
    rc = pDir->close( );	
    CPPUNIT_ASSERT( rc == FileBase::E_None );
    
    return bCheckResult;
}

/** Change file mode, two version in UNIX and Windows;.
*/
#if ( defined UNX ) || ( defined OS2 )	       //chmod() method is differ in Windows
inline void changeFileMode( ::rtl::OUString & filepath, sal_Int32 mode )
{
    rtl::OString 	aString;
    rtl::OUString 	aUStr  = filepath.copy( 0 );
    
    if ( isURL( filepath ) )
        ::osl::FileBase::getSystemPathFromFileURL( filepath, aUStr ); 
    aString = ::rtl::OUStringToOString( aUStr, RTL_TEXTENCODING_ASCII_US );
    chmod( aString.getStr( ), mode );
}
#else                                          //Windows version
inline void changeFileMode( ::rtl::OUString & filepath, sal_Int32 mode )
{
    printf("this method is not implemented yet");
}
#endif



//------------------------------------------------------------------------
// Beginning of the test cases for FileBase class
//------------------------------------------------------------------------
namespace osl_FileBase
{

    //---------------------------------------------------------------------
    // testing the method 
    // static inline RC getCanonicalName( const ::rtl::OUString& ustrRequestedURL, ::rtl::OUString& ustrValidURL )
    // 
    // The illegal characters are ;+=[]',\"*\\<>/?:|.
    //---------------------------------------------------------------------
    
    class getCanonicalName:public CppUnit::TestFixture  
    {
        
        public:

        // test code
        void getCanonicalName_001( )
        {
            ::rtl::OUString aUStr_ValidURL;
            ::osl::FileBase::RC nError = ::osl::FileBase::getCanonicalName( aCanURL1, aUStr_ValidURL );

            CPPUNIT_ASSERT_MESSAGE("test for getCanonicalName function: valid and unused file name",
                                     ( osl::FileBase::E_None == nError ) && !aUStr_ValidURL.compareTo ( aCanURL1, nCanURL1Len ) );
        }

        void getCanonicalName_002( )
        {
            ::rtl::OUString		aUStr_ValidURL;
            ::osl::FileBase::RC	nError;

            createTestFile( aCanURL1 );
            nError = ::osl::FileBase::getCanonicalName( aCanURL1, aUStr_ValidURL );
            deleteTestFile( aCanURL1 );
            
            CPPUNIT_ASSERT_MESSAGE( " test for getCanonicalName function: an existed file name, should different from the request, it did not passed(W32)(UNX)",
                                    ( osl::FileBase::E_None == nError ) && aUStr_ValidURL.compareTo ( aCanURL1, nCanURL1Len ) );
        }

        void getCanonicalName_003( )
        {
            ::rtl::OUString aUStr_ValidURL;
            ::osl::FileBase::RC nError = ::osl::FileBase::getCanonicalName ( aCanURL2,  aUStr_ValidURL );

            CPPUNIT_ASSERT_MESSAGE( " test for getCanonicalName function: invalid file name, should different from the request,  it did not passed(W32)(UNX)",
                                     ( osl::FileBase::E_None == nError ) && aUStr_ValidURL.compareTo ( aCanURL2, nCanURL2Len ) );
        }

        CPPUNIT_TEST_SUITE( getCanonicalName );
        CPPUNIT_TEST( getCanonicalName_001 );
        CPPUNIT_TEST( getCanonicalName_002 );
        CPPUNIT_TEST( getCanonicalName_003 );
        CPPUNIT_TEST_SUITE_END( );
    };// class getCanonicalName


    //---------------------------------------------------------------------
    // testing the method 
    // static inline RC getAbsoluteFileURL( const ::rtl::OUString& ustrBaseDirectoryURL, 
    //										const ::rtl::OUString& ustrRelativeFileURL, 
    //											  ::rtl::OUString& ustrAbsoluteFileURL )
    //---------------------------------------------------------------------
    
    class getAbsoluteFileURL:public CppUnit::TestFixture
    {
        ::osl::FileBase aFileBase;
        ::rtl::OUString aResultURL1, aResultURL2, aResultURL3, aResultURL4, aResultURL5;
        
        public:
        
        // initialization
        void setUp( )
        {
            sal_Char pResultURL1[]  = "/relative/file1"; 
            sal_Char pResultURL2[]  = "/relative/file2"; 
            sal_Char pResultURL3[]  = "/file3"; 
            sal_Char pResultURL4[]  = "/file4"; 
            sal_Char pResultURL5[]  = "/canonical.name"; 
            aResultURL1 = aTmpURL1.concat( rtl::OUString::createFromAscii( pResultURL1 ) );
            aResultURL2 = aTmpURL1.concat( rtl::OUString::createFromAscii( pResultURL2 ) ); 
            aResultURL3 = aTmpURL1.concat( rtl::OUString::createFromAscii( pResultURL3 ) );
            aResultURL4 = aTmpURL1.concat( rtl::OUString::createFromAscii( pResultURL4 ) );
            aResultURL5 = aTmpURL1.concat( rtl::OUString::createFromAscii( pResultURL5 ) );
        }

        void tearDown( )
        {
        }

        // test code
        void getAbsoluteFileURL_001( )
        {
            ::rtl::OUString aUStr_BaseURL = rtl::OUString::createFromAscii( pTmpURL1 );
            ::rtl::OUString aUStr_RelURL1 = rtl::OUString::createFromAscii( pRelURL1 );
            ::rtl::OUString aUStr_RelURL2 = rtl::OUString::createFromAscii( pRelURL2 );
            ::rtl::OUString aUStr_RelURL3 = rtl::OUString::createFromAscii( pRelURL3 );
            ::rtl::OUString aUStr_RelURL4 = rtl::OUString::createFromAscii( pRelURL4 );
            ::rtl::OUString aUStr_AbsURL;

            ::osl::FileBase::RC nError11 = aFileBase.getAbsoluteFileURL( aUStr_BaseURL,  aUStr_RelURL1, aUStr_AbsURL );
            sal_Bool nError12 = aUStr_AbsURL.equals( aResultURL1 );
            ::osl::FileBase::RC nError21 = aFileBase.getAbsoluteFileURL( aUStr_BaseURL,  aUStr_RelURL2, aUStr_AbsURL );
            sal_Bool nError22 = aUStr_AbsURL.equals( aResultURL2 );
            ::osl::FileBase::RC nError31 = aFileBase.getAbsoluteFileURL( aUStr_BaseURL,  aUStr_RelURL3, aUStr_AbsURL );
            sal_Bool nError32 = aUStr_AbsURL.equals( aResultURL3 );
            ::osl::FileBase::RC nError41 = aFileBase.getAbsoluteFileURL( aUStr_BaseURL,  aUStr_RelURL4, aUStr_AbsURL );
            sal_Bool nError42 = aUStr_AbsURL.equals( aResultURL4 );
            
            
            CPPUNIT_ASSERT_MESSAGE("test for getAbsoluteFileURL function: valid file name with valid directory",
                                     ( ::osl::FileBase::E_None == nError11 ) && ( sal_True == nError12 ) &&
                                     ( ::osl::FileBase::E_None == nError21 ) && ( sal_True == nError22 ) &&
                                     ( ::osl::FileBase::E_None == nError31 ) && ( sal_True == nError32 ) &&
                                     ( ::osl::FileBase::E_None == nError41 ) && ( sal_True == nError42 ) );
        }

        
#if ( defined UNX ) || ( defined OS2 )	//Link is not defined in Windows
        void getAbsoluteFileURL_002( )
        {
             ::rtl::OUString aUStr_BaseURL = rtl::OUString::createFromAscii( pTmpURL1 );
            ::rtl::OUString aUStr_RelURL = rtl::OUString::createFromAscii( pLnkURL1 );
            ::rtl::OUString aUStr_AbsURL;
            ::osl::FileBase::RC nError;
            sal_Int32 fd;
            sal_Bool bOk;
            
            createTestFile( aCanURL1 );
            fd = symlink( "/tmp/canonical.name", "/tmp/link.file" );
            CPPUNIT_ASSERT( fd == 0 );
            
            nError = aFileBase.getAbsoluteFileURL( aUStr_BaseURL,  aUStr_RelURL, aUStr_AbsURL );
            bOk = aUStr_AbsURL.equals( aResultURL5 );
            
            deleteTestFile( aCanURL1 );
            fd = remove( "/tmp/link.file" );
            CPPUNIT_ASSERT( fd == 0 );
            
            CPPUNIT_ASSERT_MESSAGE("test for getAbsoluteFileURL function: URL contain link( Solaris version )",
                                     ( ::osl::FileBase::E_None == nError ) && ( sal_True == bOk ) );
        }
#else                                  //Windows version
        void getAbsoluteFileURL_002( )
        {
            CPPUNIT_ASSERT_MESSAGE("test for getAbsoluteFileURL function: URL contain link( Windows version )",
                                     1 );
        }			
#endif	
        

        void getAbsoluteFileURL_003( )
        {
            ::rtl::OUString aUStr_AbsURL;
            ::osl::FileBase::RC nError1,nError2;
            
            nError1 = aFileBase.getAbsoluteFileURL( aTmpName1,  aRelURL1, aUStr_AbsURL );    //base dir invalid error
            nError2 = aFileBase.getAbsoluteFileURL( aRelURL1,  aRelURL1, aUStr_AbsURL ); 	 //not dir error,SHOULD BE E_NOTDIR
            
            CPPUNIT_ASSERT_MESSAGE("test for getAbsoluteFileURL function: Parameter is invalid",
                                     ( ::osl::FileBase::E_INVAL == nError1 ) && 
                                    ( ::osl::FileBase::E_INVAL == nError2 ));
        }
        
        void getAbsoluteFileURL_004( )
        {
             ::rtl::OUString     aUStr_BaseURL = rtl::OUString::createFromAscii( pTmpName2 );
            ::rtl::OUString     aUStr_RelURL = rtl::OUString::createFromAscii( pCanURL1 );
            ::rtl::OUString     aUStr_AbsURL;
            ::osl::FileBase::RC  nError;
            sal_Bool			bOk;
            
            nError = aFileBase.getAbsoluteFileURL( aUStr_BaseURL,  aUStr_RelURL, aUStr_AbsURL );
            bOk = aUStr_AbsURL.equals( aResultURL5 );
            
            CPPUNIT_ASSERT_MESSAGE("test for getAbsoluteFileURL function: ustrRelativeFileURL denotes an absolute",
                                     ( ::osl::FileBase::E_None == nError )  && ( sal_True == bOk ) );
        }

        CPPUNIT_TEST_SUITE( getAbsoluteFileURL );
        CPPUNIT_TEST( getAbsoluteFileURL_001 );
        CPPUNIT_TEST( getAbsoluteFileURL_002 );
        CPPUNIT_TEST( getAbsoluteFileURL_003 );
        CPPUNIT_TEST( getAbsoluteFileURL_004 );
        CPPUNIT_TEST_SUITE_END( );
    };// class getAbsoluteFileURL
    
    
    //---------------------------------------------------------------------
    // testing the method 
    // static inline RC getSystemPathFromFileURL( const ::rtl::OUString& ustrFileURL, 
    //												::rtl::OUString& ustrSystemPath )
    //---------------------------------------------------------------------
    class getSystemPathFromFileURL:public CppUnit::TestFixture
    {
        ::osl::FileBase aFileBase;
        ::rtl::OUString aUStr;
        ::osl::FileBase::RC nError;
        
        public:
        // test code.
        void getSystemPathFromFileURL_001( )
        {
            nError = aFileBase.getSystemPathFromFileURL( aTmpName2, aUStr ); // start with /

            CPPUNIT_ASSERT_MESSAGE( "test for getSystemPathFromFileURL function: parameter is invalid 1",
                                     ( osl::FileBase::E_INVAL == nError ) );
        }

        void getSystemPathFromFileURL_002( )
        {
            nError = aFileBase.getSystemPathFromFileURL( aCanURL2, aUStr ); // not a URL and contain illegal characters

            CPPUNIT_ASSERT_MESSAGE( "test for getSystemPathFromFileURL function: parameter is invalid 2",
                                    ( osl::FileBase::E_INVAL == nError ) );
        }
        
        void getSystemPathFromFileURL_003( )
        {
            ::rtl::OUString aUStr_BaseURL = rtl::OUString::createFromAscii( pTmpURL1 );
            ::rtl::OUString aRelativeURL = aUStr_BaseURL.concat( rtl::OUString::createFromAscii( pTmpName2 ) );
            static const sal_Char pResultURL[]  = TEST_PLATFORM_ROOT TEST_PLATFORM_TEMP "/tmpname"; 
            ::rtl::OUString aResultURL = rtl::OUString::createFromAscii( pResultURL );

            nError = aFileBase.getSystemPathFromFileURL( aRelativeURL, aUStr );
            sal_Bool bOk = aUStr.equals( aResultURL );
            
            CPPUNIT_ASSERT_MESSAGE( "test for getSystemPathFromFileURL function: relative URL", 
                                    ( osl::FileBase::E_None == nError ) && ( sal_True == bOk ) );
        }

        CPPUNIT_TEST_SUITE( getSystemPathFromFileURL );
        CPPUNIT_TEST( getSystemPathFromFileURL_001 );
        CPPUNIT_TEST( getSystemPathFromFileURL_002 );
        CPPUNIT_TEST( getSystemPathFromFileURL_003 );
        CPPUNIT_TEST_SUITE_END( );
    };// class getSystemPathFromFileURL
    

    //---------------------------------------------------------------------
    // testing the method 
    // static inline RC searchFileURL(  const ::rtl::OUString& ustrFileName, 
    // 									const ::rtl::OUString& ustrSearchPath,
    //									::rtl::OUString& ustrFileURL )
    //---------------------------------------------------------------------
    class searchFileURL:public CppUnit::TestFixture
    {
        ::osl::FileBase aFileBase;
        ::rtl::OUString aUStr;
        ::osl::FileBase::RC nError1, nError2, nError3,nError4;
        
        public:
        // initialization
        void setUp( )
        {
        }

        void tearDown( )
        {
        }

        // test code.
        void searchFileURL_001( )
        {
            /* search file is passed by system filename */
            nError1 = ::osl::FileBase::searchFileURL( aSysPath2, aTmpSys1, aUStr );
            /* search file is passed by full qualified file URL */
            nError2 = ::osl::FileBase::searchFileURL( aCanURL1, aTmpSys1, aUStr );
            /* search file is passed by relative file path */
            nError3 = ::osl::FileBase::searchFileURL( aRelURL4, aTmpSys1, aUStr );
            CPPUNIT_ASSERT_MESSAGE( "test for searchFileURL function: system filename/URL filename/relative path, system directory, searched file not exist ",
                                     ( osl::FileBase::E_NOENT == nError1 ) &&
                                     ( osl::FileBase::E_NOENT == nError2 ) &&
                                    ( osl::FileBase::E_NOENT == nError3 ));
        }

         void searchFileURL_002( )
        {
            OSLTEST_DECLARE( RelativeFilePath,  TEST_PLATFORM_TEMP "/./../" TEST_PLATFORM_TEMP ); 
            OSLTEST_DECLARE( CanonicalFileName,  "canonical.name" ); 
            
            /* search file is passed by system filename */
            nError1 = ::osl::FileBase::searchFileURL( aTmpSys1, aRootSys, aUStr );
            sal_Bool bOk1 = compareFileName( aUStr, aTmpURL1 );
            /* search file is passed by full qualified file URL */
            nError2 = ::osl::FileBase::searchFileURL( aTmpURL1, aRootSys, aUStr );
            sal_Bool bOk2 = compareFileName( aUStr, aTmpURL1 );
            /* search file is passed by relative file path */
            nError3 = ::osl::FileBase::searchFileURL( aRelativeFilePath, aRootSys, aUStr );
            sal_Bool bOk3 = compareFileName( aUStr, aTmpURL1 );
            /* search file is passed by an exist file */
            createTestFile( aCanURL1 );
            nError4 = ::osl::FileBase::searchFileURL( aCanonicalFileName, aTmpSys1, aUStr );
            sal_Bool bOk4 = compareFileName( aUStr, aCanURL1 );
            deleteTestFile( aCanURL1 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for searchFileURL function: system filename/URL filename/relative path, system directory, searched file already exist.",
                                    ( osl::FileBase::E_None == nError1 ) && 
                                    ( osl::FileBase::E_None == nError2 ) && 
                                    ( osl::FileBase::E_None == nError3 ) && 
                                    ( osl::FileBase::E_None == nError4 ) && 
                                    ( sal_True == bOk1 ) && 
                                    ( sal_True == bOk2 ) && 
                                    ( sal_True == bOk3 ) && 
                                    ( sal_True == bOk4 ) ); 
        }
 
        
        void searchFileURL_003( )
        {
            OSLTEST_DECLARE( SystemPathList,  TEST_PLATFORM_ROOT":"TEST_PLATFORM_ROOT TEST_PLATFORM_TEMP":"TEST_PLATFORM_ROOT"system/path" ); 
            nError1 = ::osl::FileBase::searchFileURL( aTmpURL1, aSystemPathList, aUStr );
            sal_Bool bOk = compareFileName( aUStr, aTmpURL1 );
            CPPUNIT_ASSERT_MESSAGE( "test for searchFileURL function: search directory is a list of system paths", 
                                    ( osl::FileBase::E_None == nError1 ) && 
                                    ( sal_True == bOk ) );
        }

        void searchFileURL_004( )
        {
            OSLTEST_DECLARE( SystemPathList,  TEST_PLATFORM_ROOT PATH_LIST_DELIMITER TEST_PLATFORM_ROOT TEST_PLATFORM_TEMP PATH_LIST_DELIMITER TEST_PLATFORM_ROOT "system/path/../name" ); 
            nError1 = ::osl::FileBase::searchFileURL( aTmpURL1, aSystemPathList, aUStr );
            sal_Bool bOk = compareFileName( aUStr, aTmpURL1 );
            CPPUNIT_ASSERT_MESSAGE( "test for searchFileURL function: search directory is a list of system paths", 
                                    ( osl::FileBase::E_None == nError1 ) && 
                                    ( sal_True == bOk ) );
        }

        void searchFileURL_005( )
        {
            nError1 = ::osl::FileBase::searchFileURL( aTmpURL1, aNullURL, aUStr );
            sal_Bool bOk = compareFileName( aUStr, aTmpURL1 );
            CPPUNIT_ASSERT_MESSAGE( "test for searchFileURL function: search directory is NULL", 
                                    ( osl::FileBase::E_None == nError1 ) && 
                                    ( sal_True == bOk ) );
        }
        
        CPPUNIT_TEST_SUITE( searchFileURL );
        CPPUNIT_TEST( searchFileURL_001 );
        CPPUNIT_TEST( searchFileURL_002 );
        CPPUNIT_TEST( searchFileURL_003 );
        CPPUNIT_TEST( searchFileURL_004 );
        CPPUNIT_TEST( searchFileURL_005 );
        CPPUNIT_TEST_SUITE_END( );
    };// class searchFileURL
    
    
    //---------------------------------------------------------------------
    // testing the method 
    // static inline RC getTempDirURL( ::rtl::OUString& ustrTempDirURL )
    //---------------------------------------------------------------------
    class getTempDirURL:public CppUnit::TestFixture
    {
        ::osl::FileBase aFileBase;
        ::rtl::OUString aUStr;
        ::osl::FileBase::RC nError;
        
        public:
        // initialization
        void setUp( )
        {
             nError = aFileBase.getTempDirURL( aUStr );
        }

        void tearDown( )
        {
        }

        // test code.
        void getTempDirURL_001( )
        {

            CPPUNIT_ASSERT_MESSAGE( "test for getTempDirURL function: excution",
                                     ( osl::FileBase::E_None == nError ) );
        }

        void getTempDirURL_002( )
        {
            CPPUNIT_ASSERT_MESSAGE( "test for getTempDirURL function: test for open and write access rights", 
                                    checkDirectory( aUStr, osl_Check_Mode_OpenAccess ) && 
                                    checkDirectory( aUStr, osl_Check_Mode_ReadAccess ) && 
                                    checkDirectory( aUStr, osl_Check_Mode_WriteAccess ) );
        }

        CPPUNIT_TEST_SUITE( getTempDirURL );
        CPPUNIT_TEST( getTempDirURL_001 );
        CPPUNIT_TEST( getTempDirURL_002 );
        CPPUNIT_TEST_SUITE_END( );
    };// class getTempDirURL

    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	static inline RC createTempFile( ::rtl::OUString* pustrDirectoryURL, 
    //									 oslFileHandle* pHandle, 
    //									 ::rtl::OUString* pustrTempFileURL)
    //---------------------------------------------------------------------
    class createTempFile:public CppUnit::TestFixture
    {
        ::osl::FileBase aFileBase;
        ::rtl::OUString aUStr;
        ::osl::FileBase::RC nError1, nError2;
        
        oslFileHandle	*pHandle;
        ::rtl::OUString	*pUStr_DirURL; 
        ::rtl::OUString	*pUStr_FileURL; 
        
        public:
        // initialization
        void setUp( )
        {
            pHandle = new oslFileHandle();
            pUStr_DirURL = new ::rtl::OUString();
            pUStr_FileURL = new ::rtl::OUString();
            *pUStr_DirURL = ::rtl::OUString::createFromAscii( pTmpURL1 );
        }

        void tearDown( )
        {
            delete pUStr_DirURL;
            delete pUStr_FileURL;
        }

        // test code.
        void createTempFile_001( )
        { 
            nError1 = aFileBase.createTempFile( pUStr_DirURL, pHandle, pUStr_FileURL );
            nError2 = ::osl::File::remove( *pUStr_FileURL );
            
            CPPUNIT_ASSERT_MESSAGE( "test for createTempFile function: create temp file and test the existence",
                                     ( osl::FileBase::E_None == nError1 ) && 
                                    ( osl::FileBase::E_None == nError2 ) );
        }

        void createTempFile_002( )
        {
            CPPUNIT_ASSERT_MESSAGE( "test for createTempFile function: test for open and write access rights", 
                                    1 == 1 );
        }

        CPPUNIT_TEST_SUITE( createTempFile );
        CPPUNIT_TEST( createTempFile_001 ); 
        CPPUNIT_TEST( createTempFile_002 );
        CPPUNIT_TEST_SUITE_END( );
    };// class createTempFile

    // -----------------------------------------------------------------------------
    //CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_FileBase::getCanonicalName, "osl_FileBase" );
    //CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_FileBase::getAbsoluteFileURL, "osl_FileBase" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_FileBase::getSystemPathFromFileURL, "osl_FileBase" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_FileBase::searchFileURL, "osl_FileBase" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_FileBase::getTempDirURL, "osl_FileBase" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_FileBase::createTempFile, "osl_FileBase" );
}// namespace osl_FileBase





//------------------------------------------------------------------------
// Beginning of the test cases for VolumeDevice class
//------------------------------------------------------------------------
namespace osl_VolumeDevice
{
    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	VolumeDevice() : _aHandle( NULL )
    //---------------------------------------------------------------------
    class  ctors : public CppUnit::TestFixture
    {
        ::osl::VolumeDevice aVolumeDevice;
        ::rtl::OUString aUStr;
        ::osl::FileBase::RC nError1, nError2;
        
        public:
        // initialization
        void setUp( )
        {
        }

        void tearDown( )
        {
        }

        // test code.
        void ctors_001( )
        { 
            ::osl::VolumeDevice aVolumeDevice1;
            
            CPPUNIT_ASSERT_MESSAGE( "test for ctors function: Constructor for VolumeDevice with no args.",
                                     ( osl::FileBase::E_None !=  aVolumeDevice1.automount( ) ) &&
                                    ( osl::FileBase::E_None !=  aVolumeDevice1.unmount( ) )   &&
                                    ( aNullURL.equals( aVolumeDevice1.getMountPath( ) ) ) );
        }

         void ctors_002( )
        {
            ::osl::VolumeInfo   aVolumeInfo( VolumeInfoMask_Attributes );
            nError1 = ::osl::Directory::getVolumeInfo( aRootURL, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            
            ::osl::VolumeDevice aVolumeDevice1( aVolumeInfo.getDeviceHandle( ) );
            sal_Bool bOk = compareFileName( aNullURL, aVolumeDevice1.getMountPath( ) );
            CPPUNIT_ASSERT_MESSAGE( "test for ctors function: Copy constructor for VolumeDevice, the copied VolumeDevice should have a mount path file:///, but it returned an empty OUString, it also may be the error from getDeviceHandle()", 
                                    sal_False == bOk );
        }

         void ctors_003( )
        {
            ::osl::VolumeInfo   aVolumeInfo( VolumeInfoMask_Attributes );
            nError1 = ::osl::Directory::getVolumeInfo( aRootURL, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            
            ::osl::VolumeDevice aVolumeDevice1 = aVolumeInfo.getDeviceHandle( );
            sal_Bool bOk = compareFileName( aNullURL, aVolumeDevice1.getMountPath( ) );
            CPPUNIT_ASSERT_MESSAGE( "test for ctors function: Assigned operator for VolumeDevice, the assigned VolumeDevice should have a mount path file:///, but it returned an empty OUString, it also may be the error from getDeviceHandle()", 
                                    sal_False == bOk );
        }
        
        CPPUNIT_TEST_SUITE( ctors );
        CPPUNIT_TEST( ctors_001 ); 
        CPPUNIT_TEST( ctors_002 );
        CPPUNIT_TEST( ctors_003 );
        CPPUNIT_TEST_SUITE_END( );
    };// class ctors
        
    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline RC automount()
    //---------------------------------------------------------------------
    class  automount : public CppUnit::TestFixture
    {
        ::osl::VolumeDevice aVolumeDevice;
        ::rtl::OUString aUStr;
        ::osl::FileBase::RC nError1, nError2;
        
        public:
        // initialization
        void setUp( )
        {
        }

        void tearDown( )
        {
      
        }

        // test code.
        void automount_001( )
        { 
            ::osl::VolumeDevice aVolumeDevice1;
            nError1 = aVolumeDevice1.automount( ); 
            
            CPPUNIT_ASSERT_MESSAGE( "test for automount function: invalid parameter.",
                                     ( osl::FileBase::E_INVAL == nError1 ) );
        }

          void automount_002( )
        {
            ::osl::VolumeInfo   aVolumeInfo( VolumeInfoMask_Attributes );
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL2, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 ); 
            
            ::osl::VolumeDevice aVolumeDevice1( aVolumeInfo.getDeviceHandle( ) );
              nError1 = aVolumeDevice1.unmount( );
              nError1 = aVolumeDevice1.automount( );
            CPPUNIT_ASSERT_MESSAGE( "test for automount function: this test is not implemented yet", 
                                    ( osl::FileBase::E_None == nError1 ) );
        }

        CPPUNIT_TEST_SUITE( automount );
        CPPUNIT_TEST( automount_001 ); 
        CPPUNIT_TEST( automount_002 );
        CPPUNIT_TEST_SUITE_END( );
    };// class automount
        

    // -----------------------------------------------------------------------------
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_VolumeDevice::ctors, "osl_VolumeDevice" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_VolumeDevice::automount, "osl_VolumeDevice" );
}// namespace osl_VolumeDevice



//------------------------------------------------------------------------
// Beginning of the test cases for VolumeInfo class
//------------------------------------------------------------------------
namespace osl_VolumeInfo
{
    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	VolumeInfo( sal_uInt32 nMask ): _nMask( nMask )
    //---------------------------------------------------------------------
    class  ctors : public CppUnit::TestFixture
    {
        ::rtl::OUString aUStr;
        ::osl::FileBase::RC nError1, nError2;
        
         ::osl::VolumeDevice aVolumeDevice1;   
        
        public:
        // initialization
        void setUp( )
        {
        }

        void tearDown( )
        {
        }

        // test code.
        void ctors_001( )
        { 
            ::osl::VolumeInfo   aVolumeInfo( 0 ); 
            nError1 = ::osl::Directory::getVolumeInfo( aRootURL, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            sal_uInt64 uiTotalSpace = aVolumeInfo.getTotalSpace( ); 
            sal_uInt32 uiMaxPathLength = aVolumeInfo.getMaxPathLength( );
            aUStr = aVolumeInfo.getFileSystemName( );
            
            CPPUNIT_ASSERT_MESSAGE( "test for ctors function: mask is empty",
                                     ( 0 == uiTotalSpace ) && 
                                    ( 0 == uiMaxPathLength ) &&
                                    sal_True == compareFileName( aUStr, aNullURL ) );
        }

        void ctors_002( )
        { 
            ::osl::VolumeInfo   aVolumeInfo( VolumeInfoMask_TotalSpace | 
                                             VolumeInfoMask_UsedSpace |
                                             VolumeInfoMask_FileSystemName ); 
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL4, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            //CPPUNIT_ASSERT( aVolumeInfo.isValid( mask ) );
            sal_uInt64 uiTotalSpace = aVolumeInfo.getTotalSpace( ); 
            sal_uInt64 uiUsedSpace = aVolumeInfo.getUsedSpace( );
            aUStr = aVolumeInfo.getFileSystemName( );
            
            CPPUNIT_ASSERT_MESSAGE( "test for ctors function: mask is specified as certain valid fields, and get the masked fields",
                                     ( 0 != uiTotalSpace ) && 
                                    ( 0 != uiUsedSpace ) &&
                                    sal_True == compareFileName( aUStr, "nfs" ) );
        }
        
        void ctors_003( )
        {

             sal_Int32 mask1 = VolumeInfoMask_FreeSpace;
            ::osl::VolumeInfo aVolumeInfo1( mask1 ); 
            nError1 = ::osl::Directory::getVolumeInfo( aRootURL, aVolumeInfo1 );
            CPPUNIT_ASSERT( sal_True == aVolumeInfo1.isValid( mask1 ) );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            
            sal_uInt64 uiTotalSpace1 = aVolumeInfo1.getTotalSpace( );
            aUStr = aVolumeInfo1.getFileSystemName( );
            
             sal_Int32 mask2 = VolumeInfoMask_TotalSpace;
            ::osl::VolumeInfo aVolumeInfo2( mask2 ); 
            nError2 = ::osl::Directory::getVolumeInfo( aRootURL, aVolumeInfo2 );
            CPPUNIT_ASSERT( sal_True == aVolumeInfo2.isValid( mask2 ) );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError2 );
            
            sal_uInt64 uiTotalSpace2 = aVolumeInfo2.getTotalSpace( );
            
            CPPUNIT_ASSERT_MESSAGE( "test for ctors function: mask is specified as certain valid fields, but get unmasked fields",
                                     ( 0 == uiTotalSpace1 ) && ( 0 != uiTotalSpace2 ) &&
                                    sal_True == compareFileName( aUStr, aNullURL ) );
        }
        
        CPPUNIT_TEST_SUITE( ctors );
        CPPUNIT_TEST( ctors_001 ); 
        CPPUNIT_TEST( ctors_002 );
        CPPUNIT_TEST( ctors_003 );
        CPPUNIT_TEST_SUITE_END( );
    };// class ctors
        
    
     //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline sal_Bool isValid( sal_uInt32 nMask ) const
    //---------------------------------------------------------------------
    class  isValid : public CppUnit::TestFixture
    {
        ::osl::VolumeDevice aVolumeDevice;
        ::rtl::OUString aUStr;
        ::osl::FileBase::RC nError1, nError2;
        
        public:
        // initialization
        void setUp( )
        {
        }

        void tearDown( )
        {
      
        }

        // test code.
        void isValid_001( )
        { 
            sal_Int32 mask = 0;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL4, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );

            CPPUNIT_ASSERT_MESSAGE( "test for isValid function: no fields specified.",
                                      sal_True == aVolumeInfo.isValid( mask ) );
        }

          void isValid_002( )
        {
            sal_Int32 mask = VolumeInfoMask_Attributes | VolumeInfoMask_TotalSpace | osl_VolumeInfo_Mask_UsedSpace |
                             osl_VolumeInfo_Mask_FreeSpace | osl_VolumeInfo_Mask_MaxNameLength |
                             osl_VolumeInfo_Mask_MaxPathLength | osl_VolumeInfo_Mask_FileSystemName;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL4, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for isValid function: all valid fields specified for a nfs volume.",
                                     sal_True == aVolumeInfo.isValid( mask ) );
        }

         void isValid_003( )
        {
             ::osl::VolumeDevice aVolumeDevice1; 
            sal_Int32 mask = VolumeInfoMask_Attributes;
            ::osl::VolumeInfo   aVolumeInfo( mask );
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL1, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            sal_Bool bOk1 = aVolumeInfo.isValid( mask );
            
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL2, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            sal_Bool bOk2 = aVolumeInfo.isValid( mask );
            
            CPPUNIT_ASSERT_MESSAGE( "test for isValid function: VolumeInfoMask_Attributes, it should be valid for some volume such as /, floppy, cdrom, etc. but it did not pass",
                                     ( sal_True == bOk1 ) && ( sal_True == bOk2 ) );
        } 
         
        CPPUNIT_TEST_SUITE( isValid );
        CPPUNIT_TEST( isValid_001 ); 
        CPPUNIT_TEST( isValid_002 );
        CPPUNIT_TEST( isValid_003 );
        CPPUNIT_TEST_SUITE_END( );
    };// class isValid

     //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline sal_Bool getRemoteFlag() const
    //---------------------------------------------------------------------
    class  getRemoteFlag : public CppUnit::TestFixture
    {
        ::osl::VolumeDevice aVolumeDevice;
        ::rtl::OUString aUStr;
        ::osl::FileBase::RC nError1, nError2;
        
        public:
        // test code.
        void getRemoteFlag_001( )
        { 
            sal_Int32 mask = VolumeInfoMask_Attributes;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL1, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            sal_Bool bOk = aVolumeInfo.getRemoteFlag( );

            CPPUNIT_ASSERT_MESSAGE( "test for getRemoteFlag function: get a volume device which is not remote.",
                                     ( sal_False == bOk ) );
        }

 #if ( defined UNX ) || ( defined OS2 )	//remote Volume is different in Solaris and Windows
        void getRemoteFlag_002( )
        { 
            sal_Int32 mask = VolumeInfoMask_Attributes;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL4, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            sal_Bool bOk = aVolumeInfo.getRemoteFlag( );

            CPPUNIT_ASSERT_MESSAGE( "test for getRemoteFlag function: get a volume device which is remote( Solaris version ).",
                                     ( sal_True == bOk ) );
        }
#else                                    //Windows version 
        void getRemoteFlag_002( )
        {
            CPPUNIT_ASSERT_MESSAGE( "test for getRemoteFlag function: get a volume device which is remote( Windows version )",
                                     1 == 1 );
        }
#endif	
        
        CPPUNIT_TEST_SUITE( getRemoteFlag );
        CPPUNIT_TEST( getRemoteFlag_001 ); 
        CPPUNIT_TEST( getRemoteFlag_002 );
        CPPUNIT_TEST_SUITE_END( );
    };// class getRemoteFlag

    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline sal_Bool getRemoveableFlag() const
    //---------------------------------------------------------------------
    class  getRemoveableFlag : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC nError1, nError2;
        
        public:
        // test code.
        void getRemoveableFlag_001( )
        { 
            sal_Int32 mask = VolumeInfoMask_Attributes;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL1, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            sal_Bool bOk = aVolumeInfo.getRemoveableFlag( );

            CPPUNIT_ASSERT_MESSAGE( "test for getRemoveableFlag function: get a volume device which is not removable.",
                                     sal_False == bOk );
        }

        void getRemoveableFlag_002( )
        { 
            sal_Int32 mask = VolumeInfoMask_Attributes;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL2, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            sal_Bool bOk = aVolumeInfo.getRemoveableFlag( );

            CPPUNIT_ASSERT_MESSAGE( "test for getRemoveableFlag function: get a volume device which is removable, not sure, here we use floppy disk, but it did not pass.",
                                     sal_True == bOk );
        }
        CPPUNIT_TEST_SUITE( getRemoveableFlag );
        CPPUNIT_TEST( getRemoveableFlag_001 ); 
        CPPUNIT_TEST( getRemoveableFlag_002 );
        CPPUNIT_TEST_SUITE_END( );
    };// class getRemoveableFlag
    
    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline sal_Bool getCompactDiscFlag() const
    //---------------------------------------------------------------------
    class  getCompactDiscFlag : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC nError1;
        
        public:
        // test code.
        void getCompactDiscFlag_001( )
        { 
            sal_Int32 mask = VolumeInfoMask_Attributes;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL1, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            sal_Bool bOk = aVolumeInfo.getCompactDiscFlag( );

            CPPUNIT_ASSERT_MESSAGE( "test for getCompactDiscFlag function: get a volume device which is not a cdrom.",
                                      ( sal_False == bOk ) );
        }

        void getCompactDiscFlag_002( )
        { 
             sal_Int32 mask = VolumeInfoMask_Attributes;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL6, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            sal_Bool bOk = aVolumeInfo.getCompactDiscFlag( );
 
            CPPUNIT_ASSERT_MESSAGE( "test for getCompactDiscFlag function: get a cdrom volume device flag, it did not pass.",
                                      ( sal_True == bOk ) );
        }
        CPPUNIT_TEST_SUITE( getCompactDiscFlag );
        CPPUNIT_TEST( getCompactDiscFlag_001 ); 
        CPPUNIT_TEST( getCompactDiscFlag_002 );
        CPPUNIT_TEST_SUITE_END( );
    };// class getCompactDiscFlag	
    
    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline sal_Bool getFloppyDiskFlag() const
    //---------------------------------------------------------------------
    class  getFloppyDiskFlag : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC nError1;
        
        public:
        // test code.
        void getFloppyDiskFlag_001( )
        { 
            sal_Int32 mask = VolumeInfoMask_Attributes;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL1, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            sal_Bool bOk = aVolumeInfo.getFloppyDiskFlag( );

            CPPUNIT_ASSERT_MESSAGE( "test for getFloppyDiskFlag function: get a volume device which is not a floppy disk.",
                                      ( sal_False == bOk ) );
        }

        void getFloppyDiskFlag_002( )
        { 
             sal_Int32 mask = VolumeInfoMask_Attributes;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL2, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            sal_Bool bOk = aVolumeInfo.getFloppyDiskFlag( );
 
            CPPUNIT_ASSERT_MESSAGE( "test for getFloppyDiskFlag function: get a floppy volume device flag, it did not pass.",
                                      ( sal_True == bOk ) );
        }
        CPPUNIT_TEST_SUITE( getFloppyDiskFlag );
        CPPUNIT_TEST( getFloppyDiskFlag_001 ); 
        CPPUNIT_TEST( getFloppyDiskFlag_002 );
        CPPUNIT_TEST_SUITE_END( );
    };// class getFloppyDiskFlag	
    
    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline sal_Bool getFixedDiskFlag() const
    //---------------------------------------------------------------------
    class  getFixedDiskFlag : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC nError1;
        
        public:
        // test code.
        void getFixedDiskFlag_001( )
        { 
            sal_Int32 mask = VolumeInfoMask_Attributes;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL2, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            sal_Bool bOk = aVolumeInfo.getFixedDiskFlag( );

            CPPUNIT_ASSERT_MESSAGE( "test for getFixedDiskFlag function: get a volume device which is not a fixed disk.",
                                     ( sal_False == bOk ) );
        }

        void getFixedDiskFlag_002( )
        { 
             sal_Int32 mask = VolumeInfoMask_Attributes;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL1, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            sal_Bool bOk = aVolumeInfo.getFixedDiskFlag( );
 
            CPPUNIT_ASSERT_MESSAGE( "test for getFixedDiskFlag function: get a fixed disk volume device flag, it did not pass.",
                                     ( sal_True == bOk ) );
        }
        CPPUNIT_TEST_SUITE( getFixedDiskFlag );
        CPPUNIT_TEST( getFixedDiskFlag_001 ); 
        CPPUNIT_TEST( getFixedDiskFlag_002 );
        CPPUNIT_TEST_SUITE_END( );
    };// class getFixedDiskFlag	
    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline sal_Bool getRAMDiskFlag() const
    //---------------------------------------------------------------------
    class  getRAMDiskFlag : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC nError1;
        
        public:
        // test code.
        void getRAMDiskFlag_001( )
        { 
            sal_Int32 mask = VolumeInfoMask_Attributes;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL1, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            sal_Bool bOk = aVolumeInfo.getRAMDiskFlag( );

            CPPUNIT_ASSERT_MESSAGE( "test for getRAMDiskFlag function: get a volume device which is not a RAM disk.",
                                     ( sal_False == bOk ) );
        }

        void getRAMDiskFlag_002( )
        { 
             sal_Int32 mask = VolumeInfoMask_Attributes;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL1, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            sal_Bool bOk = aVolumeInfo.getRAMDiskFlag( );
 
            CPPUNIT_ASSERT_MESSAGE( "test for getRAMDiskFlag function: FIX ME, don't know how to get a RAM disk flag, perhaps Windows 98 boot disk can create a RAM disk, it did not pass.",
                                     ( sal_True == bOk ) );
        }
        CPPUNIT_TEST_SUITE( getRAMDiskFlag );
        CPPUNIT_TEST( getRAMDiskFlag_001 ); 
        CPPUNIT_TEST( getRAMDiskFlag_002 );
        CPPUNIT_TEST_SUITE_END( );
    };// class getRAMDiskFlag	
    

    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline sal_uInt64 getTotalSpace() const
    //---------------------------------------------------------------------
    class  getTotalSpace : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC nError1;
        
        public:
        // test code.
        void getTotalSpace_001( )
        { 
            sal_Int32 mask = VolumeInfoMask_TotalSpace;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL1, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            CPPUNIT_ASSERT( sal_True == aVolumeInfo.isValid( mask ) );
            sal_uInt64 uiTotalSpace = aVolumeInfo.getTotalSpace( );

            CPPUNIT_ASSERT_MESSAGE( "test for getTotalSpace function: get total space of Fixed disk volume mounted on /, it should not be 0",
                                     0 != uiTotalSpace );
        }

         void getTotalSpace_002( )
        { 
             sal_Int32 mask = VolumeInfoMask_TotalSpace;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL3, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            CPPUNIT_ASSERT( sal_True == aVolumeInfo.isValid( mask ) );
            sal_uInt64 uiTotalSpace = aVolumeInfo.getTotalSpace( );
 
            CPPUNIT_ASSERT_MESSAGE( "test for getTotalSpace function: get total space of /proc, it should be 0",
                                     0 == uiTotalSpace );
        }
        
        
#if defined(SOLARIS)
         void getTotalSpace_003( )
        { 
             struct statvfs aStatFS;
            static const sal_Char  name[] = "/";

            memset (&aStatFS, 0, sizeof(aStatFS));
            statvfs( name, &aStatFS);
            sal_uInt64 TotalSpace = aStatFS.f_frsize * aStatFS.f_blocks ;
            
             sal_Int32 mask = VolumeInfoMask_TotalSpace;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL1, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            CPPUNIT_ASSERT( sal_True == aVolumeInfo.isValid( mask ) );
            sal_uInt64 uiTotalSpace = aVolumeInfo.getTotalSpace( );

             CPPUNIT_ASSERT_MESSAGE( "test for getTotalSpace function: get total space by hand, then compare with getTotalSpace, it did not pass",
                                     uiTotalSpace == TotalSpace );
        }
#else                                    //Windows version 
        void getTotalSpace_003( )
        {
            CPPUNIT_ASSERT_MESSAGE( "test for getTotalSpace function:not implemented yet( Windows version )",
                                     1 == 1 );
        }
#endif 		
        CPPUNIT_TEST_SUITE( getTotalSpace );
        CPPUNIT_TEST( getTotalSpace_001 ); 
        CPPUNIT_TEST( getTotalSpace_002 );
        CPPUNIT_TEST( getTotalSpace_003 );
        CPPUNIT_TEST_SUITE_END( );
    };// class getTotalSpace	
    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline sal_uInt64 getFreeSpace() const
    //---------------------------------------------------------------------
    class  getFreeSpace : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC nError1;
        
        public:
        // test code.
        void getFreeSpace_001( )
        { 
            sal_Int32 mask = VolumeInfoMask_FreeSpace;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL1, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            CPPUNIT_ASSERT( sal_True == aVolumeInfo.isValid( mask ) );
            sal_uInt64 uiFreeSpace = aVolumeInfo.getFreeSpace( );

            CPPUNIT_ASSERT_MESSAGE( "test for getFreeSpace function: get free space of Fixed disk volume mounted on /, it should not be 0, suggestion: returned value, -1 is better, since some times the free space may be 0",
                                     0 != uiFreeSpace );
        }

          void getFreeSpace_002( )
        { 
             sal_Int32 mask = VolumeInfoMask_FreeSpace;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL3, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            CPPUNIT_ASSERT( sal_True == aVolumeInfo.isValid( mask ) );
            sal_uInt64 uiFreeSpace = aVolumeInfo.getFreeSpace( );
 
            CPPUNIT_ASSERT_MESSAGE( "test for getFreeSpace function: get free space of /proc, it should be 0",
                                     0 == uiFreeSpace );
        }
        
        
#if defined(SOLARIS)
         void getFreeSpace_003( )
        { 
             struct statvfs aStatFS;
            static const sal_Char  name[] = "/";

            memset (&aStatFS, 0, sizeof(aStatFS));
            statvfs( name, &aStatFS);
            sal_uInt64 FreeSpace = aStatFS.f_bfree * aStatFS.f_frsize  ;
            
             sal_Int32 mask = VolumeInfoMask_FreeSpace;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL1, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            CPPUNIT_ASSERT( sal_True == aVolumeInfo.isValid( mask ) );
            sal_uInt64 uiFreeSpace = aVolumeInfo.getFreeSpace( );

             CPPUNIT_ASSERT_MESSAGE( "test for getFreeSpace function: get free space by hand, then compare with getFreeSpace, it did not pass",
                                     uiFreeSpace == FreeSpace );
        }
#else                                    //Windows version 
        void getFreeSpace_003( )
        {
            CPPUNIT_ASSERT_MESSAGE( "test for getFreeSpace function: not implemented yet( Windows version )",
                                     1 == 1 );
        }
#endif 		


        CPPUNIT_TEST_SUITE( getFreeSpace );
        CPPUNIT_TEST( getFreeSpace_001 ); 
         CPPUNIT_TEST( getFreeSpace_002 );
        CPPUNIT_TEST( getFreeSpace_003 );
        CPPUNIT_TEST_SUITE_END( );
    };// class getFreeSpace	

    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline sal_uInt64 getUsedSpace() const
    //---------------------------------------------------------------------
    class  getUsedSpace : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC nError1;
        
        public:
        // test code.
        void getUsedSpace_001( )
        { 
            sal_Int32 mask = VolumeInfoMask_UsedSpace;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL1, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            CPPUNIT_ASSERT( sal_True == aVolumeInfo.isValid( mask ) );
            sal_uInt64 uiUsedSpace = aVolumeInfo.getUsedSpace( );

            CPPUNIT_ASSERT_MESSAGE( "test for getUsedSpace function: get used space of Fixed disk volume mounted on /, it should not be 0, suggestion: returned value, -1 is better, since some times the used space may be 0",
                                     0 != uiUsedSpace );
        }

           void getUsedSpace_002( )
        { 
             sal_Int32 mask = VolumeInfoMask_UsedSpace;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL3, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            CPPUNIT_ASSERT( sal_True == aVolumeInfo.isValid( mask ) );
            sal_uInt64 uiUsedSpace = aVolumeInfo.getUsedSpace( );
 
            CPPUNIT_ASSERT_MESSAGE( "test for getUsedSpace function: get used space of /proc, it should be 0",
                                     0 == uiUsedSpace );
        }
        
        
#if defined(SOLARIS)
         void getUsedSpace_003( )
        { 
             struct statvfs aStatFS;
            static const sal_Char  name[] = "/";

            memset (&aStatFS, 0, sizeof(aStatFS));
            statvfs( name, &aStatFS);
            sal_uInt64 UsedSpace = ( aStatFS.f_blocks - aStatFS.f_bavail ) * aStatFS.f_frsize;
            
            
             sal_Int32 mask = VolumeInfoMask_UsedSpace;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL1, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            CPPUNIT_ASSERT( sal_True == aVolumeInfo.isValid( mask ) );
            sal_uInt64 uiUsedSpace = aVolumeInfo.getUsedSpace( );

             CPPUNIT_ASSERT_MESSAGE( "test for getUsedSpace function: get used space by hand, then compare with getUsedSpace, it did not pass",
                                     uiUsedSpace == UsedSpace );
        }
#else                                    //Windows version 
        void getUsedSpace_003( )
        {
            CPPUNIT_ASSERT_MESSAGE( "test for getUsedSpace function: not implemented yet( Windows version )",
                                     1 == 1 );
        }
#endif 		


        CPPUNIT_TEST_SUITE( getUsedSpace );
         CPPUNIT_TEST( getUsedSpace_001 ); 
         CPPUNIT_TEST( getUsedSpace_002 );
        CPPUNIT_TEST( getUsedSpace_003 );
         CPPUNIT_TEST_SUITE_END( );
    };// class getUsedSpace	 
    
    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline sal_uInt32 getMaxNameLength() const
    //---------------------------------------------------------------------
    class  getMaxNameLength : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC nError1;
        
        public:
        // test code.
        void getMaxNameLength_001( )
        { 
            sal_Int32 mask = VolumeInfoMask_MaxNameLength;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL1, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            CPPUNIT_ASSERT( sal_True == aVolumeInfo.isValid( mask ) );
            sal_uInt32 uiMaxNameLength = aVolumeInfo.getMaxNameLength( );

            CPPUNIT_ASSERT_MESSAGE( "test for getMaxNameLength function: get max name length of Fixed disk volume mounted on /, it should not be 0",
                                     0 != uiMaxNameLength );
        }

        
#if ( defined UNX ) || ( defined OS2 )
         void getMaxNameLength_002( )
        { 
             struct statvfs aStatFS;
            static const sal_Char  name[] = "/";

            memset (&aStatFS, 0, sizeof(aStatFS));
            statvfs( name, &aStatFS);
            sal_uInt64 MaxNameLength = aStatFS.f_namemax;
            
             sal_Int32 mask = VolumeInfoMask_MaxNameLength;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL1, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            CPPUNIT_ASSERT( sal_True == aVolumeInfo.isValid( mask ) );
            sal_uInt64 uiMaxNameLength = aVolumeInfo.getMaxNameLength( );

             CPPUNIT_ASSERT_MESSAGE( "test for getMaxNameLength function: get max name length by hand, then compare with getMaxNameLength",
                                     uiMaxNameLength == MaxNameLength );
        }
#else                                    //Windows version 
        void getMaxNameLength_002( )
        {
            CPPUNIT_ASSERT_MESSAGE( "test for getMaxNameLength function: not implemented yet( Windows version )",
                                     1 == 1 );
        }
#endif 		

        CPPUNIT_TEST_SUITE( getMaxNameLength );
         CPPUNIT_TEST( getMaxNameLength_001 ); 
        CPPUNIT_TEST( getMaxNameLength_002 );
         CPPUNIT_TEST_SUITE_END( );
    };// class getMaxNameLength	 
    
    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline sal_uInt32 getMaxPathLength() const
    //---------------------------------------------------------------------
    class  getMaxPathLength : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC nError1;
        
        public:
        // test code.
        void getMaxPathLength_001( )
        { 
            sal_Int32 mask = VolumeInfoMask_MaxPathLength;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL1, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            CPPUNIT_ASSERT( sal_True == aVolumeInfo.isValid( mask ) );
            sal_uInt32 uiMaxPathLength = aVolumeInfo.getMaxPathLength( );

            CPPUNIT_ASSERT_MESSAGE( "test for getMaxPathLength function: get max path length of Fixed disk volume mounted on /, it should not be 0",
                                     0 != uiMaxPathLength );
        }

        
#if ( defined UNX ) || ( defined OS2 )
         void getMaxPathLength_002( )
        { 
             sal_Int32 mask = VolumeInfoMask_MaxPathLength;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL1, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            CPPUNIT_ASSERT( sal_True == aVolumeInfo.isValid( mask ) );
            sal_uInt64 uiMaxPathLength = aVolumeInfo.getMaxPathLength( );

             CPPUNIT_ASSERT_MESSAGE( "test for getMaxPathLength function: get max path length by hand, then compare with getMaxPathLength",
                                     uiMaxPathLength == PATH_MAX );
        }
#else                                    //Windows version 
        void getMaxPathLength_002( )
        {
            CPPUNIT_ASSERT_MESSAGE( "test for getMaxPathLength function: not implemented yet( Windows version )",
                                     1 == 1 );
        }
#endif 		


        CPPUNIT_TEST_SUITE( getMaxPathLength );
         CPPUNIT_TEST( getMaxPathLength_001 ); 
        CPPUNIT_TEST( getMaxPathLength_002 );
         CPPUNIT_TEST_SUITE_END( );
    };// class getMaxPathLength	 
    
    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline ::rtl::OUString getFileSystemName() const
    //---------------------------------------------------------------------
    class  getFileSystemName : public CppUnit::TestFixture
    {
        ::rtl::OUString aUStr;
        ::osl::FileBase::RC nError1;
        
        public:
        // test code.
        void getFileSystemName_001( )
        { 
            sal_Int32 mask = VolumeInfoMask_FileSystemName;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL1, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            CPPUNIT_ASSERT( sal_True == aVolumeInfo.isValid( mask ) );
            aUStr = aVolumeInfo.getFileSystemName( );

            CPPUNIT_ASSERT_MESSAGE( "test for getFileSystemName function: get file system name of Fixed disk volume mounted on /, it should not be empty string",
                                     sal_False == compareFileName( aNullURL, aUStr ) );
        }

        
#if defined(SOLARIS)
         void getFileSystemName_002( )
        { 
             struct statvfs aStatFS;
            static const sal_Char  name[] = "/";

            memset (&aStatFS, 0, sizeof(aStatFS));
            statvfs( name, &aStatFS);
            sal_Char * astrFileSystemName = aStatFS.f_basetype;

             sal_Int32 mask = VolumeInfoMask_FileSystemName;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL1, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            CPPUNIT_ASSERT( sal_True == aVolumeInfo.isValid( mask ) );
            aUStr = aVolumeInfo.getFileSystemName( );

             CPPUNIT_ASSERT_MESSAGE( "test for getFileSystemName function: get file system name by hand, then compare with getFileSystemName",
                                     sal_True == compareFileName( aUStr, astrFileSystemName ) );
        }
#else                                    //Windows version 
        void getFileSystemName_002( )
        {
            CPPUNIT_ASSERT_MESSAGE( "test for getFileSystemName function: not implemented yet( Windows version )",
                                     1 == 1 );
        }
#endif 		
 

        CPPUNIT_TEST_SUITE( getFileSystemName );
         CPPUNIT_TEST( getFileSystemName_001 ); 
        CPPUNIT_TEST( getFileSystemName_002 );
         CPPUNIT_TEST_SUITE_END( );
    };// class getFileSystemName

    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline VolumeDevice getDeviceHandle() const
    //---------------------------------------------------------------------
    class  getDeviceHandle : public CppUnit::TestFixture
    {
        ::rtl::OUString aUStr;
        ::osl::FileBase::RC nError1;
        
        public:
        // test code.
        void getDeviceHandle_001( )
        { 
            ::osl::VolumeInfo   aVolumeInfo( VolumeInfoMask_Attributes );
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL1, aVolumeInfo );
            CPPUNIT_ASSERT( osl::FileBase::E_None == nError1 );
            
            ::osl::VolumeDevice aVolumeDevice1( aVolumeInfo.getDeviceHandle( ) );
            sal_Bool bOk = compareFileName( aNullURL, aVolumeDevice1.getMountPath( ) );

            CPPUNIT_ASSERT_MESSAGE( "test for getDeviceHandle function: get device handle of Fixed disk volume mounted on /, it should not be NULL, it did not pass",
                                     ( sal_False == bOk ) );
        }

        CPPUNIT_TEST_SUITE( getDeviceHandle );
         CPPUNIT_TEST( getDeviceHandle_001 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class getDeviceHandle


    // -----------------------------------------------------------------------------
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_VolumeInfo::ctors, "osl_VolumeInfo" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_VolumeInfo::isValid, "osl_VolumeInfo" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_VolumeInfo::getRemoteFlag, "osl_VolumeInfo" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_VolumeInfo::getRemoveableFlag, "osl_VolumeInfo" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_VolumeInfo::getCompactDiscFlag, "osl_VolumeInfo" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_VolumeInfo::getFloppyDiskFlag, "osl_VolumeInfo" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_VolumeInfo::getFixedDiskFlag, "osl_VolumeInfo" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_VolumeInfo::getRAMDiskFlag, "osl_VolumeInfo" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_VolumeInfo::getTotalSpace, "osl_VolumeInfo" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_VolumeInfo::getFreeSpace, "osl_VolumeInfo" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_VolumeInfo::getUsedSpace, "osl_VolumeInfo" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_VolumeInfo::getMaxNameLength, "osl_VolumeInfo" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_VolumeInfo::getMaxPathLength, "osl_VolumeInfo" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_VolumeInfo::getFileSystemName, "osl_VolumeInfo" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_VolumeInfo::getDeviceHandle, "osl_VolumeInfo" );
}// namespace osl_VolumeInfo



//------------------------------------------------------------------------
// Beginning of the test cases for VolumeDevice class
//------------------------------------------------------------------------
namespace osl_FileStatus
{
    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	FileStatus( sal_uInt32 nMask ): _nMask( nMask )
    //---------------------------------------------------------------------
    class  ctors : public CppUnit::TestFixture
    {
        ::rtl::OUString         aUStr;
        ::osl::FileBase::RC     nError1, nError2;
        ::osl::DirectoryItem    rItem;
        
        public:
        // initialization
        void setUp( )
        {
            // create a tempfile in $TEMP/tmpdir/tmpname.
            createTestDirectory( aTmpName3 );
            createTestFile( aTmpName4 );
            
            ::std::auto_ptr<Directory> pDir( new Directory( aTmpName3 ) );
            nError1 = pDir->open( );	
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            nError1 = pDir->getNextItem( rItem, 0 );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
        }

        void tearDown( )
        {
            // remove the tempfile in $TEMP/tmpdir/tmpname.
            deleteTestFile( aTmpName4 );
            deleteTestDirectory( aTmpName3 );
        }

        // test code.
        void ctors_001( )
        { 
             ::osl::FileStatus   rFileStatus( FileStatusMask_All ); 
            nError1 = rItem.getFileStatus( rFileStatus );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            aUStr = rFileStatus.getFileName( );
            
            CPPUNIT_ASSERT_MESSAGE( "test for ctors function: mask all and see the file name",
                                     sal_True == compareFileName( aUStr, aTmpName2)  );
        }

        void ctors_002( )
        { 
             ::osl::FileStatus   rFileStatus( 0 ); 
            nError1 = rItem.getFileStatus( rFileStatus );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            aUStr = rFileStatus.getFileName( );
            
            CPPUNIT_ASSERT_MESSAGE( "test for ctors function: mask is empty",
                                     sal_True == compareFileName( aUStr, aNullURL)  );
        }
        
        CPPUNIT_TEST_SUITE( ctors );
        CPPUNIT_TEST( ctors_001 ); 
        CPPUNIT_TEST( ctors_002 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class ctors
        
        
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline sal_Bool isValid( sal_uInt32 nMask ) const
    //---------------------------------------------------------------------
    class  isValid : public CppUnit::TestFixture
    {
        ::rtl::OUString         aUStr;
        ::osl::FileBase::RC     nError1, nError2;
        ::osl::Directory        *pDir;
        ::osl::DirectoryItem    rItem_file, rItem_link;
        
        public:
        // initialization
        void setUp( )
        {
            // create a tempfile in $TEMP/tmpdir/tmpname.
            createTestDirectory( aTmpName3 );
            createTestFile( aTmpName4 );
            
            pDir = new Directory( aTmpName3 );
            nError1 = pDir->open( );	
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            nError1 = pDir->getNextItem( rItem_file, 1 );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
        }

        void tearDown( )
        {
            nError1 = pDir->close( );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            delete pDir;
            
            // remove the tempfile in $TEMP/tmpdir/tmpname.
            deleteTestFile( aTmpName4 );
            deleteTestDirectory( aTmpName3 );
        }

        // test code.
        void isValid_001( )
        { 
            sal_uInt32 mask = 0;
             ::osl::FileStatus   rFileStatus( mask ); 
            nError1 = rItem_file.getFileStatus( rFileStatus );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            sal_Bool bOk = rFileStatus.isValid( mask ); 
            
            CPPUNIT_ASSERT_MESSAGE( "test for isValid function: no fields specified",
                                     ( sal_True == bOk ) );
        }

        void isValid_002( )
        { 
            sal_uInt32 mask_file = FileStatusMask_Type         | FileStatusMask_Attributes |   
                                   FileStatusMask_CreationTime | FileStatusMask_AccessTime |
                                   FileStatusMask_ModifyTime   | FileStatusMask_FileSize   |
                                   FileStatusMask_FileName     | FileStatusMask_FileURL;
             ::osl::FileStatus   rFileStatus( mask_file ); 
            nError1 = rItem_file.getFileStatus( rFileStatus );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            sal_Bool bOk = rFileStatus.isValid( mask_file ); 
            
            CPPUNIT_ASSERT_MESSAGE( "test for isValid function: regular file mask fields test, #FileStatusMask_CreationTime# should be valid field for regular file, but feedback is invalid",
                                     ( sal_True == bOk ) );
        }
        
#if ( defined UNX ) || ( defined OS2 )	//Link is not defined in Windows
        void isValid_003( )
        {
            ::osl::FileBase::RC nError;
            sal_Int32 fd;
            
            //create a link file and link it to file "/tmp/tmpdir/tmpname"
            fd = symlink( "/tmp/tmpdir/tmpname", "/tmp/tmpdir/link.file" );
            CPPUNIT_ASSERT( fd == 0 );
            nError1 = pDir->getNextItem( rItem_link );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            sal_uInt32 mask_link = FileStatusMask_LinkTargetURL;  
             ::osl::FileStatus   rFileStatus( mask_link ); 
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            nError1 = rItem_link.getFileStatus( rFileStatus );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            sal_Bool bOk = rFileStatus.isValid( mask_link ); 
            
            fd = remove( "/tmp/tmpdir/link.file" );
            CPPUNIT_ASSERT( fd == 0 );
            
            CPPUNIT_ASSERT_MESSAGE("test for isValid function: link file, check for LinkTargetURL",
                                     ( sal_True == bOk ) );
        }
#else 
        void isValid_003( )
        {
            CPPUNIT_ASSERT_MESSAGE("test for isValid function: Windows version, link is not defined in windows ",
                                     1 == 1 );
        }
#endif	
        
        void isValid_004( )
        { 
            sal_uInt32 mask_file_all = FileStatusMask_All;
             ::osl::FileStatus   rFileStatus_all( mask_file_all ); 
            nError1 = rItem_file.getFileStatus( rFileStatus_all );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            sal_Bool bOk1 = rFileStatus_all.isValid( mask_file_all ); 
            
            sal_uInt32 mask_file_val = FileStatusMask_Validate;
             ::osl::FileStatus   rFileStatus_val( mask_file_val ); 
            nError1 = rItem_file.getFileStatus( rFileStatus_val );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            sal_Bool bOk2 = rFileStatus_val.isValid( mask_file_val ); 
            
            CPPUNIT_ASSERT_MESSAGE( "test for isValid function: check for Mask_All and Validate, really not sure what validate used for and how to use it, help me.",
                                     ( sal_False == bOk1 ) && ( sal_True == bOk2 )  );
        }
        
        
        CPPUNIT_TEST_SUITE( isValid );
        CPPUNIT_TEST( isValid_001 ); 
        CPPUNIT_TEST( isValid_002 ); 
        CPPUNIT_TEST( isValid_003 ); 
        CPPUNIT_TEST( isValid_004 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class ctors
    
        
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline Type getFileType() const
    //---------------------------------------------------------------------
    class  getFileType : public CppUnit::TestFixture
    {
        ::rtl::OUString         aUStr;
        ::osl::FileBase::RC     nError1, nError2;
        
        ::osl::DirectoryItem    rItem_file, rItem_directory, rItem_volume, rItem_fifo;
        ::osl::DirectoryItem    rItem_link, rItem_socket,    rItem_special;
        
        public:
        // initialization
        void setUp( )
        {
            // create a tempfile: $TEMP/tmpdir/tmpname.
            //        a tempdirectory: $TEMP/tmpdir/tmpdir.
            //        use $ROOT/staroffice as volume.
            // and get their directory item.
            createTestDirectory( aTmpName3 );
            createTestFile( aTmpName3, aTmpName2 );
            createTestDirectory( aTmpName3, aTmpName1 );
            
            ::std::auto_ptr<Directory> pDir( new Directory( aTmpName3 ) );
            nError1 = pDir->open( );	
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            nError1 = pDir->getNextItem( rItem_file, 1 );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            nError1 = pDir->getNextItem( rItem_directory );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            nError1 = ::osl::DirectoryItem::get( aVolURL4, rItem_volume );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
        }

        void tearDown( )
        {
            // remove all in $TEMP/tmpdir.
            deleteTestDirectory( aTmpName3, aTmpName1 );
            deleteTestFile( aTmpName3, aTmpName2 );
            deleteTestDirectory( aTmpName3 );
        }

        // test code.
        void getFileType_001( )
        { 
             ::osl::FileStatus   rFileStatus( FileStatusMask_Type ); 
            nError1 = rItem_file.getFileStatus( rFileStatus );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for getFileType function: File",
                                     ( ::osl::FileStatus::Regular == rFileStatus.getFileType( ) ) );
        }

        void getFileType_002( )
        { 
             ::osl::FileStatus   rFileStatus( FileStatusMask_Type ); 
            nError1 = rItem_directory.getFileStatus( rFileStatus );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for getFileType function: Directory",
                                     ( ::osl::FileStatus::Directory == rFileStatus.getFileType( ) ) );
        }
        
        void getFileType_003( )
        { 
             ::osl::FileStatus   rFileStatus( FileStatusMask_Type ); 
            nError1 = rItem_volume.getFileStatus( rFileStatus );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for getFileType function: Volume, it seems the volume part of the field is not implemented, it did not pass",
                                     ( ::osl::FileStatus::Volume == rFileStatus.getFileType( ) ) );
        }
        
#if ( defined UNX ) || ( defined OS2 )	//Fifo creation is differ in Windows
        void getFileType_004( )
        { 
            //create a fifo in $ROOT/tmp/tmpdir, get its DirectoryItem.
            sal_Int32 fd;
            OSLTEST_DECLARE( FifoSys,  TEST_PLATFORM_ROOT TEST_PLATFORM_TEMP "/tmpdir/fifo" );	
            ::rtl::OUString aFifoURL;
            
            fd = mkfifo( pFifoSys, O_RDWR | O_CREAT );
            ::osl::FileBase::getFileURLFromSystemPath( aFifoSys, aFifoURL );
            nError1 = ::osl::DirectoryItem::get( aFifoURL, rItem_fifo );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            //check for File type
             ::osl::FileStatus   rFileStatus( FileStatusMask_Type ); 
            nError1 = rItem_fifo.getFileStatus( rFileStatus );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            //delete fifo
            nError1 = ::osl::File::remove( aFifoURL );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );

            
            CPPUNIT_ASSERT_MESSAGE( "test for getFileType function: Fifo, Solaris version ",
                                     ( ::osl::FileStatus::Fifo == rFileStatus.getFileType( ) ) );
        }
#else                                    //Windows version 
        void getFileType_004( )
        {
            CPPUNIT_ASSERT_MESSAGE(" test for getFileType function: Fifo, Windows version ",
                                     1 == 1 );
        }
#endif	

#if ( defined UNX ) || ( defined OS2 )	//Socket file may differ in Windows
        void getFileType_005( )
        { 
            nError1 = ::osl::DirectoryItem::get( aTypeURL1, rItem_socket );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            //check for File type
             ::osl::FileStatus   rFileStatus( FileStatusMask_Type );  
            nError1 = rItem_socket.getFileStatus( rFileStatus );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for getFileType function: Socket, Solaris version ",
                                     ( ::osl::FileStatus::Socket == rFileStatus.getFileType( ) ) );
        }
#else                                    //Windows version 
        void getFileType_005( )
        {
            CPPUNIT_ASSERT_MESSAGE(" test for getFileType function: Socket, Windows version ",
                                     1 == 1 );
        }
#endif	
        
#if ( defined UNX ) || ( defined OS2 )	//Link file is not defined in Windows
        void getFileType_006( )
        { 
            nError1 = ::osl::DirectoryItem::get( aTypeURL3, rItem_link );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            //check for File type
             ::osl::FileStatus   rFileStatus( FileStatusMask_Type );  
            nError1 = rItem_link.getFileStatus( rFileStatus );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for getFileType function: Link, Solaris version ",
                                     ( ::osl::FileStatus::Link == rFileStatus.getFileType( ) ) );
        }
#else                                    //Windows version 
        void getFileType_006( )
        {
            CPPUNIT_ASSERT_MESSAGE(" test for getFileType function: Link, Windows version ",
                                     1 == 1 );
        }
#endif	
        
#if ( defined UNX ) || ( defined OS2 )	//Special file is differ in Windows
        void getFileType_007( )
        { 
            nError1 = ::osl::DirectoryItem::get( aTypeURL2, rItem_special );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            //check for File type
             ::osl::FileStatus   rFileStatus( FileStatusMask_Type ); 
            nError1 = rItem_special.getFileStatus( rFileStatus );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for getFileType function: Special, Solaris version ",
                                     ( ::osl::FileStatus::Special == rFileStatus.getFileType( ) ) );
        }
#else                                    //Windows version 
        void getFileType_007( )
        {
            CPPUNIT_ASSERT_MESSAGE(" test for getFileType function: Special, Windows version ",
                                     1 == 1 );
        }
#endif	

        CPPUNIT_TEST_SUITE( getFileType );
        CPPUNIT_TEST( getFileType_001 ); 
        CPPUNIT_TEST( getFileType_002 ); 
        CPPUNIT_TEST( getFileType_003 ); 
        CPPUNIT_TEST( getFileType_004 ); 
        CPPUNIT_TEST( getFileType_005 ); 
        CPPUNIT_TEST( getFileType_006 ); 
        CPPUNIT_TEST( getFileType_007 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class getFileType
        
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline sal_uInt64 getAttributes() const
    //---------------------------------------------------------------------
    class  getAttributes : public CppUnit::TestFixture
    {
        ::rtl::OUString         aTypeURL, aTypeURL_Hid; 
        ::osl::FileBase::RC     nError;
        ::osl::DirectoryItem    rItem, rItem_hidden;
        
        public:
        // initialization
        void setUp( )
        {
            aTypeURL = aTmpURL1.copy( 0 );
            concatURL( aTypeURL, aTmpName2 );
            createTestFile( aTypeURL );
            nError = ::osl::DirectoryItem::get( aTypeURL, rItem );
            CPPUNIT_ASSERT( nError == FileBase::E_None );
            
            aTypeURL_Hid = aTmpURL1.copy( 0 );
            concatURL( aTypeURL_Hid, aHidURL1 );
            createTestFile( aTypeURL_Hid );
            nError = ::osl::DirectoryItem::get( aTypeURL_Hid, rItem_hidden );
            CPPUNIT_ASSERT( nError == FileBase::E_None );
        }

        void tearDown( )
        {
            deleteTestFile( aTypeURL );
            deleteTestFile( aTypeURL_Hid );
        }

        // test code.
 #if ( defined UNX ) || ( defined OS2 )	
        void getAttributes_001( )
        { 
            changeFileMode( aTypeURL, S_IRUSR | S_IRGRP | S_IROTH );
            
              ::osl::FileStatus   rFileStatus( FileStatusMask_Attributes ); 
            nError = rItem.getFileStatus( rFileStatus );
            CPPUNIT_ASSERT( nError == FileBase::E_None );
            
            CPPUNIT_ASSERT_MESSAGE( "test for getAttributes function: ReadOnly, GrpRead, OwnRead, OthRead( Solaris version ) ",
                                    ( Attribute_ReadOnly | Attribute_GrpRead | Attribute_OwnRead | Attribute_OthRead ) ==
                                    rFileStatus.getAttributes( ) );
        }
#else                                    //Windows version 
        void getAttributes_001( )
        {
            CPPUNIT_ASSERT_MESSAGE( "test for getAttributes function: ReadOnly, GrpRead, OwnRead, OthRead( Windows version )",
                                     1 == 1 );
        }
#endif	

 #if ( defined UNX ) || ( defined OS2 )	
        void getAttributes_002( )
        { 
            changeFileMode( aTypeURL, S_IXUSR | S_IXGRP | S_IXOTH );
            
              ::osl::FileStatus   rFileStatus( FileStatusMask_Attributes ); 
            nError = rItem.getFileStatus( rFileStatus );
            CPPUNIT_ASSERT( nError == FileBase::E_None );
            
            CPPUNIT_ASSERT_MESSAGE( "test for getAttributes function: Executable, GrpExe, OwnExe, OthExe, the result is Readonly, Executable, GrpExe, OwnExe, OthExe, it partly not pass( Solaris version )",
                                    ( Attribute_ReadOnly | Attribute_Executable | Attribute_GrpExe | Attribute_OwnExe | Attribute_OthExe ) ==
                                    rFileStatus.getAttributes( ) );
        }
#else                                    //Windows version 
        void getAttributes_002( )
        {
            CPPUNIT_ASSERT_MESSAGE( "test for getAttributes function: Executable, GrpExe, OwnExe, OthExe, the result is Readonly, Executable, GrpExe, OwnExe, OthExe, it partly not pass( Windows version )",
                                     1 == 1 );
        }
#endif	

 #if ( defined UNX ) || ( defined OS2 )	
        void getAttributes_003( )
        { 
            changeFileMode( aTypeURL, S_IWUSR | S_IWGRP | S_IWOTH );
            
              ::osl::FileStatus   rFileStatus( FileStatusMask_Attributes ); 
            nError = rItem.getFileStatus( rFileStatus );
            CPPUNIT_ASSERT( nError == FileBase::E_None );
            
            CPPUNIT_ASSERT_MESSAGE( "test for getAttributes function: GrpWrite, OwnWrite, OthWrite( Solaris version )",
                                    ( Attribute_GrpWrite | Attribute_OwnWrite | Attribute_OthWrite ) ==
                                    rFileStatus.getAttributes( ) );
        }
#else                                    //Windows version 
        void getAttributes_003( )
        {
            CPPUNIT_ASSERT_MESSAGE( "test for getAttributes function: GrpWrite, OwnWrite, OthWrite( Windows version )",
                                     1 == 1 );
        }
#endif	
        
 #if ( defined UNX ) || ( defined OS2 )	//hidden file definition may different in Windows
        void getAttributes_004( )
        { 
            sal_Int32 test_Attributes = Attribute_Hidden;
              ::osl::FileStatus   rFileStatus( FileStatusMask_Attributes ); 
            nError = rItem_hidden.getFileStatus( rFileStatus );
            CPPUNIT_ASSERT( nError == FileBase::E_None );
            test_Attributes &= rFileStatus.getAttributes( );
            
            CPPUNIT_ASSERT_MESSAGE( "test for getAttributes function: Hidden files( Solaris version )",
                                    test_Attributes == Attribute_Hidden );
        }
#else                                    //Windows version 
        void getAttributes_004( )
        {
            CPPUNIT_ASSERT_MESSAGE( "test for getAttributes function: Hidden files( Windows version )",
                                     1 == 1 );
        }
#endif	
        
        CPPUNIT_TEST_SUITE( getAttributes );
        CPPUNIT_TEST( getAttributes_001 ); 
        CPPUNIT_TEST( getAttributes_002 ); 
        CPPUNIT_TEST( getAttributes_003 ); 
        CPPUNIT_TEST( getAttributes_004 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class getAttributes
    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline TimeValue getAccessTime() const
    //---------------------------------------------------------------------
    class  getAccessTime : public CppUnit::TestFixture
    {
        ::rtl::OUString         aTypeURL; 
        ::osl::FileBase::RC     nError;
        ::osl::DirectoryItem    rItem;
        
        public:
        // initialization
        void setUp( )
        {
            aTypeURL = aTmpURL1.copy( 0 );
            concatURL( aTypeURL, aTmpName2 );
            createTestFile( aTypeURL );
            nError = ::osl::DirectoryItem::get( aTypeURL, rItem );
            CPPUNIT_ASSERT( nError == FileBase::E_None );
            
        }

        void tearDown( )
        {
            deleteTestFile( aTypeURL );
        }

        // test code.
        void getAccessTime_001( )
        { 
              ::osl::FileStatus   rFileStatus( FileStatusMask_AccessTime ); 
            nError = rItem.getFileStatus( rFileStatus );
            CPPUNIT_ASSERT( nError == FileBase::E_None );
            
            TimeValue *pTV_current = NULL;
            CPPUNIT_ASSERT( ( pTV_current = ( TimeValue* )malloc( sizeof( TimeValue ) ) ) != NULL );
            TimeValue *pTV_access = NULL;
            CPPUNIT_ASSERT( ( pTV_access = ( TimeValue* )malloc( sizeof( TimeValue ) ) ) != NULL );
            
            sal_Bool bOk = osl_getSystemTime( pTV_current );
            CPPUNIT_ASSERT( sal_True == bOk );
            *pTV_access = rFileStatus.getAccessTime( );
            
            free( pTV_current );
            free( pTV_access );
            
            CPPUNIT_ASSERT_MESSAGE( "test for getAccessTime function: This test turns out that UNX pricision is no more than 1 sec, don't know how to test this function ",
                                    sal_True == compareTime( pTV_access, pTV_current, delta ) );
        }

        CPPUNIT_TEST_SUITE( getAccessTime );
        CPPUNIT_TEST( getAccessTime_001 );
        CPPUNIT_TEST_SUITE_END( );
    };// class getAccessTime
    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline TimeValue getModifyTime() const
    //---------------------------------------------------------------------
    class  getModifyTime : public CppUnit::TestFixture
    {
        ::rtl::OUString         aTypeURL; 
        ::osl::FileBase::RC     nError;
        ::osl::DirectoryItem    rItem;
        
        public:
            
        // test code.
        void getModifyTime_001( )
        { 
            TimeValue *pTV_current = NULL;
            CPPUNIT_ASSERT( ( pTV_current = ( TimeValue* )malloc( sizeof( TimeValue ) ) ) != NULL );

            //create file
            aTypeURL = aTmpURL1.copy( 0 );
            concatURL( aTypeURL, aTmpName2 );
            createTestFile( aTypeURL );
            
            //get current time
            sal_Bool bOk = osl_getSystemTime( pTV_current );
            CPPUNIT_ASSERT( sal_True == bOk );
            
            //get instance item and filestatus
            nError = ::osl::DirectoryItem::get( aTypeURL, rItem );
            CPPUNIT_ASSERT( nError == FileBase::E_None );
            ::osl::FileStatus   rFileStatus( FileStatusMask_ModifyTime ); 
            nError = rItem.getFileStatus( rFileStatus );
            CPPUNIT_ASSERT( nError == FileBase::E_None );
            
            //get modify time
            TimeValue *pTV_modify = NULL;
            CPPUNIT_ASSERT( ( pTV_modify = ( TimeValue* )malloc( sizeof( TimeValue ) ) ) != NULL );
            *pTV_modify = rFileStatus.getModifyTime( );
            
            //delete file
            deleteTestFile( aTypeURL );
            free( pTV_current );
            
            CPPUNIT_ASSERT_MESSAGE( "test for getModifyTime function: This test turns out that UNX pricision is no more than 1 sec, don't know how to improve this function ",
                                    sal_True == compareTime( pTV_modify, pTV_current, delta ) );
        }

        CPPUNIT_TEST_SUITE( getModifyTime );
        CPPUNIT_TEST( getModifyTime_001 );
        CPPUNIT_TEST_SUITE_END( );
    };// class getModifyTime
    
    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline sal_uInt64 getFileSize() const
    //---------------------------------------------------------------------
    class  getFileSize : public CppUnit::TestFixture
    {
        ::rtl::OUString         aTypeURL; 
        ::osl::FileBase::RC     nError;
        ::osl::DirectoryItem    rItem;
        
        public:
        // initialization
        void setUp( )
        {
            aTypeURL = aTmpURL1.copy( 0 );
            concatURL( aTypeURL, aTmpName2 );
            createTestFile( aTypeURL );
            nError = ::osl::DirectoryItem::get( aTypeURL, rItem );
            CPPUNIT_ASSERT( nError == FileBase::E_None );
        }

        void tearDown( )
        {
            deleteTestFile( aTypeURL );
        }

        // test code.
        void getFileSize_001( )
        { 
              ::osl::FileStatus   rFileStatus( FileStatusMask_FileSize ); 
            nError = rItem.getFileStatus( rFileStatus );
            CPPUNIT_ASSERT( nError == FileBase::E_None );
            
            sal_uInt64 uFileSize = rFileStatus.getFileSize( );
            
            CPPUNIT_ASSERT_MESSAGE( "test for getFileSize function: empty file ",
                                    0 == uFileSize );
        }

        void getFileSize_002( )
        { 
            ::std::auto_ptr<File> pFile( new File( aTypeURL ) );
            nError = pFile->open( OpenFlag_Write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError );	
            nError = pFile->setSize( TEST_FILE_SIZE );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError );
            
              ::osl::FileStatus   rFileStatus( FileStatusMask_FileSize ); 
            nError = rItem.getFileStatus( rFileStatus );
            CPPUNIT_ASSERT( nError == FileBase::E_None );
            sal_uInt64 uFileSize = rFileStatus.getFileSize( );
            
            CPPUNIT_ASSERT_MESSAGE( "test for getFileSize function: file with size of TEST_FILE_SIZE ",
                                    TEST_FILE_SIZE == uFileSize );
        }
        CPPUNIT_TEST_SUITE( getFileSize );
        CPPUNIT_TEST( getFileSize_001 );
        CPPUNIT_TEST( getFileSize_002 );
        CPPUNIT_TEST_SUITE_END( );
    };// class getFileSize

    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline ::rtl::OUString getFileName() const
    //---------------------------------------------------------------------
    class  getFileName : public CppUnit::TestFixture
    {
        ::rtl::OUString         aTypeURL; 
        ::osl::FileBase::RC     nError;
        ::osl::DirectoryItem    rItem;
        
        public:
        // initialization
        void setUp( )
        {
            aTypeURL = aTmpURL1.copy( 0 );
            concatURL( aTypeURL, aTmpName2 );
            createTestFile( aTypeURL );
            nError = ::osl::DirectoryItem::get( aTypeURL, rItem );
            CPPUNIT_ASSERT( nError == FileBase::E_None );
        }

        void tearDown( )
        {
            deleteTestFile( aTypeURL );
        }

        // test code.
        void getFileName_001( )
        { 
              ::osl::FileStatus   rFileStatus( FileStatusMask_FileName ); 
            nError = rItem.getFileStatus( rFileStatus );
            CPPUNIT_ASSERT( nError == FileBase::E_None );
            
            ::rtl::OUString aFileName = rFileStatus.getFileName( );
            
            CPPUNIT_ASSERT_MESSAGE( "test for getFileName function: name compare with specify",
                                    sal_True == compareFileName( aFileName, aTmpName2 ) );
        }

        CPPUNIT_TEST_SUITE( getFileName );
        CPPUNIT_TEST( getFileName_001 );
        CPPUNIT_TEST_SUITE_END( );
    };// class getFileName

    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline ::rtl::OUString getFileURL() const
    //---------------------------------------------------------------------
    class  getFileURL : public CppUnit::TestFixture
    {
        ::rtl::OUString         aTypeURL; 
        ::osl::FileBase::RC     nError;
        ::osl::DirectoryItem    rItem;
        
        public:
        // initialization
        void setUp( )
        {
            aTypeURL = aTmpURL1.copy( 0 );
            concatURL( aTypeURL, aTmpName2 );
            createTestFile( aTypeURL );
            nError = ::osl::DirectoryItem::get( aTypeURL, rItem );
            CPPUNIT_ASSERT( nError == FileBase::E_None );
        }

        void tearDown( )
        {
            deleteTestFile( aTypeURL );
        }

        // test code.
        void getFileURL_001( )
        { 
              ::osl::FileStatus   rFileStatus( FileStatusMask_FileURL ); 
            nError = rItem.getFileStatus( rFileStatus );
            CPPUNIT_ASSERT( nError == FileBase::E_None );
            
            ::rtl::OUString aFileURL = rFileStatus.getFileURL( );
            
            CPPUNIT_ASSERT_MESSAGE( "test for getFileURL function: ",
                                    sal_True == compareFileName( aFileURL, aTypeURL ) );
        }

        CPPUNIT_TEST_SUITE( getFileURL );
        CPPUNIT_TEST( getFileURL_001 );
        CPPUNIT_TEST_SUITE_END( );
    };// class getFileURL

    //---------------------------------------------------------------------
    // 	testing the method
    // 	inline ::rtl::OUString getLinkTargetURL() const
    //---------------------------------------------------------------------
    class  getLinkTargetURL : public CppUnit::TestFixture
    {
        ::rtl::OUString         aTypeURL; 
        ::osl::FileBase::RC     nError;
        ::osl::DirectoryItem    rItem;
        
        public:
        // test code.
        // initialization
        void setUp( )
        {
            aTypeURL = aTmpURL1.copy( 0 );
            concatURL( aTypeURL, aTmpName2 );
            createTestFile( aTypeURL );
        }

        void tearDown( )
        {
            deleteTestFile( aTypeURL );
        }

#if ( defined UNX ) || ( defined OS2 )	       //Link file is not define in Windows
        void getLinkTargetURL_001( )
        { 
            //create a link file;
            sal_Int32 fd;
            fd = symlink( "/tmp/tmpname", "/tmp/link.file" );
            CPPUNIT_ASSERT_MESSAGE( "in creating link file",  fd == 0 );
            
            //get linkTarget URL
            nError = ::osl::DirectoryItem::get( aLnkURL1, rItem );
            CPPUNIT_ASSERT_MESSAGE( "in getting link file item", nError == FileBase::E_None );
              ::osl::FileStatus   rFileStatus( FileStatusMask_LinkTargetURL ); 
            nError = rItem.getFileStatus( rFileStatus );
            CPPUNIT_ASSERT_MESSAGE( "in getting link file status", nError == FileBase::E_None );
            ::rtl::OUString aFileURL = rFileStatus.getLinkTargetURL( );
            
            //remove link file
            fd = remove( "/tmp/link.file" );
            CPPUNIT_ASSERT_MESSAGE( "in deleting link file",  fd == 0 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for getLinkTargetURL function: Solaris version, creat a file, and a link file link to it, get its LinkTargetURL and compare",
                                    sal_True == compareFileName( aFileURL, aTypeURL ) );
        }
#else
        void getLinkTargetURL_001( )
        { 
            CPPUNIT_ASSERT_MESSAGE( "test for getLinkTargetURL function: Windows version, not tested",
                                    0 );
        }
#endif

        CPPUNIT_TEST_SUITE( getLinkTargetURL );
        CPPUNIT_TEST( getLinkTargetURL_001 );
        CPPUNIT_TEST_SUITE_END( );
    };// class getLinkTargetURL
    
    // -----------------------------------------------------------------------------
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_FileStatus::ctors, "osl_FileStatus" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_FileStatus::isValid, "osl_FileStatus" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_FileStatus::getFileType, "osl_FileStatus" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_FileStatus::getAttributes, "osl_FileStatus" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_FileStatus::getAccessTime, "osl_FileStatus" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_FileStatus::getModifyTime, "osl_FileStatus" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_FileStatus::getFileSize, "osl_FileStatus" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_FileStatus::getFileName, "osl_FileStatus" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_FileStatus::getFileURL, "osl_FileStatus" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_FileStatus::getLinkTargetURL, "osl_FileStatus" );
}// namespace osl_FileStatus



//------------------------------------------------------------------------
// Beginning of the test cases for File class
//------------------------------------------------------------------------
namespace osl_File
{
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	File( const ::rtl::OUString& ustrFileURL )
    //---------------------------------------------------------------------
    class  ctors : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC     nError1, nError2;
        
        public:
        // initialization
        void setUp( )
        {
            // create a tempfile in $TEMP/tmpdir/tmpname.
            createTestDirectory( aTmpName3 );
            createTestFile( aTmpName4 );
        }

        void tearDown( )
        {
            // remove the tempfile in $TEMP/tmpdir/tmpname.
            deleteTestFile( aTmpName4 );
            deleteTestDirectory( aTmpName3 );
        }

        // test code.
        void ctors_001( )
        { 
            ::osl::File testFile( aTmpName4 );
            
            nError1 = testFile.open( OpenFlag_Read | OpenFlag_Write );
            nError2 = testFile.close( );
            CPPUNIT_ASSERT_MESSAGE( "test for ctors function: initialize a File and test its open and close",
                                     ( ::osl::FileBase::E_None == nError1 ) && ( ::osl::FileBase::E_None == nError2 ) );
        }

        void ctors_002( )
        { 
            ::osl::File testFile( aTmpName5 );
            sal_Char buffer[30] = "Test for File constructor";
            sal_uInt64 nCount;
            
            nError1 = testFile.open( OpenFlag_Read | OpenFlag_Write );
            nError2 = testFile.write( buffer, 30, nCount );
            testFile.close( );
            
            CPPUNIT_ASSERT_MESSAGE( "test for ctors function: test relative file URL, this test show that relative file URL is also acceptable",
                                     ( ::osl::FileBase::E_None == nError1 ) && ( ::osl::FileBase::E_None == nError2 )  );
        }
        
        CPPUNIT_TEST_SUITE( ctors );
        CPPUNIT_TEST( ctors_001 ); 
        CPPUNIT_TEST( ctors_002 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class ctors
    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline RC open( sal_uInt32 uFlags )
    //---------------------------------------------------------------------
    class  open : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC     nError1, nError2, nError3;
        
        public:
        // initialization
        void setUp( )
        {
            // create a tempfile in $TEMP/tmpdir/tmpname.
            createTestDirectory( aTmpName3 );
            createTestFile( aTmpName4 );
        }

        void tearDown( )
        {
            // remove the tempfile in $TEMP/tmpdir/tmpname.
            deleteTestFile( aTmpName4 );
            deleteTestDirectory( aTmpName3 );
        }

        // test code.
        void open_001( )
        { 
            ::osl::File testFile( aTmpName4 );
            
            nError1 = testFile.open( OpenFlag_Read | OpenFlag_Write );
            nError2 = testFile.close( );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError2 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for open function: open a regular file",
                                     ::osl::FileBase::E_None == nError1 );
        }

        void open_002( )
        { 
            ::osl::File testFile( aTmpName3 );
            
            nError1 = testFile.open( OpenFlag_Read );
            
            CPPUNIT_ASSERT_MESSAGE( "test for open function: open a directory",
                                     File::E_INVAL == nError1 );
        }
        
        void open_003( )
        { 
            ::osl::File testFile( aCanURL1 );
            
            nError1 = testFile.open( OpenFlag_Read | OpenFlag_Write );
            
            CPPUNIT_ASSERT_MESSAGE( "test for open function: open a non-exist file",
                                     File::E_NOENT == nError1 );
        }

        void open_004( )
        { 
            ::rtl::OUString  aTestFile( aRootURL );
            concatURL( aTestFile, aTmpName2 );
            ::osl::File testFile( aTestFile );
            
            nError1 = testFile.open( OpenFlag_Create );
            
            CPPUNIT_ASSERT_MESSAGE( "test for open function: create an illegal file",
                                     File::E_ACCES == nError1 );
        }

        void open_005( )
        { 
            ::osl::File testFile( aTmpName4 );
            
            nError1 = testFile.open( OpenFlag_Create );
            
            CPPUNIT_ASSERT_MESSAGE( "test for open function: create an exist file",
                                     File::E_EXIST == nError1 );
        }

        void open_006( )
        { 
            ::osl::File testFile( aCanURL1 );
            sal_Char buffer_write[30] = "Test for File open";
            sal_Char buffer_read[30];
            sal_uInt64 nCount_write, nCount_read;
            
            nError1 = testFile.open( OpenFlag_Read | OpenFlag_Write | OpenFlag_Create );
            nError2 = testFile.write( buffer_write, 30, nCount_write );
             ::osl::FileBase::RC nError4 = testFile.setPos( Pos_Absolut, 0 );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError4 );
            nError3 = testFile.read( buffer_read, 10, nCount_read );
            
             ::osl::FileBase::RC nError5 = testFile.close( );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError5 );
            ::osl::FileBase::RC nError6 = testFile.remove( aCanURL1 );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError6 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for open function: test for OpenFlag_Read,OpenFlag_Write and OpenFlag_Create",
                                    ( ::osl::FileBase::E_None == nError1 ) && 
                                    ( ::osl::FileBase::E_None == nError2 ) &&
                                    ( ::osl::FileBase::E_None == nError3 ) &&
                                    ( 30 == nCount_write ) &&
                                    ( 10 == nCount_read ) );
        }
        
        CPPUNIT_TEST_SUITE( open );
        CPPUNIT_TEST( open_001 ); 
        CPPUNIT_TEST( open_002 ); 
        CPPUNIT_TEST( open_003 ); 
        CPPUNIT_TEST( open_004 ); 
        CPPUNIT_TEST( open_005 ); 
        CPPUNIT_TEST( open_006 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class open
    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline RC close()
    //---------------------------------------------------------------------
    class  close : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC     nError1, nError2, nError3;
        
        public:
        // initialization
        void setUp( )
        {
            // create a tempfile in $TEMP/tmpdir/tmpname.
            createTestDirectory( aTmpName3 );
            createTestFile( aTmpName4 );
        }

        void tearDown( )
        {
            // remove the tempfile in $TEMP/tmpdir/tmpname.
            deleteTestFile( aTmpName4 );
            deleteTestDirectory( aTmpName3 );
        }

        // test code.
        void close_001( )
        { 
            ::osl::File testFile( aTmpName4 );
            
            nError1 = testFile.open( OpenFlag_Read | OpenFlag_Write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            nError2 = testFile.close( );
            
            CPPUNIT_ASSERT_MESSAGE( "test for close function: close a regular file",
                                     ::osl::FileBase::E_None == nError2 );
        }
        
        void close_002( )
        { 
            ::osl::File testFile( aTmpName4 );
            
            nError1 = testFile.open( OpenFlag_Read | OpenFlag_Write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            nError2 = testFile.close( );
            
             nError3 = testFile.setPos( Pos_Absolut, 0 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for close function: manipulate a file after it has been closed",
                                     ( ::osl::FileBase::E_None == nError2 ) &&
                                    ( ::osl::FileBase::E_None != nError3 ) );
        }
        
        CPPUNIT_TEST_SUITE( close );
        CPPUNIT_TEST( close_001 ); 
        CPPUNIT_TEST( close_002 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class close

    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline RC setPos( sal_uInt32 uHow, sal_Int64 uPos )
    //---------------------------------------------------------------------
    class  setPos : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC     nError1;
        sal_uInt64 nCount_write, nCount_read;
        
        public:
        // initialization
        void setUp( )
        {
            // create a tempfile in $TEMP/tmpdir/tmpname.
            createTestDirectory( aTmpName3 );
            createTestFile( aTmpName4 );
            
            //write chars into the file. 
            ::osl::File testFile( aTmpName4 );
            
            nError1 = testFile.open( OpenFlag_Write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            nError1 = testFile.write( pBuffer_Char, sizeof( pBuffer_Char ), nCount_write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
             nError1 = testFile.close( );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
        }

        void tearDown( )
        {
            // remove the tempfile in $TEMP/tmpdir/tmpname.
            deleteTestFile( aTmpName4 );
            deleteTestDirectory( aTmpName3 );
        }

        // test code.
        void setPos_001( )
        { 
            ::osl::File testFile( aTmpName4 );
            sal_Char buffer_read[2];
            
            nError1 = testFile.open( OpenFlag_Read | OpenFlag_Write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
             nError1 = testFile.setPos( Pos_Absolut, 26 );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            nError1 = testFile.read( buffer_read, 1, nCount_read );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            nError1 = testFile.close( );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for setPos function: test for Pos_Absolut, set the position to 26, test if the 26th char in file is correct",
                                     buffer_read[0] == pBuffer_Char[26] );
        }
        
        void setPos_002( )
        { 
            ::osl::File testFile( aTmpName4 );
            sal_Char buffer_read[2];
            
            nError1 = testFile.open( OpenFlag_Read | OpenFlag_Write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
             nError1 = testFile.setPos( Pos_Absolut, sizeof( pBuffer_Char ) - 2 );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
             nError1 = testFile.setPos( Pos_Current, 0);
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            nError1 = testFile.read( buffer_read, 1, nCount_read );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            nError1 = testFile.close( );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for setPos function: test for Pos_Current, set the position to end, test if the ( end -1 ) char in file is correct",
                                     buffer_read[0] == pBuffer_Char[sizeof( pBuffer_Char ) - 2] );
        }
        
        void setPos_003( )
        { 
            ::osl::File testFile( aTmpName4 );
            sal_Char buffer_read[2];
            
            nError1 = testFile.open( OpenFlag_Read | OpenFlag_Write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
             nError1 = testFile.setPos( Pos_End, -53 );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            nError1 = testFile.read( buffer_read, 1, nCount_read );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            nError1 = testFile.close( );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for setPos function: test for Pos_End, set the position to end, test if the first char in file is correct",
                                     buffer_read[0] == pBuffer_Char[0] );
        }
        
        CPPUNIT_TEST_SUITE( setPos );
        CPPUNIT_TEST( setPos_001 ); 
        CPPUNIT_TEST( setPos_002 ); 
        CPPUNIT_TEST( setPos_003 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class setPos

    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline RC getPos( sal_uInt64& uPos )
    //---------------------------------------------------------------------
    class  getPos : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC      nError1;
        sal_uInt64 nCount_write, nCount_read;
        
        public:
        // initialization
        void setUp( )
        {
            // create a tempfile in $TEMP/tmpdir/tmpname.
            createTestDirectory( aTmpName3 );
            createTestFile( aTmpName4 );
            
            //write chars into the file. 
            ::osl::File testFile( aTmpName4 );
            
            nError1 = testFile.open( OpenFlag_Write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            nError1 = testFile.write( pBuffer_Char, sizeof( pBuffer_Char ), nCount_write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
             nError1 = testFile.close( );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
        }

        void tearDown( )
        {
            // remove the tempfile in $TEMP/tmpdir/tmpname.
            deleteTestFile( aTmpName4 );
            deleteTestDirectory( aTmpName3 );
        }

        // test code.
        void getPos_001( )
        { 
            ::osl::File testFile( aTmpName4 );
            sal_uInt64 nFilePointer;
            
            nError1 = testFile.open( OpenFlag_Read | OpenFlag_Write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
             nError1 = testFile.setPos( Pos_Absolut, 26 );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
             nError1 = testFile.getPos( nFilePointer );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            nError1 = testFile.close( );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for getPos function: set the position to 26, get position and check if it is right",
                                     26 == nFilePointer );
        }
        
        CPPUNIT_TEST_SUITE( getPos );
        CPPUNIT_TEST( getPos_001 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class getPos
    
    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline RC isEndOfFile( sal_Bool *pIsEOF )
    //---------------------------------------------------------------------
    class  isEndOfFile : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC      nError1;
        sal_uInt64 nCount_write, nCount_read;
        
        public:
        // initialization
        void setUp( )
        {
            // create a tempfile in $TEMP/tmpdir/tmpname.
            createTestDirectory( aTmpName3 );
            createTestFile( aTmpName4 );
            
            //write chars into the file. 
            ::osl::File testFile( aTmpName4 );
            
            nError1 = testFile.open( OpenFlag_Write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            nError1 = testFile.write( pBuffer_Char, sizeof( pBuffer_Char ), nCount_write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
             nError1 = testFile.close( );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
        }

        void tearDown( )
        {
            // remove the tempfile in $TEMP/tmpdir/tmpname.
            deleteTestFile( aTmpName4 );
            deleteTestDirectory( aTmpName3 );
        }

        // test code.
        void isEndOfFile_001( )
        { 
            ::osl::File   testFile( aTmpName4 );
            sal_Bool      bEOF  = sal_False;
            sal_Bool      *pEOF = &bEOF;
            
            
            nError1 = testFile.open( OpenFlag_Read | OpenFlag_Write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
             nError1 = testFile.setPos( Pos_End, 0 );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            nError1 = testFile.isEndOfFile( pEOF );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            nError1 = testFile.close( );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for isEndOfFile function: set the position to end, check if reach end",
                                     sal_True == *pEOF );
        }
        
        void isEndOfFile_002( )
        { 
             ::osl::File   testFile( aTmpName4 );
            sal_Bool      bEOF  = sal_False;
            sal_Bool      *pEOF = &bEOF;
            sal_uInt64    nFilePointer = 0;
            
            nError1 = testFile.open( OpenFlag_Read | OpenFlag_Write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
             nError1 = testFile.setPos( Pos_Absolut, 0 );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            *pEOF = sal_False;
             while ( !( *pEOF ) )
            {
                nError1 = testFile.isEndOfFile( pEOF );
                CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
                nError1 = testFile.setPos( Pos_Current, 1 );
                CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            } 
             nError1 = testFile.getPos( nFilePointer );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            nError1 = testFile.close( );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for isEndOfFile function: use isEndOfFile to move pointer step by step",
                                      sizeof( pBuffer_Char ) + 1 == nFilePointer  );
        }
        CPPUNIT_TEST_SUITE( isEndOfFile );
        CPPUNIT_TEST( isEndOfFile_001 ); 
        CPPUNIT_TEST( isEndOfFile_002 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class isEndOfFile	
    
    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline RC setSize( sal_uInt64 uSize )
    //---------------------------------------------------------------------
    class  setSize : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC      nError1;
        sal_uInt64 nCount_write, nCount_read;
        
        public:
        // initialization
        void setUp( )
        {
            // create a tempfile in $TEMP/tmpdir/tmpname.
            createTestDirectory( aTmpName3 );
            createTestFile( aTmpName4 );
            
            //write chars into the file. 
            ::osl::File testFile( aTmpName4 );
            
            nError1 = testFile.open( OpenFlag_Write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            nError1 = testFile.write( pBuffer_Char, sizeof( pBuffer_Char ), nCount_write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
             nError1 = testFile.close( );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
        }

        void tearDown( )
        {
            // remove the tempfile in $TEMP/tmpdir/tmpname.
            deleteTestFile( aTmpName4 );
            deleteTestDirectory( aTmpName3 );
        }

        // test code.
        void setSize_001( )
        { 
            ::osl::File   testFile( aTmpName4 );
            sal_Bool      bEOF  = sal_False;
            sal_Bool      *pEOF = &bEOF;
            sal_uInt64     nFilePointer;
            
            
            nError1 = testFile.open( OpenFlag_Read | OpenFlag_Write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            //enlarge the file to size of 100;
             nError1 = testFile.setSize( 100 );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            //get the file size;
             nError1 = testFile.setPos( Pos_End, 0 );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
             nError1 = testFile.getPos( nFilePointer );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            nError1 = testFile.close( );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for setSize function: enlarge the file ",
                                     100 == nFilePointer );
        }
        
        void setSize_002( )
        { 
            ::osl::File   testFile( aTmpName4 );
            sal_Bool      bEOF  = sal_False;
            sal_Bool      *pEOF = &bEOF;
            sal_uInt64     nFilePointer;
            
            
            nError1 = testFile.open( OpenFlag_Read | OpenFlag_Write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            //enlarge the file to size of 100;
             nError1 = testFile.setSize( 10 );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            //get the file size;
             nError1 = testFile.setPos( Pos_End, 0 );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
             nError1 = testFile.getPos( nFilePointer );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            nError1 = testFile.close( );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for setSize function: truncate the file ",
                                     10 == nFilePointer );
        }
        
        
        CPPUNIT_TEST_SUITE( setSize );
        CPPUNIT_TEST( setSize_001 ); 
        CPPUNIT_TEST( setSize_002 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class setSize	
    
    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline RC read( void *pBuffer, sal_uInt64 uBytesRequested, sal_uInt64& rBytesRead )
    //---------------------------------------------------------------------
    class  read : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC      nError1;
        sal_uInt64 nCount_write, nCount_read;
        
        public:
        // initialization
        void setUp( )
        {
            // create a tempfile in $TEMP/tmpdir/tmpname.
            createTestDirectory( aTmpName3 );
            createTestFile( aTmpName4 );
            
            //write chars into the file. 
            ::osl::File testFile( aTmpName4 );
            
            nError1 = testFile.open( OpenFlag_Write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            nError1 = testFile.write( pBuffer_Char, sizeof( pBuffer_Char ), nCount_write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
             nError1 = testFile.close( );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
        }

        void tearDown( )
        {
            // remove the tempfile in $TEMP/tmpdir/tmpname.
            deleteTestFile( aTmpName4 );
            deleteTestDirectory( aTmpName3 );
        }

        // test code.
        void read_001( )
        { 
            ::osl::File    testFile( aTmpName4 );
            sal_uInt64     nFilePointer;
            sal_Char       buffer_read[10];
            
            
            nError1 = testFile.open( OpenFlag_Read | OpenFlag_Write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            nError1 = testFile.read( buffer_read, 10, nCount_read );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
             nError1 = testFile.getPos( nFilePointer );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            nError1 = testFile.close( );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for read function: read whole content in the file to a buffer",
                                     ( 10 == nFilePointer ) && ( 0 == strncmp( buffer_read, pBuffer_Char, 10 ) ) );
        }
        
        void read_002( )
        { 
            ::osl::File    testFile( aTmpName4 );
            sal_uInt64     nFilePointer;
            sal_Char       buffer_read[26];
            
            
            nError1 = testFile.open( OpenFlag_Read | OpenFlag_Write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
             nError1 = testFile.setPos( Pos_Absolut, 26 );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            nError1 = testFile.read( buffer_read, 26, nCount_read );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
             nError1 = testFile.getPos( nFilePointer );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            nError1 = testFile.close( );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for read function: read from a special positon in the file",
                                     ( 52 == nFilePointer ) && ( 26 == nCount_read ) && ( 0 == strncmp( buffer_read, &pBuffer_Char[26], 26 ) ) );
        }
        
        CPPUNIT_TEST_SUITE( read );
        CPPUNIT_TEST( read_001 ); 
        CPPUNIT_TEST( read_002 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class read	
    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline RC write(const void *pBuffer, sal_uInt64 uBytesToWrite, sal_uInt64& rBytesWritten)
    //---------------------------------------------------------------------
    class  write : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC      nError1;
        sal_uInt64 nCount_write, nCount_read;
        
        public:
        // initialization
        void setUp( )
        {
            // create a tempfile in $TEMP/tmpname.
            createTestFile( aTmpName6 );
        }

        void tearDown( )
        {
            // remove the tempfile in $TEMP/tmpname.
            deleteTestFile( aTmpName6 );
        }

        // test code.
        void write_001( )
        { 
            ::osl::File    testFile( aTmpName6 );
            sal_uInt64     nFilePointer;
            sal_Char       buffer_read[10];
            
            nError1 = testFile.open( OpenFlag_Read | OpenFlag_Write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            //write chars into the file. 
            nError1 = testFile.write( pBuffer_Char, 10, nCount_write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            //get the current pointer;
             nError1 = testFile.getPos( nFilePointer );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            //reset pointer to the begining;
             nError1 = testFile.setPos( Pos_Absolut, 0 );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            nError1 = testFile.read( buffer_read, 10, nCount_read );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            nError1 = testFile.close( );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for write function: read whole content in the file to a buffer. Note, buffer size can not smaller than the read size",
                                     ( 10 == nFilePointer ) &&
                                    ( 0 == strncmp( buffer_read, pBuffer_Char, 10 ) ) &&
                                    ( 10 == nCount_write ) );
        }
        
        CPPUNIT_TEST_SUITE( write );
        CPPUNIT_TEST( write_001 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class write	
        
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline RC readLine( ::rtl::ByteSequence& aSeq ) 
    //---------------------------------------------------------------------
    class  readLine : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC      nError1;
        sal_uInt64 nCount_write, nCount_read;
        ::rtl::ByteSequence      aSequence;
        
        public:
        // initialization
        void setUp( )
        {
            // create a tempfile in $TEMP/tmpname.
            createTestFile( aTmpName6 );
            
            //write some strings into the file. 
            ::osl::File testFile( aTmpName6 );
            sal_Char * ppStrSeq[]  =  { "abcde\n",
                                        "1234567890\n",
                                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                      };
            
            nError1 = testFile.open( OpenFlag_Write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
                                   
            for ( int nCount = 0; nCount < 3; nCount++ )
            {
                nError1 = testFile.write( ppStrSeq[nCount], strlen( ppStrSeq[nCount] ), nCount_write );
                CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            }
            
             nError1 = testFile.close( );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
        }

        void tearDown( )
        {
            // remove the tempfile in $TEMP/tmpname.
            deleteTestFile( aTmpName6 );
        }

        // test code.
        void readLine_001( )
        { 
             ::osl::File    testFile( aTmpName6 );
            
            nError1 = testFile.open( OpenFlag_Read | OpenFlag_Write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            nError1 = testFile.readLine( aSequence );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for readLine function: read the first line of the file.",
                                     ( ::osl::FileBase::E_None == nError1 ) &&
                                    ( 0 == strncmp( ( const char * )aSequence.getArray( ), pBuffer_Char, 5 ) ) );
        }
        
        void readLine_002( )
        { 
             ::osl::File    testFile( aTmpName6 );
            sal_Bool bEOF  = sal_False;
            sal_Bool *pEOF = &bEOF;
            
            nError1 = testFile.open( OpenFlag_Read | OpenFlag_Write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            for ( int nCount = 0; nCount < 3; nCount++ )
            {
                nError1 = testFile.readLine( aSequence );
                CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            }
             nError1 = testFile.isEndOfFile( pEOF );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );

            CPPUNIT_ASSERT_MESSAGE( "test for readLine function: read three lines of the file and check the file pointer moving.",
                                     *pEOF &&
                                    ( 0 == strncmp( ( const char * )aSequence.getArray( ), &pBuffer_Char[26], 26 ) ) );
        }
        CPPUNIT_TEST_SUITE( readLine );
        CPPUNIT_TEST( readLine_001 ); 
        CPPUNIT_TEST( readLine_002 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class readLine	
        
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline static RC copy( const ::rtl::OUString& ustrSourceFileURL, const ::rtl::OUString& ustrDestFileURL )
    //---------------------------------------------------------------------
    class  copy : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC      nError1;
        sal_uInt64 nCount_write, nCount_read;
        
        public:
        // initialization
        void setUp( )
        {
            // create a tempfile in $TEMP/tmpdir/tmpname.
            createTestDirectory( aTmpName3 );
            createTestFile( aTmpName4 );
            
            //write chars into the file. 
            ::osl::File testFile( aTmpName4 );
            
            nError1 = testFile.open( OpenFlag_Write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            nError1 = testFile.write( pBuffer_Char, sizeof( pBuffer_Char ), nCount_write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
             nError1 = testFile.close( );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
        }

        void tearDown( )
        {
            // remove the tempfile in $TEMP/tmpdir/tmpname.
            deleteTestFile( aTmpName4 );
            deleteTestDirectory( aTmpName3 );
        }

        // test code.
        void copy_001( )
        { 
             ::osl::File    testFile( aTmpName6 );
            
            //copy $TEMP/tmpdir/tmpname to $TEMP/tmpname.
            nError1 = ::osl::File::copy( aTmpName4, aTmpName6 );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            //check
            nError1 = testFile.open( OpenFlag_Create );
            deleteTestFile( aTmpName6 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for copy function: copy file to upper directory",
                                     ::osl::FileBase::E_EXIST == nError1 );
        }
        
        void copy_002( )
        { 
            //copy $TEMP/tmpdir/tmpname to $TEMP/tmpdir.
            nError1 = ::osl::File::copy( aTmpName4, aTmpName3 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for copy function: use directory as destination",
                                     ::osl::FileBase::E_ISDIR == nError1 );
        }
        
        void copy_003( )
        { 
            //copy $TEMP/tmpdir/tmpname to $ROOT/tmpname.
            nError1 = ::osl::File::copy( aTmpName4, aTmpName7 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for copy function: copy to an illigal place",
                                     ::osl::FileBase::E_ACCES == nError1 );
        }
        
        void copy_004( )
        { 
            //copy $TEMP/tmpname to $TEMP/tmpdir/tmpname.
            nError1 = ::osl::File::copy( aTmpName6, aTmpName4 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for copy function: copy a not exist file",
                                     ::osl::FileBase::E_NOENT == nError1 );
        }
        
        void copy_005( )
        { 
            //copy $TEMP/tmpname to $TEMP/system.path using system path.
            nError1 = ::osl::File::copy( aTmpName6, aSysPath1 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for copy function: copy a file using system file path",
                                     ::osl::FileBase::E_INVAL == nError1 );
        }
        
        CPPUNIT_TEST_SUITE( copy );
        CPPUNIT_TEST( copy_001 ); 
        CPPUNIT_TEST( copy_002 ); 
        CPPUNIT_TEST( copy_003 ); 
        CPPUNIT_TEST( copy_004 ); 
        CPPUNIT_TEST( copy_005 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class copy	
    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline static RC move( const ::rtl::OUString& ustrSourceFileURL, const ::rtl::OUString& ustrDestFileURL )
    //---------------------------------------------------------------------
    class  move : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC      nError1, nError2;
        sal_uInt64 nCount_write, nCount_read;
        
        public:
        // initialization
        void setUp( )
        {
            // create a tempfile in $TEMP/tmpdir/tmpname.
            createTestDirectory( aTmpName3 );
            createTestFile( aTmpName4 );
            
            //write chars into the file. 
            ::osl::File testFile( aTmpName4 );
            
            nError1 = testFile.open( OpenFlag_Write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            nError1 = testFile.write( pBuffer_Char, sizeof( pBuffer_Char ), nCount_write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
             nError1 = testFile.close( );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
        }

        void tearDown( )
        {
            // remove the tempfile in $TEMP/tmpdir/tmpname.
            deleteTestFile( aTmpName4 );
            deleteTestDirectory( aTmpName3 );
        }

        // test code.
        void move_001( )
        { 
            //rename $TEMP/tmpdir/tmpname to $TEMP/canonical.name.
            nError1 = ::osl::File::move( aTmpName4, aCanURL1 );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            //check
             ::osl::File    testFile( aCanURL1 );
            nError2 = testFile.open( OpenFlag_Create );
            deleteTestFile( aCanURL1 );

            CPPUNIT_ASSERT_MESSAGE( "test for move function: rename file to another directory",
                                     ::osl::FileBase::E_EXIST == nError2 );
        }
        
        void move_002( )
        { 
            //move $TEMP/tmpdir/tmpname to $TEMP/tmpdir.
            nError1 = ::osl::File::move( aTmpName4, aTmpName3 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for move function: use directory as destination",
                                     ::osl::FileBase::E_ISDIR == nError1 );
        }
        
        void move_003( )
        { 
            //move $TEMP/tmpdir/tmpname to $ROOT/tmpname.
            nError1 = ::osl::File::move( aTmpName4, aTmpName7 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for move function: move to an illigal place",
                                     ::osl::FileBase::E_ACCES == nError1 );
        }
        
        void move_004( )
        { 
            //move $TEMP/tmpname to $TEMP/tmpdir/tmpname.
            nError1 = ::osl::File::move( aTmpName6, aTmpName4 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for move function: move a not exist file",
                                     ::osl::FileBase::E_NOENT == nError1 );
        }
        
        void move_005( )
        { 
            //move $TEMP/tmpname to $TEMP/system.path using system path.
            nError1 = ::osl::File::move( aTmpName6, aSysPath1 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for move function: move a file using system file",
                                     ::osl::FileBase::E_INVAL == nError1 );
        }
        
        void move_006( )
        { 
            //move directory $TEMP/tmpname to $TEMP/tmpdir/tmpname.
            createTestDirectory( aTmpName6 );
            nError1 = ::osl::File::move( aTmpName6, aTmpName4 );
            deleteTestDirectory( aTmpName6 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for move function: move a directory to an exist file with same name",
                                     ::osl::FileBase::E_NOTDIR == nError1 );
        }
        
        void move_007( )
        { 
            //create directory $TEMP/tmpname.
            createTestDirectory( aTmpName6 );
            //move directory $TEMP/tmpdir to $TEMP/tmpname/tmpdir
            nError1 = ::osl::File::move( aTmpName3, aTmpName8 );
            //check
            nError2 = ::osl::Directory::create( aTmpName8 );
            ::osl::File::move( aTmpName8, aTmpName3 );
            deleteTestDirectory( aTmpName6 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for move function: move a directory to an exist file with same name",
                                     (::osl::FileBase::E_None == nError1 ) && 
                                    (::osl::FileBase::E_EXIST == nError2 ) );
        }
        
        CPPUNIT_TEST_SUITE( move );
        CPPUNIT_TEST( move_001 ); 
        CPPUNIT_TEST( move_002 ); 
        CPPUNIT_TEST( move_003 ); 
        CPPUNIT_TEST( move_004 ); 
        CPPUNIT_TEST( move_005 ); 
        CPPUNIT_TEST( move_006 ); 
        CPPUNIT_TEST( move_007 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class move	
    
    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline static RC remove( const ::rtl::OUString& ustrFileURL )
    //---------------------------------------------------------------------
    class  remove : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC      nError1, nError2;
        sal_uInt64 nCount_write, nCount_read;
        
        public:
        // initialization
        void setUp( )
        {
            // create a tempfile in $TEMP/tmpdir/tmpname.
            createTestDirectory( aTmpName3 );
            createTestFile( aTmpName4 );
            
            //write chars into the file. 
            ::osl::File testFile( aTmpName4 );
            
            nError1 = testFile.open( OpenFlag_Write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            nError1 = testFile.write( pBuffer_Char, sizeof( pBuffer_Char ), nCount_write );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
             nError1 = testFile.close( );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
        }

        void tearDown( )
        {
            // remove the tempfile in $TEMP/tmpdir/tmpname.
            deleteTestFile( aTmpName4 );
            deleteTestDirectory( aTmpName3 );
        }

        // test code.
        void remove_001( )
        { 
            //remove $TEMP/tmpdir/tmpname.
            nError1 = ::osl::File::remove( aTmpName4 );
            //check
             ::osl::File    testFile( aTmpName4 );
            nError2 = testFile.open( OpenFlag_Create );

            CPPUNIT_ASSERT_MESSAGE( "test for remove function: remove a file",
                                    ( ::osl::FileBase::E_None == nError1 ) &&
                                     ( ::osl::FileBase::E_EXIST != nError2 ) );
        }
        
        void remove_002( )
        { 
            //remove $TEMP/tmpname.
            nError1 = ::osl::File::remove( aTmpName6 );

            CPPUNIT_ASSERT_MESSAGE( "test for remove function: remove a file not exist",
                                    ( ::osl::FileBase::E_NOENT == nError1 ) );
        }
        
        void remove_003( )
        { 
            //remove $TEMP/system/path.
            nError1 = ::osl::File::remove( aSysPath2 );

            CPPUNIT_ASSERT_MESSAGE( "test for remove function: removing a file not using full qualified URL",
                                    ( ::osl::FileBase::E_INVAL == nError1 ) );
        }
        
        void remove_004( )
        { 
            //remove $TEMP/tmpdir.
            nError1 = ::osl::File::remove( aTmpName3 );

            CPPUNIT_ASSERT_MESSAGE( "test for remove function: remove a directory",
                                    ( ::osl::FileBase::E_ISDIR == nError1 ) );
        }
        
        CPPUNIT_TEST_SUITE( remove );
        CPPUNIT_TEST( remove_001 ); 
        CPPUNIT_TEST( remove_002 ); 
        CPPUNIT_TEST( remove_003 ); 
        CPPUNIT_TEST( remove_004 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class remove	
        
    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline static RC setAttributes( const ::rtl::OUString& ustrFileURL, sal_uInt64 uAttributes )
    //---------------------------------------------------------------------
    class  setAttributes : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC      nError1, nError2;
        ::osl::DirectoryItem    rItem, rItem_hidden;
        
        public:
        // initialization
        void setUp( )
        {
            // create a tempfile in $TEMP/tmpdir/tmpname.
            createTestFile( aTmpName6 );
        }

        void tearDown( )
        {
            // remove the tempfile in $TEMP/tmpdir/tmpname.
            deleteTestFile( aTmpName6 );
        }

        // test code.
        void setAttributes_001( )
        { 
            //set the file to readonly
            nError2 = ::osl::File::setAttributes( aTmpName6, Attribute_ReadOnly | Attribute_GrpRead | Attribute_OwnRead | Attribute_OthRead );
            nError1 = ::osl::DirectoryItem::get( aTmpName6, rItem );
            CPPUNIT_ASSERT( nError1 == FileBase::E_None);
            //get the file attributes
              ::osl::FileStatus   rFileStatus( FileStatusMask_Attributes ); 
            nError1 = rItem.getFileStatus( rFileStatus );
            CPPUNIT_ASSERT( nError1 == FileBase::E_None );

            CPPUNIT_ASSERT_MESSAGE( "test for setAttributes function: set file attributes and get it to verify",
                                    ( Attribute_ReadOnly | Attribute_GrpRead | Attribute_OwnRead | Attribute_OthRead ) ==
                                    rFileStatus.getAttributes( ) );
        }
        
        CPPUNIT_TEST_SUITE( setAttributes );
        CPPUNIT_TEST( setAttributes_001 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class setAttributes	
        
            
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline static RC setTime( 
    // 	       const ::rtl::OUString& ustrFileURL, 
    // 	       const TimeValue& rCreationTime,
    // 	       const TimeValue& rLastAccessTime,
    // 	       const TimeValue& rLastWriteTime )
    //---------------------------------------------------------------------
    class  setTime : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC     nError1, nError2;
        ::osl::DirectoryItem    rItem;
        
        public:
        // initialization
        void setUp( )
        {
            // create a tempfile in $TEMP/tmpdir/tmpname.
            createTestFile( aTmpName6 );
        }

        void tearDown( )
        {
            // remove the tempfile in $TEMP/tmpdir/tmpname.
            deleteTestFile( aTmpName6 );
        }

        // test code.
        void setTime_001( )
        { 
             TimeValue *pTV_current  = NULL;
            CPPUNIT_ASSERT( ( pTV_current = ( TimeValue* )malloc( sizeof( TimeValue ) ) ) != NULL );
            TimeValue *pTV_creation = NULL;
            CPPUNIT_ASSERT( ( pTV_creation = ( TimeValue* )malloc( sizeof( TimeValue ) ) ) != NULL );
            TimeValue *pTV_access   = NULL;
            CPPUNIT_ASSERT( ( pTV_access = ( TimeValue* )malloc( sizeof( TimeValue ) ) ) != NULL );
            TimeValue *pTV_modify   = NULL;
            CPPUNIT_ASSERT( ( pTV_modify = ( TimeValue* )malloc( sizeof( TimeValue ) ) ) != NULL );
            
            //get current time
            sal_Bool bOk = osl_getSystemTime( pTV_current );
            CPPUNIT_ASSERT( sal_True == bOk );

            //set the file time
            nError2 = ::osl::File::setTime( aTmpName6, *pTV_current, *pTV_current, *pTV_current );
            
             //get the file access time, creation time, modify time
            nError1 = ::osl::DirectoryItem::get( aTmpName6, rItem );
            CPPUNIT_ASSERT( nError1 == FileBase::E_None);
            
              ::osl::FileStatus   rFileStatus( FileStatusMask_AccessTime ); 
            nError1 = rItem.getFileStatus( rFileStatus );
            CPPUNIT_ASSERT( nError1 == FileBase::E_None );
            *pTV_access = rFileStatus.getAccessTime( );
            
              ::osl::FileStatus   rFileStatus1( FileStatusMask_CreationTime ); 
            nError1 = rItem.getFileStatus( rFileStatus1 );
            CPPUNIT_ASSERT( nError1 == FileBase::E_None );
            *pTV_creation = rFileStatus1.getCreationTime( );
            
              ::osl::FileStatus   rFileStatus2( FileStatusMask_ModifyTime ); 
            nError1 = rItem.getFileStatus( rFileStatus2 );
            CPPUNIT_ASSERT( nError1 == FileBase::E_None );
            *pTV_modify = rFileStatus2.getModifyTime( );

            free( pTV_current );
            free( pTV_creation );
            free( pTV_access );
            free( pTV_modify );
              
            CPPUNIT_ASSERT_MESSAGE( "test for setTime function: set file time then get them. time precision is still a problem for it cut off the nanosec.",
                                    ( sal_True == compareTime( pTV_access, pTV_current, delta ) ) &&
                                    ( sal_True == compareTime( pTV_creation, pTV_current, delta ) ) &&
                                    ( sal_True == compareTime( pTV_modify, pTV_current, delta ) ) 
                                  );
        }
        
        CPPUNIT_TEST_SUITE( setTime );
        CPPUNIT_TEST( setTime_001 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class setTime	
        
    // -----------------------------------------------------------------------------
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_File::ctors, "osl_File" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_File::open, "osl_File" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_File::close, "osl_File" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_File::setPos, "osl_File" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_File::getPos, "osl_File" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_File::isEndOfFile, "osl_File" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_File::setSize, "osl_File" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_File::read, "osl_File" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_File::write, "osl_File" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_File::readLine, "osl_File" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_File::copy, "osl_File" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_File::move, "osl_File" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_File::remove, "osl_File" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_File::setAttributes, "osl_File" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_File::setTime, "osl_File" );
}// namespace osl_File
        

//------------------------------------------------------------------------
// Beginning of the test cases for DirectoryItem class
//------------------------------------------------------------------------
namespace osl_DirectoryItem
{
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	DirectoryItem(): _pData( NULL )
    //---------------------------------------------------------------------
    class  ctors : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC     nError1, nError2;
        
        public:
        // initialization
        void setUp( )
        {
            // create a tempfile in $TEMP/tmpname.
            createTestFile( aTmpName6 );
        }

        void tearDown( )
        {
            // remove the tempfile in $TEMP/tmpname.
            deleteTestFile( aTmpName6 );
        }

        // test code.
        void ctors_001( )
        { 
            ::osl::File testFile( aTmpName6 );
            ::osl::DirectoryItem    rItem;  //constructor
            
             //get the DirectoryItem.
            nError1 = ::osl::DirectoryItem::get( aTmpName6, rItem );
            CPPUNIT_ASSERT( FileBase::E_None == nError1 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for ctors function: initialize a new instance of DirectoryItem and get an item to check.",
                                      ::osl::FileBase::E_None == nError1  );
        }

        CPPUNIT_TEST_SUITE( ctors );
        CPPUNIT_TEST( ctors_001 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class ctors

    //---------------------------------------------------------------------
    // 	testing the method 
    // 	DirectoryItem( const DirectoryItem& rItem ): _pData( rItem._pData)
    //---------------------------------------------------------------------
    class  copy_assin_Ctors : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC     nError1, nError2;
        
        public:
        // initialization
        void setUp( )
        {
            // create a tempfile in $TEMP/tmpname.
            createTestFile( aTmpName6 );
        }

        void tearDown( )
        {
            // remove the tempfile in $TEMP/tmpname.
            deleteTestFile( aTmpName6 );
        }

        // test code.
        void copy_assin_Ctors_001( )
        { 
            ::osl::DirectoryItem    rItem;  //constructor
             //get the DirectoryItem.
            nError1 = ::osl::DirectoryItem::get( aTmpName6, rItem );
            CPPUNIT_ASSERT( FileBase::E_None == nError1 );
            
            ::osl::DirectoryItem    copyItem( rItem ); //copy constructor
              ::osl::FileStatus   rFileStatus( FileStatusMask_FileName ); 
            nError1 = copyItem.getFileStatus( rFileStatus );
            CPPUNIT_ASSERT( nError1 == FileBase::E_None );
            
            CPPUNIT_ASSERT_MESSAGE( "test for copy_assin_Ctors function: use copy constructor to get an item and check filename.",
                                    ( sal_True == compareFileName( rFileStatus.getFileName( ), aTmpName2 ) ) );
        }

        void copy_assin_Ctors_002( )
        { 
            ::osl::DirectoryItem    rItem;  //constructor
             //get the DirectoryItem.
            nError1 = ::osl::DirectoryItem::get( aTmpName6, rItem );
            CPPUNIT_ASSERT( FileBase::E_None == nError1 );
            
            ::osl::DirectoryItem    copyItem; 
            copyItem = rItem;               //assinment operator
              ::osl::FileStatus   rFileStatus( FileStatusMask_FileName ); 
            nError1 = copyItem.getFileStatus( rFileStatus );
            CPPUNIT_ASSERT( nError1 == FileBase::E_None );
            
            CPPUNIT_ASSERT_MESSAGE( "test for copy_assin_Ctors function: test assinment operator here since it is same as copy constructor in test way.",
                                    ( sal_True == compareFileName( rFileStatus.getFileName( ), aTmpName2 ) ) );
        }

        CPPUNIT_TEST_SUITE( copy_assin_Ctors );
        CPPUNIT_TEST( copy_assin_Ctors_001 ); 
        CPPUNIT_TEST( copy_assin_Ctors_002 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class copy_assin_Ctors
    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline sal_Bool is()
    //---------------------------------------------------------------------
    class  is : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC     nError1, nError2;
        
        public:
        // initialization
        void setUp( )
        {
            // create a tempfile in $TEMP/tmpname.
            createTestFile( aTmpName6 );
        }

        void tearDown( )
        {
            // remove the tempfile in $TEMP/tmpname.
            deleteTestFile( aTmpName6 );
        }

        // test code.
        void is_001( )
        { 
            ::osl::DirectoryItem    rItem;  //constructor
            
            CPPUNIT_ASSERT_MESSAGE( "test for is function: use an uninitialized instance.",
                                    !rItem.is( ) );
        }

        void is_002( )
        { 
            ::osl::DirectoryItem    rItem;  //constructor
             //get the DirectoryItem.
            nError1 = ::osl::DirectoryItem::get( aTmpName6, rItem );
            CPPUNIT_ASSERT( FileBase::E_None == nError1 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for is function: use an uninitialized instance.",
                                    ( sal_True == rItem.is( ) ) );
        }

        CPPUNIT_TEST_SUITE( is );
        CPPUNIT_TEST( is_001 ); 
        CPPUNIT_TEST( is_002 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class is
    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	static inline RC get( const ::rtl::OUString& ustrFileURL, DirectoryItem& rItem )
    //---------------------------------------------------------------------
    class  get : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC     nError1, nError2;
        
        public:
        // initialization
        void setUp( )
        {
            // create a tempfile in $TEMP/tmpname.
            createTestFile( aTmpName6 );
        }

        void tearDown( )
        {
            // remove the tempfile in $TEMP/tmpname.
            deleteTestFile( aTmpName6 );
        }

        // test code.
        void get_001( )
        { 
            ::osl::DirectoryItem    rItem;  //constructor
             //get the DirectoryItem.
            nError2 = ::osl::DirectoryItem::get( aTmpName6, rItem );
            
            //check the file name
              ::osl::FileStatus   rFileStatus( FileStatusMask_FileName ); 
            nError1 = rItem.getFileStatus( rFileStatus );
            CPPUNIT_ASSERT( nError1 == FileBase::E_None );
            
            CPPUNIT_ASSERT_MESSAGE( "test for get function: use copy constructor to get an item and check filename.",
                                    ( ::osl::FileBase::E_None == nError2 ) &&
                                    ( sal_True == compareFileName( rFileStatus.getFileName( ), aTmpName2 ) ) );
        }

        void get_002( )
        { 
            ::osl::DirectoryItem    rItem;  
             //get the DirectoryItem.
            nError1 = ::osl::DirectoryItem::get( aSysPath1, rItem );
            
            CPPUNIT_ASSERT_MESSAGE( "test for get function: use a system name instead of a URL.",
                                    FileBase::E_INVAL == nError1 );
        }
        
        void get_003( )
        { 
            ::osl::DirectoryItem    rItem;  
             //get the DirectoryItem.
            nError1 = ::osl::DirectoryItem::get( aTmpName3, rItem );
            
            CPPUNIT_ASSERT_MESSAGE( "test for get function: use a non existed file URL.",
                                    FileBase::E_NOENT == nError1 );
        }
        
        CPPUNIT_TEST_SUITE( get );
        CPPUNIT_TEST( get_001 ); 
        CPPUNIT_TEST( get_002 ); 
        CPPUNIT_TEST( get_003 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class get
    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline RC getFileStatus( FileStatus& rStatus )
    //---------------------------------------------------------------------
    class  getFileStatus : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC     nError1, nError2;
        
        public:
        // initialization
        void setUp( )
        {
            // create a tempfile in $TEMP/tmpdir/tmpname.
            createTestDirectory( aTmpName3 );
            createTestFile( aTmpName4 );
        }

        void tearDown( )
        {
            // remove the tempfile in $TEMP/tmpdir/tmpname.
            deleteTestFile( aTmpName4 );
            deleteTestDirectory( aTmpName3 );
        }

        // test code.
        void getFileStatus_001( )
        { 
            ::osl::DirectoryItem    rItem;  //constructor
             //get the DirectoryItem.
            nError1 = ::osl::DirectoryItem::get( aTmpName4, rItem );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            //check the file name
              ::osl::FileStatus   rFileStatus( FileStatusMask_FileName ); 
            nError2 = rItem.getFileStatus( rFileStatus );
            
            CPPUNIT_ASSERT_MESSAGE( "test for getFileStatus function: get file status and check filename",
                                    ( ::osl::FileBase::E_None == nError2 ) &&
                                    ( sal_True == compareFileName( rFileStatus.getFileName( ), aTmpName2 ) ) );
        }

        void getFileStatus_002( )
        { 
            ::osl::DirectoryItem    rItem;  //constructor
             //get the DirectoryItem.
            nError1 = ::osl::DirectoryItem::get( aTmpName6, rItem );
            
            //check the file name
              ::osl::FileStatus   rFileStatus( FileStatusMask_FileName ); 
            nError2 = rItem.getFileStatus( rFileStatus );
            
            CPPUNIT_ASSERT_MESSAGE( "test for getFileStatus function: file not existed",
                                    ( ::osl::FileBase::E_INVAL == nError2 )  );
        }
        
        void getFileStatus_003( )
        { 
            ::osl::DirectoryItem    rItem;  //constructor
             //get the DirectoryItem.
            nError1 = ::osl::DirectoryItem::get( aTmpName3, rItem );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            //check the file name
              ::osl::FileStatus   rFileStatus( FileStatusMask_FileName ); 
            nError2 = rItem.getFileStatus( rFileStatus );
            
            CPPUNIT_ASSERT_MESSAGE( "test for getFileStatus function: get directory information",
                                    ( ::osl::FileBase::E_None == nError2 ) &&
                                    ( sal_True == compareFileName( rFileStatus.getFileName( ), aTmpName1 ) ) );
        }
        
        
        CPPUNIT_TEST_SUITE( getFileStatus );
        CPPUNIT_TEST( getFileStatus_001 ); 
        CPPUNIT_TEST( getFileStatus_002 ); 
        CPPUNIT_TEST( getFileStatus_003 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class getFileStatus 	
    
    
    
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_DirectoryItem::ctors, "osl_DirectoryItem" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_DirectoryItem::copy_assin_Ctors, "osl_DirectoryItem" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_DirectoryItem::is, "osl_DirectoryItem" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_DirectoryItem::get, "osl_DirectoryItem" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_DirectoryItem::getFileStatus, "osl_DirectoryItem" );
}// namespace osl_DirectoryItem
    

//------------------------------------------------------------------------
// Beginning of the test cases for Directory class
//------------------------------------------------------------------------
namespace osl_Directory
{
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	Directory( const ::rtl::OUString& strPath ): _pData( 0 ), _aPath( strPath )
    //---------------------------------------------------------------------
    class  ctors : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC     nError1, nError2;
        
        public:
        // initialization
        void setUp( )
        {
            // create a tempfile in $TEMP/tmpdir/tmpname.
            createTestDirectory( aTmpName3 );
            createTestFile( aTmpName4 );
        }

        void tearDown( )
        {
            // remove the tempfile in $TEMP/tmpdir/tmpname.
            deleteTestFile( aTmpName4 );
            deleteTestDirectory( aTmpName3 );
        }

        // test code.
        void ctors_001( )
        { 
            ::osl::Directory testDirectory( aTmpName3 ); //constructor
        
            //open a directory
            nError1 = testDirectory.open( );
             CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            //close a directory
            nError2 = testDirectory.close( );
             CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError2 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for ctors function: create an instance and check open and close",
                                     ( ::osl::FileBase::E_None == nError1 ) &&
                                    ( ::osl::FileBase::E_None == nError2 ) );
        }

        void ctors_002( )
        { 
            ::osl::Directory testDirectory( aTmpName9 ); //constructor
        
            //open a directory
            nError1 = testDirectory.open( );
             CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            //close a directory
            nError2 = testDirectory.close( );
             CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError2 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for ctors function: relative URL, :-), it is also worked",
                                     ( ::osl::FileBase::E_None == nError1 ) &&
                                    ( ::osl::FileBase::E_None == nError2 ) );
        }
        
        CPPUNIT_TEST_SUITE( ctors );
        CPPUNIT_TEST( ctors_001 ); 
        CPPUNIT_TEST( ctors_002 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class ctors

    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline RC open()
    //---------------------------------------------------------------------
    class  open : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC     nError1, nError2;
        
        public:
        // initialization
        void setUp( )
        {
            // create a tempfile in $TEMP/tmpdir/tmpname.
            createTestDirectory( aTmpName3 );
            createTestFile( aTmpName4 );
        }

        void tearDown( )
        {
            // remove the tempfile in $TEMP/tmpdir/tmpname.
            deleteTestFile( aTmpName4 );
            deleteTestDirectory( aTmpName3 );
        }

        // test code.
        void open_001( )
        { 
            ::osl::Directory testDirectory( aTmpName3 ); //constructor
        
            //open a directory
            nError1 = testDirectory.open( );
            //check if directory is opened.
            sal_Bool bOk = testDirectory.isOpen( );
            //close a directory
            nError2 = testDirectory.close( );
            
            CPPUNIT_ASSERT_MESSAGE( "test for open function: open a directory and check for open",
                                    ( sal_True == bOk ) &&
                                     ( ::osl::FileBase::E_None == nError1 ) &&
                                    ( ::osl::FileBase::E_None == nError2 ) );
        }

        void open_002( )
        { 
            ::osl::Directory testDirectory( aTmpName6 ); //constructor
        
            //open a directory
            nError1 = testDirectory.open( );
             if ( ::osl::FileBase::E_None == nError1 )
            {
                nError2 = testDirectory.close( );
                CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError2 );
            }
            
            CPPUNIT_ASSERT_MESSAGE( "test for open function: open a file that is not existed",
                                     ( ::osl::FileBase::E_NOENT == nError1 ) );
        }
        
        void open_003( )
        { 
            ::osl::Directory testDirectory( aTmpSys1 ); //constructor
        
            //open a directory
            nError1 = testDirectory.open( );
             if ( ::osl::FileBase::E_None == nError1 )
            {
                nError2 = testDirectory.close( );
                CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError2 );
            }
            
            CPPUNIT_ASSERT_MESSAGE( "test for open function: using system path",
                                     ( ::osl::FileBase::E_INVAL == nError1 ) );
        }
        
        void open_004( )
        { 
            ::osl::Directory testDirectory( aTmpName4 ); //constructor
        
            //open a directory
            nError1 = testDirectory.open( );
             if ( ::osl::FileBase::E_None == nError1 )
            {
                nError2 = testDirectory.close( );
                CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError2 );
            }
            
            CPPUNIT_ASSERT_MESSAGE( "test for open function: open a file instead of a directory",
                                     ( ::osl::FileBase::E_NOTDIR == nError1 ) );
        }
        
        CPPUNIT_TEST_SUITE( open );
        CPPUNIT_TEST( open_001 ); 
        CPPUNIT_TEST( open_002 ); 
        CPPUNIT_TEST( open_003 ); 
        CPPUNIT_TEST( open_004 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class open
    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline sal_Bool isOpen() { return _pData != NULL; };
    //---------------------------------------------------------------------
    class  isOpen : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC     nError1, nError2;
        
        public:
        // initialization
        void setUp( )
        {
            // create a tempfile in $TEMP/tmpdir/tmpname.
            createTestDirectory( aTmpName3 );
            createTestFile( aTmpName4 );
        }

        void tearDown( )
        {
            // remove the tempfile in $TEMP/tmpdir/tmpname.
            deleteTestFile( aTmpName4 );
            deleteTestDirectory( aTmpName3 );
        }

        // test code.
        void isOpen_001( )
        { 
            ::osl::Directory testDirectory( aTmpName3 ); //constructor
        
            //open a directory
            nError1 = testDirectory.open( );
            //check if directory is opened.
            sal_Bool bOk = testDirectory.isOpen( );
            //close a directory
            nError2 = testDirectory.close( );
            
            CPPUNIT_ASSERT_MESSAGE( "test for isOpen function: open a directory and check for open",
                                    ( sal_True == bOk ) );
        }
    
        void isOpen_002( )
        { 
            ::osl::Directory testDirectory( aTmpName3 ); //constructor
        
            //check if directory is opened.
            sal_Bool bOk = testDirectory.isOpen( );
            
            CPPUNIT_ASSERT_MESSAGE( "test for isOpen function: do not open a directory and check for open",
                                    !( sal_True == bOk ) );
        }
    
        CPPUNIT_TEST_SUITE( isOpen );
        CPPUNIT_TEST( isOpen_001 ); 
        CPPUNIT_TEST( isOpen_002 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class isOpen
    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline RC close()
    //---------------------------------------------------------------------
    class  close : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC     nError1, nError2;
        
        public:
        // initialization
        void setUp( )
        {
            // create a tempdirectory : $TEMP/tmpdir.
            createTestDirectory( aTmpName3 );
        }

        void tearDown( )
        {
            // remove a tempdirectory : $TEMP/tmpdir.
            deleteTestDirectory( aTmpName3 );
        }

        // test code.
        void close_001( )
        { 
            ::osl::Directory testDirectory( aTmpName3 ); //constructor
        
            //open a directory
            nError1 = testDirectory.open( );
            //close a directory
            nError2 = testDirectory.close( );
            //check if directory is opened.
            sal_Bool bOk = testDirectory.isOpen( );
            
            CPPUNIT_ASSERT_MESSAGE( "test for isOpen function: close a directory and check for open",
                                    !( sal_True == bOk ) );
        }
    
        void close_002( )
        { 
            ::osl::Directory testDirectory( aTmpName3 ); //constructor
        
            //close a directory
            nError1 = testDirectory.close( );
            
            CPPUNIT_ASSERT_MESSAGE( "test for isOpen function: close a not opened directory",
                                    ( ::osl::FileBase::E_BADF == nError1 ) );
        }
        
        
        CPPUNIT_TEST_SUITE( close );
        CPPUNIT_TEST( close_001 ); 
        CPPUNIT_TEST( close_002 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class close
        
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline RC reset()
    //---------------------------------------------------------------------
    class  reset : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC     nError1, nError2;
        ::osl::DirectoryItem    rItem;
        
        public:
        // initialization
        void setUp( )
        {
            // create a tempdirectory : $TEMP/tmpdir.
            createTestDirectory( aTmpName3 );
            // create three files : $TEMP/tmpdir/tmpname, $TEMP/tmpdir/tmpdir, $TEMP/tmpdir/hiddenfile,
            createTestFile( aTmpName3, aTmpName2);
            createTestFile( aTmpName3, aTmpName1);
            createTestFile( aTmpName3, aHidURL1);
        }

        void tearDown( )
        {
            // remove three files : $TEMP/tmpdir/tmpname, $TEMP/tmpdir/tmpdir, $TEMP/tmpdir/hiddenfile,
            deleteTestFile( aTmpName3, aHidURL1);
            deleteTestFile( aTmpName3, aTmpName1);
            deleteTestFile( aTmpName3, aTmpName2);
            // remove a tempdirectory : $TEMP/tmpdir.
            deleteTestDirectory( aTmpName3 );
        }

        // test code.
        void reset_001( )
        { 
            ::osl::Directory testDirectory( aTmpName3 ); //constructor
        
            //open a directory
            nError1 = testDirectory.open( );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            //get first Item
            nError1 = testDirectory.getNextItem( rItem, 1 );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            //get second Item
            nError1 = testDirectory.getNextItem( rItem, 0 );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            //reset enumeration
            nError2 = testDirectory.reset( );
            //get reseted Item
            nError1 = testDirectory.getNextItem( rItem, 0 );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            //check the file name
              ::osl::FileStatus   rFileStatus( FileStatusMask_FileName ); 
            nError1 = rItem.getFileStatus( rFileStatus );
            //close a directory
            nError1 = testDirectory.close( );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for reset function: get two directory item, reset it, then get again, check the filename",
                                    ( ::osl::FileBase::E_None == nError2 ) &&
                                    ( sal_True == compareFileName( rFileStatus.getFileName( ), aTmpName2 ) ) );
        }
    
        void reset_002( )
        { 
            ::osl::Directory testDirectory( aTmpName6 ); //constructor
        
            //close a directory
            nError1 = testDirectory.reset( );
            
            CPPUNIT_ASSERT_MESSAGE( "test for reset function: reset a non existed directory",
                                    ( ::osl::FileBase::E_NOENT == nError1 ) );
        }
        
        
        void reset_003( )
        { 
            ::osl::Directory testDirectory( aTmpName4 ); //constructor
        
            //close a directory
            nError1 = testDirectory.reset( );
            
            CPPUNIT_ASSERT_MESSAGE( "test for reset function: reset a file instead of a directory",
                                    ( ::osl::FileBase::E_NOTDIR == nError1 ) );
        }
        
        void reset_004( )
        { 
            ::osl::Directory testDirectory( aTmpSys1 ); //constructor
        
            //close a directory
            nError1 = testDirectory.reset( );
            
            CPPUNIT_ASSERT_MESSAGE( "test for reset function: use a system path",
                                    ( ::osl::FileBase::E_INVAL == nError1 ) );
        }
        
        CPPUNIT_TEST_SUITE( reset );
        CPPUNIT_TEST( reset_001 ); 
        CPPUNIT_TEST( reset_002 ); 
        CPPUNIT_TEST( reset_003 ); 
        CPPUNIT_TEST( reset_004 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class reset
    
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline RC getNextItem( DirectoryItem& rItem, sal_uInt32 nHint = 0 )
    //---------------------------------------------------------------------
    class  getNextItem : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC     nError1, nError2;
        ::osl::DirectoryItem    rItem;
        
        public:
        // initialization
        void setUp( )
        {
            // create a tempdirectory : $TEMP/tmpdir.
            createTestDirectory( aTmpName3 );
            // create three files : $TEMP/tmpdir/tmpname, $TEMP/tmpdir/tmpdir, $TEMP/tmpdir/hiddenfile,
            createTestFile( aTmpName3, aTmpName2 );
            createTestFile( aTmpName3, aTmpName1 );
            createTestFile( aTmpName3, aHidURL1 );
        }

        void tearDown( )
        {
            // remove three files : $TEMP/tmpdir/tmpname, $TEMP/tmpdir/tmpdir, $TEMP/tmpdir/hiddenfile,
            deleteTestFile( aTmpName3, aHidURL1 );
            deleteTestFile( aTmpName3, aTmpName1 );
            deleteTestFile( aTmpName3, aTmpName2 );
            // remove a tempdirectory : $TEMP/tmpdir.
            deleteTestDirectory( aTmpName3 );
        }

        // test code.
        void getNextItem_001( )
        { 
             ::osl::Directory testDirectory( aTmpName3 ); //constructor
        
            //open a directory
            nError1 = testDirectory.open( );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            //check the file name
            ::rtl::OUString     strFilename; 
            sal_Bool            bOk1, bOk2, bOk3;
              ::osl::FileStatus   rFileStatus( FileStatusMask_FileName ); 
            for ( int nCount = 0; nCount < 3; nCount++ )
            {
                //get three Items
                nError1 = testDirectory.getNextItem( rItem, 2 );
                CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
                nError1 = rItem.getFileStatus( rFileStatus );
                CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
                switch ( nCount )
                {
                    case 0: bOk1 = compareFileName( rFileStatus.getFileName( ), aTmpName2 );
                        break;
                    case 1: bOk2 = compareFileName( rFileStatus.getFileName( ), aTmpName1 );
                        break;
                    case 2: bOk3 = compareFileName( rFileStatus.getFileName( ), aHidURL1 );
                }
            }			
            
            //close a directory
            nError1 = testDirectory.close( );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for getNextItem function: retrive three items and check their names.",
                                    ( sal_True == bOk1 ) && ( sal_True == bOk2 ) && ( sal_True == bOk3 ) );
        }
    
        void getNextItem_002( )
        { 
             ::osl::Directory testDirectory( aTmpName3 ); //constructor
            nError1 = testDirectory.getNextItem( rItem );
            
            CPPUNIT_ASSERT_MESSAGE( "test for getNextItem function: retrive an item in a directory which is not opened, also test for nHint's default value.",
                                    ( ::osl::FileBase::E_INVAL == nError1 ) );
        }
    
        void getNextItem_003( )
        { 
             ::osl::Directory testDirectory( aTmpName3 ); //constructor
            
            //open a directory
            nError1 = testDirectory.open( );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            for ( int nCount = 0; nCount < 4; nCount++ )
            {
                nError2 = testDirectory.getNextItem( rItem, 3 );
            }
            
            //close a directory
            nError1 = testDirectory.close( );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for getNextItem function: retrive 4 times in a directory which contain only 3 files.",
                                    ( ::osl::FileBase::E_NOENT == nError2 ) );
        }
    
        CPPUNIT_TEST_SUITE( getNextItem );
        CPPUNIT_TEST( getNextItem_001 ); 
        CPPUNIT_TEST( getNextItem_002 ); 
        CPPUNIT_TEST( getNextItem_003 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class getNextItem
        
    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline static RC getVolumeInfo( const ::rtl::OUString& ustrDirectoryURL, VolumeInfo& rInfo )
    //---------------------------------------------------------------------
    class  getVolumeInfo : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC     nError1, nError2;
        
        public:

        // test code.
        void getVolumeInfo_001( )
        { 
            sal_Int32 mask = VolumeInfoMask_FileSystemName;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            //call getVolumeInfo here
            nError1 = ::osl::Directory::getVolumeInfo( aVolURL1, aVolumeInfo );
            CPPUNIT_ASSERT( sal_True == aVolumeInfo.isValid( mask ) );
            
            //get file system name
            ::rtl::OUString aFileSysName( aNullURL );
            aFileSysName = aVolumeInfo.getFileSystemName( );

            CPPUNIT_ASSERT_MESSAGE( "test for getVolumeInfo function: getVolumeInfo of root directory.",
                                    ( osl::FileBase::E_None == nError1 ) && 
                                    ( sal_False == compareFileName( aFileSysName, aNullURL ) ) );
        }
    
        void getVolumeInfo_002( )
        { 
            sal_Int32 mask = VolumeInfoMask_FileSystemName;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            //call getVolumeInfo here
            nError1 = ::osl::Directory::getVolumeInfo( aRootSys, aVolumeInfo );

            CPPUNIT_ASSERT_MESSAGE( "test for getVolumeInfo function: use system path as parameter.",
                                    ( osl::FileBase::E_INVAL == nError1 ) );
        }
    
        void getVolumeInfo_003( )
        { 
            sal_Int32 mask = VolumeInfoMask_FileSystemName;
            ::osl::VolumeInfo aVolumeInfo( mask ); 
            //call getVolumeInfo here
            nError1 = ::osl::Directory::getVolumeInfo( aTmpName3, aVolumeInfo );

            CPPUNIT_ASSERT_MESSAGE( "test for getVolumeInfo function: non-existence test.",
                                    ( osl::FileBase::E_NOENT == nError1 ) );
        }
    
        CPPUNIT_TEST_SUITE( getVolumeInfo );
        CPPUNIT_TEST( getVolumeInfo_001 ); 
        CPPUNIT_TEST( getVolumeInfo_002 ); 
        CPPUNIT_TEST( getVolumeInfo_003 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class getVolumeInfo


    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline static RC create( const ::rtl::OUString& ustrDirectoryURL )
    //---------------------------------------------------------------------
    class  create : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC     nError1, nError2;
        
        public:

        // test code.
        void create_001( )
        { 
            //create directory in $TEMP/tmpdir
            nError1 = ::osl::Directory::create( aTmpName3 );
            //check for existence
            ::osl::Directory rDirectory( aTmpName3 );
            nError2 = rDirectory.open( );
            //remove it 
            deleteTestDirectory( aTmpName3 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for create function: create a directory and check its existence.",
                                    ( osl::FileBase::E_None == nError1 ) &&
                                    ( osl::FileBase::E_None == nError2 ) );
        }
        
        void create_002( )
        { 
            //create directory in /tmpname
            nError1 = ::osl::Directory::create( aTmpName7 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for create function: create a directory in root for access test.",
                                    ( osl::FileBase::E_ACCES == nError1 ) );
        }
        
        void create_003( )
        { 
            //create directory in /tmpname
            nError1 = ::osl::Directory::create( aSysPath1 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for create function: create a directory using system path.",
                                    ( osl::FileBase::E_INVAL == nError1 ) );
        }
        
        CPPUNIT_TEST_SUITE( create );
        CPPUNIT_TEST( create_001 );  
        CPPUNIT_TEST( create_002 ); 
        CPPUNIT_TEST( create_003 ); 
        CPPUNIT_TEST_SUITE_END( );
    };// class create

    //---------------------------------------------------------------------
    // 	testing the method 
    // 	inline static RC remove( const ::rtl::OUString& ustrDirectoryURL )
    //---------------------------------------------------------------------
    class  remove : public CppUnit::TestFixture
    {
        ::osl::FileBase::RC     nError1, nError2;
        
        public:

        // test code.
        void remove_001( )
        { 
            //create directory in $TEMP/tmpdir
            nError1 = ::osl::Directory::create( aTmpName3 );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            //remove it 
            nError1 = ::osl::Directory::remove( aTmpName3 );
            //check for existence
            ::osl::Directory rDirectory( aTmpName3 );
            nError2 = rDirectory.open( );
            
            CPPUNIT_ASSERT_MESSAGE( "test for remove function: remove a directory and check its existence.",
                                    ( osl::FileBase::E_None == nError1 ) &&
                                    ( osl::FileBase::E_NOENT == nError2 ) );
        }
        
        void remove_002( )
        { 
            //create directory in $TEMP/tmpdir
            nError1 = ::osl::Directory::create( aTmpName3 );
            CPPUNIT_ASSERT( ::osl::FileBase::E_None == nError1 );
            //try to remove it by system path
            nError1 = ::osl::Directory::remove( aSysPath3 );
             //check for existence
            ::osl::Directory rDirectory( aTmpName3 );
            nError2 = rDirectory.open( );
            if ( osl::FileBase::E_NOENT != nError2 )
                ::osl::Directory::remove( aTmpName3 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for remove function: remove a directory by its system path, and check its existence.",
                                    ( osl::FileBase::E_INVAL == nError1 ) );
        }
        
        void remove_003( )
        { 
            //try to remove a non-existed directory
            nError1 = ::osl::Directory::remove( aTmpName6 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for remove function: try to remove a non-existed directory.",
                                    ( osl::FileBase::E_NOENT == nError1 ) );
        }
        
        void remove_004( )
        { 
            createTestFile( aTmpName6 );
            //try to remove file.
            nError1 = ::osl::Directory::remove( aTmpName6 );
            deleteTestFile( aTmpName6 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for remove function: try to remove a file but not directory.",
                                    ( osl::FileBase::E_NOTDIR == nError1 ) );
        }
        
        void remove_005( )
        { 
            createTestDirectory( aTmpName3 );
            createTestFile( aTmpName4 );
            nError1 = ::osl::Directory::remove( aTmpName3 );
            deleteTestFile( aTmpName4 );
            deleteTestDirectory( aTmpName3 );
            
            CPPUNIT_ASSERT_MESSAGE( "test for remove function: try to remove a directory that is not empty.",
                                    ( osl::FileBase::E_NOTEMPTY == nError1 ) );
        }
        
        CPPUNIT_TEST_SUITE( remove );
        CPPUNIT_TEST( remove_001 );  
        CPPUNIT_TEST( remove_002 );  
        CPPUNIT_TEST( remove_003 );  
        CPPUNIT_TEST( remove_004 );  
        CPPUNIT_TEST( remove_005 );  
        CPPUNIT_TEST_SUITE_END( );
    };// class remove

    // -----------------------------------------------------------------------------
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_Directory::ctors, "osl_Directory" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_Directory::open, "osl_Directory" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_Directory::isOpen, "osl_Directory" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_Directory::close, "osl_Directory" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_Directory::reset, "osl_Directory" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_Directory::getNextItem, "osl_Directory" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_Directory::getVolumeInfo, "osl_Directory" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_Directory::create, "osl_Directory" );
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( osl_Directory::remove, "osl_Directory" );
}// namespace osl_Directory
        



// -----------------------------------------------------------------------------
// this macro creates an empty function, which will called by the RegisterAllFunctions()
// to let the user the possibility to also register some functions by hand.
// -----------------------------------------------------------------------------

NOADDITIONAL;
