/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: options.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: hr $ $Date: 2005-10-27 14:20:31 $
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

#include <stdio.h>
#include /*MSVC trouble: <cstring>*/ <string.h>

#ifndef _IDLC_OPTIONS_HXX_
#include <idlc/options.hxx>
#endif

using namespace rtl;

Options::Options(): m_stdin(false)
{
}	

Options::~Options()
{
}

sal_Bool Options::initOptions(int ac, char* av[], sal_Bool bCmdFile) 
    throw( IllegalArgument )
{
    sal_Bool 	ret = sal_True;
    sal_uInt16	i=0;
    
    if (!bCmdFile)
    {
        bCmdFile = sal_True;
        
        m_program = av[0];
        
        if (ac < 2)
        {
            fprintf(stderr, "%s", prepareHelp().getStr());
            ret = sal_False;
        }
        
        i = 1;
    } else
    {
        i = 0;
    }
    
    char	*s=NULL;
    for (i; i < ac; i++)
    {
        if (av[i][0] == '-')
        {
            switch (av[i][1])
            {
            case 'O':
                if (av[i][2] == '\0')
                {
                    if (i < ac - 1 && av[i+1][0] != '-')
                    {
                        i++;
                        s = av[i];
                    } else
                    {
                        OString tmp("'-O', please check");
                        if (i <= ac - 1)
                        {
                            tmp += " your input '" + OString(av[i+1]) + "'";
                        }
                        
                        throw IllegalArgument(tmp);
                    }
                } else
                {
                    s = av[i] + 2;
                }
                
                m_options["-O"] = OString(s);
                break;
            case 'I':
            {
                if (av[i][2] == '\0')
                {
                    if (i < ac - 1 && av[i+1][0] != '-')
                    {
                        i++;
                        s = av[i];
                    } else
                    {
                        OString tmp("'-I', please check");
                        if (i <= ac - 1)
                        {
                            tmp += " your input '" + OString(av[i+1]) + "'";
                        }
                        
                        throw IllegalArgument(tmp);
                    }
                } else
                {
                    s = av[i] + 2;
                }
                
                OString inc(s);
                if ( inc.indexOf(';') > 0 )
                {
                    OString tmp(s);
                    sal_Int32 nIndex = 0;
                    inc = OString();
                    do inc = inc + " -I" + tmp.getToken( 0, ';', nIndex ); while( nIndex != -1 );
                } else
                    inc = OString("-I") + s;
                
                if (m_options.count("-I") > 0)
                {
                    OString tmp(m_options["-I"]);
                    tmp = tmp + " " + inc;
                    m_options["-I"] = tmp;
                } else
                {
                    m_options["-I"] = inc;
                }
            }
            break;
            case 'D':
                if (av[i][2] == '\0')
                {
                    if (i < ac - 1 && av[i+1][0] != '-')
                    {
                        i++;
                        s = av[i];
                    } else
                    {
                        OString tmp("'-D', please check");
                        if (i <= ac - 1)
                        {
                            tmp += " your input '" + OString(av[i+1]) + "'";
                        }
                        
                        throw IllegalArgument(tmp);
                    }
                } else
                {
                    s = av[i];
                }
                
                if (m_options.count("-D") > 0)
                {
                    OString tmp(m_options["-D"]);
                    tmp = tmp + " " + s;
                    m_options["-D"] = tmp;
                } else
                    m_options["-D"] = OString(s);
                break;
            case 'C':
                if (av[i][2] != '\0')
                {
                    throw IllegalArgument(OString(av[i]) + ", please check your input");
                } 				
                if (m_options.count("-C") == 0)
                    m_options["-C"] = OString(av[i]);
                break;
            case 'c':
                if (av[i][2] == 'i' && av[i][3] == 'd' && av[i][4] == '\0')
                {
                    if (m_options.count("-cid") == 0)
                        m_options["-cid"] = OString(av[i]);
                } else
                    throw IllegalArgument(OString(av[i]) + ", please check your input");
                break;
            case 'w':
                if (av[i][2] == 'e' && av[i][3] == '\0') {
                    if (m_options.count("-we") == 0)
                        m_options["-we"] = OString(av[i]);
                } else {
                    if (av[i][2] == '\0') { 
                        if (m_options.count("-w") == 0)
                            m_options["-w"] = OString(av[i]);
                    } else
                        throw IllegalArgument(OString(av[i]) + ", please check your input");
                }                    
                break;
            case 'h':
            case '?':
                if (av[i][2] != '\0')
                {
                    throw IllegalArgument(OString(av[i]) + ", please check your input");
                } else
                {
                    fprintf(stdout, "%s", prepareHelp().getStr());
                    exit(0);
                }					
                break;
            case 's':
                if (/*MSVC trouble: std::*/strcmp(&av[i][2], "tdin") == 0)
                {
                    m_stdin = true;
                    break;
                }
                // fall through
            default:
                throw IllegalArgument("the option is unknown" + OString(av[i]));
                break;					
            }
        } else
        {
            if (av[i][0] == '@')
            {
                FILE* cmdFile = fopen(av[i]+1, "r");
                  if( cmdFile == NULL )
                  {
                    fprintf(stderr, "%s", prepareHelp().getStr());
                    ret = sal_False;
                } else
                {
                    int rargc=0;
                    char* rargv[512];
                    char  buffer[512]="";

                    int i=0;
                    int found = 0;
                    char c;
                    while ( fscanf(cmdFile, "%c", &c) != EOF )
                    {
                        if (c=='\"') {
                            if (found) {
                                found=0;
                            } else {
                                found=1;
                                continue;
                            }
                        } else {
                            if (c!=13 && c!=10) {
                                if (found || c!=' ') {
                                    buffer[i++]=c;
                                    continue;                
                                }
                            }
                            if (i==0)
                                continue;
                        }
                        buffer[i]='\0';
                        found=0;
                        i=0;
                        rargv[rargc]= strdup(buffer);
                        rargc++;
                        buffer[0]='\0';
                    }
                    if (buffer[0] != '\0') {
                        buffer[i]='\0';
                        rargv[rargc]= strdup(buffer);
                        rargc++;
                    }
                    fclose(cmdFile);
                    
                    ret = initOptions(rargc, rargv, bCmdFile);
                    
                    long ii = 0;
                    for (ii=0; ii < rargc; ii++) 
                    {
                        free(rargv[ii]);
                    }
                }		
            } else
            {
                OString name(av[i]);
                name = name.toAsciiLowerCase();
                if ( name.lastIndexOf(".idl") != (name.getLength() - 4) )
                {
                    throw IllegalArgument("'" + OString(av[i]) + 
                        "' is not a valid input file, only '*.idl' files will be accepted");
                }
                m_inputFiles.push_back(av[i]);
            }		
        }
    }
    
    return ret;	
}	

OString	Options::prepareHelp()
{
    OString help("\nusing: ");
    help += m_program
        + " [-options] <file_1> ... <file_n> | @<filename> | -stdin\n";
    help += "    <file_n>    = file_n specifies one or more idl files.\n";
    help += "                  Only files with the extension '.idl' are valid.\n";
    help += "    @<filename> = filename specifies the name of a command file.\n";
    help += "    -stdin      = read idl file from standard input.\n";
    help += "  Options:\n";
    help += "    -O<path>    = path specifies the output directory.\n";
    help += "                  The generated output is a registry file with\n";
    help += "                  the same name as the idl input file (or 'stdin'\n";
    help += "                  for -stdin).\n";
    help += "    -I<path>    = path specifies a directory where include\n";
    help += "                  files will be searched by the preprocessor.\n";
    help += "                  Multiple directories can be combined with ';'.\n";
    help += "    -D<name>    = name defines a macro for the preprocessor.\n";
    help += "    -C          = generate complete type information, including\n";
    help += "                  documentation.\n";
    help += "    -cid        = check if identifiers fulfill the UNO naming\n";
    help += "                  requirements.\n";
    help += "    -w          = display warning messages.\n";
    help += "    -we         = treat warnings as errors.\n";
    help += "    -h|-?       = print this help message and exit.\n";
    help += prepareVersion();
    
    return help;
}	

OString	Options::prepareVersion()
{
    OString version("\nSun Microsystems (R) ");
    version += m_program + " Version 1.1\n\n";
    return version;
}	

const OString& Options::getProgramName() const
{
    return m_program;
}	

sal_uInt16 Options::getNumberOfOptions() const
{
    return (sal_uInt16)(m_options.size());
}	

sal_Bool Options::isValid(const OString& option)
{
    return (m_options.count(option) > 0);	
}	

const OString Options::getOption(const OString& option)
    throw( IllegalArgument )
{
    const OString ret;

    if (m_options.count(option) > 0)
    {
        return m_options[option];
    } else
    {
        throw IllegalArgument("Option is not valid or currently not set.");
    }

    return ret;
}

const OptionMap& Options::getOptions()
{
    return m_options;	
}	
