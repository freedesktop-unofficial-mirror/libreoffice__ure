/*************************************************************************
 *
 *  $RCSfile: FactoryHelper.java,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: dbo $ $Date: 2001-06-14 11:54:57 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

package com.sun.star.comp.loader;


import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;

import com.sun.star.uno.XComponentContext;
import com.sun.star.lang.XInitialization;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.lang.XServiceInfo;
import com.sun.star.lang.XSingleServiceFactory;
import com.sun.star.lang.XSingleComponentFactory;

import com.sun.star.registry.XRegistryKey;

import com.sun.star.uno.UnoRuntime;


/**
 * The purpose of this class to help component implementation.
 * This class has default implementations for <code>getServiceFactory</code>
 * and <code>writeRegistryServiceInfo</code>.
 * <p>
 * @version 	$Revision: 1.5 $ $ $Date: 2001-06-14 11:54:57 $
 * @author 	    Kay Ramme
 * @see         com.sun.star.lang.XMultiServiceFactory
 * @see         com.sun.star.lang.XServiceInfo
 * @see         com.sun.star.lang.XSingleServiceFactory
 * @see         com.sun.star.registry.XRegistryKey
 * @since       UDK1.0
 */
public class FactoryHelper {
    // the factory
    static protected class Factory
        implements XSingleServiceFactory, XSingleComponentFactory, XServiceInfo {
        protected static Class __objectArray;

        static {
            try {
                __objectArray = Class.forName("[Ljava.lang.Object;");
            }
            catch(ClassNotFoundException classNotFoundException) {
                System.err.println(FactoryHelper.class.getName() + " exception occurred - " + classNotFoundException);
            }
        }

        private static final boolean DEBUG = false;

        protected XMultiServiceFactory _xMultiServiceFactory;
        protected XRegistryKey		   _xRegistryKey;
        protected int                  _nCode;
        protected Constructor		   _constructor;
        protected String               _implName;
        protected String               _serviceName;

        protected Factory(Class implClass, 
                          String serviceName, 
                          XMultiServiceFactory xMultiServiceFactory, 
                          XRegistryKey xRegistryKey) 
        {
            _xMultiServiceFactory = xMultiServiceFactory;
            _xRegistryKey         = xRegistryKey;
            _implName             = implClass.getName();
            _serviceName          = serviceName;

            Constructor constructors[] = implClass.getConstructors();
            for(int i = 0; i < constructors.length && _constructor == null; ++i) {
                Class parameters[] = constructors[i].getParameterTypes();

                if(parameters.length == 3
                   && parameters[0].equals(XComponentContext.class)
                     && parameters[1].equals(XRegistryKey.class)
                     && parameters[2].equals(__objectArray)) {
                    _nCode = 0;
                    _constructor = constructors[i];
                } 
                else if(parameters.length == 2
                     && parameters[0].equals(XComponentContext.class)
                     && parameters[1].equals(XRegistryKey.class)) {
                    _nCode = 1;
                    _constructor = constructors[i];
                }
                else if(parameters.length == 2
                     && parameters[0].equals(XComponentContext.class)
                     && parameters[1].equals(__objectArray)) {
                    _nCode = 2;
                    _constructor = constructors[i];
                }
                else if(parameters.length == 1 
                     && parameters[0].equals(XComponentContext.class)) {
                    _nCode = 3;
                    _constructor = constructors[i];
                }
                // depr
                else if(parameters.length == 3
                     && parameters[0].equals(XMultiServiceFactory.class)
                     && parameters[1].equals(XRegistryKey.class)
                     && parameters[2].equals(__objectArray)) {
                    _nCode = 4;
                    _constructor = constructors[i];
                } 
                else if(parameters.length == 2
                     && parameters[0].equals(XMultiServiceFactory.class)
                     && parameters[1].equals(XRegistryKey.class)) {
                    _nCode = 5;
                    _constructor = constructors[i];
                }
                else if(parameters.length == 2
                     && parameters[0].equals(XMultiServiceFactory.class)
                     && parameters[1].equals(__objectArray)) {
                    _nCode = 6;
                    _constructor = constructors[i];
                }
                else if(parameters.length == 1 
                     && parameters[0].equals(XMultiServiceFactory.class)) {
                    _nCode = 7;
                    _constructor = constructors[i];
                }
                else if(parameters.length == 1
                     && parameters[0].equals(__objectArray)) {
                    _nCode = 8;
                    _constructor = constructors[i];
                }
                else if(parameters.length == 0) {
                    _nCode = 9;
                    _constructor = constructors[i];
                }
            }

            if(_constructor == null) // have not found a useable constructor
                throw new com.sun.star.uno.RuntimeException(getClass().getName() + " can not find a useable constructor");
        }

        private final XMultiServiceFactory getSMgr( XComponentContext xContext )
        {
            if (xContext != null)
            {
                return (XMultiServiceFactory)UnoRuntime.queryInterface(
                    XMultiServiceFactory.class, xContext.getServiceManager() );
            }
            else
            {
                return _xMultiServiceFactory;
            }
        }
        
        // XComponentContext impl
        //______________________________________________________________________________________________
        public Object createInstanceWithContext(
            XComponentContext xContext )
            throws com.sun.star.uno.Exception
        {
            Object args[];
            switch (_nCode)
            {
            case 0:
                args = new Object [] { xContext, _xRegistryKey, new Object[ 0 ] };
                break;
            case 1:
                args = new Object [] { xContext, _xRegistryKey };
                break;
            case 2:
                args = new Object [] { xContext, new Object[ 0 ] };
                break;
            case 3:
                args = new Object [] { xContext };
                break;
            case 4:
                args = new Object [] { getSMgr( xContext ), _xRegistryKey, new Object[ 0 ] };
                break;
            case 5:
                args = new Object [] { getSMgr( xContext ), _xRegistryKey };
                break;
            case 6:
                args = new Object [] { getSMgr( xContext ), new Object[ 0 ] };
                break;
            case 7:
                args = new Object [] { getSMgr( xContext ) };
                break;
            case 8:
                args = new Object [] { new Object[ 0 ] };
                break;
            default:
                args = new Object [ 0 ];
                break;
            }
            
            try
            {
                return _constructor.newInstance( args );
            }
            catch (InvocationTargetException invocationTargetException)
            {
                Throwable targetException = invocationTargetException.getTargetException();
                
                if (targetException instanceof java.lang.RuntimeException)
                    throw (java.lang.RuntimeException)targetException;
                else if (targetException instanceof com.sun.star.uno.Exception)
                    throw (com.sun.star.uno.Exception)targetException;
                else if (targetException instanceof com.sun.star.uno.RuntimeException)
                    throw (com.sun.star.uno.RuntimeException)targetException;
                else
                    throw new com.sun.star.uno.Exception( targetException.toString() );
            }
            catch (IllegalAccessException illegalAccessException)
            {
                throw new com.sun.star.uno.Exception( illegalAccessException.toString() );
            }
            catch (InstantiationException instantiationException)
            {
                throw new com.sun.star.uno.Exception( instantiationException.toString() );
            }
        }
        //______________________________________________________________________________________________
        public Object createInstanceWithArgumentsAndContext(
            Object rArguments[], XComponentContext xContext )
            throws com.sun.star.uno.Exception
        {
            Object args[];

            boolean bInitCall = true;
            switch (_nCode)
            {
            case 0:
                args = new Object [] { xContext, _xRegistryKey, rArguments };
                bInitCall = false;
                break;
            case 1:
                args = new Object [] { xContext, _xRegistryKey };
                break;
            case 2:
                args = new Object [] { xContext, rArguments };
                bInitCall = false;
                break;
            case 3:
                args = new Object [] { xContext };
                break;
            case 4:
                args = new Object [] { getSMgr( xContext ), _xRegistryKey, rArguments };
                bInitCall = false;
                break;
            case 5:
                args = new Object [] { getSMgr( xContext ), _xRegistryKey };
                break;
            case 6:
                args = new Object [] { getSMgr( xContext ), rArguments };
                bInitCall = false;
                break;
            case 7:
                args = new Object [] { getSMgr( xContext ) };
                break;
            case 8:
                args = new Object [] { rArguments };
                bInitCall = false;
                break;
            default:
                args = new Object [ 0 ];
                break;
            }
            
            try
            {
                Object instance = _constructor.newInstance( args );
                if (bInitCall)
                {
                    XInitialization xInitialization = (XInitialization)UnoRuntime.queryInterface(
                        XInitialization.class, instance );
                    if (xInitialization != null)
                    {
                        xInitialization.initialize( rArguments );
                    }
                }
                return instance;
            }
            catch (InvocationTargetException invocationTargetException)
            {
                Throwable targetException = invocationTargetException.getTargetException();
                
                if (targetException instanceof java.lang.RuntimeException)
                    throw (java.lang.RuntimeException)targetException;
                else if (targetException instanceof com.sun.star.uno.Exception)
                    throw (com.sun.star.uno.Exception)targetException;
                else if (targetException instanceof com.sun.star.uno.RuntimeException)
                    throw (com.sun.star.uno.RuntimeException)targetException;
                else
                    throw new com.sun.star.uno.Exception( targetException.toString() );
            }
            catch (IllegalAccessException illegalAccessException)
            {
                throw new com.sun.star.uno.Exception( illegalAccessException.toString() );
            }
            catch (InstantiationException instantiationException)
            {
                throw new com.sun.star.uno.Exception( instantiationException.toString() );
            }
        }
        
        /**
         * Creates an instance of the desired service.
         * <p>
         * @return  returns an instance of the desired service
         * @see                  com.sun.star.lang.XSingleServiceFactory
         */
        public Object createInstance() 
            throws com.sun.star.uno.Exception, 
                   com.sun.star.uno.RuntimeException
        {
            return createInstanceWithContext( null );
        }

         /**
         * Creates an instance of the desired service.
         * <p>
         * @return  returns an instance of the desired service
         * @param   args     the args given to the constructor of the service   
         * @see              com.sun.star.lang.XSingleServiceFactory
         */
        public Object createInstanceWithArguments(Object[] args) 
            throws com.sun.star.uno.Exception, 
                   com.sun.star.uno.RuntimeException 
        {
            return createInstanceWithArgumentsAndContext( args, null );
        }
        
         /**
         * Gives the supported services
         * <p>
         * @return  returns an array of supported services
         * @see             com.sun.star.lang.XServiceInfo
         */
        public String[] getSupportedServiceNames() throws com.sun.star.uno.RuntimeException {
            return new String[]{_serviceName};
        }

         /**
         * Gives the implementation name
         * <p>
         * @return  returns the implementation name
         * @see             com.sun.star.lang.XServiceInfo
         */
        public String getImplementationName() throws com.sun.star.uno.RuntimeException {
            return _implName;
        }

         /**
         * Indicates if the given service is supported.
         * <p>
         * @return  returns true if the given service is supported
         * @see              com.sun.star.lang.XServiceInfo
         */
        public boolean supportsService(String serviceName) throws com.sun.star.uno.RuntimeException {
            String services[] = getSupportedServiceNames();

            boolean found = false;

            for(int i = 0; i < services.length && !found; ++i)
                found = services[i].equals(serviceName);

            return found;
        }

    }

    /**
     * Creates a factory for the given class.
     * <p>
     * @deprecated as of UDK 1.0
     * <p>
     * @return  returns a factory
     * @param   implClass     the implementing class 
     * @param   multiFactory  the given multi service factory (service manager)
     * @param   regKey        the given registry key
     * @see              com.sun.star.lang.XServiceInfo
     */
    static public XSingleServiceFactory getServiceFactory(Class implClass, 
                                                          XMultiServiceFactory multiFactory, 
                                                          XRegistryKey regKey)
    {
        XSingleServiceFactory xSingleServiceFactory = null;

        try {
            Field serviceName = null;

            try {
                serviceName = implClass.getField("__serviceName");
            }
            catch(NoSuchFieldException noSuchFieldExceptio) {
                serviceName = implClass.getField("serviceName");  // old style
            }
            
            xSingleServiceFactory =  new Factory(implClass, (String)serviceName.get(null), multiFactory, regKey);
        }
        catch(NoSuchFieldException noSuchFieldException) {
            System.err.println("##### FactoryHelper.getServiceFactory - exception:" + noSuchFieldException);
        }
        catch(IllegalAccessException illegalAccessException) {
            System.err.println("##### FactoryHelper.getServiceFactory - exception:" + illegalAccessException);
        }
        
        return xSingleServiceFactory;
    }
    
    /**
     * Creates a factory for the given class.
     * <p>
     * @return  returns a factory
     * @param   implClass     the implementing class 
     * @param   serviceName   the service name of the implementing class
     * @param   multiFactory  the given multi service factory (service manager)
     * @param   regKey        the given registry key
     * @see              com.sun.star.lang.XServiceInfo
     */
    static public XSingleServiceFactory getServiceFactory(Class implClass, 
                                                          String serviceName,
                                                          XMultiServiceFactory multiFactory, 
                                                          XRegistryKey regKey)
    {
        return new Factory(implClass, serviceName, multiFactory, regKey);
    }
    
    /** Creates a factory for the given class.
        
        @return returns a factory object
        @param   implClass     the implementing class 
    */
    static public Object createComponentFactory( Class implClass, String serviceName )
    {
        return new Factory( implClass, serviceName, null, null );
    }
    
    /**
     * Writes the registration data into the registry key
     * <p>
     * @return  returns a factory
     * @param   implName      the name of the implementing class 
     * @param   serviceName   the service name
     * @param   regKey        the given registry key
     * @see                    com.sun.star.lang.XServiceInfo
     */
    static public boolean writeRegistryServiceInfo(String implName, String serviceName, XRegistryKey regKey) {
        boolean result = false;
        
          try {
            XRegistryKey newKey = regKey.createKey("/" + implName + "/UNO/SERVICES");
           
            newKey.createKey(serviceName);
            
            result = true;
          }
          catch (Exception ex) {
              System.err.println(">>>Connection_Impl.writeRegistryServiceInfo " + ex);
          }
        
        return result;
    }
}

