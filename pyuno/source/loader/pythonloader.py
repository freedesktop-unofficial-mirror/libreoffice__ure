#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: pythonloader.py,v $
#
#   $Revision: 1.4 $
#
#   last change: $Author: rt $ $Date: 2005-09-08 16:50:23 $
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
import uno
import unohelper
import imp
import os
from com.sun.star.uno import Exception,RuntimeException
from com.sun.star.loader import XImplementationLoader
from com.sun.star.lang import XServiceInfo

MODULE_PROTOCOL = "vnd.openoffice.pymodule:"
DEBUG = 0

g_supportedServices  = "com.sun.star.loader.Python",      # referenced by the native C++ loader !
g_implementationName = "org.openoffice.comp.pyuno.Loader" # referenced by the native C++ loader !

def splitUrl( url ):
      nColon = url.find( ":" )
      if -1 == nColon:
            raise RuntimeException( "PythonLoader: No protocol in url " + url )
      return url[0:nColon], url[nColon+1:len(url)]

g_loadedComponents = {}

class Loader( XImplementationLoader, XServiceInfo, unohelper.Base ):
      def __init__(self, ctx ):
	  if DEBUG:
	     print "pythonloader.Loader ctor" 
	  self.ctx = ctx

      def getModuleFromUrl( self, url ):
          if DEBUG:
                print "pythonloader: interpreting url " +url
          protocol, dependent = splitUrl( url )
          if "vnd.sun.star.expand" == protocol:
                exp = self.ctx.getValueByName( "/singletons/com.sun.star.util.theMacroExpander" )
                url = exp.expandMacros(dependent)
                protocol,dependent = splitUrl( url )

          if DEBUG:
                print "pythonloader: after expansion " +protocol +":" + dependent
                
          try:
                if "file" == protocol:
                      # remove \..\ sequence, which may be useful e.g. in the build env
                      url = unohelper.absolutize( url, url )

                      # did we load the module already ?
                      mod = g_loadedComponents.get( url )
                      if not mod:
                            mod = imp.new_module("uno_component")

                            # read the file
                            fileHandle = file( unohelper.fileUrlToSystemPath( url ) )
                            src = fileHandle.read()

                            # execute the module
                            exec src in mod.__dict__
                            g_loadedComponents[url] = mod
                      return mod
                elif "vnd.openoffice.pymodule" == protocol:
                      return  __import__( dependent )
                else:
                      raise RuntimeException( "PythonLoader: Unknown protocol " +
                                              protocol + " in url " +url, self )
          except ImportError, e:
                raise RuntimeException( "Couldn't load "+url+ " for reason "+str(e), None)
          return None
	   
      def activate( self, implementationName, dummy, locationUrl, regKey ):
	  if DEBUG:
	     print "pythonloader.Loader.activate"

	  mod = self.getModuleFromUrl( locationUrl )
          implHelper = mod.__dict__.get( "g_ImplementationHelper" , None )
          if implHelper == None:
		return mod.getComponentFactory( implementationName, self.ctx.ServiceManager, regKey )
          else:
		return implHelper.getComponentFactory( implementationName,regKey,self.ctx.ServiceManager)
	     
      def writeRegistryInfo( self, regKey, dummy, locationUrl ):
	  if DEBUG:
	     print "pythonloader.Loader.writeRegistryInfo"
             
	  mod = self.getModuleFromUrl( locationUrl )
          implHelper = mod.__dict__.get( "g_ImplementationHelper" , None )
          if implHelper == None:
	        return mod.writeRegistryInfo( self.ctx.ServiceManager, regKey )
          else:
	        return implHelper.writeRegistryInfo( regKey, self.ctx.ServiceManager )

      def getImplementationName( self ):
	  return g_implementationName

      def supportsService( self, ServiceName ):
	  return ServiceName in self.serviceNames

      def getSupportedServiceNames( self ):
	  return g_supportedServices


