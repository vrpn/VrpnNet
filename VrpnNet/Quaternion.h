// Quaternion.h: Interface description for Vrpn.Quaternion
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

// Portions of the quaternion code are based on Quatlib, a public domain library
// initially written by Warren Robinett and Richard Holloway.

namespace Vrpn {
	value class Vector3;

	[System::Diagnostics::DebuggerDisplay("Quaternion ({X}, {Y}, {Z}, {W})")]
	public value class Quaternion
	{
	public:
		Quaternion(double x, double y, double z, double w);

		property double X;
		property double Y;
		property double Z;
		property double W;

		virtual System::String^ ToString() override;

		static Quaternion FromAxisAngle(Vector3 axis, double angle);
		
		void ToAxisAngle([System::Runtime::InteropServices::Out] Vector3 %axis,
			[System::Runtime::InteropServices::Out] double %angle);

		void Normalize();

		static Quaternion Exp(Quaternion quat);
		static Quaternion Log(Quaternion quat);
		static Quaternion Invert(Quaternion quat);
		static Quaternion Conjugate(Quaternion quat);

		static const double Tolerance = 1.0e-10;
	};

}