#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.5 $
#
#   last change: $Author: oj $ $Date: 2002-07-19 10:09:06 $
#
#   The Contents of this file are made available subject to the terms of
#   either of the following licenses
#
#          - GNU Lesser General Public License Version 2.1
#          - Sun Industry Standards Source License Version 1.1
#
#   Sun Microsystems Inc., October, 2000
#
#   GNU Lesser General Public License Version 2.1
#   =============================================
#   Copyright 2000 by Sun Microsystems, Inc.
#   901 San Antonio Road, Palo Alto, CA 94303, USA
#
#   This library is free software; you can redistribute it and/or
#   modify it under the terms of the GNU Lesser General Public
#   License version 2.1, as published by the Free Software Foundation.
#
#   This library is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public
#   License along with this library; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#   MA  02111-1307  USA
#
#
#   Sun Industry Standards Source License Version 1.1
#   =================================================
#   The contents of this file are subject to the Sun Industry Standards
#   Source License Version 1.1 (the "License"); You may not use this file
#   except in compliance with the License. You may obtain a copy of the
#   License at http://www.openoffice.org/license.html.
#
#   Software provided under this License is provided on an "AS IS" basis,
#   WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
#   WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
#   MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
#   See the License for the specific provisions governing your rights and
#   obligations concerning the Software.
#
#   The Initial Developer of the Original Code is: Sun Microsystems, Inc.
#
#   Copyright: 2000 by Sun Microsystems, Inc.
#
#   All Rights Reserved.
#
#   Contributor(s): _______________________________________
#
#
#
#*************************************************************************

PRJ=..$/..$/..$/..

PRJNAME=api

TARGET=csssdbc
PACKAGE=com$/sun$/star$/sdbc

# --- Settings -----------------------------------------------------
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

# ------------------------------------------------------------------------
IDLFILES=\
    BatchUpdateException.idl \
    BestRowScope.idl \
    BestRowType.idl \
    CallableStatement.idl \
    ColumnSearch.idl \
    ColumnType.idl \
    ColumnValue.idl \
    Connection.idl \
    ConnectionPool.idl \
    DataTruncation.idl \
    DataType.idl \
    Deferrability.idl \
    Driver.idl \
    DriverManager.idl \
    DriverPropertyInfo.idl \
    FetchDirection.idl \
    IndexType.idl \
    KeyRule.idl \
    PreparedStatement.idl \
    ProcedureColumn.idl \
    ProcedureResult.idl \
    ResultSet.idl \
    ResultSetConcurrency.idl \
    ResultSetType.idl \
    RowSet.idl \
    SQLException.idl \
    SQLWarning.idl \
    Statement.idl \
    TransactionIsolation.idl \
    XArray.idl \
    XBatchExecution.idl \
    XBlob.idl \
    XColumnLocate.idl \
    XClob.idl \
    XCloseable.idl \
    XConnection.idl \
    XPooledConnection.idl \
    XDatabaseMetaData.idl \
    XDataSource.idl \
    XDriver.idl \
    XDriverAccess.idl \
    XDriverManager.idl \
    XMultipleResults.idl \
    XOutParameters.idl \
    XParameters.idl \
    XPreparedBatchExecution.idl \
    XPreparedStatement.idl \
    XRef.idl \
    XResultSet.idl \
    XResultSetMetaData.idl \
    XResultSetMetaDataSupplier.idl \
    XResultSetUpdate.idl \
    XRow.idl \
    XRowSet.idl \
    XRowSetListener.idl \
    XRowUpdate.idl \
    XSQLData.idl \
    XSQLInput.idl \
    XSQLOutput.idl \
    XStatement.idl \
    XStruct.idl \
    XGeneratedResultSet.idl \
    XWarningsSupplier.idl

# ------------------------------------------------------------------
.INCLUDE :  target.mk
.INCLUDE :  $(PRJ)$/util$/target.pmk


