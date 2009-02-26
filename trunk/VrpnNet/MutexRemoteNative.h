// MutexRemoteNative.h: Interface description for MutexRemoteNative
//
// This class exists to work around some interface deficiencies in vrpn_Mutex_Remote:
//  1. Its event handlers are declared __cdecl, making them off-limits to .NET.
//  2. It provides no way of accessing its vrpn_Connection object after creation.
//
// Copyright (c) 2008-2009 Chris VanderKnyff
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

#pragma once

#include "vrpn_Mutex.h"

namespace Vrpn {
	namespace Internal {
		enum MutexRemoteEvent {
			RequestGranted,
			RequestDenied,
			Taken,
			Released,
		};

		typedef void (__stdcall *MutexDelegate)(int);

		class MutexRemoteNative: public ::vrpn_Mutex_Remote {
		public:
			MutexRemoteNative(const char *name, ::vrpn_Connection *lpConn);
			virtual ~MutexRemoteNative();

			::vrpn_Connection *connectionPtr() const;

			void setCallback(MutexDelegate callback);

		protected:
			MutexDelegate m_callback;

			void signalEvent(MutexRemoteEvent reason);

			static int vrpnRequestGranted(void *me);
			static int vrpnRequestDenied(void *me);
			static int vrpnTaken(void *me);
			static int vrpnReleased(void *me);
		};
	}
}