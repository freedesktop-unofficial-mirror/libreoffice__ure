eval 'exec perl -wS $0 ${1+"$@"}'
    if 0;


# #!/usr/bin/perl -w

use strict;
use POSIX;
use Cwd;
use File::Path;

use English;
use Cwd 'chdir';
my $cwd = getcwd();

my $g_sTempDir = "";
my $FS = "";

my $nGlobalFailures = 0;

# LLA: this does not exist, ... use a little bit simpler method.
# use File::Temp qw/ :POSIX /;

my $params;
my $param;
if ($#ARGV < 0)
{
    $params = "test "; # debug=t TESTOPTADD=\"-boom\"   TESTOPTADD=\"-noerroronexit\"

    # my $nNumber = 55;
    # my $sLocalParams = $params;
    # $sLocalParams =~ s/test\s/test$nNumber /;
    # print "Testparams: $sLocalParams\n";
    # exit 1;
    print "Default ";
}
else
{
    # special hack!
    if (checkForKillobj() == 1)
    {
        $params = "killobj";
    }
    else
    {
        # always run test, but envelope the other in 'TESTOPT="..."'
        $params = "test TESTOPT=\"";
        
        foreach $param (@ARGV)
        {
            $params = $params . " " . $param;
        }
        $params = $params . "\"";    
    }
    print "User defined ";
}

print "parameters for dmake: $params\n";

initEnvironment();
main($params);


# ------------------------------------------------------------------------------
sub checkForKillobj()
{
    my $sLocalParam;
    my $nBackValue = 0;
    foreach $sLocalParam (@ARGV)
    {
        if ($sLocalParam =~ /^killobj$/)
        {
            $nBackValue = 1;
            last;
        }
    }
    return $nBackValue;
}

# ------------------------------------------------------------------------------
sub initEnvironment()
{
    my $gui = $ENV{GUI};
    # no error output in forms of message boxes
    $ENV{'DISABLE_SAL_DBGBOX'}="t";

  SWITCH: {
        if ( $gui eq "WNT" ) {
            $FS             = "\\";
            $g_sTempDir         = $ENV{TMP}  ? "$ENV{TMP}${FS}" : "c:${FS}tmp${FS}";
            last SWITCH;
        } 
        if ( $gui eq "WIN" ) {
            $FS             = "\\";
            $g_sTempDir         = $ENV{TMP}  ? "$ENV{TMP}${FS}" : "c:${FS}tmp${FS}";
            last SWITCH;
        }    
        if ( $gui eq "OS2" ) {
            $FS             = "\\";
            $g_sTempDir         = $ENV{TMP}  ? "$ENV{TMP}${FS}" : "c:${FS}tmp${FS}";
            last SWITCH;
        }
        if ( $gui eq "UNX" ) {
            $FS             = "/";
            $g_sTempDir         = $ENV{TMP}  ? "$ENV{TMP}${FS}" : "${FS}tmp${FS}";
            last SWITCH;
        }
        print STDERR "buildall.pl: unkown platform\n";
        exit(1);
    }
}
# ------------------------------------------------------------------------------

sub trim($)
{
    my $oldstr = shift;
    $oldstr =~ s/^\s*(.*?)\s*$/$1/;
    return $oldstr;
}

# ------------------------------------------------------------------------------
sub getLibName($)
{
    my $sFile = shift;
    if ($OSNAME eq "linux" || $OSNAME eq "solaris")
    {
        return "lib" . $sFile . ".so";
    }
    if ($OSNAME eq "MSWin32")
    {
        return $sFile . ".dll";
    }
    return $sFile;
}
# ------------------------------------------------------------------------------
sub giveOutFailures($$)
{
    my $sTest = shift;
    my $sFailureFile = shift;

    my $bBegin = 0;
    my $nFailures = 0;

    my $line;
    local *IN;
    if (! open(IN, $sFailureFile))
    {
        print "ERROR: Can't open output file $sFailureFile\n";
        return;
    }
    while ($line = <IN>)
    {
        chomp($line);

        # handling of the states
        if ( $line =~ /^\# -- BEGIN:/)
        {
            $bBegin = 1;
        }
        elsif ( $line =~ /^\# -- END:/)
        {
            $bBegin = 0;
        }
        else
        {
            if ($bBegin == 1)
            {
                print "$line\n";
                $nFailures++;
            }
        }
    }
    close(IN);
    if ($nFailures > 0)
    {
        # extra return for a better output
        print "\nFailures occured: $nFailures\n";
        print "The whole output can be found in $sFailureFile\n";
        print "\n";

        # Statistics
        $nGlobalFailures += $nFailures;
    }
}
# ------------------------------------------------------------------------------
sub runASingleTest($$)
{
    my $sTarget = shift;
    my $params = shift;
    my $dmake = "dmake $params";

    my $sLogPath = $g_sTempDir . "dmake_out_$$";
    mkdir($sLogPath);
    my $sLogFile = $sLogPath . "/" . $sTarget . ".out";

    print "       on library: " . getLibName($sTarget) . "\n";

# redirect tcsh ">&" (stdout, stderr)
# redirect 4nt   ">" (stdout), "2>" (stderr)
# print "OSNAME: $OSNAME\n";
# LLA: redirect check canceled, seems to be not work as I want.
#     my $redirect = "";
#     if ($OSNAME eq "linux" || $OSNAME eq "solaris")
#     {
#         # print "UNIX, linux or solaris\n";
#         $redirect = '>>&!' . $sLogFile;
#     }
#     else
#     {
#         if ($OSNAME eq "MSWin32")
#         {
#             # test
#             $redirect = ">>$sLogFile 2>>$sLogFile";
#         }
#     }
#     print "$dmake $redirect\n";

# LLA: so system does also not work as I imagine
#    system("$dmake $redirect");

# LLA: next check, use open with pipe

    local *LOGFILE;
    if (! open( LOGFILE, '>' . "$sLogFile"))
    {
        print "ERROR: can't open logfile: $sLogFile\n";
        return;
    }
    
    my $line;
    local *DMAKEOUTPUT;
    if (! open( DMAKEOUTPUT, "$dmake 2>&1 |"))
    {
        print "ERROR: can't open dmake\n";
        return;
    }
    while ($line = <DMAKEOUTPUT>)
    {
        chomp($line);
        print LOGFILE "$line\n";
    }
    close(DMAKEOUTPUT);
    close(LOGFILE);

    giveOutFailures($sTarget, $sLogFile);
}
# ------------------------------------------------------------------------------
sub interpretLine($)
{
    my $line = shift;

    my $path;
    my $file;

    if ($line =~ /^\#/ || $line =~ /^$/)
    {
        # remark or empty line
    }
    else
    {
        # special format, $file == $path
        ($path, $file) = split(/;/, $line);
        if (! $file)
        {
            $file = $path;
        }
        $file = trim($file);
        $path = trim($path);
    }
    return $path, $file;
}
# ------------------------------------------------------------------------------
sub runTestsOnPath($$$)
{
    my $path = shift;
    my $file = shift;
    my $params = shift;

    # empty values
    if (!$path || $path eq "")
    {
        # DBG: print "empty path '$path'\n";
        return;
    }
    if (!$file || $file eq "")
    {
        # DBG: print "empty file '$file'\n";
        return;
    }
        
#   print "File: '$file', Path: '$path'\n";
    print "Work in directory: $path\n";
    my $newpath = $cwd . $FS . $path;
#   print "chdir to $newpath\n";

    my $error = chdir($newpath);
    cwd();

    # run through the hole makefile.mk and check if SHL<D>TARGET = ... exist, for every target call "dmake test<D>"

    local *MAKEFILE_MK;
    if (! open(MAKEFILE_MK, "makefile.mk"))
    {
        print "ERROR: can't open makefile.mk in path: $newpath\n";
        print "please check your libs2test.txt file in qa directory.\n";
    }
    my $line;
    my $nNumber;
    my $sTarget;
    my $sLocalParams;

    while($line = <MAKEFILE_MK>)
    {
        chomp($line);
        
        if ($line =~ /SHL(\d)TARGET=(.*)/)
        {
            $nNumber = $1;
            $sTarget = trim($2);
            
            # DBG: print "test$number is lib: $target\n";
            $sLocalParams = $params . " ";                  # append a whitespace, so we can check if 'test' exist without additional digits
            $sLocalParams =~ s/test\s/test$nNumber/;
            # DBG: print "$sLocalParams\n";
            runASingleTest($sTarget, $sLocalParams);
        }
    }
    close(MAKEFILE_MK);
}

# ------------------------------------------------------------------------------

sub main($)
{
    my $params = shift;
#    my $sLogFile = shift;     # "buildall_$$.out";
    local *LIBS2TEST;
    my $filename = "libs2test.txt";
    my $line;

    open(LIBS2TEST, $filename) || die "can't open $filename\n";

    while($line = <LIBS2TEST>)
    {
        chomp($line);
        # DOS Hack grrrr...
        while ($line =~ /$/)
        {
            $line = substr($line, 0, -1);
        }

        # print "$line\n";
        my $path;
        my $file;
        ($path, $file) = interpretLine($line);
        runTestsOnPath($path, $file, $params);
    }
    close(LIBS2TEST);
    
    print "\nComplete logging information will be found in dir: ".$g_sTempDir."dmake_out_$$/\n";

    if ($nGlobalFailures > 0)
    {
        print "\nFailures over all occured: $nGlobalFailures\n";
    }
}

# ------------------------------------------------------------------------------

# END!

