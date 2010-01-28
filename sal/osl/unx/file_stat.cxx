/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: file_stat.cxx,v $
 * $Revision: 1.8 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sal.hxx"

#ifndef _TYPES_H
#include <sys/types.h>
#endif

#ifndef _LIMITS_H
#include <limits.h>
#endif

#ifndef _UNISTD_H
#include <unistd.h>
#endif

#ifndef _DIRENT_H
#include <dirent.h>
#endif

#include <osl/file.h>

#ifndef _ERRNO_H
#include <errno.h>
#endif
 
#include <rtl/string.hxx>

#ifndef _OSL_UUNXAPI_H_
#include "uunxapi.hxx"
#endif
#include "file_path_helper.hxx"
#include "file_error_transl.h"

#ifdef _DIRENT_HAVE_D_TYPE
#include "file_impl.hxx"
#endif

namespace /* private */
{

    inline void set_file_type(const struct stat& file_stat, oslFileStatus* pStat)
    {
        /* links to directories state also to be a directory */
       if (S_ISLNK(file_stat.st_mode)) 
           pStat->eType = osl_File_Type_Link;              
       else if (S_ISDIR(file_stat.st_mode))                	   
           pStat->eType = osl_File_Type_Directory;       
       else if (S_ISREG(file_stat.st_mode))       
           pStat->eType = osl_File_Type_Regular;
       else if (S_ISFIFO(file_stat.st_mode)) 
           pStat->eType = osl_File_Type_Fifo;      
       else if (S_ISSOCK(file_stat.st_mode)) 
           pStat->eType = osl_File_Type_Socket;       
       else if (S_ISCHR(file_stat.st_mode) || S_ISBLK(file_stat.st_mode))        
           pStat->eType = osl_File_Type_Special;    
       else 
           pStat->eType = osl_File_Type_Unknown;

       pStat->uValidFields |= osl_FileStatus_Mask_Type;
    }
            
    inline void set_file_access_mask(const struct stat& file_stat, oslFileStatus* pStat)
    {
        // user permissions       
        if (S_IRUSR & file_stat.st_mode)           
            pStat->uAttributes |= osl_File_Attribute_OwnRead;           

        if (S_IWUSR & file_stat.st_mode)           
            pStat->uAttributes |= osl_File_Attribute_OwnWrite;           

        if (S_IXUSR & file_stat.st_mode)          
            pStat->uAttributes |= osl_File_Attribute_OwnExe;           

        // group permissions
        if (S_IRGRP & file_stat.st_mode)           
            pStat->uAttributes |= osl_File_Attribute_GrpRead;           

        if (S_IWGRP & file_stat.st_mode)           
            pStat->uAttributes |= osl_File_Attribute_GrpWrite;           

        if (S_IXGRP & file_stat.st_mode)           
            pStat->uAttributes |= osl_File_Attribute_GrpExe;           

        // others permissions
        if (S_IROTH & file_stat.st_mode)           
            pStat->uAttributes |= osl_File_Attribute_OthRead;           

        if (S_IWOTH & file_stat.st_mode)           
            pStat->uAttributes |= osl_File_Attribute_OthWrite;           

        if (S_IXOTH & file_stat.st_mode)           
            pStat->uAttributes |= osl_File_Attribute_OthExe;
            
        pStat->uValidFields |= osl_FileStatus_Mask_Attributes;
    }
    
    inline void set_file_access_rights(const struct stat& file_stat, int S_IR, int S_IW, int S_IX, oslFileStatus* pStat)
    {
        /* we cannot really map osl_File_Attribute_ReadOnly to
           the Unix access rights, it's a Windows only flag
           that's why the following hack. We set osl_FileStatus_Mask_Attributes
           but if there is no read access for a file we clear the flag
           again to signal to the caller that there are no file attributes
           to read because that's better than to give them incorrect one.
        */
        pStat->uValidFields |= osl_FileStatus_Mask_Attributes;
        
        if ((0 == (S_IW & file_stat.st_mode)) && (S_IR & file_stat.st_mode))			
            pStat->uAttributes |= osl_File_Attribute_ReadOnly;

        if (S_IX & file_stat.st_mode)
            pStat->uAttributes |= osl_File_Attribute_Executable;						
    }
    
    /* a process may belong to up to NGROUPS_MAX groups, so when
       checking group access rights, we have to check all belonging
       groups */
    inline bool is_in_process_grouplist(const gid_t file_group)
    {
        // check primary process group
        
        if (getgid() == file_group)
            return true;
        
        // check supplementary process groups
            
        gid_t grplist[NGROUPS_MAX];
        int   grp_number = getgroups(NGROUPS_MAX, grplist);
        
        for (int i = 0; i < grp_number; i++)
        {
            if (grplist[i] == file_group)
                return true;
        }
        return false;
    }
    
    /* Currently we are determining the file access right based
       on the real user ID not the effective user ID!
       We don't use access(...) because access follows links which
       may cause performance problems see #97133.
    */
    inline void set_file_access_rights(const struct stat& file_stat, oslFileStatus* pStat)
    {			  
        if (getuid() == file_stat.st_uid)
        {
            set_file_access_rights(file_stat, S_IRUSR, S_IWUSR, S_IXUSR, pStat);			        	
        }
        else if (is_in_process_grouplist(file_stat.st_gid))
        {
            set_file_access_rights(file_stat, S_IRGRP, S_IWGRP, S_IXGRP, pStat);
        }
        else
        {
            set_file_access_rights(file_stat, S_IROTH, S_IWOTH, S_IXOTH, pStat);			
        }						
    }
    
    inline void set_file_hidden_status(const rtl::OUString& file_path, oslFileStatus* pStat)
    {
        pStat->uAttributes   = osl::systemPathIsHiddenFileOrDirectoryEntry(file_path) ? osl_File_Attribute_Hidden : 0;        
        pStat->uValidFields |= osl_FileStatus_Mask_Attributes;
    }
    
    /* the set_file_access_rights must be called after set_file_hidden_status(...) and
       set_file_access_mask(...) because of the hack in set_file_access_rights(...) */
    inline void set_file_attributes(
        const rtl::OUString& file_path, const struct stat& file_stat, const sal_uInt32 uFieldMask, oslFileStatus* pStat)
    {
        set_file_hidden_status(file_path, pStat);	   	   				               	   
        set_file_access_mask(file_stat, pStat);		              
        
        // we set the file access rights only on demand
        // because it's potentially expensive		       
        if (uFieldMask & osl_FileStatus_Mask_Attributes)                  			   		   
               set_file_access_rights(file_stat, pStat);
    }
    
    inline void set_file_access_time(const struct stat& file_stat, oslFileStatus* pStat)
    {
        pStat->aAccessTime.Seconds  = file_stat.st_atime;
        pStat->aAccessTime.Nanosec  = 0;
           pStat->uValidFields        |= osl_FileStatus_Mask_AccessTime;
    }
    
    inline void set_file_modify_time(const struct stat& file_stat, oslFileStatus* pStat)
    {
        pStat->aModifyTime.Seconds  = file_stat.st_mtime;
        pStat->aModifyTime.Nanosec  = 0;
        pStat->uValidFields        |= osl_FileStatus_Mask_ModifyTime;
    }
    
    inline void set_file_size(const struct stat& file_stat, oslFileStatus* pStat)
    {
        if (S_ISREG(file_stat.st_mode))
           {
            pStat->uFileSize     = file_stat.st_size;
               pStat->uValidFields |= osl_FileStatus_Mask_FileSize;
           }
    }
    
    /* we only need to call stat or lstat if one of the 
       following flags is set */
#ifdef _DIRENT_HAVE_D_TYPE
    inline bool is_stat_call_necessary(sal_uInt32 field_mask, oslDirectoryItemImpl *pImpl)
#else
    inline bool is_stat_call_necessary(sal_uInt32 field_mask)
#endif
    {
        return (
/* on linux the dirent might have d_type */
#ifdef _DIRENT_HAVE_D_TYPE
                ((field_mask & osl_FileStatus_Mask_Type) && (!pImpl->bHasType || pImpl->DType == DT_UNKNOWN)) ||
#else
                (field_mask & osl_FileStatus_Mask_Type) ||
#endif
                (field_mask & osl_FileStatus_Mask_Attributes) ||
                (field_mask & osl_FileStatus_Mask_CreationTime) ||
                (field_mask & osl_FileStatus_Mask_AccessTime) ||
                (field_mask & osl_FileStatus_Mask_ModifyTime) ||
                (field_mask & osl_FileStatus_Mask_FileSize) ||
                (field_mask & osl_FileStatus_Mask_LinkTargetURL) ||
                (field_mask & osl_FileStatus_Mask_Validate));				
    }
    
    inline oslFileError set_link_target_url(const rtl::OUString& file_path, oslFileStatus* pStat)
    {
        rtl::OUString link_target;					
        if (!osl::realpath(file_path, link_target))
            return oslTranslateFileError(OSL_FET_ERROR, errno);

        oslFileError osl_error = osl_getFileURLFromSystemPath(link_target.pData, &pStat->ustrLinkTargetURL);
        if (osl_error != osl_File_E_None)
            return osl_error;
                                
        pStat->uValidFields |= osl_FileStatus_Mask_LinkTargetURL;							
        return osl_File_E_None;			
    }
    
    inline oslFileError setup_osl_getFileStatus(oslDirectoryItem Item, oslFileStatus* pStat, rtl::OUString& file_path)
    {		
        if ((NULL == Item) || (NULL == pStat))
            return osl_File_E_INVAL;
                            
#ifdef _DIRENT_HAVE_D_TYPE
        file_path = rtl::OUString(reinterpret_cast<rtl_uString*>(((oslDirectoryItemImpl* ) Item)->ustrFilePath));
#else
        file_path = rtl::OUString(reinterpret_cast<rtl_uString*>(Item));    
#endif

        OSL_ASSERT(file_path.getLength() > 0);
        
        if (file_path.getLength() <= 0)
            return osl_File_E_INVAL;
                            
        pStat->uValidFields = 0;
                                    
        return osl_File_E_None;		
    }
    
} // end namespace private


/****************************************************************************
 *	osl_getFileStatus 
 ****************************************************************************/

oslFileError SAL_CALL osl_getFileStatus(oslDirectoryItem Item, oslFileStatus* pStat, sal_uInt32 uFieldMask)
{	
    rtl::OUString file_path;	
    oslFileError  osl_error = setup_osl_getFileStatus(Item, pStat, file_path);	
    if (osl_File_E_None != osl_error)
        return osl_error;
    
#if defined(__GNUC__) && (__GNUC__ < 3)
    struct ::stat file_stat;
#else
    struct stat file_stat;
#endif

#ifdef _DIRENT_HAVE_D_TYPE
    oslDirectoryItemImpl* pImpl = (oslDirectoryItemImpl*) Item;
    bool bStatNeeded = is_stat_call_necessary(uFieldMask, pImpl);
#else
    bool bStatNeeded = is_stat_call_necessary(uFieldMask);
#endif

    if (bStatNeeded && (0 != osl::lstat(file_path, file_stat)))
        return oslTranslateFileError(OSL_FET_ERROR, errno);
    
    if (bStatNeeded)
    {
        // we set all these attributes because it's cheap				
        set_file_type(file_stat, pStat);
        set_file_access_time(file_stat, pStat);
        set_file_modify_time(file_stat, pStat);
        set_file_size(file_stat, pStat);		
        set_file_attributes(file_path, file_stat, uFieldMask, pStat);
                           
        // file exists semantic of osl_FileStatus_Mask_Validate 
        if ((uFieldMask & osl_FileStatus_Mask_LinkTargetURL) && S_ISLNK(file_stat.st_mode)) 
        {
            osl_error = set_link_target_url(file_path, pStat);
            if (osl_error != osl_File_E_None)      
                return osl_error;
        }
    }
#ifdef _DIRENT_HAVE_D_TYPE
    else if (uFieldMask & osl_FileStatus_Mask_Type)
    {
        OSL_ASSERT(pImpl->bHasType);

        switch(pImpl->DType)
        {
        case DT_LNK:
            pStat->eType = osl_File_Type_Link;              
            break;
        case DT_DIR:
            pStat->eType = osl_File_Type_Directory;       
            break;
        case DT_REG:
            pStat->eType = osl_File_Type_Regular;
            break;
        case DT_FIFO:
            pStat->eType = osl_File_Type_Fifo;
            break;
        case DT_SOCK:
            pStat->eType = osl_File_Type_Socket;
            break;
        case DT_CHR:
        case DT_BLK:
            pStat->eType = osl_File_Type_Special;
            break;
        default:
            OSL_ASSERT(0);
            pStat->eType = osl_File_Type_Unknown;
        }

       pStat->uValidFields |= osl_FileStatus_Mask_Type;
    }
#endif
            
    if (uFieldMask & osl_FileStatus_Mask_FileURL)
    {
        if ((osl_error = osl_getFileURLFromSystemPath(file_path.pData, &pStat->ustrFileURL)) != osl_File_E_None)
            return osl_error;
                            
        pStat->uValidFields |= osl_FileStatus_Mask_FileURL;		
    }
   
    if (uFieldMask & osl_FileStatus_Mask_FileName)
    {	  
        osl_systemPathGetFileNameOrLastDirectoryPart(file_path.pData, &pStat->ustrFileName);
        pStat->uValidFields |= osl_FileStatus_Mask_FileName;
       }	               
    return osl_File_E_None;
}

