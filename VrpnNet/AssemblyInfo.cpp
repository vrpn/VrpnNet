// AssemblyInfo.cpp: VrpnNet assembly metadata.
//
// Copyright (c) 2008 Chris VanderKnyff
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

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
// Build Number is used for minor bugfix releases. If nothing has
//     actually changed under the hood other than, for example,
//     a fixed API call, it probably warrants a new build number
//     instead of a new minor version.
//
//     Rebuilding against a newer VRPN version is a build increment
//     if no other changes are made to the code.
//
// Revision is set to *, so it changes with every build.
//
// 

[assembly:AssemblyVersionAttribute("1.0.1.*")];

[assembly:ComVisible(false)];

[assembly:CLSCompliantAttribute(true)];

[assembly:SecurityPermission(SecurityAction::RequestMinimum, UnmanagedCode = true)];
