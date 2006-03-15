/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: cppuoptions.cxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: vg $ $Date: 2006-03-15 09:13:03 $
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
#include <string.h>

#include "cppuoptions.hxx"
#include "osl/thread.h"
#include "osl/process.h"

using namespace rtl;

sal_Bool CppuOptions::initOptions(int ac, char* av[], sal_Bool bCmdFile) 
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
    for( ; i < ac; i++)
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
                case 'B':
                    if (av[i][2] == '\0')
                    {
                        if (i < ac - 1 && av[i+1][0] != '-')
                        {
                            i++;
                            s = av[i];
                        } else
                        {
                            OString tmp("'-B', please check");
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
                    
                    m_options["-B"] = OString(s);
                    break;
                case 'T':
                    if (av[i][2] == '\0')
                    {
                        if (i < ac - 1 && av[i+1][0] != '-')
                        {
                            i++;
                            s = av[i];
                        } else
                        {
                            OString tmp("'-T', please check");
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
                    
                    if (m_options.count("-T") > 0)
                    {
                        OString tmp(m_options["-T"]);
                        tmp = tmp + ";" + s;
                        m_options["-T"] = tmp;
                    } else
                    {
                        m_options["-T"] = OString(s);
                    }
                    break;
                case 'L':
                    if (av[i][2] != '\0')
                    {
                        OString tmp("'-L', please check");
                        if (i <= ac - 1)
                        {
                            tmp += " your input '" + OString(av[i]) + "'";
                        }

                        throw IllegalArgument(tmp);
                    }
                    
                    if (isValid("-C") || isValid("-CS"))
                    {
                        OString tmp("'-L' could not be combined with '-C' or '-CS' option");
                        throw IllegalArgument(tmp);
                    }
                    m_options["-L"] = OString("");
                    break;
                case 'C':
                    if (av[i][2] == 'S')
                    {
                        if (av[i][3] != '\0')
                        {
                            OString tmp("'-CS', please check");
                            if (i <= ac - 1)
                            {
                                tmp += " your input '" + OString(av[i]) + "'";
                            }

                            throw IllegalArgument(tmp);
                        }

                        if (isValid("-L") || isValid("-C"))
                        {
                            OString tmp("'-CS' could not be combined with '-L' or '-C' option");
                            throw IllegalArgument(tmp);
                        }
                        m_options["-CS"] = OString("");
                        break;
                    } else
                    if (av[i][2] != '\0')
                    {
                        OString tmp("'-C', please check");
                        if (i <= ac - 1)
                        {
                            tmp += " your input '" + OString(av[i]) + "'";
                        }

                        throw IllegalArgument(tmp);
                    }
                    
                    if (isValid("-L") || isValid("-CS"))
                    {
                        OString tmp("'-C' could not be combined with '-L' or '-CS' option");
                        throw IllegalArgument(tmp);
                    }
                    m_options["-C"] = OString("");
                    break;
                case 'G':
                    if (av[i][2] == 'c')
                    {
                        if (av[i][3] != '\0')
                        {
                            OString tmp("'-Gc', please check");
                            if (i <= ac - 1)
                            {
                                tmp += " your input '" + OString(av[i]) + "'";
                            }

                            throw IllegalArgument(tmp);
                        }

                        m_options["-Gc"] = OString("");
                        break;
                    } else
                    if (av[i][2] != '\0')
                    {
                        OString tmp("'-G', please check");
                        if (i <= ac - 1)
                        {
                            tmp += " your input '" + OString(av[i]) + "'";
                        }

                        throw IllegalArgument(tmp);
                    }
                    
                    m_options["-G"] = OString("");
                    break;                    
                case 'X': // support for eXtra type rdbs
                {
                    if (av[i][2] == '\0')
                    {
                        if (i < ac - 1 && av[i+1][0] != '-')
                        {
                            i++;
                            s = av[i];
                        } else
                        {
                            OString tmp("'-X', please check");
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
                    
                    m_extra_input_files.push_back( s );
                    break;
                }
                
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
                    char  buffer[512];

                    while ( fscanf(cmdFile, "%s", buffer) != EOF )
                    {
                        rargv[rargc]= strdup(buffer);
                        rargc++;
                    }
                    fclose(cmdFile);
                    
                    ret = initOptions(rargc, rargv, bCmdFile);
                    
                    for (long i=0; i < rargc; i++) 
                    {
                        free(rargv[i]);
                    }
                }		
            } else
            {
                if (bCmdFile)
                {
                    m_inputFiles.push_back(av[i]);
                } else
                {
                    OUString system_filepath;
                    OSL_VERIFY( osl_Process_E_None == osl_getCommandArg( i-1, &system_filepath.pData ) );
                    m_inputFiles.push_back(OUStringToOString(system_filepath, osl_getThreadTextEncoding()));
                }
            }		
        }
    }
    
    return ret;	
}	

OString	CppuOptions::prepareHelp()
{
    OString help("\nusing: ");
    help += m_program + " [-options] file_1 ... file_n\nOptions:\n";
    help += "    -O<path>   = path describes the root directory for the generated output.\n";
    help += "                 The output directory tree is generated under this directory.\n";
    help += "    -T<name>   = name specifies a type or a list of types. The output for this\n";
    help += "      [t1;...]   type is generated. If no '-T' option is specified,\n";
    help += "                 then output for all types is generated.\n";
    help += "                 Example: 'com.sun.star.uno.XInterface' is a valid type.\n";		
    help += "    -B<name>   = name specifies the base node. All types are searched under this\n";
    help += "                 node. Default is the root '/' of the registry files.\n";
    help += "    -L         = UNO type functions are generated lightweight, that means only\n";
    help += "                 the name and typeclass are given and everything else is retrieved\n";
    help += "                 from the type library dynamically. The default is that UNO type\n";
    help += "                 functions provides enough type information for boostrapping C++.\n";
    help += "                 '-L' should be the default for external components.\n";
    help += "    -C         = UNO type functions are generated comprehensive that means all\n";
    help += "                 necessary information is available for bridging the type in UNO.\n";
    help += "    -G         = generate only target files which does not exists.\n";
    help += "    -Gc        = generate only target files which content will be changed.\n";
    help += "    -X<file>   = extra types which will not be taken into account for generation.\n";
    help += prepareVersion();
    
    return help;
}	

OString	CppuOptions::prepareVersion()
{
    OString version("\nSun Microsystems (R) ");
    version += m_program + " Version 2.0\n\n";

    return version;
}	

    
