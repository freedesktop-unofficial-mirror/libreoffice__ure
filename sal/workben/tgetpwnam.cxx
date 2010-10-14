/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sal.hxx"

#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#ifndef NETBSD
#include <shadow.h>
#endif

/* exercises some reentrant libc-fucntions */

extern "C"
{
    struct tm *localtime_r(const time_t *timep, struct tm *buffer);
    struct passwd* getpwnam_r(char*, struct passwd*, char *, int); 
    struct spwd* getspnam_r(char*, struct spwd*, char *, int);
    struct hostent *gethostbyname_r(const char *name, struct hostent *result,
                                    char *buffer, int buflen, int *h_errnop);
}

static int go;



extern "C" void *workfunc1(void*)
{
    char buffer[256];
    struct tm		sttm;
    time_t			nepoch;
    struct passwd 	stpwd;
    struct hostent	sthostent;
    int 			nerr;

    printf("starting thread 1 ...\n");
    while (go) {
        getpwnam_r("hr", &stpwd, buffer, sizeof(buffer));
        gethostbyname_r("blauwal", &sthostent, buffer, sizeof(buffer), &nerr);
        time(&nepoch);
        localtime_r(&nepoch, &sttm);
    }
    return 0;
}
    
extern "C" void *workfunc2(void*)
{
    char buffer[256];
    struct tm		sttm;
    time_t			nepoch;
    struct passwd 	stpwd;
    struct hostent	sthostent;
    int 			nerr;

    printf("starting thread 2 ...\n");
    while(go) {
        getpwnam_r("mh", &stpwd, buffer, sizeof(buffer));
        gethostbyname_r("hr-1242", &sthostent, buffer, sizeof(buffer), &nerr);
        time(&nepoch);
        localtime_r(&nepoch, &sttm);
    }
    return 0;
}


extern int h_errno;

int main(int argc, char *argv[])
{
    char buffer[256];
    struct tm		*ptm;
    time_t			nepoch;
    struct passwd 	*pwd, *pres1;
#ifndef NETBSD	
    struct spwd 	*spwd, *pres2;
#endif
    struct hostent	*phostent, *pres3;
    char **p;

    pthread_t 		tid1,tid2;
    int 			res1,res2;
    
    go = 1;

    pthread_create(&tid1, NULL, workfunc1, &res1);
    pthread_create(&tid2, NULL, workfunc2, &res2);

    pwd = (struct passwd*)malloc(sizeof(struct passwd));

    pres1 = getpwnam_r("hr", pwd, buffer, sizeof(buffer));

    sleep(3);

    if (pres1) {
        printf("Name: %s\n", pwd->pw_name);
        printf("Passwd: %s\n", pwd->pw_passwd);
        printf("Uid: %d\n", pwd->pw_uid);
        printf("Gid: %d\n", pwd->pw_gid);
#ifdef NETBSD
        printf("Change: %s", ctime(&pwd->pw_change));
        printf("Class: %s\n", pwd->pw_class);
#else
        printf("Age: %s\n", pwd->pw_age);
        printf("Comment: %s\n", pwd->pw_comment);
#endif
        printf("Gecos: %s\n", pwd->pw_gecos);
        printf("Dir: %s\n", pwd->pw_dir);
        printf("Shell: %s\n", pwd->pw_shell);
    }
    else
        printf("getpwnam_r() failed!\n");

    free(pwd);

#ifndef NETBSD
    spwd = (struct spwd*)malloc(sizeof(struct spwd));
    
    pres2 = getspnam_r("hr", spwd, buffer, sizeof(buffer));

    if (pres2) {
        printf("Name: %s\n", spwd->sp_namp);
        printf("Passwd: %s\n", spwd->sp_pwdp);
        printf("Last Change: %ld\n", spwd->sp_lstchg);
        printf("Min: %ld\n", spwd->sp_min);
        printf("Max: %ld\n", spwd->sp_max);
    }
    else
        printf("getspnam_r() failed!\n");

    free(spwd);
#endif

    ptm = (struct tm*)malloc(sizeof(struct tm));

    time(&nepoch);
    localtime_r(&nepoch, ptm);

    printf("Seconds: %d\n", ptm->tm_sec); 
    printf("Minutes: %d\n", ptm->tm_min); 
    printf("Hour: %d\n", ptm->tm_hour); 
    printf("Day of Month: %d\n", ptm->tm_mday); 
    printf("Month: %d\n", ptm->tm_mon); 
    printf("Year: %d\n", ptm->tm_year); 
    printf("Day of week: %d\n", ptm->tm_wday); 
    printf("Day in the year: %d\n", ptm->tm_yday); 
    printf("Daylight saving time: %d\n", ptm->tm_isdst);
#ifdef NETBSD	
    printf("Timezone: %s\n", ptm->tm_zone);
#else
    printf("Timezone: %s\n", ptm->tm_name);
#endif

    free(ptm);

    phostent = (struct hostent*)malloc(sizeof(struct hostent));

    pres3 = gethostbyname_r("blauwal", phostent, buffer, sizeof(buffer), h_errno);

    if (pres3) {
        printf("Official Hostname: %s\n", phostent->h_name);
        for ( p = phostent->h_aliases; *p != NULL; p++ )
            printf("Alias: %s\n", *p);
        printf("Addresstype: %d\n", phostent->h_addrtype);
        printf("Address length: %d\n", phostent->h_length);
        if ( phostent->h_addrtype == AF_INET ) {
            for ( p = phostent->h_addr_list; *p != NULL; *p++ )
                printf("Address: %s\n", inet_ntoa(**((in_addr**)p)));
        }
    }
    

    /* test boundary conditions */
    char smallbuf[23]; /* buffer to small */
    pres3 = gethostbyname_r("blauwal", phostent, smallbuf, sizeof(smallbuf), h_errno);
    if (!pres3) {
        perror("Expect ERANGE");
    }
    else
    {
        printf("ERROR: Check for buffersize went wrong\n");
    }

#ifdef NETBSD
    char exactbuf[35];
#else
    char exactbuf[24]; /* should be exact the necessary size */
#endif
    pres3 = gethostbyname_r("blauwal", phostent, exactbuf, sizeof(exactbuf), &h_errno);
    if (!pres3) {
        perror("Check with exact buffersize");
    }
    else
    {
        printf("Boundary check ok\n");
    }

    /* test error conditions */
    pres3 = gethostbyname_r("nohost", phostent, buffer, sizeof(buffer), &h_errno);
    if (!pres3) {
        herror("Expect HOST_NOT_FOUND");
    }
    else 
    {
        printf("failed to detect non existant host\n");
    } 

    free(phostent);
    go = 0; /* atomic enough for our purposes */

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    exit(0);
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
