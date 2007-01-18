/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: signal.c,v $
 *
 *  $Revision: 1.33 $
 *
 *  last change: $Author: vg $ $Date: 2007-01-18 14:18:28 $
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


/* system headers */
#include "system.h"

#define MAX_STACK_FRAMES 256

#ifdef LINUX
#include <execinfo.h>
#define INCLUDE_BACKTRACE
#define STACKTYPE "Linux"
#endif

#ifdef SOLARIS

#include "backtrace.h"
#define INCLUDE_BACKTRACE

#if defined( SPARC )
#define STACKTYPE "Solaris_Sparc"
#elif defined( INTEL )
#define STACKTYPE "Solaris_X86"
#else
#define STACKTYPE "Solaris_Unknown"
#endif

#endif /* defined SOLARIS */

#include <osl/diagnose.h>
#include <osl/mutex.h>
#include <osl/signal.h>
#include <osl/process.h>
#include <osl/thread.h>
#include <rtl/digest.h>

#include "file_path_helper.h"

#define ACT_IGNORE	1
#define ACT_ABORT	2
#define ACT_EXIT	3
#define ACT_SYSTEM	4
#define ACT_HIDE	5

#define MAX_PATH_LEN	2048

typedef struct _oslSignalHandlerImpl
{
    oslSignalHandlerFunction      Handler;
    void*			        	  pData;
    struct _oslSignalHandlerImpl* pNext;
} oslSignalHandlerImpl;

static struct SignalAction
{
    int Signal;
    int Action;
    void (*Handler)(int);
} Signals[] =
{
    { SIGHUP,    ACT_IGNORE, NULL },    /* hangup */
    { SIGINT,    ACT_EXIT,   NULL },    /* interrupt (rubout) */
    { SIGQUIT,   ACT_EXIT,  NULL },    /* quit (ASCII FS) */
    { SIGILL,    ACT_SYSTEM,  NULL },    /* illegal instruction (not reset when caught) */
/* changed from ACT_ABOUT to ACT_SYSTEM to try and get collector to run*/
    { SIGTRAP,   ACT_ABORT,  NULL },    /* trace trap (not reset when caught) */
#if ( SIGIOT != SIGABRT )
    { SIGIOT,    ACT_ABORT,  NULL },    /* IOT instruction */
#endif 
    { SIGABRT,   ACT_ABORT,  NULL },    /* used by abort, replace SIGIOT in the future */
#ifdef SIGEMT	
    { SIGEMT,    ACT_SYSTEM,  NULL },    /* EMT instruction */
/* changed from ACT_ABORT to ACT_SYSTEM to remove handler*/
/* SIGEMT may also be used by the profiler - so it is probably not a good
plan to have the new handler use this signal*/
#endif	
    { SIGFPE,    ACT_ABORT,  NULL },    /* floating point exception */
    { SIGKILL,   ACT_SYSTEM, NULL },    /* kill (cannot be caught or ignored) */
    { SIGBUS,    ACT_ABORT,  NULL },    /* bus error */
    { SIGSEGV,   ACT_ABORT,  NULL },    /* segmentation violation */
#ifdef SIGSYS
    { SIGSYS,    ACT_ABORT,  NULL },    /* bad argument to system call */
#endif
    { SIGPIPE,   ACT_HIDE,   NULL },    /* write on a pipe with no one to read it */
    { SIGALRM,   ACT_EXIT,   NULL },    /* alarm clock */
    { SIGTERM,   ACT_EXIT,   NULL },    /* software termination signal from kill */
    { SIGUSR1,   ACT_SYSTEM, NULL },    /* user defined signal 1 */
    { SIGUSR2,   ACT_SYSTEM, NULL },    /* user defined signal 2 */
    { SIGCHLD,   ACT_SYSTEM, NULL },    /* child status change */
#ifdef SIGPWR
    { SIGPWR,    ACT_IGNORE, NULL },    /* power-fail restart */
#endif
    { SIGWINCH,  ACT_IGNORE, NULL },    /* window size change */
    { SIGURG,    ACT_EXIT,   NULL },    /* urgent socket condition */
#ifdef SIGPOLL
    { SIGPOLL,   ACT_EXIT,   NULL },    /* pollable event occured */
#endif
    { SIGSTOP,   ACT_SYSTEM, NULL },    /* stop (cannot be caught or ignored) */
    { SIGTSTP,   ACT_SYSTEM, NULL },    /* user stop requested from tty */
    { SIGCONT,   ACT_SYSTEM, NULL },    /* stopped process has been continued */
    { SIGTTIN,   ACT_SYSTEM, NULL },    /* background tty read attempted */
    { SIGTTOU,   ACT_SYSTEM, NULL },    /* background tty write attempted */
    { SIGVTALRM, ACT_EXIT,   NULL },    /* virtual timer expired */
    { SIGPROF,   ACT_SYSTEM,   NULL },    /* profiling timer expired */
/*Change from ACT_EXIT to ACT_SYSTEM for SIGPROF is so that profiling signals do
not get taken by the new handler - the new handler does not pass on context 
information which causes 'collect' to crash. This is a way of avoiding
what looks like a bug in the new handler*/
    { SIGXCPU,   ACT_ABORT,  NULL },    /* exceeded cpu limit */
    { SIGXFSZ,   ACT_ABORT,  NULL }     /* exceeded file size limit */
};
const int NoSignals = sizeof(Signals) / sizeof(struct SignalAction);

static sal_Bool               bErrorReportingEnabled = sal_True;
static sal_Bool  			  bInitSignal = sal_False;
static oslMutex 			  SignalListMutex;
static oslSignalHandlerImpl*  SignalList;
static sal_Bool				  bDoHardKill = sal_False;
static sal_Bool				  bSetSEGVHandler = sal_False;
static sal_Bool				  bSetWINCHHandler = sal_False;
static sal_Bool               bSetILLHandler = sal_False;

static void SignalHandlerFunction(int);

static void getExecutableName_Impl (rtl_String ** ppstrProgName)
{
    rtl_uString * ustrProgFile = 0;
    osl_getExecutableFile (&ustrProgFile);
    if (ustrProgFile)
    {
        rtl_uString * ustrProgName = 0;
        osl_systemPathGetFileNameOrLastDirectoryPart (ustrProgFile, &ustrProgName);
        if (ustrProgName != 0)
        {
            rtl_uString2String (
                ppstrProgName,
                rtl_uString_getStr (ustrProgName), rtl_uString_getLength (ustrProgName),
                osl_getThreadTextEncoding(),
                OUSTRING_TO_OSTRING_CVTFLAGS);
            rtl_uString_release (ustrProgName);
        }
        rtl_uString_release (ustrProgFile);
    }
}

static sal_Bool is_soffice_Impl (void)
{
    sal_Int32    idx       = -1;
    rtl_String * strProgName = 0;

    getExecutableName_Impl (&strProgName);
    if (strProgName)
    {
        idx = rtl_str_indexOfStr (rtl_string_getStr (strProgName), "soffice");
        rtl_string_release (strProgName);
    }
    return (idx != -1);
}

static sal_Bool InitSignal()
{
    int i;
    struct sigaction act;
    struct sigaction oact;

    if (is_soffice_Impl())
    {
        sal_uInt32	argi;
        sal_uInt32	argc;
        rtl_uString *ustrCommandArg = 0;

        argc = osl_getCommandArgCount();
        for ( argi = 0; argi < argc; argi++ )
        {
            if (osl_Process_E_None == osl_getCommandArg (argi, &ustrCommandArg))
            {
                if (0 == rtl_ustr_ascii_compare (rtl_uString_getStr (ustrCommandArg), "-bean"))
                {
                    bDoHardKill = sal_True;
                    break;
                }
            }
        }
        if (ustrCommandArg)
        {
            rtl_uString_release (ustrCommandArg);
            ustrCommandArg = 0;
        }

        // WORKAROUND FOR SEGV HANDLER CONFLICT
        //
        // the java jit needs SIGSEGV for proper work 
        // and we need SIGSEGV for the office crashguard
        //
        // TEMPORARY SOLUTION: 
        //   the office sets the signal handler during startup
        //   java can than overwrite it, if needed
        bSetSEGVHandler = sal_True;

        // WORKAROUND FOR WINCH HANDLER (SEE ABOVE)
        bSetWINCHHandler = sal_True;

        // WORKAROUND FOR ILLEGAL INSTRUCTION HANDLER (SEE ABOVE)
        bSetILLHandler = sal_True;
    }

    SignalListMutex = osl_createMutex();

    act.sa_handler = SignalHandlerFunction;
    act.sa_flags   = SA_RESTART;

    sigfillset(&(act.sa_mask));

    /* Initialize the rest of the signals */
    for (i = 0; i < NoSignals; i++) 
    {
        /* hack: stomcatd is attaching JavaVM wich dont work with an sigaction(SEGV) */
        if ((bSetSEGVHandler || Signals[i].Signal != SIGSEGV)
        && (bSetWINCHHandler || Signals[i].Signal != SIGWINCH)
        && (bSetILLHandler   || Signals[i].Signal != SIGILL))
        {
            if (Signals[i].Action != ACT_SYSTEM)
            {
                if (Signals[i].Action == ACT_HIDE)
                {
                    struct sigaction ign;

                    ign.sa_handler = SIG_IGN;
                    ign.sa_flags   = 0;
                    sigemptyset(&ign.sa_mask);

                    if (sigaction(Signals[i].Signal, &ign, &oact) == 0)
                        Signals[i].Handler = oact.sa_handler;
                    else
                        Signals[i].Handler = SIG_DFL;
                }
                else
                    if (sigaction(Signals[i].Signal, &act, &oact) == 0)
                        Signals[i].Handler = oact.sa_handler;
                    else
                        Signals[i].Handler = SIG_DFL;
            }
        }
    }

    return sal_True;
}

static sal_Bool DeInitSignal()
{
    int i;
    struct sigaction act;

    act.sa_flags   = 0;
    sigemptyset(&(act.sa_mask));

    /* Initialize the rest of the signals */
    for (i = NoSignals - 1; i >= 0; i--) 
        if (Signals[i].Action != ACT_SYSTEM)
        {
            act.sa_handler = Signals[i].Handler;

            sigaction(Signals[i].Signal, &act, NULL);
        }

    osl_destroyMutex(SignalListMutex);

    return sal_False;
}

#if defined (SAL_ENABLE_CRASH_REPORT) && defined(INCLUDE_BACKTRACE)
/*****************************************************************************/
/* Generate MD5 checksum	*/
/*****************************************************************************/

static sal_uInt32 calc_md5_checksum( const char *filename, sal_uInt8 *pChecksum, sal_uInt32 nChecksumLen )
{
    sal_uInt32	nBytesProcessed = 0;

    FILE *fp = fopen( filename, "r" );

    if ( fp )
    {
        rtlDigest digest = rtl_digest_createMD5();

        if ( digest )
        {
            size_t			nBytesRead;
            sal_uInt8		buffer[4096];
            rtlDigestError	error = rtl_Digest_E_None;

            while ( rtl_Digest_E_None == error &&
                0 != (nBytesRead = fread( buffer, 1, sizeof(buffer), fp )) )
            {
                error = rtl_digest_updateMD5( digest, buffer, nBytesRead );	
                nBytesProcessed += nBytesRead;
            }

            if ( rtl_Digest_E_None == error )
            {
                error = rtl_digest_getMD5( digest, pChecksum, nChecksumLen );
            }

            if ( rtl_Digest_E_None != error )
                nBytesProcessed = 0;

            rtl_digest_destroyMD5( digest );
        }

        fclose( fp );
    }

    return nBytesProcessed;
}

/*****************************************************************************/
/* Call crash reporter	*/
/*****************************************************************************/

/* Helper function to encode and write a string to a stream */

static int fputs_xml( const char *string, FILE *stream )
{
    int result = 0;

    while ( result >= 0 && *string )
    {
        switch( *string )
        {
        case '&':
            result = fputs( "&amp;", stream );
            break;
        case '<':
            result = fputs( "&lt;", stream );
            break;
        case '>':
            result = fputs( "&gt;", stream );
            break;
        default:
            result = fputc( *string, stream );
            break;
        }

        string++;
    }

    return result;
}
#endif

/* Create intermediate files and run crash reporter */

#define REPORTENV_PARAM		"-crashreportenv:"

static int ReportCrash( int Signal )
{
#ifdef SAL_ENABLE_CRASH_REPORT
    static sal_Bool bCrashReporterExecuted = sal_False;
    sal_Bool		bAutoCrashReport = sal_False;

    sal_uInt32	argi;
    sal_uInt32	argc;
    rtl_uString *ustrCommandArg = NULL;
    
    if ( !bErrorReportingEnabled )
        return -1;

    argc = osl_getCommandArgCount();
    
    for ( argi = 0; argi < argc; argi++ )
    {
        if ( osl_Process_E_None == osl_getCommandArg( argi, &ustrCommandArg ) )
        {
            if ( 0 == rtl_ustr_ascii_compare( rtl_uString_getStr( ustrCommandArg ), "-nocrashreport" ) )
            {
                rtl_uString_release( ustrCommandArg );
                return -1;
            }
            else if ( 0 == rtl_ustr_ascii_compare( rtl_uString_getStr( ustrCommandArg ), "-autocrashreport" ) )
            {
                bAutoCrashReport = sal_True;
            }
            else if ( 0 == rtl_ustr_ascii_shortenedCompare_WithLength( 
                rtl_uString_getStr( ustrCommandArg ), rtl_uString_getLength( ustrCommandArg ), 
                REPORTENV_PARAM, strlen(REPORTENV_PARAM) ) 
                )
            {
                rtl_uString	*ustrEnvironment = NULL;
                rtl_String *strEnv = NULL;

                rtl_uString_newFromStr( &ustrEnvironment, rtl_uString_getStr( ustrCommandArg ) + strlen(REPORTENV_PARAM) );

                if ( ustrEnvironment )
                {
                    rtl_uString2String( 
                        &strEnv, 
                        rtl_uString_getStr( ustrEnvironment ), rtl_uString_getLength( ustrEnvironment ), 
                        osl_getThreadTextEncoding(), OUSTRING_TO_OSTRING_CVTFLAGS 
                        ); 

                    if ( strEnv )
                    {
                        putenv( rtl_string_getStr( strEnv ) );
                        rtl_string_release( strEnv );
                    }

                    rtl_uString_release( ustrEnvironment );
                }

            }

        }
    }
    
    if ( ustrCommandArg )
        rtl_uString_release( ustrCommandArg );
    
    if ( !bCrashReporterExecuted )
    {
        int i;
        /* struct sigaction act; */

        for (i = 0; i < NoSignals; i++) 
        {
            if (Signals[i].Signal == Signal && Signals[i].Action == ACT_ABORT )
            {
                int  ret;
                char szShellCmd[512];
                char *pXMLTempName = NULL;
                char *pStackTempName = NULL;
                char *pChecksumTempName = NULL;

#ifdef INCLUDE_BACKTRACE
                char szXMLTempNameBuffer[L_tmpnam];
                char szChecksumTempNameBuffer[L_tmpnam];
                char szStackTempNameBuffer[L_tmpnam];

                void *stackframes[MAX_STACK_FRAMES];
                int  iFrame;
                int  nFrames = backtrace( stackframes, sizeof(stackframes)/sizeof(stackframes[0]));

                FILE *xmlout, *stackout, *checksumout;
                int fdxml, fdstk, fdchksum;

                strncpy( szXMLTempNameBuffer, P_tmpdir, sizeof(szXMLTempNameBuffer) );
                strncat( szXMLTempNameBuffer, "/crxmlXXXXXX", sizeof(szXMLTempNameBuffer) );

                strncpy( szStackTempNameBuffer, P_tmpdir, sizeof(szStackTempNameBuffer) );
                strncat( szStackTempNameBuffer, "/crstkXXXXXX", sizeof(szStackTempNameBuffer) );

                strncpy( szChecksumTempNameBuffer, P_tmpdir, sizeof(szChecksumTempNameBuffer) );
                strncat( szChecksumTempNameBuffer, "/crchkXXXXXX", sizeof(szChecksumTempNameBuffer) );

                fdxml = mkstemp(szXMLTempNameBuffer);
                fdstk = mkstemp(szStackTempNameBuffer);
                fdchksum = mkstemp(szChecksumTempNameBuffer);

                xmlout = fdopen( fdxml , "w" );
                stackout = fdopen( fdstk , "w" );
                checksumout = fdopen( fdchksum, "w" );
            
                pXMLTempName = szXMLTempNameBuffer;
                pStackTempName = szStackTempNameBuffer;
                pChecksumTempName = szChecksumTempNameBuffer;


                if ( xmlout && stackout && checksumout )
                {
                    fprintf( xmlout, "<errormail:Stack type=\"%s\">\n", STACKTYPE );

                    fprintf( checksumout, "<errormail:Checksums type=\"MD5\">\n" );

                    for ( iFrame = 0; iFrame < nFrames; iFrame++ )
                    {
                        Dl_info dl_info;

                        fprintf( stackout, "0x%x:",
                            (unsigned int)stackframes[iFrame] );

                        fprintf( xmlout, "<errormail:StackInfo pos=\"%d\" ip=\"0x%x\"",
                            iFrame,
                            (unsigned int)stackframes[iFrame]
                            );

                        memset( &dl_info, 0, sizeof(dl_info) );

                        /* dladdr may fail */
                        if ( dladdr( stackframes[iFrame], &dl_info) )
                        {
                            const char *dli_fname = NULL;
                            char *dli_fdir = NULL;
                            char szDirectory[PATH_MAX];
                            char szCanonicDirectory[PATH_MAX];

                            /* Don't expect that dladdr filled all members of dl_info */

                            dli_fname = dl_info.dli_fname ? strrchr(  dl_info.dli_fname, '/' ) : NULL;
                            if ( dli_fname )
                            {
                                ++dli_fname;
                                memcpy( szDirectory, dl_info.dli_fname, dli_fname - dl_info.dli_fname );
                                szDirectory[dli_fname - dl_info.dli_fname] = 0;

                                dli_fdir = realpath( szDirectory, szCanonicDirectory ) ? szCanonicDirectory : szDirectory;

                                if ( *dli_fdir && dli_fdir[ strlen(dli_fdir) - 1 ] != '/' )
                                    strcat( dli_fdir, "/" );
                            }
                            else
                                dli_fname = dl_info.dli_fname;

                            /* create checksum of library on stack */
                            if ( dli_fname )
                            {
                                sal_uInt8	checksum[RTL_DIGEST_LENGTH_MD5];

                                sal_uInt32 nBytesProcessed = calc_md5_checksum( 
                                    dl_info.dli_fname, checksum, sizeof(checksum) );
                                if ( nBytesProcessed )
                                {
                                    int j;

                                    fprintf( checksumout, "<errormail:Checksum sum=\"0x" );
                                    for ( j = 0; j < 16; fprintf( checksumout, "%02X", checksum[j++] ) );
                                    fprintf( checksumout,
                                        "\" bytes=\"%lu\" file=\"%s\"/>\n",
                                        SAL_INT_CAST(
                                            unsigned long, nBytesProcessed),
                                        dli_fname );
                                }
                            }

                            if ( dl_info.dli_fbase && dl_info.dli_fname )
                            {
                                fprintf( stackout, " %s + 0x%x",
                                    dl_info.dli_fname,
                                    (char*)stackframes[iFrame] - (char*)dl_info.dli_fbase
                                    );

                                fprintf( xmlout, " rel=\"0x%x\"", (char *)stackframes[iFrame] - (char *)dl_info.dli_fbase );
                                if ( dli_fname )
                                    fprintf( xmlout, " name=\"%s\"", dli_fname );

                                if ( dli_fdir )
                                    fprintf( xmlout, " path=\"%s\"", dli_fdir );
                            }
                            else
                                fprintf( stackout, " ????????" );

                            if ( dl_info.dli_sname && dl_info.dli_saddr )
                            {
                                fputs( " (", stackout );
                                fputs_xml( dl_info.dli_sname, stackout );
                                fprintf( stackout, " + 0x%x)", 
                                    (char*)stackframes[iFrame] - (char*)dl_info.dli_saddr );

                                fputs( " ordinal=\"", xmlout );
                                fputs_xml( dl_info.dli_sname, xmlout );
                                fprintf( xmlout, "+0x%x\"", 
                                    (char *)stackframes[iFrame] - (char *)dl_info.dli_saddr );
                            }

                        }
                        else /* dladdr failed */
                        {
                            fprintf( stackout, " ????????" );
                        }

                        fprintf( stackout, "\n" );
                        fprintf( xmlout, "/>\n" );

                    }

                    fprintf( xmlout, "</errormail:Stack>\n" );
                    fprintf( checksumout, "</errormail:Checksums>\n" );

                    fclose( stackout );
                    fclose( xmlout );
                    fclose( checksumout );
                }
                else
                {
                    pXMLTempName = NULL;
                    pStackTempName = NULL;
                    pChecksumTempName = NULL;
                }

#if defined( LINUX )
                if ( pXMLTempName && pChecksumTempName && pStackTempName )
                    snprintf( szShellCmd, sizeof(szShellCmd)/sizeof(szShellCmd[0]),
                        "crash_report -p %d -s %d -xml %s -chksum %s -stack %s%s", 
                        getpid(), 
                        Signal, 
                        pXMLTempName, 
                        pChecksumTempName, 
                        pStackTempName,
                        bAutoCrashReport ? " -noui -send" : " -noui" );
#elif defined ( SOLARIS )
                if ( pXMLTempName && pChecksumTempName )
                    snprintf( szShellCmd, sizeof(szShellCmd)/sizeof(szShellCmd[0]),
                        "crash_report -p %d -s %d -xml %s -chksum %s%s", 
                        getpid(), 
                        Signal, 
                        pXMLTempName, 
                        pChecksumTempName,
                        bAutoCrashReport ? " -noui -send" : " -noui" );
#endif

#else /* defined INCLUDE BACKTRACE */
                snprintf( szShellCmd, sizeof(szShellCmd)/sizeof(szShellCmd[0]),
                "crash_report -p %d -s %d%s", getpid(), Signal, bAutoCrashReport ? " -noui -send" : " -noui" );
#endif /* defined INCLUDE BACKTRACE */


                ret = system( szShellCmd );

                if ( pXMLTempName )
                    unlink( pXMLTempName );

                if ( pStackTempName )
                    unlink( pStackTempName );

                if ( pChecksumTempName )
                    unlink( pChecksumTempName );

                if ( -1 != ret )
                {
                    bCrashReporterExecuted = sal_True;
                    return 1;
                }
                else
                    return -1;
                
            }
        }
        
        return 0;
    }
    
    return 1;
#else /* defined SAL_ENABLE_CRASH_REPORT */
    /* the utility crash_report is not build, so do the same as when
       the option -nocrashreport is used */
    (void) Signal; // avoid warnings
    return -1;
#endif /* defined SAL_ENABLE_CRASH_REPORT */
}

static void PrintStack( int sig )
{
#ifndef MACOSX
    void *buffer[MAX_STACK_FRAMES];
    int size = backtrace( buffer, sizeof(buffer) / sizeof(buffer[0]) );
#endif

    fprintf( stderr, "\n\nFatal exception: Signal %d\n", sig );

#ifdef MACOSX
    fprintf( stderr, "Please turn on Enable Crash Reporting and\nAutomatic Display of Crashlogs in the Console application\n" );
#else
    if ( size > 0 )
    {
        fputs( "Stack:\n", stderr );
        backtrace_symbols_fd( buffer, size, fileno(stderr) );
    }
#endif
}

static oslSignalAction CallSignalHandler(oslSignalInfo *pInfo)
{
    oslSignalHandlerImpl* pHandler = SignalList;
    oslSignalAction Action = osl_Signal_ActCallNextHdl;

    while (pHandler != NULL)
    {
        if ((Action = pHandler->Handler(pHandler->pData, pInfo)) 
            != osl_Signal_ActCallNextHdl)
            break;

        pHandler = pHandler->pNext;
    }
    
    return Action;
}

void CallSystemHandler(int Signal)
{
    int i;
    struct sigaction act;

    for (i = 0; i < NoSignals; i++) 
    {
        if (Signals[i].Signal == Signal)
            break;
    }

    if (i < NoSignals)
    {
        if ((Signals[i].Handler == NULL)    ||
            (Signals[i].Handler == SIG_DFL) ||
            (Signals[i].Handler == SIG_IGN) ||
             (Signals[i].Handler == SIG_ERR))
        {
            switch (Signals[i].Action)
            {
                case ACT_EXIT:		/* terminate */
                    /* prevent dumping core on exit() */
                    _exit(255);
                    break;

                case ACT_ABORT:		/* terminate witch core dump */
                    ReportCrash( Signal );
                    act.sa_handler = SIG_DFL;
                    act.sa_flags   = 0;
                    sigemptyset(&(act.sa_mask));
                    sigaction(SIGABRT, &act, NULL);
                    PrintStack( Signal );
                    abort();
                    break;

                case ACT_IGNORE:	/* ignore */
                    break;

                default:			/* should never happen */
                    OSL_ASSERT(0);
            }
        }
        else
            (*Signals[i].Handler)(Signal);
    }
}


/*****************************************************************************/
/* SignalHandlerFunction	*/
/*****************************************************************************/
void SignalHandlerFunction(int Signal)
{
    oslSignalInfo	 Info;
    struct sigaction act;

    Info.UserSignal = Signal;
    Info.UserData   = NULL;

    switch (Signal)
    {
        case SIGBUS:
        case SIGILL:
        case SIGSEGV:
        case SIGIOT:
#if ( SIGIOT != SIGABRT )
        case SIGABRT:
#endif
            Info.Signal = osl_Signal_AccessViolation;
            break; 

        case -1:
            Info.Signal = osl_Signal_IntegerDivideByZero;
            break; 

        case SIGFPE:
            Info.Signal = osl_Signal_FloatDivideByZero;
            break; 

        case SIGINT:
        case SIGTERM:
    case SIGQUIT:
    case SIGHUP:
            Info.Signal = osl_Signal_Terminate;
            break; 

        default:
            Info.Signal = osl_Signal_System;
            break; 
    }

    ReportCrash( Signal );
    
    /* Portal Demo HACK !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
    if (bDoHardKill && (Info.Signal == osl_Signal_AccessViolation))
        _exit(255);
    /* Portal Demo HACK !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
    

    switch (CallSignalHandler(&Info))
    {
    case osl_Signal_ActCallNextHdl:
        CallSystemHandler(Signal);
        break;

    case osl_Signal_ActAbortApp:
        ReportCrash( Signal );
        act.sa_handler = SIG_DFL;
        act.sa_flags   = 0;
        sigemptyset(&(act.sa_mask));
        sigaction(SIGABRT, &act, NULL);
        PrintStack( Signal );
        abort();
        break;

    case osl_Signal_ActKillApp:
        /* prevent dumping core on exit() */
        _exit(255);
        break;
    default:
        break;
    }
}

/*****************************************************************************/
/* osl_addSignalHandler */
/*****************************************************************************/
oslSignalHandler SAL_CALL osl_addSignalHandler(oslSignalHandlerFunction Handler, void* pData)
{
    oslSignalHandlerImpl* pHandler;

    OSL_ASSERT(Handler != NULL);
    if ( Handler == 0 )
    {
        return 0;
    }

    if (! bInitSignal)
        bInitSignal = InitSignal();

    pHandler = (oslSignalHandlerImpl*) calloc(1, sizeof(oslSignalHandlerImpl));

    if (pHandler != NULL)
    {
        pHandler->Handler = Handler;
        pHandler->pData   = pData;

        osl_acquireMutex(SignalListMutex);
    
        pHandler->pNext = SignalList;
        SignalList      = pHandler;

        osl_releaseMutex(SignalListMutex);

        return (pHandler);
    }
    
    return (NULL);
}

/*****************************************************************************/
/* osl_removeSignalHandler */
/*****************************************************************************/
sal_Bool SAL_CALL osl_removeSignalHandler(oslSignalHandler Handler)
{
    oslSignalHandlerImpl *pHandler, *pPrevious = NULL;

    OSL_ASSERT(Handler != NULL);

    if (! bInitSignal)
        bInitSignal = InitSignal();

    osl_acquireMutex(SignalListMutex);

    pHandler = SignalList;

    while (pHandler != NULL)
    {
        if (pHandler == Handler)
        {
            if (pPrevious)
                pPrevious->pNext = pHandler->pNext;
            else
                SignalList = pHandler->pNext;

            osl_releaseMutex(SignalListMutex);

            if (SignalList == NULL)
                bInitSignal = DeInitSignal();

            free(pHandler);

            return (sal_True);
        }

        pPrevious = pHandler;
        pHandler  = pHandler->pNext;
    }

    osl_releaseMutex(SignalListMutex);

    return (sal_False);
}

/*****************************************************************************/
/* osl_raiseSignal */
/*****************************************************************************/
oslSignalAction SAL_CALL osl_raiseSignal(sal_Int32 UserSignal, void* UserData)
{
    oslSignalInfo   Info;
    oslSignalAction Action;

    if (! bInitSignal)
        bInitSignal = InitSignal();

    osl_acquireMutex(SignalListMutex);

    Info.Signal     = osl_Signal_User;
    Info.UserSignal = UserSignal;
    Info.UserData   = UserData;

    Action = CallSignalHandler(&Info);

    osl_releaseMutex(SignalListMutex);

    return (Action);
}

/*****************************************************************************/
/* osl_setErrorReporting */
/*****************************************************************************/
sal_Bool SAL_CALL osl_setErrorReporting( sal_Bool bEnable )
{
    sal_Bool bOld = bErrorReportingEnabled;
    bErrorReportingEnabled = bEnable;

    return bOld;
}
