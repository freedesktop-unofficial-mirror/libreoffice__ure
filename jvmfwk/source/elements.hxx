/*************************************************************************
 *
 *  $RCSfile: elements.hxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: jl $ $Date: 2004-04-19 15:55:35 $
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
#if !defined INCLUDED_JVMFWK_ELEMENTS_HXX
#define INCLUDED_JVMFWK_ELEMENTS_HXX

#include <vector>
#include "jvmfwk/framework.h"
#include "rtl/ustring.hxx"
#include "rtl/byteseq.hxx"
#include "libxml/parser.h"
namespace jfw
{

class CXmlCharPtr;
class CNodeJavaInfo
{
public:
    CNodeJavaInfo();
    ~CNodeJavaInfo();
    /**
       sUpdated is the value from the <updated> element from the
       javavendors.xml.
     */
    CNodeJavaInfo(const JavaInfo * pInfo, const rtl::OUString& sUpdated);

    /** if true, then javaInfo is empty. When writeToNode is called
        then all child elements are deleted.
     */
    bool m_bEmptyNode;
    /** Contains the value of the <updated> element of
        the javavendors.xml
     */
    rtl::OUString sAttrVendorUpdate;
    /** contains the nil value of the /java/javaInfo@xsi:nil attribute.
        Default is true;
     */
    bool bNil;
    rtl::OUString sVendor;
    rtl::OUString sLocation;
    rtl::OUString sVersion;
    sal_uInt64 nFeatures;
    sal_uInt64 nRequirements;
    rtl::ByteSequence arVendorData;

    /** reads the node /java/javaInfo.
        If javaInfo@xsi:nil = true then member bNil is set to true
        an no further elements are read.
     */
    javaFrameworkError loadFromNode(xmlDoc * pDoc,xmlNode * pJavaInfo);
    /** Only writes user settings. The attribut nil always gets the value
        false;
     */
    javaFrameworkError writeToNode(xmlDoc * pDoc, xmlNode * pJavaInfo) const;

    /** returns NULL if javaInfo is nil in both, user and share, settings.
     */
    JavaInfo * makeJavaInfo() const;
};

/** this class represents the javasettings.xml file
 */
class CNodeJava
{
    /** Share settings are a special case. Per default
        there are only user settings.
     */
    javaFrameworkError loadShareSettings();
    /** This function is called after loadShareSettings. Elements which have been
        modified by the user, that is, the attribute xsi:nil = false, overwrite the
        values which have been retrieved with loadShareSettings.
    */
    javaFrameworkError loadUserSettings();

    /** User configurable option.  /java/enabled
        The value is valid after loadFromSettings has been called
        successfully.
        The value is that of the user setting. If it is nil
        (/java/enabled[@xsi:nil = true]) then it represents the share setting.
        If there are no share settings or the node is also nil then the default
        is true.
    */
    sal_Bool m_bEnabled;
    /** Determines if m_bEnabled has been modified */
    bool m_bEnabledModified;
    /** User configurable option. /java/userClassPath
        The value is valid after loadFromSettings has been called successfully.
        The value is that of the user setting. If it is nil
        (/java/userClassPath[@xsi:nil = true]) then it represents the share setting.
        If there are no share settings or the node is also nil then the default
        is an empty string.
    */
    rtl::OUString m_sUserClassPath;
    /** Determines if m_sUserClassPath has been modified */
    bool m_bUserClassPathModified;
    /** User configurable option.  /java/javaInfo
        The value is valid after loadFromSettings has been called successfully.
        The value is that of the user setting. If it is nil
        (/java/javaInfo[@xsi:nil = true]) then it represents the share setting.
        If there are no share settings then the structure is regarded as empty.
     */
    CNodeJavaInfo m_aInfo;
    /** Determines if m_aInfo has been modified */
    bool m_bJavaInfoModified;

    /** User configurable option. /java/vmParameters
        The value is valid after loadFromSettings has been called successfully.
        The value is that of the user setting. If it is nil
        (/java/vmParameters[@xsi:nil = true]) then it represents the share setting.
        If there are no share settings then array is empty.
    */
    std::vector<rtl::OString>  m_arVmParameters;
    bool m_bVmParametersModified;
public:



    // Preset element (cannot be changed.  /java/classesDirectory
    rtl::OUString m_sClassesDirectory;

    
    CNodeJava();
    /** sets m_bEnabled. It also sets a flag, that the value has been
        modified. This will cause that /java/enabled[@xsi:nil] will be
        set to false. The nil value and the value of enabled are only
        written when write Settings is called.
     */
    void setEnabled(sal_Bool bEnabled);
    /** returns the value of the element /java/enabled
     */
    sal_Bool getEnabled() const;
    /** sets m_sUserClassPath. Analog to setEnabled.
     */
    void setUserClassPath(const rtl::OUString & sClassPath);
    /** returns the value of the element /java/userClassPath.
     */
    rtl::OUString const & getUserClassPath() const;
    /** sets m_aInfo. Analog to setEnabled.
        @param sVendorUpdated
        The date string that is written to /java/javaInfo@vendorUpdate
        The string is the same as the value of /javaSettings/updated in javavendors.xml
     */
    void setJavaInfo(const JavaInfo * pInfo, const rtl::OUString& sVendorUpdated);
    /** returns a JavaInfo structure representing the node
        /java/javaInfo
        If both, user and share settings are nil, then NULL is returned.
    */
    JavaInfo * getJavaInfo() const;
    /** returns the value of the attribute /java/javaInfo[@vendorUpdate].
     */
    rtl::OUString const & getJavaInfoAttrVendorUpdate() const;
    /** sets the /java/vmParameters/param elements.
        The values are kept in a vector m_arVmParameters. When this method is
        called then the vector is cleared and the new values are inserted.
        The xsi:nil attribute of vmParameters will be set to true;
     */
    void setVmParameters(rtl_uString  * * arParameters, sal_Int32 size);
    /** returns the parameters from the element /java/vmParameters/param.
     */
    const std::vector<rtl::OString> & getVmParameters();

    
    /** reads user and share settings. user data supersede
        share data. These elements can be changed by the user:
        <enabled>, <userClasspath>, <javaInfo>, <vmParameters>
        If the user has not changed them then the nil attribute is
        set to true;
     */
    javaFrameworkError loadFromSettings();
    /** writes the data to user settings.
     */
    javaFrameworkError writeSettings() const;
    
    /** returns an array.
        Caller must free the strings and the array.
     */
    void getVmParametersArray(rtl_uString *** parParameters, sal_Int32 * size);
};



class VersionInfo
{
    std::vector<rtl::OUString> vecExcludeVersions;
    rtl_uString ** arVersions;

public:
    VersionInfo();
    ~VersionInfo();

    void addExcludeVersion(const rtl::OUString& sVersion);
    
    rtl::OUString sMinVersion;
    rtl::OUString sMaxVersion;

    /** The caller DOES NOT get ownership of the strings. That is he
        does not need to release the strings.
        The array exists as long as this object exists.
    */
    
    rtl_uString** getExcludeVersions();
    sal_Int32 getExcludeVersionSize();
};

struct PluginLibrary
{
    /** contains the vendor string which is later userd in the xml API
     */
    rtl::OString sVendor;
    /** File URL the plug-in library
     */
    rtl::OUString sPath;
};

} //end namespace
#endif
