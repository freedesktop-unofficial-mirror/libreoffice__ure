/*************************************************************************
 *
 *  $RCSfile: fwkbase.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: kz $ $Date: 2004-12-16 11:47:49 $
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
#include "rtl/ustring.hxx"
#include "rtl/ustrbuf.hxx"
#include "rtl/uri.hxx"
#include "osl/thread.hxx"
#include "osl/process.h"
#include "libxml/xpathInternals.h"
#include "osl/file.hxx"
#include "osl/module.hxx"
#include "framework.hxx"
#include "fwkutil.hxx"
#include "elements.hxx"
#include "fwkbase.hxx"

using namespace osl;
using namespace rtl;
#define JAVASETTINGS "javasettings"
#define JAVASETTINGS_XML "javasettings.xml"
#define VENDORSETTINGS "javavendors.xml"

#define UNO_JAVA_JFW_PARAMETER "UNO_JAVA_JFW_PARAMETER_"
#define UNO_JAVA_JFW_JREHOME "UNO_JAVA_JFW_JREHOME"
#define UNO_JAVA_JFW_ENV_JREHOME "UNO_JAVA_JFW_ENV_JREHOME"
#define UNO_JAVA_JFW_CLASSPATH "UNO_JAVA_JFW_CLASSPATH"
#define UNO_JAVA_JFW_ENV_CLASSPATH "UNO_JAVA_JFW_ENV_CLASSPATH"
#define UNO_JAVA_JFW_CLASSPATH_URLS "UNO_JAVA_JFW_CLASSPATH_URLS"
#define UNO_JAVA_JFW_PARAMETERS "UNO_JAVA_JFW_PARAMETERS"
#define UNO_JAVA_JFW_VENDOR_SETTINGS "UNO_JAVA_JFW_VENDOR_SETTINGS"
#define UNO_JAVA_JFW_USER_DATA "UNO_JAVA_JFW_USER_DATA"
#define UNO_JAVA_JFW_SHARED_DATA "UNO_JAVA_JFW_SHARED_DATA"

namespace jfw
{
bool  g_bJavaSet = false;

VendorSettings::VendorSettings()
{
    OString sMsgExc("[Java framework] Error in constructor "
                         "VendorSettings::VendorSettings() (fwkbase.cxx)");
    //Prepare the xml document and context
    OString sSettingsPath = jfw::getVendorSettingsPath();
    if (sSettingsPath.getLength() == 0)
    {
        OString sMsg("[Java framework] A vendor settings file was not specified."
               "Check the bootstrap parameter " UNO_JAVA_JFW_VENDOR_SETTINGS ".");
        OSL_ENSURE(0, sMsg.getStr());
        throw FrameworkException(JFW_E_CONFIGURATION, sMsg);
    }
    if (sSettingsPath.getLength() > 0)
    {
        m_xmlDocVendorSettings = xmlParseFile(sSettingsPath.getStr());
        if (m_xmlDocVendorSettings == NULL)
            throw FrameworkException(
                JFW_E_ERROR,
                OString("[Java framework] Error while parsing file: ")
                + sSettingsPath + OString("."));
        
        m_xmlPathContextVendorSettings = xmlXPathNewContext(m_xmlDocVendorSettings);
        int res = xmlXPathRegisterNs(
            m_xmlPathContextVendorSettings, (xmlChar*) "jf",
            (xmlChar*) NS_JAVA_FRAMEWORK);
        if (res == -1)
            throw FrameworkException(JFW_E_ERROR, sMsgExc);
    }
}

std::vector<PluginLibrary> VendorSettings::getPluginData()
{
    OString sExcMsg("[Java framework] Error in function VendorSettings::getVendorPluginURLs "
                         "(fwkbase.cxx).");
    std::vector<PluginLibrary> vecPlugins;    
    CXPathObjectPtr result(xmlXPathEvalExpression(
        (xmlChar*)"/jf:javaSelection/jf:plugins/jf:library",
        m_xmlPathContextVendorSettings));
    if (xmlXPathNodeSetIsEmpty(result->nodesetval))
        throw FrameworkException(JFW_E_ERROR, sExcMsg);

    //get the values of the library elements + vendor attribute
    xmlNode* cur = result->nodesetval->nodeTab[0];

    while (cur != NULL)
    {
        //between library elements are also text elements
        if (cur->type == XML_ELEMENT_NODE)
        {
            CXmlCharPtr sAttrVendor(xmlGetProp(cur, (xmlChar*) "vendor"));
            CXmlCharPtr sTextLibrary(
                xmlNodeListGetString(m_xmlDocVendorSettings,
                                     cur->xmlChildrenNode, 1));
            PluginLibrary plugin;
            OString osVendor((sal_Char*)(xmlChar*) sAttrVendor);
            plugin.sVendor = OStringToOUString(osVendor, RTL_TEXTENCODING_UTF8);

            //create the file URL to the library
            OUString sUrl = findPlugin(sTextLibrary);
            if (sUrl.getLength() == 0)
            {
                OString sPlugin = OUStringToOString(
                    sTextLibrary, osl_getThreadTextEncoding());
                throw FrameworkException(
                    JFW_E_CONFIGURATION,
                    "[Java framework] The file: " + sPlugin + " does not exist.");
            }
            plugin.sPath  = sUrl;
            
            vecPlugins.push_back(plugin);
        }
        cur = cur->next;
    }
    return vecPlugins;
}

VersionInfo VendorSettings::getVersionInformation(const rtl::OUString & sVendor)
{
    OSL_ASSERT(sVendor.getLength() > 0);
    VersionInfo aVersionInfo;
    OString osVendor = OUStringToOString(sVendor, RTL_TEXTENCODING_UTF8);
    //Get minVersion
    OString sExpresion = OString(
        "/jf:javaSelection/jf:vendorInfos/jf:vendor[@name=\"") + 
        osVendor + OString("\"]/jf:minVersion");
                            
    CXPathObjectPtr xPathObjectMin;
    xPathObjectMin = 
        xmlXPathEvalExpression((xmlChar*) sExpresion.getStr(),
                               m_xmlPathContextVendorSettings);
    if (xmlXPathNodeSetIsEmpty(xPathObjectMin->nodesetval))
    {
        aVersionInfo.sMinVersion = OUString();
    }
    else
    {
        CXmlCharPtr sVersion;
        sVersion = xmlNodeListGetString(
            m_xmlDocVendorSettings,
            xPathObjectMin->nodesetval->nodeTab[0]->xmlChildrenNode, 1);
        OString osVersion((sal_Char*)(xmlChar*) sVersion);
        aVersionInfo.sMinVersion = OStringToOUString(
            osVersion, RTL_TEXTENCODING_UTF8);
    }

    //Get maxVersion
    sExpresion = OString("/jf:javaSelection/jf:vendorInfos/jf:vendor[@name=\"") + 
        osVendor + OString("\"]/jf:maxVersion");
    CXPathObjectPtr xPathObjectMax;
    xPathObjectMax = xmlXPathEvalExpression(
        (xmlChar*) sExpresion.getStr(),
        m_xmlPathContextVendorSettings);
    if (xmlXPathNodeSetIsEmpty(xPathObjectMax->nodesetval))
    {
        aVersionInfo.sMaxVersion = OUString();
    }
    else
    {
        CXmlCharPtr sVersion;
        sVersion = xmlNodeListGetString(
            m_xmlDocVendorSettings,
            xPathObjectMax->nodesetval->nodeTab[0]->xmlChildrenNode, 1);
        OString osVersion((sal_Char*) (xmlChar*) sVersion);
        aVersionInfo.sMaxVersion = OStringToOUString(
            osVersion, RTL_TEXTENCODING_UTF8);
    }

    //Get excludeVersions
    sExpresion = OString("/jf:javaSelection/jf:vendorInfos/jf:vendor[@name=\"") + 
        osVendor + OString("\"]/jf:excludeVersions/jf:version");
    CXPathObjectPtr xPathObjectVersions;
    xPathObjectVersions =
        xmlXPathEvalExpression((xmlChar*) sExpresion.getStr(),
                               m_xmlPathContextVendorSettings);
    if (!xmlXPathNodeSetIsEmpty(xPathObjectVersions->nodesetval))
    {
        xmlNode* cur = xPathObjectVersions->nodesetval->nodeTab[0];
        while (cur != NULL)
        {
            if (cur->type == XML_ELEMENT_NODE )
            {
                if (xmlStrcmp(cur->name, (xmlChar*) "version") == 0)
                {
                    CXmlCharPtr sVersion;
                    sVersion = xmlNodeListGetString(
                        m_xmlDocVendorSettings, cur->xmlChildrenNode, 1);
                    OString osVersion((sal_Char*)(xmlChar*) sVersion);
                    OUString usVersion = OStringToOUString(
                        osVersion, RTL_TEXTENCODING_UTF8);
                    aVersionInfo.addExcludeVersion(usVersion);
                }
            }
            cur = cur->next;
        }
    }
    return aVersionInfo;
}

std::vector<OUString> VendorSettings::getSupportedVendors()
{
    std::vector<rtl::OUString> vecVendors;
    //get the nodeset for the library elements
    jfw::CXPathObjectPtr result;
    result = xmlXPathEvalExpression(
        (xmlChar*)"/jf:javaSelection/jf:plugins/jf:library",
        m_xmlPathContextVendorSettings);
    if (xmlXPathNodeSetIsEmpty(result->nodesetval))
        throw FrameworkException(
            JFW_E_ERROR,
            rtl::OString("[Java framework] Error in function getSupportedVendors (fwkutil.cxx)."));
    
    //get the values of the library elements + vendor attribute
    xmlNode* cur = result->nodesetval->nodeTab[0];
    while (cur != NULL)
    {
        //between library elements are also text elements
        if (cur->type == XML_ELEMENT_NODE)
        {
            jfw::CXmlCharPtr sAttrVendor(xmlGetProp(cur, (xmlChar*) "vendor"));
            vecVendors.push_back(sAttrVendor);
        }
        cur = cur->next;
    }
    return vecVendors;
}

OUString VendorSettings::getPluginLibrary(const OUString& sVendor)
{
    OSL_ASSERT(sVendor.getLength() > 0);

    OString sExcMsg("[Java framework] Error in function getPluginLibrary (fwkutil.cxx).");
    OString sVendorsPath = getVendorSettingsPath();
    OUStringBuffer usBuffer(256);
    usBuffer.appendAscii("/jf:javaSelection/jf:plugins/jf:library[@vendor=\"");
    usBuffer.append(sVendor);
    usBuffer.appendAscii("\"]/text()");
    OUString ouExpr = usBuffer.makeStringAndClear();
    OString sExpression =
        OUStringToOString(ouExpr, osl_getThreadTextEncoding());
    CXPathObjectPtr pathObjVendor;
    pathObjVendor = xmlXPathEvalExpression(
        (xmlChar*) sExpression.getStr(), m_xmlPathContextVendorSettings);
    if (xmlXPathNodeSetIsEmpty(pathObjVendor->nodesetval))
        throw FrameworkException(JFW_E_ERROR, sExcMsg);

    CXmlCharPtr xmlCharPlugin;
    xmlCharPlugin =
        xmlNodeListGetString(
            m_xmlDocVendorSettings,pathObjVendor->nodesetval->nodeTab[0], 1);

    //make an absolute file url from the relativ plugin URL
    OUString sUrl = findPlugin(xmlCharPlugin);
    if (sUrl.getLength() == 0)
    {
        OString sPlugin = OUStringToOString(
            xmlCharPlugin, osl_getThreadTextEncoding());
        throw FrameworkException(
                    JFW_E_CONFIGURATION,
                    "[Java framework] The file: " + sPlugin + " does not exist.");
    }
    return sUrl;
}

std::vector<OString> BootParams::getVMParameters()
{
    std::vector<OString> vecParams;

    for (sal_Int32 i = 1; ; i++)
    {
        OUString sName =
            OUString(RTL_CONSTASCII_USTRINGPARAM(UNO_JAVA_JFW_PARAMETER)) +
            OUString::valueOf(i);
        OUString sValue;
        if (getBootstrap().getFrom(sName, sValue) == sal_True)
        {
            OString sParam =
                OUStringToOString(sValue, osl_getThreadTextEncoding());
            vecParams.push_back(sParam);
#if OSL_DEBUG_LEVEL >=2
            rtl::OString sParamName = rtl::OUStringToOString(sName, osl_getThreadTextEncoding());
            fprintf(stderr,"[Java framework] Using bootstrap parameter %s"
                    " = %s.\n", sParamName.getStr(), sParam.getStr());
#endif
        }
        else
            break;
    }
    return vecParams;
}

rtl::OUString BootParams::getUserData()
{
    //do not cache the URL in a static because the user data file may not exist
    //at startup and is later created when data are read or written.
    //get the system path to the javasettings_xxx_xxx.xml file
    rtl::OUString retVal;
    sal_Bool bSuccess = getBootstrap().getFrom(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(UNO_JAVA_JFW_USER_DATA)),
        retVal);
#if OSL_DEBUG_LEVEL >=2
    if (bSuccess)
    {
        rtl::OString sValue = rtl::OUStringToOString(retVal, osl_getThreadTextEncoding());
        fprintf(stderr,"[Java framework] Using bootstrap parameter "
                UNO_JAVA_JFW_USER_DATA" = %s.\n", sValue.getStr());
    }
#endif
    return retVal;
}

rtl::OUString BootParams::getSharedData()
{
    rtl::OUString sShare;
    sal_Bool bSuccess = getBootstrap().getFrom(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(UNO_JAVA_JFW_SHARED_DATA)),
        sShare);
#if OSL_DEBUG_LEVEL >=2
    if (bSuccess)
    {
        rtl::OString sValue = rtl::OUStringToOString(sShare, osl_getThreadTextEncoding());
        fprintf(stderr,"[Java framework] Using bootstrap parameter "
                UNO_JAVA_JFW_SHARED_DATA" = %s.\n", sValue.getStr());
    }
#endif
    return sShare;
}



rtl::OString BootParams::getClasspath()
{
    rtl::OString sClassPath;
    rtl::OUString sCP;
    char szSep[] = {SAL_PATHSEPARATOR,0};
    if (getBootstrap().getFrom(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(UNO_JAVA_JFW_CLASSPATH)),
        sCP) == sal_True)
    {
        sClassPath = rtl::OUStringToOString(
            sCP, osl_getThreadTextEncoding());
#if OSL_DEBUG_LEVEL >=2
        fprintf(stderr,"[Java framework] Using bootstrap parameter "
            UNO_JAVA_JFW_CLASSPATH " = %s.\n", sClassPath.getStr());
#endif
    }
    
    rtl::OUString sEnvCP;
    if (getBootstrap().getFrom(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(UNO_JAVA_JFW_ENV_CLASSPATH)),
        sEnvCP) == sal_True)
    {
        char * pCp = getenv("CLASSPATH");
        if (pCp)
        {
            sClassPath += rtl::OString(szSep) + rtl::OString(pCp);
        }
#if OSL_DEBUG_LEVEL >=2
        fprintf(stderr,"[Java framework] Using bootstrap parameter "
            UNO_JAVA_JFW_ENV_CLASSPATH " and class path is:\n %s.\n", pCp ? pCp : "");
#endif
    }

    return sClassPath;
}

rtl::OUString BootParams::getVendorSettings()
{
    rtl::OUString sVendor;
    rtl::OUString sName(
        RTL_CONSTASCII_USTRINGPARAM(UNO_JAVA_JFW_VENDOR_SETTINGS));
    if (getBootstrap().getFrom(sName ,sVendor) == sal_True)
    {
        //check the value of the bootstrap variable
        jfw::FileStatus s = checkFileURL(sVendor);
        if (s != FILE_OK)
        {
            //This bootstrap parameter can contain a relative URL
            rtl::OUString sAbsoluteUrl;
            rtl::OUString sBaseDir = getLibraryLocation();
            if (File::getAbsoluteFileURL(sBaseDir, sVendor, sAbsoluteUrl)
                != File::E_None)
                throw FrameworkException(
                    JFW_E_CONFIGURATION,
                    rtl::OString("[Java framework] Invalid value for bootstrap variable: "
                             UNO_JAVA_JFW_VENDOR_SETTINGS));
            sVendor = sAbsoluteUrl;
            s = checkFileURL(sVendor);
            if (s == jfw::FILE_INVALID || s == jfw::FILE_DOES_NOT_EXIST)
            {
                throw FrameworkException(
                    JFW_E_CONFIGURATION,
                    rtl::OString("[Java framework] Invalid value for bootstrap variable: "
                                 UNO_JAVA_JFW_VENDOR_SETTINGS));
            }
        }
#if OSL_DEBUG_LEVEL >=2
    rtl::OString sValue = rtl::OUStringToOString(sVendor, osl_getThreadTextEncoding());
    fprintf(stderr,"[Java framework] Using bootstrap parameter "
            UNO_JAVA_JFW_VENDOR_SETTINGS" = %s.\n", sValue.getStr());
#endif
    }
    return sVendor;
}

rtl::OUString BootParams::getJREHome()
{
    rtl::OUString sJRE;
    rtl::OUString sEnvJRE;
    sal_Bool bJRE = getBootstrap().getFrom(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(UNO_JAVA_JFW_JREHOME)) ,sJRE);
    sal_Bool bEnvJRE = getBootstrap().getFrom(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(UNO_JAVA_JFW_ENV_JREHOME)) ,sEnvJRE);

    if (bJRE == sal_True && bEnvJRE == sal_True)
    {
        throw FrameworkException(
            JFW_E_CONFIGURATION,
            rtl::OString("[Java framework] Both bootstrap parameter "
                         UNO_JAVA_JFW_JREHOME" and "
                         UNO_JAVA_JFW_ENV_JREHOME" are set. However only one of them can be set."
                             "Check bootstrap parameters: environment variables, command line "
                             "arguments, rc/ini files for executable and java framework library."));
    }
    else if (bEnvJRE == sal_True)
    {
        const char * pJRE = getenv("JAVA_HOME");
        if (pJRE == NULL)
        {
            throw FrameworkException(
            JFW_E_CONFIGURATION,
            rtl::OString("[Java framework] Both bootstrap parameter "
                         UNO_JAVA_JFW_ENV_JREHOME" is set, but the environment variable "
                         "JAVA_HOME is not set."));
        }
        rtl::OString osJRE(pJRE);
        rtl::OUString usJRE = rtl::OStringToOUString(osJRE, osl_getThreadTextEncoding());
        if (File::getFileURLFromSystemPath(usJRE, sJRE) != File::E_None)
            throw FrameworkException(
                JFW_E_ERROR,
                rtl::OString("[Java framework] Error in function BootParams::getJREHome() "
                             "(fwkbase.cxx)."));
#if OSL_DEBUG_LEVEL >=2
    fprintf(stderr,"[Java framework] Using bootstrap parameter "
            UNO_JAVA_JFW_ENV_JREHOME" with JAVA_HOME = %s.\n", pJRE);
#endif
    }
    else if (getMode() == JFW_MODE_DIRECT
        && bEnvJRE == sal_False
        && bJRE == sal_False)
    {
        throw FrameworkException(
            JFW_E_CONFIGURATION,
            rtl::OString("[Java framework] The bootstrap parameter "
                         UNO_JAVA_JFW_ENV_JREHOME" or " UNO_JAVA_JFW_JREHOME
                         " must be set in direct mode."));
    }

#if OSL_DEBUG_LEVEL >=2
    if (bJRE == sal_True)
    {
        rtl::OString sValue = rtl::OUStringToOString(sJRE, osl_getThreadTextEncoding());
        fprintf(stderr,"[Java framework] Using bootstrap parameter "
            UNO_JAVA_JFW_JREHOME" = %s.\n", sValue.getStr());
    }
#endif    
    return sJRE;
}

rtl::OUString BootParams::getClasspathUrls()
{
    rtl::OUString sParams;
    getBootstrap().getFrom(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(UNO_JAVA_JFW_CLASSPATH_URLS)),
        sParams);
#if OSL_DEBUG_LEVEL >=2
    rtl::OString sValue = rtl::OUStringToOString(sParams, osl_getThreadTextEncoding());
    fprintf(stderr,"[Java framework] Using bootstrap parameter "
            UNO_JAVA_JFW_CLASSPATH_URLS " = %s.\n", sValue.getStr());
#endif
    return sParams;
}

JFW_MODE getMode()
{
    static bool g_bMode = false;
    static JFW_MODE g_mode = JFW_MODE_APPLICATION;

    if (g_bMode == false)
    {
        //check if either of the "direct mode" bootstrap variables is set
        bool bDirectMode = true;
        rtl::OUString sValue;
        const rtl::Bootstrap & aBoot = getBootstrap();
        rtl::OUString sJREHome(
            RTL_CONSTASCII_USTRINGPARAM(UNO_JAVA_JFW_JREHOME));
        if (aBoot.getFrom(sJREHome, sValue) == sal_False)
        {
            rtl::OUString sEnvJRE(
            RTL_CONSTASCII_USTRINGPARAM(UNO_JAVA_JFW_JREHOME));
            if (aBoot.getFrom(sEnvJRE, sValue) == sal_False)
            {
                rtl::OUString sClasspath(
                    RTL_CONSTASCII_USTRINGPARAM(UNO_JAVA_JFW_CLASSPATH));
                if (aBoot.getFrom(sClasspath, sValue) == sal_False)
                {
                    rtl::OUString sEnvClasspath(
                        RTL_CONSTASCII_USTRINGPARAM(UNO_JAVA_JFW_ENV_CLASSPATH));
                    if (aBoot.getFrom(sEnvClasspath, sValue) == sal_False)
                    {
                        rtl::OUString sParams = rtl::OUString(
                            RTL_CONSTASCII_USTRINGPARAM(UNO_JAVA_JFW_PARAMETER)) +
                            rtl::OUString::valueOf((sal_Int32)1);
                        if (aBoot.getFrom(sParams, sValue) == sal_False)
                        {
                            bDirectMode = false;
                        }
                    }
                }
            }
        }

        if (bDirectMode)
            g_mode = JFW_MODE_DIRECT;
        else
            g_mode = JFW_MODE_APPLICATION;
        g_bMode = true;
    }
        
    return g_mode;
}

rtl::OUString getApplicationClassPath()
{
    OSL_ASSERT(getMode() == JFW_MODE_APPLICATION);
    rtl::OUString retVal;
    rtl::OUString sParams = BootParams::getClasspathUrls();
    if (sParams.getLength() == 0)
        return retVal;

    rtl::OUStringBuffer buf;
    sal_Int32 index = 0;
    char szClassPathSep[] = {SAL_PATHSEPARATOR,0};
    do
    {
        ::rtl::OUString token( sParams.getToken( 0, ' ', index ).trim() );
        if (token.getLength())
        {
            ::rtl::OUString systemPathElement;
            oslFileError rc = osl_getSystemPathFromFileURL(
                token.pData, &systemPathElement.pData );
            OSL_ASSERT( rc == osl_File_E_None );
            if (rc == osl_File_E_None && systemPathElement.getLength() > 0)
            {
                if (buf.getLength() > 0)
                    buf.appendAscii( RTL_CONSTASCII_STRINGPARAM(
                                         szClassPathSep) );
                buf.append( systemPathElement );
            }
        }
    }
    while (index >= 0);
    return buf.makeStringAndClear();
}

rtl::OString makeClassPathOption(CNodeJava & javaSettings)
{
    //Compose the class path
    rtl::OString sPaths;
    rtl::OUStringBuffer sBufCP(4096);
    char szSep[] = {SAL_PATHSEPARATOR,0};
    JFW_MODE mode = getMode();
    bool bUseEnvClasspath = false;
    if (mode == JFW_MODE_APPLICATION)
    {
        // append all user selected jars to the class path
        rtl::OUString sUser = javaSettings.getUserClassPath();
        if (sUser.getLength() > 0)
        {
            sBufCP.append(sUser);
            sBufCP.appendAscii(szSep);
        }
        //append all jar libraries and components to the class path
        sBufCP.append(getApplicationClassPath());

        sPaths = rtl::OUStringToOString(
            sBufCP.makeStringAndClear(), osl_getThreadTextEncoding());
    }
    else if (mode == JFW_MODE_DIRECT)
    {
        sPaths = BootParams::getClasspath();
    }
    
    rtl::OString sOptionClassPath("-Djava.class.path=");
    sOptionClassPath += sPaths;
    return sOptionClassPath;
}
rtl::OString getUserSettingsStoreLocation()
{	
    OSL_ASSERT(getMode() == JFW_MODE_APPLICATION);
    static rtl::OString g_sUserSettings;
    if (g_sUserSettings.getLength() > 0)
        return g_sUserSettings;
    //get the system path to the javasettings_xxx_xxx.xml file
    rtl::OUString sUser = BootParams::getUserData();
    OSL_ENSURE(sUser.getLength() > 0, "[Java framework]: The bootstrap parameter "
               "UNO_JAVA_JFW_USER_DATA must be set in application mode!");
    if (sUser.getLength() > 0)
    {
        rtl::OUString sSystemPath;
        if (osl_getSystemPathFromFileURL(sUser.pData,
                                         & sSystemPath.pData) != osl_File_E_None)
            throw FrameworkException(
                JFW_E_ERROR, rtl::OString("[Java framework] Error in function "
                                          "getUserSettingsStoreLocation (fwkutil.cxx)."));
        g_sUserSettings =
            rtl::OUStringToOString(sSystemPath,osl_getThreadTextEncoding());
    }
    return g_sUserSettings;
}


rtl::OString getUserSettingsPath()
{
    rtl::OUString sURL = BootParams::getUserData();
    if (sURL.getLength() == 0)
        return rtl::OString();
    rtl::OUString sSystemPathSettings;
    if (osl_getSystemPathFromFileURL(sURL.pData,
        & sSystemPathSettings.pData) != osl_File_E_None)
        throw FrameworkException(
            JFW_E_ERROR, rtl::OString("[Java framework] Error in function "
                                      "getUserSettingsPath (fwkutil.cxx)."));

    rtl::OString osSystemPathSettings =
        rtl::OUStringToOString(sSystemPathSettings,osl_getThreadTextEncoding());
    return osSystemPathSettings;
}


rtl::OString getSharedSettingsPath()
{
    rtl::OUString sURL = BootParams::getSharedData();
    if (sURL.getLength() == 0)
        return rtl::OString();
    rtl::OUString sSystemPathSettings;
    if (osl_getSystemPathFromFileURL(sURL.pData,
        & sSystemPathSettings.pData) != osl_File_E_None)
        throw FrameworkException(
            JFW_E_ERROR,
            rtl::OString("[Java framework] Error in function "
                         "getShareSettingsPath (fwkutil.cxx)."));

    rtl::OString osSystemPathSettings =
        rtl::OUStringToOString(sSystemPathSettings,osl_getThreadTextEncoding());
    return osSystemPathSettings;
}

rtl::OString getVendorSettingsPath()
{
    rtl::OUString sURL = BootParams::getVendorSettings();
    if (sURL.getLength() == 0)
        return rtl::OString();
    rtl::OUString sSystemPathSettings;
    if (osl_getSystemPathFromFileURL(sURL.pData,
        & sSystemPathSettings.pData) != osl_File_E_None)
        throw FrameworkException(
            JFW_E_ERROR,
            rtl::OString("[Java framework] Error in function "
                         "getVendorSettingsPath (fwkutil.cxx) "));
    rtl::OString osSystemPathSettings =
        rtl::OUStringToOString(sSystemPathSettings,osl_getThreadTextEncoding());
    return osSystemPathSettings;
}



rtl::OUString getApplicationBase()
{
    rtl::OString sExcMsg("[Java framework] Error in function getApplicationBase (fwkutil.cxx)");

    rtl_uString* sExe = NULL;
    if (osl_getExecutableFile( & sExe) != osl_Process_E_None)
        throw FrameworkException(JFW_E_ERROR, sExcMsg);
    
    rtl::OUString ouExe(sExe, SAL_NO_ACQUIRE);
    rtl::OUString sBase = getDirFromFile(ouExe);
    
    OSL_ASSERT(sBase.getLength());
    return sBase;
}
void setJavaSelected()
{
    g_bJavaSet = true;
}

bool wasJavaSelectedInSameProcess()
{
    //g_setJavaProcId not set means no Java selected
    if (g_bJavaSet == true)
        return true;
    return false;
}


}
