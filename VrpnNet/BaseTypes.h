// BaseTypes.h: Interface description for common VRPN utility types
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

#include "vrpn_Connection.h"

namespace Vrpn {
	/// <summary>
	/// Represents an internal VRPN exception.
	/// </summary>
	/// <remarks>
	/// This exception is thrown when VRPN methods return generic error codes.
	/// Because of VRPN's error-handling design (i.e., everything goes to stderr
	/// or text messages), encountering this exception usually means something
	/// has gone seriously wrong. The user should check the console (or wherever
	/// stderr goes) for error details.
	/// </remarks>
	public ref class VrpnException: public System::Exception
	{
	};

	[System::Flags]
	public enum class ServiceClass
	{
		Reliable = (1 << 0),
		FixedLatency = (1 << 1),
		LowLatency = (1 << 2),
		FixedThroughput = (1 << 3),
		HighThroughput = (1 << 4)
	};

	ref class Connection;

	public interface class IVrpnObject
	{
		void Update();

		Connection^ GetConnection();

		property System::Boolean MuteWarnings
		{
			void set(System::Boolean shutUp);
			System::Boolean get();
		}
	};

	ref class VrpnUtils
	{
	public:
		static System::DateTime ConvertTimeval(struct timeval tm);
		static System::Windows::Media::Media3D::Vector3D ConvertVector(const double vec[3]);
		static System::Windows::Media::Media3D::Quaternion ConvertQuat(const double quat[4]);
		static void CreateTimeval(System::DateTime time, struct timeval *tm);
		static void CreateVector(System::Windows::Media::Media3D::Vector3D vec, double vArray[]);
		static void CreateVector(System::Windows::Media::Media3D::Vector3D vec, float vArray[]);
		static void CreateQuat(System::Windows::Media::Media3D::Quaternion quat, double qArray[]);
	
	private:
		VrpnUtils();
	};
}