#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.14 $
#
#   last change: $Author: obo $ $Date: 2007-06-12 06:17:56 $
#
#   The Contents of this file are made available subject to
#   the terms of GNU Lesser General Public License Version 2.1.
#
#
#     GNU Lesser General Public License Version 2.1
#     =============================================
#     Copyright 2005 by Sun Microsystems, Inc.
#     901 San Antonio Road, Palo Alto, CA 94303, USA
#
#     This library is free software; you can redistribute it and/or
#     modify it under the terms of the GNU Lesser General Public
#     License version 2.1, as published by the Free Software Foundation.
#
#     This library is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#     Lesser General Public License for more details.
#
#     You should have received a copy of the GNU Lesser General Public
#     License along with this library; if not, write to the Free Software
#     Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#     MA  02111-1307  USA
#
#*************************************************************************

PRJ = ..$/..$/..$/..

PRJNAME=offapi

TARGET = cssdeployment
PACKAGE = com$/sun$/star$/deployment

.INCLUDE : $(PRJ)$/util$/makefile.pmk

IDLFILES = \
        DeploymentException.idl                 \
        XPackageTypeInfo.idl                    \
        XPackage.idl                            \
        XPackageRegistry.idl                    \
        PackageRegistryBackend.idl              \
        XPackageManager.idl                     \
        XPackageManagerFactory.idl              \
        thePackageManagerFactory.idl			\
        DependencyException.idl                 \
        LicenseException.idl                    \
        VersionException.idl                    \
        InstallException.idl                    \
        XUpdateInformationProvider.idl          \
        UpdateInformationProvider.idl           \
        XPackageInformationProvider.idl         \
        PackageInformationProvider.idl          \
        LicenseIndividualAgreementException.idl

.INCLUDE : target.mk
.INCLUDE : $(PRJ)$/util$/target.pmk

