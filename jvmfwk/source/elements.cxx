/*************************************************************************
 *
 *  $RCSfile: elements.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: jl $ $Date: 2004-04-19 15:54:55 $
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
#include "elements.hxx"
#include "osl/mutex.hxx"
#include "fwkutil.hxx"
#include "libxmlutil.hxx"
#include "osl/thread.hxx"
#include "libxml/parser.h"
#include "libxml/xpath.h"
#include "libxml/xpathinternals.h"
#define NS_JAVA_FRAMEWORK "http://openoffice.org/2004/java/framework/1.0"
#define NS_SCHEMA_INSTANCE "http://www.w3.org/2001/XMLSchema-instance"

namespace jfw
{

//====================================================================
VersionInfo::VersionInfo(): arVersions(NULL)
{
}

VersionInfo::~VersionInfo()
{
    delete [] arVersions;
}

void VersionInfo::addExcludeVersion(const rtl::OUString& sVersion)
{
    vecExcludeVersions.push_back(sVersion);
}

rtl_uString** VersionInfo::getExcludeVersions()
{
    osl::MutexGuard guard(getFwkMutex());
    if (arVersions != NULL)
        return arVersions;

    arVersions = new rtl_uString*[vecExcludeVersions.size()];
    int j=0;
    typedef std::vector<rtl::OUString>::const_iterator it;
    for (it i = vecExcludeVersions.begin(); i != vecExcludeVersions.end();
         i++, j++)
    {
        arVersions[j] = vecExcludeVersions[j].pData;
    }
    return arVersions;
}

sal_Int32 VersionInfo::getExcludeVersionSize()
{
    return vecExcludeVersions.size();
}
//==================================================================

CNodeJava::CNodeJava():
    m_bEnabled(sal_True), m_bEnabledModified(false),
    m_bUserClassPathModified(false), m_bJavaInfoModified(false),
    m_bVmParametersModified(false)
{
}

void CNodeJava::getVmParametersArray(rtl_uString *** parParams, sal_Int32 * size)
{
    osl::MutexGuard guard(getFwkMutex());
    OSL_ASSERT(parParams != NULL && size != NULL);
    
    *parParams = (rtl_uString **)
        rtl_allocateMemory(sizeof(rtl_uString*) * m_arVmParameters.size());
    if (*parParams == NULL)
        return;
        
    int j=0;
    typedef std::vector<rtl::OString>::const_iterator it;
    for (it i = m_arVmParameters.begin(); i != m_arVmParameters.end();
         i++, j++)
    {
        rtl::OUString sParam =
            rtl::OStringToOUString(*i, RTL_TEXTENCODING_UTF8);
        (*parParams)[j] = sParam.pData;
        rtl_uString_acquire(sParam.pData);
    }
    *size = m_arVmParameters.size();
}

javaFrameworkError CNodeJava::loadFromSettings()
{
    javaFrameworkError errcode = JFW_E_NONE;
    // share settings may not be given
    errcode = loadShareSettings();
    OSL_ASSERT(errcode == JFW_E_NONE);
    if (errcode == JFW_E_NONE)
        errcode = loadUserSettings();
    return errcode;
}


javaFrameworkError CNodeJava::loadUserSettings()
{
    javaFrameworkError errcode = JFW_E_NONE;
    CXmlDocPtr docUser;

    //Read the user elements
    rtl::OString sSettingsPath = jfw::getUserSettingsPath();
    //There must not be a share settings file
    docUser = xmlParseFile(sSettingsPath.getStr());
    if (docUser == NULL)
        return JFW_E_CONFIG_READWRITE;

    xmlNode * cur = xmlDocGetRootElement(docUser);
    if (cur == NULL || cur->children == NULL)
        return JFW_E_FORMAT_STORE;

    cur = cur->children;
    while (cur != NULL)
    {
        if (xmlStrcmp(cur->name, (xmlChar*) "enabled") == 0)
        {
            //only overwrite share settings if xsi:nil="false"
            CXmlCharPtr sNil = xmlGetNsProp(
                cur, (xmlChar*) "nil", (xmlChar*) NS_SCHEMA_INSTANCE);
            if (sNil == NULL)
            {
                OSL_ASSERT(0);
                return JFW_E_FORMAT_STORE;
            }
            if (xmlStrcmp(sNil, (xmlChar*) "false") == 0)
            {
                CXmlCharPtr sEnabled = xmlNodeListGetString(
                    docUser, cur->children, 1);
                if (xmlStrcmp(sEnabled, (xmlChar*) "true") == 0)
                    m_bEnabled = sal_True;
                else if (xmlStrcmp(sEnabled, (xmlChar*) "false") == 0)
                    m_bEnabled = sal_False;
                else
                    m_bEnabled = sal_True;
            }
        }
        else if (xmlStrcmp(cur->name, (xmlChar*) "classesDirectory") == 0)
        {
            CXmlCharPtr sCls = xmlNodeListGetString(
                docUser, cur->children, 1);
            m_sClassesDirectory = sCls;
        }
        else if (xmlStrcmp(cur->name, (xmlChar*) "userClassPath") == 0)
        {
            CXmlCharPtr sNil = xmlGetNsProp(
                cur, (xmlChar*) "nil", (xmlChar*) NS_SCHEMA_INSTANCE);
            if (sNil == NULL)
            {
                OSL_ASSERT(0);
                return JFW_E_FORMAT_STORE;
            }
            if (xmlStrcmp(sNil, (xmlChar*) "false") == 0)
            {
                CXmlCharPtr sUser = xmlNodeListGetString(
                    docUser, cur->children, 1);
                m_sUserClassPath = sUser;
            }
        }
        else if (xmlStrcmp(cur->name, (xmlChar*) "javaInfo") == 0)
        {
            CXmlCharPtr sNil = xmlGetNsProp(
                cur, (xmlChar*) "nil", (xmlChar*) NS_SCHEMA_INSTANCE);
            if (sNil == NULL)
            {
                OSL_ASSERT(0);
                return JFW_E_FORMAT_STORE;
            }
            if (xmlStrcmp(sNil, (xmlChar*) "false") == 0)
            {
                m_aInfo.loadFromNode(docUser, cur);
            }
        }
        else if (xmlStrcmp(cur->name, (xmlChar*) "vmParameters") == 0)
        {
            CXmlCharPtr sNil = xmlGetNsProp(
                cur, (xmlChar*) "nil", (xmlChar*) NS_SCHEMA_INSTANCE);
            if (sNil == NULL)
            {
                OSL_ASSERT(0);
                return JFW_E_FORMAT_STORE;
            }
            if (xmlStrcmp(sNil, (xmlChar*) "false") == 0)
            {
                //throw away share settings
                m_arVmParameters.clear();
                xmlNode * pOpt = cur->children;
                while (pOpt != NULL)
                {
                    if (xmlStrcmp(pOpt->name, (xmlChar*) "param") == 0)
                    {
                        CXmlCharPtr sOpt = xmlNodeListGetString(
                            docUser, pOpt->children, 1);
                        m_arVmParameters.push_back(sOpt);
                    }
                    pOpt = pOpt->next;
                }
            }
        }
        cur = cur->next;
    }    
    return errcode;
}

javaFrameworkError CNodeJava::loadShareSettings()
{
    javaFrameworkError errcode = JFW_E_NONE;
    CXmlDocPtr docShare;

    //Read the share elements, do not head the nil attributes
    rtl::OString sSettingsPath = jfw::getSharedSettingsPath();
    //There must not be a share settings file
    docShare = xmlParseFile(sSettingsPath.getStr());
    if (docShare == NULL)
        return JFW_E_NONE;

    xmlNode * cur = xmlDocGetRootElement(docShare);
    if (cur == NULL)
    {
        OSL_ASSERT(cur);
        return JFW_E_FORMAT_STORE;
    }
    if (cur->children == NULL)
        return JFW_E_NONE;
    cur = cur->children;
    while (cur != NULL)
    {
        if (xmlStrcmp(cur->name, (xmlChar*) "enabled") == 0)
        {
            CXmlCharPtr sEnabled = xmlNodeListGetString(
                docShare, cur->children, 1);
            if (xmlStrcmp(sEnabled, (xmlChar*) "true") == 0)
                m_bEnabled = sal_True;
            else if (xmlStrcmp(sEnabled, (xmlChar*) "false") == 0)
                m_bEnabled = sal_False;
            else
                m_bEnabled = sal_True;
        }
        else if (xmlStrcmp(cur->name, (xmlChar*) "classesDirectory") == 0)
        {
            CXmlCharPtr sCls = xmlNodeListGetString(
                docShare, cur->children, 1);
            m_sClassesDirectory = sCls;
        }
        else if (xmlStrcmp(cur->name, (xmlChar*) "userClassPath") == 0)
        {
            CXmlCharPtr sUser = xmlNodeListGetString(
                docShare, cur->children, 1);
            m_sUserClassPath = sUser;
        }
        else if (xmlStrcmp(cur->name, (xmlChar*) "javaInfo") == 0)
        {
            m_aInfo.loadFromNode(docShare, cur);
        }
        else if (xmlStrcmp(cur->name, (xmlChar*) "vmParameters") == 0)
        {
            CXmlCharPtr sNil = xmlGetNsProp(
                cur, (xmlChar*) "nil", (xmlChar*) NS_SCHEMA_INSTANCE);
            if (sNil == NULL)
            {
                OSL_ASSERT(0);
                return JFW_E_FORMAT_STORE;
            }
            if (xmlStrcmp(sNil, (xmlChar*) "false") == 0)
            {
                xmlNode * pOpt = cur->children;
                while (pOpt != NULL)
                {
                    if (xmlStrcmp(pOpt->name, (xmlChar*) "param") == 0)
                    {
                        CXmlCharPtr sOpt = xmlNodeListGetString(
                            docShare, pOpt->children, 1);
                        m_arVmParameters.push_back(sOpt);
                    }
                    pOpt = pOpt->next;
                }
            }
        }

        cur = cur->next;
    }    
    return errcode;
}

javaFrameworkError CNodeJava::writeSettings() const
{
    javaFrameworkError errcode = JFW_E_NONE;
    CXmlDocPtr docUser;
    CXPathContextPtr contextUser;
    CXPathObjectPtr pathObj;

    //Read the user elements
    rtl::OString sSettingsPath = jfw::getUserSettingsPath();
    docUser = xmlParseFile(sSettingsPath.getStr());
    if (docUser == NULL)
        return JFW_E_CONFIG_READWRITE;
    contextUser = xmlXPathNewContext(docUser);
    if (xmlXPathRegisterNs(contextUser, (xmlChar*) "jf",
        (xmlChar*) NS_JAVA_FRAMEWORK) == -1)
        return JFW_E_CONFIG_READWRITE;

    xmlNode * root = xmlDocGetRootElement(docUser);
    //Get xsi:nil namespace
    xmlNs* nsXsi = xmlSearchNsByHref(docUser, 
                             root,
                             (xmlChar*)  NS_SCHEMA_INSTANCE);

    //set the <enabled> element
    //The element must exist
    if (m_bEnabledModified)
    {
        rtl::OString sExpression= rtl::OString(
            "/jf:java/jf:enabled");
        pathObj = xmlXPathEvalExpression((xmlChar*) sExpression.getStr(),
                                         contextUser);
        if ( ! pathObj || xmlXPathNodeSetIsEmpty(pathObj->nodesetval))
            return JFW_E_FORMAT_STORE;
        
        xmlNode * nodeEnabled = pathObj->nodesetval->nodeTab[0];
        xmlSetNsProp(nodeEnabled,
                     nsXsi,
                     (xmlChar*) "nil",
                     (xmlChar*) "false");

        if (m_bEnabled == sal_True)
            xmlNodeSetContent(nodeEnabled,(xmlChar*) "true");
        else
            xmlNodeSetContent(nodeEnabled,(xmlChar*) "false");
    }

    //set the <userClassPath> element
    //The element must exist
    if (m_bUserClassPathModified)
    {
        rtl::OString sExpression= rtl::OString(
            "/jf:java/jf:userClassPath");
        pathObj = xmlXPathEvalExpression((xmlChar*) sExpression.getStr(),
                                         contextUser);
        if ( ! pathObj || xmlXPathNodeSetIsEmpty(pathObj->nodesetval))
            return JFW_E_FORMAT_STORE;
        
        xmlNode * nodeEnabled = pathObj->nodesetval->nodeTab[0];
        xmlSetNsProp(nodeEnabled, nsXsi, (xmlChar*) "nil",(xmlChar*) "false");

        rtl::OString osUserCP =
            rtl::OUStringToOString(m_sUserClassPath, osl_getThreadTextEncoding());
        xmlNodeSetContent(nodeEnabled,(xmlChar*) osUserCP.getStr());
    }

    //set <javaInfo> element
    if (m_bJavaInfoModified)
    {
        rtl::OString sExpression= rtl::OString(
            "/jf:java/jf:javaInfo");                    
        pathObj = xmlXPathEvalExpression((xmlChar*) sExpression.getStr(),
                                                contextUser);
        if ( ! pathObj || xmlXPathNodeSetIsEmpty(pathObj->nodesetval))
            return JFW_E_FORMAT_STORE;

        errcode = m_aInfo.writeToNode(
            docUser, pathObj->nodesetval->nodeTab[0]);
        if (errcode != JFW_E_NONE)
            return errcode;
    }

    //set <vmParameters> element
    if (m_bVmParametersModified)
    {
        rtl::OString sExpression= rtl::OString(
            "/jf:java/jf:vmParameters");                    
        pathObj = xmlXPathEvalExpression((xmlChar*) sExpression.getStr(),
                                         contextUser);
        if ( ! pathObj || xmlXPathNodeSetIsEmpty(pathObj->nodesetval))
            return JFW_E_FORMAT_STORE;
        xmlNode* vmParameters = pathObj->nodesetval->nodeTab[0];
        //set xsi:nil = false;
        xmlSetNsProp(vmParameters, nsXsi,(xmlChar*) "nil",
                     (xmlChar*) "false");

        //remove option elements
        xmlNode* cur = vmParameters->children;
        while (cur != NULL)
        {
            xmlNode* lastNode = cur;
            cur = cur->next;
            xmlUnlinkNode(lastNode);
            xmlFreeNode(lastNode);
        }
        //add a new line after <vmParameters>
        if (m_arVmParameters.size() > 0)
        {
            xmlNode * nodeCrLf = xmlNewText((xmlChar*) "\n");
            xmlAddChild(vmParameters, nodeCrLf);
        }

        typedef std::vector<rtl::OString>::const_iterator cit;
        for (cit i = m_arVmParameters.begin(); i != m_arVmParameters.end(); i++)
        {
            xmlNewTextChild(vmParameters, NULL, (xmlChar*) "option",
                            (xmlChar*) i->getStr());
            //add a new line
            xmlNode * nodeCrLf = xmlNewText((xmlChar*) "\n");
            xmlAddChild(vmParameters, nodeCrLf);
        }
    }
        

    //set <userClassPath>, element must exist
//    sExpression =
    if (xmlSaveFormatFile(sSettingsPath.getStr(), docUser, 1) == -1)
        return JFW_E_CONFIG_READWRITE;

    return JFW_E_NONE;
}

void CNodeJava::setEnabled(sal_Bool bEnabled)
{
    m_bEnabled = bEnabled;
    m_bEnabledModified = true;
}

sal_Bool CNodeJava::getEnabled() const
{
    return m_bEnabled;
}

void CNodeJava::setUserClassPath(const rtl::OUString & sClassPath)
{
    m_sUserClassPath = sClassPath;
    m_bUserClassPathModified = true;
}

rtl::OUString const &  CNodeJava::getUserClassPath() const
{
    return m_sUserClassPath;
}

void CNodeJava::setJavaInfo(const JavaInfo * pInfo,
                            const rtl::OUString & sVendorUpdate)
{
    OSL_ASSERT(sVendorUpdate.getLength() > 0);
    m_aInfo.bNil = false;
    m_aInfo.sAttrVendorUpdate = sVendorUpdate;
    if (pInfo != NULL)
    {
        m_aInfo.m_bEmptyNode = false;
        m_aInfo.sVendor = pInfo->sVendor;
        m_aInfo.sLocation = pInfo->sLocation;
        m_aInfo.sVersion = pInfo->sVersion;
        m_aInfo.nFeatures = pInfo->nFeatures;
        m_aInfo.nRequirements = pInfo->nRequirements;
        m_aInfo.arVendorData = pInfo->arVendorData;
    }
    else
    {
        m_aInfo.m_bEmptyNode = true;
        rtl::OUString sEmpty;
        m_aInfo.sVendor = sEmpty;
        m_aInfo.sLocation = sEmpty;
        m_aInfo.sVersion = sEmpty;
        m_aInfo.nFeatures = 0;
        m_aInfo.nRequirements = 0;
        m_aInfo.arVendorData = rtl::ByteSequence();
    }
    m_bJavaInfoModified = true;
}

JavaInfo * CNodeJava::getJavaInfo() const
{
    return m_aInfo.makeJavaInfo();
}

rtl::OUString const & CNodeJava::getJavaInfoAttrVendorUpdate() const
{
    return m_aInfo.sAttrVendorUpdate;
}

const std::vector<rtl::OString> & CNodeJava::getVmParameters()
{
    return m_arVmParameters;
}

void CNodeJava::setVmParameters(rtl_uString * * arOptions, sal_Int32 size)
{
    OSL_ASSERT( !(arOptions == 0 && size != 0));
    m_arVmParameters.clear();
    if (arOptions != NULL)
    {
        for (int i  = 0; i < size; i++)
        {
            const rtl::OUString usOption = (rtl_uString*) arOptions[i];
            rtl::OString osOption = rtl::OUStringToOString(
                usOption, RTL_TEXTENCODING_UTF8);
            m_arVmParameters.push_back(osOption);
        }
    }
    m_bVmParametersModified = true;
}

//=====================================================================
CNodeJavaInfo::CNodeJavaInfo() :
    nFeatures(0), nRequirements(0), bNil(true), m_bEmptyNode(false)
{
}

CNodeJavaInfo::~CNodeJavaInfo()
{
}

CNodeJavaInfo::CNodeJavaInfo(const JavaInfo * pInfo,
                             const rtl::OUString & sUpdated)
{
    if (pInfo != NULL)
    {
        sAttrVendorUpdate = sUpdated;
        sVendor = pInfo->sVendor;
        sLocation = pInfo->sLocation;
        sVersion = pInfo->sVersion;
        nFeatures = pInfo->nFeatures;
        nRequirements = pInfo->nRequirements;
        arVendorData = pInfo->arVendorData;
    }
}
javaFrameworkError CNodeJavaInfo::loadFromNode(xmlDoc * pDoc, xmlNode * pJavaInfo)
{
    javaFrameworkError errcode = JFW_E_NONE;
    OSL_ASSERT(pJavaInfo && pDoc);
    if (pJavaInfo->children == NULL)
        return JFW_E_NONE;
    //Get the xsi:nil attribute;
    CXmlCharPtr sNil = xmlGetNsProp(
        pJavaInfo, (xmlChar*) "nil", (xmlChar*) NS_SCHEMA_INSTANCE);
    if ( ! sNil)
        return JFW_E_FORMAT_STORE;
    if (xmlStrcmp(sNil, (xmlChar*) "true") == 0)
        bNil = true;
    else if (xmlStrcmp(sNil, (xmlChar*) "false") == 0)
        bNil = false;
    else
        return JFW_E_FORMAT_STORE;
    if (bNil == true)
        return JFW_E_NONE;
    
    xmlNode * cur = pJavaInfo->children;

    while (cur != NULL)
    {
        if (xmlStrcmp(cur->name, (xmlChar*) "vendor") == 0)
        {
            CXmlCharPtr xmlVendor = xmlNodeListGetString(
                pDoc, cur->children, 1);
            if (! xmlVendor)
                return JFW_E_NONE;
            sVendor = xmlVendor;
        }
        else if (xmlStrcmp(cur->name, (xmlChar*) "location") == 0)
        {
            CXmlCharPtr xmlLocation = xmlNodeListGetString(
                pDoc, cur->children, 1);
            sLocation = xmlLocation;
        }
        else if (xmlStrcmp(cur->name, (xmlChar*) "version") == 0)
        {
            CXmlCharPtr xmlVersion = xmlNodeListGetString(
                pDoc, cur->children, 1);
            sVersion = xmlVersion;
        }
        else if (xmlStrcmp(cur->name, (xmlChar*) "features")== 0)
        {
            CXmlCharPtr xmlFeatures = xmlNodeListGetString(
                    pDoc, cur->children, 1);
            rtl::OUString sFeatures = xmlFeatures;
            nFeatures = sFeatures.toInt64(16);
        }
        else if (xmlStrcmp(cur->name, (xmlChar*) "requirements") == 0)
        {
            CXmlCharPtr xmlRequire = xmlNodeListGetString(
                pDoc, cur->children, 1);
            rtl::OUString sRequire = xmlRequire;
            nRequirements = sRequire.toInt64(16);
        }
        else if (xmlStrcmp(cur->name, (xmlChar*) "vendorData") == 0)
        {
            CXmlCharPtr xmlData = xmlNodeListGetString(
                pDoc, cur->children, 1);
            xmlChar* _data = (xmlChar*) xmlData;
            if (_data)
            {
                rtl::ByteSequence seq((sal_Int8*) _data, strlen((char*)_data));
                arVendorData = decodeBase16(seq);
            }
        }
        cur = cur->next;
    }

    //Get the javainfo attributes
    CXmlCharPtr sVendorUpdate = xmlGetProp(pJavaInfo,
                                    (xmlChar*) "vendorUpdate");
    if ( ! sVendorUpdate)
        return JFW_E_FORMAT_STORE;
    sAttrVendorUpdate = sVendorUpdate;


    return errcode;
}


javaFrameworkError CNodeJavaInfo::writeToNode(xmlDoc* pDoc,
                                              xmlNode* pJavaInfoNode) const
                                       
{
    OSL_ASSERT(pJavaInfoNode && pDoc);
    javaFrameworkError errcode = JFW_E_NONE;
    //write the attribute vendorSettings
    rtl::OString osUpdate = rtl::OUStringToOString(
        sAttrVendorUpdate, osl_getThreadTextEncoding());
    //creates the attribute if necessary
    xmlSetProp(pJavaInfoNode, (xmlChar*)"vendorUpdate",
                   (xmlChar*) osUpdate.getStr());

    //Set xsi:nil in javaInfo element to false
    //the xmlNs pointer must not be destroyed
    xmlNs* nsXsi = xmlSearchNsByHref((xmlDoc*) pDoc, 
                             pJavaInfoNode,
                             (xmlChar*)  NS_SCHEMA_INSTANCE);

    xmlSetNsProp(pJavaInfoNode,
                 nsXsi,
                 (xmlChar*) "nil",
                 (xmlChar*) "false");

    //Check if the JavaInfo was set with an empty value, then all
    //children are deleted
    if (m_bEmptyNode && pJavaInfoNode->children != NULL)
    {
        xmlNode* cur = pJavaInfoNode->children;
        while (cur != NULL)
        {
            xmlNode* lastNode = cur;
            cur = cur->next;
            xmlUnlinkNode(lastNode);
            xmlFreeNode(lastNode);
        }
        return errcode;
    }

    //add a new line after <javaInfo>
    xmlNode * nodeCrLf = xmlNewText((xmlChar*) "\n");
    xmlAddChild(pJavaInfoNode, nodeCrLf);
    
    //Create the vendor element
    rtl::OString osVendor = rtl::OUStringToOString(
        sVendor, osl_getThreadTextEncoding());
    xmlNewTextChild(pJavaInfoNode, NULL, (xmlChar*) "vendor",
                    (xmlChar*) osVendor.getStr());
    //add a new line for better readability
    nodeCrLf = xmlNewText((xmlChar*) "\n");
    xmlAddChild(pJavaInfoNode, nodeCrLf);

    //Create the location element
    rtl::OString osLocation = rtl::OUStringToOString(
        sLocation, osl_getThreadTextEncoding());

    xmlNewTextChild(pJavaInfoNode, NULL, (xmlChar*) "location",
                    (xmlChar*) osLocation.getStr());
    //add a new line for better readability
    nodeCrLf = xmlNewText((xmlChar*) "\n");
    xmlAddChild(pJavaInfoNode, nodeCrLf);
    
    //Create the version element
    rtl::OString osVersion = rtl::OUStringToOString(
        sVersion, osl_getThreadTextEncoding());

    xmlNewTextChild(pJavaInfoNode, NULL, (xmlChar*) "version",
                    (xmlChar*) osVersion.getStr());
    //add a new line for better readability
    nodeCrLf = xmlNewText((xmlChar*) "\n");
    xmlAddChild(pJavaInfoNode, nodeCrLf);
    
    //Create the features element
    rtl::OUString usFeatures = rtl::OUString::valueOf(
        (sal_Int64)nFeatures, 16);
    rtl::OString sFeatures = rtl::OUStringToOString(
        usFeatures, osl_getThreadTextEncoding());
    
    xmlNewTextChild(pJavaInfoNode, NULL, (xmlChar*) "features",
                    (xmlChar*) sFeatures.getStr());
    //add a new line for better readability
    nodeCrLf = xmlNewText((xmlChar*) "\n");
    xmlAddChild(pJavaInfoNode, nodeCrLf);
    

    //Create the requirements element
    rtl::OUString usRequirements = rtl::OUString::valueOf(
        (sal_Int64) nRequirements, 16);
    rtl::OString sRequirements = rtl::OUStringToOString(
        usRequirements, osl_getThreadTextEncoding());
    
    xmlNewTextChild(pJavaInfoNode, NULL, (xmlChar*) "requirements",
                    (xmlChar*) sRequirements.getStr());
    //add a new line for better readability
    nodeCrLf = xmlNewText((xmlChar*) "\n");
    xmlAddChild(pJavaInfoNode, nodeCrLf);
    

    //Create the features element
    rtl::ByteSequence data = encodeBase16(arVendorData);
    xmlNode* dataNode = xmlNewChild(pJavaInfoNode, NULL,
                                    (xmlChar*) "vendorData",
                                    (xmlChar*) "");
    xmlNodeSetContentLen(dataNode,
                         (xmlChar*) data.getArray(), data.getLength());
    //add a new line for better readability
    nodeCrLf = xmlNewText((xmlChar*) "\n");
    xmlAddChild(pJavaInfoNode, nodeCrLf);

    return JFW_E_NONE;
}

JavaInfo * CNodeJavaInfo::makeJavaInfo() const
{
    if (bNil == true)
        return NULL;
    JavaInfo * pInfo = (JavaInfo*) rtl_allocateMemory(sizeof JavaInfo);
    if (pInfo == NULL)
        return NULL;
    memset(pInfo, 0, sizeof JavaInfo);
    pInfo->sVendor = sVendor.pData;
    rtl_uString_acquire(pInfo->sVendor);
    pInfo->sLocation = sLocation.pData;
    rtl_uString_acquire(pInfo->sLocation);
    pInfo->sVersion = sVersion.pData;
    rtl_uString_acquire(pInfo->sVersion);
    pInfo->nFeatures = nFeatures;
    pInfo->nRequirements = nRequirements;
    pInfo->arVendorData = arVendorData.getHandle();
    rtl_byte_sequence_acquire(pInfo->arVendorData);
    return pInfo;
}
}
