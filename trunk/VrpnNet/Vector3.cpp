// Vector3.cpp: Implementation for Vrpn.Vector3
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

#include "stdafx.h"

#include "BaseTypes.h"

using namespace System;

using namespace Vrpn;

Vector3::Vector3(Double x, Double y, Double z)
{
	X = x;
	Y = y;
	Z = z;
}

String^ Vector3::ToString()
{
	return String::Format("[{0}, {1}, {2}]", X, Y, Z);
}

double Vector3::Magnitude::get()
{
	return Math::Sqrt(X * X + Y * Y + Z * Z);
}

void Vector3::Normalize()
{
	double length = Magnitude;

	X /= length;
	Y /= length;
	Z /= length;
}