#include <stdlib.h>
#include <rtl/ustring.hxx>
#include <rtl/strbuf.hxx>
#include <rtl/uri.hxx>
#include <osl/thread.h>
#include <osl/file.hxx>

#include <cppunit/simpleheader.hxx>

// -----------------------------------------------------------------------------

namespace Stringtest
{
    rtl::OString toHex(unsigned char _c)
    {
        rtl::OStringBuffer sStrBuf;
        static char cHex[] = "0123456789ABCDEF";

        int nhigh = int(_c) >> 4 & 0xf;
        int nlow  = int(_c) & 0xf;
        sStrBuf.append( cHex[nhigh] );
        sStrBuf.append( cHex[nlow] );
        return sStrBuf.makeStringAndClear();
    }
    
    rtl::OString escapeString(rtl::OString const& _sStr)
    {
        rtl::OStringBuffer sStrBuf;
        sal_Int32 nLength = _sStr.getLength();
        for(int i=0;i<nLength;++i)
        {
            unsigned char c = (unsigned char)_sStr[i];
            if (c > 127)
            {
                sStrBuf.append("%");
                sStrBuf.append(toHex(c));
            }
            else
            {
                sStrBuf.append((char)c);
            }
        }
        return sStrBuf.makeStringAndClear();
    }
    
    // -----------------------------------------------------------------------------

    class Convert : public CppUnit::TestFixture  
    {
        rtl::OUString m_aStr;
    public:
        void setUp()
            {
                // m_aStr = rtl::OUString::createFromAscii( "/tmp/��ä���ˤ���" );
            }
        
        /*
          rtl::OString toUTF8(rtl::OUString const& _suStr)
            {
                rtl::OString sStrAsUTF8 = rtl::OUStringToOString(_suStr, RTL_TEXTENCODING_UTF8);
                t_print("%s\n", escapeString(sStrAsUTF8).getStr());
                return sStrAsUTF8;
            }
        */
        rtl::OUString fromUTF8(rtl::OString const& _suStr)
            {
                rtl::OUString suStr = rtl::OStringToOUString(_suStr, RTL_TEXTENCODING_UTF8);
                return suStr;
            }
        
        rtl::OString convertToOString(rtl::OUString const& _suStr)
            {
                return rtl::OUStringToOString(_suStr, osl_getThreadTextEncoding()/*RTL_TEXTENCODING_ASCII_US*/);
            }
        
        void showContent(rtl::OUString const& _suStr)
            {
                rtl::OString sStr = convertToOString(_suStr);
                t_print("%s\n", sStr.getStr());
            }

        void toUTF8_mech(rtl::OUString const& _suStr, rtl_UriEncodeMechanism _eMechanism)
            {
                rtl::OUString suStr;
                suStr = rtl::Uri::encode(_suStr, rtl_UriCharClassNone,          _eMechanism, RTL_TEXTENCODING_UTF8);
                showContent(suStr);
                suStr = rtl::Uri::encode(_suStr, rtl_UriCharClassUric,          _eMechanism, RTL_TEXTENCODING_UTF8);
                showContent(suStr);
                suStr = rtl::Uri::encode(_suStr, rtl_UriCharClassUricNoSlash,   _eMechanism, RTL_TEXTENCODING_UTF8);
                showContent(suStr);
                suStr = rtl::Uri::encode(_suStr, rtl_UriCharClassRelSegment,    _eMechanism, RTL_TEXTENCODING_UTF8);
                showContent(suStr);
                suStr = rtl::Uri::encode(_suStr, rtl_UriCharClassRegName,       _eMechanism, RTL_TEXTENCODING_UTF8);
                showContent(suStr);
                suStr = rtl::Uri::encode(_suStr, rtl_UriCharClassUserinfo,      _eMechanism, RTL_TEXTENCODING_UTF8);
                showContent(suStr);
                suStr = rtl::Uri::encode(_suStr, rtl_UriCharClassPchar,         _eMechanism, RTL_TEXTENCODING_UTF8);
                showContent(suStr);
                suStr = rtl::Uri::encode(_suStr, rtl_UriCharClassUnoParamValue, _eMechanism, RTL_TEXTENCODING_UTF8);
                showContent(suStr);
            }
        
        void toUTF8(rtl::OUString const& _suStr)
            {
                t_print("rtl_UriEncodeIgnoreEscapes \n");
                toUTF8_mech(_suStr, rtl_UriEncodeIgnoreEscapes);
                t_print("\n");
                t_print("# rtl_UriEncodeKeepEscapes\n");
                toUTF8_mech(_suStr, rtl_UriEncodeKeepEscapes);
                t_print("\n");
                t_print("# rtl_UriEncodeCheckEscapes\n");
                toUTF8_mech(_suStr, rtl_UriEncodeCheckEscapes);
                t_print("\n");
            }
        
        void test_FromUTF8_001()
            {
                rtl::OString sStr("h%C3%A4llo");
                rtl::OUString suStr = rtl::OStringToOUString(sStr, RTL_TEXTENCODING_ASCII_US);

                rtl::OUString suStr_UriDecodeToIuri      = rtl::Uri::decode(suStr, rtl_UriDecodeToIuri, RTL_TEXTENCODING_UTF8);
                showContent(suStr_UriDecodeToIuri);

                rtl::OUString suStr2 = rtl::Uri::encode(suStr_UriDecodeToIuri, rtl_UriCharClassUnoParamValue, rtl_UriEncodeKeepEscapes, RTL_TEXTENCODING_UTF8);
                showContent(suStr2);

                // suStr should be equal to suStr2
            }
        
        // "%C3%84qypten";
        // testshl2 ../../../unxlngi4.pro/lib/libConvert.so "-onlyerrors"
        // # Type: 'Directory' file name '%E6%89%8B%E6%9C%BA%E5%8F%B7%E7%A0%81'
        // # Type: 'Directory' file name '%E6%9C%AA%E5%91%BD%E5%90%8Dzhgb18030'
        // # Type: 'Regular file' file name '%E5%B7%A5%E4%BD%9C'
        // # Type: 'Regular file' file name '%E4%BA%8C%E6%89%8B%E6%88%BF%E4%B9%B0%E5%8D%96%E5%90%88%E5%90%8C%E8%8D%89%E7%A8%BF.doc'
        // ls 
        rtl::OString getFileTypeName(osl::FileStatus const& _aStatus)
            {
                rtl::OString sType;
                if (_aStatus.isValid(osl_FileStatus_Mask_Type))
                {
                    osl::FileStatus::Type aType = _aStatus.getFileType();
                    if (aType == osl::FileStatus::Directory)
                    {
                        sType = "Directory";
                    }
                    else if (aType == osl::FileStatus::Regular)
                    {
                        sType = "Regular file";
                    }
                    else if (aType == osl::FileStatus::Volume)
                    {
                        sType = "Volume";
                    }
                    else if (aType == osl::FileStatus::Fifo)
                    {
                        sType = "Fifo";
                    }
                    else if (aType == osl::FileStatus::Socket)
                    {
                        sType = "Socket";
                    }
                    else if (aType == osl::FileStatus::Link)
                    {
                        sType = "Link";
                    }
                    else if (aType == osl::FileStatus::Special)
                    {
                        sType = "Special";
                    }
                    else if (aType == osl::FileStatus::Unknown)
                    {
                        sType = "Unknown";
                    }
                    else
                    {
                        sType = "Not handled yet";
                    }
                }
                else
                {
                    sType = "ERROR: osl_FileStatus_Mask_Type not set for FileStatus!";
                }
                return sType;   
            }
        
                
        void test_UTF8_files()
            {
#ifdef UNX
                rtl::OUString suDirURL(rtl::OUString::createFromAscii("file:///tmp/atestdir"));
#else /* Windows */
                rtl::OUString suDirURL(rtl::OUString::createFromAscii("file:///c:/temp/atestdir"));
#endif
                osl::Directory aDir(suDirURL);
                aDir.open();
                if (aDir.isOpen())
                {
                    osl::DirectoryItem aItem;
                    osl::FileStatus aStatus(osl_FileStatus_Mask_FileName | osl_FileStatus_Mask_Attributes | osl_FileStatus_Mask_Type);
                    while (aDir.getNextItem(aItem) == ::osl::FileBase::E_None)
                    {
                        if (osl::FileBase::E_None == aItem.getFileStatus(aStatus) &&
                            aStatus.isValid(osl_FileStatus_Mask_FileName | osl_FileStatus_Mask_Attributes))
                        {
                            rtl::OString sType = getFileTypeName(aStatus);
                            
                            rtl::OUString suFilename = aStatus.getFileName();
                            // rtl::OUString suFullFileURL;
                            
                            rtl::OUString suStrUTF8 = rtl::Uri::encode(suFilename, rtl_UriCharClassUnoParamValue, rtl_UriEncodeKeepEscapes, RTL_TEXTENCODING_UTF8);
                            rtl::OString sStrUTF8 = convertToOString(suStrUTF8);
                            t_print("Type: '%s' file name '%s'\n", sType.getStr(), sStrUTF8.getStr());
                        }
                    }
                    aDir.close();
                }
                else
                {
                    rtl::OString sStr;
                    sStr = rtl::OUStringToOString(suDirURL, osl_getThreadTextEncoding());
                    t_print("can't open dir:'%s'\n", sStr.getStr());
                }
            }
        
        void test_FromUTF8()
            {
                rtl::OString sStr("h%C3%A4llo");
                // rtl::OString sStr("h�llo");
                // rtl::OUString suStr = rtl::OStringToOUString(sStr, RTL_TEXTENCODING_ASCII_US);
                rtl::OUString suStr = rtl::OStringToOUString(sStr, osl_getThreadTextEncoding());
                // rtl::OString  sStr = escapeString( rtl::OString("/tmp/��ä���ˤ���") );
                // rtl::OUString suStr = rtl::OUString::createFromAscii( sStr.getStr() );

//    rtl_UriEncodeIgnoreEscapes,
//    rtl_UriEncodeKeepEscapes,
//     rtl_UriEncodeCheckEscapes,
//                rtl::OUString suStr2 = rtl::Uri::encode(suStr, rtl_UriCharClassRegName, rtl_UriEncodeCheckEscapes, RTL_TEXTENCODING_UTF8);
                rtl::OUString suStr_UriDecodeNone        = rtl::Uri::decode(suStr, rtl_UriDecodeNone, RTL_TEXTENCODING_UTF8);
                showContent(suStr_UriDecodeNone);
                toUTF8(suStr_UriDecodeNone);
                
                rtl::OUString suStr_UriDecodeToIuri      = rtl::Uri::decode(suStr, rtl_UriDecodeToIuri, RTL_TEXTENCODING_UTF8);
                showContent(suStr_UriDecodeToIuri);
                toUTF8(suStr_UriDecodeToIuri);
                
                rtl::OUString suStr_UriDecodeWithCharset = rtl::Uri::decode(suStr, rtl_UriDecodeWithCharset, RTL_TEXTENCODING_UTF8);
                showContent(suStr_UriDecodeWithCharset);
                toUTF8(suStr_UriDecodeWithCharset);
            }
        
        void test_UTF8()
            {
                rtl::OUString aStr = rtl::OUString::createFromAscii( "/tmp/��ä���ˤ���" );

            }
        
/*
        void UTF8()
            {
                rtl::OString sSysPath5_AsUTF8 = rtl::OUStringToOString(m_aStr, RTL_TEXTENCODING_UTF8);
                t_print("aSysPath5 as UTF8 '%s'\n", sSysPath5_AsUTF8.getStr());
                
            {
                rtl::OUString suSysPath5Str = rtl::OStringToOUString(sSysPath5_AsUTF8, RTL_TEXTENCODING_UTF8);
                if (suSysPath5Str.equals(m_aStr))
                {
                    t_print("1. convert works.\n");
                }
            }
            
            sal_Char aSysPath5Str[] = "/tmp/�￣ﾺ�ﾤﾳﾤ￳ﾤ�ﾤ�ﾤ�";
            rtl::OString aStr (aSysPath5Str);
            rtl::OUString suSysPath5Str = rtl::OStringToOUString(aSysPath5Str, RTL_TEXTENCODING_UTF8);
            if (suSysPath5Str.equals(m_aStr))
            {
                t_print("2. convert works.\n");
            }
            }
*/
        
        CPPUNIT_TEST_SUITE( Convert );
//      CPPUNIT_TEST( test_FromUTF8_001 );
        CPPUNIT_TEST( test_UTF8_files );
//      CPPUNIT_TEST( test_FromUTF8 );
        CPPUNIT_TEST_SUITE_END( );
    };
    
}


CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( Stringtest::Convert, "Stringtest" );

// LLA: doku anpassen!!!

NOADDITIONAL;
