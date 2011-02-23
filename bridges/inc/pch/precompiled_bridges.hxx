/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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

// MARKER(update_precomp.py): Generated on 2006-09-01 17:49:31.967936

#ifdef PRECOMPILED_HEADERS
//---MARKER---
#include "boost/static_assert.hpp"

#include "com/sun/star/bridge/InvalidProtocolChangeException.hdl"
#include "com/sun/star/bridge/InvalidProtocolChangeException.hpp"
#include "com/sun/star/bridge/ProtocolProperty.hdl"
#include "com/sun/star/bridge/ProtocolProperty.hpp"
#include "com/sun/star/bridge/XBridge.hpp"
#include "com/sun/star/bridge/XBridgeFactory.hpp"
#include "com/sun/star/bridge/XInstanceProvider.hpp"
#include "com/sun/star/bridge/XUnoUrlResolver.hpp"
#include "com/sun/star/connection/ConnectionSetupException.hpp"
#include "com/sun/star/connection/NoConnectException.hpp"
#include "com/sun/star/connection/XAcceptor.hpp"
#include "com/sun/star/connection/XConnection.hpp"
#include "com/sun/star/connection/XConnector.hpp"
#include "com/sun/star/frame/XComponentLoader.hpp"
#include "com/sun/star/io/XInputStream.hpp"
#include "com/sun/star/io/XOutputStream.hpp"
#include "com/sun/star/lang/IllegalArgumentException.hpp"
#include "com/sun/star/lang/WrappedTargetRuntimeException.hpp"
#include "com/sun/star/lang/XComponent.hpp"
#include "com/sun/star/lang/XInitialization.hpp"
#include "com/sun/star/lang/XMain.hpp"
#include "com/sun/star/lang/XMultiComponentFactory.hpp"
#include "com/sun/star/lang/XServiceInfo.hpp"
#include "com/sun/star/lang/XSingleComponentFactory.hpp"
#include "com/sun/star/registry/InvalidRegistryException.hpp"
#include "com/sun/star/registry/XImplementationRegistration.hpp"
#include "com/sun/star/registry/XRegistryKey.hpp"
#include "com/sun/star/text/XTextDocument.hpp"
#include "com/sun/star/uno/Any.h"
#include "com/sun/star/uno/Any.hxx"
#include "com/sun/star/uno/DeploymentException.hpp"
#include "com/sun/star/uno/Exception.hdl"
#include "com/sun/star/uno/Exception.hpp"
#include "com/sun/star/uno/Reference.h"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/RuntimeException.hdl"
#include "com/sun/star/uno/RuntimeException.hpp"
#include "com/sun/star/uno/Sequence.h"
#include "com/sun/star/uno/Sequence.hxx"
#include "com/sun/star/uno/Type.h"
#include "com/sun/star/uno/Type.hxx"
#include "com/sun/star/uno/TypeClass.hdl"
#include "com/sun/star/uno/XComponentContext.hpp"
#include "com/sun/star/uno/XInterface.hdl"
#include "com/sun/star/uno/XInterface.hpp"
#include "com/sun/star/uno/XNamingService.hpp"
#include "com/sun/star/uno/genfunc.hxx"

#include "cppu/macros.hxx"
#include "cppu/unotype.hxx"

#include "cppuhelper/factory.hxx"
#include "cppuhelper/implbase1.hxx"
#include "cppuhelper/implbase2.hxx"
#include "cppuhelper/implbase3.hxx"
#include "cppuhelper/implementationentry.hxx"
#include "cppuhelper/servicefactory.hxx"
#include "cppuhelper/weak.hxx"


#include "osl/conditn.h"
#include "osl/conditn.hxx"
#include "osl/diagnose.h"
#include "osl/doublecheckedlocking.h"
#include "osl/getglobalmutex.hxx"
#include "osl/interlck.h"
#include "osl/module.h"
#include "osl/module.hxx"
#include "osl/mutex.h"
#include "osl/mutex.hxx"
#include "osl/process.h"
#include "osl/semaphor.h"
#include "osl/thread.h"
#include "osl/thread.hxx"
#include "osl/time.h"

#include "rtl/alloc.h"
#include "rtl/byteseq.h"
#include "rtl/byteseq.hxx"
#include "rtl/instance.hxx"
#include "rtl/memory.h"
#include "rtl/process.h"
#include "rtl/random.h"
#include "rtl/strbuf.hxx"
#include "rtl/string.h"
#include "rtl/string.hxx"
#include "rtl/unload.h"
#include "rtl/ustrbuf.hxx"
#include "rtl/ustring.h"
#include "rtl/ustring.hxx"
#include "rtl/uuid.h"

#include "sal/alloca.h"
#include "sal/config.h"
#include "sal/types.h"

#include "sys/types.h"


#include "typelib/typeclass.h"
#include "typelib/typedescription.h"
#include "typelib/typedescription.hxx"

#include "uno/any2.h"
#include "uno/data.h"
#include "uno/dispatcher.h"
#include "uno/environment.h"
#include "uno/environment.hxx"
#include "uno/lbnames.h"
#include "uno/mapping.h"
#include "uno/mapping.hxx"
#include "uno/sequence2.h"
#include "uno/threadpool.h"
//---MARKER---
#endif
