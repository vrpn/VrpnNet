// PoserRemote.h: Interface description for Vrpn.PoserRemote
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

#pragma once

#include "vrpn_Poser.h"
#include "BaseTypes.h"
#include "Connection.h"

namespace Vrpn {
	public ref class PoserRemote: public IVrpnObject
	{
	public:
		PoserRemote(System::String ^name);
		PoserRemote(System::String ^name, Vrpn::Connection ^connection);
		~PoserRemote();

		virtual void Update();
		virtual Vrpn::Connection^ GetConnection();
		property System::Boolean MuteWarnings
		{
			virtual void set(System::Boolean);
			virtual System::Boolean get();
		}

		void RequestPose(System::DateTime time,
			Vrpn::Vector3 position,
			Vrpn::Quaternion quaternion);

		void RequestPoseRelative(System::DateTime time,
			Vrpn::Vector3 positionDelta,
			Vrpn::Quaternion quaternion);

		void RequestPoseVelocity(System::DateTime time,
			Vrpn::Vector3 velocity,
			Vrpn::Quaternion quaternion,
			double interval);

		void RequestPoseVelocityRelative(System::DateTime time,
			Vrpn::Vector3 velocityDelta,
			Vrpn::Quaternion quaternion,
			double intervalDelta);

	private:
		::vrpn_Poser_Remote *m_poser;

		void Initialize(System::String ^name, vrpn_Connection *lpConn);
	};
}