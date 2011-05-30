#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2000, 2010 Oracle and/or its affiliates.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# This file is part of OpenOffice.org.
#
# OpenOffice.org is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3
# only, as published by the Free Software Foundation.
#
# OpenOffice.org is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU Lesser General Public License version 3 for more details
# (a copy is included in the LICENSE file that accompanied this code).
#
# You should have received a copy of the GNU Lesser General Public License
# version 3 along with OpenOffice.org.	If not, see
# <http://www.openoffice.org/license.html>
# for a copy of the LGPLv3 License.
#
#*************************************************************************

$(eval $(call gb_UnoApiTarget_UnoApiTarget,udkapi))

#$(eval $(call gb_UnoApiTarget_set_defs,udkapi,\
#	$$(DEFS) \
#	-DTOOLS_DLLIMPLEMENTATION \
#	-DVCL \
#))

$(eval $(call gb_UnoApiTarget_set_include,udkapi,\
	$$(INCLUDE) \
	-I$(SRCDIR)/udkapi \
	-I$(OUTDIR)/idl \
))

$(eval $(call gb_UnoApiTarget_add_idlfiles_noheader,udkapi,\
	udkapi/com/sun/star/beans/Introspection \
	udkapi/com/sun/star/beans/PropertyBag \
	udkapi/com/sun/star/beans/PropertySet \
	udkapi/com/sun/star/bridge/Bridge \
	udkapi/com/sun/star/bridge/BridgeFactory \
	udkapi/com/sun/star/bridge/IiopBridge \
	udkapi/com/sun/star/bridge/OleApplicationRegistration \
	udkapi/com/sun/star/bridge/OleBridgeSupplier \
	udkapi/com/sun/star/bridge/OleBridgeSupplier2 \
	udkapi/com/sun/star/bridge/OleBridgeSupplierVar1 \
	udkapi/com/sun/star/bridge/OleObjectFactory \
	udkapi/com/sun/star/bridge/UrpBridge \
	udkapi/com/sun/star/bridge/oleautomation/ApplicationRegistration \
	udkapi/com/sun/star/bridge/oleautomation/BridgeSupplier \
	udkapi/com/sun/star/bridge/oleautomation/Factory \
	udkapi/com/sun/star/corba/corba \
	udkapi/com/sun/star/corba/giop/giop \
	udkapi/com/sun/star/corba/iiop/iiop \
	udkapi/com/sun/star/corba/iop/iop \
	udkapi/com/sun/star/io/DataInputStream \
	udkapi/com/sun/star/io/DataOutputStream \
	udkapi/com/sun/star/io/MarkableInputStream \
	udkapi/com/sun/star/io/MarkableOutputStream \
	udkapi/com/sun/star/io/ObjectInputStream \
	udkapi/com/sun/star/io/ObjectOutputStream \
	udkapi/com/sun/star/io/Pipe \
	udkapi/com/sun/star/io/Pump \
	udkapi/com/sun/star/io/TextInputStream \
	udkapi/com/sun/star/io/TextOutputStream \
	udkapi/com/sun/star/java/JavaVirtualMachine \
	udkapi/com/sun/star/lang/MultiServiceFactory \
	udkapi/com/sun/star/lang/RegistryServiceManager \
	udkapi/com/sun/star/lang/ServiceManager \
	udkapi/com/sun/star/loader/Dynamic \
	udkapi/com/sun/star/loader/Java \
	udkapi/com/sun/star/loader/Java2 \
	udkapi/com/sun/star/loader/SharedLibrary \
	udkapi/com/sun/star/reflection/CoreReflection \
	udkapi/com/sun/star/reflection/ProxyFactory \
	udkapi/com/sun/star/reflection/TypeDescriptionManager \
	udkapi/com/sun/star/reflection/TypeDescriptionProvider \
	udkapi/com/sun/star/registry/DefaultRegistry \
	udkapi/com/sun/star/registry/ImplementationRegistration \
	udkapi/com/sun/star/registry/NestedRegistry \
	udkapi/com/sun/star/registry/SimpleRegistry \
	udkapi/com/sun/star/script/AllListenerAdapter \
	udkapi/com/sun/star/script/Converter \
	udkapi/com/sun/star/script/Engine \
	udkapi/com/sun/star/script/Invocation \
	udkapi/com/sun/star/script/InvocationAdapterFactory \
	udkapi/com/sun/star/script/JavaScript \
	udkapi/com/sun/star/security/AccessController \
	udkapi/com/sun/star/security/Policy \
	udkapi/com/sun/star/test/TestFactory \
	udkapi/com/sun/star/udk-modules \
	udkapi/com/sun/star/uno/NamingService \
	udkapi/com/sun/star/util/BootstrapMacroExpander \
	udkapi/com/sun/star/util/MacroExpander \
	udkapi/com/sun/star/util/logging/Logger \
	udkapi/com/sun/star/util/logging/LoggerRemote \
	udkapi/com/sun/star/util/theMacroExpander \
))

$(eval $(call gb_UnoApiTarget_add_idlfiles_nohdl,udkapi,\
	udkapi/com/sun/star/bridge/UnoUrlResolver \
	udkapi/com/sun/star/connection/Acceptor \
	udkapi/com/sun/star/connection/Connector \
	udkapi/com/sun/star/container/EnumerableMap \
	udkapi/com/sun/star/io/SequenceInputStream \
	udkapi/com/sun/star/io/SequenceOutputStream \
	udkapi/com/sun/star/io/TempFile \
	udkapi/com/sun/star/uri/ExternalUriReferenceTranslator \
	udkapi/com/sun/star/uri/UriReferenceFactory \
	udkapi/com/sun/star/uri/UriSchemeParser_vndDOTsunDOTstarDOTexpand \
	udkapi/com/sun/star/uri/UriSchemeParser_vndDOTsunDOTstarDOTscript \
	udkapi/com/sun/star/uri/VndSunStarPkgUrlReferenceFactory \
))

$(eval $(call gb_UnoApiTarget_add_idlfiles,udkapi,\
	udkapi/com/sun/star/beans/Ambiguous \
	udkapi/com/sun/star/beans/Defaulted \
	udkapi/com/sun/star/beans/GetDirectPropertyTolerantResult \
	udkapi/com/sun/star/beans/GetPropertyTolerantResult \
	udkapi/com/sun/star/beans/IllegalTypeException \
	udkapi/com/sun/star/beans/IntrospectionException \
	udkapi/com/sun/star/beans/MethodConcept \
	udkapi/com/sun/star/beans/NamedValue \
	udkapi/com/sun/star/beans/NotRemoveableException \
	udkapi/com/sun/star/beans/Optional \
	udkapi/com/sun/star/beans/Pair \
	udkapi/com/sun/star/beans/Property \
	udkapi/com/sun/star/beans/PropertyAttribute \
	udkapi/com/sun/star/beans/PropertyChangeEvent \
	udkapi/com/sun/star/beans/PropertyConcept \
	udkapi/com/sun/star/beans/PropertyExistException \
	udkapi/com/sun/star/beans/PropertySetInfoChange \
	udkapi/com/sun/star/beans/PropertySetInfoChangeEvent \
	udkapi/com/sun/star/beans/PropertyState \
	udkapi/com/sun/star/beans/PropertyStateChangeEvent \
	udkapi/com/sun/star/beans/PropertyValue \
	udkapi/com/sun/star/beans/PropertyValues \
	udkapi/com/sun/star/beans/PropertyVetoException \
	udkapi/com/sun/star/beans/SetPropertyTolerantFailed \
	udkapi/com/sun/star/beans/StringPair \
	udkapi/com/sun/star/beans/TolerantPropertySetResultType \
	udkapi/com/sun/star/beans/UnknownPropertyException \
	udkapi/com/sun/star/beans/XExactName \
	udkapi/com/sun/star/beans/XFastPropertySet \
	udkapi/com/sun/star/beans/XHierarchicalPropertySet \
	udkapi/com/sun/star/beans/XHierarchicalPropertySetInfo \
	udkapi/com/sun/star/beans/XIntroTest \
	udkapi/com/sun/star/beans/XIntrospection \
	udkapi/com/sun/star/beans/XIntrospectionAccess \
	udkapi/com/sun/star/beans/XMaterialHolder \
	udkapi/com/sun/star/beans/XMultiHierarchicalPropertySet \
	udkapi/com/sun/star/beans/XMultiPropertySet \
	udkapi/com/sun/star/beans/XMultiPropertyStates \
	udkapi/com/sun/star/beans/XPropertiesChangeListener \
	udkapi/com/sun/star/beans/XPropertiesChangeNotifier \
	udkapi/com/sun/star/beans/XProperty \
	udkapi/com/sun/star/beans/XPropertyAccess \
	udkapi/com/sun/star/beans/XPropertyChangeListener \
	udkapi/com/sun/star/beans/XPropertyContainer \
	udkapi/com/sun/star/beans/XPropertySet \
	udkapi/com/sun/star/beans/XPropertySetInfo \
	udkapi/com/sun/star/beans/XPropertySetInfoChangeListener \
	udkapi/com/sun/star/beans/XPropertySetInfoChangeNotifier \
	udkapi/com/sun/star/beans/XPropertyState \
	udkapi/com/sun/star/beans/XPropertyStateChangeListener \
	udkapi/com/sun/star/beans/XPropertyWithState \
	udkapi/com/sun/star/beans/XTolerantMultiPropertySet \
	udkapi/com/sun/star/beans/XVetoableChangeListener \
	udkapi/com/sun/star/bridge/BridgeExistsException \
	udkapi/com/sun/star/bridge/InvalidProtocolChangeException \
	udkapi/com/sun/star/bridge/ModelDependent \
	udkapi/com/sun/star/bridge/ProtocolProperty \
	udkapi/com/sun/star/bridge/XBridge \
	udkapi/com/sun/star/bridge/XBridgeFactory \
	udkapi/com/sun/star/bridge/XBridgeSupplier \
	udkapi/com/sun/star/bridge/XBridgeSupplier2 \
	udkapi/com/sun/star/bridge/XInstanceProvider \
	udkapi/com/sun/star/bridge/XProtocolProperties \
	udkapi/com/sun/star/bridge/XUnoUrlResolver \
	udkapi/com/sun/star/bridge/oleautomation/Currency \
	udkapi/com/sun/star/bridge/oleautomation/Date \
	udkapi/com/sun/star/bridge/oleautomation/Decimal \
	udkapi/com/sun/star/bridge/oleautomation/NamedArgument \
	udkapi/com/sun/star/bridge/oleautomation/PropertyPutArgument \
	udkapi/com/sun/star/bridge/oleautomation/SCode \
	udkapi/com/sun/star/bridge/oleautomation/XAutomationObject \
	udkapi/com/sun/star/connection/AlreadyAcceptingException \
	udkapi/com/sun/star/connection/ConnectionSetupException \
	udkapi/com/sun/star/connection/NoConnectException \
	udkapi/com/sun/star/connection/SocketPermission \
	udkapi/com/sun/star/connection/XAcceptor \
	udkapi/com/sun/star/connection/XConnection \
	udkapi/com/sun/star/connection/XConnection2 \
	udkapi/com/sun/star/connection/XConnectionBroadcaster \
	udkapi/com/sun/star/connection/XConnector \
	udkapi/com/sun/star/container/ContainerEvent \
	udkapi/com/sun/star/container/ElementExistException \
	udkapi/com/sun/star/container/NoSuchElementException \
	udkapi/com/sun/star/container/XChild \
	udkapi/com/sun/star/container/XComponentEnumeration \
	udkapi/com/sun/star/container/XComponentEnumerationAccess \
	udkapi/com/sun/star/container/XContainer \
	udkapi/com/sun/star/container/XContainerApproveBroadcaster \
	udkapi/com/sun/star/container/XContainerApproveListener \
	udkapi/com/sun/star/container/XContainerListener \
	udkapi/com/sun/star/container/XContainerQuery \
	udkapi/com/sun/star/container/XContentEnumerationAccess \
	udkapi/com/sun/star/container/XElementAccess \
	udkapi/com/sun/star/container/XEnumerableMap \
	udkapi/com/sun/star/container/XEnumeration \
	udkapi/com/sun/star/container/XEnumerationAccess \
	udkapi/com/sun/star/container/XHierarchicalName \
	udkapi/com/sun/star/container/XHierarchicalNameAccess \
	udkapi/com/sun/star/container/XHierarchicalNameContainer \
	udkapi/com/sun/star/container/XHierarchicalNameReplace \
	udkapi/com/sun/star/container/XIdentifierAccess \
	udkapi/com/sun/star/container/XIdentifierContainer \
	udkapi/com/sun/star/container/XIdentifierReplace \
	udkapi/com/sun/star/container/XImplicitIDAccess \
	udkapi/com/sun/star/container/XImplicitIDContainer \
	udkapi/com/sun/star/container/XImplicitIDReplace \
	udkapi/com/sun/star/container/XIndexAccess \
	udkapi/com/sun/star/container/XIndexContainer \
	udkapi/com/sun/star/container/XIndexReplace \
	udkapi/com/sun/star/container/XMap \
	udkapi/com/sun/star/container/XNameAccess \
	udkapi/com/sun/star/container/XNameContainer \
	udkapi/com/sun/star/container/XNameReplace \
	udkapi/com/sun/star/container/XNamed \
	udkapi/com/sun/star/container/XSet \
	udkapi/com/sun/star/container/XStringKeyMap \
	udkapi/com/sun/star/container/XUniqueIDAccess \
	udkapi/com/sun/star/io/AlreadyConnectedException \
	udkapi/com/sun/star/io/BufferSizeExceededException \
	udkapi/com/sun/star/io/ConnectException \
	udkapi/com/sun/star/io/DataTransferEvent \
	udkapi/com/sun/star/io/FilePermission \
	udkapi/com/sun/star/io/IOException \
	udkapi/com/sun/star/io/NoRouteToHostException \
	udkapi/com/sun/star/io/NotConnectedException \
	udkapi/com/sun/star/io/SocketException \
	udkapi/com/sun/star/io/UnexpectedEOFException \
	udkapi/com/sun/star/io/UnknownHostException \
	udkapi/com/sun/star/io/WrongFormatException \
	udkapi/com/sun/star/io/XActiveDataControl \
	udkapi/com/sun/star/io/XActiveDataSink \
	udkapi/com/sun/star/io/XActiveDataSource \
	udkapi/com/sun/star/io/XActiveDataStreamer \
	udkapi/com/sun/star/io/XAsyncOutputMonitor \
	udkapi/com/sun/star/io/XConnectable \
	udkapi/com/sun/star/io/XDataExporter \
	udkapi/com/sun/star/io/XDataImporter \
	udkapi/com/sun/star/io/XDataInputStream \
	udkapi/com/sun/star/io/XDataOutputStream \
	udkapi/com/sun/star/io/XDataTransferEventListener \
	udkapi/com/sun/star/io/XInputStream \
	udkapi/com/sun/star/io/XInputStreamProvider \
	udkapi/com/sun/star/io/XMarkableStream \
	udkapi/com/sun/star/io/XObjectInputStream \
	udkapi/com/sun/star/io/XObjectOutputStream \
	udkapi/com/sun/star/io/XOutputStream \
	udkapi/com/sun/star/io/XPersist \
	udkapi/com/sun/star/io/XPersistObject \
	udkapi/com/sun/star/io/XSeekable \
	udkapi/com/sun/star/io/XSeekableInputStream \
	udkapi/com/sun/star/io/XSequenceOutputStream \
	udkapi/com/sun/star/io/XStream \
	udkapi/com/sun/star/io/XStreamListener \
	udkapi/com/sun/star/io/XTempFile \
	udkapi/com/sun/star/io/XTextInputStream \
	udkapi/com/sun/star/io/XTextOutputStream \
	udkapi/com/sun/star/io/XTruncate \
	udkapi/com/sun/star/io/XXMLExtractor \
	udkapi/com/sun/star/java/InvalidJavaSettingsException \
	udkapi/com/sun/star/java/JavaDisabledException \
	udkapi/com/sun/star/java/JavaInitializationException \
	udkapi/com/sun/star/java/JavaNotConfiguredException \
	udkapi/com/sun/star/java/JavaNotFoundException \
	udkapi/com/sun/star/java/JavaVMCreationFailureException \
	udkapi/com/sun/star/java/MissingJavaRuntimeException \
	udkapi/com/sun/star/java/RestartRequiredException \
	udkapi/com/sun/star/java/WrongJavaVersionException \
	udkapi/com/sun/star/java/XJavaThreadRegister_11 \
	udkapi/com/sun/star/java/XJavaVM \
	udkapi/com/sun/star/lang/ArrayIndexOutOfBoundsException \
	udkapi/com/sun/star/lang/ClassNotFoundException \
	udkapi/com/sun/star/lang/DisposedException \
	udkapi/com/sun/star/lang/EventObject \
	udkapi/com/sun/star/lang/IllegalAccessException \
	udkapi/com/sun/star/lang/IllegalArgumentException \
	udkapi/com/sun/star/lang/IndexOutOfBoundsException \
	udkapi/com/sun/star/lang/InvalidListenerException \
	udkapi/com/sun/star/lang/ListenerExistException \
	udkapi/com/sun/star/lang/Locale \
	udkapi/com/sun/star/lang/NoSuchFieldException \
	udkapi/com/sun/star/lang/NoSuchMethodException \
	udkapi/com/sun/star/lang/NoSupportException \
	udkapi/com/sun/star/lang/NotInitializedException \
	udkapi/com/sun/star/lang/NullPointerException \
	udkapi/com/sun/star/lang/ServiceNotRegisteredException \
	udkapi/com/sun/star/lang/SystemDependent \
	udkapi/com/sun/star/lang/WrappedTargetException \
	udkapi/com/sun/star/lang/WrappedTargetRuntimeException \
	udkapi/com/sun/star/lang/XComponent \
	udkapi/com/sun/star/lang/XConnectionPoint \
	udkapi/com/sun/star/lang/XConnectionPointContainer \
	udkapi/com/sun/star/lang/XEventListener \
	udkapi/com/sun/star/lang/XInitialization \
	udkapi/com/sun/star/lang/XLocalizable \
	udkapi/com/sun/star/lang/XMain \
	udkapi/com/sun/star/lang/XMultiComponentFactory \
	udkapi/com/sun/star/lang/XMultiServiceFactory \
	udkapi/com/sun/star/lang/XServiceDisplayName \
	udkapi/com/sun/star/lang/XServiceInfo \
	udkapi/com/sun/star/lang/XServiceName \
	udkapi/com/sun/star/lang/XSingleComponentFactory \
	udkapi/com/sun/star/lang/XSingleServiceFactory \
	udkapi/com/sun/star/lang/XTypeProvider \
	udkapi/com/sun/star/lang/XUnoTunnel \
	udkapi/com/sun/star/loader/CannotActivateFactoryException \
	udkapi/com/sun/star/loader/XImplementationLoader \
	udkapi/com/sun/star/reflection/FieldAccessMode \
	udkapi/com/sun/star/reflection/InvalidTypeNameException \
	udkapi/com/sun/star/reflection/InvocationTargetException \
	udkapi/com/sun/star/reflection/MethodMode \
	udkapi/com/sun/star/reflection/NoSuchTypeNameException \
	udkapi/com/sun/star/reflection/ParamInfo \
	udkapi/com/sun/star/reflection/ParamMode \
	udkapi/com/sun/star/reflection/TypeDescriptionSearchDepth \
	udkapi/com/sun/star/reflection/XArrayTypeDescription \
	udkapi/com/sun/star/reflection/XCompoundTypeDescription \
	udkapi/com/sun/star/reflection/XConstantTypeDescription \
	udkapi/com/sun/star/reflection/XConstantsTypeDescription \
	udkapi/com/sun/star/reflection/XEnumTypeDescription \
	udkapi/com/sun/star/reflection/XIdlArray \
	udkapi/com/sun/star/reflection/XIdlClass \
	udkapi/com/sun/star/reflection/XIdlClassProvider \
	udkapi/com/sun/star/reflection/XIdlField \
	udkapi/com/sun/star/reflection/XIdlField2 \
	udkapi/com/sun/star/reflection/XIdlMember \
	udkapi/com/sun/star/reflection/XIdlMethod \
	udkapi/com/sun/star/reflection/XIdlReflection \
	udkapi/com/sun/star/reflection/XIndirectTypeDescription \
	udkapi/com/sun/star/reflection/XInterfaceAttributeTypeDescription \
	udkapi/com/sun/star/reflection/XInterfaceAttributeTypeDescription2 \
	udkapi/com/sun/star/reflection/XInterfaceMemberTypeDescription \
	udkapi/com/sun/star/reflection/XInterfaceMethodTypeDescription \
	udkapi/com/sun/star/reflection/XInterfaceTypeDescription \
	udkapi/com/sun/star/reflection/XInterfaceTypeDescription2 \
	udkapi/com/sun/star/reflection/XMethodParameter \
	udkapi/com/sun/star/reflection/XModuleTypeDescription \
	udkapi/com/sun/star/reflection/XParameter \
	udkapi/com/sun/star/reflection/XPropertyTypeDescription \
	udkapi/com/sun/star/reflection/XProxyFactory \
	udkapi/com/sun/star/reflection/XPublished \
	udkapi/com/sun/star/reflection/XServiceConstructorDescription \
	udkapi/com/sun/star/reflection/XServiceTypeDescription \
	udkapi/com/sun/star/reflection/XServiceTypeDescription2 \
	udkapi/com/sun/star/reflection/XSingletonTypeDescription \
	udkapi/com/sun/star/reflection/XSingletonTypeDescription2 \
	udkapi/com/sun/star/reflection/XStructTypeDescription \
	udkapi/com/sun/star/reflection/XTypeDescription \
	udkapi/com/sun/star/reflection/XTypeDescriptionEnumeration \
	udkapi/com/sun/star/reflection/XTypeDescriptionEnumerationAccess \
	udkapi/com/sun/star/reflection/XUnionTypeDescription \
	udkapi/com/sun/star/registry/CannotRegisterImplementationException \
	udkapi/com/sun/star/registry/InvalidRegistryException \
	udkapi/com/sun/star/registry/InvalidValueException \
	udkapi/com/sun/star/registry/MergeConflictException \
	udkapi/com/sun/star/registry/RegistryKeyType \
	udkapi/com/sun/star/registry/RegistryValueType \
	udkapi/com/sun/star/registry/XImplementationRegistration \
	udkapi/com/sun/star/registry/XImplementationRegistration2 \
	udkapi/com/sun/star/registry/XRegistryKey \
	udkapi/com/sun/star/registry/XSimpleRegistry \
	udkapi/com/sun/star/script/AllEventObject \
	udkapi/com/sun/star/script/ArrayWrapper \
	udkapi/com/sun/star/script/BasicErrorException \
	udkapi/com/sun/star/script/CannotConvertException \
	udkapi/com/sun/star/script/CannotCreateAdapterException \
	udkapi/com/sun/star/script/ContextInformation \
	udkapi/com/sun/star/script/FailReason \
	udkapi/com/sun/star/script/FinishEngineEvent \
	udkapi/com/sun/star/script/FinishReason \
	udkapi/com/sun/star/script/InterruptEngineEvent \
	udkapi/com/sun/star/script/InterruptReason \
	udkapi/com/sun/star/script/InvocationInfo \
	udkapi/com/sun/star/script/MemberType \
	udkapi/com/sun/star/script/ScriptEvent \
	udkapi/com/sun/star/script/ScriptEventDescriptor \
	udkapi/com/sun/star/script/XAllListener \
	udkapi/com/sun/star/script/XAllListenerAdapterService \
	udkapi/com/sun/star/script/XDebugging \
	udkapi/com/sun/star/script/XDefaultMethod \
	udkapi/com/sun/star/script/XDefaultProperty \
	udkapi/com/sun/star/script/XDirectInvocation \
	udkapi/com/sun/star/script/XEngine \
	udkapi/com/sun/star/script/XEngineListener \
	udkapi/com/sun/star/script/XEventAttacher \
	udkapi/com/sun/star/script/XEventAttacherManager \
	udkapi/com/sun/star/script/XInvocation \
	udkapi/com/sun/star/script/XInvocation2 \
	udkapi/com/sun/star/script/XInvocationAdapterFactory \
	udkapi/com/sun/star/script/XInvocationAdapterFactory2 \
	udkapi/com/sun/star/script/XLibraryAccess \
	udkapi/com/sun/star/script/XScriptEventsAttacher \
	udkapi/com/sun/star/script/XScriptEventsSupplier \
	udkapi/com/sun/star/script/XScriptListener \
	udkapi/com/sun/star/script/XStarBasicAccess \
	udkapi/com/sun/star/script/XStarBasicDialogInfo \
	udkapi/com/sun/star/script/XStarBasicLibraryInfo \
	udkapi/com/sun/star/script/XStarBasicModuleInfo \
	udkapi/com/sun/star/script/XTypeConverter \
	udkapi/com/sun/star/security/AccessControlException \
	udkapi/com/sun/star/security/AllPermission \
	udkapi/com/sun/star/security/RuntimePermission \
	udkapi/com/sun/star/security/XAccessControlContext \
	udkapi/com/sun/star/security/XAccessController \
	udkapi/com/sun/star/security/XAction \
	udkapi/com/sun/star/security/XPolicy \
	udkapi/com/sun/star/task/XInteractionAbort \
	udkapi/com/sun/star/task/XInteractionContinuation \
	udkapi/com/sun/star/task/XInteractionHandler \
	udkapi/com/sun/star/task/XInteractionHandler2 \
	udkapi/com/sun/star/task/XInteractionRequest \
	udkapi/com/sun/star/task/XInteractionRetry \
	udkapi/com/sun/star/test/TestEvent \
	udkapi/com/sun/star/test/XSimpleTest \
	udkapi/com/sun/star/test/XTest \
	udkapi/com/sun/star/test/XTestListener \
	udkapi/com/sun/star/test/bridge/XBridgeTest \
	udkapi/com/sun/star/test/performance/XPerformanceTest \
	udkapi/com/sun/star/uno/DeploymentException \
	udkapi/com/sun/star/uno/Exception \
	udkapi/com/sun/star/uno/RuntimeException \
	udkapi/com/sun/star/uno/SecurityException \
	udkapi/com/sun/star/uno/TypeClass \
	udkapi/com/sun/star/uno/Uik \
	udkapi/com/sun/star/uno/XAdapter \
	udkapi/com/sun/star/uno/XAggregation \
	udkapi/com/sun/star/uno/XComponentContext \
	udkapi/com/sun/star/uno/XCurrentContext \
	udkapi/com/sun/star/uno/XInterface \
	udkapi/com/sun/star/uno/XNamingService \
	udkapi/com/sun/star/uno/XReference \
	udkapi/com/sun/star/uno/XUnloadingPreference \
	udkapi/com/sun/star/uno/XWeak \
	udkapi/com/sun/star/uri/RelativeUriExcessParentSegments \
	udkapi/com/sun/star/uri/XExternalUriReferenceTranslator \
	udkapi/com/sun/star/uri/XUriReference \
	udkapi/com/sun/star/uri/XUriReferenceFactory \
	udkapi/com/sun/star/uri/XUriSchemeParser \
	udkapi/com/sun/star/uri/XVndSunStarExpandUrl \
	udkapi/com/sun/star/uri/XVndSunStarExpandUrlReference \
	udkapi/com/sun/star/uri/XVndSunStarPkgUrlReferenceFactory \
	udkapi/com/sun/star/uri/XVndSunStarScriptUrl \
	udkapi/com/sun/star/uri/XVndSunStarScriptUrlReference \
	udkapi/com/sun/star/util/XMacroExpander \
	udkapi/com/sun/star/util/XVeto \
	udkapi/com/sun/star/util/logging/LogLevel \
	udkapi/com/sun/star/util/logging/XLogger \
	udkapi/com/sun/star/util/logging/XLoggerRemote \
))


# vim: set noet sw=4 ts=4:
