/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: sockimpl.h,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 15:12:25 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

#ifndef _OSL_SOCKETIMPL_H_
#define _OSL_SOCKETIMPL_H_

#include <osl/socket.h>
#include <osl/interlck.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/
/* oslSocketImpl */
/*****************************************************************************/
#define OSL_SOCKET_FLAGS_NONBLOCKING	0x0001

typedef void* (SAL_CALL * oslCloseCallback) (void*);

struct oslSocketImpl {
    oslInterlockedCount m_nRefCount;
    SOCKET				m_Socket;
    int					m_Flags;
    oslCloseCallback	m_CloseCallback;
    void*				m_CallbackArg;
};

struct oslSocketAddrImpl
{
    struct sockaddr m_sockaddr;
    oslInterlockedCount m_nRefCount;
};
    
oslSocket __osl_createSocketImpl(SOCKET Socket);
void __osl_destroySocketImpl(oslSocket pImpl);

/*****************************************************************************/
/* oslSocketDialupImpl */
/*****************************************************************************/
#define INTERNET_MODULE_NAME "wininet.dll"

#define INTERNET_CONNECTION_MODEM  0x00000001L
#define INTERNET_CONNECTION_LAN    0x00000002L
#define INTERNET_CONNECTION_HANGUP 0x80000000L

typedef DWORD (WINAPI *INTERNETATTEMPTCONNECT) (
    DWORD dwReserved);
typedef BOOL (WINAPI *INTERNETAUTODIAL) (
    DWORD dwFlags, DWORD dwReserved);
typedef BOOL (WINAPI *INTERNETAUTODIALHANGUP) (
    DWORD dwReserved);
typedef BOOL (WINAPI *INTERNETGETCONNECTEDSTATE) (
    LPDWORD lpdwFlags, DWORD dwReserved);

typedef struct osl_socket_dialup_impl_st
{
    CRITICAL_SECTION          m_hMutex;
    HINSTANCE                 m_hModule;
    INTERNETATTEMPTCONNECT    m_pfnAttemptConnect;
    INTERNETAUTODIAL          m_pfnAutodial;
    INTERNETAUTODIALHANGUP    m_pfnAutodialHangup;
    INTERNETGETCONNECTEDSTATE m_pfnGetConnectedState;
    DWORD                     m_dwFlags;
} oslSocketDialupImpl;

static oslSocketDialupImpl* __osl_createSocketDialupImpl (void);
static void __osl_initSocketDialupImpl (oslSocketDialupImpl *pImpl);
static void __osl_destroySocketDialupImpl (oslSocketDialupImpl *pImpl);

static sal_Bool __osl_querySocketDialupImpl (void);
static sal_Bool __osl_attemptSocketDialupImpl (void);

/*****************************************************************************/
/* The End */
/*****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif 


