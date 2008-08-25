#include "stdafx.h"

using namespace System;
using namespace System::Reflection;
using namespace System::Runtime::CompilerServices;
using namespace System::Runtime::InteropServices;
using namespace System::Security::Permissions;

//
// General Information about an assembly is controlled through the following
// set of attributes. Change these attribute values to modify the information
// associated with an assembly.
//
[assembly:AssemblyTitleAttribute("VrpnNet")];
[assembly:AssemblyDescriptionAttribute("VRPN bindings for .NET languages")];
[assembly:AssemblyConfigurationAttribute("")];
[assembly:AssemblyCompanyAttribute("Chris VanderKnyff")];
[assembly:AssemblyProductAttribute("VrpnNet")];
[assembly:AssemblyCopyrightAttribute("Copyright (c) 2008 Chris VanderKnyff.")];
[assembly:AssemblyTrademarkAttribute("")];
[assembly:AssemblyCultureAttribute("")];

// VrpnNet versioning policy:
//
// Major Version is set when breaking changes are made to the API.
//     Version 0 is an exception; major changes may happen between
//     any two 0.x versions as I get the API to a stable point.
//
// Minor Version is the typical increment-per-release counter.
//     Minor version changes should be backwards compatible.
//
// Build Number is the VRPN minor release we linked against.
//     VRPN 7.17, for instance, is build 17. Should we go to VRPN 8.0
//     while this project still lives, the major version will change
//     (due to the breaking VRPN change) and the build will go to (in
//     this case) zero.
//
// Revision is set to *, so it changes with every build.

[assembly:AssemblyVersionAttribute("0.6.17.*")];

[assembly:ComVisible(false)];

[assembly:CLSCompliantAttribute(true)];

[assembly:SecurityPermission(SecurityAction::RequestMinimum, UnmanagedCode = true)];
