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
#include "precompiled_idlc.hxx"

#include "idlc/options.hxx"

#include <stdio.h>
#include <string.h>

using namespace rtl;

Options::Options(): m_stdin(false), m_verbose(false), m_quiet(false)
{
}	

Options::~Options()
{
}

sal_Bool Options::initOptions(int ac, char* av[], sal_Bool bCmdFile) 
    throw( IllegalArgument )
{
    sal_Bool 	ret = sal_True;
    sal_uInt16	j=0;
    
    if (!bCmdFile)
    {
        bCmdFile = sal_True;
        
        m_program = av[0];
        
        if (ac < 2)
        {
            fprintf(stderr, "%s", prepareHelp().getStr());
            ret = sal_False;
        }
        
        j = 1;
    } else
    {
        j = 0;
    }
    
    char	*s=NULL;
    for (; j < ac; j++)
    {
        if (av[j][0] == '-')
        {
            switch (av[j][1])
            {
            case 'O':
                if (av[j][2] == '\0')
                {
                    if (j < ac - 1 && av[j+1][0] != '-')
                    {
                        j++;
                        s = av[j];
                    } else
                    {
                        OString tmp("'-O', please check");
                        if (j <= ac - 1)
                        {
                            tmp += " your input '" + OString(av[j+1]) + "'";
                        }
                        
                        throw IllegalArgument(tmp);
                    }
                } else
                {
                    s = av[j] + 2;
                }
                
                m_options["-O"] = OString(s);
                break;
            case 'I':
            {
                if (av[j][2] == '\0')
                {
                    if (j < ac - 1 && av[j+1][0] != '-')
                    {
                        j++;
                        s = av[j];
                    } else
                    {
                        OString tmp("'-I', please check");
                        if (j <= ac - 1)
                        {
                            tmp += " your input '" + OString(av[j+1]) + "'";
                        }
                        
                        throw IllegalArgument(tmp);
                    }
                } else
                {
                    s = av[j] + 2;
                }
                
                OString inc(s);
                if ( inc.indexOf(';') > 0 )
                {
                    OString tmp(s);
                    sal_Int32 nIndex = 0;
                    inc = OString();
                    do inc = inc + " -I\"" + tmp.getToken( 0, ';', nIndex ) +"\""; while( nIndex != -1 );
                } else
                    inc = OString("-I\"") + s + "\"";
                
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
                if (av[j][2] == '\0')
                {
                    if (j < ac - 1 && av[j+1][0] != '-')
                    {
                        j++;
                        s = av[j];
                    } else
                    {
                        OString tmp("'-D', please check");
                        if (j <= ac - 1)
                        {
                            tmp += " your input '" + OString(av[j+1]) + "'";
                        }
                        
                        throw IllegalArgument(tmp);
                    }
                } else
                {
                    s = av[j];
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
                if (av[j][2] != '\0')
                {
                    throw IllegalArgument(OString(av[j]) + ", please check your input");
                } 				
                if (m_options.count("-C") == 0)
                    m_options["-C"] = OString(av[j]);
                break;
            case 'c':
                if (av[j][2] == 'i' && av[j][3] == 'd' && av[j][4] == '\0')
                {
                    if (m_options.count("-cid") == 0)
                        m_options["-cid"] = OString(av[j]);
                } else
                    throw IllegalArgument(OString(av[j]) + ", please check your input");
                break;
            case 'v':
                if ( 0 == strcmp( &av[j][1], "verbose" ) )
                {
                    m_verbose = true;
                }
                else
                    throw IllegalArgument(OString(av[j]) + ", please check your input");
                break;
            case 'q':
                if ( 0 == strcmp( &av[j][1], "quiet" ) )
                {
                    m_quiet = true;
                }
                else
                    throw IllegalArgument(OString(av[j]) + ", please check your input");
                break;
            case 'w':
                if (av[j][2] == 'e' && av[j][3] == '\0') {
                    if (m_options.count("-we") == 0)
                        m_options["-we"] = OString(av[j]);
                } else {
                    if (av[j][2] == '\0') { 
                        if (m_options.count("-w") == 0)
                            m_options["-w"] = OString(av[j]);
                    } else
                        throw IllegalArgument(OString(av[j]) + ", please check your input");
                }                    
                break;
            case 'h':
            case '?':
                if (av[j][2] != '\0')
                {
                    throw IllegalArgument(OString(av[j]) + ", please check your input");
                } else
                {
                    fprintf(stdout, "%s", prepareHelp().getStr());
                    exit(0);
                }					
            case 's':
                if (strcmp(&av[j][2], "tdin") == 0)
                {
                    m_stdin = true;
                    break;
                }
                // fall through
            default:
                throw IllegalArgument("the option is unknown" + OString(av[j]));
            }
        } else
        {
            if (av[j][0] == '@')
            {
                FILE* cmdFile = fopen(av[j]+1, "r");
                  if( cmdFile == NULL )
                  {
                    fprintf(stderr, "%s", prepareHelp().getStr());
                    ret = sal_False;
                } else
                {
                    std::vector< std::string > args;

                    std::string buffer;
                    buffer.reserve(256);

                    bool quoted = false;
                    int c = EOF;
                    while ((c = fgetc(cmdFile)) != EOF)
                    {
                        switch(c)
                        {
                        case '\"':
                            quoted = !quoted;
                            break;
                        case ' ':
                        case '\t':
                        case '\r':
                        case '\n':
                            if (!quoted)
                            {
                                if (!buffer.empty())
                                {
                                    // append current argument.
                                    args.push_back(buffer);
                                    buffer.clear();
                                }
                                break;
                            }
                        default:
                            // quoted white-space fall through
                            buffer.push_back(sal::static_int_cast<char>(c));
                            break;
                        }
                    }
                    if (!buffer.empty())
                    {
                        // append unterminated argument.
                        args.push_back(buffer);
                        buffer.clear();
                    }
                    (void) fclose(cmdFile);
                    
                    ret = initOptions(args.size(), args.data(), bCmdFile);
                }		
            } else
            {
                OString name(av[j]);
                name = name.toAsciiLowerCase();
                if ( name.lastIndexOf(".idl") != (name.getLength() - 4) )
                {
                    throw IllegalArgument("'" + OString(av[j]) + 
                        "' is not a valid input file, only '*.idl' files will be accepted");
                }
                m_inputFiles.push_back(av[j]);
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

sal_Bool Options::isValid(const OString& option)
{
    return (m_options.count(option) > 0);	
}	

const OString Options::getOption(const OString& option)
    throw( IllegalArgument )
{
    if (m_options.count(option) > 0)
    {
        return m_options[option];
    } else
    {
        throw IllegalArgument("Option is not valid or currently not set.");
    }
}
