#include <stdio.h>

#include <rtl/process.h>
#include <rtl/bootstrap.hxx>
#include <rtl/string.hxx>
#include <rtl/byteseq.hxx>

#include <osl/process.h>

using namespace ::rtl;


void main( int argc, char *argv[] )
{
    sal_Int32 nCount = rtl_getCommandArgCount();
    fprintf( stdout, "rtl-commandargs (%d)" , nCount );
    for( sal_Int32 i = 0 ; i < nCount ; i ++ )
    {
        OUString data;
        rtl_getCommandArg( i , &(data.pData) );
        OString o = OUStringToOString( data, RTL_TEXTENCODING_ASCII_US );
        fprintf( stdout, " %s", o.getStr() );
    }
    fprintf( stdout, "\n" );

    if( nCount == 0 )
    {
        printf( "usage : testbootstrap <checkedValueOfMyBootstrapValue>\n" );
        exit( 1 );
    }

    OUString name( RTL_CONSTASCII_USTRINGPARAM( "MYBOOTSTRAPTESTVALUE" ));
    OUString myDefault( RTL_CONSTASCII_USTRINGPARAM( "default" ));

    OUString value;
      Bootstrap::get( name, value, myDefault );
    
    OSL_ASSERT( OUString::createFromAscii( argv[1] ) == value );

    // test the default case
    name = OUString( RTL_CONSTASCII_USTRINGPARAM( "no_one_has_set_this_name" ) );
    OSL_ASSERT( ! Bootstrap::get( name, value ) );
    
    myDefault = OUString( RTL_CONSTASCII_USTRINGPARAM( "1" ) );
    OUString myDefault2 = OUString( RTL_CONSTASCII_USTRINGPARAM( "2" ) );

    Bootstrap::get( name, value, myDefault );
    OSL_ASSERT( value == myDefault );
    Bootstrap::get( name, value, myDefault2 );
    OSL_ASSERT( value == myDefault2 );
}
